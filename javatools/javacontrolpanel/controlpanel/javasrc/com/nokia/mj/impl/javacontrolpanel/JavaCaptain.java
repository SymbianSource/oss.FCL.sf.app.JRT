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

package com.nokia.mj.impl.javacontrolpanel;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.comms.*;
import com.nokia.mj.impl.comms.exception.*;


class JavaCaptain
{
    private final static int PLUGIN_ID_JAVACAPTAIN_CORE_C = 13;
    private final static int CORE_MSG_ID_STOP_JAVACAPTAIN = 130;
    private final static int CORE_MSG_ID_DO_THREAD_DUMP = 131;
    private final static int CORE_MSG_ID_START_PREWARM = 132;
    private final static int CORE_MSG_ID_STOP_PREWARM = 133;
    private final static int CORE_MSG_ID_GET_PREWARM = 134;

    private final static int PLUGIN_ID_RTC_C = 1;
    private final static int RTC_MSG_ID_SET_EXTRA_ARGUMENTS = 150;
    private final static int RTC_MSG_ID_GET_EXTRA_ARGUMENTS = 151;

    private final static String JVM_DEBUG_ARGS = "-Xrunjdwp:server=y,address=localhost:8000";

    public JavaCaptain()
    {
    }

    public boolean isRunning()
    {
        CommsEndpoint comms = new CommsEndpoint();
        boolean isRunning = false;
        try
        {
            comms.connect(CommsEndpoint.JAVA_CAPTAIN);
            comms.disconnect();
            isRunning = true;
        }
        catch (CommsException e)
        {
            e.printStackTrace();
        }
        finally
        {
            comms.destroy();
        }
        return isRunning;
    }

    public void start()
    {
        _start();
        // wait until JC message loop is up
        for(int i = 0; i < 5; i++)
        {
            try
            {
                Thread.sleep(30);
            }
            catch(InterruptedException e) {}
            if (isRunning())
            {
                break;
            }
        }
    }

    public void stop()
    {
        CommsMessage message = createMessage();
        message.setMessageId(CORE_MSG_ID_STOP_JAVACAPTAIN);
        // waits until JC message loop is down
        sendReceive(message);
    }

    public boolean isPrewarmEnabled()
    {
        CommsMessage message = createMessage();
        message.setMessageId(CORE_MSG_ID_GET_PREWARM);
        CommsMessage reply = sendReceive(message);
        int enabled = reply.readInt();
        return enabled == 1 ? true : false;
    }

    public void enablePrewarm(boolean aEnable)
    {
        CommsMessage message = createMessage();
        if (aEnable)
        {
            message.setMessageId(CORE_MSG_ID_START_PREWARM);
        }
        else
        {
            message.setMessageId(CORE_MSG_ID_STOP_PREWARM);
        }
        send(message);
    }

    public void doThreadDump()
    {
        CommsMessage message = createMessage();
        message.setMessageId(CORE_MSG_ID_DO_THREAD_DUMP);
        send(message);
    }

    public boolean isDebugEnabled()
    {
        CommsMessage message = createDebugMessage();
        message.setMessageId(RTC_MSG_ID_GET_EXTRA_ARGUMENTS);
        CommsMessage receivedMessage = sendReceive(message);
        String vmArgs = receivedMessage.readString();
        boolean enabled = false;
        if(vmArgs.compareTo(JVM_DEBUG_ARGS) == 0)
        {
            enabled = true;
        }
        return enabled;
    }

    public void enableDebug(boolean aEnable)
    {
        String vmArgs = "";
        if (aEnable)
        {
            vmArgs = JVM_DEBUG_ARGS;
        }

        CommsMessage message = createDebugMessage();
        message.setMessageId(RTC_MSG_ID_SET_EXTRA_ARGUMENTS);
        message.write(vmArgs);
        message.write(""); // appArgs
        send(message);
    }

    private void send(CommsMessage aMessage)
    {
        CommsEndpoint comms = new CommsEndpoint();
        try
        {
            comms.connect(CommsEndpoint.JAVA_CAPTAIN);
            comms.send(aMessage);
            comms.disconnect();
        }
        catch (CommsException e)
        {
            e.printStackTrace();
        }
        finally
        {
            comms.destroy();
        }
    }

    private CommsMessage sendReceive(CommsMessage aMessage)
    {
        CommsEndpoint comms = new CommsEndpoint();
        CommsMessage receivedMessage = new CommsMessage();
        try
        {
            comms.connect(CommsEndpoint.JAVA_CAPTAIN);
            receivedMessage = comms.sendReceive(aMessage, CommsEndpoint.WAIT_FOR_EVER);
            comms.disconnect();
        }
        catch (CommsException e)
        {
            e.printStackTrace();
        }
        finally
        {
            comms.destroy();
        }
        return receivedMessage;
    }

    private CommsMessage createMessage()
    {
        CommsMessage message = new CommsMessage();
        message.setModuleId(PLUGIN_ID_JAVACAPTAIN_CORE_C);
        return message;
    }

    private CommsMessage createDebugMessage()
    {
        CommsMessage message = new CommsMessage();
        message.setModuleId(PLUGIN_ID_RTC_C);
        return message;
    }

    private native void _start();

}
