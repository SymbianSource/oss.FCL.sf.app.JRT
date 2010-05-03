/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of the CInstalledAppsRegistry class.
*
*/


#ifndef INSTALLEDAPPSREGISTRY_H
#define INSTALLEDAPPSREGISTRY_H

// INCLUDE FILES

#include <e32base.h>

#include "minstalledappsregistry.h"
#include "javaregistry.h"

// FORWARD DECLARATIONS

class CInstalledAppsRegistryEntry;

// CLASS DECLARATIONS

/**
* The server side implementation of the installed apps registry. It
* inherits from MInstalledAppsRegistry to implement read-only operations
* when accessed through an MInstalledAppsRegistry type. In addition,
* it also implements operations to modify the registry (for the server only).
* See CInstalledAppsRegistryEntry for a description of each entry.
*
* This object is privately derived from CBase so that clients are
* disallowed from destroying the object via a CBase pointer. To push
* onto the cleanup stack, use CleanupReleasePushL.
*/
class CInstalledAppsRegistry : private CBase, public MInstalledAppsRegistry
{
public:
    /**
    * Returns a CInstalledAppsRegistry object. Note that only a single
    * instance is ever created. If one already exists, NewL will return
    * the existing one.
    * @return The object created.
    */
    IMPORT_C static CInstalledAppsRegistry* NewL();

public: // API available to the server
    /**
    * Adds a new entry into the registry and permanent storage.
    * @param aEntry    The entry which is to be added.
    */
    IMPORT_C void AddEntryL(const CInstalledAppsRegistryEntry& aEntry);

    /**
    * Deletes an entry from the registry with a given UID.
    * @param aUid  The UID of the entry to be deleted.
    * @return      Symbian OS return code. KErrNone if successful,
    *              KErrAccessDenied if the entry resides in ROM, KErrNotFound
    *              if the entry was not found in the registry.
    */
    IMPORT_C TInt DeleteEntry(const TUid& aUid);

    /**
    * Returns an array of UIDs whose entries use a given filename.
    * @param aFileName    The filename to be used to search the entries.
    * @param[out] aUids   An array of UIDs which is populated.
    */
    IMPORT_C void AppsUsingFileL(const TDesC& aFileName,
                                 RArray<TUid>& aUids) const;

    /**
    * Given a list of files, it returns an array of arrays of UIDs which
    * indicate which UIDs which which files
    * @param aFiles    The list of files queries
    * @param aUids     An array of arrays of UIDs. For each file, the same
    *                  index into a files is used as the primary index into
    *                  the second array. The second array contains a list of
    *                  UIDs for that particular file.
    */
    IMPORT_C void AppsUsingFilesL(const RPointerArray<HBufC>& aFiles,
                                  RPointerArray<RArray<TUid> >& aUids) const;

    /**
    * Returns a list of UIDs whose entries are corrupt. This allows
    * for recovery in case an entry has become corrupted.
    * @param aUidArray    The list of UIDs returned.
    * @return             Symbian OS error code. ONLY will leave if unable to
    *                     populate the array or we have run out of memory.
    */
    IMPORT_C void CorruptUidsL(RArray<TUid>& aUidArray) const;

    /**
    * Given a UID, return a list of UIDs which are dependent on this package.
    * A being "Dependent" on B means that either B is in A's dependency list
    * or B is in A's EmbeddedPackage list.
    * @param aUid               The UID to search for dependents.
    * @param[out] aDependents   The returned list of dependents.
    */
    IMPORT_C void DependentsL(TUid aUid, RArray<TUid>& aDependents) const;

    /**
    * Returns a writeable entry - This mirrors the EntryL API call but
    * returns an entry which can be modified. Before the entry can be
    * written, the DeleteEntry API must be called, and then the AddEntry
    * @param aUid.
    * @return    A pointer to the CInstalledAppsRegistryEntry object.
    */
    IMPORT_C CInstalledAppsRegistryEntry* WriteableEntryLC
    (const TUid& aUid) const;
    IMPORT_C CInstalledAppsRegistryEntry* WriteableEntryL
    (const TUid& aUid) const;
    IMPORT_C CInstalledAppsRegistryEntry* WriteableEntryLC
    (TLanguage aLang,
     const TDesC& aPackage,
     const TDesC& aVendor) const;
    IMPORT_C CInstalledAppsRegistryEntry* WriteableEntryL
    (TLanguage aLang,
     const TDesC& aPackage,
     const TDesC& aVendor) const;

public: // From MInstalledAppsRegistry

    /**
    * @see MInstalledAppsRegistry.
    */
    virtual void Release();

    /**
    * @see MInstalledAppsRegistry.
    */
    virtual void InstalledUidsL(RArray<TUid>& aUidArray) const;

    /**
    * @see MInstalledAppsRegistry.
    */
    virtual TBool EntryPresentL(const TUid& aUid) const;

    /**
    * @see MInstalledAppsRegistry.
    */
    virtual MInstalledAppsRegistryEntry* EntryLC(const TUid& aUid) const;

    /**
    * @see MInstalledAppsRegistry.
    */
    virtual MInstalledAppsRegistryEntry* EntryLC
    (TLanguage aLang,
     const TDesC& aPackage,
     const TDesC& aVendor) const;

    /**
    * @see MInstalledAppsRegistry.
    */
    virtual MInstalledAppsRegistryEntry* EntryL(const TUid& aUid) const;

    /**
    * @see MInstalledAppsRegistry.
    */
    virtual MInstalledAppsRegistryEntry* EntryL
    (TLanguage aLang,
     const TDesC& aPackage,
     const TDesC& aVendor) const;

    /**
    * @see MInstalledAppsRegistry.
    */
    virtual HBufC8* GetSystemPropertyL(TUid aPropertyId) const;

private:
    // Constructors and destructors
    /**
    * Default constructor.
    */
    CInstalledAppsRegistry();

    /**
    * Second-phase constructor.
    */
    void ConstructL();

    /**
    * Destructor.
    */
    virtual ~CInstalledAppsRegistry();

private:
    // The reference count of this object. It is destroyed when
    // it gets decremented to 0
    TInt iRefCount;
    Java::CJavaRegistry* iRegistry;

};

#endif // INSTALLEDAPPSREGISTRY_H
