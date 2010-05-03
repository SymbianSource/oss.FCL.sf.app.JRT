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


//  INCLUDE FILES
#include "cstsapduexchanger.h"
#include "cstschannelmanager.h"
#include "cstsmanagechannel.h"
#include "cstsmanagechannelresp.h"
#include "stsapduconstants.h"
#include "logger.h"

namespace java
{
namespace satsa
{

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSChannelManager::CSTSChannelManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSChannelManager::CSTSChannelManager(CSTSApduExchanger* aApduExchanger)
{
    iApduExchanger = aApduExchanger;
}

// -----------------------------------------------------------------------------
// CSTSChannelManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSChannelManager::ConstructL()
{
    iCmdApdu = CSTSManageChannel::NewL(0, //can be zero at first
                                       CSTSManageChannel::ESTSOpenChannel, CSTSApdu::ESTSUICC);
    iRespApdu = CSTSManageChannelResp::NewL(KSTSLogicalChannelsMax, //three in Open channel case
                                            CSTSApdu::ESTSUICC);

}

// -----------------------------------------------------------------------------
// CSTSChannelManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSChannelManager* CSTSChannelManager::NewL(CSTSApduExchanger* aApduExchanger)
{
    CSTSChannelManager* self = new(ELeave) CSTSChannelManager(aApduExchanger);

    CleanupStack::PushL(self);
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSChannelManager::~CSTSChannelManager()
{
    delete iCmdApdu;
    delete iRespApdu;
}

// -----------------------------------------------------------------------------
// CSTSChannelManager::RequestChannelL
// Uses MANAGE CHANNEL APDU for opening any channel. Returns opened channel if
// open was succesfull or otherwise leave code.
// or KErrNotFound if there was no free channels
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSTSChannelManager::RequestChannel()
{
    LOG(ESATSA, EInfo, "+ CSTSChannelManager::RequestChannel");
    TInt returnValue;

    //channel number chan be left empty in open channel case
    iCmdApdu->SetApduType(CSTSManageChannel::ESTSOpenChannel, 0);

    returnValue = iApduExchanger->ExchangeApdu(*iCmdApdu, *iRespApdu);

    if (returnValue == KErrNone)
    {
        if (!iRespApdu->IsNormalEnding())
        {
            returnValue = KErrNotFound;
        }
        else
        {
            returnValue = iRespApdu->GetChannel();
        }
    }
    LOG(ESATSA, EInfo, "+ CSTSChannelManager::RequestChannel");
    return returnValue;

}

// -----------------------------------------------------------------------------
// CSTSChannelManager::CloseChannelL
// Uses manage channel apdu for closing the channel. Returns error code if
// something was wrong.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSTSChannelManager::CloseChannel(TInt aChannel)
{
    TInt err = KErrNone;
    //channel 0 can not be closed
    if (aChannel > 0)
    {
        iCmdApdu->SetApduType(CSTSManageChannel::ESTSCloseChannel, aChannel);

        err = iApduExchanger->ExchangeApdu(*iCmdApdu, *iRespApdu);

        if (err == KErrNone)
        {
            if (!iRespApdu->IsNormalEnding())
            {
                err = KErrGeneral;
            }
        }
    }
    return err;
}

} // namespace satsa
} // namespace java
//  End of File


