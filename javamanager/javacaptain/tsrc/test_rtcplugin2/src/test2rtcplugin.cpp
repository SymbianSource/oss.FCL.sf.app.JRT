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
* Description:  Test2RtcPlugin
*
*/


#include "logger.h"
#include "javacommonutils.h"

#include "rtcplugininterface.h"
#include "test2runtime.h"

#include "test2rtcplugin.h"

#ifdef __SYMBIAN32__
java::captain::RtcPluginInterface* getRtcPlugin()
{
#else
extern "C" java::captain::RtcPluginInterface* getRtcPlugin()
{
#endif
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return new java::captain::Test2RtcPlugin();
}

namespace java
{
namespace captain
{

RtcRuntimeInterface* Test2RtcPlugin::createNewRuntimeInstance(const Uid& aUID,
        CoreInterface* aCore)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return new Test2Runtime(aUID, aCore);
}

} // namespace captain
} // namespace java

