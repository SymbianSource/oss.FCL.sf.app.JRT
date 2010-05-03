/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java Size helper implementation
*
*/


#include "javasizehelpsession.h"
#include "javasizehelpserver.h"
#include "javasizehelpcommon.h"
#include "logger.h"

/**
 * Defines directory which can be udes
 */
_LIT(KDirAllowed,":\\private\\102033E6\\apps\\");
_LIT(KDirPath,"\\");


// ---------------------------------------------------------------------------
// CJavaSizeHelpSession::CJavaSizeHelpSession
// ---------------------------------------------------------------------------
CJavaSizeHelpSession::CJavaSizeHelpSession()
{
    JELOG2(EUtils);
}

// ---------------------------------------------------------------------------
// CJavaSizeHelpSession::Create
// ---------------------------------------------------------------------------
void CJavaSizeHelpSession::Create()
{
    JELOG2(EUtils);
    Server().AddSession();
}

// ---------------------------------------------------------------------------
// CJavaSizeHelpSession::~CJavaSizeHelpSession
// ---------------------------------------------------------------------------
CJavaSizeHelpSession::~CJavaSizeHelpSession()
{
    JELOG2(EUtils);
    Server().RemoveSession();
}

// ---------------------------------------------------------------------------
// CJavaSizeHelpSession::Server
// ---------------------------------------------------------------------------
CJavaSizeHelpServer& CJavaSizeHelpSession::Server()
{
    JELOG2(EUtils);
    return *static_cast<CJavaSizeHelpServer*>(const_cast<CServer2*>(CSession2::Server()));
}

// ---------------------------------------------------------------------------
// CJavaSizeHelpSession::ServiceL
// ---------------------------------------------------------------------------
void CJavaSizeHelpSession::ServiceL(const RMessage2& aMessage)
{
    JELOG2(EUtils);
    switch (aMessage.Function())
    {
    case EGetUserUsedDiskSpace:
        GetUserUsedDiskSpaceL(aMessage);
        break;

    default:
        aMessage.Complete(KErrNotSupported);
        break;
    }
}

// ---------------------------------------------------------------------------
// CJavaSizeHelpSession::GetUserUsedDiskSpaceL
// ---------------------------------------------------------------------------
void CJavaSizeHelpSession::GetUserUsedDiskSpaceL(const RMessage2& aMessage)
{
    JELOG2(EUtils);
    // Read directory path from client.
    TUint length = aMessage.GetDesLength(0);
    HBufC* descDir = HBufC::NewLC(length);
    TPtr descDirPtr(descDir->Des());
    aMessage.ReadL(0, descDirPtr);

    // Leave if access to not allowed directory
    if (KErrNotFound == descDirPtr.Find(KDirAllowed))
    {
        User::Leave(KErrAccessDenied);
    }

    TInt64 sizeSum = 0;

    //Connect to file server
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Array for direcories and subdirectories.
    const TInt KArrayGranularity(4);
    CDesC16ArrayFlat* dirNameList = new(ELeave) CDesC16ArrayFlat(KArrayGranularity);
    CleanupStack::PushL(dirNameList);
    dirNameList->AppendL(descDirPtr);

    while (dirNameList->Count() > 0)
    {
        CDir *fileList;
        CDir *dirList;

        User::LeaveIfError(fs.GetDir((*dirNameList)[0], KEntryAttNormal | KEntryAttSystem
                                     | KEntryAttHidden, ESortNone, fileList, dirList));
        CleanupStack::PushL(fileList);
        CleanupStack::PushL(dirList);

        //Remember first directory from array
        HBufC* temDirbuf = HBufC::NewLC((*dirNameList)[0].Length());
        TPtr temDirbufPtr = temDirbuf->Des();
        temDirbuf->Des().Append((*dirNameList)[0]);

        //Remove first directory from array
        dirNameList->Delete(0,1);

        //Add to begin of array directories from
        //current processed directory
        TEntry entryDir;
        for (TInt j=0; j<dirList->Count(); j++)
        {
            entryDir =(*dirList)[j];

            TDesC& dirName = entryDir.iName;
            HBufC* dirNameBuf = HBufC::NewLC((*temDirbuf).Length() + dirName.Length()+ 2);
            TPtr dirNameBufPtr = dirNameBuf->Des();

            dirNameBuf->Des().Append((*temDirbuf));
            dirNameBuf->Des().Append(dirName);
            dirNameBuf->Des().Append(KDirPath);

            dirNameList->InsertL(j, *dirNameBuf);
            CleanupStack::PopAndDestroy(dirNameBuf);
        }

        //Count all files size from current directory
        TEntry entryFiles;
        for (TInt i=0; i< fileList->Count(); i++)
        {
            entryFiles =(*fileList)[i];
            TInt fileSize = entryFiles.iSize;
            sizeSum +=fileSize;
        }


        CleanupStack::PopAndDestroy(temDirbuf);
        CleanupStack::PopAndDestroy(dirList);
        CleanupStack::PopAndDestroy(fileList);

    }

    //Send files size amount to client
    aMessage.Complete(sizeSum);

    CleanupStack::PopAndDestroy(dirNameList);
    CleanupStack::PopAndDestroy(&fs);
    CleanupStack::PopAndDestroy(descDir);

}
