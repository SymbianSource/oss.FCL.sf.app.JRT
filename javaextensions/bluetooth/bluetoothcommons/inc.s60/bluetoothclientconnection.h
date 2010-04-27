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


#ifndef BLUETOOTHCLIENTCONNECTION_H
#define BLUETOOTHCLIENTCONNECTION_H

#include <e32base.h>
#include <bt_sock.h>
#include <btsdp.h>

#include "monitor.h"
#include "bluetoothfunctionserver.h"
#include "bluetoothconsts.h"

#include <sys/time.h>

//#define LAUNCH_TYPE_PUSH                     3
//#define LAUNCH_TYPE_JSR                      4

#define DEFAULT_MTU    672
namespace java
{
namespace bluetooth
{

const TInt KReceivedBufferSize = 512;

/**
 * BluetoothClientConnection manages all Bluetooth Client operations.
 * Responsible for send/receive data.
 *
 *
 */
class BluetoothClientConnection: public MBluetoothSocketNotifier
{
public:
    OS_IMPORT BluetoothClientConnection(BluetoothFunctionServer* server);

    /**
     * Constructor to be used by Server Side connections.
     * @param[in] aSocket: Socket got as a result of acceptAndOpen call.
     * @param[in] aServer: Handle to the Function Server in the context of which
     *                   aSocket has to be used. Since RObjects are not sharable
     *                   across threads, we need to do this.
     * @param[in] aProtocol: Protocol to which the Socket belongs to.
     *                     PROTOCOL_L2CAP or PROTOCOL_RFCOMM
     *
     * @see Call void init() after construction of the object.
     */
    OS_IMPORT BluetoothClientConnection(CBluetoothSocket* aSocket,
                                        BluetoothFunctionServer* aServer);

    OS_IMPORT ~BluetoothClientConnection();

    /**
     * To be used after creating a ClientConnection object from the Server
     * implementation. Since the RSocket passed is already initialized during
     * connection open, all we do here is to add the object to Active Scheduler.
     */
    OS_IMPORT
    void BluetoothClientConnection::initialize(int protocol,
            TInt64 aRemoteAddr, int aReceiveMtu, int aTransmitMtu);

    /**
     * Used to Initialize the protocol specific options of the Bluetooth Socket.
     * This needs to be called before a call to Connect is made.
     * @param[in] integer value representing the Protocol.
     *            Defined in BluetoothConsts: PROTOCOL_L2CAP and PROTOCOL_RFCOMM
     * @return error code.
     */
    OS_IMPORT int init(int protocol);

    /**
     * Used to Connect to a remote Bluetooth device.
     * The same call serves for both L2CAP and RFCOMM. Just need to make sure we
     * call init before making call to connect. With protocol specified.
     *
     * @param[in] btAddress: Address of the remote device to connect to.
     * @param[in] channel: The port at which the remote device is waiting.
     * @param[in] authenticate: Indicates if authenticate has to be switched on.
     * @param[in] encrypt: Indicates if encryption has to be enabled on socket.
     * @param[in] receiveMTU: In case of L2CAP connections, indicates the
     *                      Receive MTU value to be set. For RFCOMM Connections,
     *                      nothing needs to be passed.
     * @param[in] transmitMTU: In case of L2CAP connections, indicates the
     *                      Transmit MTU value to be set. For RFCOMM Connections,
     *                      nothing needs to be passed.
     *
     * @return error code.
     */
    OS_IMPORT
    int connect(long long btAddress, int channel, bool authenticate,
                bool encrypt, int receiveMTU = 0, int transmitMTU = 0);


    /**
     * Receive is used to read data from Bluetooth Socket
     * Returns immediately when some data is available in the socket.
     * @param[out]  inBuf: Pointer to a byte array containing data received from
     *                   the socket. Memory needed will be allocated within
     *                   receive based on the amount of data received.
     * @return Number of bytes received. 0 if EOF and -<error> in case of error.
     */
    OS_IMPORT int receive(char* &inBuf);

    /**
     * Send data to Bluetooth Socket
     * Returns immediately when data is sent through the socket.
     * @param[in]  data: Data that has to be sent through the socket.
     * @param[in]  length: Length of data that needs to be sent over.
     * @return Number of bytes successfully sent. -<error> in case of error.
     */
    OS_IMPORT int send(const char* data, int length);

    /**
     * Closes the current connection with remote device.
     */
    OS_IMPORT void close();
    /**
     * Retrieves the remote address of the device the currently connected to.
     * @return address of the remote device.
     */
    OS_IMPORT long long getRemoteAddress();

    /**
     * Retreives the Transmit MTU of the Connection. This is valid only in case
     * of L2CAP Connections.
     * @return transmit MTU value.
     */
    OS_IMPORT int getTransmitMTU();

    /**
     * Retreives the Receive MTU of the Connection. This is valid only in case
     * of L2CAP Connections.
     * @return Receive MTU value.
     */
    OS_IMPORT int getReceiveMTU();

    /**
     * returns available data.
     */

    OS_IMPORT long available();

    OS_IMPORT void registerCallback(JNIEnv* ajni, jobject peer);

public:
    // Methods from MBluetoothSocketNotifier
    //Notification of an accept complete event
    void HandleAcceptCompleteL(TInt err);

    // Notification of a baseband event
    void HandleActivateBasebandEventNotifierCompleteL(TInt aErr,
            TBTBasebandEventNotification &aEventNotification);

    //Notification of a connection complete event
    void HandleConnectCompleteL(TInt err);

    //Notification of a ioctl complete event
    void HandleIoctlCompleteL(TInt err);

    //Notification of a receive complete event
    void HandleReceiveCompleteL(TInt err);

    //Notification of a send complete event
    void HandleSendCompleteL(TInt err);

    //Notification of a shutdown complete event
    void HandleShutdownCompleteL(TInt err);

private:
    BluetoothClientConnection();

    //Used in case of L2CAP Connection
    TInt Connect(TInt64 btAddress, TInt channel, TInt authenticate,
                 TInt encrypt, TInt receiveMTU, TInt transmitMTU);

    TInt ReceiveData();
    TInt SendData(const TDesC8& aData);
    void InitL(TInt protocol);
    void CloseFs();

    void makeReadCompleteCallback();

private:
    // Function Server handle within which the Socket operations have to be
    // performed.
    BluetoothFunctionServer* mServer;

    // Client socket
    CBluetoothSocket *mSocket;

    // Monitor used to Notify on completion of Shutdown.
    java::util::Monitor* mShutdownNotifyMonitor;

    // Monitor used to Notify on completion of read.
    java::util::Monitor* mReadNotifyMonitor;

    //Holds the read status value. Needed in case of errors
    int mReadStatus;

    // Buffer holding received data.
    TPtr8 mBuffer;

    // Monitor used to Notify on completion of send.
    java::util::Monitor* mSendNotifyMonitor;

    TInt mRequestedRMtu;
    TInt mRequestedTMtu;
    //Holds the read status value
    int mWriteStatus;

    // Knows which protocol the object relates to
    int mProtocol;

private:
    int mNegotiatedReceiveMtu;

    int mNegotiatedTransmitMtu;

    TInt64 mRemoteBTAddr;

    // Length of received data
    TSockXfrLength mLen;

    // Socket server handle.
    RSocketServ mSocketServ;

    TSockXfrLength mSockXfrLength;

    // Monitor used to notify completion of async operations to BlueCoveS60.
    java::util::Monitor* mConnectNotifyMonitor;

    TInt mConnectError;
    TBool mMakeJavaCallbackOnRead;

    TBool mReadPending;

    TBool mBufferInitialized;
    jmethodID mReadCompleteCallback;
};

} //end namespace bluetooth
} //end namespace java
#endif // BLUETOOTHCLIENTCONNECTION_H
