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

#include  "cstsmanagechannel.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TUint8 KSTSP1OpenChannel = 0x00;
const TUint8 KSTSP1CloseChannel = 0x80;
const TUint8 KSTSP2AnyChannel = 0x00;
const TUint8 KSTSINSManageChannel = 0x70;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSManageChannel::CSTSManageChannel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSManageChannel::CSTSManageChannel()
{
}

// -----------------------------------------------------------------------------
// CSTSManageChannel::ConstructL
// Symbian 2nd phase constructor can leave.
// Constructs Manage Channel apdu for opening or closing channel
// If opening channel, aChannel variable is not used.
// -----------------------------------------------------------------------------
//
void CSTSManageChannel::ConstructL(TInt aChannel,
                                   TSTSManageChannelApduType aType, CSTSApdu::TSTSApduStandard aStandard)
{
    TUint8 p1 = 0x00;
    TUint8 p2 = 0x00;

    TInt maxLength = KSTSApduMandatoryHeaderLen + 1;

    GenerateP1P2(p1, p2, aChannel, aType);

    iApduData = CSTSApdu::NewL(maxLength, aStandard);

    // fills whole area with zeros
    iApduData->ApduPtr().FillZ(maxLength);

    SetHeader(KSTSCLABasicUMTS, KSTSINSManageChannel, p1, p2);

    // we expect 1 byte long response, which contains channel number
    if (aType == ESTSOpenChannel)
    {
        SetLe(0x01);
    }
    // else no response expected, Le is zero
}
// -----------------------------------------------------------------------------
// CSTSManageChannel::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSManageChannel* CSTSManageChannel::NewLC(TInt aChannel,
        TSTSManageChannelApduType aType, CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSManageChannel* self = new(ELeave) CSTSManageChannel();
    CleanupStack::PushL(self);
    self->ConstructL(aChannel, aType, aStandard);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSManageChannel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSManageChannel* CSTSManageChannel::NewL(TInt aChannel,
        TSTSManageChannelApduType aType, CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSManageChannel* self = CSTSManageChannel::NewLC(aChannel, aType,
                              aStandard);
    CleanupStack::Pop(self);

    return self;
}

// -----------------------------------------------------------------------------
// CSTSManageChannel::SetApduType
//
// -----------------------------------------------------------------------------
//
void CSTSManageChannel::SetApduType(TSTSManageChannelApduType aType,
                                    TInt aChannel)
{
    iType = aType;

    TUint8 p1;
    TUint8 p2;
    GenerateP1P2(p1, p2, aChannel, aType);
    SetApduByte(p1, ESTSP1);
    SetApduByte(p2, ESTSP2);

    // we expect 1 byte long response, which contains channel number
    if (aType == ESTSOpenChannel)
    {
        SetLe(0x01);
    }
    else
    {
        // no response expected, Le is zero
        SetLe(0x00);
    }
}

// -----------------------------------------------------------------------------
// CSTSManageChannel::GenerateP1P2
//
// -----------------------------------------------------------------------------
//
void CSTSManageChannel::GenerateP1P2(TUint8& aP1, TUint8& aP2, TInt aChannel,
                                     TSTSManageChannelApduType aType)
{

    switch (aType)
    {
    case ESTSOpenChannel:
    {
        aP1 = KSTSP1OpenChannel;
        aP2 = KSTSP2AnyChannel;
        break;
    }
    case ESTSCloseChannel:
    {
        aP1 = KSTSP1CloseChannel;
        aP2 = (TUint8) aChannel;
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
