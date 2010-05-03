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
package javax.microedition.lcdui;

/**
 * Listener which receives events from commands associated with item.
 */
public interface ItemCommandListener {

    /**
     * Called when event occurred.
     * 
     * @param command Command which invoked the event.
     * @param item Item containing the command.
     */
    void commandAction(Command command, Item item);

}
