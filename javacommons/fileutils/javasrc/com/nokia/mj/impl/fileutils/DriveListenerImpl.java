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


package com.nokia.mj.impl.fileutils;

import java.util.Vector;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.exception.CommsException;

final class DriveListenerImpl implements CommsListener
{
    public static boolean registerListenerStatic(DriveListener aListener)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveListenerImpl.registerListenerStatic()");
        boolean regSuccess = getInstance().registerListener(aListener);
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveListenerImpl.registerListenerStatic() returns "
                   + regSuccess);
        return regSuccess;
    }

    public static boolean unregisterListenerStatic(DriveListener aListener)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveListenerImpl.unregisterListenerStatic()");
        boolean unregSuccess = getInstance().unregisterListener(aListener);
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveListenerImpl.unregisterListenerStatic() returns "
                   + unregSuccess);
        return unregSuccess;
    }

    private static Vector iDriveListeners = null;
    private static Vector getListeners()
    {
        if (null == iDriveListeners)
        {
            iDriveListeners = new Vector();
        }
        return iDriveListeners;
    }

    private static CommsEndpoint iComms = null;
    private static boolean iCommsOwner = false;
    private static CommsEndpoint getComms()
    {
        if (null == iComms)
        {
            iComms = CommsEndpoint.find("javacaptain");
            if (null == iComms)
            {
                iComms = new CommsEndpoint();
                iComms.connect(CommsEndpoint.JAVA_CAPTAIN);
                iCommsOwner = true;
            }
            iComms.registerListener(DriveObserverMessages.PLUGIN_ID_DRIVE_OBSERVER_JAVA_C,
                                    getInstance());
        }
        return iComms;
    }

    private static DriveListenerImpl iDriveListenerImpl = null;
    private static DriveListenerImpl getInstance()
    {
        if (null == iDriveListenerImpl)
        {
            iDriveListenerImpl = new DriveListenerImpl();
        }
        return iDriveListenerImpl;
    }

    boolean registerListener(DriveListener aListener)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveListenerImpl.registerListener()");
        boolean regSuccess = true;
        try
        {
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "    listeners before " + getListeners().size());
            if (getListeners().size() == 0 &&
                    null != getComms())
            {
                DriveObserverMessages.subscribeEvents(getComms(), false);
            }

            getListeners().addElement(aListener);
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "    listeners after " + getListeners().size());
        }
        catch (CommsException e)
        {
            regSuccess = false;
            Logger.ELOG(Logger.EJavaFile, "registerListener failed", e);
        }
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveListenerImpl.registerListener() returns "+
                   regSuccess);

        return regSuccess;
    }

    boolean unregisterListener(DriveListener aListener)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveListenerImpl.unregisterListener()");

        boolean unregSuccess = true;
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "    listeners before " + getListeners().size());
        getListeners().removeElement(aListener);
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "    listeners after " + getListeners().size());

        try
        {
            if (getListeners().size() == 0 &&
                    null != getComms())
            {
                DriveObserverMessages.subscribeEvents(getComms(), true);
                getComms().unregisterListener(DriveObserverMessages.PLUGIN_ID_DRIVE_OBSERVER_JAVA_C);
                if (iCommsOwner)
                {
                    getComms().destroy();
                    iCommsOwner = false;
                }
                iComms = null;
            }
        }
        catch (CommsException e)
        {
            unregSuccess = false;
            Logger.ELOG(Logger.EJavaFile, "unregisterListener failed", e);
        }
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveListenerImpl.unregisterListener()");

        return unregSuccess;
    }

    public void processMessage(CommsMessage aMessage)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "--> DriveListenerImpl.processMessage()");
        switch (aMessage.getMessageId())
        {
        case DriveObserverMessages.DRIVEOBSERVER_MSG_DRIVE_CHANGED_EVENT:
            int oper = aMessage.readInt();
            DriveInfo drive = DriveObserverMessages.extractDriveInfo(aMessage);
            int listenersCount = getListeners().size();
            for (int i = 0 ; i < listenersCount ; ++i)
            {
                DriveListener listener = (DriveListener) getListeners().elementAt(i);
                listener.driveChanged(oper, drive);
            }
            break;
        }
        Logger.LOG(Logger.EJavaFile, Logger.EInfo, "<-- DriveListenerImpl.processMessage()");
    }
}

