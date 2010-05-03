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
* Description:  Definition of internal Java Registry Suite Entry API.
*
*/


#ifndef JAVAREGISTRYSUITEENTRY_H
#define JAVAREGISTRYSUITEENTRY_H

// INCLUDE FILES

#include "MJavaRegistry.h"
#include "javastorage.h"

// FORWARD DECLARATIONS

class CInstalledAppsRegistryEntry;

namespace Java
{
class CJavaRegistryPackageEntry;
} // namespace Java

// CLASS DECLARATION

/**
* This class implements the MJavaRegistrySuiteEntry interface. In addition,
* it provides write access to the members of the suite entry.
*/
class CJavaRegistrySuiteEntry : private CBase, public MJavaRegistrySuiteEntry
{
public: // Interface only available to JavaMIDletInstaller components
    /**
    * Constructor which takes a registry entry. This class will add a
    * reference to the registry entry.
    */
    IMPORT_C static CJavaRegistrySuiteEntry* NewLC
    (CInstalledAppsRegistryEntry& aEntry);
    IMPORT_C static CJavaRegistrySuiteEntry* NewL
    (CInstalledAppsRegistryEntry& aEntry);

    /**
    * Constructs an empty entry. A new suite UID will be automatically allocated.
    */
    IMPORT_C static CJavaRegistrySuiteEntry* NewLC();
    IMPORT_C static CJavaRegistrySuiteEntry* NewL();

    /**
    * Returns the writeable installed apps entry. Note that ownership is NOT
    * passed onto the client.
    * @return Writeable installed apps entry.
    */
    IMPORT_C CInstalledAppsRegistryEntry& WriteableEntry();

    /**
    * Direct access to the array of MIDlet UIDs.
    * @return Array of MIDlet UIDs belonging to this suite.
    */
    IMPORT_C const RArray<TUid>& MIDletUids() const;

public: // Interface available only to the JavaRegistry DLL
    /**
    * Adds a reference to the ref count. Its pair, the Release() method,
    * is inherited from MJavaRegistrySuiteEntry, thus defined elsewhere.
    */
    void AddRef() const;

    /**
     * CJavaRegistryEntry::CJavaRegistryEntry constructor
     *
     * @since S60 v9.2
     * @param aEntry Pointer to a pacakge entry, to delegate
     *          getter methods. Pointer ownership taken.
     */
    static CJavaRegistrySuiteEntry* NewL(
        Java::CJavaRegistryPackageEntry* aEntry);


public: // External read-only interface from MJavaRegistrySuiteEntry
    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual void Release() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual TUid UidL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual const TDesC& SuiteNameL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual const TDesC& VendorL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual TAppVersion VersionL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual const TDesC& JadPathL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual const TDesC& JadUrlL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual const TDesC& JarPathL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual const TDesC& JarUrlL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual void MIDletUidsL(RArray<TUid>& aMIDletUids) const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual TInt NumberOfMIDletsL() const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual MJavaRegistryMIDletEntry* MIDletByUidL(TUid aMIDletUid) const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual MJavaRegistryMIDletEntry* MIDletByNumberL
    (TInt aMIDletNum) const;

    /**
    * @see MJavaRegistrySuiteEntry.
    */
    virtual const MInstalledAppsRegistryEntry& InstalledAppsEntryL() const;

private:
    /**
    * Constructor.
    */
    CJavaRegistrySuiteEntry();

    /**
    * Destructor.
    */
    virtual ~CJavaRegistrySuiteEntry();

    /**
    * Symbian-style second-phase constructor.
    */
    void ConstructL();

    /**
    * Symbian-style second-phase constructor.
    * @param aEntry SisRegistry entry.
    */
    void ConstructL(CInstalledAppsRegistryEntry& aEntry);

    /**
    * Symbian-style second-phase constructor.
    * @param aEntry SisRegistry entry.
    */
    void ConstructL(Java::CJavaRegistryPackageEntry* aEntry);

private:    // internal helper methods

    /**
     * Get given entry value. Entry values is left empty if it is
     * not found from entries.
     *
     * @param aName Entry name to be read.
     * @param[out] aValue Entry value.
     */
    void EntryAttributeValue(const std::wstring& aName,
                             std::wstring& aValue) const;

    /**
     * Read application entry from JavaStorage. Entry is read based on its
     * uid. Entry is left empty if it is not found from the storage.
     *
     * @param aUid uid used to identify application entry.
     * @param[out] aAppEntry to be populated.
     */
    void ApplicationEntry(
        const TUid aUid,
        java::storage::JavaStorageApplicationEntry_t& aAppEntry) const;


    /**
     * Read entry attribute value from given entry.
     * If not found value is set as empty.
     *
     * @param aEntry where to search.
     * @param aName to be search.
     * @param[out] aValue to be populated.
     */
    void EntryAttributeValue(
        const java::storage::JavaStorageApplicationEntry_t& aEntry,
        const std::wstring& aName,
        std::wstring& aValue) const;

private:
    mutable CInstalledAppsRegistryEntry* iSisRegistryEntry;
    Java::CJavaRegistryPackageEntry* iPackageEntry;

    // Reference count
    mutable TInt iRefCount;
    mutable HBufC16* iJarPath;
    mutable HBufC16* iJadPath;
    mutable HBufC16* iJarUrl;
    mutable HBufC16* iJadUrl;
    RArray<TUid> iEmbeddedUids;
    mutable java::storage::JavaStorageApplicationEntry_t iSuiteEntry;
};

#endif // JAVAREGISTRYSUITEENTRY_H
