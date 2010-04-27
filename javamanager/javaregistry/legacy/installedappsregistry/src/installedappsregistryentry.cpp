/*
* Copyright (c) 1997-2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CInstalledAppsRegistryEntry class.
*
*/


#include <javaregistryincludes.h>

#include "installedappsregistryentry.h"
#include "javacommonutils.h"
#include "javaregistryapplicationentry.h"
#include "javaregistrypackageentry.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "logger.h"

using namespace java::storage;
using namespace java::util;
using namespace std;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistryEntry* CInstalledAppsRegistryEntry::NewLC()
{
    CInstalledAppsRegistryEntry* self =
        new(ELeave) CInstalledAppsRegistryEntry;
    CleanupReleasePushL(*self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistryEntry* CInstalledAppsRegistryEntry::NewLC
(const CInstalledAppsRegistryEntry& aOther)
{
    CInstalledAppsRegistryEntry* self =
        new(ELeave) CInstalledAppsRegistryEntry(aOther);
    CleanupReleasePushL(*self);
    self->ConstructL(aOther);
    return self;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistryEntry* CInstalledAppsRegistryEntry::NewLC
(RReadStream& aStream)
{
    CInstalledAppsRegistryEntry* self =
        new(ELeave) CInstalledAppsRegistryEntry;
    CleanupReleasePushL(*self);
    self->ConstructL(aStream);
    return self;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistryEntry* CInstalledAppsRegistryEntry::NewL(
    Java::CJavaRegistryEntry* aRegistryEntry)
{
    CInstalledAppsRegistryEntry* self =
        new(ELeave) CInstalledAppsRegistryEntry;
    self->ConstructL(aRegistryEntry);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryEntry::~CInstalledAppsRegistryEntry()
{
    // Oops, someone called delete directly and not release if this fails
    ASSERT(!iRefCount);

    if (iRegistryEntry)
    {
        delete iRegistryEntry;
        iRegistryEntry = NULL;
    }
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::CInstalledAppsRegistryEntry
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryEntry::CInstalledAppsRegistryEntry()
        : iIsInRom(EFalse)
{
    AddRef();
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::CInstalledAppsRegistryEntry
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryEntry::CInstalledAppsRegistryEntry
(const CInstalledAppsRegistryEntry& /*aOther*/)
{
    // WARNING - make sure all member variables have been copied - some
    // of them (eg RArrays, heap objects) will need to be copied in the
    // ConstructL part
    AddRef();
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddRef
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddRef()
{
    ++iRefCount;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::Release
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::Release()
{
    // if the ref count goes to zero, then we delete
    if (!--iRefCount)
    {
        delete this;
    }
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::ConstructL()
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::ConstructL(
    Java::CJavaRegistryEntry* aRegistryEntry)
{
    iRegistryEntry = aRegistryEntry;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::ConstructL
(const CInstalledAppsRegistryEntry& /*aOther*/)
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::ConstructL(RReadStream& /*aStream*/,
        TBool /*aStoreFileList*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::InstalledFilesL
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistryFiles&
CInstalledAppsRegistryEntry::InstalledFilesL() const
{
    User::Leave(KErrNotSupported);
    return *iInstalledFiles;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::WriteL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::WriteL
(RWriteStream& /*aStream*/) const
{
    User::Leave(KErrNotSupported);
}

/////////////////////////////////////////////////////////////////////////////
// Attribute Setters
/////////////////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::SetUid
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::SetUid(const TUid& /*aUid*/)
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::SetVersion
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::SetVersion
(const TAppVersion& /*aVersion*/)
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::SetType
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::SetType(TInstallType /*aType*/)
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::SetDrive
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::SetDrive(TChar /*aDrive*/)
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddFileL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddFileL(const TDesC& /*aFileName*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddDirectoryL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddDirectoryL
(const TDesC& /*aDirName*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddApparcIdentifierL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddApparcIdentifierL
(const TDesC& /*aApparcIdentifier*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddCertificateChainL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddCertificateChainL
(const TDesC8& /*aCertChain*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddPropertyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddPropertyL(TUid /*aPropertyId*/,
        const TDesC8& /*aValue*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddLanguageL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddLanguageL(TLanguage /*aLang*/,
        const TDesC& /*aPackage*/,
        const TDesC& /*aVendor*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddDependencyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddDependencyL(TUid /*aUid*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::AddEmbeddedPackageL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistryEntry::AddEmbeddedPackageL
(TUid /*aUid*/)
{
    User::Leave(KErrNotSupported);
}

/////////////////////////////////////////////////////////////////////////////
// Attribute Getters
/////////////////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::UidL
// ---------------------------------------------------------------------------
//
TUid CInstalledAppsRegistryEntry::UidL() const
{
    JELOG2(EJavaStorage);
    return iRegistryEntry->Uid();
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::PackageNameL
// ---------------------------------------------------------------------------
//
const TDesC& CInstalledAppsRegistryEntry::PackageNameL
(TLanguage /*aLanguage*/) const
{
    User::Leave(KErrNotSupported);
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::VendorNameL
// ---------------------------------------------------------------------------
//
const TDesC& CInstalledAppsRegistryEntry::VendorNameL
(TLanguage /*aLanguage*/) const
{
    User::Leave(KErrNotSupported);
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::VersionL
// ---------------------------------------------------------------------------
//
TAppVersion CInstalledAppsRegistryEntry::VersionL() const
{
    JELOG2(EJavaStorage);

    TAppVersion appVersion(0, 0, 0);

    if (Java::EMidp2Midlet == iRegistryEntry->Type())
    {
        Java::CJavaRegistryPackageEntry* packageEntry =
            ((Java::CJavaRegistryApplicationEntry*)
             iRegistryEntry)->PackageEntryL();

        if (packageEntry)
        {
            appVersion = packageEntry->Version();
            delete packageEntry;
            packageEntry = NULL;
        }
    }
    else // Application suite
    {
        appVersion = ((Java::CJavaRegistryPackageEntry*)
                      iRegistryEntry)->Version();
    }

    return appVersion;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::DriveL
// ---------------------------------------------------------------------------
//
TChar CInstalledAppsRegistryEntry::DriveL() const
{
    JELOG2(EJavaStorage);
    TChar driveChar;
    User::LeaveIfError(RFs::DriveToChar(iRegistryEntry->Drive(), driveChar));
    return driveChar;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::TypeL
// ---------------------------------------------------------------------------
//
TInstallType CInstalledAppsRegistryEntry::TypeL() const
{
    return EMIDLETSuite;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::LanguagesL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::LanguagesL
(RArray<TLanguage>& /*aLangArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::LanguageSupportedL
// ---------------------------------------------------------------------------
//
TBool CInstalledAppsRegistryEntry::LanguageSupportedL
(TLanguage /*aLanguage*/) const
{
    User::Leave(KErrNotSupported);
    return EFalse;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::ListFilesL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::ListFilesL
(RPointerArray<HBufC>& /*aFileArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::FindFileWithExtL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::FindFileWithExtL
(const TDesC& /*aExtension*/,
 TFileName& /*aFileName*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::UsesFileL
// ---------------------------------------------------------------------------
//
TBool CInstalledAppsRegistryEntry::UsesFileL
(const TDesC& /*aFileName*/) const
{
    User::Leave(KErrNotSupported);
    return ETrue;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::IsInRomL
// ---------------------------------------------------------------------------
//
TBool CInstalledAppsRegistryEntry::IsInRomL() const
{
    /* If JAR file is in ROM backup and restore cannot restore application
     * JAR file to ROM thus causing restore to fail. Although ROOT_PATH
     * point to RAM drive application is marked as ROM application.
     */
    TUid packageUid = TUid::Null();

    if (Java::EMidp2Midlet == iRegistryEntry->Type())
    {
        Java::CJavaRegistryPackageEntry* packageEntry =
            ((Java::CJavaRegistryApplicationEntry*)
             iRegistryEntry)->PackageEntryL();

        if (packageEntry)
        {
            packageUid = packageEntry->Uid();
            delete packageEntry;
            packageEntry = NULL;
        }
    }
    else
    {
        packageUid = iRegistryEntry->Uid();
    }

    java::util::Uid uid;
    JavaStorageApplicationEntry_t suiteEntry;

    try
    {
        auto_ptr<JavaStorage> js(JavaStorage::createInstance());
        js->open();
        js->read(APPLICATION_PACKAGE_TABLE, TUidToUid(packageUid, uid), suiteEntry);
        js->close();
        js.reset(0);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "Read failed: %s ",
              jse.toString().c_str());
    }

    wstring value = L"";

    JavaStorageEntry attr;
    attr.setEntry(JAR_PATH, L"");

    JavaStorageApplicationEntry_t::iterator finder = suiteEntry.find(attr);

    if (finder != suiteEntry.end())
    {
        value = (*finder).entryValue();
    }

    if (value.size() == 0)
    {
        User::Leave(KErrGeneral);
    }

    suiteEntry.clear();

    wstring::size_type start = 0;
    wstring::size_type end = 1;

    char* driveLetter =  JavaCommonUtils::wstringToUtf8(
                             value.substr(start, end));

    TBool isInRom = EFalse;

    if (driveLetter[0] == 'z' || driveLetter[0] == 'Z')
    {
        isInRom = ETrue;
    }
    delete[] driveLetter;

    return isInRom;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::ListDirectoriesL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::ListDirectoriesL
(RPointerArray<HBufC>& /*aDirArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::NumberOfCertificateChainsL
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryEntry::NumberOfCertificateChainsL() const
{
    return iRegistryEntry->NumberOfCertificateChains();
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::CertificateChainL
// ---------------------------------------------------------------------------
//
const TDesC8& CInstalledAppsRegistryEntry::CertificateChainL
(TInt aChain) const
{
    return iRegistryEntry->CertificateChain(aChain);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::SizeL
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryEntry::SizeL() const
{
    TUid packageUid = TUid::Null();

    if (Java::EMidp2Midlet == iRegistryEntry->Type())
    {
        Java::CJavaRegistryPackageEntry* packageEntry =
            ((Java::CJavaRegistryApplicationEntry*)
             iRegistryEntry)->PackageEntryL();

        if (packageEntry)
        {
            packageUid = packageEntry->Uid();
            delete packageEntry;
            packageEntry = NULL;
        }
    }
    else
    {
        packageUid = iRegistryEntry->Uid();
    }

    std::auto_ptr<Java::CJavaRegistry> registry(Java::CJavaRegistry::NewL());

    std::auto_ptr<Java::CJavaRegistryPackageEntry> entry(
        (Java::CJavaRegistryPackageEntry*) registry->RegistryEntryL(packageUid));

    TInt diskSpace = -1;
    if (entry.get())
    {
        diskSpace = entry->UsedUserDiskSpace();
    }
    return diskSpace;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::GetPropertyL
// ---------------------------------------------------------------------------
//
HBufC8* CInstalledAppsRegistryEntry::GetPropertyL(TUid /*aPropertyId*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::ListApparcIdentifiersL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistryEntry::ListApparcIdentifiersL
(RPointerArray<HBufC>& /*aApparcIdentifiersArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::FindLanguage
// ---------------------------------------------------------------------------
//
const CEntryLangDetails* CInstalledAppsRegistryEntry::FindLanguage
(TLanguage /*aLanguage*/) const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::FindLanguage
// ---------------------------------------------------------------------------
//
const CEntryLangDetails& CInstalledAppsRegistryEntry::FindLanguageL
(TLanguage /*aLanguage*/) const
{
    CEntryLangDetails* temp = NULL;
    User::Leave(KErrNotSupported);
    return *temp;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::GetProperty
// ---------------------------------------------------------------------------
//
CPropertyValuePair* CInstalledAppsRegistryEntry::GetProperty
(TUid /*aPropertyId*/) const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistryEntry::SizeForStreamingL
// ---------------------------------------------------------------------------
//
TInt CInstalledAppsRegistryEntry::SizeForStreamingL() const
{
    // This is not part of MInstalledAppsRegistryEntry.
    User::Leave(KErrNotSupported);
    return 0;
}
