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
* Description: RecordStoreObserver
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import java.util.Hashtable;
import com.nokia.mj.impl.comms.*;
import com.nokia.mj.impl.comms.exception.*;
import com.nokia.mj.impl.utils.Logger;

/**
 * RecordStoreObserver observes record store changes made by other MIDlets and
 * notifies other MIDlets when changes are made locally
 */
class RecordStoreObserver implements CommsListener
{
    private CommsEndpoint iComms;
    Hashtable iListeners = new Hashtable();
    Hashtable iStoreObservers = new Hashtable();

    public static final int PLUGIN_ID_RMS_C = 2;
    public static final int MSG_ID_RECORD_CHANGE = 1;
    public static final int MSG_ID_LISTENER_CHANGE = 2;
    public static final int MSG_ID_LISTENER_COUNT = 3;

    public static final int ADD_LISTENER = 0;
    public static final int REMOVE_LISTENER = 1;

    public static final int ADD_RECORD = 0;
    public static final int SET_RECORD = 1;
    public static final int DELETE_RECORD = 2;

    private static final int REPLY_TIMEOUT = 2;

    public RecordStoreObserver()
    {
        try
        {
            // use MIDP runtime's connection to JC
            iComms = CommsEndpoint.find("javacaptain");
            if (iComms == null)
            {
                // some unit tests are run within installer main
                iComms = CommsEndpoint.find("InstallerJavaCaptain");
                Logger.WLOG(Logger.EMidpRms, "RecordStoreObserver: using 'InstallerJavaCaptain'");
            }

            if (iComms != null)
            {
                iComms.registerListener(PLUGIN_ID_RMS_C, this);
            }
        }
        catch (CommsException e)
        {
            iComms = null;
            Logger.WLOG(Logger.EMidpRms, "registerListener() failed", e);
        }
    }

    public void addObserver(String aName, RecordListeners aListeners)
    {
        iListeners.put(aName, aListeners);
        CommsMessage msg = createAddListenerMsg(aName);
        // use blocking send so that observer count is known
        syncSend(msg);
    }

    public void removeObserver(String aName)
    {
        iListeners.remove(aName);
        iStoreObservers.remove(aName);
        CommsMessage msg = createRemoveListenerMsg(aName);
        send(msg);
    }

    public void processMessage(CommsMessage aMessage)
    {
        switch (aMessage.getMessageId())
        {
        case MSG_ID_RECORD_CHANGE:
        {
            String storeName = aMessage.readString();
            int operation = aMessage.readInt();
            int recordId = aMessage.readInt();
            RecordListeners listeners = (RecordListeners)iListeners.get(storeName);
            if (listeners != null)
            {
                notifyListeners(listeners, operation, recordId);
            }
            else
            {
                Logger.WLOG(Logger.EMidpRms, "MSG_ID_RECORD_CHANGE invalid name:" + storeName);
            }
            break;
        }
        case MSG_ID_LISTENER_COUNT:
        {
            String storeName = aMessage.readString();
            int observerCount = aMessage.readInt();
            iStoreObservers.put(storeName, new Integer(observerCount));
            break;
        }
        default:
            break;
        }
    }

    public void notifyObservers(String aName, int aOperation, int aRecordId)
    {
        int count = getObserverCount(aName);
        if (count > 1)
        {
            // there is another observer besides ourself so notification is needed
            CommsMessage msg = createRecordChangedMsg(aName, aOperation, aRecordId);
            send(msg);
        }
    }

    public int getObserverCount(String aName)
    {
        Integer observerCount = (Integer)iStoreObservers.get(aName);
        if (observerCount != null)
        {
            return observerCount.intValue();
        }
        return 0;
    }

    private void notifyListeners(RecordListeners aListeners, int aOperation, int aRecordId)
    {
        switch (aOperation)
        {
        case ADD_RECORD:
            aListeners.recordAdded(aRecordId);
            break;
        case SET_RECORD:
            aListeners.recordChanged(aRecordId);
            break;
        case DELETE_RECORD:
            aListeners.recordDeleted(aRecordId);
            break;
        default:
            Logger.WLOG(Logger.EMidpRms, "notifyListeners() failed, invalid operation:" + aOperation);
            break;
        }
    }

    private CommsMessage createAddListenerMsg(String aName)
    {
        CommsMessage msg = createMessage(aName);
        msg.setMessageId(MSG_ID_LISTENER_CHANGE);
        msg.write(ADD_LISTENER);
        return msg;
    }

    private CommsMessage createRemoveListenerMsg(String aName)
    {
        CommsMessage msg = createMessage(aName);
        msg.setMessageId(MSG_ID_LISTENER_CHANGE);
        msg.write(REMOVE_LISTENER);
        return msg;
    }

    private CommsMessage createRecordChangedMsg(String aName, int aOperation, int aRecordId)
    {
        CommsMessage msg = createMessage(aName);
        msg.setMessageId(MSG_ID_RECORD_CHANGE);
        msg.write(aOperation);
        msg.write(aRecordId);
        return msg;
    }

    private CommsMessage createMessage(String aName)
    {
        CommsMessage msg = new CommsMessage();
        msg.setModuleId(PLUGIN_ID_RMS_C);
        msg.write(aName);
        return msg;
    }

    private void send(CommsMessage aMsg)
    {
        try
        {
            if (iComms != null)
            {
                iComms.send(aMsg);
            }
        }
        catch (CommsException e)
        {
            Logger.WLOG(Logger.EMidpRms, "send() failed", e);
        }
    }

    private void syncSend(CommsMessage aMsg)
    {
        try
        {
            if (iComms != null)
            {
                CommsMessage reply = iComms.sendReceive(aMsg, REPLY_TIMEOUT);
                processMessage(reply);
            }
        }
        catch (CommsException e)
        {
            Logger.WLOG(Logger.EMidpRms, "sendReceive() failed", e);
        }
    }

}

