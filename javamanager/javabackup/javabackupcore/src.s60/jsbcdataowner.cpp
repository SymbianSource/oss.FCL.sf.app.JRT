/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of JsbcDataOwner class
*
*/


#include "jsbcdataowner.h"
#include "jsbcactive.h"
#include "backupplugin.h"
#include "logger.h"

using namespace java::backup;

_LIT8(KMidpBackupPluginString, "midp2");
// ======== MEMBER FUNCTIONS ========

CJsbcDataOwner::CJsbcDataOwner()
{
    LOG(EBackup, EInfo, "CJsbcDataOwner constructor");
}

void CJsbcDataOwner::ConstructL(CJsbcActive* aJsbcActive)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::ConstructL");
    iPlugin = CBackupPlugin::NewL(KMidpBackupPluginString);
    iJsbcActive = aJsbcActive;
}

CJsbcDataOwner* CJsbcDataOwner::NewL(CJsbcActive* aJsbcActive)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::NewL");
    CJsbcDataOwner* self = CJsbcDataOwner::NewLC(aJsbcActive);
    CleanupStack::Pop(self);
    return self;
}

CJsbcDataOwner* CJsbcDataOwner::NewLC(CJsbcActive* aJsbcActive)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::NewLC");
    CJsbcDataOwner* self = new(ELeave) CJsbcDataOwner;
    CleanupStack::PushL(self);
    self->ConstructL(aJsbcActive);
    return self;
}

CJsbcDataOwner::~CJsbcDataOwner()
{
    LOG(EBackup, EInfo, "CJsbcDataOwner destructor");
    delete iPlugin;
    REComSession::FinalClose();
}

void CJsbcDataOwner::PrepareForBURL(TInt aBackupStateValue)
{
    LOG1(EBackup, EInfo, "CJsbcDataOwner::PrepareForBURL, BUR state: %d", aBackupStateValue);
    iPlugin->PrepareForBURL(aBackupStateValue);

}

void CJsbcDataOwner::AllSnapshotsSuppliedL()
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::AllSnapshotsSuppliedL");
    User::Leave(KErrNotSupported);
}


void CJsbcDataOwner::ReceiveSnapshotDataL(TDriveNumber /* aDrive */, TDesC8& /* aBuffer */, TBool /* aLastSection */)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::ReceiveSnapshotDataL");
    User::Leave(KErrNotSupported);
}

TUint CJsbcDataOwner::GetExpectedDataSize(TDriveNumber aDrive)
{
    LOG1(EBackup, EInfo, "CJsbcDataOwner::GetExpectedDataSize, Drive: %d", aDrive);
    return iPlugin->GetExpectedDataSize(aDrive);
}

void CJsbcDataOwner::GetSnapshotDataL(TDriveNumber /* aDrive */, TPtr8& /* aBuffer */, TBool& /* aFinished */)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::GetSnapshotDataL+++++++++++");
    User::Leave(KErrNotSupported);
}

void CJsbcDataOwner::InitialiseGetBackupDataL(TDriveNumber aDrive)
{
    LOG1(EBackup, EInfo, "CJsbcDataOwner::InitialiseGetBackupDataL, Drive: %d", aDrive);
    iPlugin->InitialiseGetBackupDataL(aDrive);

}

void CJsbcDataOwner::GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::GetBackupDataSectionL");
    iPlugin->GetBackupDataSectionL(aBuffer, aFinished);

}

void CJsbcDataOwner::InitialiseRestoreBaseDataL(TDriveNumber aDrive)
{
    LOG1(EBackup, EInfo, "CJsbcDataOwner::InitialiseRestoreBaseDataL, Drive: %d", aDrive);
    iPlugin->InitialiseRestoreBaseDataL(aDrive);

}

void CJsbcDataOwner::RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::RestoreBaseDataSectionL");
    iPlugin->RestoreBaseDataSectionL(aBuffer, aFinished);

}

void CJsbcDataOwner::InitialiseRestoreIncrementDataL(TDriveNumber /* aDrive */)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::InitialiseRestoreIncrementDataL");
    User::Leave(KErrNotSupported);
}

void CJsbcDataOwner::RestoreIncrementDataSectionL(TDesC8& /* aBuffer */, TBool /* aFinished */)
{
    LOG(EBackup, EInfo, "CJsbcDataOwner::RestoreIncrementDataSectionL+");
    User::Leave(KErrNotSupported);
}

void CJsbcDataOwner::RestoreComplete(TDriveNumber aDrive)
{
    LOG1(EBackup, EInfo, "CJsbcDataOwner::RestoreComplete, Drive: %d", aDrive);
    iPlugin->RestoreComplete(aDrive);

}

void CJsbcDataOwner::TerminateMultiStageOperation()
{
    LOG(EBackup,EInfo, "CJsbcDataOwner::TerminateMultiStageOperation");
}

TUint CJsbcDataOwner::GetDataChecksum(TDriveNumber /* aDrive */)
{
    LOG(EBackup,EInfo,"CJsbcDataOwner::GetDataChecksum");
    return 0;
}


