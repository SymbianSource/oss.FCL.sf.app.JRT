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
* Description:  javaregistrybackupsupport definition
*
*/


#ifndef JAVAREGISTRYBACKUPSUPPORT_H
#define JAVAREGISTRYBACKUPSUPPORT_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "javaregburstates.h"

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATION
class RJavaRegSession;

// CLASS DECLARATION
/**
 * This class is used to indicate the registry the phases of backup and
 * restore. Mainly used by the Java Backup component.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CJavaRegistryBackupSupport :
        public CBase
{
public:

    // constructors

    /**
     * CJavaRegistryBackupSupport::NewL method creates a
     * CJavaRegistryBackupSupport object.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @return A pointer to a CJavaRegistryBackupSupport object.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C static CJavaRegistryBackupSupport* NewL();

    /**
     * CJavaRegistryBackupSupport::NewLC method creates a
     * CJavaRegistryBackupSupport object, and puts it on the
     * CleanupStack.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @return A pointer to a CJavaRegistryBackupSupport object.
     *         Caller takes over the ownership of the object being
     *         transferred.
     */
    IMPORT_C static CJavaRegistryBackupSupport* NewLC();

    // destructor

    /**
     * ~CJavaRegistryBackupSupport::~CJavaRegistryBackupSupport
     * Destructor
     */
    IMPORT_C ~CJavaRegistryBackupSupport();

    // general methods

    /**
     * CJavaRegistryBackupSupport::EntriesExistForDriveL
     * Returns whether registry entries on the specified drive exist.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aDrive The entries are searched for this drive.
     * @return ETrue if registry entries on the specified drive exist,
     *         EFalse otherwise.
     */
    IMPORT_C TBool EntriesExistForDriveL
    (const TDriveNumber& aDrive) const;

    /**
     * CJavaRegistryBackupSupport::ChangeBURState method changes the
     * registry BackUp and Restore state, according to the given
     * TBackupState type param.
     *
     * @since S60 v3.2
     * @param aState The state to be changed.
     * @return KErrNone if successful, otherwise one of the
     *         system-wide error codes.
     */
    IMPORT_C TInt ChangeBURState(const TBackupState& aState) const;

private:

    // 2nd phase constructor
    /**
     * CJavaRegistryBackupSupport::ConstructL method
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     */
    void ConstructL();

private: // Data

    /**
     * CJavaRegistryBackupSupport::iJavaRegSession member, the client class
     * to the JavaRegistry Server.
     *
     * @since S60 v3.2
     */
    Java::Manager::Registry::RJavaRegSession* iJavaRegSession;

};

}// namespace Registry
}// namespace Manager
}// namespace Java

#endif // JAVAREGISTRYBACKUPSUPPORT_H

// End of File
