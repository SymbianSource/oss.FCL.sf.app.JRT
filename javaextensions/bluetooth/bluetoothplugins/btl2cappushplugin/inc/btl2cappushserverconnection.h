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


#ifndef BTL2CAPPUSHSERVERCONNECTION_H
#define BTL2CAPPUSHSERVERCONNECTION_H

#include <string>
#include <dirent.h>
#include <sys/types.h>

#include "logger.h"
#include "javaoslayer.h"
#include "javacommonutils.h"

#include "serverconnection.h"
#include "connectionlistener.h"

#include "bluetoothfunctionserver.h"
#include "bluetoothpusheventlistener.h"
#include "bluetoothclientconnection.h"
#include "btl2capserverconnection.h"

#include "bturlparams.h"

namespace java
{
namespace bluetooth
{

/**
 * L2CapPushServerConnection implements ServerConnection. Used by Push framework.
 *
 */
class L2CapPushServerConnection: public java::push::ServerConnection,
        public BluetoothPushEventListener
{

public:
    OS_IMPORT L2CapPushServerConnection(const std::wstring aUri,
                                        const std::wstring aFilter);

    OS_IMPORT virtual ~L2CapPushServerConnection();

    /**
     * This operation opens physical protocol connection.
     * @param aListener This operation does not take ownership of this pointer.
     * @throws PushException with COMMON_SRV_CONN_PLUGIN_ERROR error code if
     *         creation of the connection fails.
     */
    OS_IMPORT virtual void open(java::push::ConnectionListener* aListener);

    /**
     * This operation closes physical protocol connection.
     * This operation does not throw an exception.
     */
    OS_IMPORT virtual void close();

    /**
     * @return connection URI.
     */
    OS_IMPORT virtual std::wstring getUri() const;

    /**
     * @return filter URI of the push connection. Empty string is returned in the normal
     *         ServerConnection case.
     */
    OS_IMPORT virtual std::wstring getFilter() const;

    /**
     * Sets 'filter' argument for the connection. This method is needed in the situation where
     * 'normal' ServerConnection has been created before dynamic push registration.
     */
    OS_IMPORT virtual void setFilter(const std::wstring& aFilter);

    virtual void handleConnectionRequest(
        BluetoothClientConnection* aClientConnection, TInt err);

    OS_IMPORT bool isActive();

    OS_IMPORT bool isConnectionAccepted();

    OS_IMPORT bool isListening();

    OS_IMPORT bool isCreatedByPush();

    OS_IMPORT L2CAPServerConnection* getServerObject();

    OS_IMPORT void deleteServer();

    OS_IMPORT BluetoothClientConnection* getConnectedClient();

    OS_IMPORT void setAcceptMonitor(java::util::Monitor* aMonitor);

    OS_IMPORT void setCreatedByPush();

    OS_IMPORT void unsetAcceptMonitor();

    OS_IMPORT void unsetClearServiceClassBitsFlag();

private:
    java::util::Monitor* mAcceptMonitor;

    java::push::ConnectionListener* mConnectionListener;

    std::wstring mConnectionUri;

    std::wstring mConnectionFilter;

    // Holds the URL/Filter parameters
    BtUrlParams* mBtUrlParams;

    // Bluetooth Function Server. To be used for async calls.
    BluetoothFunctionServer* mFunctionServer;

    // Reference to Server object.
    L2CAPServerConnection* mL2CAPServer;

    // Holds reference to Client connection on successful push invocation.
    BluetoothClientConnection* mClientConnection;

    // Keeps track if current object caused the application to start
    bool mPendingConnection;

    // In case application starts while listening to push.
    bool mListening;
    // Keeps track of current object is created by push.
    bool mCreatedByPush;

    // If set, then it will clear the Service class bits at the time of close()
    bool mClearServiceClassBitsFlag;
};

} // namespace java
} // namespace bluetooth

#endif // BTL2CAPPUSHSERVERCONNECTION_H

