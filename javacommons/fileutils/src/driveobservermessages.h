/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DriveObserverMessages
*
*/

#ifndef DRIVEOBSERVERMESSAGES_H
#define DRIVEOBSERVERMESSAGES_H

#include "logger.h"
#include "comms.h"
#include "commsmessage.h"

#include "driveutilities.h"

using java::comms::CommsMessage;

namespace java
{
namespace fileutils
{
const int DRIVEOBSERVER_MSG_SUBSCRIBE_EVENTS    = 1;
const int DRIVEOBSERVER_MSG_UNSUBSCRIBE_EVENTS  = 2;
const int DRIVEOBSERVER_MSG_DRIVE_CHANGED_EVENT = 3;
const int DRIVEOBSERVER_MSG_GETDRIVES_REQUEST   = 4;
const int DRIVEOBSERVER_MSG_GETDRIVES_RESULT    = 5;

const int DRIVEOBSERER_GET_DRIVES_ALL           = 1;
const int DRIVEOBSERER_GET_DRIVES_ACCESIBLE     = 2;

inline void setSubscribeParams(CommsMessage& aMessage, int moduleId)
{
    aMessage.setMessageId(DRIVEOBSERVER_MSG_SUBSCRIBE_EVENTS);
    aMessage.setModuleId(java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C);
    aMessage << moduleId;
}
inline void getSubscribeParams(CommsMessage& aMessage, int& moduleId)
{
    if (aMessage.getMessageId() == DRIVEOBSERVER_MSG_SUBSCRIBE_EVENTS)
    {
        aMessage >> moduleId;
    }
    else
    {
        moduleId = 0;
    }
}

inline void setUnsubscribeMessage(CommsMessage& aMessage, int moduleId)
{
    aMessage.setMessageId(DRIVEOBSERVER_MSG_UNSUBSCRIBE_EVENTS);
    aMessage.setModuleId(java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C);
    aMessage << moduleId;
}

inline void getUnsubscribeParams(CommsMessage& aMessage, int& moduleId)
{
    if (aMessage.getMessageId() == DRIVEOBSERVER_MSG_UNSUBSCRIBE_EVENTS)
    {
        aMessage >> moduleId;
    }
    else
    {
        ELOG2(EJavaFile, "wrong messageId was %d expecting %d",
              aMessage.getMessageId(), DRIVEOBSERVER_MSG_UNSUBSCRIBE_EVENTS);
        moduleId = 0;
    }
}

inline void appendDriveInfo(CommsMessage& aMessage, const driveInfo& aDriveInfo)
{
    aMessage << aDriveInfo.iRootPath;
    aMessage << (aDriveInfo.iIsPresent ? 1 : 0);
    aMessage << (aDriveInfo.iIsRemovable ? 1 : 0);
    aMessage << (aDriveInfo.iIsLocal ? 1 : 0);
    aMessage << (aDriveInfo.iIsReadOnly ? 1 : 0);
    aMessage << (int) aDriveInfo.iId;
    aMessage << (aDriveInfo.iIsExternallyMountable ? 1 : 0);
}

inline void extractDriveInfo(CommsMessage& aMessage, driveInfo& aDriveInfo)
{
    aMessage >> aDriveInfo.iRootPath;

    int tempInt = 0;
    aMessage >> tempInt;
    aDriveInfo.iIsPresent = (tempInt == 1 ? true : false);

    aMessage >> tempInt;
    aDriveInfo.iIsRemovable = (tempInt == 1 ? true : false);

    aMessage >> tempInt;
    aDriveInfo.iIsLocal = (tempInt == 1 ? true : false);

    aMessage >> tempInt;
    aDriveInfo.iIsReadOnly = (tempInt == 1 ? true : false);

    aMessage >> tempInt;
    aDriveInfo.iId = tempInt;
    aMessage >> tempInt;
    aDriveInfo.iIsExternallyMountable = (tempInt == 1 ? true : false);
}

inline void setDriveChangedParams(CommsMessage& aMessage,
                                  int aReceiver,
                                  int aModuleId,
                                  int aOperation,
                                  const driveInfo& aDriveInfo)
{
    aMessage.setMessageId(DRIVEOBSERVER_MSG_DRIVE_CHANGED_EVENT);
    aMessage.setReceiver(aReceiver);
    aMessage.setModuleId(aModuleId);
    aMessage << aOperation;
    appendDriveInfo(aMessage, aDriveInfo);
}

inline void getDriveChangedParams(CommsMessage& aMessage, int& aOperation,
                                  driveInfo& aDriveInfo)
{
    if (aMessage.getMessageId() == DRIVEOBSERVER_MSG_DRIVE_CHANGED_EVENT)
    {
        aMessage >> aOperation;
        extractDriveInfo(aMessage, aDriveInfo);
    }
    else
    {
        ELOG2(EJavaFile, "wrong messageId was %d expecting %d",
              aMessage.getMessageId(), DRIVEOBSERVER_MSG_DRIVE_CHANGED_EVENT);
    }
}

inline void setGetDrivesParams(CommsMessage& aMessage, const int& aDriveTypes)
{
    aMessage.setMessageId(DRIVEOBSERVER_MSG_GETDRIVES_REQUEST);
    aMessage.setModuleId(java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C);
    aMessage << aDriveTypes;
}

inline void getGetDrivesParams(CommsMessage& aMessage, int& aDriveTypes)
{
    if (aMessage.getMessageId() == DRIVEOBSERVER_MSG_GETDRIVES_REQUEST)
    {
        aMessage >> aDriveTypes;
    }
    else
    {
        ELOG2(EJavaFile, "wrong messageId was %d expecting %d",
              aMessage.getMessageId(), DRIVEOBSERVER_MSG_GETDRIVES_REQUEST);
    }
}

inline void setGetDrivesResultParams(CommsMessage& aMessage,
                                     const driveInfos& aDriveInfos)
{
    aMessage.setMessageId(DRIVEOBSERVER_MSG_GETDRIVES_RESULT);
    aMessage.setModuleId(java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C);
    int numOfDrives = aDriveInfos.size();
    aMessage << numOfDrives;
    for (int i = 0 ; i < numOfDrives ; i++)
    {
        appendDriveInfo(aMessage, aDriveInfos[i]);
    }
}

inline void getGetDrivesResultParams(CommsMessage& aMessage,
                                     driveInfos& aDriveInfos)
{
    aDriveInfos.clear();

    if (aMessage.getMessageId() == DRIVEOBSERVER_MSG_GETDRIVES_RESULT)
    {
        int numOfDrives = 0;
        aMessage >> numOfDrives;
        if (numOfDrives > 0)
        {
            driveInfo di;
            for (int i = 0 ; i < numOfDrives ; i++)
            {
                extractDriveInfo(aMessage, di);
                aDriveInfos.push_back(di);
            }
        }
    }
    else
    {
        ELOG2(EJavaFile, "wrong messageId was %d expecting %d",
              aMessage.getMessageId(), DRIVEOBSERVER_MSG_GETDRIVES_RESULT);
    }
}

} // namespace fileutils
} // namespace java
#endif // DRIVEOBSERVERMESSAGES_H
