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

import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.exception.CommsException;
import com.nokia.mj.impl.utils.Logger;

class DriveObserverMessages
{
    // keep in sync with driveutilities.h!!

    final static int DRIVEOBSERVER_MSG_SUBSCRIBE_EVENTS    = 1;
    final static int DRIVEOBSERVER_MSG_UNSUBSCRIBE_EVENTS  = 2;
    final static int DRIVEOBSERVER_MSG_DRIVE_CHANGED_EVENT = 3;
    final static int DRIVEOBSERVER_MSG_GETDRIVES_REQUEST   = 4;
    final static int DRIVEOBSERVER_MSG_GETDRIVES_RESULT    = 5;

    final static int DRIVEOBSERER_GET_DRIVES_ALL           = 1;
    final static int DRIVEOBSERER_GET_DRIVES_ACCESIBLE     = 2;

    final static int PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C     = 50;
    final static int PLUGIN_ID_DRIVE_OBSERVER_JAVA_C       = 51;


    final static DriveInfo[] getDrives(boolean allDrives)
    {
        DriveInfo[] drives = null;

        try
        {
            CommsEndpoint comms;
            boolean commsOwner = false;
            comms = CommsEndpoint.find("javacaptain");
            if (comms == null)
            {
                comms = new CommsEndpoint();
                comms.connect(CommsEndpoint.JAVA_CAPTAIN);
                commsOwner = true;
            }
            CommsMessage msg = new CommsMessage();
            msg.setMessageId(DRIVEOBSERVER_MSG_GETDRIVES_REQUEST);
            msg.setModuleId(PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C);
            if (allDrives)
            {
                msg.write(DRIVEOBSERER_GET_DRIVES_ALL);
            }
            else
            {
                msg.write(DRIVEOBSERER_GET_DRIVES_ACCESIBLE);
            }

            CommsMessage replyMsg = comms.sendReceive(msg, 5);

            if (commsOwner)
            {
                comms.destroy();
            }

            int numOfDrives = 0;
            numOfDrives = replyMsg.readInt();
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "extracting " + numOfDrives + " driveInfos");

            if (numOfDrives > 0)
            {
                drives = new DriveInfo[numOfDrives];

                for (int i = 0 ; i < numOfDrives ; ++i)
                {
                    drives[i] = extractDriveInfo(replyMsg);
                }
            }
        }
        catch (CommsException e)
        {
            Logger.ELOG(Logger.EJavaFile, "getDrives failed", e);
        }

        return drives;
    }

    static DriveInfo extractDriveInfo(CommsMessage aMessage)
    {
        DriveInfo drive = new DriveInfo();
        try
        {
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "extracting drive");

            drive.iRootPath = aMessage.readString();
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "drives[].iRootPath=" + drive.iRootPath);
            drive.iIsPresent = (aMessage.readInt() != 0);
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "drives[].iIsPresent=" + drive.iIsPresent);
            drive.iIsRemovable = (aMessage.readInt() != 0);
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "drives[].iIsRemovable=" + drive.iIsRemovable);
            drive.iIsLocal =(aMessage.readInt() != 0);
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "drives[].iIsLocal=" + drive.iIsLocal);
            drive.iIsReadOnly =(aMessage.readInt() != 0);
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "drives[].iIsReadOnly=" + drive.iIsReadOnly);
            drive.iId = aMessage.readInt();
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "drives[].iId=" + drive.iId);
            drive.iIsExternallyMountable = (aMessage.readInt() != 0);
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "drives[].iIsExternallyMountable=" + drive.iIsPresent);
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaFile, Logger.EInfo, "Exception in extraction", e);
        }
        return drive;
    }

    static void subscribeEvents(CommsEndpoint aComms, boolean aUnsubscribe)
    {
        CommsMessage msg = new CommsMessage();
        if (aUnsubscribe)
        {
            msg.setMessageId(DRIVEOBSERVER_MSG_UNSUBSCRIBE_EVENTS);
        }
        else
        {
            msg.setMessageId(DRIVEOBSERVER_MSG_SUBSCRIBE_EVENTS);
        }
        msg.setModuleId(PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C);
        msg.write(PLUGIN_ID_DRIVE_OBSERVER_JAVA_C);
        aComms.send(msg);
    }
}
