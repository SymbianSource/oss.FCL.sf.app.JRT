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
* Description:  writeablejavaregistryapplicationentry definition
*
*/


#ifndef WRITEABLEJAVAREGISTRYAPPLICATIONENTRY_H
#define WRITEABLEJAVAREGISTRYAPPLICATIONENTRY_H

// INCLUDES
#include <e32cmn.h>
#include "writeablejavaregistryentry.h"
#include "javastorage.h"

using namespace Java;

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATION
class CWriteableJavaRegistry;
class CWriteableJavaRegistryPackageEntry;


// CLASS DECLARATION
/**
 * This subclass of CWriteableJavaRegistryEntry supports attributes
 * for single applications, that can't contain embedded entries.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CWriteableJavaRegistryApplicationEntry :
        public CWriteableJavaRegistryEntry
{
public:

    // constructors

    CWriteableJavaRegistryApplicationEntry(
        java::storage::JavaStorageApplicationEntry_t& aEntry,
        TJavaRegistryEntryType aType);

    // destructor
    /**
     * CWriteableJavaRegistryApplicationEntry::
     * ~CWriteableJavaRegistryApplicationEntry destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C ~CWriteableJavaRegistryApplicationEntry();

    // general methods
    /**
     * Tells if this application is startable.
     *
     * @since S60 v3.2
     * @return ETrue if the application is startable, EFalse otherwise.
     *         Default value if not set: ETrue.
     */
    IMPORT_C TBool IsStartable() const;


    /**
     * Tells if this application must be shown in Application Shell or
     * it must rather be hidden.
     *
     * @since S60 v3.2
     * @return ETrue if the application is shown in Application Shell,
     *         EFalse otherwise.
     *         Default value if not set: ETrue.
     */
    IMPORT_C TBool IsVisible() const;


    /**
     * Tells if this application is resident.
     *
     * @since S60 v3.2
     * @return ETrue if the application is resident, EFalse otherwise.
     *         Default value if not set: EFalse.
     */
    IMPORT_C TBool IsResident() const;


    /**
     * CWriteableJavaRegistryApplicationEntry::PackageEntryL method
     * gets a reference to the install package entry, which is the parent
     * of this application.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @return A pointer to a CJavaRegistryEntry object. Caller takes
     *         over the ownership of the object being transferred.
     */
    IMPORT_C CWriteableJavaRegistryPackageEntry* PackageEntryL() const;

    /**
     * CWriteableJavaRegistryAplicationEntry::AttributeL method gets the
     * additional attribute.
     *
     * @since S60 v5.0
     * @param aName The name of the searched attribute.
     * @return MJavaAttribute poiter to instance, which contain attribute.
     * If attribute doesn't exist return NULL.
     */
    IMPORT_C virtual const MJavaAttribute* AttributeL(const TDesC& aName) const;

    /**
     * CWriteableJavaRegistryApplicationEntry::AttributesL method gets the
     * additional attributes array.
     *
     * @since S60 v5.0
     * @return RPointerArray& with all additional attributes.
     */
    IMPORT_C virtual const RPointerArray<MJavaAttribute>& AttributesL() const;


private: // Data

    CWriteableJavaRegistryPackageEntry* iWriteablePackage;

};

} // namespace Registry
} // namespace Manager
} // namespace Java

#endif // WRITEABLEJAVAREGISTRYAPPLICATIONENTRY_H

// End of File
