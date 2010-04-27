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
* Description: test midlet for making platform requests
*/

import javax.microedition.io.ConnectionNotFoundException;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.MIDlet;

public class platRequest extends MIDlet
        implements CommandListener
{

    public platRequest()
    {
        lstMenu = new List("Select req", 1, strMenu, null);
        nextCommand = new Command("Next", 1, 1);
        exitCommand = new Command("Exit", 1, 1);
        urlTB = new TextBox("", "", 256, 4);
        String as[] =
        {
            getAppProperty("platreq_0002a"), getAppProperty("platreq_0002b"), getAppProperty("platreq_0003"), getAppProperty("platreq_0004"), getAppProperty("platreq_0005"), getAppProperty("platreq_0006"), getAppProperty("platreq_0007"), getAppProperty("platreq_0008"), getAppProperty("platreq_0009"), getAppProperty("platreq_0010"),
            getAppProperty("platreq_0012"), getAppProperty("platreq_0013"), getAppProperty("platreq_0014"), getAppProperty("platreq_0015")
        };
        strUrl = as;
        urlAlert = new Alert("Alert", "", null, AlertType.WARNING);
        display = Display.getDisplay(this);
        urlTB.addCommand(nextCommand);
        urlTB.addCommand(exitCommand);
        urlTB.setCommandListener(this);
        lstMenu.addCommand(nextCommand);
        lstMenu.addCommand(exitCommand);
        lstMenu.setCommandListener(this);
        display.setCurrent(lstMenu);
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
        if (command == exitCommand)
        {
            destroyApp(true);
            notifyDestroyed();
        }
        else if (command == nextCommand)
            if (displayable == lstMenu)
            {
                urlTB.setTitle(strMenu[lstMenu.getSelectedIndex()]);
                urlTB.setString(strUrl[lstMenu.getSelectedIndex()]);
                display.setCurrent(urlTB);
            }
            else if (displayable == urlTB)
                try
                {
                    if (platformRequest(urlTB.getString()))
                    {
                        destroyApp(true);
                        notifyDestroyed();
                    }
                    else
                    {
                        urlAlert.setString("Req executed");
                        display.setCurrent(urlAlert, urlTB);
                    }
                }
                catch (ConnectionNotFoundException connectionnotfoundexception)
                {
                    urlAlert.setString("Error: " + connectionnotfoundexception.getMessage());
                    display.setCurrent(urlAlert, urlTB);
                }
                catch (Exception exception) { }
    }

    String strMenu[] =
    {
        "platreq_0002a", "platreq_0002b", "platreq_0003", "platreq_0004", "platreq_0005", "platreq_0006", "platreq_0007", "platreq_0008", "platreq_0009", "platreq_0010",
        "platreq_0012", "platreq_0013", "platreq_0014", "platreq_0015"
    };
    String strUrl[];
    List lstMenu;
    Command nextCommand;
    Command exitCommand;
    public Display display;
    TextBox urlTB;
    String browURL;
    String telNo;
    String dlURL;
    String upURL;
    String dwURL;
    Alert urlAlert;
}
