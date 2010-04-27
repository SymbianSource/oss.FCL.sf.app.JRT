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
#include <MSecurityPreferences.h>

namespace MIDP
{

class CSecurityPreferences: public CBase, public MSecurityPreferences
{
public:

    static CSecurityPreferences* NewL();

public:

    virtual ~CSecurityPreferences();

    virtual MPermission::TMode InteractionModeL(const MFunctionGroupBinding&);
    virtual void InteractionModeL(const MFunctionGroupBinding&, MPermission::TMode);

    virtual TBool IsFromDefaults(void) const;

    virtual void SetInteractionMode(const MFunctionGroupBinding&, MPermission::TMode);

private:

    CSecurityPreferences();

    void ConstructL();
};

} // end of namespace MIDP

