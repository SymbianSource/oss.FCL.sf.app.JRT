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
 * Classes which want to listen command events should implement this interface.
 */
public interface CommandListener
{

    /**
     * Called by underlying system when event occurs.
     *
     * @param command a Command that occurred.
     * @param displayable The Displayable where event occurred.
     */
    void commandAction(Command command, Displayable displayable);

}
