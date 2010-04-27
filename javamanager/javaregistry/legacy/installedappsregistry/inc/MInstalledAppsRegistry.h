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
* Description:  Definition of the MInstalledAppsRegistry.
*
*/


#ifndef MINSTALLEDAPPSREGISTRY_H
#define MINSTALLEDAPPSREGISTRY_H

// INCLUDE FILES

#include <e32base.h>
#include <appversion.h>

#include "installtypes.h"

// FORWARD DECLARATIONS

class MInstalledAppsRegistryEntry;

// CLASS DECLARATIONS

/**
* The registry of installed applications. This registry allows read access
* to entries. Each entry is defined by class MInstalledAppsRegistryEntry.
*
* The client implementation marshals the requests to the server. Access
* is only read only. The server implementation allows additionally for
* write access.
*/
class MInstalledAppsRegistry
{
public:
    /**
    * Release destroys this object (or removes the reference to
    * a reference counted implementation).
    */
    virtual void Release() = 0;

    /**
    * Returns an array of installed UIDs. The array is supplied by the client
    * which is then populated. There is no guarantee about the order in which
    * the uids are put in the array.
    * @param aUidArray The array object to be populated. This array should be
    * empty before passing it in. The result is undefined if the array is
    * not empty.
    */
    virtual void InstalledUidsL(RArray<TUid>& aUidArray) const = 0;

    /**
    * API call to check if an entry with the given UID is present in the
    * registry.
    * @param aUid The UID to query.
    * @return ETrue if there exists an entry present with the given UID,
    *         EFalse otherwise.
    */
    virtual TBool EntryPresentL(const TUid& aUid) const = 0;

    /**
    * Returns an entry from the registry whose UID is equal to aUid
    * @param aUid The UID of the entry to be found.
    * @return A pointer to the entry. Note that ownership of this object
    *         is transferred to the calling client and is therefore
    *         required to destroy it. The pointer is guarenteed to be
    *         non-NULL.
    */
    virtual MInstalledAppsRegistryEntry* EntryLC
    (const TUid& aUid) const = 0;
    virtual MInstalledAppsRegistryEntry* EntryL
    (const TUid& aUid) const = 0;

    /**
    * Returns an entry from the registry with a given package name and
    * vendor name for a given language.
    * @return A pointer to the entry. Note that ownership of this object
    *         is transferred to the calling client and is therefore
    *         required to destroy it. The pointer is guarenteed to be
    *         non-NULL.
    */
    virtual MInstalledAppsRegistryEntry* EntryLC
    (TLanguage aLang,
     const TDesC& aPackage,
     const TDesC& aVendor) const = 0;
    virtual MInstalledAppsRegistryEntry* EntryL
    (TLanguage aLang,
     const TDesC& aPackage,
     const TDesC& aVendor) const = 0;

    /**
    * Returns the system property given by the UID. Leaves with KErrNotFound
    * if the property was not found.
    * @param aPropertyId The UID of the given property.
    * @return The value - ownership of the buffer is transferred to client.
    */
    virtual HBufC8* GetSystemPropertyL(TUid aPropertyId) const = 0;

    /**
    * Function to create a registry. This returns a server registry or client
    * registry, depending on which library is linked
    */
    IMPORT_C static MInstalledAppsRegistry* CreateL();

protected:
    // Forces client to use the Release method
    inline virtual ~MInstalledAppsRegistry();
};


/**
* An entry of an installed application. These entries created and retrieved
* using the MInstalledAppsRegistry API.
*
* Each entry is keyed by a UID, and contains various fields and parameters.
* It also contains a list of files which make up the application, and
* a list of languages the application supports.
*/
class MInstalledAppsRegistryEntry
{
public:
    /**
    * Release destroys this object (or removes the reference to
    * a reference counted implementation)
    */
    virtual void Release() = 0;

    // API attribute getters - synchronous
    virtual TUid UidL() const = 0;
    virtual TAppVersion VersionL() const = 0;
    virtual TChar DriveL() const = 0;
    virtual TInstallType TypeL() const = 0;

    /**
    * Given a language, return the package name. If the language is not
    * found, it will leave with KErrNotFound.
    * @param aLanguage Language of the package.
    * @return The returned package name in the given language.
    */
    virtual const TDesC& PackageNameL(TLanguage aLanguage) const = 0;

    /**
    * Given a language, return the vendor name. If the language is not found,
    * it will leave with KErrNotFound.
    * @param aLanguage Language of the package.
    * @return The returned vendor name
    */
    virtual const TDesC& VendorNameL(TLanguage aLanguage) const = 0;

    /**
    * Populates a client owned array with the list of languages.
    * @param[out] aLangArray List of supported languages.
    */
    virtual void LanguagesL(RArray<TLanguage>& aLangArray) const = 0;

    /**
    * Returns whether or not a particular language is supported.
    * @param aLanguage Language support to check.
    */
    virtual TBool LanguageSupportedL(TLanguage aLanguage) const = 0;

    /**
    * Populates the input array with the list of files used by the
    * application - synchronous.
    * @param[out] aFileArray The array to be populated.
    */
    virtual void ListFilesL(RPointerArray<HBufC>& aFileArray) const = 0;

    /**
    * Given a filename (with a full path specifier), this function
    * returns whether or not this application uses the file.
    * @param aFileName Name of the file to check.
    * @return ETrue if the specified file is used by this application,
    * EFalse otherwise.
    */
    virtual TBool UsesFileL(const TDesC& aFileName) const = 0;

    /**
    * Populates the input array with the list of directories "owned" by the
    * application.
    * @param[out] aDirArray The array to be populated.
    */
    virtual void ListDirectoriesL
    (RPointerArray<HBufC>& aDirArray) const = 0;

    /**
    * Returns the number of certificate chains.
    */
    virtual TInt NumberOfCertificateChainsL() const = 0;

    /**
    * Returns the n-th chain.
    */
    virtual const TDesC8& CertificateChainL(TInt aChain) const = 0;

    /**
    * Returns the size in bytes used by the installed application.
    * The size is computed dynamically by adding the size of the files owned
    * by this entry. If a file is not found (it may be on an absent removable
    * media) its size is considered to be 0. Entries may own directories,
    * in this case all the files in these directories are considered to be
    * owned and are included in the computation.
    */
    virtual TInt SizeL() const = 0;

    /**
    * Given a file extension, return a single filename with that
    * extension. Leaves if not found. It is generally used to find
    * a .jad or .jar file location in the registry.
    * @param aExtension The extension to search for (eg ".app").
    * @param aFileName The returned file name with the extension.
    */
    virtual void FindFileWithExtL(const TDesC& aExtension,
                                  TFileName& aFileName) const = 0;

    /**
    * Returns a boolean which indicates whether or not this entry is
    * located in ROM. An entry located in ROM cannot be uninstalled.
    * @return ETrue if located in ROM, EFalse otherwise.
    */
    virtual TBool IsInRomL() const = 0;

    /**
    * Returns property of the entry keyed by the supplied UID. Leaves with
    * KErrNotFound if the property was not found in the entry.
    * @param aPropertyId The UID of the given property.
    * @return The value - ownership of the buffer is transferred to client.
    */
    virtual HBufC8* GetPropertyL(TUid aPropertyId) const = 0;

    /**
    * Populates the array parameter with the list of Apparc registration
    * file "ID"-s.
    * @param aApparcIdentifiersArray The array to be populated.
    */
    virtual void ListApparcIdentifiersL
    (RPointerArray<HBufC>& aApparcIdentifiersArray) const = 0;

protected:
    inline virtual ~MInstalledAppsRegistryEntry();
};

#include <swi/minstalledappsregistry.inl>

#endif // MINSTALLEDAPPSREGISTRY_H
