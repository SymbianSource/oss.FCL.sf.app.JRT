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
* Description:  writeablejavaregistryapplicationentry definition
*
*/


#ifndef WRITEABLEJAVAREGISTRYAPPLICATIONENTRY_H
#define WRITEABLEJAVAREGISTRYAPPLICATIONENTRY_H

// INCLUDES
#include <e32cmn.h>
#include "writeablejavaregistryentry.h"
#include "javaattributes.h"

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

    /**
     * CWriteableJavaRegistryApplicationEntry::NewL method creates a
     * CWriteableJavaRegistryApplicationEntry object. This constructor
     * sets the type-property to EGeneralApplication.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of this entry.
     * @return A pointer to a CWriteableJavaRegistryApplicationEntry
     *         object. Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C static CWriteableJavaRegistryApplicationEntry*
    NewL(const TUid& aUid);

    /**
     * CWriteableJavaRegistryApplicationEntry::NewLC method creates a
     * CWriteableJavaRegistryApplicationEntry object. This constructor
     * sets the type-property to EGeneralApplication.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of this entry.
     * @return A pointer to a CWriteableJavaRegistryApplicationEntry
     *         object. Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C static CWriteableJavaRegistryApplicationEntry*
    NewLC(const TUid& aUid);

    // destructor
    /**
     * CWriteableJavaRegistryApplicationEntry::~CWriteableJavaRegistryApplicationEntry destructor
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
     * CWriteableJavaRegistryApplicationEntry::GroupName method gets
     * the group name of this application. This property tells in what
     * folder should this application to be put.
     *
     * @since S60 v3.2
     * @return Group name of this application.
     *         Default value if not set: "Installed".
     */
    IMPORT_C const TDesC& GroupName() const;


    /**
     * CWriteableJavaRegistryApplicationEntry::InstallPackageUid method
     * gets the Uid of the install package entry, which is the parent
     * of this application.
     *
     * @since S60 v3.2
     * @return The Uid of the parent package.
     *         Default value if not set: TUid( 0 ).
     */
    IMPORT_C TUid InstallPackageUid() const;


    /**
     * Clients can set startability of an application, i.e. whether
     * this application is startable or not.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aStartable ETrue if the application shall be startable,
     *                   EFalse otherwise.
     */
    IMPORT_C void SetStartableL(TBool aStartable);


    /**
     * Clients can specify if this application is visible in Application
     * Shell or not.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aVisible ETrue if the application shall be shown in
     *                 Application Shell, EFalse otherwise.
     */
    IMPORT_C void SetVisibleL(TBool aVisible);


    /**
     * Clients can specify whether this application shall be resident or
     * not.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aResident ETrue if the application shall be resident,
     *                  EFalse otherwise.
     */
    IMPORT_C void SetResidentL(TBool aResident);


    /**
     * CWriteableJavaRegistryApplicationEntry::SetGroupNameL method sets
     * the group name of this application. This property tells in what
     * folder should this application to be put.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aGroupName Group name of this application.
     */
    IMPORT_C void SetGroupNameL(const TDesC& aGroupName);


    /**
     * CWriteableJavaRegistryApplicationEntry::SetInstallPackageL method
     * sets the Uid of the install package entry, which is the parent
     * of this application.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aInstallPackage The Uid of the parent package.
     */
    IMPORT_C void SetInstallPackageUidL(const TUid& aInstallPackage);

    /**
     * CWriteableJavaRegistryApplicationEntry::SetPropertyL method sets
     * a new, or updtates a stored property if already exists.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aProperty Property to be setted. Pointer ownership taken.
     */
    IMPORT_C virtual void SetPropertyL(CJavaProperty* aProperty);


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



protected:

    // constructor
    /**
     * CWriteableJavaRegistryApplicationEntry::
     *             CWriteableJavaRegistryApplicationEntry constructor
     *
     * @since S60 v3.2
     * @param aUid The Uid of this entry.
     */
    IMPORT_C CWriteableJavaRegistryApplicationEntry(const TUid& aUid);

private: // Data

    CWriteableJavaRegistryPackageEntry* iWriteablePackage;

    RPointerArray<MJavaAttribute> iAttributes;

};

}// namespace Registry
}// namespace Manager
}// namespace Java

#endif // WRITEABLEJAVAREGISTRYAPPLICATIONENTRY_H

// End of File
