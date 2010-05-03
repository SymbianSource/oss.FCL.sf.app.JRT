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
 * WMA 2.0 module test midlet for receiving push messages.
 */
public class WmaPushStaticReceiver extends MIDlet
{

    /** MessageConnection for receiving messages. */
    private MessageConnection connection = null;

    /** Main form for the midlet. */
    private Form form = null;

    /** Commands for this midlet. */
    private String commands[] = new String[]
    {
        "Open connection",
        "Close connection",
        "Register midlet",
        "Unregister midlet",
        "Receive message",
        "Clear screen",
        "Exit"
    };

    public WmaPushStaticReceiver()
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
        form = new Form("WMA 2.0 push static receiver MT midlet");
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
                    openConnection();
                }
                else if (c.getLabel().equals(commands[1]))
                {
                    closeConnection();
                }
                else if (c.getLabel().equals(commands[2]))
                {
                    registerMidlet();
                }
                else if (c.getLabel().equals(commands[3]))
                {
                    unregisterMidlet();
                }
                else if (c.getLabel().equals(commands[4]))
                {
                    receiveMms();
                }
                else if (c.getLabel().equals(commands[5]))
                {
                    form.deleteAll();
                }
                else if (c.getLabel().equals(commands[6]))
                {
                    try
                    {
                        destroyApp(false);
                        notifyDestroyed();
                    }
                    catch (MIDletStateChangeException msce)
                    {
                        displayMessage("Exception during exit: " + msce.toString());
                    }
                }
            }
        });
        Display.getDisplay(this).setCurrent(form);

        WmaPushUtils.initUrls(this, form);

        // Check if we were started by incoming messages.
        String connections[] = PushRegistry.listConnections(true);
        if (connections.length > 0)
        {
            // Open connection and set listener.
            openConnection();
        }
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
        if (connection != null)
        {
            closeConnection();
        }
    }

    /**
     * Registers midlet to push registry, opens connection, and
     * registers message listener.
     */
    private void registerMidlet()
    {
        try
        {
            // Register midlet to push registry.
            PushRegistry.registerConnection(WmaPushUtils.serverConnectionUrlStatic,
                                            "wma.mt.pushtests.WmaPushStaticReceiver", WmaPushUtils.senderMsisdn);
            displayMessage("Midlet registered");
        }
        catch (Exception e)
        {
            displayMessage("Midlet registration failed: " + e.toString());
        }
    }

    /**
     * Unregisters midlet from push registry and closes connection.
     */
    private void unregisterMidlet()
    {
        try
        {
            // Unregister midlet from push registry.
            boolean result =
                PushRegistry.unregisterConnection(WmaPushUtils.serverConnectionUrlStatic);
            if (result)
            {
                displayMessage("Midlet unregistered");
            }
            else
            {
                displayMessage("Midlet was not unregistered");
            }
        }
        catch (Exception e)
        {
            displayMessage("Midlet unregistration failed: " + e.toString());
        }
    }

    /**
     * Opens connection.
     */
    private void openConnection()
    {
        if (connection == null)
        {
            try
            {
                connection = (MessageConnection)Connector.open(
                                 WmaPushUtils.serverConnectionUrlStatic);
                displayMessage("Connection opened");
            }
            catch (Exception e)
            {
                displayMessage("Connection opening failed: " + e.toString());
            }

            if (connection != null)
            {
                try
                {
                    // Set message listener.
                    connection.setMessageListener(new MsgListener());
                    displayMessage("Listener set");
                }
                catch (Exception e)
                {
                    displayMessage("Setting message listener failed: " + e.toString());
                }
            }
        }
        else
        {
            displayMessage("Connection already open");
        }
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
                displayMessage("Connection closed");
            }
            catch (Exception e)
            {
                displayMessage("Connection closing failed: " + e.toString());
            }
        }
        else
        {
            displayMessage("Connection already closed");
        }
    }

    /**
     * Receives a message.
     */
    private void receiveMms()
    {
        if (connection == null)
        {
            openConnection();
        }

        try
        {
            MultipartMessage msg = (MultipartMessage)connection.receive();
            checkMsg(msg);
        }
        catch (Exception e)
        {
            displayMessage("Message receiving failed: " + e.toString());
        }
    }

    /**
     * Returns null if given message has expected content, the expected
     * message otherwise.
     */
    private MultipartMessage validMsg(MultipartMessage receivedMsg)
    {
        MultipartMessage sentMsg =
            WmaPushUtils.getTestMessage(
                connection, WmaPushUtils.clientConnectionUrlStatic);
        if (WmaPushUtils.messagesEqual(sentMsg, receivedMsg,
                                       WmaPushUtils.clientConnectionUrlStatic, "mms://" + WmaPushUtils.receiverMsisdn))
        {
            return null;
        }
        return sentMsg;
    }

    /**
     * Checks that the received message has the expected contents.
     */
    private void checkMsg(MultipartMessage msg)
    {
        MultipartMessage sentMsg = validMsg(msg);
        if (sentMsg == null)
        {
            displayMessage("Received valid message");
        }
        else
        {
            displayMessage("Received invalid message:\n"
                           + WmaPushUtils.msgToString(msg)
                           + "\nexpected:\n"
                           + WmaPushUtils.msgToString(sentMsg));
        }
    }

    /**
     * Simple implementation of the MessageListener interface.
     */
    class MsgListener implements MessageListener
    {
        public void notifyIncomingMessage(MessageConnection connection)
        {
            try
            {
                MultipartMessage msg = (MultipartMessage)connection.receive();
                checkMsg(msg);
            }
            catch (Exception e)
            {
                displayMessage("Message receiving failed: " + e.toString());
            }
        }
    }

    private void displayMessage(String msg)
    {
        form.append(msg + "\n");
        WmaPushUtils.wmaDebug(msg);
    }
}
