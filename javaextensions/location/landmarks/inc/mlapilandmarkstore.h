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
* Description:  Interface for Location API landmark store
 *
*/


#ifndef MLAPILANDMARKSTORE_H
#define MLAPILANDMARKSTORE_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <e32def.h>

// FORWARD DECLARATIONS
class MLAPICategoryManager;
class CLAPILandmark;

/**
 * Interface to Location API landmark store implementation
 * This interface provides the basic functionalities for handling
 * individual landmarks such as adding, deleting and updating landmarks
 *
 * The interface also provides methods for reading partial landmark data
 * and full landmarks. Partial landmarks should be used for better performance
 * when reading multiple landmarks from a landmark store
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(MLAPILandmarkStore)
{
public: // New functions

    /**
     * Reads the full landmark data from the database. The data
     * is added to the item which is passed as a parameter. Note
     * that this function may be time consuming if it is called
     * for huge amount of landmarks subsequently.
     *
     * The function should only be used when the item is needed
     * to be fully writen into the native database.
     *
     * @param aLandmark The landmark which will be read from the database
     */
    virtual void ReadFullLandmarkL(CLAPILandmark& aLandmark) = 0;

    /**
     * Reads the requested attributes to a landmark from
     * the native database. It also possible to read certain
     * position fields from the native database.
     *
     * @param aLandmark The landmark which is to be read
     * @param aAttributes The attributes which are read
     * @param aAddressInfos The requested address info fields which
     *        are read from the native database. If this parameter is
     *        NULL, then no position fields are read. Note that the
     *        ownership of the array does NOT transfer to this class
     */
    virtual void ReadLandmarkAttributesL(CLAPILandmark& aLandmark,
                                         const TUint aAttributes, const RArray<TUint>* aAddressInfos) = 0;

    /**
     * Returns the category manager for this database. If NULL is returned
     * this database does not support categories at all.
     *
     * @return Category manager for this landmark store. The ownership is
     *         NOT transferred to the caller. NULL indicates that the store
     *         does not support category management
     */
    virtual MLAPICategoryManager* CategoryManagerL() const = 0;

    /**
     * Indicates that a landmark is going to be disposed. This removes
     * any possible references to the landmark from this store if the
     * landmark belongs to the store
     *
     * @param aLandmark A references to the disposed landmark object
     */
    virtual void LandmarkDisposed(CLAPILandmark& aLandmark) = 0;

protected:

    /**
     * Destructor.
     */
    virtual ~MLAPILandmarkStore()
    {}

}
;

#endif // MLAPILANDMARKSTORE_H
// End of file
