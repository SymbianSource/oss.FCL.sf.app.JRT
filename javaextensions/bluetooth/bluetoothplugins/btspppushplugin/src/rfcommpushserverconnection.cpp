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


#include "btrfcommpushserverconnection.h"
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

const wchar_t BT_GOEP_PROTOCOL[]=L"btgoep";

OS_EXPORT RFCOMMPushServerConnection::RFCOMMPushServerConnection(
    const wstring aUri, const wstring aFilter,ServerConnectionFactoryBase* aFactory):
        mAcceptMonitor(NULL),
        mConnectionUri(aUri),
        mConnectionFilter(aFilter),
        mRFCOMMServer(NULL),
        mConnectionFactory(aFactory),
        mIsGOEP(false),
        mPendingConnection(false),
        mListening(false),
        mCreatedByPush(false),
        mClearServiceClassBitsFlag(true)
{
    JELOG2(EJavaBluetooth);

    // Create Bluetooth Parameters
    mBtUrlParams = new BtUrlParams(mConnectionUri, mConnectionFilter);
    mFunctionServer = new BluetoothFunctionServer();
    // Create a Server Object and store it as member.
    mRFCOMMServer = new RFCOMMServerConnection(mFunctionServer);
}

OS_EXPORT RFCOMMPushServerConnection::~RFCOMMPushServerConnection()
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
 * 2. Create BtRFCOMMServer and ask it to start listening.
 *
 *
 */
OS_EXPORT void RFCOMMPushServerConnection::open(ConnectionListener* aListener)
{
    JELOG2(EJavaBluetooth);
    mConnectionListener = aListener;

    bool authenticate = mBtUrlParams->getParamAuthenticate();
    bool authorize = mBtUrlParams->getParamAuthorize();
    bool encrypt = mBtUrlParams->getParamEncrypt();

    int error = 0;

    // Open RFCOMM Server.
    error = mRFCOMMServer->openServer(authorize, authenticate, encrypt);
    if (error < 0)
    {
        std::string errTxt("ERROR!!! Unable to Open RFCOMM Server.");
        throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                            __FUNCTION__, __LINE__);
    }

    int channel = mRFCOMMServer->GetRfListeningChannel();

    std::wstring uuid = mBtUrlParams->getServiceUuid();
    std::wstring serviceName = mBtUrlParams->getParamName();

    LOG1(EJavaBluetooth, EInfo,
         "  RFCOMMPushServerConnection::open Protocol:%S",
         mBtUrlParams->getProtocol().c_str());
    if (0 == mBtUrlParams->getProtocol().compare(BT_GOEP_PROTOCOL))
    {
        mIsGOEP = true;
    }
    // Initialize Services
    error = mRFCOMMServer->initializeServiceRecord(
                channel, uuid,serviceName, mIsGOEP);

    if (error < 0)
    {
        std::string errTxt("ERROR!!! Unable to Initialize channel Server.");
        throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                            __FUNCTION__, __LINE__);
    }

    // Restore Persistent Record (if any)
    mRFCOMMServer->restorePersistentRecord();

    // Ask server to start
    mRFCOMMServer->asyncAccept(this, mBtUrlParams);

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
OS_EXPORT void RFCOMMPushServerConnection::unsetClearServiceClassBitsFlag()
{
    JELOG2(EJavaBluetooth);
    mClearServiceClassBitsFlag = false;
}

OS_EXPORT void RFCOMMPushServerConnection::close()
{
    JELOG2(EJavaBluetooth);
    if (true == mClearServiceClassBitsFlag)
    {
        ServiceClassHandler::setDeviceServiceClass(0);
    }
    mPendingConnection = false;
    mAcceptMonitor = NULL;
    mListening = false;
    mRFCOMMServer->CloseServer();
}

/*
 * Gets the connection URI used to open this server connection.
 */
OS_EXPORT std::wstring RFCOMMPushServerConnection::getUri() const
{
    JELOG2(EJavaBluetooth);
    return mConnectionUri;
}

/*
 * Gets the connection Filter string associate with this connection.
 */
OS_EXPORT std::wstring RFCOMMPushServerConnection::getFilter() const
{
    JELOG2(EJavaBluetooth);
    return mConnectionFilter;
}

OS_EXPORT void RFCOMMPushServerConnection::setFilter(
    const std::wstring& aFilter)
{
    JELOG2(EJavaBluetooth);
    mConnectionFilter = aFilter;
}

void RFCOMMPushServerConnection::handleConnectionRequest(
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
    mConnectionFactory->setPendingMsgFlag(mConnectionUri, true);

    std::wstring* remoteDeviceName = NULL;

    BluetoothNameLookup * nameLookup = NULL;

    long long remoteDevAddr = aClientConnection->getRemoteAddress();

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

    // Store this as member. We can later return this when queried from jsr.
    mClientConnection = aClientConnection;

    if (NULL != mAcceptMonitor)
    {
        mAcceptMonitor->notify();
    }
}

OS_EXPORT void RFCOMMPushServerConnection::setAcceptMonitor(
    java::util::Monitor* aMonitor)
{
    mAcceptMonitor = aMonitor;
    mRFCOMMServer->avoidFilter();
}

OS_EXPORT void RFCOMMPushServerConnection::unsetAcceptMonitor()
{
    mAcceptMonitor = NULL;
}

OS_EXPORT bool RFCOMMPushServerConnection::isActive()
{
    JELOG2(EJavaBluetooth);
    bool result = (mPendingConnection || mListening);
    LOG1(EJavaBluetooth, EInfo,
         "+ RFCOMMPushServerConnection::isActive: bool result: %d", result);
    return result;
}

OS_EXPORT bool RFCOMMPushServerConnection::isGOEPConnection()
{
    return mIsGOEP;
}

OS_EXPORT bool RFCOMMPushServerConnection::isConnectionAccepted()
{
    return mPendingConnection;
}

OS_EXPORT bool RFCOMMPushServerConnection::isListening()
{
    return mListening;
}

OS_EXPORT bool RFCOMMPushServerConnection::isCreatedByPush()
{
    return mCreatedByPush;
}

OS_EXPORT void RFCOMMPushServerConnection::setCreatedByPush()
{
    mCreatedByPush = true;
}

OS_EXPORT
RFCOMMServerConnection* RFCOMMPushServerConnection::getServerObject()
{
    JELOG2(EJavaBluetooth);
    if (mRFCOMMServer)
    {
        LOG(EJavaBluetooth,EInfo,
            "-  RFCOMMPushServerConnection::getServerObject Returning existing RFCOMM Server");
        return mRFCOMMServer;
    }
    else
    {
        mRFCOMMServer = new RFCOMMServerConnection(mFunctionServer);
        LOG(EJavaBluetooth, EInfo,
            "-  RFCOMMPushServerConnection::getServerObject Returning new RFCOMM Server");
        return mRFCOMMServer;
    }
}

OS_EXPORT void RFCOMMPushServerConnection::deleteServer()
{
    JELOG2(EJavaBluetooth);
    if (mRFCOMMServer)
    {
        delete mRFCOMMServer;
        mRFCOMMServer = NULL;
        return;
    }
}

OS_EXPORT
BluetoothClientConnection* RFCOMMPushServerConnection::getConnectedClient()
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
    mConnectionFactory->setPendingMsgFlag(mConnectionUri, false);

    // Handed over the accepted connection. And hence, resetting the flag  back
    mPendingConnection = false;

    return mClientConnection;
}

} //end namespace bluetooth
} //end namespace java
