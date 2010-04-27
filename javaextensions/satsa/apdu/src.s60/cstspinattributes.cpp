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
* Description:
 *
*/


// INCLUDE FILES
#include "cstspinattributes.h"

namespace java
{
namespace satsa
{

// CONSTANTS

//0000 0000 1000 0000 -->9. bit from left but first byte ignored
const TUint8 KSTSDisableAllowed = 0x80;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSPinAttributes::CSTSPinAttributes
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSPinAttributes::CSTSPinAttributes()
{
    iPinType = ENotInitialized;
    iMaxLength = KErrNotFound;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::CSTSPinAttributes
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSPinAttributes::CSTSPinAttributes(TInt aPinType, TInt aMinLength,
                                     TInt aStoredLength, TInt aMaxLength, TInt aPinReference)
{

    iPinType = (TPinType) aPinType;

    //if maxLenght is zero or less it is not found correctly
    if (aMaxLength <= 0)
    {
        iMaxLength = KErrNotFound;
    }
    else
    {
        iMaxLength = aMaxLength;
    }

    iMinLength = aMinLength;
    iStoredLength = aStoredLength;

    iPinReference = aPinReference;

}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSPinAttributes::ConstructL()
{
    // creating with empty values
    iPinFlags = KNullDesC8().AllocL();
    iPadChar = KNullDesC8().AllocL();
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSPinAttributes::ConstructL(TInt aPadChar, TInt aFlags)
{
    // creating with empty values
    iPinFlags = HBufC8::NewL(2);//pinflags contains maximum of two bytes of data
    iPinFlags->Des().Append(aFlags);
    iPadChar = HBufC8::NewL(1);
    iPadChar->Des().Append(aPadChar);
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSPinAttributes* CSTSPinAttributes::NewLC(TInt aPinType, TInt aMinLength,
        TInt aStoredLength, TInt aMaxLength, TInt aPinReference, TInt aPadChar,
        TInt aFlags)
{
    CSTSPinAttributes
    * self =
        new(ELeave) CSTSPinAttributes(aPinType, aMinLength, aStoredLength, aMaxLength, aPinReference);
    CleanupStack::PushL(self);
    self->ConstructL(aPadChar, aFlags);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSPinAttributes* CSTSPinAttributes::NewL()
{
    CSTSPinAttributes* self = new(ELeave) CSTSPinAttributes();

    return self;
}

// Destructor
CSTSPinAttributes::~CSTSPinAttributes()
{
    delete iPinFlags;
    delete iPadChar;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::PinType
// Getter for PIN type
// -----------------------------------------------------------------------------
CSTSPinAttributes::TPinType CSTSPinAttributes::PinType() const
{
    return iPinType;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::MinLength
// Getter for PIN minimum length
// -----------------------------------------------------------------------------
TInt CSTSPinAttributes::MinLength() const
{
    return iMinLength;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::StoredLength
// Getter for PIN stored lenght
// -----------------------------------------------------------------------------

TInt CSTSPinAttributes::StoredLength() const
{
    return iStoredLength;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::MaxLength
// Getter for PIN maximum length
// -----------------------------------------------------------------------------

TInt CSTSPinAttributes::MaxLength() const
{
    return iMaxLength;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::PinReference
// Getter for PIN reference
// -----------------------------------------------------------------------------

TUint8 CSTSPinAttributes::PinReference() const
{
    return iPinReference;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::PadChar
// Getter for padding character
// -----------------------------------------------------------------------------

const TDesC8& CSTSPinAttributes::PadChar() const
{
    return *iPadChar;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::CopyL
// Takes information from parameter and sets it, takes copy
// -----------------------------------------------------------------------------
void CSTSPinAttributes::CopyL(const CSTSPinAttributes& aPinAttributes)
{
    HBufC8* tmpPinFlags = aPinAttributes.iPinFlags->AllocL();
    delete iPinFlags;
    iPinFlags = tmpPinFlags;

    iPinType = aPinAttributes.PinType();
    iMinLength = aPinAttributes.MinLength();
    iStoredLength = aPinAttributes.StoredLength();
    iMaxLength = aPinAttributes.MaxLength();
    iPinReference = aPinAttributes.PinReference();

    HBufC8* tmpPadChar = aPinAttributes.PadChar().AllocL();
    delete iPadChar;
    iPadChar = tmpPadChar;
}

// -----------------------------------------------------------------------------
// CSTSPinAttributes::IsPinFlagSet
// Uses mask to find out is flag set or not. Mask is already set in header file
// where TFlagType is defined. Supports only case sensitive and needs-padding
// flags.
// -----------------------------------------------------------------------------
TBool CSTSPinAttributes::IsPinFlagSet(TFlagType aFlagType) const
{

    TBool returnValue = EFalse;
    TInt8 correctByte = 0x00;

    //case sensitive, needs-padding, change-disabled and unblock-disabled
    //information will be found from first byte
    TInt index = 0;

    //disable allowed information will be found from second byte
    if (aFlagType == EDisableAllowed)
    {
        //special case because value is in second byte of iPinFlags
        //and thats why when presenting flagtype in one byte form
        //DisableAllowed has same value than ECaseSensitive flag.
        //Thats why own constant for disable allowed.
        aFlagType = (TFlagType) KSTSDisableAllowed;
    }

    if (iPinFlags->Length() > index)
    {
        correctByte = iPinFlags->Ptr()[index];
        //check is wanted bit set or not
        returnValue = ((aFlagType & correctByte) == aFlagType);
    }
    //if correctByte is missing, we return EFalse
    return returnValue;
}

} // namespace satsa
} // namespace java
//  End of File
