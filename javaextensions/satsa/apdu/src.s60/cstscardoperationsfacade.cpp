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
#include "rmmcustomapi.h"
#include "cstscardoperationsfacade.h"
#include <mmtsy_names.h>// TSY and Phone name
#include "logger.h"

namespace java
{
namespace satsa
{

//  CONSTANTS
//TApdu info and data
const TUint8 KSTSTApduInfoLength = 16;

//service types, used for RMmCustomAPI's TApdu info part
const TUint8 KSTSSendApdu = 79;
const TUint8 KSTSGetATR = 80;

//application type, used for RMmCustomAPI's TApdu info part
const TUint8 KSTSAPDUAppTypeJavaApp = 3;

//Empty
const TUint8 KSTSEmpty = 0x00;

//header + lc + data +le
const TInt KSTSApduDataMaxLength = 4 + 1 + 0xFF + 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::CSTSCardOperationsFacade
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSCardOperationsFacade::CSTSCardOperationsFacade() :
        iCustomAPI(), iInfoPtr(NULL, 0), iDataPtr(NULL, 0), iGSMNetwork(EFalse)
{

}

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSCardOperationsFacade::ConstructL()
{
    User::LeaveIfError(iSession.Connect());
    //Load in the phone device driver
    User::LeaveIfError(iSession.LoadPhoneModule(KMmTsyModuleName));

    User::LeaveIfError(iPhone.Open(iSession, KMmTsyPhoneName));

    User::LeaveIfError(iCustomAPI.Open(iPhone));

    //find out which network mode phone is using
    RMobilePhone::TMobilePhoneNetworkMode networkMode;
    TInt err = iPhone.GetCurrentMode(networkMode);

    if (err == KErrNone && networkMode == RMobilePhone::ENetworkModeGsm)
    {
        iGSMNetwork = ETrue;
    }

    iPhone.Close();

    iInfoBuf = HBufC8::NewL(KSTSTApduInfoLength);
    iDataBuf = HBufC8::NewL(KSTSApduDataMaxLength);

    iApduReq = new(ELeave) RMmCustomAPI::TApdu();
}

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSCardOperationsFacade* CSTSCardOperationsFacade::NewL()
{
    CSTSCardOperationsFacade* self = new(ELeave) CSTSCardOperationsFacade;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSCardOperationsFacade::~CSTSCardOperationsFacade()
{
    delete iApduReq;
    delete iDataBuf;
    delete iInfoBuf;

    iCustomAPI.Close();
    iSession.Close();

    //is constructor has leaved, this is not closed so do it again
    iPhone.Close();
}

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::SendAPDUReq
//
// -----------------------------------------------------------------------------
//
void CSTSCardOperationsFacade::SendAPDUReq(TRequestStatus& aStatus,
        TUint8 aCardReaderId, TDes8* aCmdData)
{
    LOG(ESATSA, EInfo, "CSTSCardOperationsFacade::SendAPDUReq() called");
    DoSend(aStatus, KSTSSendApdu, aCardReaderId, aCmdData);
}

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::GetResponseBytes
//
// -----------------------------------------------------------------------------
//
void CSTSCardOperationsFacade::GetResponseBytes(TDes8* aRspData)
{
    *aRspData = *iApduReq->iData;
}

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::CancelAPDUReq
//
// -----------------------------------------------------------------------------
//
void CSTSCardOperationsFacade::CancelAPDUReq()
{
    iCustomAPI.CancelAsyncRequest(ECustomSendAPDUReqIPC);
}

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::GetATRL
//
// -----------------------------------------------------------------------------
//
void CSTSCardOperationsFacade::GetATR(TDes8& aATR, TUint8 aReader)
{
    LOG(ESATSA, EInfo, "CSTSCardOperationsFacade::GetATRL called!");
    TRequestStatus status;
    DoSend(status, KSTSGetATR, aReader, &aATR);
    User::WaitForRequest(status);
    LOG1(ESATSA, EInfo, "status: %d", status.Int());
    if (status.Int() != KErrNone)
    {
        //null will be returned, if some error occured
        aATR = KNullDesC8();
    }
    else
    {
        LOG(ESATSA, EInfo, "Copy the response");
        //copy ATR data
        aATR = *iApduReq->iData;
        LOG(ESATSA, EInfo, "After copying the response");

    }

}

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::DoSend
//
// -----------------------------------------------------------------------------
//
void CSTSCardOperationsFacade::DoSend(TRequestStatus& aStatus,
                                      TUint8 aServiceType, TUint8 aReader, TDes8* aCmdData)
{
    LOG(ESATSA, EInfo, "CSTSCardOperationsFacade::DoSend() called");

    _LIT(KSTSBufferTooSmall,
         "CSTSCardOperationsFacade::DoSend: iInfoBuf too small");
    __ASSERT_ALWAYS((iInfoBuf->Des().MaxLength() >= KSTSTApduInfoLength),
                    User::Panic(KSTSBufferTooSmall, KErrArgument));

    LOG(ESATSA, EInfo, "Check Comand Data");

    _LIT(KSTSEmptyCommand, "CSTSCardOperationsFacade::DoSend: aCmdData is NULL");
    __ASSERT_ALWAYS((aCmdData), User::Panic(KSTSEmptyCommand, KErrArgument));

    LOG(ESATSA, EInfo, "Set Info Part");
    //set info part
    iInfoBuf->Des().SetLength(0);
    iInfoBuf->Des().Append(aServiceType);
    iInfoBuf->Des().Append(aReader);
    iInfoBuf->Des().Append(KSTSAPDUAppTypeJavaApp);
    iInfoBuf->Des().Append(KSTSEmpty); //Padding byte, always zero

    iInfoPtr.Set(iInfoBuf->Des());
    iApduReq->iInfo = &iInfoPtr;

    LOG(ESATSA, EInfo, "Set Data Part");
    //set data part
    iDataBuf->Des().SetLength(0);
    iDataBuf->Des().Append(*aCmdData);

    iDataPtr.Set(iDataBuf->Des());
    iApduReq->iData = &iDataPtr;
    LOG(ESATSA, EInfo, "Calling iCustomAPI.SendAPDUReq");
    iCustomAPI.SendAPDUReq(aStatus, *iApduReq);
    LOG(ESATSA, EInfo, "--- CSTSCardOperationsFacade::DoSend() ");
}

// -----------------------------------------------------------------------------
// CSTSCardOperationsFacade::IsGSMNetwork
//
// -----------------------------------------------------------------------------
//
TBool CSTSCardOperationsFacade::IsGSMNetwork() const
{
    return iGSMNetwork;
}

} // namespace satsa
} // namespace java
//  End of File

