/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  writeablejavaregistrypackageentry definition
*
*/


#ifndef WRITEABLEJAVAREGISTRYPACKAGEENTRY_H
#define WRITEABLEJAVAREGISTRYPACKAGEENTRY_H

// INCLUDES
#include "writeablejavaregistryentry.h"
#include "javastorage.h"

using namespace Java;

// FORWARD DECLARATION
class TAppVersion;

namespace Java
{
namespace Manager
{
namespace Registry
{

// CLASS DECLARATION
/**
 * This subclass of CWriteableJavaRegistryEntry supports attributes
 * for packages, that can contain embedded entries.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CWriteableJavaRegistryPackageEntry :
        public CWriteableJavaRegistryEntry
{
public:

    // constructors

    /**
     * CWriteableJavaRegistryPackageEntry constructor.
     *
     * @since S60 v3.2
     * @param aEntry application package attributes.
     * @param aType application package type.
     */
    CWriteableJavaRegistryPackageEntry(
        java::storage::JavaStorageApplicationEntry_t& aEntry,
        TJavaRegistryEntryType aType);

    // destructor

    /**
     * CWriteableJavaRegistryPackageEntry::
     *                              ~CWriteableJavaRegistryPackageEntry
     * Destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C ~CWriteableJavaRegistryPackageEntry();

    // general methods
    /**
     * CWriteableJavaRegistryPackageEntry::VersionL method gets the
     * version of this install package suite.
     *
     * @since S60 v3.2
     * @return A TAppVersion object that represents the version
     *         of this install package suite.
     *         Default value if not set: TAppVersion(0, 0, 0).
     */
    IMPORT_C TAppVersion Version() const;


    /**
     * CWriteableJavaRegistryPackageEntry::Vendor method gets the
     * vendor name of this install package suite.
     *
     * @since S60 v3.2
     * @return Vendor name of the installation package.
     *         Default value if not set: KNullDesC.
     */
    IMPORT_C const TDesC& Vendor() const;


    /**
     * Tells if this package is uninstallable.
     *
     * @since S60 v3.2
     * @return ETrue if the package is uninstallable, EFalse otherwise.
     *         Default value if not set: ETrue.
     */
    IMPORT_C TBool IsUninstallable() const;


    /**
     * CWriteableJavaRegistryPackageEntry::GetEmbeddedEntries method
     * returns an array of Uids associated with this install package.
     * E.g. if this entry represents a MIDlet suite this method returns
     * an array of MIDlet Uids.
     *
     * @since S60 v3.2
     * @param aUids [out] Returns Uids of the embedded entries in this
     *                    install package.
     */
    IMPORT_C void GetEmbeddedEntries(RArray<TUid>& aUids) const;


    /**
     * CWriteableJavaRegistryPackageEntry::NumberOfEmbeddedEntries method
     * Gets the number of embedded entries in this install package.
     *
     * @since S60 v3.2
     * @return The number of embedded entries.
     */
    IMPORT_C TInt NumberOfEmbeddedEntries() const;


    /**
     * CWriteableJavaRegistryPackageEntry::EmbeddedEntryByUidL method
     * Returns the corresponding embedded entry for the given Uid.
     * The returned entry can be casted to the appropriate subclass
     * according to the type property. If type is in the package range
     * (EGeneralPackage <= entryType < EGeneralApplication), it can
     * be casted to CWriteableJavaRegistryPackageEntry, and if type is
     * in the application range (EGeneralApplication <= entryType),
     * it can be casted to CWriteableJavaRegistryApplicationEntry.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aEntryUid The Uid of the embedded entry to be retrieved.
     * @return The returned embedded entry or null if entry is not found.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C CWriteableJavaRegistryEntry* EmbeddedEntryByUidL(
        const TUid& aEntryUid) const;


    /**
     * CWriteableJavaRegistryPackageEntry::EmbeddedEntryByNumberL method
     * Returns the corresponding embedded entry for the given application
     * number.
     * The returned entry can be casted to the appropriate subclass
     * according to the type property. If type is in the package range
     * (EGeneralPackage <= entryType < EGeneralApplication), it can
     * be casted to CWriteableJavaRegistryPackageEntry, and if type is
     * in the application range (EGeneralApplication <= entryType),
     * it can be casted to CWriteableJavaRegistryApplicationEntry.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aEntryNum Number of the embedded entry to be retrieved
     *                  (NOTE: Starting from 0)
     * @return The returned embedded entry or null if entry is not found.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C CWriteableJavaRegistryEntry* EmbeddedEntryByNumberL(
        TInt aEntryNum) const;


    /**
     * CWriteableJavaRegistryPackageEntry::Attribute method gets the
     * additional attribute.
     *
     * @since S60 v5.0
     * @param aName The name the searched attribute.
     * @return MJavaAttribute poiter to instance, which contain attribute.
     * If attribute doesn't exist return NULL.
     */
    IMPORT_C virtual const MJavaAttribute* AttributeL(
        const TDesC& aName) const;

    /**
     * CWriteableJavaRegistryPackageEntry::Attributes method gets the
     * additional attributes array.
     *
     * @since S60 v5.0
     * @return RPointerArray& with all additional attributes.
     */
    IMPORT_C virtual const RPointerArray<MJavaAttribute>& AttributesL() const;


    /**
     * CWriteableJavaRegistryPackageEntry::UsedUserDiskSpace gets the
     * amount of disk space taken by the installed application.
     *
     * @since S60 v5.1
     * @return Returns the size of used disk space in bytes.
     */
    IMPORT_C TInt64 UsedUserDiskSpace() const;

    /**
     * Tells if the application package is pre-installed.
     *
     * If a pre-installed application package is upgraded,
     * it remains marked as pre-installed.
     *
     * If a pre-installed application package is uninstalled
     * and later installed again, it is no more marked as
     * pre-installed.
     *
     * @since S60 v5.1
     * @return ETrue if the application suite is pre-installed,
     *         EFalse otherwise.
     */
    IMPORT_C TBool IsPreinstalled() const;


private:

    /**
     * Read all application entries belogning to this suite.
     *
     * @param aUid application suite uid.
     * @param[out] aApps application list to be populated.
     */
    void SuiteEntries(const java::util::Uid& aUid,
                      java::storage::JavaStorageApplicationList_t& aApps) const;

private: // Data

    mutable HBufC16* iVendor;
};

} // namespace Registry
} // namespace Manager
} // namespace Java

#endif // WRITEABLEJAVAREGISTRYPACKAGEENTRY_H

// End of File
