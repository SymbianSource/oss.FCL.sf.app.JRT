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
 * @file    PathAccess.h
 *
 * @internal
 *
 */

#ifndef PATHACCESS_H
#define PATHACCESS_H

#include "PathPattern.h"

namespace MIDP
{

class CPathAccess: public CPathPattern
{
public:

    enum TMode
    {
        ENone = 0,
        ERead,
        EReadWrite
    };
};

} // end of namespace MIDP

#endif // PATHACCESS_H

