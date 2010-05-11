/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implements native landmark store functionality
 *
*/


// INTERNAL INCLUDES
#include    "clapilandmarkstore.h"
#include    "mlapicategorymanager.h"
#include    "mlapilmdatabaseeventnotifier.h"
#include    "clapilandmarksearchfactory.h"
#include    "clapilandmark.h"
#include    "tlapisearchcriteria.h"
#include    "javasymbianoslayer.h"
#include    "lapipanics.h"
#include    "logger.h"

// EXTERNAL INCLUDES
#include    <EPos_CPosLandmark.h>
#include    <EPos_CPosLandmarkDatabase.h>
#include    <EPos_CPosLmItemIterator.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// Minimum usage of the native landmark database. If the usage drops
// below this level, the database will be compacted to avoid that the
// Landmark Server would do that which eventually locks the database
const TReal KLAPIMinCompactUsage = 0.7;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::CLAPILandmarkStore
// ---------------------------------------------------------------------------
//
CLAPILandmarkStore::~CLAPILandmarkStore()
{
    JELOG2(EJavaLocation);
    // Closes the database and releases all resources
    Close();
    delete iStoreUri;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::ConstructL
// (other items were commented in a header
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::ConstructL()
{
    JELOG2(EJavaLocation);
    // Get the name of the landmark store
    iStoreUri = iLandmarkDatabase->DatabaseUriLC();
    CleanupStack::Pop(iStoreUri);
    // Create landmark search factory for filtering landmarks
    iSearchFactory = CLAPILandmarkSearchFactory::NewL(*iLandmarkDatabase);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::NewL
// ---------------------------------------------------------------------------
//
CLAPILandmarkStore* CLAPILandmarkStore::NewL(const TCtorParams& aParams)
{
    JELOG2(EJavaLocation);
    CLAPILandmarkStore* self = CLAPILandmarkStore::NewLC(aParams);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::NewLC
// ---------------------------------------------------------------------------
//
CLAPILandmarkStore* CLAPILandmarkStore::NewLC(const TCtorParams& aParams)
{
    JELOG2(EJavaLocation);
    CLAPILandmarkStore* self = new(ELeave) CLAPILandmarkStore(aParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::ReadFullLandmarkL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::ReadFullLandmarkL(CLAPILandmark& aLandmark)
{
    JELOG2(EJavaLocation);
    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabase, User::Leave(KErrSessionClosed));

    TLAPIItemId id = aLandmark.Id();
    // Check that the landmark id is valid
    __ASSERT_DEBUG(id != KLAPINullItemId, LAPIError::Panic(
                       ELAPIPanicInvalidLandmarkId));

    // Read the full landmark from the database and add it to the item
    // Note that this overwrites the native entry in the item. The function
    // leaves with KErrNotFound if there is no such item in the database
    CPosLandmark* fullLandmark = iLandmarkDatabase->ReadLandmarkLC(id);
    aLandmark.SetPosLandmark(fullLandmark);
    // The ownership of fullLandmark is transferred to landmark object
    CleanupStack::Pop(fullLandmark);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::ReadLandmarkAttributesL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::ReadLandmarkAttributesL(CLAPILandmark& aLandmark,
        const TUint aAttributes, const RArray<TUint>* aAddressInfos)
{
    JELOG2(EJavaLocation);
    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabase, User::Leave(KErrSessionClosed));

    // Create new set of partial read parameters. The old parameters in the
    // store will not be used since those can have additional fields which
    // are not needed when reading the requested landmark data
    CPosLmPartialReadParameters* params = CPosLmPartialReadParameters::NewLC();
    params->SetRequestedAttributes(aAttributes);
    // Set requested address info fields if specified
    if (aAddressInfos)
    {
        params->SetRequestedPositionFields(*aAddressInfos);
    }

    TLAPIItemId id = aLandmark.Id();
    // Check that the landmark id is valid
    __ASSERT_DEBUG(id != KLAPINullItemId, LAPIError::Panic(
                       ELAPIPanicInvalidLandmarkId));

    // Read the landmark using the partial read parameters
    iLandmarkDatabase->SetPartialReadParametersL(*params);
    CleanupStack::PopAndDestroy(params);
    // Read the partial landmark from the landmark database
    CPosLandmark* newLm = iLandmarkDatabase->ReadPartialLandmarkLC(id);
    // The ownership is transferred to landmark object
    aLandmark.SetPosLandmark(newLm);
    CleanupStack::Pop(newLm);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::CategoryManager
// ---------------------------------------------------------------------------
//
MLAPICategoryManager* CLAPILandmarkStore::CategoryManagerL() const
{
    JELOG2(EJavaLocation);
    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabase, User::Leave(KErrSessionClosed));
    return iCategoryManager;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::LandmarkDisposed
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::LandmarkDisposed(CLAPILandmark& aLandmark)
{
    JELOG2(EJavaLocation);
    TInt landmarkIndex = iLandmarks.Find(&aLandmark);
    // Remove the specified landmark if it was found from this store
    if (landmarkIndex != KErrNotFound)
    {
        iLandmarks.Remove(landmarkIndex);
    }
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::StoreUri
// ---------------------------------------------------------------------------
//
const TDesC& CLAPILandmarkStore::StoreUri() const
{
    JELOG2(EJavaLocation);
    return *iStoreUri;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::LandmarksL
// ---------------------------------------------------------------------------
//
CArrayPtr<CLAPILandmark>* CLAPILandmarkStore::LandmarksL(
    const TUint aAttributes, const TLAPISearchCriteria* aSearchCriteria)
{
    JELOG2(EJavaLocation);
    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabase, User::Leave(KErrSessionClosed));

    CPosLmItemIterator* iter = iSearchFactory->CreateIteratorL(aSearchCriteria);
    CleanupStack::PushL(iter);
    // Do not initialize anything if there is nothing to initialize
    TInt itemCount = iter->NumOfItemsL();
    // Ensure that the granularity is always more than zero. Flat array
    // is used because the buffer will not be increased after it has
    // been initialized
    CArrayPtr<CLAPILandmark>* landmarks = new(ELeave) CArrayPtrFlat<
    CLAPILandmark> (itemCount + 1);
    CleanupStack::PushL(landmarks);
    // Put the array to cleanup stack for reset and destroy since the
    // array owns the objects and those must also be deleted
    CleanupResetAndDestroyPushL(*landmarks);

    if (itemCount > 0)
    {
        // Get item identifiers only from the iterator
        RArray<TPosLmItemId> ids;
        CleanupClosePushL(ids);
        iter->GetItemIdsL(ids, 0, iter->NumOfItemsL());

        // Set partial read paramaters to the native database.
        CPosLmPartialReadParameters* params =
            CPosLmPartialReadParameters::NewLC();
        params->SetRequestedAttributes(aAttributes);
        iLandmarkDatabase->SetPartialReadParametersL(*params);
        CleanupStack::PopAndDestroy(params);
        // Prepare partial landmarks. This reads the specified landmark id array
        // from the native database and initializes the requested attributes to
        // the previously read landmark objects.
        CPosLmOperation* op = iLandmarkDatabase->PreparePartialLandmarksL(ids);
        // ids are on top of the stack and are not needed anymore
        CleanupStack::PopAndDestroy(&ids);
        CleanupStack::PushL(op);
        // Execute the operation. This needs to be done before the partial
        // landmarks can be taken from the landmark database. The time of the
        // operation depends on the partial read parameters set above
        op->ExecuteL();

        // The operation has been completed and prepared landmarks are available
        CArrayPtr<CPosLandmark>* preparedLandmarks =
            iLandmarkDatabase->TakePreparedPartialLandmarksL(op);
        CleanupStack::PopAndDestroy(op);
        // Make this leave-safe since the prepared array cannot be put to
        // cleanup stack safely because it needs two leaving operations
        TRAPD(error, HandlePreparedLandmarksL(*preparedLandmarks, *landmarks));
        // Cleanup the prepared landmarks
        preparedLandmarks->ResetAndDestroy();
        delete preparedLandmarks;
        // Now it is safe to leave
        User::LeaveIfError(error);
    }

    CleanupStack::Pop(2, landmarks); // The object and ResetAndDestroy
    CleanupStack::PopAndDestroy(iter);
    return landmarks;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::AddLandmarkL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::AddLandmarkL(CLAPILandmark& aLandmark)
{
    JELOG2(EJavaLocation);
    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabase, User::Leave(KErrSessionClosed));
    TUint id = aLandmark.Id();
    LOG1(EJavaLocation, EInfo, "CLAPILandmarkStore::AddLandmarkL - id %d", id);

    CPosLandmark* landmark(NULL);
    TRAPD(err, landmark = iLandmarkDatabase->ReadLandmarkLC(id);
          CleanupStack::PopAndDestroy(landmark));    // We dont need the landmark

    // The landmark was not found from the database. Add a new landmark
    if (err == KErrNotFound)
    {
        LOG(EJavaLocation, EInfo,
            "CLAPILandmarkStore::AddLandmarkL - adding new landmark");
        // Prepare the landmark for saving. This will guarantee that partially
        // read landmarks will be up to date when those are added to the native
        // database. All data are not necessary available in the item if it has
        // been partially loaded from the native database
        aLandmark.PrepareForSaveL();

        // Add new landmark to the database. Note that the added landmark
        // should not initially belong to any categories
        CPosLandmark& landmark = aLandmark.PosLandmark();
        landmark.RemoveLandmarkAttributes(CPosLandmark::ECategoryInfo);
        iLandmarkDatabase->AddLandmarkL(landmark);
        err = iLandmarks.Append(&aLandmark);
        if (err != KErrNone)
        {
            // Remove the added landmark to keep the store in sync. Note
            // that this call leaves only if the database hasn't been initialized
            iLandmarkDatabase->RemoveLandmarkL(id);
            User::Leave(err);
        }

        // Mark that this landmark has been associated to a landmark store
        aLandmark.AssociateToStore(this);
        err = KErrNone;
        // Compact the database if it is necessary
        CompactIfNeededL();
    }
    User::LeaveIfError(err);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::UpdateLandmarkL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::UpdateLandmarkL(CLAPILandmark& aLandmark)
{
    JELOG2(EJavaLocation);
    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabase, User::Leave(KErrSessionClosed));
    LOG1(EJavaLocation, EInfo,
         "CLAPILandmarkStore::UpdateLandmarkL - id %d",
         aLandmark.Id());
    // Refresh all landmarks before updating this landmark. This needs to be
    // done because it is expected that all Java side landmark objects will
    // not be updated if one specific landmark is updated
    RefreshLandmarksL(aLandmark.Id());

    // Prepare the landmark for saving. This will guarantee that partially
    // read landmarks will be up to date when those are added to the native
    // database. All data are not necessary available in the item if it has
    // been partially loaded from the native database
    aLandmark.PrepareForSaveL();
    // Update the existing landmark into the database. The existing data
    // will be overwritten
    iLandmarkDatabase->UpdateLandmarkL(aLandmark.PosLandmark());
    // Compact the database if it is necessary
    CompactIfNeededL();
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::DeleteLandmarkL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::RemoveLandmarkL(CLAPILandmark& aLandmark)
{
    JELOG2(EJavaLocation);
    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabase, User::Leave(KErrSessionClosed));
    TUint32 id = aLandmark.Id();
    LOG1(EJavaLocation, EInfo,
         "CLAPILandmarkStore::RemoveLandmarkL - id %d", id);

    // Refresh all related landmarks which match for the given landmark's id
    TRAPD(err, RefreshLandmarksL(id, ETrue));
    // Do not leave if the landmark was already removed from the store
    if (err == KErrNone)
    {
        LOG(EJavaLocation, EInfo,
            "CLAPILandmarkStore::RemoveLandmarkL - removing from database");
        // Remove the landmark from the native database.
        iLandmarkDatabase->RemoveLandmarkL(id);
        // Compact the database if it is necessary
        CompactIfNeededL();
    }
    __ASSERT_ALWAYS(err == KErrNone || err == KErrNotFound,
                    User::Leave(err));
    // Not associated anymore. Remove from list
    aLandmark.AssociateToStore(NULL);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::CompactIfNeededL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::CompactIfNeededL()
{
    JELOG2(EJavaLocation);
    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabase, User::Leave(KErrSessionClosed));

    CPosLandmarkDatabase::TSize databaseSize =
        iLandmarkDatabase->SizeL();
    // Execute synchronised compact operation if the the usage is below
    // the minimum compact limit. This prevents that Landmarks Server will
    // not do this operation and lock the database
    if (databaseSize.iUsage < KLAPIMinCompactUsage)
    {
        ExecuteAndDeleteLD(iLandmarkDatabase->CompactL());
    }
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::Close
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::Close()
{
    JELOG2(EJavaLocation);
    TInt count = iLandmarks.Count();
    for (TInt i = 0; i < count; i++)
    {
        // The store has been closed. This indicates that there are no
        // landmark objects in the java side and the landmark store has
        // gone out of scope
        iLandmarks[i]->StoreClosed();
    }

    // The landmark objects are not owned by the store
    iLandmarks.Close();

    delete iCategoryManager;
    iCategoryManager = NULL;
    delete iSearchFactory;
    iSearchFactory = NULL;
    delete iEventNotifier;
    iEventNotifier = NULL;
    delete iLandmarkDatabase;
    iLandmarkDatabase = NULL;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::HandlePreparedLandmarksL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::HandlePreparedLandmarksL(CArrayPtr<
        CPosLandmark>& aSrcArray, CArrayPtr<CLAPILandmark>& aDestArray)
{
    JELOG2(EJavaLocation);
    aDestArray.Reset();

    // Create Location API landmark objects from each native landmark
    TInt lmCount = aSrcArray.Count();
    while (lmCount-- > 0)
    {
        // Handle items in accending order from the start of the array
        CPosLandmark* landmark = aSrcArray.At(0);
        // The new landmark takes the ownership of the CPosLandmark object
        // Associate the new landmark to this landmark store
        CLAPILandmark::TCtorParams params;
        params.iLandmark = landmark;
        params.iLandmarkStore = this;

        CLAPILandmark* newLandmark = CLAPILandmark::NewLC(params);
        // Remove the landmark from the prepared array since newLandmark
        // takes the ownership of the returned value
        aSrcArray.Delete(0);
        aDestArray.AppendL(newLandmark);
        CleanupStack::Pop(newLandmark);
        iLandmarks.AppendL(newLandmark);
    }
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::RefreshLandmarksL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStore::RefreshLandmarksL(TLAPIItemId aLandmarkId,
        TBool aRemoveFromStore)
{
    JELOG2(EJavaLocation);
    TInt landmarksCount = iLandmarks.Count();
    CPosLandmark* posLm =
        iLandmarkDatabase->ReadLandmarkLC(aLandmarkId);

    // Refresh all landmarks which match for the given id. Iterate backwards
    // if the landmarks are removed from the store
    while (landmarksCount-- > 0)
    {
        CLAPILandmark* landmark = iLandmarks[landmarksCount];
        if (landmark->Id() == aLandmarkId)
        {
            CPosLandmark* copyLandmark = CPosLandmark::NewLC(*posLm);
            landmark->SetPosLandmark(copyLandmark);
            // The landmark takes the ownership of copyLandmark
            CleanupStack::Pop(copyLandmark);

            // Remove the landmark from this store if requested. This is
            // usually done because removed landmark must update all its
            // duplicates since those landmarks must not be updated
            if (aRemoveFromStore)
            {
                // The landmark disposes itself from this store
                landmark->AssociateToStore(NULL);
            }
        }
    }

    CleanupStack::PopAndDestroy(posLm);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStore::CLAPILandmarkStore
// ---------------------------------------------------------------------------
//
CLAPILandmarkStore::CLAPILandmarkStore(const TCtorParams& aParams) :
        iLandmarkDatabase(aParams.iLandmarkDatabase), iCategoryManager(
            aParams.iCategoryManager), iEventNotifier(
                aParams.iEventNotifier)
{
    JELOG2(EJavaLocation);
}

// End of file
