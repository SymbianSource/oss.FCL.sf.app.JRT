/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class asks from Displayable is midlet foreground or not.
 *
*/


// PACKAGE
package com.nokia.microedition.volumekeys;

// IMPORTS
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;
import com.nokia.mj.impl.rt.legacy.LegacyRtPort;

// CLASS DESCRIPTION
/**
 * Class which asks the current of the running MIDlet
 */
public class LCDUIForegroundListener
{
    /**
     * Constructor
     */
    public LCDUIForegroundListener()
    {
    }
    /*
     * Returns the status of the current MIDlet. Use UIToolkitRegistry
     * when possible.
     */
    public boolean isForeground()
    {
        MIDlet midlet = LegacyRtPort.getMidlet();
        Displayable displayable = Display.getDisplay(midlet).getCurrent();
        return displayable.isShown();
    }
}

// End of File
