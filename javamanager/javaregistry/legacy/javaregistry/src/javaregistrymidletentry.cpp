/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of internal Java Registry API class.
*
*/


// INCLUDE FILES
#include <javaregistryincludes.h>

#include "javacommonutils.h"
#include "JavaRegistryMIDletEntry.h"
#include "javastorage.h"
#include "javastoragenames.h"
#include "javastorageexception.h"
#include "javasymbianoslayer.h"
#include "logger.h"

using namespace java::storage;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CJavaRegistryMIDletEntry::NewL
// ---------------------------------------------------------------------------
//
CJavaRegistryMIDletEntry* CJavaRegistryMIDletEntry::NewL
(const CJavaRegistrySuiteEntry& aSuiteEntry,
 TUid aMIDletUid,
 const TDesC& aMIDletName,
 TBool aAddRef)
{
    JELOG2(EJavaStorage);
    CJavaRegistryMIDletEntry* self = new(ELeave)
    CJavaRegistryMIDletEntry(
        aSuiteEntry, aMIDletUid, aAddRef);
    CleanupStack::PushL(self);
    self->ConstructL(aMIDletName);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistryMIDletEntry::CJavaRegistryMIDletEntry
// ---------------------------------------------------------------------------
//
CJavaRegistryMIDletEntry::CJavaRegistryMIDletEntry
(const CJavaRegistrySuiteEntry& aSuiteEntry,
 TUid aMIDletUid,
 TBool aAddRef)
        : iSuiteEntry(aSuiteEntry), iMIDletUid(aMIDletUid)
{
    JELOG2(EJavaStorage);
    if (aAddRef)
    {
        iSuiteEntry.AddRef();
    }
}

// ---------------------------------------------------------------------------
// CJavaRegistryMIDletEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CJavaRegistryMIDletEntry::ConstructL(const TDesC& aMIDletName)
{
    JELOG2(EJavaStorage);
    iMIDletName = aMIDletName.AllocL();
}

// ---------------------------------------------------------------------------
// CJavaRegistryMIDletEntry::~CJavaRegistryMIDletEntry
// ---------------------------------------------------------------------------
//
CJavaRegistryMIDletEntry::~CJavaRegistryMIDletEntry()
{
    JELOG2(EJavaStorage);
    iSuiteEntry.Release();
    if (iMIDletName)
    {
        delete iMIDletName;
        iMIDletName = NULL;
    }
}

// ---------------------------------------------------------------------------
// CJavaRegistryMIDletEntry::Release
// ---------------------------------------------------------------------------
//
void CJavaRegistryMIDletEntry::Release() const
{
    JELOG2(EJavaStorage);
    // non-ref counted so just delete
    delete this;
}

// ---------------------------------------------------------------------------
// CJavaRegistryMIDletEntry::UidL
// ---------------------------------------------------------------------------
//
TUid CJavaRegistryMIDletEntry::UidL() const
{
    JELOG2(EJavaStorage);
    return iMIDletUid;
}

// ---------------------------------------------------------------------------
// CJavaRegistryMIDletEntry::MIDletNameL
// ---------------------------------------------------------------------------
//
const TDesC& CJavaRegistryMIDletEntry::MIDletNameL() const
{
    JELOG2(EJavaStorage);
    return *iMIDletName;
}

// ---------------------------------------------------------------------------
// CJavaRegistryMIDletEntry::SuiteEntryL
// ---------------------------------------------------------------------------
//
const MJavaRegistrySuiteEntry& CJavaRegistryMIDletEntry::SuiteEntryL() const
{
    JELOG2(EJavaStorage);
    return iSuiteEntry;
}

// ---------------------------------------------------------------------------
//
// Use the JavaHelperServer to get file sizes from files in the Private data
// cage of JavaVM
// ---------------------------------------------------------------------------
//
TInt CJavaRegistryMIDletEntry::SizeL() const
{
    JELOG2(EJavaStorage);

    TUid suiteUid = iSuiteEntry.UidL();
    std::auto_ptr<Java::CJavaRegistry> registry(Java::CJavaRegistry::NewL());
    std::auto_ptr<Java::CJavaRegistryPackageEntry> entry(
        (Java::CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid));

    TInt diskSpace = -1;
    if (entry.get())
    {
        diskSpace = entry->UsedUserDiskSpace();
    }
    return diskSpace;
}
