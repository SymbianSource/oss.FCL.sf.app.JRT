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
* Description:  Container for sharable endpoints. Sharable endpoints can be
                 found using name (wstring)
*
*/

#include <sstream>
#include <errno.h>

#include "logger.h"
#include "javacommonutils.h"

#include "commscontext.h"
#include "commsclientendpoint.h"

using namespace java::comms;
using java::util::ScopedLock;

using namespace java::util;

CommsContext::CommsContext()
{
}

CommsContext::~CommsContext()
{
    for (endpoints_t::iterator it = mEndpoints.begin(); it != mEndpoints.end(); it++)
    {
        delete it->second;
    }
    mEndpoints.clear();

    for (serverEndpoints_t::iterator it = mServerEndpoints.begin(); it != mServerEndpoints.end(); it++)
    {
        delete it->second;
    }
    mServerEndpoints.clear();
}

#if defined(__SYMBIAN32__) && defined(__WINSCW__)

#include <pls.h>
CommsContext& CommsContext::getContext()
{
    // Access the PLS of this process
    CommsContext* context  = Pls<CommsContext>(TUid::Uid(0x200211DF));
    return *context;
}

#else

static CommsContext* sContext = 0;
CommsContext& CommsContext::getContext()
{

    if (sContext == 0)
    {
        sContext = new CommsContext();
    }
    return *sContext;
}
#endif

void CommsContext::add(CommsClientEndpoint* aEndpoint, const wstring& aName)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mEndpointsMutex);

    endpoints_t::iterator it = mEndpoints.find(aEndpoint);
    if (it == mEndpoints.end() && find(aName) == 0)
    {
        mEndpoints.insert(std::make_pair(aEndpoint, new wstring(aName)));
        LOG1WSTR(EJavaComms, EInfo, "Added client endpoint=%s", aName);
    }
    else
    {
        ELOG1WSTR(EJavaComms, "Add client endpoint failed, endpoint=%s", aName);
    }
}

void CommsContext::remove(CommsClientEndpoint* aEndpoint)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mEndpointsMutex);

    endpoints_t::iterator it = mEndpoints.find(aEndpoint);

    if (it != mEndpoints.end())
    {
        LOG1WSTR(EJavaComms, EInfo, "Removed client endpoint=%s", *(it->second));
        delete it->second;
        mEndpoints.erase(it);
    }
}

CommsClientEndpoint* CommsContext::find(const wstring& aName)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mEndpointsMutex);

    CommsClientEndpoint* client = 0;
    for (endpoints_t::iterator it = mEndpoints.begin(); it != mEndpoints.end(); it++)
    {
        if (aName.compare(*(it->second)) == 0)
        {
            client = it->first;
            LOG1WSTR(EJavaComms, EInfo, "Found client endpoint=%s", aName);
            break;
        }
    }
    return client;
}

void CommsContext::add(CommsServerEndpoint* aEndpoint, const wstring& aName)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mEndpointsMutex);

    serverEndpoints_t::iterator it = mServerEndpoints.find(aEndpoint);
    if (it == mServerEndpoints.end() && findServer(aName) == 0)
    {
        mServerEndpoints.insert(std::make_pair(aEndpoint, new wstring(aName)));
        LOG1WSTR(EJavaComms, EInfo, "Added server endpoint=%s", aName);
    }
    else
    {
        ELOG1WSTR(EJavaComms, "Add server endpoint failed, endpoint=%s", aName);
    }
}

void CommsContext::remove(CommsServerEndpoint* aEndpoint)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mEndpointsMutex);

    serverEndpoints_t::iterator it = mServerEndpoints.find(aEndpoint);

    if (it != mServerEndpoints.end())
    {
        LOG1WSTR(EJavaComms, EInfo, "Removed server endpoint=%s", *(it->second));
        delete it->second;
        mServerEndpoints.erase(it);
    }
}

CommsServerEndpoint* CommsContext::findServer(const wstring& aName)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mEndpointsMutex);

    CommsServerEndpoint* server = 0;
    for (serverEndpoints_t::iterator it = mServerEndpoints.begin(); it != mServerEndpoints.end(); it++)
    {
        if (aName.compare(*(it->second)) == 0)
        {
            server = it->first;
            LOG1WSTR(EJavaComms, EInfo, "Found server endpoint=%s", aName);
            break;
        }
    }
    return server;
}

