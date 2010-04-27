/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CRedirectServer forwards standard outs and log messages to
*              listener
*
*/

#ifndef JAVAREDIRECTSERVER_H
#define JAVAREDIRECTSERVER_H

#include <e32base.h>
#include "javaosheaders.h"

namespace java
{
namespace debug
{
class DiagnosticListener;
}
using java::debug::DiagnosticListener;

namespace util
{

OS_NONSHARABLE_CLASS(CRedirectServer) : public CServer2
{
public:
    OS_IMPORT CRedirectServer(DiagnosticListener& aListener);
    OS_IMPORT virtual ~CRedirectServer();
    OS_IMPORT virtual void StartL();

private:
    void StartL(const TDesC& aName);
    virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    DiagnosticListener& mListener;
};


} // namespace util
} // namespace java

#endif // JAVAREDIRECTSERVER_H
