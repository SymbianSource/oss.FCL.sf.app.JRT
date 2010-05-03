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
* Description:  RtcPluginInterface
*
*/

#ifndef RTCPLUGININTERFACE_H
#define RTCPLUGININTERFACE_H

#include "javauid.h"

using java::util::Uid;

namespace java
{
namespace captain
{

class RtcRuntimeInterface;
class CoreInterface;
class ApplicationRuntimeEventsInterface;

class RtcPluginInterface
{
public:
    virtual ~RtcPluginInterface() {}

    virtual RtcRuntimeInterface* createNewRuntimeInstance(const Uid& aUID,
            CoreInterface* aCore) = 0;
};

} // namespace captain
} // namespace java

#endif // RTCPLUGININTERFACE_H

