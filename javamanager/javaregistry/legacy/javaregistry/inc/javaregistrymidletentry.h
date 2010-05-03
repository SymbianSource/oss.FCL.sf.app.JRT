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
* Description:  Definition of Java Registry MIDlet entry.
*
*/


#ifndef JAVAREGISTRYMIDLETENTRY_H
#define JAVAREGISTRYMIDLETENTRY_H

// INCLUDE FILES

#include <e32base.h>
#include "mjavaregistry.h"
#include "javaregistrysuiteentry.h"

// CLASS DECLARATIONS

/**
* This class implements the MJavaRegistryMIDletEntry read only interface.
*/
class CJavaRegistryMIDletEntry : public CBase,
        public MJavaRegistryMIDletEntry
{
public:
    /**
    * Two-phase constructor.
    * @param aSuiteEntry
    * @param aMIDletUid
    * @param aMIDletName Name of the MIDlet.
    * @param aAddRef add suite entry reference. Default value is ETrue.
    * @return Newly created MIDlet entry object.
    */
    static CJavaRegistryMIDletEntry* NewL
    (const CJavaRegistrySuiteEntry& aSuiteEntry,
     TUid aMIDletUid,
     const TDesC& aMIDletName,
     TBool aAddRef = ETrue);

    /**
    * Destructor.
    */
    virtual ~CJavaRegistryMIDletEntry();

public: // From MJavaRegistryMIDletEntry

    /**
    * @see MJavaRegistryMIDletEntry
    */
    virtual void Release() const;

    /**
    * @see MJavaRegistryMIDletEntry
    */
    virtual TUid UidL() const;

    /**
    * @see MJavaRegistryMIDletEntry
    */
    virtual const TDesC& MIDletNameL() const;

    /**
    * @see MJavaRegistryMIDletEntry
    */
    virtual const MJavaRegistrySuiteEntry& SuiteEntryL() const;

    /**
    * @see MJavaRegistryMIDletEntry
    */
    virtual TInt SizeL() const;

private:

    /**
    * C++ Constructor.
    */
    CJavaRegistryMIDletEntry(const CJavaRegistrySuiteEntry& aSuiteEntry,
                             TUid aMIDletUid, TBool aAddRef = ETrue);

    /**
    * 2nd-phase constructor.
    * @param aMIDletName Name of the MIDlet.
    */
    void ConstructL(const TDesC& aMIDletName);

private:
    const CJavaRegistrySuiteEntry&  iSuiteEntry;
    TUid                            iMIDletUid;
    HBufC*                          iMIDletName;
};

#endif // JAVAREGISTRYMIDLETENTRY_H
