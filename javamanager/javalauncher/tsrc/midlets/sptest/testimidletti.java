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
        System.out.println("mode="             +System.getProperty("mode"));
        System.out.println("difficultY="       +System.getProperty("difficultY"));
        System.out.println("sounds="           +System.getProperty("sounds"));
        System.out.println("wizard_mode="      +System.getProperty("wizard_mode"));
        System.out.println("non_existing="     +System.getProperty("param.non_existing"));


        _form.append("com.nokia.mid.cmdline.instance="               +System.getProperty("com.nokia.mid.cmdline.instance"));
        _form.append("com.nokia.mid.cmdline="                        +System.getProperty("com.nokia.mid.cmdline"));
        _form.append("mode="             +System.getProperty("mode"));


        Display.getDisplay(this).setCurrent(_form);

    }

    protected void pauseApp()
    {
    }

    protected void destroyApp(boolean p1) throws MIDletStateChangeException
    {
    }

};
