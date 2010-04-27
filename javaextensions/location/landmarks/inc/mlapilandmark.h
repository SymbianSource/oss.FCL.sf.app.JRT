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
* Description:  Interface to a single Location API native landmark
 *
*/


#ifndef MLAPILANDMARK_H
#define MLAPILANDMARK_H

// INTERNAL INCLUDES
#include    <e32std.h>

// FORWARD DECLARATIONS
class TLocality;
class CLAPIAddressInfo;

// Landmark item id definition
typedef TUint32 TLAPIItemId;

// Null landmark id definition
const TLAPIItemId KLAPINullItemId = 0;

// Landmark attributes. See CPosLandmark attributes for more detail
// These attributes are used by Location API when reading landmark data
enum TLAPILandmarkAttributesList
{
    // No attributes
    ELAPILmNoAttributes = 0x0000,
    // The name of the landmark
    ELAPILmAttrName = 0x0001,
    // The position of the landmark
    ELAPILmAttrPosition = 0x0002,
    // The categories of the landmark
    ELAPILmAttrCategoryInfo = 0x0008,
    // The description of the landmark
    ELAPILmAttrDescription = 0x0020,
    // Additional attributes (not applicable to native API)
    ELAPILmAttrAddressInfo = 0x1000
};

// Address info fields which can be set to a landmark
enum TLAPIAddressInfoFields
{
    // Address field denoting address extension, e.g. flat number
    ELAPIAddressInfoExtension = 1,
    // Address field denoting street name and number
    ELAPIAddressInfoStreet,
    // Address field denoting zip or postal code
    ELAPIAddressInfoPostalCode,
    // Address field denoting town or city name
    ELAPIAddressInfoCity,
    // Address field denoting a county, which is between a state and a city
    ELAPIAddressInfoCounty,
    // Address field denoting state or province
    ELAPIAddressInfoState,
    // Address field denoting country
    ELAPIAddressInfoCountry,
    // Address field denoting country as a two-letter ISO 3166-1 code
    ELAPIAddressInfoCountryCode,
    // Address field denoting a municipal district
    ELAPIAddressInfoDistrict,
    // Address field denoting a building name
    ELAPIAddressInfoBuildingName,
    // Address field denoting a building floor
    ELAPIAddressInfoBuildingFloor,
    // Address field denoting a building room
    ELAPIAddressInfoBuildingRoom,
    // Address field denoting a building zone
    ELAPIAddressInfoBuildingZone,
    // Address field denoting a street in a crossing
    ELAPIAddressInfoCrossing1,
    // Address field denoting a street in a crossing (2nd)
    ELAPIAddressInfoCrossing2,
    // Address field denoting a URL for this place.
    ELAPIAddressInfoUrl,
    // Address field denoting a phone number for this place
    ELAPIAddressInfoPhoneNumber,
    // Number of addressInfo fields
    ELAPINumAddressInfos = ELAPIAddressInfoPhoneNumber
};

/**
 * Interface to a single Location API native landmark.
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(MLAPILandmark)
{

public: // New functions

    /**
     * Returns the unique id of this landmark object.
     *
     * It this landmark does not belong to any landmark store,
     * KLAPINullItemId is returned.
     *
     * The identifier cannot be externally changed. It is modified
     * during the database operations
     *
     * @return The item identifier of this landmark. KLAPINullItemId
     *         is returned if this landmark does not belong to a store
     */
    virtual TLAPIItemId Id() const = 0;

    /**
     * Sets the name of this landmark.
     *
     * The name must not be longer than the maximum specified length in
     * Landmarks API. Otherwise the saving of the landmark will fail
     * with KErrArgument
     *
     * @param aName The new name of the landmark.
     */
    virtual void SetNameL(const TDesC& aName) = 0;

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
    virtual const TDesC& NameL() = 0;

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
    virtual void SetDescriptionL(const TDesC* aDescription) = 0;

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
    virtual const TDesC* DescriptionL() = 0;

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
    virtual void SetCoordinatesL(const TLocality* aCoordinates) = 0;

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
    virtual const TLocality* CoordinatesL() = 0;

    /**
     * Sets the address information in this landmark.
     *
     * NULL argument indicates that the address information should
     * be removed from the landmark
     *
     * @param aAddressInfo The new address information for this landmark.
     *        The ownership is transferred to this class
     */
    virtual void SetAddressInfoL(CLAPIAddressInfo* aAddressInfo) = 0;

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
    virtual const CLAPIAddressInfo* AddressInfoL() = 0;

protected: // Constructor

    /**
     * Destructor. Does not allow to delete objects via this interface
     */
    virtual ~MLAPILandmark()
    {}

}
;

#endif // MLAPILANDMARK_H
// End of file
