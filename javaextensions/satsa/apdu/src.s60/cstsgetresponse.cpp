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

#include  "cstsgetresponse.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TUint8 KSTSINSGetResponse = 0xC0;
const TUint8 KSTSP1GetResponse = 0x00;
const TUint8 KSTSP2GetResponse = 0x00;
const TInt KSTSGetResponseApduLength = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSGetResponse::CSTSGetResponse
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSGetResponse::CSTSGetResponse()
{
}

// -----------------------------------------------------------------------------
// CSTSGetResponse::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSGetResponse* CSTSGetResponse::NewLC(TInt aExpectedRespLength,
                                        CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSGetResponse* self = new(ELeave) CSTSGetResponse();
    CleanupStack::PushL(self);
    self->ConstructL(aExpectedRespLength, aStandard);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSGetResponse::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSGetResponse* CSTSGetResponse::NewL(TInt aExpectedRespLength,
                                       CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSGetResponse* self = CSTSGetResponse::NewLC(aExpectedRespLength,
                            aStandard);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CSTSGetResponse::~CSTSGetResponse()
{
}

// -----------------------------------------------------------------------------
// CSTSGetResponse::SetExpectedResponseLength
// Sets Le value (in this case same location than Lc).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSGetResponse::SetExpectedResponseLength(TInt aExpectedRespLength)
{

    SetApduByte((TUint8) aExpectedRespLength, ESTSLc);
}

// -----------------------------------------------------------------------------
// CSTSGetResponse::ConstructL
// Symbian 2nd phase constructor can leave.
// Constructs Get Response apdu.
// APDU is constructed according to standard ETSI TS 102 221
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSGetResponse::ConstructL(TInt aExpectedRespLength,
                                 CSTSApdu::TSTSApduStandard aStandard)
{
    iApduData = CSTSApdu::NewL(KSTSGetResponseApduLength, aStandard);

    //fills needed area with zeros
    iApduData->ApduPtr().FillZ(KSTSGetResponseApduLength);

    SetHeader(KSTSCLABasicUMTS, KSTSINSGetResponse, KSTSP1GetResponse,
              KSTSP2GetResponse);

    //we expect gived bytes long response
    SetLe((TUint8) aExpectedRespLength);

}

} // namespace satsa
} // namespace java
//  End of File
