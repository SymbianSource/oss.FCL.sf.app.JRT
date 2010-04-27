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
* Description:  writeablejavaregistry definition
*
*/


#ifndef WRITEABLEJAVAREGISTRY_H
#define WRITEABLEJAVAREGISTRY_H

// INCLUDES
#include <e32base.h>
#include "javaregistryentrytype.h"
#include "javastorage.h"

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATION
class CWriteableJavaRegistryEntry;

// CLASS DECLARATION
/**
 * This class provides the Internal API for JavaRegistry.
 * It allows for trusted clients to retrieve and set properties of
 * Java install packages, applications, services etc.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CWriteableJavaRegistry : public CBase
{
public:

    // constructors
    /**
     * CWriteableJavaRegistry::NewL method creates a
     * CWriteableJavaRegistry object.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUseIntegrity ETrue if Integrity Server must be used during
     *                      the lifetime of this object, EFalse if it's
     *                      not required.
     * @return A pointer to a CWriteableJavaRegistry object. Caller takes
     *         over the ownership of the object being transferred.
     */
    IMPORT_C static CWriteableJavaRegistry* NewL(TBool aUseIntegrity);

    /**
     * CWriteableJavaRegistry::NewLC method creates a
     * CWriteableJavaRegistry object.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUseIntegrity ETrue if Integrity Server must be used during
     *                      the lifetime of this object, EFalse if it's
     *                      not required.
     * @return A pointer to a CWriteableJavaRegistry object. Caller takes
     *         over the ownership of the object being transferred.
     */
    IMPORT_C static CWriteableJavaRegistry* NewLC(TBool aUseIntegrity);

    // destructor
    /**
     * CWriteableJavaRegistry::~CWriteableJavaRegistry destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C ~CWriteableJavaRegistry();

    // general methods

    /**
     * CWriteableJavaRegistry::RegistryEntryExistsL method returns whether
     * registry entry with the specified Uid exists.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid to be checked.
     * @return ETrue if the registry entry exists, EFalse otherwise.
     */
    IMPORT_C TBool RegistryEntryExistsL(const TUid& aUid) const;


    /**
     * CWriteableJavaRegistry::GetRegistryEntryUidsL method returns
     * an array of Uids of the registry entries, that are installed to
     * the phone, or to the mmc drive currently plugged in.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUids [out] Uids of the registry entries.
     */
    IMPORT_C void GetRegistryEntryUidsL(RArray<TUid>& aUids) const;


    /**
     * CWriteableJavaRegistry::GetRegistryEntryUidsL method returns
     * an array of Uids of the registry entries, that are installed to
     * the phone, or to the mmc drive currently plugged in.
     * Entries are searched by the specified type.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aType Type of registry entries whose Uids are to be returned.
     * @param aUids [out] Uids of the matching registry entries.
     */
    IMPORT_C void GetRegistryEntryUidsL(TJavaRegistryEntryType aType,
                                        RArray<TUid>& aUids,
                                        TBool aAllEntries = EFalse) const;

    /**
     * CWriteableJavaRegistry::RegistryEntryL method returns the
     * corresponding registry entry for a given Uid.
     * The returned entry can be casted to the appropriate subclass
     * according to the type property. If type is in the package range
     * (EGeneralPackage <= entryType < EGeneralApplication), it can
     * be casted to CWriteableJavaRegistryPackageEntry, and if type is
     * in the application range (EGeneralApplication <= entryType),
     * it can be casted to CWriteableJavaRegistryApplicationEntry.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of the entry to be retrieved.
     * @return The returned registry entry or null if registry entry not
     *         found. Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C CWriteableJavaRegistryEntry* RegistryEntryL(
        const TUid& aUid,
        TBool aAllEntries = EFalse) const;

protected:

    // constructor
    /**
     * CWriteableJavaRegistry::CWriteableJavaRegistry constructor
     *
     * @since S60 v3.2
     */
    IMPORT_C CWriteableJavaRegistry();

    // 2nd phase constructors
    /**
     * CWriteableJavaRegistry::ConstructL method
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUseIntegrity ETrue if Integrity Server must be used during
     *                      the lifetime of this object, EFalse if it's
     *                      not required.
     * @param aLegacy Indicates, that the registry is used in legacy
     *                mode.
     */
    IMPORT_C void ConstructL(TBool aUseIntegrity, TBool aLegacy = EFalse);

private:

    /**
     * Read all application Uids from JavaStorage.
     *
     * @param[out] aFoundApps Application list to be populated.
     */
    void ApplicationUids(
        java::storage::JavaStorageApplicationList_t& aFoundApps) const;

    /**
     * Match given Uid to Uids in the given list. Application type is populated
     * after matching and it is checked if application is present.
     *
     * @since S60 vx.x
     * @param aApps Application list holding Uids.
     * @param aUid to match.
     * @param[out] aType application type to be populated.
     * @return true if uid matches one with the application list and
     *         it is present, false otherwise.
     */
    TBool MatchUid(const java::storage::JavaStorageApplicationList_t& aApps,
                   const java::util::Uid& aUid,
                   Java::TJavaRegistryEntryType& aType) const;

    /**
     * Read application entry from JavaStorage. Entry is read based on its
     * uid. Entry is left empty if it is
     * not found from the storage.
     *
     * @param aUid uid used to identify application entry.
     * @param aTableName where entry is read.
     * @param[out] aAppEntry to be populated.
     */
    void StorageEntry(
        const java::util::Uid& aUid,
        const std::string& aTableName,
        java::storage::JavaStorageApplicationEntry_t& aAppEntry) const;

    /**
     * Populate TUid array. Array is populated Uids matching to JavaStorageEntry
     * defined Uid. Add TUid to array if not exists already. If error happens
     * in Uid to TUid conversion, uid is skipped.
     *
     * @param aApps Applications which Uids are populated.
     * @param aEntry Used to match correct Uid.
     * @param aUids Array to be populated.
     */
    void AddUids(
        const java::storage::JavaStorageApplicationList_t& aApps,
        const java::storage::JavaStorageEntry& aEntry,
        RArray<TUid>& aUids) const;

    /**
     * Check is entry present on the device. Entry must contain at least
     * ID or PACKAGE_ID and MEDIA_ID attributes.
     *
     * @param aEntry to be checked.
     * @return true if application is present, false otherwise.
     */
    TBool IsPresent(
        const java::storage::JavaStorageApplicationEntry_t& aEntry) const;

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


private: // Data

    java::storage::JavaStorage* iStorage;

};

} // namespace Registry
} // namespace Manager
} // namespace Java

#endif // WRITEABLEJAVAREGISTRY_H

// End of File
