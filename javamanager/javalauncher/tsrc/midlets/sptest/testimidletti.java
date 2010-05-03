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
* Description: test midlet that shows the command line passed to it
*/



import javax.microedition.io.ConnectionNotFoundException;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.StringItem;
//import javax.microedition.midlet.*;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

public class testimidletti extends MIDlet
{

    Form _form = new Form("formi");
    String _instance = "0";

    public testimidletti()
    {

    }
    protected void startApp() throws MIDletStateChangeException
    {
        String instance = System.getProperty("com.nokia.mid.cmdline.instance");

        if (instance == _instance)
        {
            // No relaunch -> no new command line parameters
            return;
        }
        _instance = instance;

        System.out.println("com.nokia.mid.cmdline.instance="               +System.getProperty("com.nokia.mid.cmdline.instance"));
        System.out.println("com.nokia.mid.cmdline="                        +System.getProperty("com.nokia.mid.cmdline"));
        System.out.println("com.nokia.mid.cmdline.param.mode="             +System.getProperty("com.nokia.mid.cmdline.param.mode"));
        System.out.println("com.nokia.mid.cmdline.param.difficultY="       +System.getProperty("com.nokia.mid.cmdline.param.difficultY"));
        System.out.println("com.nokia.mid.cmdline.param.sounds="           +System.getProperty("com.nokia.mid.cmdline.param.sounds"));
        System.out.println("com.nokia.mid.cmdline.param.wizard_mode="      +System.getProperty("com.nokia.mid.cmdline.param.wizard_mode"));
        System.out.println("com.nokia.mid.cmdline.param.non_existing="     +System.getProperty("com.nokia.mid.cmdline.param.non_existing"));


        _form.append("com.nokia.mid.cmdline.instance="               +System.getProperty("com.nokia.mid.cmdline.instance"));
        _form.append("com.nokia.mid.cmdline="                        +System.getProperty("com.nokia.mid.cmdline"));
        _form.append("com.nokia.mid.cmdline.param.mode="             +System.getProperty("com.nokia.mid.cmdline.param.mode"));


        Display.getDisplay(this).setCurrent(_form);

    }

    protected void pauseApp()
    {
    }

    protected void destroyApp(boolean p1) throws MIDletStateChangeException
    {
    }


};
