/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PENDINGCONNECTIONLISTENER_H
#define PENDINGCONNECTIONLISTENER_H

#include <string>

namespace java
{
namespace push
{
/**
 * Server Connection Plugin informs Push Controller whether connection waits handing.
 * This interface is needed to support functionality of PushRegistry.listConnection() operation
 * in midp2.1 spec. This interface can be removed when implementation supports midp3 spec.
 */

class PendingConnectionListener
{
public:

    virtual ~PendingConnectionListener() {}

    /**
     * This operation must be called when message arrives to the connection and
     * when application has opened a ServerConnection to read a arrived message.
     * Note: This operation must be called before ConnectionListener::msgArrived()
     * operation.
     * @param aUri        URI of the connection.
    * @param aMsgWaitsHandling Logic of this flag:
    *                    - Set it 'true' when message arrives to the connection.
    *                    - Set it 'false' when ServerConnection::open() operation has been called.
     */
    virtual void setPendingStatus(const std::wstring& aUri,bool aMsgWaitsHandling) = 0;

};


}//end namespace push
}//end namespace java

#endif // PENDINGCONNECTIONLISTENER_H
