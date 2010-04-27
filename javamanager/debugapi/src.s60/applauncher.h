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
* Description: AppLauncher can be used to start and stop Java application
*
*/


#ifndef APPLAUNCHER_H
#define APPLAUNCHER_H

#include <string>
#include <unistd.h>

#include "javauid.h"
#include "javaosheaders.h"

namespace java
{
// forward declarations
namespace comms
{
class CommsMessage;
}

namespace debug
{
OS_NONSHARABLE_CLASS(AppLauncher)
{
public:
    AppLauncher(const java::util::Uid& aAppUid);
    virtual ~AppLauncher();

    int startApp(const std::wstring& aJvmArgs);
    int stopApp();
    pid_t getPid();

protected:
    virtual java::comms::CommsMessage createStartAppMessage(const std::wstring& aJvmArgs);
    virtual java::comms::CommsMessage createStopAppMessage();
    virtual int sendToJavaCaptain(java::comms::CommsMessage& aMessage);

private:
    java::util::Uid mUid;
    pid_t mPid;
};

} // end namespace debug
} // end namespace java


#endif // APPLAUNCHER_H
