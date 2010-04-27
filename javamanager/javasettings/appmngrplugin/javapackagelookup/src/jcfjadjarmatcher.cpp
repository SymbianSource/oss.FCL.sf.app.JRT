/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition. This class is used to find matching JAD file for JAR file
*                or vice versa. This class is derived from CActive.
*
*/


#include "jcfjadjarmatcher.h"
#include "jcfjadjarmatcherstates.h"
#include "jcfjadjarmatcherscanjadfiles.h"
#include "jcfjadjarmatcherscanjarfiles.h"
#include "logger.h"


// ---------------------------------------------------------------------------
//    To create a new CJcfJadJarMatcher.
// -----------------------------------------------------------------------------
CJcfJadJarMatcher* CJcfJadJarMatcher::NewL(MJcfJadJarMatcherObserver* aObs,
        RFs& aFileSession)
{
    CJcfJadJarMatcher* self = new(ELeave) CJcfJadJarMatcher(aObs, aFileSession);
    return self;
}


// ---------------------------------------------------------------------------
//    To destruct CJcfJadJarMatcher.
// -----------------------------------------------------------------------------
CJcfJadJarMatcher::~CJcfJadJarMatcher()
{
    if (iScanJad)
    {
        delete iScanJad;
    }
    if (iScanJar)
    {
        delete iScanJar;
    }
}


// ---------------------------------------------------------------------------
//    To construct new CJcfJadJarMatcher object.
// -----------------------------------------------------------------------------
CJcfJadJarMatcher::CJcfJadJarMatcher(MJcfJadJarMatcherObserver*  aObs,
                                     RFs&  aFileSession) : iFs(aFileSession)
{
    AddObserver(aObs);
}

// ---------------------------------------------------------------------------
//    To find matching JAD file for given JAR file from public folders.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcher::MatchJadL(const TDesC&  aJarFile,
                                  const TDesC&  aDir,
                                  TDes*  aJadFile)
{
    JELOG2(EJavaStorage);

    if (NULL == iScanJad)
    {
        iScanJad = new(ELeave) CJcfJadJarMatcherScanJadFiles(this, iFs);
    }
    iScanJad->ExecuteL(aJarFile, aDir, aJadFile);
}


// ---------------------------------------------------------------------------
//    To find matching JAR file for given JAD file from public folders.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcher::MatchJarL(const TDesC&  aJadFile,
                                  const TDesC&  aDir,
                                  TDes*  aJarFile)
{
    JELOG2(EJavaStorage);

    if (NULL == iScanJar)
    {
        iScanJar = new(ELeave) CJcfJadJarMatcherScanJarFiles(this, iFs);
    }
    iScanJar->ExecuteL(aJadFile, aDir, aJarFile);
}


// ---------------------------------------------------------------------------
//    This method is called whenever observed object needs to.
// -----------------------------------------------------------------------------
void CJcfJadJarMatcher::Update(TInt aStatus)
{
    NotifyObserver(aStatus);
}

