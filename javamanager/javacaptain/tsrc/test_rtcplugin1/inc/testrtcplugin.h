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
* Description:  TestRtcPlugin
*
*/

#ifndef TESTRTCPLUGIN_H
#define TESTRTCPLUGIN_H

#include "javaosheaders.h"

#include "rtcplugininterface.h"

java::captain::RtcPluginInterface* getTestRtcPlugin();

namespace java
{
namespace captain
{

OS_NONSHARABLE_CLASS(TestRtcPlugin) : public RtcPluginInterface
{
public:
    virtual ~TestRtcPlugin();

    virtual RtcRuntimeInterface* createNewRuntimeInstance(const Uid& aUID,
            CoreInterface* aCore);
};

} // namespace captain
} // namespace java

#endif // TESTRTCPLUGIN_H

