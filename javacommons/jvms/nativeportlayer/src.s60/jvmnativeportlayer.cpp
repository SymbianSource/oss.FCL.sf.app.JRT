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
* Description:  CJvmNativePortLayer
*
*/


#include <flogger.h>

#include "logger.h"
#include "mjvmnativeportlayer.h"
#include "mjavafile.h"
#include "javafile.h"
#include "jvmnativeportlayer.h"

using namespace javaruntime;

MJvmNativePortLayer* CJvmNativePortLayer::NewL(void)
{
    JELOG2(EJVM);
    CJvmNativePortLayer* portLayer = new(ELeave) CJvmNativePortLayer;
    CleanupStack::PushL(portLayer);
    portLayer->ConstructL();
    CleanupStack::Pop(portLayer);
    return portLayer;
}

CJvmNativePortLayer::CJvmNativePortLayer():iStdOut(0), iStdErr(0)
{
    JELOG2(EJVM);
}

CJvmNativePortLayer::~CJvmNativePortLayer()
{
    JELOG2(EJVM);
    if (iStdOut != 0)
    {
        delete iStdOut;
        iStdOut = 0;
    }
    if (iStdErr != 0)
    {
        delete iStdErr;
        iStdErr = 0;
    }
    iRedirector.close();
}


_LIT(KOutpuDir,"java");
_LIT(KOutpuFullDir,"c:\\logs\\java");
_LIT(KConsole,"vmconsole.txt");

void CJvmNativePortLayer::ConstructL(void)
{
    JELOG2(EJVM);
    _LIT(KStdOut,"stdout_%02i%02i_%02i%02i%02i.txt");
    _LIT(KStdErr,"stderr_%02i%02i_%02i%02i%02i.txt");
    iStdOut = HBufC::NewL(KDesLength);
    iStdErr = HBufC::NewL(KDesLength);

    RFs rfs;
    TUint dummy;

    if (rfs.Connect() != KErrNone)
    {
        iLogging = EFalse;
        return;
    }
    if (rfs.Att(KOutpuFullDir, dummy) != KErrNone)
    {
        iLogging = EFalse;
    }
    else
    {
        iLogging = ETrue;
        TTime time;
        time.HomeTime();
        TDateTime dt = time.DateTime();

        iStdOut->Des().Format(KStdOut, dt.Month()+1, dt.Day()+1,
                              dt.Hour(), dt.Minute(), dt.Second());
        iStdErr->Des().Format(KStdErr, dt.Month()+1, dt.Day()+1,
                              dt.Hour(), dt.Minute(), dt.Second());
    }
    rfs.Close();

    iRedirector.connect();
}

MJavaFile* CJvmNativePortLayer::GetJavaFileL()
{
    JELOG2(EJVM);
    MJavaFile* javaFile = CJavaFile::NewL();
    return javaFile;
}

void CJvmNativePortLayer::WriteToStdoutL(const TDesC8& aData)
{
    if (iLogging)
    {
//              RFileLogger::Write( KOutpuDir, *iStdOut, EFileLoggingModeAppendRaw, aData);
        RFileLogger::Write(KOutpuDir, KConsole, EFileLoggingModeAppendRaw, aData);
    }
    iRedirector.systemOut(aData);
}
void CJvmNativePortLayer::WriteToStderrL(const TDesC8& aData)
{
    if (iLogging)
    {
//              RFileLogger::Write( KOutpuDir, *iStdErr, EFileLoggingModeAppendRaw, aData);
        RFileLogger::Write(KOutpuDir, KConsole, EFileLoggingModeAppendRaw, aData);
    }
    iRedirector.systemErr(aData);
}



//Obsolite method, but keeping to be in sync with legacy
EXPORT_C void CJvmNativePortLayer::SetPointerToCafMethod(TBool(* /*aUseCafForFileOpen*/)(const TDesC&))
{
    JELOG2(EJVM);
}
EXPORT_C MJvmNativePortLayer* GetPortLayerL()
{
    JELOG2(EJVM);
    return CJvmNativePortLayer::NewL();
}
const TThreadPriority priorityMapping[] =
{
    EPriorityMuchLess, //0
    EPriorityMuchLess, //1
    EPriorityMuchLess, //2
    EPriorityMuchLess, //3
    EPriorityMuchLess, //4
    EPriorityLess,     //5
    EPriorityLess,     //6
    EPriorityLess,     //7
    EPriorityLess,     //8
    EPriorityNormal,   //9
    EPriorityNormal,   //10
    EPriorityMore,     //11
};
EXPORT_C TThreadPriority GetSingleThreadPriority(const TInt& aJavaPriority)
{
    JELOG2(EJVM);
    TInt index = Max(Min(aJavaPriority,11),0);
    return priorityMapping[index];
}

