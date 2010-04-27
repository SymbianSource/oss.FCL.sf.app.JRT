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
* Description:  ProcessManagementEventsInterface
*
*/

#ifndef PROCESSMANAGEMENTEVENTSINTERFACE_H
#define PROCESSMANAGEMENTEVENTSINTERFACE_H

namespace java
{
namespace captain
{

class ProcessManagementEventsInterface
{
public:
    virtual ~ProcessManagementEventsInterface() {}

    virtual void pmcTerminated(const int& pid, const int& exitCode) = 0;
};

} // namespace captain
} // namespace java

#endif // PROCESSMANAGEMENTEVENTSINTERFACE_H

