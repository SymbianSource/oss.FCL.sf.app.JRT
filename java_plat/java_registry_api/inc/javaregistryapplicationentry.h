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
* Description: Java Registy API header file
*
*/


#ifndef JAVAREGISTRYAPPLICATIONENTRY_H
#define JAVAREGISTRYAPPLICATIONENTRY_H

#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>
#include "javaregistryentry.h"
#include "javaregistryentrytype.h"

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATION
class CWriteableJavaRegistryApplicationEntry;

}// namespace Registry
}// namespace Manager

// FORWARD DECLARATION
class CJavaRegistryPackageEntry;

/**
 * This subclass of CJavaRegistryEntry supports attributes
 * for single application, that can't contain embedded entries.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CJavaRegistryApplicationEntry : public CJavaRegistryEntry
{
public:


    /**
     * CJavaRegistryApplicationEntry::CJavaRegistryApplicationEntry
     * constructor
     *
     * @since S60 v3.2
     * @param aWritableEntry Pointer to a writable entry, to delegate
     *          getter methods. Pointer ownership taken.
     */
    CJavaRegistryApplicationEntry(Java::Manager::Registry::
                                  CWriteableJavaRegistryApplicationEntry* aWritableEntry);

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
     * Tells if the end user can not uninstall this application.
     *
     * @since S60 v3.2
     * @return ETrue if the application is resident, EFalse otherwise.
     *         Default value if not set: EFalse.
     */
    IMPORT_C TBool IsResident() const;

    /**
     * Gets the group name of this application. This property tells in
     * which folder the application icon shall appear. It can be a
     * null-string: in that case the Installed folder will be hosting
     * midlet icons.
     *
     * @since S60 v3.2
     * @return Group name of this application.
     *         Default value if not set:
     */
    IMPORT_C const TDesC& GroupName() const;

    /**
     * Gets a reference to the install package entry, which is the parent
     * of this application. E.g. if this entry represents a MIDlet this
     * method returns a reference to the corresponding MIDlet Suite entry.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @return a CJavaRegistryEntry object. Caller takes over the
     *         ownership of the object being transferred.
     */
    IMPORT_C CJavaRegistryPackageEntry* PackageEntryL() const;

protected:

    /**
     * CJavaRegistryApplicationEntry::WritableEntry method returns the member
     * of the parent class, but first casts it to
     * CWriteableJavaRegistryApplicationEntry.
     *
     * @since S60 v3.2
     * @return Returns the member of this class.
     */
    Java::Manager::Registry::
    CWriteableJavaRegistryApplicationEntry* WritableEntry() const;

};

} // namespace Java

#endif // JAVAREGISTRYAPPLICATIONENTRY_H
