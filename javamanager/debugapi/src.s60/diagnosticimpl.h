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
* Description: DiagnosticImpl provides implementation for JavaDiagnostic
*
*/


#ifndef DIAGNOSTICIMPL_H
#define DIAGNOSTICIMPL_H

#include <e32base.h>
#include "javadiagnostic.h"
#include "javaosheaders.h"

namespace java
{
namespace debug
{

OS_NONSHARABLE_CLASS(DiagnosticImpl) : public CActive,
        public JavaDiagnostic,
        public DiagnosticListener
{
public:
    DiagnosticImpl();
    virtual ~DiagnosticImpl();

    // JavaDiagnostic methods
    virtual TInt setDiagnosticListener(DiagnosticListener& aListener);
    virtual void removeDiagnosticListener();

protected:
    // CActive methods
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

    //DiagnosticListener methods
    virtual void systemOut(const TDesC8& aData);
    virtual void systemErr(const TDesC8& aData);
    virtual void log(const TDesC8& aData);

    virtual TInt start();
    virtual void stop();
    static void messageLoop(TAny* aArgs);
    void runServerL();

    DiagnosticListener* mListener;
    RThread mThread;
};

} // end namespace debug
} // end namespace java


#endif // DIAGNOSTICIMPL_H
