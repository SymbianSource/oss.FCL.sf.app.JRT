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

package com.nokia.mj.impl.comms;

/**
 * CommsListener interface
 *
 * CommsListener provides a way to receive messages asynchronously. Listeners are registered
 * to endpoint. When message belonging to listener is received, will Comms endpoint make a
 * callback for listener.
 * <p>
 * Example how to use CommsListener class:
 *
 * <pre>
 * <code>
 *       class MyListener implements CommsListener {
 *            public void processMessage(CommsMessage aMessage) {
 *                switch(aMessage.getMessageId())
 *                {
 *                case MESSAGE_ID_A:
 *                    handleMessageA(aMessage);
 *                    break;
 *                case MESSAGE_ID_B:
 *                    handleMessageB(aMessage);
 *                    break;
 *                default:
 *                    break;
 *                }
 *            }
 *       }
 *
 *       MyListener listener = new MyListener();
 *
 *       comms.registerListener(MODULE_ID, listener);
 *       ...
 *       comms.unregisterListener(MODULE_ID, CommsEndpoint.JAVA_CAPTAIN_C, listener);
 * </code>
 * </pre>
 */

public interface CommsListener
{

    /**
     * This method will be called when CommsEndpoint receives message
     *
     * @param aMessage
     *            the received message
     * @see CommsEndpoint
     * @see CommsMessage
     */
    public void processMessage(CommsMessage aMessage);
}
