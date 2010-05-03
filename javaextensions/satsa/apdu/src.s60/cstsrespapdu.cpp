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
#include  "cstsrespapdu.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TUint8 KSTSNormalEndingSW1 = 0x90;
const TUint8 KSTSNormalEndingSW2 = 0x00;
const TUint8 KSTSNormalEndingSW1WithExtraInfo = 0x91;
const TUint8 KSTSGetResponseSW1Normal = 0x61;
const TUint8 KSTSGetResponseSW1USIM1 = 0x9E;
const TUint8 KSTSGetResponseSW1USIM2 = 0x9F;
const TUint8 KSTSGetResponseSW1EmtpyLe = 0x62;
const TUint8 KSTSGetResponseSW1EmtpyLe2 = 0x63;
const TUint8 KSTSResendSW1 = 0x6C;

// ============================ MEMBER FUNCTIONS ===============================


CSTSRespApdu::CSTSRespApdu()
{
    iConnTarget = ESTSAID;
}

void CSTSRespApdu::ConstructL(TInt aMaxLength,
                              CSTSApdu::TSTSApduStandard aStandard)
{
    iRespData = CSTSApdu::NewL(aMaxLength, aStandard);
}

CSTSRespApdu* CSTSRespApdu::NewLC(TInt aMaxLength,
                                  CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSRespApdu* self = new(ELeave) CSTSRespApdu();
    CleanupStack::PushL(self);
    self->ConstructL(aMaxLength, aStandard);
    return self;
}

CSTSRespApdu* CSTSRespApdu::NewL(TInt aMaxLength,
                                 CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSRespApdu* self = CSTSRespApdu::NewLC(aMaxLength, aStandard);
    CleanupStack::Pop(self);
    return self;
}

CSTSRespApdu::~CSTSRespApdu()
{
    delete iRespData;
}

// -----------------------------------------------------------------------------
// CSTSRespApdu::IsNormalEnding
// Panics, if apdu data buffer contains too little information.
// Checks if SW1 and SW2 bytes of data contain normal ending information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSRespApdu::IsNormalEnding() const
{
    TBool returnValue = EFalse;
    TUint8 sw1 = GetSW1();

    if ((sw1 == KSTSNormalEndingSW1 && GetSW2() == KSTSNormalEndingSW2) || (sw1
            == KSTSNormalEndingSW1WithExtraInfo))
    {
        returnValue = ETrue;
    }

    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSRespApdu::GetResponseNeeded
// Panics, if apdu data buffer contains too little information.
// Checks if SW1 byte contain information that GET RESPONSE APDU is needed.
// Saves to reference parameter new expected response lenth.
// Implementation is done according to JSR177 SATSA-APDU part.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSRespApdu::GetResponseNeeded(TUint8& aNewLength) const
{

    TBool returnValue = EFalse;
    TUint8 sw1 = GetSW1();
    // get response is needed with new length
    if ((sw1 == KSTSGetResponseSW1Normal) || // both AID/USAT OR
            ((iConnTarget == ESTSUSAT) && // only in USAT
             ((sw1 == KSTSGetResponseSW1USIM1) || (sw1
                                                   == KSTSGetResponseSW1USIM2))))
    {
        returnValue = ETrue;
        aNewLength = GetSW2();
    }
    // get response is needed with empty length in USAT connection
    else if ((iConnTarget == ESTSUSAT) && ((sw1 == KSTSGetResponseSW1EmtpyLe)
                                           || (sw1 == KSTSGetResponseSW1EmtpyLe2)))
    {
        returnValue = ETrue;
        aNewLength = 0x00; // must be emtpy in this case
    }
    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSRespApdu::ResendNeeded
// Panics, if apdu data buffer contains too little information.
// Checks if SW1 byte contain information that apdu resending is needed.
// Saves to reference parameter new expected response lenth.
// Implementation is done according to JSR177 SATSA-APDU part.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSRespApdu::ResendNeeded(TUint8& aNewLength) const
{

    TBool returnValue = EFalse;
    // resend is needed with new length
    if (GetSW1() == KSTSResendSW1)
    {
        returnValue = ETrue;
        aNewLength = GetSW2();
    }
    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSRespApdu::GetSW1
// Panics, if apdu data buffer contains too little information.
// Second last byte of data is SW1
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TUint8 CSTSRespApdu::GetSW1() const
{
    // SW1 is second last byte
    return iRespData->ApduPtr().Ptr()[iRespData->ApduPtr().Length() - 2];
}

// -----------------------------------------------------------------------------
// CSTSRespApdu::GetSW2
// Panics, if apdu data buffer contains too little information.
// Last byte of data is SW2
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TUint8 CSTSRespApdu::GetSW2() const
{
    // SW2 is last byte
    return iRespData->ApduPtr().Ptr()[iRespData->ApduPtr().Length() - 1];
}

// -----------------------------------------------------------------------------
// CSTSRespApdu::ResponseBytes
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TPtr8& CSTSRespApdu::ResponseBytes() const
{
    return iRespData->ApduPtr();
}

// -----------------------------------------------------------------------------
// CSTSRespApdu::SetConnectionTarget
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSRespApdu::SetConnectionTarget(TSTSConnectionTarget aConnTarget)
{
    iConnTarget = aConnTarget;
}

} // namespace satsa
} // namespace java
//  End of File
