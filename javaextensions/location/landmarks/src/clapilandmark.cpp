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


// INTERNAL INCLUDES
#include    "clapilandmark.h"
#include    "clapiaddressinfo.h"
#include    "mlapilandmarkstore.h"
#include    "lapipanics.h"
#include    "logger.h"
// EXTERNAL INCLUDES
#include    <EPos_CPosLandmark.h>
#include    <lbsfields.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// Number of the columns in the position field map
const TInt KLAPIPositionFieldMapColumnCount = 2;
// Address info field map which maps the Java address info
// fields and the native position fields which provide the same info
const TUint KLAPIPositionFieldMap[][KLAPIPositionFieldMapColumnCount] =
{
    { ELAPIAddressInfoExtension, EPositionFieldStreetExtension },
    { ELAPIAddressInfoStreet, EPositionFieldStreet },
    { ELAPIAddressInfoPostalCode, EPositionFieldPostalCode },
    { ELAPIAddressInfoCity, EPositionFieldCity },
    { ELAPIAddressInfoCounty, EPositionFieldCounty },
    { ELAPIAddressInfoState, EPositionFieldState },
    { ELAPIAddressInfoCountry, EPositionFieldCountry },
    { ELAPIAddressInfoCountryCode, EPositionFieldCountryCode },
    { ELAPIAddressInfoDistrict, EPositionFieldDistrict },
    { ELAPIAddressInfoBuildingName, EPositionFieldBuildingName },
    { ELAPIAddressInfoBuildingFloor, EPositionFieldBuildingFloor },
    { ELAPIAddressInfoBuildingRoom, EPositionFieldBuildingRoom },
    { ELAPIAddressInfoBuildingZone, EPositionFieldBuildingZone },
    { ELAPIAddressInfoCrossing1, EPositionFieldCrossing1 },
    { ELAPIAddressInfoCrossing2, EPositionFieldCrossing2 },
    { ELAPIAddressInfoUrl, EPositionFieldMediaLinksStart },
    { ELAPIAddressInfoPhoneNumber, EPositionFieldBuildingTelephone }
};
}

// ---------------------------------------------------------------------------
// CLAPILandmark::NewL
// ---------------------------------------------------------------------------
//
CLAPILandmark* CLAPILandmark::NewL(const TCtorParams& aParams)
{
    JELOG2(EJavaLocation);
    CLAPILandmark* self = CLAPILandmark::NewLC(aParams);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::NewLC
// ---------------------------------------------------------------------------
//
CLAPILandmark* CLAPILandmark::NewLC(const TCtorParams& aParams)
{
    JELOG2(EJavaLocation);
    CLAPILandmark* self = new(ELeave) CLAPILandmark(aParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::CLAPILandmark
// ---------------------------------------------------------------------------
//
CLAPILandmark::~CLAPILandmark()
{
    JELOG2(EJavaLocation);
    if (iLandmarkStore)
    {
        // Notify store that this landmark has been disposed
        iLandmarkStore->LandmarkDisposed(*this);
    }

    delete iAddressInfo;
    delete iLandmarkName;
    delete iDescription;
    delete iCoordinates;
    delete iLandmark;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::CLAPILandmark
// ---------------------------------------------------------------------------
//
TLAPIItemId CLAPILandmark::Id() const
{
    JELOG2(EJavaLocation);
    return iLandmark->LandmarkId();
}

// ---------------------------------------------------------------------------
// CLAPILandmark::CLAPILandmark
// ---------------------------------------------------------------------------
//
void CLAPILandmark::SetNameL(const TDesC& aName)
{
    JELOG2(EJavaLocation);
    HBufC* name = aName.AllocL();
    delete iLandmarkName;
    iLandmarkName = name;
    iChangedAttributes |= ELAPILmAttrName;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::CLAPILandmark
// ---------------------------------------------------------------------------
//
const TDesC& CLAPILandmark::NameL()
{
    JELOG2(EJavaLocation);
    // Check that the name has been read from the native landmark store
    // If not, then read the attribute and add it to the item
    if (!iLandmarkName && !(iChangedAttributes & ELAPILmAttrName))
    {
        TPtrC namePtr;
        // Try to get the name from the native landmark
        TInt err = iLandmark->GetLandmarkName(namePtr);
        // The name could not be read. Try reading it from the store
        if (err != KErrNone && iLandmarkStore && iLandmark->IsPartial())
        {
            // Read the name attribute from the store
            iLandmarkStore->ReadLandmarkAttributesL(*this, ELAPILmAttrName,
                                                    NULL);
            // Store the name attribute to this landmark. The name attribute
            // is added to the attributes of this landmark to indicate that the
            // field has been read from the native landmark store. This should
            // not return error since the name is always available after reading
            err = iLandmark->GetLandmarkName(namePtr);
        }
        // The landmark name should be available now if the landmark is in a store
        User::LeaveIfError(err);
        iLandmarkName = namePtr.AllocL();
    }

    // If the name was not read (i.e it has not been set yet and
    // this item does not belong to a database, return KNullDesC
    return !iLandmarkName ? KNullDesC() : *iLandmarkName;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::CLAPILandmark
// ---------------------------------------------------------------------------
//
void CLAPILandmark::SetDescriptionL(const TDesC* aDescription)
{
    JELOG2(EJavaLocation);
    HBufC* description(NULL);
    if (aDescription)
    {
        // The description must be concatenated because the API does not support
        // throwing correct exceptions when handling too long descriptions
        description = aDescription->Left(KPosLmMaxDescriptionLength).AllocL();
    }

    delete iDescription;
    iDescription = description;
    iChangedAttributes |= ELAPILmAttrDescription;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::CLAPILandmark
// ---------------------------------------------------------------------------
//
const TDesC* CLAPILandmark::DescriptionL()
{
    JELOG2(EJavaLocation);
    // Check that the description has been read from the native landmark store
    // If not, then read the attribute and add it to the item
    if (!iDescription && !(iChangedAttributes & ELAPILmAttrDescription))
    {
        TPtrC descPtr;
        // Try to get the description from the native landmark
        TInt err = iLandmark->GetLandmarkDescription(descPtr);
        // The description could not be read. Try reading it from the store
        if (err != KErrNone && iLandmarkStore && iLandmark->IsPartial())
        {
            // Read the name attribute from the store
            iLandmarkStore->ReadLandmarkAttributesL(*this,
                                                    ELAPILmAttrDescription, NULL);
            // Store the description attribute to this landmark. The  attribute
            // is added to the attributes of this landmark to indicate that the
            // field has been read from the native landmark store. This should
            // not return error since the field is always available after reading
            err = iLandmark->GetLandmarkDescription(descPtr);
        }
        // The description should be available now if it has been set
        if (err == KErrNone)
        {
            iDescription = descPtr.AllocL();
        }
    }

    return iDescription; // Returns NULL if not found
}

// ---------------------------------------------------------------------------
// CLAPILandmark::SetCoordinatesL
// ---------------------------------------------------------------------------
//
void CLAPILandmark::SetCoordinatesL(const TLocality* aCoordinates)
{
    JELOG2(EJavaLocation);
    TLocality* coordinates(NULL);
    if (aCoordinates)
    {
        coordinates = new(ELeave) TLocality(*aCoordinates);
    }

    delete iCoordinates;
    iCoordinates = coordinates;
    iChangedAttributes |= ELAPILmAttrPosition;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::CoordinatesL
// ---------------------------------------------------------------------------
//
const TLocality* CLAPILandmark::CoordinatesL()
{
    JELOG2(EJavaLocation);
    // Check that the position has been read from the native landmark store
    // If not, then read the attribute and add it to the item
    if (!iCoordinates && !(iChangedAttributes & ELAPILmAttrPosition))
    {
        // First, try to get the position from the native item
        TLocality loc;
        TInt err = iLandmark->GetPosition(loc);
        // The position has not been read. Try reading it from the store
        if (err != KErrNone && iLandmarkStore && iLandmark->IsPartial())
        {
            // Attributes must be read based on the previous ones
            iLandmarkStore->ReadLandmarkAttributesL(*this, ELAPILmAttrPosition,
                                                    NULL);
            // Store the position attribute to this landmark. The  attribute
            // is added to the attributes of this landmark to indicate that the
            // field has been read from the native landmark store. This should
            // not return error since the field is always available after reading
            err = iLandmark->GetPosition(loc);
        }
        // The position should be available now if it has been set
        if (err == KErrNone)
        {
            iCoordinates = new(ELeave) TLocality(loc);
        }
    }

    return iCoordinates; // Returns NULL if not found
}

// ---------------------------------------------------------------------------
// CLAPILandmark::SetAddressInfoL
// ---------------------------------------------------------------------------
//
void CLAPILandmark::SetAddressInfoL(CLAPIAddressInfo* aAddressInfo)
{
    JELOG2(EJavaLocation);
    delete iAddressInfo;
    iAddressInfo = aAddressInfo;
    // Mark that the address information has been changed
    iChangedAttributes |= ELAPILmAttrAddressInfo;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::AddressInfoL
// ---------------------------------------------------------------------------
//
const CLAPIAddressInfo* CLAPILandmark::AddressInfoL()
{
    JELOG2(EJavaLocation);
    // Read the address information if it has not been read or set yet
    if (!iAddressInfo && !(iChangedAttributes & ELAPILmAttrAddressInfo))
    {
        iAddressInfo = CLAPIAddressInfo::NewL();
        RArray<TUint> fields(ELAPINumAddressInfos);
        CleanupClosePushL(fields);
        // Add all supported address information fields to the array
        // mapped to Landmarks API position field identifiers
        for (TInt i = 0; i < ELAPINumAddressInfos; i++)
        {
            User::LeaveIfError(fields.Append(KLAPIPositionFieldMap[i][1]));
        }

        // Try to get address information from the native landmark
        TInt fieldCount = DoGetAddressInfoFieldValuesL(fields);
        // Read the position fields from the native landmark store if the
        // native landmark did not have any values in it and this landmark
        // belongs to a landmark store

        // Note that reading high amount of position field data is very
        // slow if the landmark has data in all fields
        if (!fieldCount && iLandmarkStore && iLandmark->IsPartial())
        {
            // Read supported position fields from the landmark store
            iLandmarkStore->ReadLandmarkAttributesL(*this, ELAPILmNoAttributes,
                                                    &fields);
            // Read the address information from the native landmark. The
            // return value is ignored because we do not need it here
            DoGetAddressInfoFieldValuesL(fields);
        }

        CleanupStack::PopAndDestroy(&fields);
    }

    return iAddressInfo; // Returns NULL if not found
}

// ---------------------------------------------------------------------------
// CLAPILandmark::PrepareForSaveL
// ---------------------------------------------------------------------------
//
void CLAPILandmark::PrepareForSaveL()
{
    JELOG2(EJavaLocation);
    // Read full landmark from the store to prevent user data loss. There
    // can be some fields which are not accessible using Location API and
    // the data of external landmarks must not be lost
    if (iLandmarkStore)
    {
        iLandmarkStore->ReadFullLandmarkL(*this);
    }

    // Clear all attributes other than the name. The landmark should always have
    // a name. Unchanged attributes must not be cleared since some other third
    // party application may have changed those attributes already
    iLandmark->RemoveLandmarkAttributes(iChangedAttributes & ~ELAPILmAttrName);

    if (iLandmarkName && iChangedAttributes & ELAPILmAttrName)
    {
        // The name of the landmark must not be too long when saving the entry
        __ASSERT_ALWAYS(iLandmarkName->Length() <= KPosLmMaxTextFieldLength,
                        User::Leave(KErrArgument));
        // Set the name of the landmark. Note that it must never be empty
        iLandmark->SetLandmarkNameL(*iLandmarkName);
    }

    if (iDescription && iChangedAttributes & ELAPILmAttrDescription)
    {
        iLandmark->SetLandmarkDescriptionL(*iDescription);
    }

    if (iCoordinates && iChangedAttributes & ELAPILmAttrPosition)
    {
        iLandmark->SetPositionL(*iCoordinates);
    }

    if (iChangedAttributes & ELAPILmAttrAddressInfo)
    {
        // Store address info to the native landmark
        for (TInt i = 0; i < ELAPINumAddressInfos; i++)
        {
            TUint32 positionField = KLAPIPositionFieldMap[i][1];
            if (!iAddressInfo || !iAddressInfo->Value(i + 1))
            {
                // Clear the position field information if the address info
                // array does not contain any data or the field is NULL
                iLandmark->RemovePositionField(positionField);
            }
            else
            {
                const TDesC* value = iAddressInfo->Value(i + 1);
                TPtrC truncatedValue = value->Left(KPosLmMaxTextFieldLength);
                // The position field has been set so update the value
                iLandmark->SetPositionFieldL(positionField, truncatedValue);
            }
        }
    }

    // Note that category information is not added on purpose because
    // this class is not responsible for handling the category information

    // All data is stored so reset changed attributes
    iChangedAttributes = ELAPILmNoAttributes;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::PosLandmark
// ---------------------------------------------------------------------------
//
CPosLandmark& CLAPILandmark::PosLandmark() const
{
    JELOG2(EJavaLocation);
    return *iLandmark;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::SetPosLandmark
// ---------------------------------------------------------------------------
//
void CLAPILandmark::SetPosLandmark(CPosLandmark* aLandmark)
{
    JELOG2(EJavaLocation);
    delete iLandmark;
    iLandmark = aLandmark;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::AssociateToStore
// ---------------------------------------------------------------------------
//
void CLAPILandmark::AssociateToStore(MLAPILandmarkStore* aLandmarkStore)
{
    JELOG2(EJavaLocation);
    // This landmark has been associated to a different store
    if (iLandmarkStore && iLandmarkStore != aLandmarkStore)
    {
        iLandmarkStore->LandmarkDisposed(*this);
    }

    iLandmarkStore = aLandmarkStore;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::StoreClosed
// ---------------------------------------------------------------------------
//
void CLAPILandmark::StoreClosed()
{
    JELOG2(EJavaLocation);
    iLandmarkStore = NULL;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::DoGetAddressInfoFieldValuesL
// ---------------------------------------------------------------------------
//
TInt CLAPILandmark::DoGetAddressInfoFieldValuesL(const RArray<TUint>& aPositionFields)
{
    JELOG2(EJavaLocation);
    // Address information should have been allocated before this
    __ASSERT_DEBUG(iAddressInfo, LAPIError::Panic(ELAPIPanicNullArgument));
    TInt valueCount = 0; // The number of available position fields

    // Read the position field value from each supported address information field
    TInt count = aPositionFields.Count();
    for (TInt i = 0; i < count; i++)
    {
        TPtrC fieldPtr;
        // Get the position field from the native landmark. At this point, if
        // the function returns an error, it indicates that there is no value
        // in the specified positioning field. Use NULL to indicate that
        TInt err = iLandmark->GetPositionField(aPositionFields[i], fieldPtr);
        if (err == KErrNone)
        {
            iAddressInfo->SetValueL(i + 1, &fieldPtr);
            valueCount++;
        }
    }

    return valueCount;
}

// ---------------------------------------------------------------------------
// CLAPILandmark::CLAPILandmark
// ---------------------------------------------------------------------------
//
CLAPILandmark::CLAPILandmark(const TCtorParams& aParams) :
        iLandmarkStore(aParams.iLandmarkStore), iLandmark(aParams.iLandmark),
        iChangedAttributes(ELAPILmNoAttributes)
{
    JELOG2(EJavaLocation);
}

// ---------------------------------------------------------------------------
// CLAPILandmark::ConstructL
// ---------------------------------------------------------------------------
//
void CLAPILandmark::ConstructL()
{
    JELOG2(EJavaLocation);
    // Check if this landmark is a new landmark
    if (!iLandmark)
    {
        iLandmark = CPosLandmark::NewL();
    }
}

// End of file
