/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
    portLayer->Construct();
    return portLayer;
}

CJvmNativePortLayer::CJvmNativePortLayer()
{
    JELOG2(EJVM);
}

CJvmNativePortLayer::~CJvmNativePortLayer()
{
    JELOG2(EJVM);
    iRedirector.close();
}


_LIT(KOutpuDir,"java");
_LIT(KOutpuFullDir,"c:\\logs\\java");
_LIT(KConsole,"vmconsole.txt");

void CJvmNativePortLayer::Construct()
{
    JELOG2(EJVM);

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
    }
    rfs.Close();

    iRedirector.connect();
}

MJavaFile* CJvmNativePortLayer::GetJavaFileL()
{
    JELOG2(EJVM);
    return new(ELeave) CJavaFile();
}

void CJvmNativePortLayer::WriteToStdoutL(const TDesC8& aData)
{
    if (iLogging)
    {
        RFileLogger::Write(KOutpuDir, KConsole, EFileLoggingModeAppendRaw, aData);
    }
    iRedirector.systemOut(aData);
}
void CJvmNativePortLayer::WriteToStderrL(const TDesC8& aData)
{
    if (iLogging)
    {
        RFileLogger::Write(KOutpuDir, KConsole, EFileLoggingModeAppendRaw, aData);
    }
    iRedirector.systemErr(aData);
}


EXPORT_C MJvmNativePortLayer* javaruntime::GetPortLayerL()
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

EXPORT_C TThreadPriority javaruntime::GetSingleThreadPriority(const TInt& aJavaPriority)
{
    JELOG2(EJVM);
    TInt index = Max(Min(aJavaPriority,11),0);
    return priorityMapping[index];
}

