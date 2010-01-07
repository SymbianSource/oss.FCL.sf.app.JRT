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
* Description:  The main class of Java Registy API. Java Registy API
*               can be used to query the installed Java applications
*               and their properties.
*
*/



// JAVAREGISTRY.H
#ifndef __JAVAREGISTRY_H
#define __JAVAREGISTRY_H

#include <e32base.h>
#include "javaregistryentrytype.h"


namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATION
class CWriteableJavaRegistry;

}// namespace Registry
}// namespace Manager
}// namespace Java

namespace Java
{
class CJavaRegistryEntry;

/**
 * This class provides the Platform API for JavaRegistry.
 * It allows for clients to retrieve properties of
 * Java install packages, applications, services etc.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CJavaRegistry : public CBase
{

    // Constructors
public:

    /**
     * Creates a CJavaRegistry object.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @return A pointer to a CJavaRegistry object.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C static CJavaRegistry* NewL();

    /**
     * Creates a CJavaRegistry object.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @return A pointer to a CJavaRegistry object.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C static CJavaRegistry* NewLC();


public:

    // Generic methods

    /**
     * Returns whether registry entry with the specified Uid exists.
     * Registry entries representing entities installed on
     * removable media which are currently not present in the device
     * are considered as not existent.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid to be checked.
     * @return ETrue if the registry entry exists, EFalse otherwise.
     */
    IMPORT_C TBool RegistryEntryExistsL(const TUid& aUid) const;

    /**
     * Returns an array of Uids of the existing registry entries.
     * Uids of registry entries representing entities installed on
     * removable media which are currently not present in the device
     * are not returned.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUids [out] Uids of the registry entries.
     */
    IMPORT_C void GetRegistryEntryUidsL(RArray<TUid>& aUids) const;

    /**
     * Returns an array of Uids of the existing registry entries.
     * Entries are searched by the specified type.
     * Uids of registry entries representing entities installed on
     * removable media which are currently not present in the device
     * are not returned.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aType Type of registry entries whose Uids are to be returned.
     * @param aUids [out] Uids of the matching registry entries.
     */
    IMPORT_C void GetRegistryEntryUidsL
    (TJavaRegistryEntryType aType, RArray<TUid>& aUids) const;

    /**
     * Returns the corresponding registry entry for a given Uid.
     * The returned entry can be casted to the appropriate subclass
     * according to the type property. If type is in the package range
     * ( EGeneralPackage <= entryType < EGeneralApplication ), it can
     * be casted to CJavaRegistryPackageEntry, and if type is
     * in the application range ( EGeneralApplication <= entryType ),
     * it can be casted to CJavaRegistryApplicationEntry.
     * Registry entries representing entities installed on
     * removable media which are currently not present in the device
     * are not returned.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aUid The Uid of the entry to be retrieved
     * @return The returned registry entry or null if registry entry
     *         not found. Caller takes over the ownership of the object
     *         being transferred.
     */
    IMPORT_C CJavaRegistryEntry* RegistryEntryL(const TUid& aUid) const;

    // Destructor
    /**
     * CJavaRegistry::~CJavaRegistry destructor
     *
     * @since S60 v3.2
     */
    virtual ~CJavaRegistry();

private:

    // Constructor

    /**
     * CJavaRegistry::CJavaRegistry constructor
     *
     * @since S60 v3.2
     */
    CJavaRegistry();

    // 2nd phase constructor

    /**
     * CJavaRegistry::ConstructL method
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     */
    void ConstructL();

private: // Data


    /**
     * CJavaRegistry::iJavaReg member
     * Pointer Owned
     * @since S60 v3.2
     */
    Java::Manager::Registry::CWriteableJavaRegistry* iJavaReg;

};

}// namespace Java

#endif // __JAVAREGISTRY_H
