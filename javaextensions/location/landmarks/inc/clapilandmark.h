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
* Description:  Implements a single Location API native landmark
 *
*/


#ifndef CLAPILANDMARK_H
#define CLAPILANDMARK_H

// INTERNAL INCLUDES
#include    "mlapilandmark.h"

// EXTERNAL INCLUDES
#include    <e32base.h>
#include    <lbsposition.h>

// FORWARD DECLARATIONS
class MLAPILandmarkStore;
class CPosLandmark;

/**
 * Implements a single Location API native landmark
 *
 * The landmark can have different attributes and address information
 * such as name, description, position or country and city information.
 *
 * A single landmark can be associated to many different categories but
 * the categories must defined in the store in which the landmark belongs
 * to. The landmark can implicitly belong to many different stores but
 * explicitly it can belong to one landmark store at a time.
 *
 * If the landmark is associated to a store it tries to read its data
 * when the data is requested from the object. This optimizes the
 * performance of the API in cases where the client does not need to
 * display the whole item at once.
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CLAPILandmark) : public CBase,
        public MLAPILandmark
{
public: // Constructors and destructor

    // Construction parameters of this class
    class TCtorParams
    {
    public:
        // The native landmark. NULL indicates that this landmark
        // is a new one and not from a landmark store
        CPosLandmark* iLandmark;
        // The database to which this landmark belongs to. NULL
        // indicates that this landmark is a new landmark
        MLAPILandmarkStore* iLandmarkStore;
    };

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @param aParams The construction parameters of this class
     * @return New instance from this class
     */
    static CLAPILandmark* NewL(const TCtorParams& aParams);

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance which
     * is left to the cleanup stack
     *
     * @param aParams The construction parameters of this class
     * @return New instance from this class
     */
    static CLAPILandmark* NewLC(const TCtorParams& aParams);

    /**
     * Destructor.
     */
    virtual ~CLAPILandmark();

public: // From MLAPILandmark

    /**
     * Returns the unique id of this landmark object.
     *
     * If this landmark does not belong to any landmark store,
     * KLAPINullItemId is returned.
     *
     * The identifier cannot be externally changed. It is modified
     * during the database operations
     *
     * @return The item identifier of this landmark. KLAPINullItemId
     *         is returned if this landmark does not belong to a store
     */
    TLAPIItemId Id() const;

    /**
     * Sets the name of this landmark.
     *
     * The name must not be longer than the maximum specified length in
     * Landmarks API. Otherwise the saving of the landmark will fail
     * with KErrArgument
     *
     * @param aName The new name of the landmark.
     */
    void SetNameL(const TDesC& aName);

    /**
     * Returns the name of this landmark. KNullDesC will be returned
     * if there is no description in this landmark yet
     *
     * If the item belongs to a Landmark Store, the name of the item
     * is read from the native landmark store.
     *
     * @return The name of the landmark. KNullDesC is returned if the
     *         landmark does not have a name yet
     */
    const TDesC& NameL();

    /**
     * Sets the description of this landmark.
     *
     * The description must not be longer than the maximum specified length
     * in the Landmarks API Otherwise the rest of the description will be
     * ignored
     *
     * @param aDescription The new description of the landmark. Note that
     *        NULL argument will remove the existing value when the
     *        landmark is stored again
     */
    void SetDescriptionL(const TDesC* aDescription);

    /**
     * Returns the description of this landmark
     *
     * If the item belongs to a Landmark Store, the description of the
     * landmark is read from the native landmark store.
     *
     * @return The description of the landmark. NULL is returned if the landmark
     *         does not have a description. The ownership is NOT transferred
     *         to the caller
     */
    const TDesC* DescriptionL();

    /**
     * Sets coordinates for this landmark
     *
     * By default, the landmark does not have coordinates meaning that
     * an empty TLocality object is used when requesting the position
     * of the landmark
     *
     * Note that after setting the coordinates for a database item, the
     * database values will not be fetched anymore and are overwritten
     * when the item is committed to the native database
     *
     * @param aCoordinates Coordinates of this landmark. Note that NULL
     *        argument will remove the existing value when then landmark
     *        is stored again
     */
    void SetCoordinatesL(const TLocality* aCoordinates);

    /**
     * Returns the coordinates of this landmark
     *
     * By default, the landmark does not have coordinates meaning that
     * an empty TLocality object is used when requesting the position
     *
     * If the item belongs to a Landmark Store, the coordinates of the
     * landmark are read from the native landmark store.
     *
     * @return Coordinates of this landmark. NULL is returned if the landmark
     *         does not have coordinates. The ownership is NOT transferred
     *         to the caller.
     */
    const TLocality* CoordinatesL();

    /**
     * Sets the address information in this landmark.
     *
     * NULL argument indicates that the address information should
     * be removed from the landmark
     *
     * @param aAddressInfo The new address information for this landmark.
     *        The ownership is transferred to this class
     */
    void SetAddressInfoL(CLAPIAddressInfo* aAddressInfo);

    /**
     * Returns the address information for this landmark.
     *
     * The fields are loaded from the native database if those are not
     * yet loaded.
     *
     * Note that depending from the amount of supported address information,
     * this operation may be performance critical if done for many items
     * subsequently.
     *
     * @return The address information of this landmark. NULL or empty object
     *         is returned if the landmark does not have address information.
     *         The caller does NOT take the ownership of the object
     */
    const CLAPIAddressInfo* AddressInfoL();

public: // New functions

    /**
     * Prepares the item for saving. This operation must be called
     * always when the item is to be persisted to some existing database
     * It guarantees that the data which has been modified is correctly
     * written and stored to the native database so, in practice, this
     * operation commits the changes made to the item after previous
     * call of PrepareForSaveL
     *
     * Note that at this point, any existing data will be overwritten
     * and the full item is loaded from the landmark store so this
     * should be used with caution and only when the item is to be
     * fully saved to a database or exported to some external module
     */
    void PrepareForSaveL();

    /**
     * Returns the native counterpart of this class. Note that
     * the object may not be up to date if PrepareForSaveL() has
     * not been called
     * @return Landmarks API landmark implementation
     */
    CPosLandmark& PosLandmark() const;

    /**
     * Sets new CPosLandmark object to this landmark. The existing
     * object will be replaced. The ownership is transferred to
     * this class.
     *
     * @param aLandmark New CPosLandmark object
     */
    void SetPosLandmark(CPosLandmark* aLandmark);

    /**
     * Associates this landmark to a landmark store.. The landmark can
     * belong to one landmark store at a time. If the landmark is transferred
     * to a different store, the old store will be notified
     *
     * @param aStore The store where this landmark has been added
     */
    void AssociateToStore(MLAPILandmarkStore* aStore);

    /**
     * Indicates that the store in which this landmark belongs to
     * has been closed
     */
    void StoreClosed();

private: // New functions

    /**
     * Read address information values from the native CPosLandmark
     * object owned by this class.
     *
     * The values are read to iAddressInfo which must not be null.
     * Otherwise this function will panic
     *
     * @param aPositionFields The position fields which should be read
     *        from the native CPosLandmark object
     * @return The number of available position fields in the native
     *         landmark
     */
    TInt DoGetAddressInfoFieldValuesL(const RArray< TUint>& aPositionFields);

private: // Constructors

    /**
     * Constructor
     * @param aParams The constructions parameters of this class
     */
    CLAPILandmark(const TCtorParams& aParams);

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // Data

    // The landmark store to which this landmark belongs. Used. Can be NULL
    MLAPILandmarkStore* iLandmarkStore;
    // Associated native landmark object. Owned
    CPosLandmark* iLandmark;

    // The name of the landmark. Owned
    HBufC* iLandmarkName;
    // The description of the landmark. Owned
    HBufC* iDescription;
    // Coordinates fo the landmark. Owned
    TLocality* iCoordinates;
    // Address information. Owned
    CLAPIAddressInfo* iAddressInfo;

    // Attributes of this landmark. This marks the landmark attributes
    // which are changed during the lifetime of the landmark.
    TUint iChangedAttributes;

};

#endif // CLAPILANDMARK_H
// End of file
