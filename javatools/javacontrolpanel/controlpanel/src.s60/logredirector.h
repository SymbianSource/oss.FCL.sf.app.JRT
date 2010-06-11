/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LOGREDIRECTOR_H
#define LOGREDIRECTOR_H

#include <memory>
#include "javadiagnostic.h"
using namespace java::debug;

class LogRedirector : public DiagnosticListener
{
public:
    LogRedirector();
    ~LogRedirector();

    void start();
    void stop();

    // DiagnosticListener methods
    virtual void systemOut(const TDesC8& aData);
    virtual void systemErr(const TDesC8& aData);
    virtual void log(const TDesC8& aData);

private:
    void write(const TDesC8& aData);
    std::auto_ptr<JavaDiagnostic> iJavaDiagnostic;
    int mFd;
};

#endif // LOGREDIRECTOR_H

