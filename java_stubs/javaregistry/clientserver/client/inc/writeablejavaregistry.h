/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATION
class CWriteableJavaRegistryEntry;
class CJavaProperty;
class CJavaPropertyArray;
class RJavaRegSession;

// CLASS DECLARATION
/**
 * This class provides the Internal API for JavaRegistry.
 * It allows for trusted clients to retrieve and set properties of
 * Java install packages, applications, services etc.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CWriteableJavaRegistry :
        public CBase
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

    /**
     * CWriteableJavaRegistry::NewL method creates a
     * CWriteableJavaRegistry object.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aIntegritySessionId A transaction Id, identifying an
     *                              integrity session, the registry
     *                               has to connect to.
     * @return A pointer to a CWriteableJavaRegistry object. Caller takes
     *         over the ownership of the object being transferred.
     */
    IMPORT_C static CWriteableJavaRegistry*
    NewL(TInt64 aIntegritySessionId);

    /**
     * CWriteableJavaRegistry::NewLC method creates a
     * CWriteableJavaRegistry object.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aIntegritySessionId A transaction Id, identifying an
     *                              integrity session, the registry
     *                               has to connect to.
     * @return A pointer to a CWriteableJavaRegistry object. Caller takes
     *         over the ownership of the object being transferred.
     */
    IMPORT_C static CWriteableJavaRegistry*
    NewLC(TInt64 aIntegritySessionId);

    // destructor
    /**
     * CWriteableJavaRegistry::~CWriteableJavaRegistry destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C ~CWriteableJavaRegistry();

    // general methods
    /**
     * CWriteableJavaRegistry::GenerateUidL method allocates some uids for
     * registry entries, specified the count in aNumber param, and returns
     * the uids in the aUids param.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUids [out] Return the generated Uids.
     * @param aNumber The number of the Uids to be created.
     */
    IMPORT_C void GenerateUidsL(RArray<TUid>& aUids, TInt aNumber) const;


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
     * @param aAllEntries means, that method give all entries including
     *          entries, which regards with midlets stored in not presented
     *          media. EFalse means entries with regarding not presented
     *          media will not return.
     */
    IMPORT_C void GetRegistryEntryUidsL(TJavaRegistryEntryType aType,
                                        RArray<TUid>& aUids,
                                        TBool aAllEntries = EFalse) const;


    /**
     * CWriteableJavaRegistry::GetRegistryEntryUidsL method returns
     * an array of Uids of the registry entries, that are installed to
     * the phone, or to the mmc drive currently plugged in.
     * Entries are searched to match the specified properties.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aProperties The properties, to the returned entries have to
     *          match.
     * @param aUids [out] Uids of the matching registry entries, .
     * @param aAllEntries means, that method give all entries including
     *          entries, which regards with midlets stored in not presented
     *          media. EFalse means entries with regarding not presented
     *          media will not return.
     */
    IMPORT_C void GetRegistryEntryUidsL(
        const RPointerArray<CJavaProperty>& aProperties,
        RArray<TUid>& aUids,
        TBool aAllEntries = EFalse) const;


    /**
     * CWriteableJavaRegistry::RegistryEntryL method returns the
     * corresponding registry entry for a given Uid.
     * The returned entry can be casted to the appropriate subclass
     * according to the type property. If type is in the package range
     * ( EGeneralPackage <= entryType < EGeneralApplication ), it can
     * be casted to CWriteableJavaRegistryPackageEntry, and if type is
     * in the application range ( EGeneralApplication <= entryType ),
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


    /**
     * CWriteableJavaRegistry::SetRegistryEntryL method creates, or updates
     * the given registry entry in the registry.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aEntry The registry entry to be stored in the registry.
     */
    IMPORT_C void SetRegistryEntryL(CWriteableJavaRegistryEntry& aEntry) const;


    /**
     * CWriteableJavaRegistry::RemoveRegistryEntryL method removes the
     * specified registry entry, and its embedded entries if requested.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of the entry to be removed.
     * @param aEmbeddedEntries If aUid identifies a packageentry, this
     *                         parameter indicates to remove its
     *                         embedded entries as well.
     * @param aAllEntries means, that method give all entries including
     *          entries, which regards with midlets stored in not presented
     *          media. EFalse means entries with regarding not presented
     *          media will not return.
     */
    IMPORT_C void RemoveRegistryEntryL(
        const TUid& aUid,
        TBool aEmbeddedEntries = EFalse,
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


    /**
     * CWriteableJavaRegistry::ConstructL method
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aIntegritySessionId A transaction Id, identifying an
     *                              integrity session, the registry
     *                               has to connect to.
     */
    IMPORT_C void ConstructL(TInt64 aIntegritySessionId);

    // general methods
    /**
     * CWriteableJavaRegistry::DecideEntryTypeAndCreateL method decides
     * wich subclass of CWriteableJavaRegistryEntry represent the given
     * array of properties, and creates it.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of the entry to be created with.
     * @param aPropArray The method chooses wich subclass to create
     *                      according to these properties.
     * @return A pointer to a subclass of CWriteableJavaRegistryEntry,
     *           or NULL if can't decide type. Caller takes over the
     *         ownership of the object being transferred.
     */
    IMPORT_C virtual CWriteableJavaRegistryEntry* DecideEntryTypeAndCreateL
    (const TUid& aUid,
     CJavaPropertyArray* aPropArray) const;

protected: // Data

    /**
     * CWriteableJavaRegistry::iJavaRegSession member, the client class
     * to the javaregistry server.
     *
     * @since S60 v3.2
     */
    Java::Manager::Registry::RJavaRegSession* iJavaRegSession;

};

}// namespace Registry
}// namespace Manager
}// namespace Java

#endif // WRITEABLEJAVAREGISTRY_H

// End of File
