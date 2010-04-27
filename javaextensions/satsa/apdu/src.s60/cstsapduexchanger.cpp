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
#include "stsapduconstants.h"
#include "cstscmdapdu.h"
#include "cstsrespapdu.h"
#include "cstscardoperationsfacade.h"
#include "mstsresphandler.h"
#include "cstsgetresponse.h"
#include "fs_methodcall.h"
#include "logger.h"

namespace java
{
namespace satsa
{

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSApduExchanger::CSTSApduExchanger
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CSTSApduExchanger::CSTSApduExchanger(CSTSCardOperationsFacade* aCOFacade) :
        CActive(EPriorityNormal)
{
    CActiveScheduler::Add(this);
    iCOFacade = aCOFacade;
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::ConstructL()
{
    iWait = new(ELeave) CActiveSchedulerWait();
    iRespBuf = HBufC8::NewL(KSTSResponseApduMaxLength);
    iRespApdu = CSTSRespApdu::NewL(KSTSResponseApduMaxLength);
    iGetResponseApdu = CSTSGetResponse::NewL(0x00, //empty at this point
                       CSTSApdu::ESTSUICC);
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSApduExchanger* CSTSApduExchanger::NewL(JNIEnv* aJni, jobject aPeer,
        CSTSCardOperationsFacade* aCOFacade)
{
    CSTSApduExchanger* self = new(ELeave) CSTSApduExchanger(aCOFacade);
    self->mJni = aJni;
    self->mPeer = aPeer;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSApduExchanger::~CSTSApduExchanger()
{
    Cancel();
    delete iRespBuf;
    delete iWait;
    delete iRespApdu;
    delete iGetResponseApdu;
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::DoExchange
// Set channel, apdu and empties response buffer. Exchanges apdu message
// asynchronously
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::DoExchange(CSTSCmdApdu& aCmdApdu)
{
    LOG(ESATSA, EInfo, "+ CSTSApduExchanger::DoExchange");
    aCmdApdu.SetChannel(iChannel);

    iCmdApdu = &aCmdApdu;
    //response buffer is "reusable" so empty length before next use
    iRespBuf->Des().SetLength(0);
    //send apdu to card
    LOG(ESATSA, EInfo, "CSTSApduExchanger::DoExchange: send apdu to card");
    iCOFacade->SendAPDUReq(iStatus, (TUint8) iReader, &aCmdApdu.ApduBytes());
    SetActive();
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::ExchangeApduL
// Exchanges apdu message synchronously
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSTSApduExchanger::ExchangeApdu(CSTSCmdApdu& aCmdApdu,
                                     CSTSRespApdu& aRespApdu)
{
    DoExchange(aCmdApdu);
    if (!iWait->IsStarted())
    {
        iWait->Start();
    }
    //save response
    aRespApdu.ResponseBytes() = iRespBuf->Des();
    return iStatus.Int();

}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::ExchangeApdu
// Exchanges apdu message asynchronously
// Sets iClientStatus information for RunL purposes.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::ExchangeApduAsync(CSTSCmdApdu& aCmdApdu,
        MSTSRespHandler* aRespHandler)
{
    LOG(ESATSA, EInfo, "+ CSTSApduExchanger::ExchangeApduAsync 1");

    iRespHandler = aRespHandler;
    LOG(ESATSA, EInfo, "Calling DoExchnage()");
    DoExchange(aCmdApdu);
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::ExchangeApdu
// Exchanges apdu message asynchronously
// Response must be get separately after asynchronous call is returned
// When using this method for sending apdu and GetResponseBytes method for
// getting response bytes there is no checked is resend needed or get response
// needed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::ExchangeApduAsync(TRequestStatus& aStatus,
        CSTSCmdApdu& aCmdApdu)
{
    aCmdApdu.SetChannel(iChannel);
    // send apdu to card
    iCOFacade->SendAPDUReq(aStatus, (TUint8) iReader, &aCmdApdu.ApduBytes());
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::GetResponseBytes

// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::GetResponseBytes(CSTSRespApdu& aRespApdu)
{
    iCOFacade->GetResponseBytes(&aRespApdu.ResponseBytes());
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::SetChannel
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::SetChannel(TInt aChannel)
{
    LOG(ESATSA, EInfo, "+ CSTSApduExchanger::SetChannel");
    iChannel = aChannel;
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::SetReader
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::SetReader(TInt aReader)
{
    iReader = aReader;
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::SetConnectionType
// Sets connection target to response adpu
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::SetConnectionType(TBool aSatConnection)
{
    if (aSatConnection)
    {
        iRespApdu->SetConnectionTarget(CSTSRespApdu::ESTSUSAT);
    }
    else
    {
        iRespApdu->SetConnectionTarget(CSTSRespApdu::ESTSAID);
    }
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::CancelExchange
// Can be used, if J2ME application calls CLOSE method in another thread for
// closing connection and in other thread is pending apdu send at the same time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::CancelExchange()
{
    Cancel();
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::RunL
// If apdu send was called synchronously, CActiveSchedulerWait is used to continue
// in ExchangeApduL method. If apdu send was called synchronously, we notify
// and deliver response to java side
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::RunL()
{
    LOG(ESATSA, EInfo, "+ CSTSApduExchanger::RunL");
    if (iStatus == KErrNone)
    {
        LOG(ESATSA, EInfo, "CSTSApduExchanger::RunL:Status is KErrNone.");
        iRespApdu->ResponseBytes().SetLength(0);
        iCOFacade->GetResponseBytes(&iRespApdu->ResponseBytes());

        //append new response after old response
        iRespBuf->Des().Append(iRespApdu->ResponseBytes());

        TUint8 newRespLength = 0x00;
        if (iRespApdu->GetResponseNeeded(newRespLength))
        {
            LOG(ESATSA, EInfo, "CSTSApduExchanger::RunL:GetResponseNeeded");
            //remove sw1 and sw2, because more data will be appended
            iRespBuf->Des().SetLength(iRespBuf->Length() - 2);
            //we got new length from earlier called method
            iGetResponseApdu->SetExpectedResponseLength(newRespLength);
            iGetResponseApdu->SetChannel(iChannel);

            //send get response apdu to card
            LOG(ESATSA, EInfo, "send get response apdu to card");
            iCOFacade->SendAPDUReq(iStatus, (TUint8) iReader,
                                   &iGetResponseApdu->ApduBytes());
            SetActive();
        }
        else if (iRespApdu->ResendNeeded(newRespLength))
        {
            LOG(ESATSA, EInfo, "CSTSApduExchanger::RunL: Resend needed ");
            //Resend apdu with correct length
            iCmdApdu->SetLe(newRespLength);
            iRespBuf->Des().SetLength(0); //empty old response

            LOG(ESATSA, EInfo, "send same apdu to card again (resend)");
            //send same apdu to card again (resend)
            iCOFacade->SendAPDUReq(iStatus, (TUint8) iReader,
                                   &iCmdApdu->ApduBytes());
            SetActive();
        }
        else
        {
            //if synchronous ExchangeApdu was called
            if (iWait->IsStarted())
            {
                LOG(ESATSA, EInfo, "Synch Exchng called!");
                iWait->AsyncStop();
            }
            //if asynchronous ExchangeApduAsync was called
            else
            {
                LOG(ESATSA, EInfo, "calling response handler to complete operation");
                iRespHandler->OperationComplete(mJni, mPeer, iRespBuf, KErrNone);
            }
        }
    }
    else //if the iStatus is not KErrNone
    {
        LOG(ESATSA, EInfo, "CSTSApduExchanger::RunL:iStatus is not KErrNone");
        //if synchronous ExchangeApdu was called
        if (iWait->IsStarted())
        {
            iWait->AsyncStop();
        }
        else
        {
            LOG(ESATSA, EInfo, "calling response handler to complete operation");
            iRespHandler->OperationComplete(mJni, mPeer, NULL, KSTSErrIO);
        }
    }
}

// -----------------------------------------------------------------------------
// CSTSApduExchanger::DoCancel
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduExchanger::DoCancel()
{

    if (iWait->IsStarted())
    {
        //synchronous call can not be cancelled

    }
    else
    {
        iCOFacade->CancelAPDUReq();
        iRespHandler->OperationComplete(mJni, mPeer, NULL, KSTSErrInterruptedIO
                                        + KSTSErrIIConnClosedDuring);
    }
}

} // namespace satsa
} // namespace java
//  End of File

