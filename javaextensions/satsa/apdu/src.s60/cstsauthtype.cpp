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

#include "cstsauthtype.h"

namespace java
{
namespace satsa
{


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSAuthType::CSTSAuthType
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSAuthType::CSTSAuthType()
{
}

// -----------------------------------------------------------------------------
// CSTSAuthType::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSAuthType::ConstructL()
{
    // creating with empty values
    iAuthId = KNullDesC8().AllocL();
    iLabel = KNullDesC().AllocL();
    iPinAttributes = CSTSPinAttributes::NewL();
}

// -----------------------------------------------------------------------------
// CSTSAuthType::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSAuthType* CSTSAuthType::NewLC()
{
    CSTSAuthType* self = new(ELeave) CSTSAuthType();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CSTSAuthType::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSAuthType* CSTSAuthType::NewL()
{

    CSTSAuthType* self = CSTSAuthType::NewLC();
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSAuthType::~CSTSAuthType()
{
    delete iAuthId;
    delete iLabel;
    delete iPinAttributes;
}

// -----------------------------------------------------------------------------
// CSTSAuthType::AuthID
// Getter for Authentication ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC8& CSTSAuthType::AuthID() const
{
    return *iAuthId;
}

// -----------------------------------------------------------------------------
// CSTSAuthType::SetAuthIDL
// Setter for Authentication ID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSAuthType::SetAuthIDL(TInt aAuthId)
{

    HBufC8* tmpAuthId = HBufC8::NewL(1);//first
    delete iAuthId;
    iAuthId = tmpAuthId;
    iAuthId->Des().Append(aAuthId);

}

// -----------------------------------------------------------------------------
// CSTSAuthType::Label
// Getter for label
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC& CSTSAuthType::Label() const
{
    return *iLabel;
}

// -----------------------------------------------------------------------------
// CSTSAuthType::SetLabelL
// Setter for label
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSAuthType::SetLabelL(const TDesC& aLabel)
{
    HBufC* tmpLabel = aLabel.AllocL();
    delete iLabel;
    iLabel = tmpLabel;
}

// -----------------------------------------------------------------------------
// CSTSAuthType::PinAttributes
// Getter for PIN attributes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const CSTSPinAttributes& CSTSAuthType::PinAttributes() const
{
    return *iPinAttributes;
}

// -----------------------------------------------------------------------------
// CSTSAuthType::SetPinAttributesL
// Getter for PIN attributes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSAuthType::SetPinAttributesL(const CSTSPinAttributes& aPinAttr)
{
    iPinAttributes->CopyL(aPinAttr);
}

// -----------------------------------------------------------------------------
// CSTSPath::CopyL
// Takes information from parameter and sets it, takes copy
// -----------------------------------------------------------------------------
void CSTSAuthType::CopyL(const CSTSAuthType& aAuthType)
{

    HBufC8* tmpId = aAuthType.AuthID().AllocL();
    delete iAuthId;
    iAuthId = tmpId;

    HBufC* tmpLabel = aAuthType.Label().AllocL();
    delete iLabel;
    iLabel = tmpLabel;

    iPinAttributes->CopyL(aAuthType.PinAttributes());

}

} // namespace satsa
} // namespace java
//  End of File
