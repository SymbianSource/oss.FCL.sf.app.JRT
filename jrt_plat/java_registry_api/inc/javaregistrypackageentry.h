/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java Registy API header file
*
*/


#ifndef JAVAREGISTRYPACKAGEENTRY_H
#define JAVAREGISTRYPACKAGEENTRY_H

#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>
#include "javaregistryentry.h"
#include "javaregistryentrytype.h"

// FORWARD DECLARATION
class TAppVersion;

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATION
class CWriteableJavaRegistryPackageEntry;

}// namespace Registry
}// namespace Manager


/**
 * This subclass of CJavaRegistryEntry supports attributes
 * for packages, that can contain embedded entries.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CJavaRegistryPackageEntry : public CJavaRegistryEntry
{
public:


    /**
     * CJavaRegistryPackageEntry::CJavaRegistryPackageEntry constructor
     *
     * @since S60 v3.2
     * @param aWritableEntry Pointer to a writable entry, to delegate
     *          getter methods. Pointer ownership taken.
     */
    CJavaRegistryPackageEntry(Java::Manager::Registry::
                              CWriteableJavaRegistryPackageEntry* aWritableEntry);

    /**
     * Gets the version of Java entity represented by this package entry.
     *
     * @since S60 v3.2
     * @return A TAppVersion object.
     *         Default value if not set: TAppVersion( 0, 0, 0 ).
     */
    IMPORT_C TAppVersion Version() const;

    /**
     * Gets the vendor name of the Java entity represented by this
     * package entry.
     *
     * @since S60 v3.2
     * @return Vendor name of the Java entity.
     *         Default value if not set: KNullDesC.
     */
    IMPORT_C const TDesC& Vendor() const;


    /**
     * Tells if the Java entity represented by this package entry
     * is uninstallable.
     *
     * @since S60 v3.2
     * @return ETrue if the entity is uninstallable, EFalse otherwise.
     *         Default value if not set: ETrue.
     */
    IMPORT_C TBool IsUninstallable() const;

    /**
     * Returns an array of Uids associated with the Java entity
     * represented by this package entry.
     * E.g. if this entry represents a MIDlet suite this method returns
     * an array of MIDlet Uids.
     *
     * @since S60 v3.2
     * @param aUids [out] Returns Uids of the embedded entries.
     */
    IMPORT_C void GetEmbeddedEntries(RArray<TUid>& aUids) const;

    /**
     * Gets the number of embedded entries in Java entity represented
     * by this package entry.
     *
     * @since S60 v3.2
     * @return the number of embedded entries
     */
    IMPORT_C TInt NumberOfEmbeddedEntries() const;

    /**
     * Returns the corresponding embedded entry for the given Uid.
     * The returned entry can be casted to the appropriate subclass
     * according to the type property. If type is in the package range
     * ( EGeneralPackage <= entryType < EGeneralApplication ), it can
     * be casted to CJavaRegistryPackageEntry, and if type is
     * in the application range ( EGeneralApplication <= entryType ),
     * it can be casted to CJavaRegistryApplicationEntry.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aAppUid The Uid of the embedded entry to be retrieved
     * @return The returned embedded entry or null if entry is not found.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C CJavaRegistryEntry*
    EmbeddedEntryByUidL(const TUid& aEntryUid) const;

    /**
     * Returns an embedded entry specified by a zero-based index.
     * There is not any guaranteed order for embedded entries inside this
     * package entry.
     * The returned entry can be casted to the appropriate subclass
     * according to the type property. If type is in the package range
     * ( EGeneralPackage <= entryType < EGeneralApplication ), it can
     * be casted to CJavaRegistryPackageEntry, and if type is
     * in the application range ( EGeneralApplication <= entryType ),
     * it can be casted to CJavaRegistryApplicationEntry.
     *
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aAppNum Zero-based index of the embedded entry to be
     *                retrieved.
     * @return The returned embedded entry or null if entry is not found.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C CJavaRegistryEntry*
    EmbeddedEntryByNumberL(TInt aEntryNum) const;

    /**
     * Returns the amount of disk space taken by the installed application.
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

protected:

    /**
     * CJavaRegistryPackageEntry::WritableEntry method returns the member
     * of the parent class, but first casts it to
     * CWriteableJavaRegistrypackageEntry.
     *
     * @since S60 v3.2
     * @return Returns the member of this class.
     */
    Java::Manager::Registry::
    CWriteableJavaRegistryPackageEntry* WritableEntry() const;

};

} // namespace Java

#endif // JAVAREGISTRYPACKAGEENTRY_H
