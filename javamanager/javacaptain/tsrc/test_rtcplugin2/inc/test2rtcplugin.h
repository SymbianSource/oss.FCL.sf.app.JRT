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

#ifndef TEST2RTCPLUGIN_H
#define TEST2RTCPLUGIN_H

#include "javaosheaders.h"

#include "rtcplugininterface.h"

namespace java
{
namespace captain
{

OS_NONSHARABLE_CLASS(Test2RtcPlugin) : public RtcPluginInterface
{
public:
    virtual RtcRuntimeInterface* createNewRuntimeInstance(const Uid& aUID, CoreInterface* aCore);
};

} // namespace captain
} // namespace java

#endif // TEST2RTCPLUGIN_H

