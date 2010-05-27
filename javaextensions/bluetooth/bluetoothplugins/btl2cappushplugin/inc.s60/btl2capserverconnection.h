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


#ifndef BTL2CAPSERVERCONNECTION_H
#define BTL2CAPSERVERCONNECTION_H

#include <e32base.h>
#include <bt_sock.h>
#include <btsdp.h>

#include "bluetoothfunctionserver.h"
#include "bluetoothpusheventlistener.h"
#include "monitor.h"
#include "servicerecord.h"
#include "bturlparams.h"

namespace java
{
namespace bluetooth
{

class L2CAPServerConnection: public MBluetoothSocketNotifier
{
public:
    OS_IMPORT L2CAPServerConnection(
        java::bluetooth::BluetoothFunctionServer* server);
            
    OS_IMPORT ~L2CAPServerConnection();

    OS_IMPORT int openServer(bool authorize, bool authenticate, bool encrypt,
                             bool master, int receiveMTU, int transmitMTU);

    // To create and initializes the service record.
    OS_IMPORT int initializeServiceRecord(
        TInt aPsmValue, TUUID &aServiceUUID, TDesC8 &aServiceName);

    // To create and initializes the service record.
    OS_IMPORT int initializeServiceRecord(int aPsmValue,
                                          std::wstring aServiceUUID, std::wstring aServiceName);

    // Needed in case of push. Accept should not block.
    OS_IMPORT int asyncAccept(
        java::bluetooth::BluetoothPushEventListener* aEventListener,
        BtUrlParams *aBtUrlParams);

    // Needed in case of push. Restore the service updates from file.
    OS_IMPORT int restorePersistentRecord();

    // NOTE: We could still retain these methods. We can avoid double conversion
    //      In case of push we need to use convertion from c++ to symbian c.
    //      In case of Jni, we can still convert directly to symbian.
    OS_IMPORT int ServerOpen(TBool authorize, TBool authenticate,
                             TBool encrypt, TBool master, TInt receiveMTU, TInt transmitMTU);

    OS_IMPORT int GetServerPSM();

    OS_IMPORT long Accept();

    OS_IMPORT ServiceRecord *getServiceRecordHandle();

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

    void avoidFilter();

    int CloseServer();

private:
    L2CAPServerConnection();

    void ServerOpenL(TBool authorize, TBool authenticate, TBool encrypt,
                     TBool master, TInt receiveMTU, TInt transmitMTU);

    void BindL(TBool authorize, TBool authenticate, TBool encrypt,
               TBool master, TInt receiveMTU, TInt transmitMTU);

    void ListenL();

    ServiceRecord *createServiceRecordL();

    int GetServerPSMValue();

    long AcceptL();

    void DoCancel();

    void Close();

    bool isConnectionAllowed(TBTSockAddr aBtAddr);

private:
    java::util::Monitor *mAcceptMonitor;
    int mAcceptStatus;
    CBluetoothSocket * mAcceptedSocket;
    TBool mMasterRoleRequested;

    // Used in case of Push.
    bool mAsyncAccept;
    //
    bool mAvoidFilter;

    // To notify once client has connected to push server
    BluetoothPushEventListener* mPushEventListener;

    // In case of Push, Filter can be used. This parameters used
    // to filter the incoming connections according to the Filter speicified
    // while registering for Push.
    BtUrlParams *mBtUrlParams;
private:
    BluetoothClientConnection *mBtClientConn;
    java::bluetooth::BluetoothFunctionServer* mServer;

    // Listening socket
    CBluetoothSocket* mListenSock;

    // Socket server handle
    RSocketServ mSocketServ;

    // Bluetooth socket address object
    TBTSockAddr mBtSockAddr;

    // Listener connection status
    TBool mIsConnected;

    // Service Record created for advertising the service
    ServiceRecord *mServRec;

    // State of the listener
    enum TState
    {
        ENone = 1,
        EConnecting,
        EWaiting,
        ESending
    };
    TState mState;
};

} //end namespace bluetooth
} //end namespace java

#endif // BTL2CAPSERVERCONNECTION_H
