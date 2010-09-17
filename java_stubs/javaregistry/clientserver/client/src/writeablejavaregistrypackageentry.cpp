/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  writeablejavaregistrypackageentry implementation
*              : (__JAVA_EXCLUDED VERSION)
*
*/


// INCLUDES
#include "writeablejavaregistrypackageentry.h"
#include "javaattributes.h"
#include <appversion.h>

using namespace Java;
using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================


// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryPackageEntry*
CWriteableJavaRegistryPackageEntry::NewL(const TUid& /* aUid */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryPackageEntry*
CWriteableJavaRegistryPackageEntry::NewLC(const TUid& /* aUid */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::~CWriteableJavaRegistryPackageEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryPackageEntry::
~CWriteableJavaRegistryPackageEntry()
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::Version
// ---------------------------------------------------------------------------
//
EXPORT_C TAppVersion CWriteableJavaRegistryPackageEntry::Version() const
{
    return TAppVersion(0, 0, 0);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::Vendor
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWriteableJavaRegistryPackageEntry::Vendor() const
{
    return KNullDesC;
}


// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::IsUninstallable
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryPackageEntry::IsUninstallable() const
{
    return ETrue;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::GetEmbeddedEntries
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
GetEmbeddedEntries(RArray<TUid>& /* aUids */) const
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::DownloadPlugin
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWriteableJavaRegistryPackageEntry::
DownloadPlugin() const
{
    return KNullDesC8;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::InstallPlugin
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWriteableJavaRegistryPackageEntry::
InstallPlugin() const
{
    return KNullDesC8;
}


// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::Size
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CWriteableJavaRegistryPackageEntry::Size() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SetVersionL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
SetVersionL(const TAppVersion& /* aVersion */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SetVendorL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
SetVendorL(const TDesC& /* aVendor */)
{
    User::Leave(KErrNotSupported);
}


// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SetUninstallableL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
SetUninstallableL(TBool /* aUninstallable */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SetDownloadPluginL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
SetDownloadPluginL(const TDesC8& /* aDownloadPlugin */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SetInstallPluginL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
SetInstallPluginL(const TDesC8& /* aInstallPlugin */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SetSize
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::SetSizeL(TInt /* aSize */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::AddEmbeddedEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
AddEmbeddedEntryL(const TUid& /* aUid */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::NumberOfEmbeddedEntries
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CWriteableJavaRegistryPackageEntry::
NumberOfEmbeddedEntries() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::RemoveEmbeddedEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
RemoveEmbeddedEntryL(const TUid& /* aUid */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::EmbeddedEntryByUidL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry*
CWriteableJavaRegistryPackageEntry::
EmbeddedEntryByUidL(const TUid& /* aEntryUid */) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::EmbeddedEntryByNumberL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry*
CWriteableJavaRegistryPackageEntry::
EmbeddedEntryByNumberL(TInt /* aEntryNum */) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SetPropertyL
// Takes ownership of aProperty
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
SetPropertyL(CJavaProperty* /* aProperty */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::CWriteableJavaRegistryPackageEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryPackageEntry::
CWriteableJavaRegistryPackageEntry(const TUid& aUid) :
        CWriteableJavaRegistryEntry(aUid),
        iDownloadPlugin(NULL),
        iInstallPlugin(NULL)
{
}
// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SetAttribute
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
SetAttributeL(const TDesC& /* aName */, const TDesC& /* aValue */, TBool /* aTrusted */)
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C const MJavaAttribute* CWriteableJavaRegistryPackageEntry::
AttributeL(const TDesC& /* aName */) const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::AttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<MJavaAttribute>&
CWriteableJavaRegistryPackageEntry::AttributesL() const
{
    return iAttrib;
}


// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SerializeAttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
SerializeAttributesL()
{

}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::UsedUserDiskSpace
// ---------------------------------------------------------------------------
EXPORT_C TInt64 CWriteableJavaRegistryPackageEntry::UsedUserDiskSpace() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::IsPreinstalled
// ---------------------------------------------------------------------------
EXPORT_C TBool CWriteableJavaRegistryPackageEntry::IsPreinstalled() const
{
    return EFalse;
}

