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
package com.nokia.mj.test.wma;

import javax.microedition.io.Connector;
import javax.microedition.io.PushRegistry;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

import javax.wireless.messaging.Message;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessageListener;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;

/**
 * WMA 2.0 module test midlet for sending messages.
 */
public class WmaPushSender extends MIDlet
{

    /** MessageConnection for sending messages. */
    private MessageConnection connection = null;

    /** MessageConnection for sending messages to static midlet. */
    private MessageConnection connectionStatic = null;

    /** Main form for the midlet. */
    private Form form = null;

    /** Commands for this midlet. */
    private String commands[] = new String[]
    {
        "Send message",
        "Send message to static midlet",
        "Clear screen",
        "Exit"
    };

    public WmaPushSender()
    {
        super();
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.microedition.midlet.MIDlet#startApp()
     */
    protected void startApp() throws MIDletStateChangeException
    {
        form = new Form("WMA 2.0 push sender MT midlet");
        for (int i = 0; i < commands.length; i++)
        {
            form.addCommand(new Command(commands[i], Command.ITEM, 1));
        }
        form.setCommandListener(new CommandListener()
        {
            public void commandAction(Command c, Displayable d)
            {
                if (c.getLabel().equals(commands[0]))
                {
                    sendMms();
                }
                else if (c.getLabel().equals(commands[1]))
                {
                    sendMmsToStatic();
                }
                else if (c.getLabel().equals(commands[2]))
                {
                    form.deleteAll();
                }
                else if (c.getLabel().equals(commands[3]))
                {
                    try
                    {
                        destroyApp(false);
                        notifyDestroyed();
                    }
                    catch (MIDletStateChangeException msce)
                    {
                        displayMessage("Exception during exit: "
                                       + msce.toString());
                    }
                }
            }
        });
        Display.getDisplay(this).setCurrent(form);

        WmaPushUtils.initUrls(this, form);
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.microedition.midlet.MIDlet#pauseApp()
     */
    protected void pauseApp()
    {
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.microedition.midlet.MIDlet#destroyApp(boolean)
     */
    protected void destroyApp(boolean unconditional)
    throws MIDletStateChangeException
    {
        closeConnection();
    }

    /**
     * Closes connection.
     */
    private void closeConnection()
    {
        if (connection != null)
        {
            try
            {
                // Close connection.
                connection.close();
                connection = null;
            }
            catch (Exception e)
            {
                displayMessage("Closing connection failed: "
                               + e.toString());
            }
        }
        if (connectionStatic != null)
        {
            try
            {
                // Close connection.
                connectionStatic.close();
                connectionStatic = null;
            }
            catch (Exception e)
            {
                displayMessage("Closing connection to static midlet failed: "
                               + e.toString());
            }
        }
    }

    /**
     * Sends a message.
     */
    private void sendMms()
    {
        if (connection == null)
        {
            try
            {
                connection = (MessageConnection)Connector.open(
                                 WmaPushUtils.clientConnectionUrl);
                displayMessage("Connection opened");
            }
            catch (Exception e)
            {
                displayMessage("Connection opening failed: "
                               + e.toString());
            }
        }

        try
        {
            MultipartMessage msg =
                WmaPushUtils.getTestMessage(
                    connection, WmaPushUtils.clientConnectionUrl);
            connection.send(msg);
            displayMessage("Message sent");
        }
        catch (Exception e)
        {
            displayMessage("Message sending failed: " + e.toString());
        }
    }

    /**
     * Sends a message to static midlet.
     */
    private void sendMmsToStatic()
    {
        if (connectionStatic == null)
        {
            try
            {
                connectionStatic = (MessageConnection)Connector.open(
                                       WmaPushUtils.clientConnectionUrlStatic);
                displayMessage("Connection to static midlet opened");
            }
            catch (Exception e)
            {
                displayMessage("Connection opening to static midlet failed: "
                               + e.toString());
            }
        }

        try
        {
            MultipartMessage msg =
                WmaPushUtils.getTestMessage(
                    connectionStatic, WmaPushUtils.clientConnectionUrlStatic);
            connectionStatic.send(msg);
            displayMessage("Message sent to static midlet");
        }
        catch (Exception e)
        {
            displayMessage("Message sending to static midlet failed: "
                           + e.toString());
        }
    }

    private void displayMessage(String msg)
    {
        form.append(msg + "\n");
        WmaPushUtils.wmaDebug(msg);
    }
}
