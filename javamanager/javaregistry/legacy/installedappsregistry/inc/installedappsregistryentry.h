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
* Description:  Definition of CInstalledAppsRegistryEntry class.
*
*/


#ifndef INSTALLEDAPPSREGISTRYENTRY_H
#define INSTALLEDAPPSREGISTRYENTRY_H

// INCLUDE FILES

#include <e32base.h>
#include <e32const.h>
#include <s32std.h>
#include <appversion.h>
#include <javaregistryentry.h>
#include <javaregistryentrytype.h>

#include "minstalledappsregistry.h"
#include "installtypes.h"
#include "javastorage.h"

// FORWARD DECLARATIONS

class CInstalledAppsRegistryFiles;
class CInstalledAppsRegistryEntry;
class CEntryLangDetails;
class CPropertyValuePair;
class RFs;
class RWriteStream;
class RReadStream;

// CLASS DECLARATIONS

/**
* An entry of an installed application. These entries created and retrieved
* using the CInstalledAppsRegistry API.
*
* Each entry is keyed by a UID, and contains various fields and parameters.
* It also contains a list of files which make up the application, and
* a list of languages the application supports.
*/
class CInstalledAppsRegistryEntry : private CBase,
        public MInstalledAppsRegistryEntry
{
public: // API available for server (Installer and IntegritySupport)
    /**
    * Two-phase constructors.
    */
    inline   static CInstalledAppsRegistryEntry* NewL();
    IMPORT_C static CInstalledAppsRegistryEntry* NewLC();

    /**
    * Two-phase constructors.
    * @param aOther Another registry entry used for initialization (basically
    * for copying).
    */
    inline static CInstalledAppsRegistryEntry* NewL
    (const CInstalledAppsRegistryEntry& aOther);
    IMPORT_C static CInstalledAppsRegistryEntry* NewLC
    (const CInstalledAppsRegistryEntry& aOther);

    /**
    * Two-phase constructors.
    * @param aStream Stream used for initialization.
    */
    inline static CInstalledAppsRegistryEntry* NewL(RReadStream& aStream);
    IMPORT_C static CInstalledAppsRegistryEntry* NewLC(RReadStream& aStream);

    /**
     * Two-phase constructor.
     *
     * @param aRegEntry JavaRegistry entry. Entry ownership is taken.
     */
    IMPORT_C static CInstalledAppsRegistryEntry* NewL(
        Java::CJavaRegistryEntry* aRegEntry);

    /**
    * Writes the object to a stream
    * @param aStream
    */
    IMPORT_C void WriteL(RWriteStream& aStream) const;

    /**
    * Adds an extra reference to the ref count.
    */
    IMPORT_C void AddRef();

public: // Setter methods.

    /**
    * Used to set the uid of the package/application.
    */
    IMPORT_C void SetUid(const TUid& aUid);

    /**
    * Used to set the version of the package/application.
    */
    IMPORT_C void SetVersion(const TAppVersion& aVersion);

    /**
    * Used to set the drive, where the package/application is stored.
    */
    IMPORT_C void SetDrive(TChar aDrive);

    /**
    * Used to set the type of the package/application.
    */
    IMPORT_C void SetType(TInstallType aType);

    /**
    * An application-specific filename can be added to the entry with
    * this method.
    * @param aFileName Name of the file that belongs to the application.
    */
    IMPORT_C void AddFileL(const TDesC& aFileName);

    /**
    * An application-specific directory can be added to the entry with
    * this method.
    * @param aFileName Name of the directory that belongs to the application.
    */
    IMPORT_C void AddDirectoryL(const TDesC& aFileName);

    /**
    * An AppArc identifier can be added to the application entry with this
    * method.
    * @param aApparcIdentifier AppArc identifier to be added.
    */
    IMPORT_C void AddApparcIdentifierL(const TDesC& aApparcIdentifier);

    /**
    * A new certificate chain belonging to the application can be added to
    * the entry.
    * @param aCertChain Certification chain to be added.
    */
    IMPORT_C void AddCertificateChainL(const TDesC8& aCertChain);

    /**
    * Adds a property/value pair to the entry.
    * @param aPropertyId  The UID of the property.
    * @param aValue          The property value.
    */
    IMPORT_C void AddPropertyL(TUid aPropertyId, const TDesC8& aValue);

    /**
    * Adds support for a particular language. With it, the package name and
    * vendor name for this language must be included.
    * @param aLang     The language to add.
    * @param aPackage  The package name for this language.
    * @param aVendor   The vendor name for this language.
    */
    IMPORT_C void AddLanguageL(TLanguage aLang,
                               const TDesC& aPackage,
                               const TDesC& aVendor);

    /**
    * Dependencies API calls. Adds a package UID on which the entry is
    * dependent.
    * @param aUid Package Uid.
    */
    IMPORT_C void AddDependencyL(TUid aUid);

    /**
    * Lists uids this package is dependent upon.
    * @return List of uids this package is dependent upon.
    */
    inline const RArray<TUid>& Dependencies() const;

    /**
    * Embedded packages API calls. Adds the UID of a package which has been
    * embedded in this package
    * @param aUid Uid of the package this application is dependent upon.
    */
    IMPORT_C void AddEmbeddedPackageL(TUid aUid);

    /**
    * Lists uids identifying entries embedded into this package.
    * @return List of uids of embedded entries of this package.
    */
    inline const RArray<TUid>& EmbeddedPackages() const;

public: // From MInstalledAppsRegistryEntry - Read-only interface

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual void Release();

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TUid UidL() const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TAppVersion VersionL() const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TChar DriveL() const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TInstallType TypeL() const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual const TDesC& PackageNameL(TLanguage aLanguage) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual const TDesC& VendorNameL(TLanguage aLanguage) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual void LanguagesL(RArray<TLanguage>& aLangArray) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TBool LanguageSupportedL(TLanguage aLanguage) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual void ListFilesL(RPointerArray<HBufC>& aFileArray) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TBool UsesFileL(const TDesC& aFileName) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual void ListDirectoriesL(RPointerArray<HBufC>& aDirArray) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TInt NumberOfCertificateChainsL() const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual const TDesC8& CertificateChainL(TInt aChain) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TInt SizeL() const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual void FindFileWithExtL(const TDesC& aExtension,
                                  TFileName& aFileName) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual TBool IsInRomL() const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual HBufC8* GetPropertyL(TUid aPropertyId) const;

    /**
    * @see MInstalledAppsRegistryEntry.
    */
    virtual void ListApparcIdentifiersL
    (RPointerArray<HBufC>& aApparcIdentifiersArray) const;

public: // API used only by CInstalledAppsRegistry
    /**
    * Returns the value of a property for a given UID. Returns NULL if the
    * property does not exists
    * @param aPropertyId  The property ID
    * @return             The value - NULL if not found.
    *                     Ownership is NOT transferred to the caller
    */
    CPropertyValuePair* GetProperty(TUid aPropertyId) const;

    /**
    * Returns the size of this entry in bytes. Used for allocating a buffer
    * with the right size for streaming.
    * @return Size of entry in bytes.
    */
    TInt SizeForStreamingL() const;

private:
    // 1st and 2nd phase constructors
    /**
    * Default constructor.
    */
    CInstalledAppsRegistryEntry();

    /**
    * Constructor.
    * @param aOther Another registry entry based on which initialization can
    * be performed.
    */
    CInstalledAppsRegistryEntry(const CInstalledAppsRegistryEntry& aOther);

    /**
    * Second-phase constructor.
    */
    void ConstructL();

    /**
    * Second-phase constructor.
    */
    void ConstructL(Java::CJavaRegistryEntry* aRegistryEntry);

    /**
    * Second-phase constructor.
    */
    void ConstructL(RReadStream& aStream, TBool aStoreFileList = ETrue);

    /**
    * Second-phase constructor.
    */
    void ConstructL(const CInstalledAppsRegistryEntry& aOther);

    /**
    * Private destructor: everything must go through Release() method.
    */
    virtual ~CInstalledAppsRegistryEntry();

    /**
    * Disallow assignment.
    */
    const CInstalledAppsRegistryEntry& operator=
    (const CInstalledAppsRegistryEntry&);

private:    // other internal helper methods
    /**
    * Given a language, returns the element containing data for that language.
    * @param aLanguage Language to be queried.
    * @return Language details.
    */
    const CEntryLangDetails* FindLanguage(TLanguage aLanguage) const;

    /**
    * Given a language, returns the element containing data for that language.
    * This version leaves if it details are not found.
    * @param aLanguage Language to be queried.
    * @return Language details.
    */
    const CEntryLangDetails& FindLanguageL(TLanguage aLanguage) const;

    /**
    * Accessor function to the iInstalledFiles member - these ensure that the
    * installed files have been loaded from the iFromFile.
    */
    CInstalledAppsRegistryFiles& InstalledFilesL() const;

private:
    Java::TJavaRegistryEntryType iType;

    // This attribute is ETrue if the entry is physically stored in ROM
    TBool iIsInRom;

    // Attribute which contains the list of files - when being read from a
    // file, it is fetched in a lazy fashion such that it is not set until the
    // client asks for it. It should never be access directly, but rather
    // through the InstalledFilesL() function
    mutable CInstalledAppsRegistryFiles* iInstalledFiles;

    // The reference count - gets deleted if it gets decremented to
    // zero
    TInt iRefCount;

    Java::CJavaRegistryEntry* iRegistryEntry;
};

#include "installedappsregistryentry.inl"

#endif // INSTALLEDAPPSREGISTRYENTRY_H
