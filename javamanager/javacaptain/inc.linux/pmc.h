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
* Description:  Pmc
*
*/

#ifndef PMC_H
#define PMC_H

#include "pmcinterface.h"

namespace java
{

namespace captain
{
class CoreInterface;
class ProcessManagementEventsInterface;

class Pmc : public PmcInterface
{
public:
    Pmc();
    virtual ~Pmc();

    bool start(CoreInterface* aCore,
               ProcessManagementEventsInterface* aProcessManagementEventsDispatcher);
    bool stop();

    // PmcInterface
    virtual int launch(const cmdLine_t& cmdLine, const int& options);
    virtual int terminate(const int& pid);
    virtual int kill(const int& pid);
};

} // namespace captain
} // namespace java

#endif // PMC_H

