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
#include "cstsacie.h"
#include "cstspath.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSMinNumberOfSubModules = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSAcie::CSTSAcie
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSAcie::CSTSAcie()
{
}

// -----------------------------------------------------------------------------
// CSTSAcie::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSAcie::ConstructL()
{
    // creating with empty values
    iAid = KNullDesC8().AllocL();
    iACFPath = CSTSPath::NewL();
}

// -----------------------------------------------------------------------------
// CSTSAcie::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSAcie* CSTSAcie::NewLC()
{
    CSTSAcie* self = new(ELeave) CSTSAcie();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// Destructor
CSTSAcie::~CSTSAcie()
{
    delete iAid;
    delete iACFPath;
}

// -----------------------------------------------------------------------------
// CSTSAcie::DecodeL
// Decrypts raw data to this instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSAcie::DecodeL(const TDesC8& aRawData)
{
    CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC(ETrue, //must be sequence
                                            aRawData, KSTSMinNumberOfSubModules);

    // we would not get this far if there is not 1 element at minimum
    TInt pos = 0;
    //decoding optional aid (octet string)
    if ((pos < itemsData->Count()) && (itemsData->At(0)->Tag()
                                       == EASN1OctetString))
    {
        TASN1DecOctetString aid;
        HBufC8* tmp = aid.DecodeDERL(*itemsData->At(pos++));
        delete iAid;
        iAid = tmp;
    }
    CSTSPath* acfPath = CSTSPath::NewLC();
    acfPath->DecodeL(itemsData->At(pos)->Encoding());
    CleanupStack::Pop(acfPath);
    delete iACFPath;
    iACFPath = acfPath;
    CleanupStack::PopAndDestroy(itemsData);

}

// -----------------------------------------------------------------------------
// CSTSAcie::AID
// Getter for aid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC8& CSTSAcie::AID() const
{
    return *iAid;
}

// -----------------------------------------------------------------------------
// CSTSAcie::ACFPath
// Getter for ACF path
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const CSTSPath& CSTSAcie::ACFPath() const
{
    return *iACFPath;
}

} // namespace satsa
} // namespace java
//  End of File
