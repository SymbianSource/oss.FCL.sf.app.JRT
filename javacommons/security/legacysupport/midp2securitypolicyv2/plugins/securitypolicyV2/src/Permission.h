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
* Description:
*
*/


/**
 *
 * @file Permission.h
 *
 * @internal
 *
 */

#ifndef PERMISSION_H
#define PERMISSION_H

#include "j2me/midp2/security/MPermission.h"
#include <e32base.h>

namespace MIDP
{

class CPermission: public CBase, public MPermission
{
public:

    CPermission(const TDesC& aName, TType aType, TMode aMode);

    virtual ~CPermission();

    // MPermission interface

    virtual const TDesC& Name(void) const;
    virtual TType Type(void) const;
    virtual TMode Mode(void) const;

    // End of MPermission interface

private:

    const TDesC&    iName;
    TType           iType;
    TMode           iMode;
};


} // end of namespace MIDP

#endif // PERMISSION_H
