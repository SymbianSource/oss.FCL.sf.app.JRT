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
* Description:  MidletPackageInfo Factory implementation
*
*/


#include <caf/caf.h>                         // ContentAccess::CContent

#include "appmngr2midletpckginfofactory.h"   // CAppMngr2MidletPakageInfoFactory
#include "appmngr2midletpackageinfo.h"       // CAppMngr2MidletPackageInfo
#include "appmngr2recognizedfile.h"          // CAppMngr2RecognizedFile
#include "appmngr2midletresourcehandler.h" // CAppMngr2MidletResourceHandler

#include "logger.h"                          // LOG


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2MidletPackageInfo::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletPckgInfoFactory* CAppMngr2MidletPckgInfoFactory::NewL(
    CAppMngr2Runtime& aRuntime, RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletPckgInfoFactory::NewL ");
    CAppMngr2MidletPckgInfoFactory* self
    = new(ELeave) CAppMngr2MidletPckgInfoFactory(aRuntime, aFs, aResourceHandler);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletPckgInfoFactory::NewL ");
    return self;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::~CAppMngr2MidletPckgInfoFactory()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletPckgInfoFactory::~CAppMngr2MidletPckgInfoFactory()
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::~CAppMngr2MidletPckgInfoFactory ");
    Cancel();
    delete iMatcher;
    iMatcher = NULL;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::GetInstallationFilesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory::GetInstallationFilesL(
    RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
    const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
    TRequestStatus& aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletPckgInfoFactory::GetInstallationFilesL ");

    if (IsActive())
    {
        User::Leave(KErrInUse);
    }

    iCounter = 0;
    iState = ENextPckg;
    iPackageInfos = &aPackageInfos;
    iFileList = &aFileList;
    iExtStatus = &aStatus;
    *iExtStatus = KRequestPending;
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::GetInstallationFilesL:SetActive()");
    SetActive();
    CompleteRequest(iStatus, KErrNone);

    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletPckgInfoFactory::GetInstallationFilesL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::RunL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory::RunL()
{
    LOG1(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletPckgInfoFactory::RunL, iState: %d ", iState);

    CAppMngr2PackageInfo* packageInfo = NULL;

    switch (iState)
    {
    case ENextPckg :
    {
        if (iCounter < iFileList->Count())
        {
            LOG1(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::RunL:ENextPckg: iCounter = %d", iCounter);

            // skip processing if this files was already matched
            TBool found(EFalse);
            CAppMngr2RecognizedFile* recognized = (*iFileList)[iCounter]; // not owned
            for (TInt i(0); i < iPackageInfos->Count(); i++)
            {
                CAppMngr2MidletPackageInfo* package
                = static_cast<CAppMngr2MidletPackageInfo*>((*iPackageInfos)[i]); // not owned

                if ((recognized->FileName() == package->JadFileName())
                        || (recognized->FileName() == package->JarFileName()))
                {
                    found = ETrue;
                    break;
                }
            }

            if (found)
            {
                // Continue with next file
                iCounter++;
                iState = ENextPckg;
                CompleteRequest(iStatus, KErrNone);
            }
            else
            {
                // Get installation file
                // Counter must be incremented before leave
                TInt index = iCounter;
                iCounter++;
                GetInstallationFileL((*iFileList)[index]->FileName(),
                                     (*iFileList)[index]->DataType());
                // GetInstallationFileL() changes iState
            }

            LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::RunL:ENextPckg:SetActive()");
            SetActive();
        }
        else
        {
            // File name list has been processed
            // Let's complete GetInstallationFilesL
            User::RequestComplete(iExtStatus, KErrNone);
        }
        break;
    }
    case EMatchedJar :
    {
        if (iJadFile.Length() == 0)
        {
            // Standalone JAR
            iJadFile = KNullDesC;
        }
        iState = EAppendPckg; // Next state
        LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::RunL:EMatchedJar:SetActive()");
        SetActive();
        CompleteRequest(iStatus, KErrNone);
        break;
    }
    case EMatchedJad :
    {
        if (iJarFile.Length() == 0)
        {
            // Standalone JAD
            iJarFile = KNullDesC;
        }
        iState = EAppendPckg; // Next state
        LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::RunL:EMatchedJad:SetActive()");
        SetActive();
        CompleteRequest(iStatus, KErrNone);
        break;
    }
    case EAppendPckg :
    {
        packageInfo = CAppMngr2MidletPackageInfo::NewL(
                          iRuntime, iJadFile, iJarFile, iFs, iResourceHandler);
        CleanupStack::PushL(packageInfo);
        iPackageInfos->AppendL(packageInfo);

        CleanupStack::Pop(packageInfo);
        iState = ENextPckg; // Next state
        LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::RunL:EAppendPckg:SetActive()");
        SetActive();
        CompleteRequest(iStatus, KErrNone);
        break;
    }
    default :
    {
        LOG1(EJavaAppMngrPlugin, EInfo, " Incorrect state, iCounter = %d", iCounter);

        User::RequestComplete(iExtStatus, KErrGeneral);
        break;
    }
    }

    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletPckgInfoFactory::RunL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::DoCancel()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory::DoCancel()
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::DoCancel ");
    CancelMatching();
    if (iExtStatus->Int() == KRequestPending)
    {
        User::RequestComplete(iExtStatus, KErrCancel);
    }
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::RunError()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2MidletPckgInfoFactory::RunError(TInt /*aError*/)
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::RunError ");
    // Creation of pckg info fails continue with next one
    iState = ENextPckg; // Next state
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::RunError:SetActive()");
    SetActive();
    CompleteRequest(iStatus, KErrGeneral);
    return KErrNone;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::GetInstallationFileL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory
::GetInstallationFileL(const TDesC& aFileName, const TDataType& aDataType)

{
    LOG(EJavaAppMngrPlugin, EInfo, "+ CAppMngr2MidletPckgInfoFactory::GetInstallationFileL ");

    TInt err(KErrNone);
    iJadFile.Zero();
    iJarFile.Zero();

    // Check MIME type
    TPtrC8 mimePtr8(aDataType.Des8());
    if (mimePtr8 == SwiUI::KJarMIMEType)
    {
        // JAR
        iJarFile = aFileName;
        // Matching JAR's JAD. MatchL is internally asynchronous
        TRAP(err, MatchL(iJarFile, SwiUI::KJarMIMEType, iJadFile, iStatus));

        if (err != KErrNone)
        {
            // Corrupted JAR, allow to create standalone package
            CancelMatching();
            iState = EAppendPckg; // Next state
            CompleteRequest(iStatus, KErrNone);
        }
        iState = EMatchedJar; // Next state
    }
    else if (mimePtr8 == SwiUI::KJadMIMEType)
    {
        // JAD
        iJadFile = aFileName;
        // Matching JAD's JAR. MatchL is internally asynchronous
        TRAP(err, MatchL(iJadFile, SwiUI::KJadMIMEType, iJarFile, iStatus));

        if (err != KErrNone)
        {
            // Corrupted JAD, allow to create standalone package
            CancelMatching();
            iState = EAppendPckg; // Next state
            CompleteRequest(iStatus, KErrNone);
        }
        iState = EMatchedJad; // Next state
    }
    else
    {
        // File name is not recognized
        User::Leave(KErrGeneral);
    }

    LOG(EJavaAppMngrPlugin, EInfo, "- CAppMngr2MidletPckgInfoFactory::GetInstallationFileL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::CAppMngr2MidletPckgInfoFactory()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletPckgInfoFactory
::CAppMngr2MidletPckgInfoFactory(CAppMngr2Runtime& aRuntime, RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler) :
        CActive(CActive::EPriorityStandard), iFs(aFs), iRuntime(aRuntime), iResourceHandler(aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::CAppMngr2MidletPckgInfoFactory ");
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory::ConstructL()
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::ConstructL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::CompleteRequest()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory::CompleteRequest(TRequestStatus& aRequestStatus,
        TInt aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletPckgInfoFactory::CompleteRequest ");
    TRequestStatus* status = &aRequestStatus;
    User::RequestComplete(status, aStatus);
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::MatchL()
// -----------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory::MatchL(const TDesC& aMatchingFileName,
        const TDesC8& aMatchingMimeType,
        TDes& aMatchedFileName,
        TRequestStatus &aStatus)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPckgInfoFactory::MatchL ");
    TParse parser;
    User::LeaveIfError(parser.Set(aMatchingFileName, NULL, NULL));

    if (!iMatcher)
    {
        iMatcher = CJcfJadJarMatcher::NewL(this, iFs);
    }

    if (aMatchingMimeType == SwiUI::KJarMIMEType)
    {
        iMatcher->MatchJadL(
            aMatchingFileName, parser.DriveAndPath(), &aMatchedFileName);
    }
    else if (aMatchingMimeType == SwiUI::KJadMIMEType)
    {
        iMatcher->MatchJarL(
            aMatchingFileName, parser.DriveAndPath(), &aMatchedFileName);
    }
    else
    {
        User::Leave(KErrGeneral);
    }

    aStatus = KRequestPending;
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPckgInfoFactory::MatchL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::CancelMathing()
// -----------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory::CancelMatching()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletPckgInfoFactory::CancelMatching ");
    delete iMatcher;
    iMatcher = NULL;
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletPckgInfoFactory::CancelMatching ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletPckgInfoFactory::Update()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletPckgInfoFactory::Update(TInt aArg1)
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletPckgInfoFactory::UpdateL ");
    CompleteRequest(iStatus, aArg1);
}
