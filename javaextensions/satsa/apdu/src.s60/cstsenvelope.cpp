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

#include  "cstsenvelope.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TUint8 KSTSCLAEnvelopeUMTS = 0x80;
const TUint8 KSTSCLAEnvelopeGSM = 0xA0;
const TUint8 KSTSINSEnvelope = 0xC2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSEnvelope::CSTSEnvelope
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSEnvelope::CSTSEnvelope()
{
}

// -----------------------------------------------------------------------------
// CSTSEnvelope::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSEnvelope* CSTSEnvelope::NewLC(TDesC8& aApduBytes,
                                  CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSEnvelope* self = new(ELeave) CSTSEnvelope();
    CleanupStack::PushL(self);
    self->ConstructL(aApduBytes, aStandard);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSEnvelope::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSEnvelope* CSTSEnvelope::NewL(TDesC8& aApduBytes,
                                 CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSEnvelope* self = CSTSEnvelope::NewLC(aApduBytes, aStandard);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CSTSEnvelope::~CSTSEnvelope()
{
}

// -----------------------------------------------------------------------------
// CSTSEnvelope::CheckL
// Checks apdu data and leaves with proper error code
// Checking is done according to JSR177 design document.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSEnvelope::CheckL() const
{
    CheckApduLengthL();
    //only envelope apdus are allowed
    if (ApduByte(ESTSINS) != KSTSINSEnvelope) //not envelope
    {
        User::Leave(KSTSErrIllegalArgument + KSTSErrIANotEnvelope);
    }
}

// -----------------------------------------------------------------------------
// CSTSEnvelope::SetClassByte
// Checks which standard is used and sets the class byte either A0 or 80
// depending on whether the phone is running GSM or UMTS.
// Specified in JSR177 in chapter "Support for (U)SIM Application Toolkit".
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSEnvelope::SetClassByte()
{
    if (iApduData->Standard() == CSTSApdu::ESTSGSM)
    {
        SetApduByte(KSTSCLAEnvelopeGSM, ESTSCLA);
    }
    else if (iApduData->Standard() == CSTSApdu::ESTSUICC)
    {
        SetApduByte(KSTSCLAEnvelopeUMTS, ESTSCLA);
    }
}

} // namespace satsa
} // namespace java
//  End of File
