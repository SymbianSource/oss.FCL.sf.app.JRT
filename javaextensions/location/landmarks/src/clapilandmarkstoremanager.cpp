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
* Description:  Handles native landmark databases
 *
*/


// INTERNAL INCLUDES
#include    "clapilandmarkstoremanager.h"
#include    "clapilmdatabaseeventnotifier.h"
#include    "clapicategorymanager.h"
#include    "clapilandmarkstore.h"
#include    "logger.h"

// EXTERNAL INCLUDES
#include    <EPos_CPosLandmarkDatabase.h>
#include    <EPos_CPosLmDatabaseManager.h>
#include    <EPos_CPosLmCategoryManager.h>
#include    <EPos_CPosLmOperation.h>
#include    <EPos_Landmarks.h>
#include    <EPos_HPosLmDatabaseInfo.h>

namespace
{
// The default protocol for internal stores
_LIT(KLAPIDefaultProtocol, "file://");
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::NewL
// ---------------------------------------------------------------------------
//
CLAPILandmarkStoreManager* CLAPILandmarkStoreManager::NewL()
{
    JELOG2(EJavaLocation);
    CLAPILandmarkStoreManager* self = CLAPILandmarkStoreManager::NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::NewLC
// ---------------------------------------------------------------------------
//
CLAPILandmarkStoreManager* CLAPILandmarkStoreManager::NewLC()
{
    JELOG2(EJavaLocation);
    CLAPILandmarkStoreManager* self = new(ELeave) CLAPILandmarkStoreManager;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::~CLAPILandmarkStoreManager
// ---------------------------------------------------------------------------
//
CLAPILandmarkStoreManager::~CLAPILandmarkStoreManager()
{
    JELOG2(EJavaLocation);
    // The objects inside the array are not owned by this class
    iStores.Close();
    delete iLmDbManager;
    // Release all landmark resources because Landmarks FW will not be used
    // anymore. This has basicly the same effect as REComSeesion::FinalClose()
    // All allocated memory and garbage plug-ins will be released
    ReleaseLandmarkResources();
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::LandmarkStoresL
// ---------------------------------------------------------------------------
//
CDesCArray* CLAPILandmarkStoreManager::LandmarkStoresL()
{
    JELOG2(EJavaLocation);
    // List landmark databases but remove the default store from the array
    CDesCArray* stores = iLmDbManager->ListDatabasesLC();
    HBufC* defaultDbName = iLmDbManager->DefaultDatabaseUriLC();

    // Remove the default database from the store names array. External stores
    // are also removed because this API does not support other than stores
    // which are located in the file system of the device
    TInt count = stores->Count();
    for (TInt i = 0; i < count; i++)
    {
        TPtrC storeUri = stores->MdcaPoint(i);
        if (defaultDbName->CompareF(storeUri) == 0 || storeUri.FindF(
                    KLAPIDefaultProtocol) == KErrNotFound)
        {
            stores->Delete(i);
            break;
        }
    }

    CleanupStack::PopAndDestroy(defaultDbName);
    CleanupStack::Pop(stores);
    return stores;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::OpenStoreL
// ---------------------------------------------------------------------------
//
CLAPILandmarkStore* CLAPILandmarkStoreManager::OpenStoreL(
    const TDesC& aStoreUri)
{
    JELOG2(EJavaLocation);
    CPosLandmarkDatabase* database(NULL);
    // Open default landmark database if the passed URI is a null descriptor
    database = (aStoreUri == KNullDesC() ? CPosLandmarkDatabase::OpenL()
                : CPosLandmarkDatabase::OpenL(aStoreUri));

    CleanupStack::PushL(database);

    // Initialize the database if needed. Note that the store is not usable
    // in any way if it has not been initialized
    if (database->IsInitializingNeeded())
    {
        ExecuteAndDeleteLD(database->InitializeL());
    }

    // Create landmark store event notifier
    MLAPILmDatabaseEventNotifier* notifier =
        CLAPILmDatabaseEventNotifier::NewLC(*database);

    // Create category manager
    CLAPICategoryManager::TCtorParams managerParams;
    managerParams.iLandmarkDatabase = database;
    managerParams.iEventNotifier = notifier;
    MLAPICategoryManager* manager = CLAPICategoryManager::NewLC(managerParams);

    // Construct the landmark store. The store takes ownership of
    // the native landmark database, the notifier and the category manager.
    CLAPILandmarkStore::TCtorParams storeParams;
    storeParams.iLandmarkDatabase = database;
    storeParams.iCategoryManager = manager;
    storeParams.iEventNotifier = notifier;
    CLAPILandmarkStore* lmStore = CLAPILandmarkStore::NewLC(storeParams);

    // The store is not owned but is still monitored since it is possible
    // that an open store would be deleted which means that the store
    // must be closed in order for successful deletion
    iStores.AppendL(lmStore);

    CleanupStack::Pop(4, database);
    return lmStore;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::CreateStoreL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStoreManager::CreateStoreL(const TDesC& aStoreUri)
{
    JELOG2(EJavaLocation);
    __ASSERT_ALWAYS(aStoreUri != KNullDesC(), User::Leave(KErrBadName));
    // Create database info which specifies the place of the new database
    HPosLmDatabaseInfo* dbInfo = HPosLmDatabaseInfo::NewLC(aStoreUri);
    // Create the database. This leaves if there is already a database in
    // the same URI location
    iLmDbManager->CreateDatabaseL(*dbInfo);
    CleanupStack::PopAndDestroy(dbInfo);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::DeleteStoreL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStoreManager::DeleteStoreL(const TDesC& aStoreUri)
{
    JELOG2(EJavaLocation);
    __ASSERT_ALWAYS(aStoreUri != KNullDesC(), User::Leave(KErrBadName));
    // Find if the requested store has been opened and close it before
    // deleting the database. This is done because if some client uses
    // the store it cannot be deleted
    TInt storeCount = iStores.Count();
    for (TInt i = 0; i < storeCount; i++)
    {
        if (iStores[i]->StoreUri().CompareF(aStoreUri) == 0)
        {
            // Database has been opened so close it. Make sure that all
            // databases will be closed
            iStores[i]->Close();
        }
    }
    // Now it is safe to delete the requested store
    iLmDbManager->DeleteDatabaseL(aStoreUri);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::RemoveStore
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStoreManager::RemoveStore(CLAPILandmarkStore& aLandmarkStore)
{
    JELOG2(EJavaLocation);
    TInt index = iStores.Find(&aLandmarkStore);
    if (index != KErrNotFound)
    {
        // Do not delete the store since this class does not own it
        iStores.Remove(index);
    }
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::CLAPILandmarkStoreManager
// ---------------------------------------------------------------------------
//
CLAPILandmarkStoreManager::CLAPILandmarkStoreManager()
{
    JELOG2(EJavaLocation);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkStoreManager::ConstructL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CLAPILandmarkStoreManager::ConstructL()
{
    JELOG2(EJavaLocation);
    iLmDbManager = CPosLmDatabaseManager::NewL();
}

// End of file
