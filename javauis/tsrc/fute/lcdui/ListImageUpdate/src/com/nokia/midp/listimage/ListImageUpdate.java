/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.midp.listimage;
/**
 * import midp classes.
 */
import javax.microedition.midlet.*;
/**
 * This is the main class for List and image update testing midlet.
 */

public class ListImageUpdate extends MIDlet
{
    /**
     * Signals the MIDlet to start and enter the Active state.
     */
    protected void startApp()
    {
        new ListTest(this);
    }
    /**
     *  Signals the MIDlet to terminate and enter the Destroyed state.
     *
     */
    protected void destroyApp(boolean unconditional)
    {
    }
    /**
     *  Signals the MIDlet to stop and enter the Paused state.
     */
    protected void pauseApp()
    {
    }
}

