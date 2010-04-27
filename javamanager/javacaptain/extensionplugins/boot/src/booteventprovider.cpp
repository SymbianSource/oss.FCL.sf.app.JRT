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
* Description:  BootEventProvider
*
*/

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "logger.h"

#include "coreinterface.h"
#include "eventconsumerinterface.h"

#include "booteventprovider.h"
#include "booteventprovidermessages.h"

namespace java
{
namespace captain
{

// In S60 working directory is initalized to C:\private\<UID> by OpenC
const char IAD_BOOT_FLAG[]   = "iadboot.dat";
const char FIRST_BOOT_FLAG[] = "firstbootdone.dat";

BootEventProvider::BootEventProvider() : mCore(0)
{
    JELOG2(EJavaCaptain);
}

BootEventProvider::~BootEventProvider()
{
    JELOG2(EJavaCaptain);
}

void BootEventProvider::startPlugin(CoreInterface* aCore)
{
    JELOG2(EJavaCaptain);
    mCore = aCore;

    int bootType = NORMAL_BOOT_C;
    if (isIadBoot())
    {
        bootType = IAD_BOOT_C;
        setBootFlag(IAD_BOOT_FLAG, false);
    }
    else if (isFirstDeviceBoot())
    {
        bootType = FIRST_DEVICE_BOOT_C;
        setBootFlag(FIRST_BOOT_FLAG, true);
    }

    dispatchEvent(BOOT_EVENT_PROVIDER, bootType);
}

void BootEventProvider::stopPlugin()
{
    JELOG2(EJavaCaptain);
    mCore = 0;
}

void BootEventProvider::dispatchEvent(const std::string& aEvent, int aType)
{
    LOG2(EJavaCaptain, EInfo, "BootEventProvider dispatching event=%s type=%d", aEvent.c_str(), aType);

    CommsMessage eventMsg;
    setBootMessageParams(eventMsg, aType);
    mCore->getEventDispatcher()->event(aEvent, eventMsg);
}

bool BootEventProvider::isFirstDeviceBoot() const
{
    JELOG2(EJavaCaptain);
    bool firstBoot = !fileExists(FIRST_BOOT_FLAG);
    return firstBoot;
}

bool BootEventProvider::isIadBoot() const
{
    JELOG2(EJavaCaptain);
    bool iadBoot = fileExists(IAD_BOOT_FLAG);
    return iadBoot;
}

void BootEventProvider::setBootFlag(const std::string& aPath, bool aCreate) const
{
    JELOG2(EJavaCaptain);
    if (aCreate)
    {
        createFile(aPath);
    }
    else
    {
        removeFile(aPath);
    }
}

bool BootEventProvider::fileExists(const std::string& aPath) const
{
    JELOG2(EJavaCaptain);

    bool exists = false;
    struct stat fileStatBuf;
    if (stat(aPath.c_str(), &fileStatBuf) == 0)
    {
        exists = true;
    }
    return exists;
}
void BootEventProvider::createFile(const std::string& aPath) const
{
    JELOG2(EJavaCaptain);
    int fd = open(aPath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        ELOG1(EJavaCaptain, "createFile() failed: %s", strerror(errno));
    }
    else
    {
        close(fd);
    }
}

void BootEventProvider::removeFile(const std::string& aPath) const
{
    JELOG2(EJavaCaptain);
    int rc = unlink(aPath.c_str());
    if (rc < 0)
    {
        ELOG1(EJavaCaptain, "removeFile() failed: %s", strerror(errno));
    }
}

} // namespace captain
} // namespace java

