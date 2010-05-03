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
* Description:  This class provides container for message.
*
*/


#ifndef APPLICATIONSTATECONTROLLER_H
#define APPLICATIONSTATECONTROLLER_H

#include "javaosheaders.h"

namespace java // codescanner::namespace
{

namespace runtime // codescanner::namespace
{

enum ApplicationState
{
    START_APPLICATION,
    CONTINUE_APPLICATION_STARTUP,
    CANCEL_APPLICATION_STARTUP,
    JVM_STARTED,
    STOPPING_JVM,
    JVM_STOPPED,
    CLOSE_APPLICATION
};

OS_NONSHARABLE_CLASS(ApplicationStateController)
{
public:

    //API
    virtual void ApplicationStateChangeRequest(ApplicationState state) = 0;

protected:
    //Destructor
    virtual ~ApplicationStateController() {}

};
} //end namespace runtime
} //end namespace java

#endif // APPLICATIONSTATECONTROLLER_H

