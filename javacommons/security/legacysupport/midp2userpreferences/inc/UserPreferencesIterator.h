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
#include <MMIDletSuiteUserPreferencesIterator.h>
#include <vector>
#include "StorageUtils.h"

namespace MIDP
{

class CMIDletSuiteUserPreferencesIterator: public CBase, public MIDP::MMIDletSuiteUserPreferencesIterator
{
public:

    static CMIDletSuiteUserPreferencesIterator* NewL();

    virtual ~CMIDletSuiteUserPreferencesIterator();

    virtual TBool HasNext();

    virtual MIDP::MMIDletSuiteUserPreferences* NextL();

private:

    void ConstructL();

    CMIDletSuiteUserPreferencesIterator();

private:
    int iCurrentMidletSuite;

    std::vector<java::security::legacysupport::MidletSuiteInfo*> iMidletSuites;
};

} // end of namespace MIDP
