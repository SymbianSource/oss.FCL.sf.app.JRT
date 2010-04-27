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


#ifndef CLAPILANDMARKSTOREMANAGER_H
#define CLAPILANDMARKSTOREMANAGER_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <e32base.h>
#include    <badesca.h>

// FORWARD DECLARATIONS
class CPosLmDatabaseManager;
class CLAPILandmarkStore;

/**
 * Handles native landmark stores
 *
 * Wrapper class for CPosLmDatabaseManager. This class handles native landmark
 * stores (i.e. opens, creates and deletes them and provides an access to list
 * the databases via the JSR-179 Location API)
 *
 * The manager keeps track of the opened stores and closes an opened store
 * if the client ties to delete it. It is also possible to list available
 * landmark stores via this class
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CLAPILandmarkStoreManager) : public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     * @return New instance from this class
     */
    static CLAPILandmarkStoreManager* NewL();

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance which
     * is left to the cleanup stack
     * @return New instance from this class
     */
    static CLAPILandmarkStoreManager* NewLC();

    /**
     * Destructor.
     */
    virtual ~CLAPILandmarkStoreManager();

public: // New functions

    /**
     * Lists all landmark stores which are available in the file system
     * of the device. External landmark stores and the default store
     * are not included in the array
     *
     * @return Array of non-default landmark stores. The caller takes
     *         the ownership of the returned value
     */
    CDesCArray* LandmarkStoresL();

    /**
     * Opens an existing landmark store. The store URI points the
     * path where the store is so it should be in the following form
     *      - <schema>://<path>
     *      - <drive letter>:<path>
     * This function will leave if there is no database in the specified URI
     * or if the URI of the database is not correct
     *
     * The caller takes the ownership of the opened store object
     *
     * @param aStoreUri URI where the store lies
     * @return A pointer to the opened store. The ownership is transferred
     *         to the caller
     */
    CLAPILandmarkStore* OpenStoreL(const TDesC& aStoreUri = KNullDesC);

    /**
     * Creates a landmark store to the specified URI. Note that the
     * URI must be in one of the following forms:
     *      - <schema>://<path>
     *      - <drive letter>:<path>
     *
     * The operation leaves with KErrAlreadyExists if there is a store
     * in the specified URI
     *
     * @param aStoreUri URI where the store should be created
     */
    void CreateStoreL(const TDesC& aStoreUri);

    /**
     * Removes an existing landmark database from the specified URI
     * Note that the URI must be in one of the following forms:
     *      - <schema>://<path>
     *      - <drive letter>:<path>
     *
     * Note that any open stores will be closed and are not usable
     * any more. All operations to a closed store will leave with
     * KErrSessionClosed
     *
     * @param aStoreUri URI where the store lies
     */
    void DeleteStoreL(const TDesC& aStoreUri);

    /**
     * Removes a landmark store native side peer from this class
     * The object will not be deleted since it is owned by its
     * Java side counterpart
     *
     * @param aLandmarkStore The landmark store object which should be
     *        removed
     */
    void RemoveStore(CLAPILandmarkStore& aLandmarkStore);

protected: // Constructors

    /**
     * Default C++ constructor
     */
    CLAPILandmarkStoreManager();

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // Data

    // Database manager. Owned
    CPosLmDatabaseManager* iLmDbManager;
    // Array of landmark stores. The objects are NOT owned
    RPointerArray< CLAPILandmarkStore> iStores;

};

#endif // CLAPILANDMARKSTOREMANAGER_H
// End of file
