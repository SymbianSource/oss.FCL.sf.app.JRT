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


#ifndef BLUETOOTHPUSHEVENTLISTENER_H
#define BLUETOOTHPUSHEVENTLISTENER_H

namespace java
{
namespace bluetooth
{

class BluetoothClientConnection;

/**
 * BluetoothPushEventListener should be implemented by an object that needs to
 * handle different Push events.
 */
class BluetoothPushEventListener
{
public:
    /**
     * Ideally, handleConnectionRequest must check for incoming connections
     * against the URL parameters and the filter specified by the connection.
     */
    virtual void handleConnectionRequest(BluetoothClientConnection *aBtClient,
                                         TInt err) = 0;
};

} //end namespace bluetooth
} //end namespace java
#endif // BLUETOOTHPUSHEVENTLISTENER_H
