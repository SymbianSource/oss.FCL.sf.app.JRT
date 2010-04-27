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


#ifndef MFUNCTIONGROUPBINDING_H
#define MFUNCTIONGROUPBINDING_H

/**
 *
 * @file    MFunctionGroupBinding.h
 *
 * @internalComponent
 *
 */

#include "j2me/midp2/security/mpermission.h"

namespace MIDP
{
class MFunctionGroup;

/**
 *
 * An abstract operational interface to a Function Group binding.
 *
 * A function group binding defines the type, default and allowed
 * interaction modes, of all permissions contained in a single
 * function group, for a particular protection domain.
 *
 * @see MExtendedProtectionDomain
 *
 * @see MFunctionGroup
 *
 */

class MFunctionGroupBinding
{
public:

    /**
     *
     * Returns the function group this binding applies to,
     *
     */

    virtual const MFunctionGroup& FunctionGroup(void) const = 0;

    /**
     *
     * Returns the id of this binding.
     *
     */

    virtual TUint32 Id(void) const = 0;

    /**
     *
     * Returns the type of all permissions associated with this binding.
     *
     */

    virtual MPermission::TType PermissionType(void) const = 0;

    /**
     *
     * Returns the default interaction mode for all permissions associated
     * with this binding.
     *
     */

    virtual MPermission::TMode DefaultInteractionMode(void) const = 0;

    /**
     *
     * Returns the allowed interaction modes for all permissions associated
     * witn this binding.
     *
     */

    virtual void AllowedInteractionModesL(
        RArray<MPermission::TMode>& aAllowedModes) const = 0;

    /**
     *
     * Returns the interaction mode for the given permission on the basis
     * of the current interaction mode for the group.
     *
     * @param aPermissionName   the name of the permission.
     *
     * @param aCurrentGroupMode the current interaction mode of the group
     *
     */

    virtual MPermission::TMode
    PermissionInteractionModeL(
        const TDesC&        aPermissionName,
        MPermission::TMode  aCurrentGroupMode) const = 0;
};

} // end of namespace MIDP

#endif // MFUNCTIONGROUPBINDING_H
