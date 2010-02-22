/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef JAVAREGISTRYENTRY_H
#define JAVAREGISTRYENTRY_H

#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>
#include "javaregistryentrytype.h"
#include "javaattribute.h"


namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATION
class CWriteableJavaRegistryEntry;

}// namespace Registry
}// namespace Manager

/**
 * This class provides the Domain API for entries in JavaRegistry.
 * This class is intended for further derivation.
 * It supports only those attributes which exist in all types of
 * entries.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CJavaRegistryEntry : public CBase
{
public:

    /**
     * Gets the Uid of this registry entry.
     *
     * @since S60 v3.2
     * @return A TUid object.
     */
    IMPORT_C TUid Uid() const;

    /**
     * Gets the type of this registry entry.
     * Enumeration TJavaRegistryEntryType defines the currently possible
     * options. Mandatory property.
     *
     * @since S60 v3.2
     * @return Type of this registry entry.
     */
    IMPORT_C TJavaRegistryEntryType Type() const;

    /**
     * Gets the drive letter where the content of the installation package
     * has been installed.
     *
     * @since S60 v3.2
     * @return Drive letter where the content of the installation package
     *         has been installed.
     *         Default value if not set: EDriveC.
     */
    IMPORT_C TDriveNumber Drive() const;

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
     * Returns the number of certificate chains belonging to the entity
     * this entry represents.
     *
     * @since S60 v3.2
     * @return Number of certificate chains.
     */
    IMPORT_C TInt NumberOfCertificateChains() const;

    /**
     * Returns the n-th certificate chain of the entity this entry
     * represents consisting of X.509 version 3 certificates
     * in their DER encoded format concatenated together
     * without any separators.
     * All certificate chains are returned indifferent if they could be
     * validated or not.
     * This format is directly suitable to be used with
     * function CX509CertChain::DecodeCertsL().
     *
     * @since S60 v3.2
     * @param aChain Zero-based index of the certificate chain to be
     *               retrieved.
     * @return N-th certificate chain
     *         Default value if not set: KNullDesC8.
     */
    IMPORT_C const TDesC8& CertificateChain(TInt aChain) const;

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
     * CJavaRegistryEntry::~CJavaRegistryEntry method
     * Destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C ~CJavaRegistryEntry();

    /**
     * CJavaRegistryEntry::Attribute method gets the
     * additional attribute.
     *
     * @since S60 v5.0
     * @param aName The name the searched attribute.
     * @return MJavaAttribute poiter to instance, which contain attribute.
     * If attribute doesn't exist return NULL.
     */
    IMPORT_C const MJavaAttribute*
    CJavaRegistryEntry::AttributeL(const TDesC& aName) const;

    /**
     * CJavaRegistryEntry::Attributes method gets the
     * array of additional attributes.
     *
     * @since S60 v5.0
     * @return RPointerArray& with all additional attributes.
     */
    IMPORT_C const RPointerArray<MJavaAttribute>&
    CJavaRegistryEntry::AttributesL() const;

protected:

    /**
     * CJavaRegistryEntry::CJavaRegistryEntry constructor
     *
     * @since S60 v3.2
     * @param aWritableEntry Pointer to a writable entry, to delegate
     *          getter methods. Pointer ownership taken.
     */
    CJavaRegistryEntry(Java::Manager::Registry::
                       CWriteableJavaRegistryEntry* aWritableEntry);

    /**
     * CJavaRegistryEntry::WritableEntry method returns the member
     * of this class;
     *
     * @since S60 v3.2
     * @return Returns the member of this class.
     */
    Java::Manager::Registry::
    CWriteableJavaRegistryEntry* WritableEntry() const;

protected: // Data


    /**
     * CJavaRegistryEntry::iWritableEntry member
     * Pointer Owned
     *
     * @since S60 v3.2
     */
    Java::Manager::Registry::CWriteableJavaRegistryEntry* iWritableEntry;

private: // Data


    RPointerArray<MJavaAttribute> iAttributes;

};

} // namespace Java

#endif // JAVAREGISTRYENTRY_H
