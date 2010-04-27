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
 * @file MFunctionGroup.h
 *
 * @internalComponent
 */

#ifndef MFUNCTIONGROUP_H
#define MFUNCTIONGROUP_H

#include <e32base.h>

namespace MIDP
{

/**
 *
 * An abstract operational interface to a Function Group.
 *
 * A Function Group is a named set of permissions defined
 * by a security policy.
 *
 */

class MFunctionGroup
{
public:

    /**
     *
     * Returns the name of this function group.
     *
     */

    virtual const TDesC& Name(void) const = 0;

    /**
     *
     * Returns the permissions contained in this function group.
     *
     */

    virtual const RPointerArray<const HBufC>& Permissions(void) const = 0;

};

} // end of namespace MIDP

#endif // MFUNCTIONGROUP_H
