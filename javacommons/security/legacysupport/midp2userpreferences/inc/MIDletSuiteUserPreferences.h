/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include <e32base.h>
#include "StorageUtils.h"
#include <MMIDletSuiteUserPreferences.h>

_LIT(KMidletSuiteName, "MIDletSuite.Name");
_LIT(KMidletSuiteVendor, "MIDletSuite.Vendor");
_LIT(KMidletSuiteVersion, "MIDletSuite.Version");

namespace MIDP
{

class CMIDletSuiteUserPreferences: public CBase, public MMIDletSuiteUserPreferences
{
public:

    static CMIDletSuiteUserPreferences* NewL(java::security::legacysupport::MidletSuiteInfo&);

    virtual ~CMIDletSuiteUserPreferences();

    virtual TUint32 Id(void) const;

    virtual TInt PropertyL(const TDesC&, TPtrC&) const;

    virtual MSecurityPreferences& SecurityPreferencesL(void);

private:

    CMIDletSuiteUserPreferences(java::security::legacysupport::MidletSuiteInfo&);
    void ConstructL();
    java::security::legacysupport::MidletSuiteInfo& iMidletSuiteInfo;
};

} // end of namespace MIDP
