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


#include "logger.h"
#include "functionserver.h"
#include "bluetoothfunctionserver.h"
#include "bluetoothremotedevice.h"
#include "fs_methodcall.h"
#include "bluetoothclientconnection.h"
#include "btrfcommserverconnection.h"
#include "bluetoothpusheventlistener.h"
#include "javasymbianoslayer.h"

using namespace std;
using namespace java::util;

namespace java
{
namespace bluetooth
{

const TInt KSizeOfListenQueue = 7;
#define SERVICE_AVAILABLE 0xFF
#define SERVICE_NOTAVAILABLE 0x00
_LIT(KRfComm, "RFCOMM");

OS_EXPORT RFCOMMServerConnection::RFCOMMServerConnection(
    java::bluetooth::BluetoothFunctionServer* server):
        mAcceptMonitor(NULL),
        mAsyncAccept(false),
        mBtUrlParams(NULL),
        mBtClientConn(NULL),
        mServer(server),
        mIsConnected(EFalse),
        mServRec(NULL),
        mState(ENone)
{
    JELOG2(EJavaBluetooth);
}

OS_EXPORT RFCOMMServerConnection::~RFCOMMServerConnection()
{
    JELOG2(EJavaBluetooth);
    CloseServer();
    delete mAcceptMonitor;
}

OS_EXPORT int RFCOMMServerConnection::openServer(bool authorize,
        bool authenticate, bool encrypt)
{
    JELOG2(EJavaBluetooth);
    return ServerOpen(authorize, authenticate, encrypt);
}

OS_EXPORT int RFCOMMServerConnection::ServerOpen(TBool authorize,
        TBool authenticate, TBool encrypt)
{
    JELOG2(EJavaBluetooth);
    int result = 0;
    TRAP(result, CallMethodL(this, &RFCOMMServerConnection::ServerOpenL,
                             authorize, authenticate, encrypt, mServer));
    return result;
}

OS_EXPORT ServiceRecord *RFCOMMServerConnection::getServiceRecordHandle()
{
    return mServRec;
}

void RFCOMMServerConnection::ServerOpenL(TBool authorize, TBool authenticate,
        TBool encrypt)
{
    JELOG2(EJavaBluetooth);

    if (mState != ENone)
    {
        User::Leave(KErrInUse);
    }

    User::LeaveIfError(mSocketServ.Connect());
    User::LeaveIfError(mSocketServ.ShareAuto());

    TRAPD(err,
    {
        // Set this active object to connecting state
        mState = EConnecting;

        // Set a Bluetooth socket address
        BindL(authorize, authenticate, encrypt);

        // Set a socket to listen for incoming connections
        ListenL();
    }
         );

    if (KErrNone != err)
    {
        ELOG(EJavaBluetooth,
             "- RFCOMMServerConnection::ServerOpenL: Exception caught ");
        mState = ENone;
        mSocketServ.Close();
        User::LeaveIfError(err);
    }

}

void RFCOMMServerConnection::BindL(TBool authorize, TBool authenticate,
                                   TBool encrypt)
{
    JELOG2(EJavaBluetooth);
    // Load protocol, RFCOMM
    TProtocolDesc protocolDesc;
    User::LeaveIfError(mSocketServ.FindProtocol(KRfComm(), protocolDesc));

    LOG(EJavaBluetooth, EInfo,
        "  RFCOMMServerConnection::Bind: Opening A Listener Socket ");

    // Open a socket
    mListenSock = CBluetoothSocket::NewL(*this, mSocketServ,
                                         protocolDesc.iSockType, KRFCOMM);

    mListenSock->SetNotifier(*this);

    TInt Channel = 0;
    // Get listening channel
    User::LeaveIfError(mListenSock->GetOpt(KRFCOMMGetAvailableServerChannel,
                                           KSolBtRFCOMM, Channel));

    // Set security
    TBTServiceSecurity secSettings;

    secSettings.SetAuthentication(authenticate);
    secSettings.SetAuthorisation(authorize);
    secSettings.SetEncryption(encrypt);

    LOG(EJavaBluetooth, EInfo,
        "  RFCOMMServerConnection::Accept: setting The Security Settings ");
    // Attach the security settings.
    mBtSockAddr.SetSecurity(secSettings);

    LOG1(EJavaBluetooth, EInfo,
         "  RFCOMMServerConnection::Bind: setting port to: %d", Channel);
    // Setting Rfcom Port
    mBtSockAddr.SetPort(Channel);

    LOG(EJavaBluetooth, EInfo,
        "  RFCOMMServerConnection::Bind: Binding Bluetooth Socket ");
    // Bind bluetooth socket
    User::LeaveIfError(mListenSock->Bind(mBtSockAddr));
}

OS_EXPORT int RFCOMMServerConnection::GetRfListeningChannel()
{
    JELOG2(EJavaBluetooth);
    int result = 0;
    TRAPD(err, CallMethodL(result, this,
                           &RFCOMMServerConnection::GetRfListeningChannelFs, mServer));

    if (err != KErrNone)
    {
        return err;
    }

    return result;
}

int RFCOMMServerConnection::GetRfListeningChannelFs()
{
    JELOG2(EJavaBluetooth);
    TInt channel = 0;
    if (mState != EConnecting)
    {
        return -1;
    }

    channel = mListenSock->LocalPort();
    LOG1(EJavaBluetooth, EInfo,
         "  RFCOMMServerConnection::GetRfListeningChannelFs: %d", channel);
    return channel;

}

void RFCOMMServerConnection::ListenL()
{
    JELOG2(EJavaBluetooth);
    // Listen on port
    User::LeaveIfError(mListenSock->Listen(KSizeOfListenQueue));
}

OS_EXPORT int RFCOMMServerConnection::asyncAccept(
    java::bluetooth::BluetoothPushEventListener* aEventListener,
    BtUrlParams *aBtUrlParams)
{
    JELOG2(EJavaBluetooth);
    //Store event listener and notify it when client is accepted.
    //Make a call to accept and do not wait.
    mAsyncAccept = true;
    mPushEventListener = aEventListener;
    mBtUrlParams = aBtUrlParams;
    int result = 0;
    TRAPD(err, CallMethodL(result, this, &RFCOMMServerConnection::AcceptL,
                           mServer));
    return err;
}

OS_EXPORT long RFCOMMServerConnection::Accept()
{
    JELOG2(EJavaBluetooth);
    long result = 0;
    mBtClientConn = NULL;

    TRAPD(err, CallMethodL(result, this, &RFCOMMServerConnection::AcceptL,
                           mServer));

    if (err != KErrNone)
    {
        return err;
    }

    mAcceptMonitor->wait();

    if (mAcceptStatus != KErrNone)
    {
        return mAcceptStatus;
    }

    mAcceptedSocket = NULL;

    return reinterpret_cast<long>(mBtClientConn);
}

long RFCOMMServerConnection::AcceptL()
{
    JELOG2(EJavaBluetooth);

    // Open blank socket and pass it to accept to be assigned a proper
    // socket upon completion of Accept()
    mAcceptedSocket = CBluetoothSocket::NewL(*this, mSocketServ);
    LOG(EJavaBluetooth, EInfo,
        "  RFCOMMServerConnection::Accept: Set To Accept Incoming Connections ");

    if (NULL == mAcceptMonitor)
    {
        mAcceptMonitor = java::util::Monitor::createMonitor();
    }

    mAcceptStatus = 0;

    // Enabling the advertising flag
    if (NULL != mServRec)
    {
        mServRec -> setAdvertiseFs(ETrue);
    }
    int err = mListenSock->Accept((*mAcceptedSocket));
    LOG1(EJavaBluetooth,EInfo,
         "  RFCOMMServerConnection::Accept: accept return: %d", err);

    return err;
}

OS_EXPORT int RFCOMMServerConnection::initializeServiceRecord(
    TInt aChannel, TUUID &aServiceUUID, TDesC8 &aServiceName, bool aIsGOEP)
{
    JELOG2(EJavaBluetooth);
    int result = 0;

    if (NULL == mServRec)
    {
        TRAP(result, CallMethodL(mServRec, this,
                                 &RFCOMMServerConnection::createServiceRecordL, mServer));

        if (KErrNone == result)
        {
            TInt protocol = (true == aIsGOEP)?PROTOCOL_GOEP : PROTOCOL_RFCOMM;

            TRAP(result, mServRec->initializeRecordL(protocol,
                    aChannel, aServiceUUID, aServiceName));
        }
    }
    return result;
}

OS_EXPORT int RFCOMMServerConnection::initializeServiceRecord(
    int aChannel, wstring aServiceUUID, wstring aServiceName, bool aIsGOEP)
{
    JELOG2(EJavaBluetooth);
    int result = 0;

    if (NULL == mServRec)
    {
        TRAP(result, CallMethodL(mServRec, this,
                                 &RFCOMMServerConnection::createServiceRecordL, mServer));

        if (KErrNone == result)
        {
            TInt protocol = (true == aIsGOEP)?PROTOCOL_GOEP : PROTOCOL_RFCOMM;

            result = mServRec->initializeRecord(protocol,
                                                aChannel, aServiceUUID, aServiceName);
        }
    }
    return result;
}

OS_EXPORT int RFCOMMServerConnection::restorePersistentRecord()
{
    JELOG2(EJavaBluetooth);
    int result = 0;
    if (NULL != mServRec)
    {
        result = mServRec->restorePersistentRecord();
    }
    return result;
}

ServiceRecord *RFCOMMServerConnection::createServiceRecordL()
{
    return ServiceRecord::NewL(mServer);
}

int RFCOMMServerConnection::CloseServer()
{
    JELOG2(EJavaBluetooth);
    int result = 0;
    mIsConnected = EFalse;
    mAsyncAccept = false;
    CallMethod(this, &RFCOMMServerConnection::Close, mServer);
    return result;
}

void RFCOMMServerConnection::Close()
{
    JELOG2(EJavaBluetooth);

    // Deleting service record.
    ServiceRecord::cleanup(mServRec);

    if (mState != ENone)
    {
        mListenSock->CancelAll();
        mListenSock->Shutdown(RSocket::EImmediate);
        delete mListenSock; //Destructor closes the socket
        mListenSock = NULL;
        mState = ENone;
        mAcceptStatus = KErrCancel;
        if (mAcceptMonitor)
            mAcceptMonitor->notify();
    }
}

void RFCOMMServerConnection::DoCancel()
{
    JELOG2(EJavaBluetooth);
}

bool RFCOMMServerConnection::isConnectionAllowed(TBTSockAddr aBtAddr)
{
    JELOG2(EJavaBluetooth);
    if (NULL == mBtUrlParams)
    {
        return true;
    }

    bool authorize = false;
    bool authenticate = false;

    HBufC* devAddr = HBufC::New(aBtAddr.Length());
    TPtr ptr = devAddr->Des();
    TBTDevAddr btDeviceAddress = aBtAddr.BTAddr();
    btDeviceAddress.GetReadable((TDes&)ptr);
    if (true == mBtUrlParams->isBlockedSender(*devAddr))
    {
        delete devAddr;
        return false;
    }
    if (true == mBtUrlParams->isAllowedSender(*devAddr, authorize, authenticate))
    {
        LOG(EJavaBluetooth, EInfo,
            "  RFCOMMServerConnection::isConnectionAllowed AllowedSender");
        bool flag = true;
        TInt64 longBtAddr = 0;
        TLex16 toParse(*devAddr);
        toParse.Val(longBtAddr, EHex);
        LOG1(EJavaBluetooth, EInfo,
             " RFCOMMServerConnection::isConnectionAllowed Long BTRemote address %llx",longBtAddr);
        if (true == authorize)
        {
            flag = BluetoothRemoteDevice::getSecurityProperty(
                       REMOTE_AUTHORIZED, longBtAddr);
        }
        if (true == flag && true == authenticate)
        {
            flag = BluetoothRemoteDevice::getSecurityProperty(
                       REMOTE_AUTHENTICATED, longBtAddr);
        }
        delete devAddr;
        return flag;
    }
    delete devAddr;
    LOG(EJavaBluetooth, EInfo, "- RFCOMMServerConnection::isConnectionAllowed");
    return false;
}

void RFCOMMServerConnection::avoidFilter()
{
    JELOG2(EJavaBluetooth);
    mAvoidFilter = true;
}
//------------------------------------------------------------------------------
//  Methods from MBluetoothSocketNotifier to handle Bluetooth Events
//------------------------------------------------------------------------------

//Notification of an accept complete event
void RFCOMMServerConnection::HandleAcceptCompleteL(TInt err)
{
    TBTSockAddr btRemoteAddr;

    mAcceptStatus = err;

    LOG1(EJavaBluetooth, EInfo, "+  RFCOMMServerConnection::HandleAcceptCompleteL Err:%d", err);

    if (KErrNone == err)
    {
        mAcceptedSocket->RemoteName(btRemoteAddr);
        TBuf<20> addr;
        TInt64 longBtAddr = 0;
        TBTDevAddr btDeviceAddress = btRemoteAddr.BTAddr();
        LOG4(EJavaBluetooth,EInfo,"TBTDevAddr Address is %x %x %x %x ",
                                   btDeviceAddress[0],btDeviceAddress[1], 
                                   btDeviceAddress[2],btDeviceAddress[3]);
        LOG2(EJavaBluetooth,EInfo,"%x %x",btDeviceAddress[4], 
                                          btDeviceAddress[5]);
        btDeviceAddress.GetReadable(addr);
        LOG1(EJavaBluetooth,EInfo,"BT: Readable address is %S",&addr);
        TLex16 toParse(addr);
        toParse.Val(longBtAddr, EHex);
        LOG1(EJavaBluetooth, EInfo,
             "L2CAPServerConnection::HandleAcceptCompleteL: Address: %llx", longBtAddr);
        if ((longBtAddr == 0) ||(mAsyncAccept && (!mAvoidFilter) && (false == isConnectionAllowed(btRemoteAddr))))
        {
            mAcceptedSocket->CancelAll();
            mAcceptedSocket->Shutdown(RSocket::EImmediate);
            delete mAcceptedSocket;
            mAcceptedSocket = NULL;
            ELOG(EJavaBluetooth,
                 "RFCOMMServerConnection::HandleAcceptCompleteL Connection Rejected");
            AcceptL();
            return;
        }
        
        mBtClientConn = new BluetoothClientConnection(mAcceptedSocket, mServer);
        mBtClientConn->initialize(PROTOCOL_RFCOMM, longBtAddr, 0, 0);
        if (mAsyncAccept)
        {
            mPushEventListener->handleConnectionRequest(mBtClientConn, err);
        }
        else
        {
            mAcceptMonitor->notify();
        }
    }
    else
    {
        if (mAsyncAccept)
        {
            // If Pushframework cancels the listening then return otherwise
            // continue listening.
            if (KErrCancel == err)
            {
                mPushEventListener->handleConnectionRequest(NULL, err);
            }
            else
            {
                ELOG(EJavaBluetooth,
                     "  RFCOMMServerConnection::HandleAcceptCompleteL: Error while accept. Listening again.");
                AcceptL();
            }
            return;
        }
        else
        {
            mAcceptMonitor->notify();
        }
    }

    // If connection is accepted, then it should
    // disable the advertising flag
    if (NULL != mServRec)
    {
        mServRec -> setAdvertiseFs(EFalse);
    }

    LOG(EJavaBluetooth, EInfo, "-  RFCOMMServerConnection::HandleAcceptCompleteL");
}

// Notification of a baseband event
void RFCOMMServerConnection::HandleActivateBasebandEventNotifierCompleteL(
    TInt /*aErr*/, TBTBasebandEventNotification& /*aEventNotification*/)
{
    LOG1(EJavaBluetooth,EInfo,
         "+  RFCOMMServerConnection::HandleActivateBasebandEventNotifierCompleteL %s",
         "WARNING: Nothing to handle in this event !!");
}

//Notification of a connection complete event
void RFCOMMServerConnection::HandleConnectCompleteL(TInt /*aErr*/)
{
    LOG1(EJavaBluetooth,EInfo,
         "+  RFCOMMServerConnection::HandleConnectCompleteL %s",
         "WARNING: Nothing to handle in this event !!");
}

//Notification of a ioctl complete event
void RFCOMMServerConnection::HandleIoctlCompleteL(TInt /*aErr*/)
{
    LOG1(EJavaBluetooth,EInfo,
         "+  RFCOMMServerConnection::HandleIoctlCompleteL %s",
         "WARNING: Nothing to handle in this event !!");
}

//Notification of a receive complete event
void RFCOMMServerConnection::HandleReceiveCompleteL(TInt /*aErr*/)
{
    LOG1(EJavaBluetooth,EInfo,
         "+  RFCOMMServerConnection::HandleReceiveCompleteL %s",
         "WARNING: Nothing to handle in this event !!");
}

//Notification of a send complete event
void RFCOMMServerConnection::HandleSendCompleteL(TInt /*aErr*/)
{
    LOG1(EJavaBluetooth,EInfo,
         "+  RFCOMMServerConnection::HandleSendCompleteL %s",
         "WARNING: Nothing to handle in this event !!");
}

//Notification of a shutdown complete event
void RFCOMMServerConnection::HandleShutdownCompleteL(TInt /*aErr*/)
{
    LOG1(EJavaBluetooth,EInfo,
         "+  RFCOMMServerConnection::HandleShutdownCompleteL %s",
         "WARNING: Nothing to handle in this event !!");
}

} //end namespace bluetooth
} //end namespace java
