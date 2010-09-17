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
* Description:  writeablejavaregistrypackageentry definition
*
*/


#ifndef WRITEABLEJAVAREGISTRYPACKAGEENTRY_H
#define WRITEABLEJAVAREGISTRYPACKAGEENTRY_H

// INCLUDES
#include "writeablejavaregistryentry.h"
#include "javaattributes.h"

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
     * CWriteableJavaRegistryPackageEntry::NewL method creates a
     * CWriteableJavaRegistryPackageEntry object. This constructor
     * sets the type-property to EGeneralPackage.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of this entry.
     * @return A pointer to a CWriteableJavaRegistryPackageEntry
     *         object. Caller takes over the ownership of the object
     *         being transferred.
     */
    IMPORT_C static CWriteableJavaRegistryPackageEntry*
    NewL(const TUid& aUid);

    /**
     * CWriteableJavaRegistryPackageEntry::NewLC method creates a
     * CWriteableJavaRegistryPackageEntry object. This constructor
     * sets the type-property to EGeneralPackage.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of this entry.
     * @return A pointer to a CWriteableJavaRegistryPackageEntry
     *         object. Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C static CWriteableJavaRegistryPackageEntry*
    NewLC(const TUid& aUid);

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
     *         Default value if not set: TAppVersion( 0, 0, 0 ).
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
     * CWriteableJavaRegistryPackageEntry::DownloadPlugin method gets
     * the downloadplugin of this package.
     *
     * @since S60 v3.2
     * @return Descriptor identifying the downloadplugin of this package.
     *         Default value if not set: KNullDesC8.
     */
    IMPORT_C const TDesC8& DownloadPlugin() const;


    /**
     * CWriteableJavaRegistryPackageEntry::InstallPlugin method gets
     * the installplugin of this package.
     *
     * @since S60 v3.2
     * @return Descriptor identifying the installplugin of this package.
     *         Default value if not set: KNullDesC8.
     */
    IMPORT_C const TDesC8& InstallPlugin() const;

    /**
     * CWriteableJavaRegistryPackageEntry::Size method gets the size of
     * this package.
     *
     * @since S60 v3.2
     * @return The size of this package.
     *         Default value if not set: 0.
     */
    IMPORT_C TInt Size() const;

    /**
     * CWriteableJavaRegistryPackageEntry::SetVersionL method sets the
     * version of this install package suite.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aVersion the version of this install package suite.
     */
    IMPORT_C void SetVersionL(const TAppVersion& aVersion);


    /**
     * CWriteableJavaRegistryPackageEntry::SetVendorL method sets the
     * vendor name of this install package suite.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aVendor Vendor name of the installation package.
     */
    IMPORT_C void SetVendorL(const TDesC& aVendor);


    /**
     * Tells if this package is uninstallable.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUninstallable ETrue if the package is uninstallable,
     *                       EFalse otherwise.
     */
    IMPORT_C void SetUninstallableL(TBool aUninstallable);


    /**
     * CWriteableJavaRegistryPackageEntry::SetDownloadPluginL method sets
     * the downloadplugin of this package.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aDownloadPlugin Descriptor identifying the downloadplugin of
     *          this package.
     */
    IMPORT_C void SetDownloadPluginL(const TDesC8& aDownloadPlugin);


    /**
     * CWriteableJavaRegistryPackageEntry::SetInstallPluginL method sets
     * the installplugin of this package.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aInstallPlugin Descriptor identifying the installplugin of
     *           this package.
     */
    IMPORT_C void SetInstallPluginL(const TDesC8& aInstallPlugin);


    /**
     * CWriteableJavaRegistryPackageEntry::SetSizeL method sets the size of
     * this package.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aSize The size of this package.
     */
    IMPORT_C void SetSizeL(TInt aSize);


    /**
     * CWriteableJavaRegistryPackageEntry::AddEmbeddedEntryL method adds
     * the given Uid to the embedded entries.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid Adds the given Uid to the embedded entries.
     */
    IMPORT_C void AddEmbeddedEntryL(const TUid& aUid);


    /**
     * CWriteableJavaRegistryPackageEntry::NumberOfEmbeddedEntries method
     * Gets the number of embedded entries in this install package.
     *
     * @since S60 v3.2
     * @return The number of embedded entries.
     */
    IMPORT_C TInt NumberOfEmbeddedEntries() const;


    /**
     * CWriteableJavaRegistryPackageEntry::RemoveEmbeddedEntryL method
     * removes the embedded entry specified by the given Uid.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of the embedded entry to be removed.
     */
    IMPORT_C void RemoveEmbeddedEntryL(const TUid& aUid);


    /**
     * CWriteableJavaRegistryPackageEntry::EmbeddedEntryByUidL method
     * Returns the corresponding embedded entry for the given Uid.
     * The returned entry can be casted to the appropriate subclass
     * according to the type property. If type is in the package range
     * ( EGeneralPackage <= entryType < EGeneralApplication ), it can
     * be casted to CWriteableJavaRegistryPackageEntry, and if type is
     * in the application range ( EGeneralApplication <= entryType ),
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
     * ( EGeneralPackage <= entryType < EGeneralApplication ), it can
     * be casted to CWriteableJavaRegistryPackageEntry, and if type is
     * in the application range ( EGeneralApplication <= entryType ),
     * it can be casted to CWriteableJavaRegistryApplicationEntry.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aEntryNum Number of the embedded entry to be retrieved
     *                  ( NOTE: Starting from 0 )
     * @return The returned embedded entry or null if entry is not found.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C CWriteableJavaRegistryEntry* EmbeddedEntryByNumberL(
        TInt aEntryNum) const;
    /**
     * CWriteableJavaRegistryPackageEntry::SetPropertyL method sets a new,
     * or updtates a stored property if already exists.
     * This is a temporary overload of
     * CWriteableJavaRegistryEntry::SetPropertyL
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aProperty Property to be setted. Pointer ownership taken.
     */
    IMPORT_C virtual void SetPropertyL(CJavaProperty* aProperty);

    /**
     * CWriteableJavaRegistryPackageEntry::SetAttribute method sets a new,
     * or updtates a stored attribute if already exists.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v5.0
     * @param aProperty Property to be setted. Pointer ownership taken.
     */
    IMPORT_C void SetAttributeL(const TDesC& aName, const TDesC& aValue, TBool aTrusted);

    /**
     * CWriteableJavaRegistryPackageEntry::Attribute method gets the
     * additional attribute.
     *
     * @since S60 v5.0
     * @param aName The name the searched attribute.
     * @return MJavaAttribute poiter to instance, which contain attribute.
     * If attribute doesn't exist return NULL.
     */
    IMPORT_C virtual const MJavaAttribute* AttributeL(const TDesC& aName) const;

    /**
     * CWriteableJavaRegistryPackageEntry::Attributes method gets the
     * additional attributes array.
     *
     * @since S60 v5.0
     * @return RPointerArray& with all additional attributes.
     */
    IMPORT_C virtual const RPointerArray<MJavaAttribute>& AttributesL() const;

    /**
     * CWriteableJavaRegistryPackageEntry::SerializeAttributes method gets the
     * streamed attributes and store stream to CJavaProperty as
     * special ESerializedAttribute
     * If don't exists any attribute method do nothing
     *
     * @since S60 v5.0
     *
     */
    IMPORT_C virtual void SerializeAttributesL();

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


protected:

    // constructor
    /**
     * CWriteableJavaRegistryPackageEntry::
     *                        CWriteableJavaRegistryPackageEntry constructor
     *
     * @since S60 v3.2
     * @param aUid The Uid of this entry.
     */
    IMPORT_C CWriteableJavaRegistryPackageEntry(const TUid& aUid);

    /**
    * CWriteableJavaRegistryEntry::iAttributes member stores the
    * additional attributes. The addtional attributes is also stored in the
    * iProperties as BLOB, and are synchronised at every modification.
    */
    CJavaAttributes* iAttributes;

private: // Data

    /**
     * CWriteableJavaRegistryPackageEntry::iDownloadPlugin member
     * stores the 8bit descriptor represented downloadplugin
     * property. This property is stored also in the
     * iProperties, and are synchronised at every modification.
     *
     * @since S60 v3.2
     */
    HBufC8* iDownloadPlugin;

    /**
     * CWriteableJavaRegistryPackageEntry::iInstallPlugin member
     * stores the 8bit descriptor represented installplugin
     * property. This property is stored also in the
     * iProperties, and are synchronised at every modification.
     *
     * @since S60 v3.2
     */
    HBufC8* iInstallPlugin;

    RPointerArray<MJavaAttribute> iAttrib;


};

}// namespace Registry
}// namespace Manager
}// namespace Java

#endif // WRITEABLEJAVAREGISTRYPACKAGEENTRY_H

// End of File
