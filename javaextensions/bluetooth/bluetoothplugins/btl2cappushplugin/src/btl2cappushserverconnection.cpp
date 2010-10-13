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


#include "btl2cappushserverconnection.h"
#include "btl2capserverconnectionfactory.h"
#include "fs_methodcall.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "serviceclasshandler.h"
#include "bluetoothnamelookup.h"
#include "javacommonutils.h"

using namespace std;
using namespace java::util;
using namespace java::push;

namespace java
{
namespace bluetooth
{

OS_EXPORT L2CapPushServerConnection::L2CapPushServerConnection(
    const wstring aUri, const wstring aFilter):
        mAcceptMonitor(NULL),
        mConnectionListener(0),
        mConnectionUri(aUri),
        mConnectionFilter(aFilter),
        mL2CAPServer(NULL),
        mClientConnection(0),
        mPendingConnection(false),
        mListening(false),
        mCreatedByPush(false),
        mClearServiceClassBitsFlag(true)
{
    JELOG2(EJavaBluetooth);

    mFunctionServer = new BluetoothFunctionServer();
    // Create Bluetooth Parameters
    mBtUrlParams = new BtUrlParams(mConnectionUri, mConnectionFilter);

    // Create a Server Object and store it as member.
    mL2CAPServer = new L2CAPServerConnection(mFunctionServer);
}

OS_EXPORT L2CapPushServerConnection::~L2CapPushServerConnection()
{
    JELOG2(EJavaBluetooth);
    delete mBtUrlParams;
    deleteServer();
    delete mFunctionServer;
}

/**
 * This method is called by Push framework to start listening.
 * Steps to be followed to start listening.
 * 1. Prompt user to switch in Bluetooth if it is switched off.
 * 2. Create Btl2capServer and ask it to start listening.
 *
 *
 */
OS_EXPORT void L2CapPushServerConnection::open(ConnectionListener* aListener)
{
    JELOG2(EJavaBluetooth);
    mConnectionListener = aListener;

    bool authenticate = mBtUrlParams->getParamAuthenticate();
    bool authorize = mBtUrlParams->getParamAuthorize();
    bool encrypt = mBtUrlParams->getParamEncrypt();
    bool master = mBtUrlParams->getParamMaster();
    int recvMtu = mBtUrlParams->getParamReceiveMtu();
    int transMtu = mBtUrlParams->getParamTransmitMtu();

    int error = 0;

    // Open L2CAP Server.
    error = mL2CAPServer->openServer(authorize, authenticate, encrypt, master,
                                     recvMtu, transMtu);
    if (error < 0)
    {
        std::string errTxt("ERROR!!! Unable to Open L2CAP Server.");
        throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                            __FUNCTION__, __LINE__);
    }

    int psmValue = mL2CAPServer->GetServerPSM();

    std::wstring uuid = mBtUrlParams->getServiceUuid();
    std::wstring serviceName = mBtUrlParams->getParamName();

    LOG1(EJavaBluetooth, EInfo, "  L2CapPushServerConnection::open uuid:%S",
         uuid.c_str());

    // Initializing the Service record
    error = mL2CAPServer->initializeServiceRecord(psmValue, uuid, serviceName);

    if (error < 0)
    {
        std::string errTxt("ERROR: Unable to Initialize L2CAP Server");
        throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                            __FUNCTION__, __LINE__);
    }

    // Restore Persistent Record (if any)
    mL2CAPServer->restorePersistentRecord();

    // Ask server to start
    mL2CAPServer->asyncAccept(this, mBtUrlParams);

    // We are listening now.
    mListening = true;

    // Reset the flag
    mClearServiceClassBitsFlag = true;
}

/*
 * When server is push registered, and manual launched server is closed,
 * it is not required to clear the service class bits
 * associated with this service.
 */
OS_EXPORT void L2CapPushServerConnection::unsetClearServiceClassBitsFlag()
{
    JELOG2(EJavaBluetooth);
    mClearServiceClassBitsFlag = false;
}

OS_EXPORT void L2CapPushServerConnection::close()
{
    JELOG2(EJavaBluetooth);
    if (true == mClearServiceClassBitsFlag)
    {
        ServiceClassHandler::setDeviceServiceClass(0);
    }
    mAcceptMonitor = NULL;
    mPendingConnection = false;
    mListening = false;
    mL2CAPServer->CloseServer();
}

/*
 * Gets the connection URI used to open this server connection.
 */
OS_EXPORT std::wstring L2CapPushServerConnection::getUri() const
{
    JELOG2(EJavaBluetooth);
    return mConnectionUri;
}

/*
 * Gets the connection Filter string associate with this connection.
 */
OS_EXPORT std::wstring L2CapPushServerConnection::getFilter() const
{
    JELOG2(EJavaBluetooth);
    return mConnectionFilter;
}

OS_EXPORT
void L2CapPushServerConnection::setFilter(const std::wstring& aFilter)
{
    JELOG2(EJavaBluetooth);
    mConnectionFilter = aFilter;
}

void L2CapPushServerConnection::handleConnectionRequest(
    BluetoothClientConnection* aClientConnection, TInt err)
{
    JELOG2(EJavaBluetooth);
    //Here we handle checking of parameters and deciding to invoke push.
    //Check all parameters
    if (KErrNone != err)
    {
        if (NULL != mAcceptMonitor)
        {
            mAcceptMonitor->notify();
        }
        mListening = false;
        return;
    }
    BtL2CapServerConnectionFactory& connectionFactory =
        BtL2CapServerConnectionFactory::getFactory();
    connectionFactory.setPendingMsgFlag(mConnectionUri, true);

    std::wstring* remoteDeviceName = NULL;

    BluetoothNameLookup * nameLookup = NULL;

    long long remoteDevAddr = aClientConnection->getRemoteAddress();
    LOG1(EJavaBluetooth, EInfo,
     "L2CapPushServerConnection::handleConnectionRequest: remoteDevAddr: %llx",
                                                                remoteDevAddr); 
    TRAPD(lookupErr,
    {
        nameLookup = BluetoothNameLookup::NewL();
        remoteDeviceName = nameLookup->doDeviceNameLookupL(remoteDevAddr);
        delete nameLookup;
    }
         );

    if (KErrNone != lookupErr)
    {
        remoteDeviceName = new std::wstring(
            JavaCommonUtils::longLongToWstring(remoteDevAddr));
    }

    mConnectionListener->msgArrived(*remoteDeviceName);

    delete remoteDeviceName;

    // This means that when jsr gets this object, it must complete
    // acceptAndOpen immediately
    mPendingConnection = true;
    mListening = false;

    // Store this as member. We can later return this when
    // queried from jsr.
    mClientConnection = aClientConnection;

    if (NULL != mAcceptMonitor)
    {
        mAcceptMonitor->notify();
    }

}

OS_EXPORT void L2CapPushServerConnection::setAcceptMonitor(
    java::util::Monitor* aMonitor)
{
    mAcceptMonitor = aMonitor;
    mL2CAPServer->avoidFilter();
}

OS_EXPORT void L2CapPushServerConnection::unsetAcceptMonitor()
{
    mAcceptMonitor = NULL;
}

OS_EXPORT bool L2CapPushServerConnection::isActive()
{
    JELOG2(EJavaBluetooth);
    bool result = (mPendingConnection || mListening);
    LOG1(EJavaBluetooth, EInfo,
         "+ L2CapPushServerConnection::isActive result: %d", result);
    return result;
}

OS_EXPORT bool L2CapPushServerConnection::isConnectionAccepted()
{
    return mPendingConnection;
}

OS_EXPORT bool L2CapPushServerConnection::isListening()
{
    return mListening;
}

OS_EXPORT void L2CapPushServerConnection::setCreatedByPush()
{
    mCreatedByPush = true;
}

OS_EXPORT bool L2CapPushServerConnection::isCreatedByPush()
{
    return mCreatedByPush;
}
OS_EXPORT L2CAPServerConnection* L2CapPushServerConnection::getServerObject()
{
    JELOG2(EJavaBluetooth);
    if (mL2CAPServer)
    {
        LOG(EJavaBluetooth, EInfo,
            "- L2CapPushServerConnection::getServerObject Returning existing L2CAP Server");
        return mL2CAPServer;
    }
    else
    {
        mL2CAPServer = new L2CAPServerConnection(mFunctionServer);
        LOG(EJavaBluetooth, EInfo,
            "- L2CapPushServerConnection::getServerObject Returning new L2CAP Server");
        return mL2CAPServer;
    }
}

OS_EXPORT void L2CapPushServerConnection::deleteServer()
{
    JELOG2(EJavaBluetooth);
    if (mL2CAPServer)
    {
        delete mL2CAPServer;
        mL2CAPServer = NULL;
    }
}

OS_EXPORT
BluetoothClientConnection* L2CapPushServerConnection::getConnectedClient()
{
    JELOG2(EJavaBluetooth);
    //NOTE: ISSUE: IS there a chance that there can be multiple clients
    //connecting to the push midlet before launch of midlet?
    //In that case we need to return clients in the order in which they were
    //accepted.
    //Code will be something like return acceptedConnections->deQueue()
    //and in handleConnectionRequest we must
    //have acceptedConnection->enQueue(client);
    //
    BtL2CapServerConnectionFactory& connectionFactory =
        BtL2CapServerConnectionFactory::getFactory();
    connectionFactory.setPendingMsgFlag(mConnectionUri, false);

    // Handed over the accepted connection. And hence, resetting the flag  back
    mPendingConnection = false;

    return mClientConnection;
}

} //end namespace bluetooth
} //end namespace java
