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
#include <MUserPreferences.h>
#include <MSecurityPreferences.h>
#include <MMIDletSuiteUserPreferencesIterator.h>
#include <MMIDletSuiteCustomAttributes.h>
#include <MMIDletSuiteInfo.h>

namespace MIDP
{

class CUserPreferences: public CBase, public MIDP::MUserPreferences
{
public:

    static CUserPreferences* NewL();

    virtual ~CUserPreferences();

    virtual MIDP::MMIDletSuiteUserPreferencesIterator* IteratorL();

    virtual MIDP::MMIDletSuiteCustomAttributes* CustomAttributesL(TUint32);

    virtual MIDP::MSecurityPreferences* SecurityPreferencesL(const MIDP::MMIDletSuiteInfo&);

private:

    void ConstructL();

    CUserPreferences();
};

} // end of namespace MIDP
