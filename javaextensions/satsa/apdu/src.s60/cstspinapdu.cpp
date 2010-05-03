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

#include  "cstspinapdu.h"

namespace java
{
namespace satsa
{

// CONSTANTS
//PIN related constants according to standard ETSI TS 102 221
const TUint8 KSTSINSVerifyPIN = 0x20;
const TUint8 KSTSINSChangePIN = 0x24;
const TUint8 KSTSINSDisablePIN = 0x26;
const TUint8 KSTSINSEnablePIN = 0x28;
const TUint8 KSTSINSUnblockPIN = 0x2C;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSPinApdu::CSTSPinApdu
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSPinApdu::CSTSPinApdu()
{
}

// -----------------------------------------------------------------------------
// CSTSPinApdu::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSPinApdu* CSTSPinApdu::NewLC(TSTSPinApduType aType, TUint8 aP2,
                                CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSPinApdu* self = new(ELeave) CSTSPinApdu();
    CleanupStack::PushL(self);
    self->ConstructL(aType, aP2, aStandard);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSPinApdu::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSPinApdu* CSTSPinApdu::NewL(TSTSPinApduType aType, TUint8 aP2,
                               CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSPinApdu* self = CSTSPinApdu::NewLC(aType, aP2, aStandard);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CSTSPinApdu::~CSTSPinApdu()
{
}

// -----------------------------------------------------------------------------
// CSTSPinApdu::ConstructL
//
// -----------------------------------------------------------------------------
//
void CSTSPinApdu::ConstructL(TSTSPinApduType aType, TUint8 aP2,
                             CSTSApdu::TSTSApduStandard aStandard)
{
    iApduData = CSTSApdu::NewL(KSTSApduMandatoryHeaderLen, aStandard);
    iApduData->ApduPtr().FillZ(KSTSApduMandatoryHeaderLen);

    SetApduByte(KSTSCLABasicUMTS, ESTSCLA);
    SetPinINS(aType);
    SetApduByte(0x00, ESTSP1); //empty in all cases
    SetApduByte(aP2, ESTSP2);
}

// -----------------------------------------------------------------------------
// CSTSPinApdu::SetPinINS
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinApdu::SetPinINS(TSTSPinApduType aType)
{

    switch (aType)
    {
    case ESTSVerifyPin:
    {
        SetApduByte(KSTSINSVerifyPIN, ESTSINS);
        break;
    }
    case ESTSChangePin:
    {
        SetApduByte(KSTSINSChangePIN, ESTSINS);
        break;
    }
    case ESTSDisablePin:
    {
        SetApduByte(KSTSINSDisablePIN, ESTSINS);
        break;
    }
    case ESTSEnablePin:
    {
        SetApduByte(KSTSINSEnablePIN, ESTSINS);
        break;
    }
    case ESTSUnblockPin:
    {
        SetApduByte(KSTSINSUnblockPIN, ESTSINS);
        break;
    }
    default:
    {
        break;
    }
    }
}

} // namespace satsa
} // namespace java
//  End of File
