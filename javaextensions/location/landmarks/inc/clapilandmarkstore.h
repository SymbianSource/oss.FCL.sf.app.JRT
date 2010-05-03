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
* Description:  Implements the native part of the Location API landmark store
 *
*/


#ifndef CLAPILANDMARKSTORE_H
#define CLAPILANDMARKSTORE_H

// INTERNAL INCLUDES
#include    "mlapilandmarkstore.h"
#include    "mlapilmdatabaseobserver.h"
#include    "mlapilandmark.h"

// EXTERNAL INCLUDES
#include    <e32def.h>
#include    <e32base.h>
#include    <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CLAPILandmarkSearchFactory;
class TLAPISearchCriteria;
class CPosLandmarkDatabase;
class CPosLandmark;
class MLAPICategoryManager;
class MLAPILmDatabaseEventNotifier;

/**
 * Implements the native part of Location API landmark store.
 *
 * This class provides the basic functionalities for handling
 * individual landmarks such as adding, deleting and updating landmarks
 *
 * The stores are available always when it once has been opened. This
 * is because some items may still exists in the java side after the
 * store has been closed. Function calls to a closed store leaves with
 * KErrSessionClosed. The stores cannot be closed explicitly from the Java
 * but only when a store is deleted from the device
 *
 * Ensuring that stores are not always open prevents the API to reserve
 * huge amount of memory if many stores are used concurrently
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CLAPILandmarkStore) : public CBase,
        public MLAPILandmarkStore
{
public: // Type definitions

    // Construction parameters of this class
    class TCtorParams
    {
    public:
        // The ownership of these are transferred in the construction
        CPosLandmarkDatabase* iLandmarkDatabase;
        MLAPICategoryManager* iCategoryManager;
        MLAPILmDatabaseEventNotifier* iEventNotifier;
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @param aParams Construction parameters of this class
     * @return New instance from this class
     */
    static CLAPILandmarkStore* NewL(const TCtorParams& aParams);

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance which
     * is left to the cleanup stack
     *
     * @param aParams Construction parameters of this class
     * @return New instance from this class
     */
    static CLAPILandmarkStore* NewLC(const TCtorParams& aParams);

    /**
     * Destructor.
     */
    virtual ~CLAPILandmarkStore();

public: // From MLAPILandmarkStore

    /**
     * Reads the full landmark data from the database. The data
     * is added to the item which is passed as a parameter. Note
     * that this function may be time consuming if it is called
     * for huge amount of landmarks subsequently.
     *
     * The function should only be used when the item is needed
     * to be fully writen into the native database. The operation
     *
     * @param aLandmark The landmark which will be read from the database
     */
    void ReadFullLandmarkL(CLAPILandmark& aLandmark);

    /**
     * Reads the requested attributes to a landmark from the native database.
     * It also possible to read certain position fields from the native
     * database.
     *
     * @param aLandmark The landmark which is to be read
     * @param aAttributes The attributes which are read
     * @param aAddressInfos The requested address info fields which
     *        are read from the native database. If this parameter is
     *        NULL, then no position fields are read. Note that the
     *        ownership of the array does NOT transfer to this class
     */
    void ReadLandmarkAttributesL(
        CLAPILandmark& aLandmark,
        const TUint aAttributes,
        const RArray< TUint>* aAddressInfos);

    /**
     * Returns the category manager for this database. If NULL is returned
     * this database does not support categories at all.
     *
     * @return Category manager for this landmark store. The ownership is
     *         NOT transferred to the caller. NULL indicates that the store
     *         does not support category management
     */
    MLAPICategoryManager* CategoryManagerL() const;

    /**
     * Indicates that a landmark is going to be disposed. This removes
     * any possible references to the landmark from this store if the
     * landmark belongs to the store
     *
     * @param aLandmark A references to the disposed landmark object
     */
    void LandmarkDisposed(CLAPILandmark& aLandmark);

public: // New functions

    /**
     * Returns the URI of this landmark store
     * @return The URI of the landmark store
     */
    const TDesC& StoreUri() const;

    /**
     * Returns an array of landmarks based on the search attributes and
     * initialization parameters. The caller takes the ownership of the
     * returned array.
     *
     * Currently it is possible to search using three different parameters
     * and additionally a different mixture of these parameters. If no
     * criteria is specified, all landmarks in the store will be returned
     *
     *      - A text with attributes can be specified to perform a search
     *        from every string field in a landmark
     *      - A category name can be specified to perform a search from
     *        the specified category
     *      - An area can be specified to perform an area specific search
     *
     * @param aAttributes The attributes which will be pre-initialized
     *        to the matched landmarks. Note that initializing time depends
     *        on the number of initialized attributes
     * @param aSearchCriteria The search criteria which is used to search
     *        the landmarks. NULL matches all landmarks
     * @return An array of landmark. The caller takes the ownership of the
     *         array and the landmarks in it
     */
    CArrayPtr< CLAPILandmark>* LandmarksL(
        const TUint aAttributes,
        const TLAPISearchCriteria* aSearchCriteria);

    /**
     * Adds a new landmark to this landmark store. If the landmark already
     * belongs to this store nothing happens
     *
     * @param aLandmark The landmark which will be added
     */
    void AddLandmarkL(CLAPILandmark& aLandmark);

    /**
     * Updates an existing landmark in the database. The data is copied
     * from aLandmark object and added to the existing landmark in the
     * database.
     *
     * This operation leaves with KErrNotFound if there is no such landmark
     * in the database
     *
     * @param aLandmark The landmark which will be updated
     */
    void UpdateLandmarkL(CLAPILandmark& aLandmark);

    /**
     * Removes an existing landmark from this landmark store
     *
     * The call is silently ignored if the landmark does not belong to this
     * store or if it has already been removed.
     *
     * @param aLandmark The landmark which is to be removed
     */
    void RemoveLandmarkL(CLAPILandmark& aLandmark);

    /**
     * Compacts the native database if it is necessary.
     *
     * The compacting is done synchronously so it may take some time to complete
     * depending on the number of the landmarks and categories which
     * are stored to this landmark store
     */
    void CompactIfNeededL();

    /**
     * Closes this landmark store. Note that this does not delete
     * this object but only closes the access to the native database
     * The object is still usable, but invocations will leave with
     * KErrSessionClosed.
     *
     * Stores can be closed internally when the object needs to be
     * deleted or externally when the store is needed to be closed
     * due to, for example, database deletion.
     */
    void Close();

private: // New functions

    /**
     * Converts prepared landmarks to Location API landmarks
     * Note that this is only needed because of the cleanup problems
     * related to ownership transferring of CArrayPtr which comes
     * from Landmarks API
     *
     * @param aSrcArray The array from which the objects are created
     *        on return, the array is empty
     * @param aDestArray The array to which the new landmarks are added
     */
    void HandlePreparedLandmarksL(
        CArrayPtr< CPosLandmark>& aSrcArray,
        CArrayPtr< CLAPILandmark>& aDestArray);

    /**
     * Refreshes all landmarks which match for the given identifier.
     * Note that this operation may be a very huge operation if one
     * landmark has many references from the Java side
     *
     * @param aLandmarkId The identifier of updated landmarks
     * @param aRemoveFromStore Indicates that the refreshed landmarks
     *        should be removed from this landmark store. By default,
     *        the landmarks are not removed
     */
    void RefreshLandmarksL(
        TLAPIItemId aLandmarkId,
        TBool aRemoveFromStore = EFalse);

private: // Construtors

    /**
     * C++ constructor.
     * @param aParams Construction parameters
     */
    CLAPILandmarkStore(const TCtorParams& aParams);

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // Data

    // All landmark objects. Used
    RPointerArray< CLAPILandmark> iLandmarks;

    // Landmark database. Owned
    CPosLandmarkDatabase* iLandmarkDatabase;

    // Category manager. Owned
    MLAPICategoryManager* iCategoryManager;

    // Database event notifier. Owned
    MLAPILmDatabaseEventNotifier* iEventNotifier;

    // Landmark search factory. Owned
    CLAPILandmarkSearchFactory* iSearchFactory;

    // The URI of this store. Owned
    HBufC* iStoreUri;
};

#endif // CLAPILANDMARKSTORE_H
// End of file
