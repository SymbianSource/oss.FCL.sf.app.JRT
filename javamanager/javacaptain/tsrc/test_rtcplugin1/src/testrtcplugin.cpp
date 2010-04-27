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


#include "logger.h"
#include "javacommonutils.h"

#include "testruntime.h"

#include "testrtcplugin.h"

#ifdef __SYMBIAN32__
java::captain::RtcPluginInterface* getRtcPlugin()
{
#else
extern "C" java::captain::RtcPluginInterface* getRtcPlugin()
{
#endif
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return new java::captain::TestRtcPlugin();
}

namespace java
{
namespace captain
{

TestRtcPlugin::~TestRtcPlugin()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

RtcRuntimeInterface* TestRtcPlugin::createNewRuntimeInstance(const Uid& aUID,
        CoreInterface* aCore)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return new TestRuntime(aUID, aCore);
}

} // namespace captain
} // namespace java

