/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CInstalledAppsRegistryFiles class.
*
*/


#include "installedappsregistryfiles.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::NewLC
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles::CDirFiles*
CInstalledAppsRegistryFiles::CDirFiles::NewLC(const TDesC& aDirName)
{
    CDirFiles* self = new(ELeave) CDirFiles;
    CleanupStack::PushL(self);
    self->ConstructL(aDirName);
    return self;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::NewLC
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles::CDirFiles*
CInstalledAppsRegistryFiles::CDirFiles::NewLC
(const CInstalledAppsRegistryFiles::CDirFiles& aOther)
{
    CDirFiles* self = new(ELeave) CDirFiles;
    CleanupStack::PushL(self);
    self->ConstructL(aOther);
    return self;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::NewLC
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles::CDirFiles*
CInstalledAppsRegistryFiles::CDirFiles::NewLC(RReadStream& aStream)
{
    CDirFiles* self = new(ELeave) CDirFiles;
    CleanupStack::PushL(self);
    self->ConstructL(aStream);
    return self;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::CDirFiles
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles::CDirFiles::CDirFiles()
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::CDirFiles::ConstructL
(const TDesC& /*aDirName*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::CDirFiles::ConstructL
(const CInstalledAppsRegistryFiles::CDirFiles& /*aOther*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::CDirFiles::ConstructL
(RReadStream& /*aStream*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::ExternalizeL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::CDirFiles::ExternalizeL
(RWriteStream& /*aStream*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::SizeForStreaming
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryFiles::CDirFiles::SizeForStreamingL() const
{
    User::Leave(KErrNotSupported);
    return 0;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::~CDirFiles
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles::CDirFiles::~CDirFiles()
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::DirectoryName
// ---------------------------------------------------------------------------
//
const TDesC& CInstalledAppsRegistryFiles::CDirFiles::DirectoryName() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::AddFileL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::CDirFiles::AddFileL(
    const TDesC& /*aFileName*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::Exists
// ---------------------------------------------------------------------------
//
TBool CInstalledAppsRegistryFiles::CDirFiles::Exists
(const TDesC& /*aFileName*/) const
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::ListFilesL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::CDirFiles::ListFilesL
(RPointerArray<HBufC>& /*aFileArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::SetOwned
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::CDirFiles::SetOwned(TBool /*aIsOwned*/)
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::IsOwned
// ---------------------------------------------------------------------------
//
TBool CInstalledAppsRegistryFiles::CDirFiles::IsOwned() const
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::FindFileWithExtL
// ---------------------------------------------------------------------------
//
TBool CInstalledAppsRegistryFiles::CDirFiles::FindFileWithExtL
(const TDesC& /*aExtension*/,
 TFileName& /*aFileName*/) const
{
    User::Leave(KErrNotSupported);
    return EFalse;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::FileCount
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryFiles::CDirFiles::FileCount() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::Size
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryFiles::CDirFiles::Size(RFs& /*aFs*/) const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CDirFiles::SizeOfFile
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryFiles::CDirFiles::SizeOfFile(RFs& /*aFs*/,
        const TDesC& /*aFileName*/) const
{
    return 0;
}


// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::NewL
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles* CInstalledAppsRegistryFiles::NewL()
{
    return new(ELeave) CInstalledAppsRegistryFiles;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::NewL
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles* CInstalledAppsRegistryFiles::NewL
(RReadStream& aStream)
{
    CInstalledAppsRegistryFiles* self =
        new(ELeave) CInstalledAppsRegistryFiles;
    CleanupStack::PushL(self);
    self->ConstructL(aStream);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::NewL
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles* CInstalledAppsRegistryFiles::NewL
(const CInstalledAppsRegistryFiles& aOther)
{
    CInstalledAppsRegistryFiles* self =
        new(ELeave) CInstalledAppsRegistryFiles;
    CleanupStack::PushL(self);
    self->ConstructL(aOther);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::ConstructL
(const CInstalledAppsRegistryFiles& /*aOther*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::CInstalledAppsRegistryFiles
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles::CInstalledAppsRegistryFiles()
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::~CInstalledAppsRegistryFiles
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles::~CInstalledAppsRegistryFiles()
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::AddFileL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::AddFileL(const TDesC& /*aFileName*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::ListFilesL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::ListFilesL
(RPointerArray<HBufC>& /*aFileArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::AddDirectoryL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::AddDirectoryL(const TDesC& /*aDirName*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::ListDirectoriesL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::ListDirectoriesL
(RPointerArray<HBufC>& /*aDirArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// FoldedTextOrder
// ---------------------------------------------------------------------------
//
// LOCAL_C TInt FoldedTextOrder( const HBufC& /*aLeft*/, const HBufC& /*aRight*/ )
//    {
//    return -1;
//    }

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::AddApparcIdentifierL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::AddApparcIdentifierL
(const TDesC& /*aApparcIdentifier*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::ListApparcIdentifiersL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::ListApparcIdentifiersL
(RPointerArray<HBufC>& /*aApparcIdentifiersArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::FindFileWithExtL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::FindFileWithExtL(
    const TDesC& /*aExtension*/, TFileName& /*aFileName*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::UsesFileL
// ---------------------------------------------------------------------------
//
TBool CInstalledAppsRegistryFiles::UsesFileL(
    const TDesC& /*aFileName*/) const
{
    User::Leave(KErrNotSupported);
    return EFalse;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::Size
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryFiles::Size() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::FileCount
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryFiles::FileCount() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::GetDirectory
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles::CDirFiles*
CInstalledAppsRegistryFiles::GetDirectory(
    const TDesC& /*aDirectory*/) const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::ExternalizeL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::ExternalizeL(
    RWriteStream& /*aStream*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::SizeForStreaming
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryFiles::SizeForStreamingL() const
{
    User::Leave(KErrNotSupported);
    return 0;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryFiles::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryFiles::ConstructL(RReadStream& /*aStream*/)
{
    User::Leave(KErrNotSupported);
}
