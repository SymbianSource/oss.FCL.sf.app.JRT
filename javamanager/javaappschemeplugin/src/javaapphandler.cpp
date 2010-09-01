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
* Description:  Implementation of Scheme handler interface implementation
*      for localapp://jam/launch scheme
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <escapeutils.h>
#include <apgcli.h>
#include <eikproc.h>
#include <apacmdln.h>
#include <e32const.h>
#include <flogger.h>

#include "javaapphandler.h"

using namespace java;

namespace java
{

namespace javaapphandler
{

// ================= CONSTANTS =======================

_LIT(KJavaLogDir, "java\\full");
_LIT(KJavaLogFile, "JavaUtils.log");

// ================= MACROS =======================

// Define logging macros here to remove dependencies to Java 2.0 libraries.
#define LOG(str)  { TBuf8<1024> f((const TUint8 *)str); RFileLogger::Write(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f); }
#define LOG1(str, a)  { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a); }
#define LOG1WSTR(str, wstr) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::Write(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f); TBuf16<KLogBufferSize> w((const TUint16 *)wstr); RFileLogger::Write(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, w); }

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CJavaAppHandler::CJavaAppHandler()
// ---------------------------------------------------------
//
CJavaAppHandler::CJavaAppHandler()
        : CSchemeHandler()
{
}

// ---------------------------------------------------------
// CJavaAppHandler::ConstructL()
// ---------------------------------------------------------
//
void CJavaAppHandler::ConstructL(const TDesC& aUrl)
{
    mParsedUrl = aUrl.AllocL();
}

// ---------------------------------------------------------
// CJavaAppHandler::Observer()
// ---------------------------------------------------------
//

void CJavaAppHandler::Observer(MAknServerAppExitObserver* /* aSchemeDoc */)
{
    // do nothing
}

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CJavaAppHandler::NewL()
// ---------------------------------------------------------
//
CJavaAppHandler* CJavaAppHandler::NewL(const TDesC& aUrl)
{
    CJavaAppHandler* self=new(ELeave) CJavaAppHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aUrl);
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------
// CJavaAppHandler::~CJavaAppHandler()
// ---------------------------------------------------------
//
CJavaAppHandler::~CJavaAppHandler()
{
    delete mParsedUrl;
}

// ---------------------------------------------------------
// CJavaAppHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CJavaAppHandler::HandleUrlEmbeddedL()
{
    HandleUrlStandaloneL();
}

// ---------------------------------------------------------
// CJavaAppHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CJavaAppHandler::HandleUrlStandaloneL()
{


    // Start javalauncher.exe and pass the Url to it
    _LIT(KJavaLauncherExe, "javalauncher.exe");
    RProcess rProcess;

    // start
    TInt err = rProcess.Create(KJavaLauncherExe, *mParsedUrl);
    if (KErrNone == err)
    {
        // This call will wait until javalauncher exits (or panics)
        TRequestStatus status;
        rProcess.Logon(status);
        rProcess.Resume();

        // now wait until javalauncher exits
        User::WaitForRequest(status);
        err = status.Int();
        if (err != KErrNone)
        {
            LOG1("javaapphandler: javalauncher exited with error %d", err);
        }
    }
    else
    {
        LOG1("javaapphandler: Cannot create javalauncher process, error %d", err);
    }

    // free resources before returning
    rProcess.Close();

    if (KErrNone != err)
    {
#ifndef _DEBUG
        // Make sure Url is logged always if an error has happened
        TBuf<255> logBuf = mParsedUrl->Left(250);
        LOG1WSTR("javaapphandler: command line starts with : %s", (TUint16 *)(logBuf.PtrZ()));
#endif
        User::Leave(err);
    }
}

} // namespace javaapphandler
} // namespace java
