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
* Description: test midlet for making platform requests
*/

import javax.microedition.io.ConnectionNotFoundException;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.MIDlet;

public class Launcher extends MIDlet
        implements CommandListener
{
    private List        mMainView;
    private Command     mExitCommand;
    private static String mUri = "localapp://jam/launch?midlet-name=LocalAppSuite;midlet-vendor=Nokia;";
    private static String mMidlet = "midlet-n=";
    private static String mParams = "sounds=ON;difficulty=medium;wizard_mode;microedition.locale=my.locale;com.test.notVisible=com.test.notVisible=should not be seen";
    private static String mParams2 = "sounds=OFF;difficulty=low;wizard_mode=yes;microedition.locale=my.locale2;com.test.extra=extra;com.test.notVisible=com.test.notVisible=should not be seen";
    private boolean mUse2 = false;
    public Launcher()
    {
        String[] elements = {"Start default", "Start 1", "Start 2", "Start 3", "Start 4", "Start 5"};
        mMainView = new List("Select MIDlet", List.IMPLICIT, elements, null);
        mExitCommand = new Command("Exit", 1, 1);
        mMainView.addCommand(mExitCommand);
        mMainView.setCommandListener(this);
        Display.getDisplay(this).setCurrent(mMainView);
    }

    public void startApp()
    {
    }

    public void pauseApp()
    {
    }

    public void destroyApp(boolean flag)
    {
    }

    public void commandAction(Command command, Displayable displayable)
    {
        if (command == mExitCommand)
        {
            destroyApp(true);
            notifyDestroyed();
        }
        else
        {
            String params = mUse2 ? mParams2 : mParams;
            switch (mMainView.getSelectedIndex())
            {
                case 0:
                    sendPlatReq(mUri + params);
                    break;
                case 1:
                    sendPlatReq(mUri + mMidlet + "1;" + params);
                    break;
                case 2:
                    sendPlatReq(mUri + mMidlet + "2;" + params);
                    break;
                case 3:
                    sendPlatReq(mUri + mMidlet + "3;" + params);
                    break;
                case 4:
                    sendPlatReq(mUri + mMidlet + "4;" + params);
                    break;
                case 5:
                    sendPlatReq(mUri + mMidlet + "5;" + params);
                    break;
            }
            mUse2 = !mUse2;
        }
    }
    private void sendPlatReq(String uri)
    {
        System.err.println("Plat req: " + uri);
        try
        {
            if (platformRequest(uri))
//            if (false)
            {
                destroyApp(true);
                notifyDestroyed();
            }
            else
            {
                System.err.println("  ok");
            }
        }
        catch (Exception e)
        {
            System.err.println("Plat req failed");
            e.printStackTrace();
        }
    }
}
