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

#ifndef MIDPRTCPLUGIN_H
#define MIDPRTCPLUGIN_H

#include "rtcplugininterface.h"

java::captain::RtcPluginInterface* getMidpRtcPlugin();

namespace java
{
namespace captain
{

class MidpRtcPlugin : public RtcPluginInterface
{
public:
    virtual ~MidpRtcPlugin();
    virtual RtcRuntimeInterface* createNewRuntimeInstance(const Uid& aUID,
            CoreInterface* aCore);
};

} // namespace captain
} // namespace java

#endif // MIDPRTCPLUGIN_H
