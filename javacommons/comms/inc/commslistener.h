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
* Description:  CommsListener interface
*
*/

#ifndef COMMSLISTENER_H
#define COMMSLISTENER_H

namespace java
{
namespace comms
{

class CommsMessage;

/**
* CommsListener interface
*
* CommsListener provides a way to receive messages asynchronously. Listeners are registered
* to endpoint. When message belonging to listener is received, will Comms endpoint make a
* callback for listener.
*
* Usage:
* Application needs to implement CommsListener interface and register it to CommsEndpoint.
* @code
* class Listener : public CommsListener
* {
*     virtual void processMessage(CommsMessage& aMessage);
* //...
* };
*
* Listener::processMessage(CommsMessage& aMessage)
* {
*     switch(aMessage.getMessageId())
*     {
*       case MESSAGE_ID_A:
*       handleMessageA(aMessage);
*           break;
*       case MESSAGE_ID_B:
*       handleMessageB(aMessage);
*           break;
*       default:
*           break;
*     }
* }
*
* Listener myListener;
* comms.registerListener(MY_MODULE_ID, &myListener);
* @endcode
*
* @see CommsEndpoint, CommsMessage
*/
class CommsListener
{
public:
    /**
    * This method will be called when CommsEndpoint receives message
    * @param[out] aMessage Received message
    * @see CommsEndpoint::send()
    * @see CommsEndpoint::registerListener()
    * @return -
    */
    virtual void processMessage(CommsMessage& aMessage) = 0;

};

} // namespace comms
} // namespace java

#endif // COMMSLISTENER_H
