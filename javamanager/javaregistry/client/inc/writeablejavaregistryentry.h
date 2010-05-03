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
* Description:  writeablejavaregistryentry definition
*
*/


#ifndef WRITEABLEJAVAREGISTRYENTRY_H
#define WRITEABLEJAVAREGISTRYENTRY_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "javaregistryentrytype.h"
#include "javaattribute.h"
#include "javastorage.h"

using namespace Java;

namespace Java
{
namespace Manager
{
namespace Registry
{
// FORWARD DECLARATION
class CJavaProperty;

// CLASS DECLARATION
/**
 * This class provides the Internal API for entries in JavaRegistry.
 * It supports common attributes only.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CWriteableJavaRegistryEntry : public CBase
{
public:

    /**
     * CWriteableJavaRegistryEntry::CWriteableJavaRegistryEntry
     * constructor.
     *
     * @since S60 v3.2
     * @param aEntry application entry attributes.
     * @param aType application entry type.
     */
    CWriteableJavaRegistryEntry(
        java::storage::JavaStorageApplicationEntry_t& aEntry,
        TJavaRegistryEntryType aType);

    // destructor
    /**
     * CWriteableJavaRegistryEntry::~CWriteableJavaRegistryEntry destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual ~CWriteableJavaRegistryEntry();

    // general methods
    /**
     * CWriteableJavaRegistryEntry::Uid method gets the Uid of
     * this registry entry.
     *
     * @since S60 v3.2
     * @return A TUid object.
     */
    IMPORT_C TUid Uid() const;


    /**
     * Gets the name of the entity (midlet, suite, etc.) this entry
     * represents.
     *
     * @since S60 v3.2
     * @return Name of the entity this entry represents.
     *         Default value if not set: KNullDesC.
     */
    IMPORT_C const TDesC& Name() const;


    /**
     * CWriteableJavaRegistryEntry::Type method gets the type of this
     * registry entry. Enumeration TJavaRegistryEntryType defines
     * the currently possible options. Mandatory property.
     *
     * @since S60 v3.2
     * @return Type of this registry entry.
     */
    IMPORT_C TJavaRegistryEntryType Type() const;


    /**
     * CWriteableJavaRegistryEntry::Drive method gets the letter
     * of drive where the install package has been installed.
     *
     * @since S60 v3.2
     * @return Letter of drive where the install package has been
     *         installed.
     *         Default value if not set: EDriveC.
     */
    IMPORT_C TDriveNumber Drive() const;


    /**
     * CWriteableJavaRegistryEntry::NumberOfCertificateChains method
     * returns the number of certificate chains of this Java application.
     *
     * @since S60 v3.2
     * @return Number of certificate chains.
     */
    IMPORT_C TInt NumberOfCertificateChains() const;


    /**
     * CWriteableJavaRegistryEntry::CertificateChain method returns the
     * n-th certificate chain of this Java application.
     *
     * @since S60 v3.2
     * @param aIndex Index of the certificate chain to be retrieved.
     * @return N-th certificate chain
     *         Default value if not set: KNullDesC.
     */
    IMPORT_C const TDesC8& CertificateChain(TInt aIndex) const;


    /**
     * Returns the content id of the entity this entry represents.
     * The content id can be used to check if a DRM protected entity
     * has enough right to be launched or not.
     * The method can also be used to check if the entity is DRM
     * protected or not.
     *
     * @since S60 v3.2
     * @return Content id of the entity this entry represent or
     *          KNullDesC if the entity is not DRM protected.
     */
    IMPORT_C const TDesC& DrmContentId() const;

    /**
     * CWriteableJavaRegistryEntry::Attribute method gets the
     * additional attribute.
     *
     * @since S60 v5.0
     * @param aName The name the searched attribute.
     * @return MJavaAttribute poiter to instance, which contain attribute.
     * If attribute doesn't exist return NULL.
     */
    IMPORT_C virtual const MJavaAttribute* AttributeL(const TDesC& aName) const;

    /**
     * CWriteableJavaRegistryEntry::Attributes method gets the
     * additional attributes array.
     *
     * @since S60 v5.0
     * @return RPointerArray& with all additional attributes.
     */
    IMPORT_C virtual const RPointerArray<MJavaAttribute>& AttributesL() const;

protected:

    /**
     * 2nd phase constructor. Used by the subclasses.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aType The type to be setted.
     */
    IMPORT_C void ConstructL(TJavaRegistryEntryType aType);

    /**
     * Read attribute value from entry. Value is left empty if no value exists.
     *
     * @param aEntry application entry.
     * @param aName attribute name to read value.
     * @param[out] aValue to be populated.
     */
    void EntryAttributeValue(
        const java::storage::JavaStorageApplicationEntry_t& aEntry,
        const std::wstring& aName,
        std::wstring& aValue) const;

    /**
     * Read application suite entry from JavaStorage. Entry is read based
     * on its uid.
     *
     * @param aUid uid used to identify application suite entry.
     * @param aTableName table where entry is read.
     * @param[out] aSuiteEntry to be populated.
     */
    void StorageEntry(
        const java::util::Uid& aUid,
        const std::string& aTableName,
        java::storage::JavaStorageApplicationEntry_t& aSuiteEntry) const;


protected: // Data
    java::storage::JavaStorageApplicationEntry_t iEntry;

private:
    /**
     * CJavaRegistryEntry::readAttributes method gets the
     * array of additional attributes.
     *
     * @param aUid identifying attributes.
     * @param[out] aAttributes entry attributes.
     */
    void ReadAttributesL(const std::wstring& aUid,
                         java::storage::JavaStorageApplicationList_t& aAttributes) const;

    void PopulateCertChains() const;

private: // Data
    mutable HBufC16* iName;
    mutable HBufC16* iDrmContentId;
    TJavaRegistryEntryType iType;
    mutable RPointerArray<MJavaAttribute> iAttributes;
    mutable RPointerArray<HBufC8> iCertificates;

};

} // namespace Registry
} // namespace Manager
} // namespace Java


#endif // WRITEABLEJAVAREGISTRYENTRY_H

// End of File
