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
* Description: CRedirectServer session

*
*/

#ifndef JAVAREDIRECTSESSION_H
#define JAVAREDIRECTSESSION_H

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

OS_NONSHARABLE_CLASS(CRedirectSession): public CSession2
{
public:
    CRedirectSession(DiagnosticListener& aListener);
    virtual ~CRedirectSession();

    virtual void ServiceL(const RMessage2& aMessage);
private:
    void handleMessageL(const RMessage2& aMessage);
    void dispatchToListener(TInt aType, const TDesC8& aData);

    DiagnosticListener& mListener;
};

} // namespace util
} // namespace java

#endif // JAVAREDIRECTSESSION_H
