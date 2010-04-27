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
* Description:  MidpRtcPlugin
*
*/


#include "logger.h"
#include "javacommonutils.h"

#include "midpruntime.h"

#include "midprtcplugin.h"

java::captain::RtcPluginInterface* getMidpRtcPlugin()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return new java::captain::MidpRtcPlugin();
}

namespace java
{
namespace captain
{

MidpRtcPlugin::~MidpRtcPlugin()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

RtcRuntimeInterface* MidpRtcPlugin::createNewRuntimeInstance(const Uid& aUID,
        CoreInterface* aCore)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return new MidpRuntime(aUID, aCore);
}

} // namespace captain
} // namespace java

