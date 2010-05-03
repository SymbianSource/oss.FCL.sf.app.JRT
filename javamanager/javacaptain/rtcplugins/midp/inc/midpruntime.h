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
* Description:  MidpRtcRuntime
*
*/

#ifndef MIDPRUNTIME_H
#define MIDPRUNTIME_H

#include "rtcbaseplugin.h"

namespace java
{
namespace captain
{

class MidpRuntime : public RtcBasePlugin
{
public:
    MidpRuntime(const Uid& aUID,
                CoreInterface* aCore);
    virtual ~MidpRuntime();

protected:
    // Currently only platform/runtime specific part
    virtual cmdLine_t& generateCommandLine(const int& aLaunchType,
                                           cmdLine_t& params);
};

} // namespace captain
} // namespace java

#endif // MIDPRUNTIME_H


