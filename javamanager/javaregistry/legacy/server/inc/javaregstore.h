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
* Description:  javaregstore definition
*
*/


#ifndef JAVAREGSTORE_H
#define JAVAREGSTORE_H

#include <e32base.h>
#include "javastorage.h"
#include "javauid.h"
#include "javaregistryentrytype.h"

// FORWARD DECLARATIONS
namespace Java
{
namespace Manager
{
namespace Registry
{

class CJavaPropertyArray;
class CJavaProperty;

/**
 * CJavaRegStore read application information from Java Storage.
 * Information is then converterd to Java Registry format and sent
 * back to client. Only reading is possible.
 */
class CJavaRegStore : public CBase
{

public:

    /**
     * Creates object of this class.
     *
     * @since S60 v3.2
     */
    static CJavaRegStore* NewL();

    /**
     * Creates object of this class.
     *
     * @since S60 v3.2
     */
    static CJavaRegStore* NewLC();

    /**
     * Reads properties from the file identified with the given uid.
     * If no properties found for the given uid aProperties is null else
     * it is allocated a new CJavaPropertyArray object containing the read
     * properties.
     *
     * @since S60 v3.2
     * @param aUid properties belonging to this uid must be read
     * @param aProperties [out] properties are read in this parameter
     * @param aAllEntries decision, if we want to read entry
     *          from not presented MMC
     */
    void ReadPropertiesL(
        TUid aUid,
        CJavaPropertyArray*& aProperties,
        TBool aAllEntries = EFalse);

    /**
     * Returns the stored uids of all drives.
     *
     * @since S60 v3.2
     * @param [out] aUid stored uids are returned in this parameter
     */
    void GetUidsL(RArray<TUid>& aUids);

    /**
     * Returns with a set of uids whose entries contains the properties (and
     * has the same values) given in the aProperties parameter.
     *
     * @since S60 v3.2
     * @param aProperties the set of properties which must exists
     *          (and must have the same values) in the looked entries
     * @param[out] aFoundUids uids of the found entries are returned
     *          in this parameter
     * @param aAllEntries means, that method give all entries including
     *          entries, which regards with midlets stored in not presented
     *          media. EFalse means entries with regarding not presented
     *          media will not return.
     */
    void FindEntriesL(
        CJavaPropertyArray& aProperties,
        RArray<TUid>& aFoundUids,
        TBool aAllEntries = EFalse);

    /**
     * Destructs object.
     *
     * @since S60 v3.2
     */
    ~CJavaRegStore();

private:

    /**
     * Constructor
     *
     * @since S60 v3.2
     */
    CJavaRegStore();

    /**
     * ConstructL
     *
     * @since S60 v3.2
     */
    void ConstructL();

    /**
     * Match given Uid to entry type.
     *
     * @param aApps List of applications where to match.
     * @param aUid Uid to be matched.
     * @param aType[out] Matched application type.
     * @return ETrue if Uid matched to one in the application list, false
     *               otherwise.
     */
    TBool MatchUidL(
        const java::storage::JavaStorageApplicationList_t& aApps,
        const java::util::Uid& aUid,
        Java::TJavaRegistryEntryType& aType) const;

    /**
     * Read storage entry from Java Storage.
     * The function may leave one of the Java Storage error codes.
     *
     * @param aUid Entry Uid to be read.
     * @param aTableName Table where to read.
     * @param aAppEntry[out] Application entry. Empty if not found from
     *                       the storage.
     */
    void StorageEntryL(
        const java::util::Uid& aUid,
        const std::string& aTableName,
        java::storage::JavaStorageApplicationEntry_t& aAppEntry) const;

    /**
     * Convert application entries to registry format.
     *
     * @param aArray[out] Converted properties.
     * @param aAttrs Application entries to be converted.
     */
    void PopulatePropertiesL(
        CJavaPropertyArray*& aArray,
        const java::storage::JavaStorageApplicationEntry_t& aAttrs) const;

    /**
     * Convert application entry to registry property.
     *
     * @param aName Name of the application entry.
     * @param aValue Value of the application entry.
     * @return created property. Ownership is moved to caller.
     *         If property cannot be created NULL is returned.
     */
    CJavaProperty* CreatePropertyL(
        const std::wstring& aName,
        const std::wstring& aValue) const;

    /**
     * Search from Java Storage.
     * The function may leave one of the Java Storage error codes.
     *
     * @param aTableName Table name where to search.
     * @param Query used in search.
     * @param aFoundApps Matched applications. Empty if no matches.
     */
    void SearchL(
        const std::string& aTableName,
        const java::storage::JavaStorageApplicationEntry_t& aQuery,
        java::storage::JavaStorageApplicationList_t& aFoundApps) const;

    /**
     * Populate Uid array by Uids found from storage application list.
     *
     * @param aApps Application list.
     * @param aEntry Entry to define Application type.
     * @param[out] aUids Uid array to be populated.
     */
    void AddUidsL(
        const java::storage::JavaStorageApplicationList_t& aApps,
        const java::storage::JavaStorageEntry& aEntry,
        RArray<TUid>& aUids) const;

    /**
     * Read application complementary information. This information is such
     * that cannot be read from information belogning to the application
     * i.e. application suite stored information. One example is
     * application installation drive.
     *
     * @param aUid Uid which application information to read.
     * @param[out] aAttrs Attribute container where new attributes are
     *                    added.
     */
    void ComplementaryInformationL(
        const java::util::Uid& aUid,
        java::storage::JavaStorageApplicationEntry_t& aAttrs) const;

    void PopulateManifestAttributesL(
        const java::util::Uid& aUid,
        CJavaPropertyArray*& aProperties) const;

    /**
     * Read attribute value from entry. Value is left empty if no value exists.
     *
     * @param aEntry application entry.
     * @param aName attribute name to be read.
     * @param aValue to be populated.
     */
    void EntryAttributeValue(
        const java::storage::JavaStorageApplicationEntry_t& aEntry,
        const std::wstring& aName,
        std::wstring& aValue) const;

    /**
     * Read Entry attributes from the storage.
     *
     * @param aUid Entry Uid.
     * @param aAppType Entry type.
     * @param[out] aAttributes Entry attributes.
     */
    void EntryAttributesL(
        java::util::Uid& aUid,
        Java::TJavaRegistryEntryType& aAppType,
        java::storage::JavaStorageApplicationEntry_t& aAttributes) const;

    /**
     * Populate Entry type specific attributes.
     *
     * @param aUid Entry Uid.
     * @param aAppType Entry type.
     * @param[out] aProperties Entry specific attributes.
     */
    void PopulateAppTypePropertiesL(
        const java::util::Uid& aUid,
        const Java::TJavaRegistryEntryType aAppType,
        CJavaPropertyArray*& aProperties) const;

    /**
     * Check application is stored in present media.
     *
     * @param aEntry Entry to be checked.
     * @return ETrue if application is stored to present media,
     *               false otherwise.
     **/
    TBool IsPresentL(
        const java::storage::JavaStorageApplicationEntry_t& aEntry) const;

private:    // members

    java::storage::JavaStorage* iStorage;

};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGSTORE_H

