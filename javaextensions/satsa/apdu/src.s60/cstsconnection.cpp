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

#include "cstsconnection.h"
//  INCLUDE FILES
#include "s60commonutils.h"
#include "cstsconnection.h"
#include "cstschannelmanager.h"
#include "cstsslotmanager.h"
#include "cstspinmanager.h"
#include "cstsapduexchanger.h"
#include "cstsaccesscontrol.h"
#include "cstscardoperationsfacade.h"
#include "cstscardappmanager.h"
#include "cstsenvelope.h"
#include "cstsresphandler.h"
#include "cstsselectfile.h"
#include "cstsrespapdu.h"
#include "cstsmidletinfo.h"
#include "cstsuri.h"
#include "cstsbtsaplistener.h"
#include "cstscardstatelistener.h"
#include "fs_methodcall.h"
#include "logger.h"
#include "javasymbianoslayer.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSBasicChannel = 0;
const TInt KATRMaxLength = 0xFF;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSConnection::CSTSConnection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSConnection::CSTSConnection(MSTSRespHandler* aRespHandler,
                               CSTSMidletInfo* aMidletInfo) :
        java::util::FunctionServer("MyAPDUServer"), iChannel(0)
{
    LOG(ESATSA, EInfo,  "+ CSTSConnection::CSTSConnection");
    createServerToNewThread();
    iState = EClosed;
    iRespHandler = aRespHandler;
    iMidletInfo = aMidletInfo;

    LOG(ESATSA, EInfo,  "-- Returning from CSTSConnection::CSTSConnection ");
}

// -----------------------------------------------------------------------------
// CSTSConnection::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSConnection::ConstructL()
{
    LOG(ESATSA, EInfo,  "+ CSTSConnection::ConstructL ");

    iCOFacade = CSTSCardOperationsFacade::NewL();
    iApduExchanger = CSTSApduExchanger::NewL(mJniEnv, mJavaPeerObject,
                     iCOFacade);
    iAccessControl = CSTSAccessControl::NewL(iMidletInfo);
    iPinManager = CSTSPinManager::NewL(mJniEnv, mJavaPeerObject,
                                       iApduExchanger, iAccessControl, iRespHandler);
    iSlotManager = CSTSSlotManager::NewL(iCOFacade);
    iChannelManager = CSTSChannelManager::NewL(iApduExchanger);
    iATR = HBufC8::NewL(KATRMaxLength);
    iBTSapListener = CSTSBTSapListener::NewL(this);
    iCardStateListener = CSTSCardStateListener::NewL(this);

    LOG(ESATSA, EInfo,  "-- CSTSConnection::ConstructL ");
}

// -----------------------------------------------------------------------------
// CSTSConnection::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSConnection* CSTSConnection::NewL(JNIEnv& aJni, jobject aPeer,
                                     MSTSRespHandler* aRespHandler, CSTSMidletInfo* aMidletInfo)
{
    LOG(ESATSA, EInfo,  "+ CSTSConnection::NewL ");
    CSTSConnection* self =
        new(ELeave) CSTSConnection(aRespHandler, aMidletInfo);

    CleanupStack::PushL(self);
    self->attachToVm(aJni, aPeer);
    CallMethodL(self, &CSTSConnection::ConstructL, self);
    CleanupStack::Pop(self);
    LOG(ESATSA, EInfo,  "-- CSTSConnection::NewL ");
    return self;
}

void CSTSConnection::vmAttached()
{


}

// Destructor
CSTSConnection::~CSTSConnection()
{
    LOG(ESATSA, EInfo,  "+ CSTSConnection::~CSTSConnection ");
    // for safety, close is called here
    Close();

    delete iCardStateListener;
    delete iBTSapListener;
    delete iATR;
    delete iChannelManager;
    delete iSlotManager;
    delete iPinManager;
    delete iAccessControl;
    delete iApduExchanger;
    delete iCOFacade;

    delete iCardAppManager;
    delete iRespHandler;
    delete iMidletInfo;
    delete iCmdApdu;
    delete iURI;

}

void CSTSConnection::WrapOpenL(CSTSConnection* connection)
{

    CallMethodL(connection, &CSTSConnection::OpenL, connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::OpenL
// Checks does URI exist, checks is it AID or SAT connection, in AID
// case checks Access Control related issues, opens connection to gived card
// application or opens SAT connection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::OpenL()
{
    LOG(ESATSA, EInfo,  "+ CSTSConnection::OpenL");

    LOG(ESATSA, EInfo,  "CSTSConnection::OpenL: Checking if BT SAP is ON");
    //if BT Sap is in use, connection cannot be established
    if (iBTSapListener->IsInUse())
    {
        ELOG(ESATSA, "CSTSConnection::OpenL:BT SAP is ON");
        User::Leave(KSTSErrConnectionNotFound + KSTSErrCNTBtSapInUse);

    }
    LOG(ESATSA, EInfo,  "CSTSConnection::OpenL: Checking if Card is available");
    //if card is not available, connection cannot be established
    if (!iCardStateListener->CardIsAvailable())
    {
        ELOG(ESATSA, "CSTSConnection::OpenL: Card is NOT available");
        User::Leave(KSTSErrConnectionNotFound + KSTSErrCNTNoCard);
    }

    //start listening BTSap events
    if (!iBTSapListener->IsActive())
    {
        LOG(ESATSA, EInfo,  "CSTSConnection::OpenL: start bt sap listener");
        iBTSapListener->Start();
    }
    //is slot one of existing slots
    if (!iSlotManager->DoesExistL(iURI->Slot()))
    {
        ELOG(ESATSA, "CSTSConnection::OpenL: Slot does not exist!! ");
        //ConnectionNotFoundException
        User::Leave(KSTSErrConnectionNotFound + KSTSErrCNTNoSlot);
    }

    iApduExchanger->SetReader(iURI->Slot());
    iURITarget = iURI->Type();
    iApduExchanger->SetConnectionType(iURITarget == CSTSURI::ESAT);

    // if SAT we try to check if card GSM or UICC card
    if (iURITarget == CSTSURI::ESAT)
    {
        LOG(ESATSA, EInfo,  "CSTSConnection::OpenL: connection type is SAT");
        // check is phone running in GSM mode
        if (iCOFacade->IsGSMNetwork())
        {
            iNetwork = ESTSTypeGSM;
        }
        else
        {
            iNetwork = ESTSTypeUMTS;
        }
    }

    // if AID then parse ACF and check domain.
    if (iURITarget == CSTSURI::EAID)
    {
        LOG(ESATSA, EInfo,  "CSTSConnection::OpenL: connection type is AID");
        CSTSCardAppManager* tmpAppManager = CSTSCardAppManager::NewL(
                                                iApduExchanger, iURI->AID());
        delete iCardAppManager;
        iCardAppManager = tmpAppManager;
        // try to select card application to determine does it exist or not
        LOG(ESATSA, EInfo,  "CSTSConnection::OpenL: going to select card application");
        DoSelectApplicationL();
        // close channel(closes also application) so that access control can
        // use it for reading access control files from card
        LOG(ESATSA, EInfo,  "CSTSConnection::OpenL: closing channel");
        iChannelManager->CloseChannel(iChannel);
        LOG(ESATSA, EInfo, "CSTSConnection::OpenL: setting basic channel");
        iApduExchanger->SetChannel(0); //basic channel
        LOG(ESATSA, EInfo,  "CSTSConnection::OpenL: going to set AID");
        iAccessControl->SetAIDL(iURI->AID());
        LOG(ESATSA, EInfo, "CSTSConnection::OpenL: going to read files");
        iAccessControl->ReadFilesL();
        if (!iAccessControl->IsAllowedDomain())
        {
            ELOG(ESATSA, "CSTSConnection::OpenL: Domain is not allowed");
            // SecurityException
            User::Leave(KSTSErrSecurity + KSTSErrSECAccessNotAllowed);
        }

        // if access control rights were ok we can select card application again
        // and leave it open
        LOG(ESATSA, EInfo, "CSTSConnection::OpenL: Select App if access is allowed");
        DoSelectApplicationL();
    }

    // if all was ok
    iState = EOpened;
    LOG(ESATSA, EInfo, "-- CSTSConnection::OpenL");
}

void CSTSConnection::WrapCloseL(CSTSConnection* connection)
{
    CallMethodL(connection, &CSTSConnection::CloseL, connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::CloseL
// Cancels any pending operation, deselects card application if needed and
// closes opened channel if needed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::CloseL()
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::CloseL");
    iPinManager->CancelOperation();
    iApduExchanger->CancelExchange();

    if (iURITarget == CSTSURI::EAID)
    {
        if (iState == EOpened)
        {
            TInt appErr = iCardAppManager->DeselectApplication();
            TInt channelErr = iChannelManager->CloseChannel(iChannel);

            if (channelErr == KErrNone)
            {
                iState = EClosed;
            }
            // if application deselection or channel closing fails
            // we can throw IOException in java side
            if (appErr != KErrNone || channelErr != KErrNone)
            {
                User::Leave(KSTSErrIO + KSTSErrIOErrorIO);
            }
        }
        else
        {
            // for safety, close access control also
            iAccessControl->Close();
            iApduExchanger->SetChannel(KSTSBasicChannel);
        }
    }
    iState = EClosed;
    LOG(ESATSA, EInfo, "-- CSTSConnection::CloseL");
}

// -----------------------------------------------------------------------------
// CSTSConnection::Close
// WILL NOT LEAVE. For destructor use. Cancels any pending operation, deselects
// card application if needed and closes opened channel if needed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CSTSConnection::Close()
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::Close");
    // Close is called in desctructor, so have to check in case
    // that contructor has leaved
    if (iPinManager)
    {
        iPinManager->CancelOperation();
    }
    if (iApduExchanger)
    {
        iApduExchanger->CancelExchange();
    }
    if (iURITarget == CSTSURI::EAID && iState == EOpened)
    {
        if (iCardAppManager)
        {
            iCardAppManager->DeselectApplication();
        }
        if (iChannelManager)
        {
            iChannelManager->CloseChannel(iChannel);
        }
    }
    iState = EClosed;
    LOG(ESATSA, EInfo, "+ CSTSConnection::Close");
}

void CSTSConnection::WrapChangePinL(CSTSConnection* connection, TInt aPinID)
{
    CallMethodL(connection, &CSTSConnection::ChangePinL, aPinID, connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::ChangePinL
// Delivers change pin call to pinmanager
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::ChangePinL(TInt aPinID)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::ChangePinL");
    CheckStateL(ETrue); //In SAT connection PIN method is not allowed
    iPinManager->ChangePinL(aPinID);
    LOG(ESATSA, EInfo, "-- CSTSConnection::ChangePinL");
}

void CSTSConnection::WrapDisablePinL(CSTSConnection* connection, TInt aPinID)
{
    CallMethodL(connection, &CSTSConnection::DisablePinL, aPinID, connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::DisablePinL
// Delivers DisablePin call to pinmanager
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::DisablePinL(TInt aPinID)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::DisablePinL");
    CheckStateL(ETrue); //In SAT connection PIN method is not allowed
    iPinManager->DisablePinL(aPinID);
    LOG(ESATSA, EInfo, "-- CSTSConnection::DisablePinL");
}

void CSTSConnection::WrapEnablePinL(CSTSConnection* connection, TInt aPinID)
{
    CallMethodL(connection, &CSTSConnection::EnablePinL, aPinID, connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::EnablePinL
// Delivers EnablePin call to pinmanager
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::EnablePinL(TInt aPinID)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::EnablePinL");
    CheckStateL(ETrue); //In SAT connection PIN method is not allowed
    iPinManager->EnablePinL(aPinID);
    LOG(ESATSA, EInfo, "-- CSTSConnection::EnablePinL");
}

void CSTSConnection::WrapEnterPinL(CSTSConnection* connection, TInt aPinID)
{
    CallMethodL(connection, &CSTSConnection::EnterPinL, aPinID, connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::EnterPinL
// Delivers EnterPin call to pinmanager
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::EnterPinL(TInt aPinID)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::EnterPinL");
    CheckStateL(ETrue); //In SAT connection PIN method is not allowed
    iPinManager->EnterPinL(aPinID);
    LOG(ESATSA, EInfo, "-- CSTSConnection::EnterPinL");
}

void CSTSConnection::WrapUnblockPinL(CSTSConnection* connection,
                                     TInt aBlockedPinID, TInt aUnblockingPinID)
{
    CallMethodL(connection, &CSTSConnection::UnblockPinL, aBlockedPinID,
                aUnblockingPinID, connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::UnblockPinL
// Delivers UnblockPin call to pinmanager
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::UnblockPinL(TInt aBlockedPinID, TInt aUnblockingPinID)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::UnblockPinL");
    CheckStateL(ETrue); //In SAT connection PIN method is not allowed
    iPinManager->UnblockPinL(aBlockedPinID, aUnblockingPinID);
    LOG(ESATSA, EInfo, "-- CSTSConnection::UnblockPinL");
}

void CSTSConnection::WrapExchangeAPDUL(CSTSConnection* connection,
                                       TPtrC8 aCommandDes)
{
    CallMethodL(connection, &CSTSConnection::ExchangeAPDUL, aCommandDes,
                connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::ExchangeAPDUL
// Checks is apdu allowed and sends it to smart card.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::ExchangeAPDUL(TPtrC8 aCommandAPDU)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::ExchangeAPDUL");
    CheckStateL();

    LOG(ESATSA, EInfo, "CSTSConnection::ExchangeAPDUL:After the conversion");
    if (iURITarget == CSTSURI::EAID)
    {
        LOG(ESATSA, EInfo, "CSTSConnection::ExchangeAPDUL: Type is AID");
        CSTSCmdApdu* cmdApdu = NULL;
        cmdApdu = CSTSCmdApdu::NewL(aCommandAPDU, CSTSApdu::ESTSUICC);
        if (!cmdApdu)
        {
            ELOG(ESATSA, "CSTSConnection::ExchangeAPDUL: failed to create CSTSCmdApdu object!!");
        }
        delete iCmdApdu;
        //must be saved to member variable for later use in asynchronous call
        iCmdApdu = cmdApdu;
        LOG(ESATSA, EInfo, "CSTSConnection::ExchangeAPDUL: check the channel");
        iCmdApdu->CheckL(iChannel);
    }
    else
    {
        CSTSApdu::TSTSApduStandard standard = CSTSApdu::ESTSUICC;
        if (iNetwork == ESTSTypeGSM)
        {
            standard = CSTSApdu::ESTSGSM;
        }
        CSTSEnvelope* envelopeApdu = CSTSEnvelope::NewL(aCommandAPDU, standard);
        CleanupStack::PushL(envelopeApdu);
        envelopeApdu->CheckL();
        envelopeApdu->SetClassByte();
        delete iCmdApdu;
        //must be saved to member variable for later use in asynchronous call
        iCmdApdu = envelopeApdu;
        CleanupStack::Pop(envelopeApdu);
    }

    //only check in AID case
    if (iURITarget == CSTSURI::EAID)
    {
        LOG(ESATSA, EInfo, "CSTSConnection::ExchangeAPDUL:only check in AID case");
        if (!iAccessControl->IsAllowedApduL(iCmdApdu->Header()))
        {
            //SecurityException
            User::Leave(KSTSErrSecurity + KSTSErrSECIllegalApdu);
        }
    }
    LOG(ESATSA, EInfo, "CSTSConnection::ExchangeAPDUL: Going to call ExchangeApduAsync ");
    iApduExchanger->ExchangeApduAsync(*iCmdApdu, iRespHandler);
    LOG(ESATSA, EInfo, "-- CSTSConnection::ExchangeAPDUL");
}

std::wstring CSTSConnection::WrapGetATRL(CSTSConnection* connection)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::WrapGetATRL");
    std::wstring atrData;
    CallMethodL(atrData, connection, &CSTSConnection::GetATRL, connection);
    LOG(ESATSA, EInfo, "-- CSTSConnection::WrapGetATRL");
    return atrData;
}
// -----------------------------------------------------------------------------
// CSTSConnection::GetATRL
// Delivers GetATRL call to card operations facade
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
std::wstring CSTSConnection::GetATRL()
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::GetATRL");
    CheckStateL();

    TPtr8 atrData(iATR->Des());
    //aATR->Set( iATR->Des() );
    iCOFacade->GetATR(atrData, iURI->Slot());

    LOG(ESATSA, EInfo, "CSTSConnection::GetATRL: After iCOFacade->GetATR call ");

    HBufC16* tmp = HBufC16::NewL(KATRMaxLength);
    TPtr16 aTr(tmp->Des());// = NULL; // = (TPtr16*)(&atrData);//( NULL, 0 );
    aTr.Copy(atrData);

    LOG(ESATSA, EInfo, "CSTSConnection::GetATRL: convert to wstring");
    std::wstring atrResp = desToWstring(aTr);
    delete tmp;
    return atrResp;
}

std::wstring CSTSConnection::WrapGetSlotsL(CSTSConnection* connection)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::WrapGetSlotsL");
    std::wstring slotStr;
    CallMethodL(slotStr, connection, &CSTSConnection::GetSlotsL, connection);
    LOG(ESATSA, EInfo, "-- return CSTSConnection::WrapGetSlotsL");
    return slotStr;

}
// -----------------------------------------------------------------------------
// CSTSConnection::GetSlotsL
// Delivers GetSlots call to slot manager
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
std::wstring CSTSConnection::GetSlotsL()
{

    LOG(ESATSA, EInfo, "+ CSTSConnection::GetSlotsL");
    //TPtr* respAPDU = reinterpret_cast<TPtr*>( aResponseAPDU );
    std::wstring slotString = iSlotManager->GetSlotsL();
    LOG(ESATSA, EInfo, "-- CSTSConnection::GetSlotsL");
    return slotString;

}

void CSTSConnection::WrapSetURI(CSTSConnection* connection, CSTSURI* aURI)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::WrapSetURI");
    TInt connectionHandle = reinterpret_cast<TInt>(aURI);
    CallMethodL(connection, &CSTSConnection::SetURI, connectionHandle,
                connection);
}
// -----------------------------------------------------------------------------
// CSTSConnection::SetURIL
// Sets URI member variable. Ownership transferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::SetURI(TInt /* CSTSURI& */aURI)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::SetURI");
    CSTSURI* aUri = reinterpret_cast<CSTSURI *>(aURI);
    delete iURI;
    iURI = aUri;
    LOG(ESATSA, EInfo, "--CSTSConnection::SetURI");
}

// -----------------------------------------------------------------------------
// CSTSConnection::CheckStateL
// Leaves with correct leave code if state is not corret one
// -----------------------------------------------------------------------------
//
void CSTSConnection::CheckStateL(TBool aSATNotAllowed)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::CheckStateL");
    if (iURITarget == CSTSURI::ESAT && aSATNotAllowed)
    {
        ELOG(ESATSA, "+ CSTSConnection::CheckStateL:SAT not allowed");
        User::Leave(KSTSErrSecurity);
    }

    //connection was closed before
    if (iState == EClosed)
    {
        User::Leave(KSTSErrIO + KSTSErrIOConnClosedBefore);
    }
    //card was removed
    else if (iState == ECardRemoved)
    {
        User::Leave(KSTSErrInterruptedIO + KSTSErrIICardRemoved);
    }
    //BT SAP was taken in use
    else if (iState == EBTSapInUse)
    {
        User::Leave(KSTSErrInterruptedIO + KSTSErrIIBTSapInUse);
    }
    LOG(ESATSA, EInfo, "-- CSTSConnection::CheckStateL");
}

// -----------------------------------------------------------------------------
// CSTSConnection::NotifyClose
// Resets apdumanager and pinmanager and saves state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::NotifyClose(TInt aReason)
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::NotifyClose");
    iPinManager->CancelOperation();
    iApduExchanger->CancelExchange();

    //card was removed
    if (aReason == MSTSCloseObserver::ESTSCardNotAvailable)
    {
        iState = ECardRemoved;
    }
    //BT SAP was taken in use
    else
    {
        iState = EBTSapInUse;
    }
}

// -----------------------------------------------------------------------------
// CSTSConnection::DoSelectApplicationL
// Opens channel and selects card application. If application selection fails
// closes the channel and leaves with proper error code.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSConnection::DoSelectApplicationL()
{
    LOG(ESATSA, EInfo, "+ CSTSConnection::DoSelectApplicationL");
    //in AID case we can try to open own channel
    LOG(ESATSA, EInfo, "CSTSConnection::DoSelectApplicationL: open own channel");
    iApduExchanger->SetChannel(0); //basic channel
    LOG(ESATSA, EInfo, "CSTSConnection::DoSelectApplicationL: request channel");
    iChannel = iChannelManager->RequestChannel();

    // if there is no logical channel available, throw IOException
    if (iChannel < 0)
    {
        ELOG(ESATSA, "CSTSConnection::DoSelectApplicationL:no logical channel available, throw IOException ");
        User::Leave(KSTSErrIO + KSTSErrIONoFreeChannels);
    }

    iApduExchanger->SetChannel(iChannel);
    LOG(ESATSA, EInfo, "CSTSConnection::DoSelectApplicationL: going to select app ");
    TInt err = iCardAppManager->SelectApplication();

    // if selection was not ok Close opened channel
    if (err != KErrNone)
    {
        ELOG(ESATSA, "CSTSConnection::DoSelectApplicationL: app selection was not ok Close opened channel");
        // close channel error code not used because we will leave anyway
        iChannelManager->CloseChannel(iChannel);
        if (err == KErrNoMemory)
        {
            User::Leave(KErrNoMemory);
        }
        else
        {
            User::Leave(KSTSErrConnectionNotFound + KSTSErrCNTNoApplication);
        }
    }
    LOG(ESATSA, EInfo, "-- CSTSConnection::DoSelectApplicationL");
}

void CSTSConnection::doServerSideInit()
{

    FunctionServer::doServerSideInit();

}

} // namespace satsa
} // namespace java
//  End of File
