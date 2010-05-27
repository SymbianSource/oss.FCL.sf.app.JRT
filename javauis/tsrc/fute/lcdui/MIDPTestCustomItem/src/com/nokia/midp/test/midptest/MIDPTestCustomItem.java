/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.midp.test.midptest;

import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import javax.microedition.io.*;
import javax.microedition.media.*;
import javax.microedition.media.control.*;
import java.io.*;

/**
 * Tests CustomItem repaint.
 *
 */
public class MIDPTestCustomItem
        extends MIDlet
        implements CommandListener
{

    private Form iForm;
    private Command iExitCommand = new Command("Exit", Command.EXIT, 2);
    private Command iAppendCommand = new Command("append", Command.SCREEN, 1);

    private Display iDisplay;// The display for this MIDlet
    private VideoItem iVideoItemCommand;
    private VideoItem iVideoItem;

    private TestTimer iVideoItemCommandTimer;
    private TestTimer iVideoItemTimer;

    /**
     * Constructs the MIDlet instance.
     */
    public MIDPTestCustomItem()
    {
        iDisplay = Display.getDisplay(this);

        iForm = new Form("CustomItem test");

        iForm.addCommand(iExitCommand);
        iForm.addCommand(iAppendCommand);

        iForm.setCommandListener(this);

        iVideoItem = new VideoItem();

        iForm.append(iVideoItem);

        iVideoItemCommand = new VideoItem();

        iVideoItemCommandTimer = new TestTimer(iVideoItem);
        iVideoItemTimer = new TestTimer(iVideoItemCommand);
    }


    public void startApp()
    {
        iDisplay.setCurrent(iForm);
    }


    public void pauseApp()
    {
        System.out.println("INFO: pauseApp()");
    }


    public void destroyApp(boolean unconditional)
    {
        System.out.println("INFO: destroyApp()");
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == iExitCommand)
        {
            destroyApp(false);
            notifyDestroyed();
        }
        else if (c == iAppendCommand)
        {
            iForm.append(iVideoItemCommand);
        }
    }

}

