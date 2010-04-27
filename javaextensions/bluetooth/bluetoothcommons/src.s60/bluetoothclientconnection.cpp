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


#include <string.h>
#include <unistd.h>
#include <bttypes.h>

#include "logger.h"
#include "fs_methodcall.h"
#include "functionserver.h"
#include "javasymbianoslayer.h"

#include "bluetoothfunctionserver.h"
#include "bluetoothclientconnection.h"
#include <string.h>

using namespace std;

using namespace java::util;
using namespace java::bluetooth;

OS_EXPORT BluetoothClientConnection::BluetoothClientConnection
(BluetoothFunctionServer* server):
        mServer(server),
        mShutdownNotifyMonitor(NULL),
        mReadNotifyMonitor(NULL),
        mBuffer(NULL, 0),
        mSendNotifyMonitor(NULL),
        mNegotiatedReceiveMtu(0),
        mNegotiatedTransmitMtu(0),
        mRemoteBTAddr(0),
        mConnectNotifyMonitor(NULL),
        mMakeJavaCallbackOnRead(EFalse),
        mReadPending(EFalse),
        mBufferInitialized(EFalse)
{
    JELOG2(EJavaBluetooth);
}

OS_EXPORT BluetoothClientConnection::BluetoothClientConnection
(CBluetoothSocket* aSocket, BluetoothFunctionServer* aServer):
        mServer(aServer),
        mSocket(aSocket),
        mShutdownNotifyMonitor(NULL),
        mReadNotifyMonitor(NULL),
        mBuffer(NULL, 0),
        mSendNotifyMonitor(NULL),
        mNegotiatedReceiveMtu(0),
        mNegotiatedTransmitMtu(0),
        mRemoteBTAddr(0),
        mMakeJavaCallbackOnRead(EFalse),
        mReadPending(EFalse),
        mBufferInitialized(EFalse)
{
    JELOG2(EJavaBluetooth);
}

OS_EXPORT BluetoothClientConnection::~BluetoothClientConnection()
{
    JELOG2(EJavaBluetooth);

    CloseFs();
    delete mReadNotifyMonitor;
    delete mSendNotifyMonitor;
    delete mShutdownNotifyMonitor;
    if (mBufferInitialized)
    {
        delete mBuffer.Ptr();
    }
}

// NOTE:
// TO be called by server objects doing accept and open.
// To be called always in Function server context
// Must be called only after connection has been successfully established
OS_EXPORT void BluetoothClientConnection::initialize(int protocol,
        TInt64 aRemoteAddr, int aReceiveMtu, int aTransmitMtu)
{
    JELOG2(EJavaBluetooth);
    // Set CBluetoothSocket to notify this object on events.
    mSocket->SetNotifier(*this);
    mProtocol = protocol;
    mRemoteBTAddr = aRemoteAddr;
    mNegotiatedReceiveMtu = aReceiveMtu;
    mNegotiatedTransmitMtu = aTransmitMtu;

    // Because of this call, we must call initialize in FS context
    ReceiveData();
}

OS_EXPORT int BluetoothClientConnection::init(int protocol)
{
    JELOG2(EJavaBluetooth);
    TRAPD(result, CallMethodL(this, &BluetoothClientConnection::InitL,
                              protocol, mServer));
    return result;
}

/**
 * Should be called in FunctionServer Context before any method of this class is called
 * Look at bluetoothconsts.h for constants:
 * PROTOCOL_L2CAP for L2CAP & PROTOCOL_RFCOMM for RFCOMM
 */
void BluetoothClientConnection::InitL(TInt protocol)
{
    JELOG2(EJavaBluetooth);
    //Connect to SocketServer
    User::LeaveIfError(mSocketServ.Connect());
    User::LeaveIfError(mSocketServ.ShareAuto());
    TProtocolDesc pdesc;

    if (PROTOCOL_L2CAP == protocol)
    {
        User::LeaveIfError(mSocketServ.FindProtocol(KL2CAPDesC(), pdesc));
        mSocket = CBluetoothSocket::NewL(*this, mSocketServ, pdesc.iSockType,
                                         KL2CAP);
    }
    else
    {
        User::LeaveIfError(mSocketServ.FindProtocol(KRFCOMMDesC(), pdesc));
        mSocket = CBluetoothSocket::NewL(*this, mSocketServ, pdesc.iSockType,
                                         KRFCOMM);
    }
    mProtocol = protocol;
}

OS_EXPORT void BluetoothClientConnection::close()
{
    if (NULL == mShutdownNotifyMonitor)
    {
        mShutdownNotifyMonitor = Monitor::createMonitor();
    }
    CallMethod(this, &BluetoothClientConnection::CloseFs, mServer);
    mShutdownNotifyMonitor->wait();
    delete mSocket;
    mSocket = 0;
}

void BluetoothClientConnection::CloseFs()
{
    JELOG2(EJavaBluetooth);
    if (mSocket)
    {
        mMakeJavaCallbackOnRead = EFalse;
        mSocket->CancelAll();
        mSocket->Shutdown(RSocket::EImmediate);
    }
}

/**
 * Calls ReceiveData in FunctionServer Context and allocates
 * memory needed and assigns to inBuf
 */
OS_EXPORT int BluetoothClientConnection::receive(char* &inBuf)
{
    JELOG2(EJavaBluetooth);
    // This receive must read the data already buffered and return the data.
    // For L2CAP the size of internal buffer should be the same as the
    // negotiated receive mtu.
    // For RFCOMM, it must be the same as java side buffer.
    // receive never blocks.

    int returnValue = mReadStatus;
    if (KErrNone == mReadStatus)
    {
        //Assign output parameter.
        int dataRead = mBuffer.Length();

        inBuf = new char[dataRead + 1];
        memcpy(inBuf, mBuffer.Ptr(), dataRead);
        inBuf[dataRead] = 0;

        LOG1(EJavaBluetooth, EInfo,
             "  BluetoothClientConnection::receive: Length of inBuf: %d",
             strlen((inBuf)));
        LOG1(EJavaBluetooth, EInfo,
             "- BluetoothClientConnection::receive: Length: %d", dataRead);
        returnValue = dataRead;
    }
    else
    {
        if (KErrEof == mReadStatus)
        {
            LOG(EJavaBluetooth, EInfo,
                "- BluetoothClientConnection::receive: EOF");
            returnValue = 0;
        }
        else if (KErrDisconnected == mReadStatus)
        {
            ELOG(EJavaBluetooth,
                 "- BluetoothClientConnection::receive Disconnected");
            return returnValue;
        }
    }

    // Trigger next read before returning.
    CallMethod(mReadStatus, this, &BluetoothClientConnection::ReceiveData,
               mServer);
    // IF we reach here, it means that something got messed up :)
    return returnValue;
}

OS_EXPORT void BluetoothClientConnection::registerCallback(JNIEnv* aJni,
        jobject peer)
{
    JELOG2(EJavaBluetooth);
    if (!(mServer->attachedToVm()))
    {
        LOG(EJavaBluetooth, EInfo,
            "+ BluetoothClientConnection::registerCallback Attaching to VM."
            " Should be called only in case of Push");
        mServer->attach(aJni, peer);
    }
    mMakeJavaCallbackOnRead = ETrue;

    jclass peerClass = aJni->GetObjectClass(peer);
    mReadCompleteCallback = aJni->GetMethodID(peerClass,
                            "receiveCompleteCallback", "(J)V");
}

void BluetoothClientConnection::makeReadCompleteCallback()
{
    JELOG2(EJavaBluetooth);
    JNIEnv* jni = mServer->getValidJniEnv();
    jobject peer = mServer->getPeer();

    (*jni).CallVoidMethod(peer, mReadCompleteCallback,
                          reinterpret_cast<jlong>(this));

    mMakeJavaCallbackOnRead = EFalse;
}

//Should be called in FunctionServer context.
TInt BluetoothClientConnection::ReceiveData()
{
    JELOG2(EJavaBluetooth);

    if (!mBufferInitialized)
    {
        TUint8 *tempBuf = NULL;
        int length = 512;
        if (PROTOCOL_L2CAP == mProtocol)
        {
            length = mNegotiatedReceiveMtu;
            tempBuf = new TUint8[length];
        }
        else
        {
            tempBuf = new TUint8[length];
            // NOTE: Size of this buffer should be the same as that specifie in BluetoothStreamer.
        }
        mBuffer.Set(tempBuf, 0, length);
        mBufferInitialized = ETrue;
    }

    TInt error = 0;

    //RecvOneOrMore does not block until the whole buffer is filled.
    //Returns as soon as some data is available.
    if (PROTOCOL_L2CAP == mProtocol)
    {
        // mBuffer should be the same as the negotiated MTU value.
        error = mSocket->Read(mBuffer);
    }
    else
    {
        LOG1(
            EJavaBluetooth,
            EInfo,
            "  BluetoothClientConnection::ReceiveData on RFCOMM Length: %d",
            mBuffer.MaxSize());
        error = mSocket->RecvOneOrMore(mBuffer, 0, mSockXfrLength);
    }

    if (KErrNone == error)
    {
        mReadPending = true;
    }

    LOG1(EJavaBluetooth, EInfo,
         "- BluetoothClientConnection::ReceiveData Err:%d", error);
    return error;
}

/**
 * Sends data by calling SendData in Function Server context.
 * Blocks until send is complete.
 */
OS_EXPORT int BluetoothClientConnection::send(const char* data, int length)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo, "  BluetoothClientConnection::send: Data Length: %d ",
         length);

    if (KErrDisconnected == mReadStatus)
    {
        ELOG(EJavaBluetooth, "  BluetoothClientConnection::send Disconnected");
        return mReadStatus;
    }

    if (NULL == data || 0 == length)
    {
        return 0;
    }

    if (!mSendNotifyMonitor)
    {
        mSendNotifyMonitor = Monitor::createMonitor();
    }

    TPtr8 dataPtr((TUint8 *) data, length, length);
    LOG1(EJavaBluetooth, EInfo,
         "  BluetoothClientConnection::send: Sending data length:%d", length);

    TInt error = 0;
    CallMethod(error, this, &BluetoothClientConnection::SendData, dataPtr,
               mServer);

    LOG(EJavaBluetooth, EInfo,
        "  BluetoothClientConnection::send: Waiting for data to come back");

    if (KErrNone == error)
    {
        mSendNotifyMonitor->wait();
    }
    else
    {
        return error;
    }

    ELOG1(EJavaBluetooth,
          "- BluetoothClientConnection::send: Sent status:%d", mWriteStatus);

    return mWriteStatus;
}


TInt BluetoothClientConnection::SendData(const TDesC8& aData)
{
    JELOG2(EJavaBluetooth);
    return mSocket->Send(aData, 0);
}

OS_EXPORT int BluetoothClientConnection::connect(long long aBtAddress,
        int aChannel, bool aAuthenticate, bool aEncrypt, int aReceiveMTU,
        int aTransmitMTU)
{
    JELOG2(EJavaBluetooth);
    if (NULL == mConnectNotifyMonitor)
    {
        mConnectNotifyMonitor = Monitor::createMonitor();
    }

    TInt auth = (aAuthenticate) ? 1 : 0;
    TInt enc = (aEncrypt) ? 1 : 0;
    TInt64 btAddress = aBtAddress;
    TInt channel = aChannel;
    mRequestedRMtu = aReceiveMTU;
    mRequestedTMtu = aTransmitMTU;

    TInt retVal = 0;
    CallMethod(retVal, this, &BluetoothClientConnection::Connect, btAddress,
               channel, auth, enc, mRequestedRMtu, mRequestedTMtu, mServer);

    if (0 == retVal)
    {
        mRemoteBTAddr = aBtAddress;
        // Will be notified by HandleConnectCompleteL()
        mConnectNotifyMonitor->wait();
        return mConnectError;
    }

    return retVal;
}

TInt BluetoothClientConnection::Connect(TInt64 btAddress, TInt channel,
                                        TInt authenticate, TInt encrypt, TInt receiveMTU, TInt transmitMTU)
{
    JELOG2(EJavaBluetooth);
    LOG3(
        EJavaBluetooth,
        EInfo,
        "  BluetoothClientConnection::Connect: Channel %d, RMTU: %d TMTU %d",
        channel, receiveMTU, transmitMTU);
    LOG1(EJavaBluetooth, EInfo, "  BluetoothClientConnection::Connect %lX",
         btAddress);
    LOG(EJavaBluetooth, EInfo,
        "  BluetoothClientConnection::Connect: Applying Settings");

    TBTDevAddr btDeviceAddress(btAddress);
    TBTSockAddr btsockaddr;
    btsockaddr.SetBTAddr(btDeviceAddress);
    btsockaddr.SetPort(channel);

    // Set security
    TBTServiceSecurity secSettings;
    if (authenticate == EFalse)
        secSettings.SetAuthentication(EFalse);
    else
        secSettings.SetAuthentication(ETrue);

    if (encrypt == EFalse)
        secSettings.SetEncryption(EFalse);
    else
        secSettings.SetEncryption(ETrue);
    // Attach the security settings.
    btsockaddr.SetSecurity(secSettings);

    if (PROTOCOL_L2CAP == mProtocol)
    {
        TL2CapConfigPkg options;

        if (receiveMTU >= KL2MinMTU)
            options().SetMaxReceiveUnitSize(receiveMTU);
        else
            options().SetMaxReceiveUnitSize(DEFAULT_MTU);

        if (transmitMTU >= KL2MinMTU)
            options().SetMaxTransmitUnitSize(transmitMTU);

        mSocket->SetOpt(KL2CAPUpdateChannelConfig, KSolBtL2CAP, options);
    }

    // Will be set once handle connect complete is called.
    mConnectError = 0;
    LOG(EJavaBluetooth, EInfo,
        "  BluetoothClientConnection::StartListenerL: Connecting to Server");
    int error = mSocket->Connect(btsockaddr);
    return error;
}

OS_EXPORT int BluetoothClientConnection::getTransmitMTU()
{
    return mNegotiatedTransmitMtu;
}

OS_EXPORT int BluetoothClientConnection::getReceiveMTU()
{
    return mNegotiatedReceiveMtu;
}

OS_EXPORT long long BluetoothClientConnection::getRemoteAddress()
{
    JELOG2(EJavaBluetooth);

    if (mRemoteBTAddr)
    {
        return mRemoteBTAddr;
    }

    return -1;
}

OS_EXPORT long BluetoothClientConnection::available()
{
    JELOG2(EJavaBluetooth);
    if (KErrDisconnected == mReadStatus)
    {
        ELOG(EJavaBluetooth,
             "  BluetoothClientConnection::available Disconnected");
        return mReadStatus;
    }

    if (mReadPending)
    {
        return 0;
    }
    else
    {
        return mBuffer.Length();
    }
}

//------------------------------------------------------------------------------
//  Methods from MBluetoothSocketNotifier to handle Bluetooth Events
//------------------------------------------------------------------------------

//Notification of an accept complete event
void BluetoothClientConnection::HandleAcceptCompleteL(TInt /*err*/)
{
    LOG1(EJavaBluetooth, EInfo,
         "+  BluetoothClientConnection::HandleAcceptCompleteL %s",
         "WARNING: Nothing to handle in this event !!");
}

// Notification of a baseband event
void BluetoothClientConnection::HandleActivateBasebandEventNotifierCompleteL(
    TInt /*aErr*/, TBTBasebandEventNotification& /*aEventNotification*/)
{
    LOG1(
        EJavaBluetooth,
        EInfo,
        "+  BluetoothClientConnection::HandleActivateBasebandEventNotifierCompleteL %s",
        "WARNING: Nothing to handle in this event !!");
}

//Notification of a connection complete event
void BluetoothClientConnection::HandleConnectCompleteL(TInt err)
{
    JELOG2(EJavaBluetooth);
    mConnectError = err;
    LOG1(EJavaBluetooth, EInfo,
         "  BluetoothClientConnection::HandleConnectCompleteL: %d", err);

    if (KErrNone == mConnectError && PROTOCOL_L2CAP == mProtocol)
    {
        mSocket->GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, mNegotiatedReceiveMtu);
        mSocket->GetOpt(KL2CAPNegotiatedOutboundMTU, KSolBtL2CAP,
                        mNegotiatedTransmitMtu);

        LOG1(
            EJavaBluetooth,
            EInfo,
            "  BluetoothClientConnection::HandleConnectCompleteL Negotiate Rx %d",
            mNegotiatedReceiveMtu);
        LOG1(
            EJavaBluetooth,
            EInfo,
            "  BluetoothClientConnection::HandleConnectCompleteL Negotiate Tx %d",
            mNegotiatedTransmitMtu);

    }
    mConnectNotifyMonitor->notify();

    if (KErrNone == mConnectError)
    {
        // Start to fetch data immediately.
        ReceiveData();
    }
}

//Notification of a ioctl complete event
void BluetoothClientConnection::HandleIoctlCompleteL(TInt /*err*/)
{
    LOG1(EJavaBluetooth, EInfo,
         "+  BluetoothClientConnection::HandleIoctlCompleteL %s",
         "WARNING: Nothing to handle in this event !!");
}

//Notification of a receive complete event
void BluetoothClientConnection::HandleReceiveCompleteL(TInt err)
{
    JELOG2(EJavaBluetooth);
    mReadStatus = 0;

    // Here following operations are performed.
    //  1. Store receive status.
    //  2. Store amount of data read.
    //  3. Change flag to not receiving.

    if (err != KErrNone)
    {
        ELOG1(
            EJavaBluetooth,
            "  BluetoothClientConnection::HandleReceiveCompleteL: Error: %d",
            err);
        mReadStatus = err;
    }
    else
    {
        LOG1(
            EJavaBluetooth,
            EInfo,
            "  BluetoothClientConnection::HandleReceiveCompleteL: EWaiting Length: %d",
            mBuffer.Length());
    }
    if (mMakeJavaCallbackOnRead)
    {
        LOG(EJavaBluetooth, EInfo,
            "  BluetoothClientConnection::HandleReceiveCompleteL: Calling java");
        makeReadCompleteCallback();
    }
    mReadPending = EFalse;

}

//Notification of a send complete event
void BluetoothClientConnection::HandleSendCompleteL(TInt err)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "  BluetoothClientConnection::HandleSendCompleteL %d: ", err);
    mWriteStatus = 0;
    if (err != KErrNone)
    {
        mWriteStatus = err;
    }
    mSendNotifyMonitor->notify();
}

//Notification of a shutdown complete event
void BluetoothClientConnection::HandleShutdownCompleteL(TInt /*err*/)
{
//    LOG1(EJavaBluetooth, EInfo,
//         "+  BluetoothClientConnection::HandleShutdownCompleteL Err:%d", err);

    mShutdownNotifyMonitor->notify();
}

