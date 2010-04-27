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
 * @file Permission.cpp
 *
 * @internal
 *
 */

#include "Permission.h"

/**
 *
 * @internal
 *
 * A concrete implementation of the MPermission interface.
 *
 * @see MPermission
 *
 */

namespace MIDP
{

CPermission::CPermission(const TDesC& aName, TType aType, TMode aMode):
        iName(aName),
        iType(aType),
        iMode(aMode)
{
}

CPermission::~CPermission()
{
}

const TDesC& CPermission::Name(void) const
{
    return iName;
}

MPermission::TType CPermission::Type(void) const
{
    return iType;
}

MPermission::TMode CPermission::Mode(void) const
{
    return iMode;
}

} // end of namespace MIDP

