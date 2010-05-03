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
#include <asn1dec.h>
#include "cstsapdumaskpermission.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSMinNumberOfSubModules = 2;
const TInt KSTSMaxNumberOfSubModules = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSAPDUMaskPermission::CSTSAPDUMaskPermission
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSAPDUMaskPermission::CSTSAPDUMaskPermission()
{
}

// -----------------------------------------------------------------------------
// CSTSAPDUMaskPermission::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSAPDUMaskPermission::ConstructL()
{
    // creating with empty values
    iApduMask = KNullDesC8().AllocL();
    iApduHeader = KNullDesC8().AllocL();
}

// -----------------------------------------------------------------------------
// CSTSAPDUMaskPermission::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSAPDUMaskPermission* CSTSAPDUMaskPermission::NewLC()
{
    CSTSAPDUMaskPermission* self = new(ELeave) CSTSAPDUMaskPermission();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// Destructor
CSTSAPDUMaskPermission::~CSTSAPDUMaskPermission()
{
    delete iApduMask;
    delete iApduHeader;
}

// -----------------------------------------------------------------------------
// CSTSAPDUMaskPermission::DecodeL
// Decrypts raw data to this instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSAPDUMaskPermission::DecodeL(const TDesC8& aRawData)
{
    CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC(EFalse, //no need to check sequence tag
                                            aRawData, KSTSMinNumberOfSubModules, KSTSMaxNumberOfSubModules);
    // we would not get this far if there is not 2 elements
    TInt pos = 0;

    //decoding apdu header (octet string)
    TASN1DecOctetString apduHeader;
    HBufC8* tmpHeader = apduHeader.DecodeDERL(*itemsData->At(pos++));
    CleanupStack::PushL(tmpHeader);

    //decoding apdu mask (octet string)
    TASN1DecOctetString apduMask;
    HBufC8* tmpMask = apduMask.DecodeDERL(*itemsData->At(pos++));

    CleanupStack::Pop(tmpHeader);
    delete iApduHeader;
    iApduHeader = tmpHeader;
    delete iApduMask;
    iApduMask = tmpMask;

    CleanupStack::PopAndDestroy(itemsData);

}

// -----------------------------------------------------------------------------
// CSTSAPDUMaskPermission::ApduMask
// Getter for apdu mask
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC8& CSTSAPDUMaskPermission::ApduMask() const
{
    return *iApduMask;
}

// -----------------------------------------------------------------------------
// CSTSAPDUMaskPermission::ApduHeader
// Getter for apdu header
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC8& CSTSAPDUMaskPermission::ApduHeader() const
{
    return *iApduHeader;
}

} // namespace satsa
} // namespace java
//  End of File
