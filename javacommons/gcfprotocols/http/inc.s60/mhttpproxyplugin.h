/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef MHTTPPROXYPLUGIN_H
#define MHTTPPROXYPLUGIN_H

#include <http/rhttpsession.h>

class MMidHttpProxyPlugin
{
public:
    virtual void InstallFilterL(RHTTPSession* aSession) =0 ;
    virtual void SetIAPId(TUint32 aAPNId) = 0;
    virtual ~MMidHttpProxyPlugin() = 0;
};

inline MMidHttpProxyPlugin::~MMidHttpProxyPlugin()
{}

#endif // MHTTPPROXYPLUGIN_H
