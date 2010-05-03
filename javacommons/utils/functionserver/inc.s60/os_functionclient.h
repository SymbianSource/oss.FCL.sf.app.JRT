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
* Description:  ?Description
*
*/


#ifndef OS_FUNCTIONCLIENT_H
#define OS_FUNCTIONCLIENT_H

#include <e32std.h>
#include <string>

#include "functionserver.h"
class Functor;

namespace java
{
namespace util
{

class FunctionServer;
class OsFunctionServer;

NONSHARABLE_CLASS(OsFunctionClient) : public RSessionBase
{
public:
    int connect(const OsFunctionServer& server);
    TInt execute(const Functor& functor) const;
    TInt sendCloseMessage(FunctionServer* server) const;

    OsFunctionClient();
    virtual ~OsFunctionClient();

private:
    HBufC* mServerName;
    mutable RFastLock mLock;
};

} //end namespace util
} //end namespace java
#endif // OS_FUNCTIONCLIENT_H

