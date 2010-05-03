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
#include "cstsprincipal.h"

namespace java
{
namespace satsa
{

CSTSPrincipal::CSTSPrincipal()
{
}

void CSTSPrincipal::ConstructL()
{
    // creating with empty values
    iPrincipalID = KNullDesC8().AllocL();
    iDomain = KNullDesC().AllocL();
}

CSTSPrincipal* CSTSPrincipal::NewLC()
{
    CSTSPrincipal* self = new(ELeave) CSTSPrincipal();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CSTSPrincipal::~CSTSPrincipal()
{
    delete iPrincipalID;
    delete iDomain;
}

// --------------------------------------------------------------------
// CSTSPrincipal::DecodeL
// Decrypts raw data to this instance
// --------------------------------------------------------------------
void CSTSPrincipal::DecodeL(const TDesC8& aRawData)
{
    TASN1DecGeneric decGen(aRawData);
    decGen.InitL();
    TInt tag = decGen.Tag();
    switch (tag)
    {
    case ERootID:
    case EEndEntityID:
    {
        iType = (TType) tag;
        if (decGen.Class() != EContextSpecific)
        {
            User::Leave(KErrArgument);
        }
        TASN1DecOctetString octetDecoder;
        TInt pos = 0; // next method's parameter is reference to int
        HBufC8* tmpPrincipalID =
            octetDecoder.DecodeDERL(decGen.Encoding(), pos);
        delete iPrincipalID;
        iPrincipalID = tmpPrincipalID;
        break;
    }
    case EDomain:
    {
        iType = EDomain;
        if (decGen.Class() != EContextSpecific)
        {
            User::Leave(KErrArgument);
        }
        TInt pos = 0; //next method's parameter is reference to int
        TASN1DecObjectIdentifier decOidDomain;
        HBufC* oid = decOidDomain.DecodeDERL(decGen.Encoding(), pos);
        delete iDomain;
        iDomain = oid;
        break;
    }
    default:
    {
        //rest is for future extensions, we will not need them
    }
    }
}
// --------------------------------------------------------------------
// CSTSPrincipal::Type
// Getter for principal type
// --------------------------------------------------------------------
CSTSPrincipal::TType CSTSPrincipal::Type() const
{
    return iType;
}

// --------------------------------------------------------------------
// CSTSPrincipal::PrincipalID
// Getter for principal id
// --------------------------------------------------------------------
const TDesC8& CSTSPrincipal::PrincipalID() const
{
    return *iPrincipalID;
}

// ---------------------------------------------------------------------
// CSTSPrincipal::Domain
// Getter for domain
// ---------------------------------------------------------------------
const TDesC& CSTSPrincipal::Domain() const
{
    return *iDomain;
}

} // namespace satsa
} // namespace java
//  End of File
