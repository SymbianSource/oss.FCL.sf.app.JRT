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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "logredirector.h"

LogRedirector::LogRedirector() : mFd(-1)
{
    iJavaDiagnostic.reset(JavaDiagnostic::createInstance());
}

LogRedirector::~LogRedirector()
{
}

void LogRedirector::start()
{
    iJavaDiagnostic->setDiagnosticListener(*this);
    mFd = open("c:\\data\\javaredirect.log", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
}

void LogRedirector::stop()
{
    iJavaDiagnostic->removeDiagnosticListener();
    close(mFd);
    mFd = -1;
}

void LogRedirector::systemOut(const TDesC8& aData)
{
    write(aData);
}

void LogRedirector::systemErr(const TDesC8& aData)
{
    write(aData);
}

void LogRedirector::log(const TDesC8& aData)
{
    write(aData);
}

void LogRedirector::write(const TDesC8& aData)
{
    if (mFd > 0)
    {
        ::write(mFd, (const char*)aData.Ptr(), aData.Size());
    }
};

