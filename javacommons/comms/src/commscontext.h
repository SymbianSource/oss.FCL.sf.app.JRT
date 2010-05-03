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

#ifndef COMMSCONTEXT_H
#define COMMSCONTEXT_H
#include <map>
#include <string>

#include "javaosheaders.h"
#include "scopedlocks.h"

using std::wstring;

namespace java
{
namespace comms
{
class CommsClientEndpoint;
class CommsServerEndpoint;

OS_NONSHARABLE_CLASS(CommsContext)
{
public:
    CommsContext();
    virtual ~CommsContext();
    static CommsContext& getContext();

    void add(CommsClientEndpoint* endpoint, const wstring& name);
    void remove(CommsClientEndpoint* endpoint);
    CommsClientEndpoint* find(const wstring& name);

    void add(CommsServerEndpoint* endpoint, const wstring& name);
    void remove(CommsServerEndpoint* endpoint);
    CommsServerEndpoint* findServer(const wstring& name);

private:
    typedef std::map<CommsClientEndpoint*, wstring*> endpoints_t;
    endpoints_t mEndpoints;

    typedef std::map<CommsServerEndpoint*, wstring*> serverEndpoints_t;
    serverEndpoints_t mServerEndpoints;

    java::util::ScopedMutex mEndpointsMutex;
};

} // namespace comms
} // namespace java

#endif // COMMSCONTEXT_H
