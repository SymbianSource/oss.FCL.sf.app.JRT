/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of internal Java Registry API class, CJavaRegistry.
*
*/


#ifndef JAVAREGISTRYLEGACY_H
#define JAVAREGISTRYLEGACY_H

// INCLUDE FILES

#include <e32base.h>

#include "mjavaregistry.h"

// FORWARD DECLARATIONS

class CInstalledAppsRegistry;
class CJavaRegistrySuiteEntry;
class MJavaRegistrySuiteEntry;
class MJavaRegistryMIDletEntry;

namespace Java
{
class CJavaRegistry;
} // namespace Java

// CLASS DECLARATIONS

/**
* This class implements the MJavaRegistry read only registry, and also
* privates a read-write interface.
*/
class CJavaRegistry : public CBase, public MJavaRegistry
{
public: // Interface only available to JavaMIDletInstaller components
    /**
    * Two-phase constructor. Leaves object on cleanup stack.
    * @return Pointer to the newly create object.
    */
    IMPORT_C static CJavaRegistry* NewLC();

    /**
    * Two-phase constructor.
    * @return Pointer to the newly create object.
    */
    IMPORT_C static CJavaRegistry* NewL();

public: // External read-only interface from MJavaRegistry
    /**
    * @see MJavaRegistry.
    */
    virtual void Release() const;

    /**
    * @see MJavaRegistry.
    */
    virtual void InstalledSuiteUidsL(RArray<TUid>& aSuiteUids) const;

    /**
    * @see MJavaRegistry.
    */
    virtual void InstalledMIDletUidsL(RArray<TUid>& aMIDletUids) const;

    /**
    * @see MJavaRegistry.
    */
    virtual TBool SuiteEntryExistsL(TUid aSuiteUid) const;

    /**
    * @see MJavaRegistry.
    */
    virtual MJavaRegistrySuiteEntry* SuiteEntryL(TUid aSuiteUid) const;

    /**
    * @see MJavaRegistry.
    */
    virtual MJavaRegistrySuiteEntry* SuiteEntryL(const TDesC& aSuiteName,
            const TDesC& aSuiteVendor) const;

    /**
    * @see MJavaRegistry.
    */
    virtual MJavaRegistryMIDletEntry* MIDletEntryL(TUid aMIDletUid) const;

    /**
    * @see MJavaRegistry.
    */
    virtual const MInstalledAppsRegistry& InstalledAppsRegistryL() const;

private:
    /**
    * Destructor. No explicit call allowed, but user must call Release().
    */
    virtual ~CJavaRegistry();

    /**
    * Symbian-style second-phase constructor.
    */
    void ConstructL();

private:
    CInstalledAppsRegistry* iSisRegistry;  ///< Own.
    Java::CJavaRegistry* iRegistry;
};

#endif // JAVAREGISTRYLEGACY_H
