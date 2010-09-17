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
#include "bluetoothclientconnection.h"
#include "fs_methodcall.h"

#include "btl2capserverconnection.h"
#include "bluetoothpusheventlistener.h"
#include "javasymbianoslayer.h"
#include <e32cmn.h>

using namespace std;

namespace java
{
namespace bluetooth
{

const TInt KSizeOfListenQueue = 7;

OS_EXPORT L2CAPServerConnection::L2CAPServerConnection(
    java::bluetooth::BluetoothFunctionServer* server):
        mAcceptMonitor(NULL),
        mAcceptStatus(0),
        mAcceptedSocket(0),
        mMasterRoleRequested(EFalse),
        mAsyncAccept(false),
        mAvoidFilter(false),
        mPushEventListener(0),
        mBtUrlParams(NULL),
        mBtClientConn(NULL),
        mServer(server),
        mListenSock(0),
        mIsConnected(EFalse),
        mServRec(NULL),
        mState(ENone)
{
}

OS_EXPORT L2CAPServerConnection::~L2CAPServerConnection()
{
    CloseServer();
    delete mAcceptMonitor;
}

OS_EXPORT int L2CAPServerConnection::openServer(bool authorize,
        bool authenticate, bool encrypt, bool master, int receiveMTU,
        int transmitMTU)
{
    //convert and call ServerOpen()
    JELOG2(EJavaBluetooth);
    LOG2(EJavaBluetooth, EInfo,
         "* L2CAPServerConnection::openServer TxMTU:%d RxMTU:%d",
         transmitMTU, receiveMTU);
    return ServerOpen(authorize, authenticate, encrypt, master, receiveMTU,
                      transmitMTU);
}

OS_EXPORT int L2CAPServerConnection::ServerOpen(TBool authorize,
        TBool authenticate, TBool encrypt, TBool master, TInt receiveMTU,
        TInt transmitMTU)
{
    int result = 0;
    LOG2(EJavaBluetooth, EInfo,
         "+ L2CAPServerConnection::ServerOpen TxMTU:%d RxMTU:%d",
         transmitMTU, receiveMTU);
    TRAP(result, CallMethodL(this, &L2CAPServerConnection::ServerOpenL,
                             authorize, authenticate, encrypt, master, receiveMTU, transmitMTU,
                             mServer));
    return result;
}

OS_EXPORT ServiceRecord *L2CAPServerConnection::getServiceRecordHandle()
{
    return mServRec;
}

void L2CAPServerConnection::ServerOpenL(TBool authorize, TBool authenticate,
                                        TBool encrypt, TBool master, TInt receiveMTU, TInt transmitMTU)
{
    LOG(EJavaBluetooth, EInfo, "+ L2CAPServerConnection::ServerOpenL ");

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
        BindL(authorize, authenticate,
              encrypt, master, receiveMTU, transmitMTU);

        // Set a socket to listen for incoming connections
        ListenL();
    }
         );

    if (KErrNone != err)
    {
        mState = ENone;
        mSocketServ.Close();
        User::LeaveIfError(err);
    }

    LOG(EJavaBluetooth, EInfo, "- L2CAPServerConnection::ServerOpenL ");
}

void L2CAPServerConnection::BindL(TBool authorize, TBool authenticate,
                                  TBool encrypt, TBool master, TInt receiveMTU, TInt transmitMTU)
{
    LOG2(EJavaBluetooth, EInfo,
         "+ L2CAPServerConnection::BindL TxMTU:%d RxMTU:%d",
         transmitMTU, receiveMTU);
    // Load protocol, L2CAP
    TProtocolDesc protocolDesc;
    User::LeaveIfError(mSocketServ.FindProtocol(KL2CAPDesC(), protocolDesc));

    LOG(EJavaBluetooth, EInfo,
        "  L2CAPServerConnection::BindL: Opening A Listerner Socket ");
    // Open a socket
    mListenSock = CBluetoothSocket::NewL(*this, mSocketServ,
                                         protocolDesc.iSockType, KL2CAP);

    // Set security
    TBTServiceSecurity secSettings;

    secSettings.SetAuthentication(authenticate);
    secSettings.SetAuthorisation(authorize);
    secSettings.SetEncryption(encrypt);

    // Later once connection accepted this flag is used to set the role
    mMasterRoleRequested = master;

    LOG(EJavaBluetooth, EInfo,
        "  L2CAPServerConnection::BindL: Attaching The Security Settings ");
    // Attach the security settings.
    mBtSockAddr.SetSecurity(secSettings);

    if (transmitMTU >= KL2MinMTU || receiveMTU >= KL2MinMTU)
    {
        TL2CapConfigPkg options;
        TInt errMtu = KErrNone;

        if (transmitMTU >= KL2MinMTU)
        {
            errMtu = options().SetMaxTransmitUnitSize(transmitMTU);
            if (KErrNone != errMtu)
            {
                ELOG2(EJavaBluetooth,
                      "  L2CAPServerConnection::BindL: TxMTU(%d) Err:%d",
                      transmitMTU, errMtu);
            }
        }
        else
        {
            errMtu = options().SetMaxTransmitUnitSize(DEFAULT_MTU);
            if (KErrNone != errMtu)
            {
                ELOG2(EJavaBluetooth,
                      "  L2CAPServerConnection::BindL: Default TxMTU(%d) Err:%d",
                      transmitMTU, errMtu);
            }
        }

        if (receiveMTU >= KL2MinMTU)
        {
            errMtu = options().SetMaxReceiveUnitSize(receiveMTU);
            if (KErrNone != errMtu)
            {
                ELOG2(EJavaBluetooth,
                      "  L2CAPServerConnection::BindL: RxMTU(%d) Err:%d",
                      receiveMTU, errMtu);
            }
        }
        else
        {
            errMtu = options().SetMaxReceiveUnitSize(DEFAULT_MTU);
            if (KErrNone != errMtu)
            {
                ELOG2(EJavaBluetooth,
                      "  L2CAPServerConnection::BindL: Default RxMTU(%d) Err:%d",
                      receiveMTU, errMtu);
            }
        }
        //L2CAP Specific stuff
        mListenSock->SetOpt(KL2CAPUpdateChannelConfig, KSolBtL2CAP, options);
    }

    // Setting l2cap passive port
    mBtSockAddr.SetPort(KL2CAPPassiveAutoBind);

    LOG(EJavaBluetooth, EInfo,
        "  L2CAPServerConnection::BindL: Binding Bluetooth Socket ");
    // Bind bluetooth socket
    User::LeaveIfError(mListenSock->Bind(mBtSockAddr));
    LOG(EJavaBluetooth, EInfo, "- L2CAPServerConnection::BindL");
}

OS_EXPORT int L2CAPServerConnection::GetServerPSM()
{
    int result = 0;
    CallMethod(result, this, &L2CAPServerConnection::GetServerPSMValue, mServer);
    return result;
}

int L2CAPServerConnection::GetServerPSMValue()
{
    JELOG2(EJavaBluetooth);
    if (mState != EConnecting)
    {
        return -1;
    }
    return mListenSock->LocalPort();
}

void L2CAPServerConnection::ListenL()
{
    JELOG2(EJavaBluetooth);
    // Listen on port
    User::LeaveIfError(mListenSock->Listen(KSizeOfListenQueue));
}

OS_EXPORT int L2CAPServerConnection::asyncAccept(
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
    TRAPD(err, CallMethodL(result, this, &L2CAPServerConnection::AcceptL,
                           mServer));
    return err;
}

OS_EXPORT long L2CAPServerConnection::Accept()
{
    JELOG2(EJavaBluetooth);
    long result = 0;
    mBtClientConn = NULL;

    TRAPD(err, CallMethodL(result, this, &L2CAPServerConnection::AcceptL,
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

long L2CAPServerConnection::AcceptL()
{
    JELOG2(EJavaBluetooth);

    // Open blank socket and pass it to accept to be assigned a proper
    // socket upon completion of Accept()
    mAcceptedSocket = CBluetoothSocket::NewL(*this, mSocketServ);
    LOG(EJavaBluetooth, EInfo,
        "  L2CAPServerConnection::Accept: Set To Accept Incoming Connections ");

    if (NULL == mAcceptMonitor)
    {
        mAcceptMonitor = java::util::Monitor::createMonitor();
    }

    if (true == mMasterRoleRequested)
    {
        if (KErrNone == mAcceptedSocket->RequestMasterRole())
        {
            LOG(EJavaBluetooth, EInfo,
                "  L2CAPServerConnection::Accept: Added the master role");
        }
    }
    mAcceptStatus = 0;

    // Enabling the advertising flag
    if (NULL != mServRec)
    {
        mServRec -> setAdvertiseFs(ETrue);
    }
    // Initiating accepting for the new client connection
    mListenSock->Accept((*mAcceptedSocket));

    return 0;
}

OS_EXPORT int L2CAPServerConnection::initializeServiceRecord(
    TInt aPsmValue, TUUID &aServiceUUID, TDesC8 &aServiceName)
{
    JELOG2(EJavaBluetooth);

    int result = 0;

    if (NULL == mServRec)
    {
        TRAP(result, CallMethodL(mServRec, this,
                                 &L2CAPServerConnection::createServiceRecordL, mServer));

        if (KErrNone == result)
        {
            TInt protocol = PROTOCOL_L2CAP;

            TRAP(result, mServRec->initializeRecordL(protocol,
                    aPsmValue, aServiceUUID, aServiceName));
        }
    }

    return result;
}

OS_EXPORT int L2CAPServerConnection::initializeServiceRecord(
    int aPsmValue, wstring aServiceUUID, wstring aServiceName)
{
    JELOG2(EJavaBluetooth);
    int result = 0;

    if (NULL == mServRec)
    {
        TRAP(result, CallMethodL(mServRec, this,
                                 &L2CAPServerConnection::createServiceRecordL, mServer));

        if (KErrNone == result)
        {
            result = mServRec->initializeRecord(PROTOCOL_L2CAP,
                                                aPsmValue, aServiceUUID, aServiceName);
        }
    }
    return result;
}

OS_EXPORT int L2CAPServerConnection::restorePersistentRecord()
{
    JELOG2(EJavaBluetooth);
    int result = 0;

    if (NULL != mServRec)
    {
        result = mServRec->restorePersistentRecord();
    }
    return result;
}

ServiceRecord *L2CAPServerConnection::createServiceRecordL()
{
    JELOG2(EJavaBluetooth);
    return ServiceRecord::NewL(mServer);
}

int L2CAPServerConnection::CloseServer()
{
    JELOG2(EJavaBluetooth);
    int result = 0;
    mIsConnected = EFalse;
    mAsyncAccept = false;
    CallMethod(this, &L2CAPServerConnection::Close, mServer);
    return result;
}

void L2CAPServerConnection::Close()
{
    JELOG2(EJavaBluetooth);

    // Deleting service record.
    ServiceRecord::cleanup(mServRec);

    if (mState != ENone)
    {
        LOG(EJavaBluetooth, EInfo, "  L2CAPServerConnection::Close ");
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

void L2CAPServerConnection::DoCancel()
{
    JELOG2(EJavaBluetooth);
}

bool L2CAPServerConnection::isConnectionAllowed(TBTSockAddr aBtAddr)
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
            "  L2CAPServerConnection::isConnectionAllowed AllowedSender");
        bool flag = true;
        TInt64 longBtAddr = 0;
        TLex16 toParse(*devAddr);
        toParse.Val(longBtAddr, EHex);
        LOG1(EJavaBluetooth, EInfo,
             " L2CAPServerConnection::isConnectionAllowed Long BTRemote address %llx",longBtAddr);
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
    LOG(EJavaBluetooth, EInfo, "- L2CAPServerConnection::isConnectionAllowed");
    return false;
}

//
void L2CAPServerConnection::avoidFilter()
{
    JELOG2(EJavaBluetooth);
    mAvoidFilter = true;
}
//------------------------------------------------------------------------------
//  Methods from MBluetoothSocketNotifier to handle Bluetooth Events
//------------------------------------------------------------------------------

//Notification of an accept complete event
void L2CAPServerConnection::HandleAcceptCompleteL(TInt err)
{
    TBTSockAddr btRemoteAddr;
    int negotiatedReceiveMtu = 0;
    int negotiatedTransmitMtu = 0;

    mAcceptStatus = err;

    LOG1(EJavaBluetooth, EInfo, "+  L2CAPServerConnection::HandleAcceptCompleteL Err:%d", err);

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
                 "L2CAPServerConnection::HandleAcceptCompleteL Connection Rejected");
            AcceptL();
            return;
        }
        
        mBtClientConn = new BluetoothClientConnection(mAcceptedSocket, mServer);
        mAcceptedSocket->GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, negotiatedReceiveMtu);
        mAcceptedSocket->GetOpt(KL2CAPNegotiatedOutboundMTU, KSolBtL2CAP, negotiatedTransmitMtu);
        mBtClientConn->initialize(
            PROTOCOL_L2CAP, longBtAddr, negotiatedReceiveMtu, negotiatedTransmitMtu);
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
            // If Push framework cancels the listening then return otherwise
            // continue listening.
            if (KErrCancel == err)
            {
                mPushEventListener->handleConnectionRequest(NULL, err);
            }
            else
            {
                ELOG(EJavaBluetooth,
                     "  L2CAPServerConnection::HandleAcceptCompleteL: Error while accept. Listening again.");
                AcceptL();
            }
            return;
        }
        else
        {
            mAcceptMonitor->notify();
        }
    }

    // If connection accepted, then it should
    // disable the advertising flag
    if (NULL != mServRec)
    {
        mServRec -> setAdvertiseFs(EFalse);
    }

    LOG(EJavaBluetooth, EInfo, "-  L2CAPServerConnection::HandleAcceptCompleteL");
}

// Notification of a baseband event
void L2CAPServerConnection::HandleActivateBasebandEventNotifierCompleteL(
    TInt /*aErr*/, TBTBasebandEventNotification& /*aEventNotification*/)
{
}

//Notification of a connection complete event
void L2CAPServerConnection::HandleConnectCompleteL(TInt /*aErr*/)
{
}

//Notification of a ioctl complete event
void L2CAPServerConnection::HandleIoctlCompleteL(TInt /*aErr*/)
{
}

//Notification of a receive complete event
void L2CAPServerConnection::HandleReceiveCompleteL(TInt /*aErr*/)
{
}

//Notification of a send complete event
void L2CAPServerConnection::HandleSendCompleteL(TInt /*aErr*/)
{
}

//Notification of a shutdown complete event
void L2CAPServerConnection::HandleShutdownCompleteL(TInt /*aErr*/)
{
}

} //end namespace bluetooth
} //end namespace java
