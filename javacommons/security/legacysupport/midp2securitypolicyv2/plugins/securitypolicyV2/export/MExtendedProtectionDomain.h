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
 * @file    MExtendedProtectionDomain.h
 *
 * @internalComponent
 *
 */

#ifndef MEXTENDEDPROTECTIONDOMAIN_H
#define MEXTENDEDPROTECTIONDOMAIN_H

#include <e32std.h>
#include "j2me/midp2/security/mpermission.h"
#include "j2me/midp2/security/mprotectiondomain.h"

namespace MIDP
{
class MConnectionFilter;
class MContentAccessPolicy;
class MFileSystemAccessPolicy;
class MFunctionGroupBinding;
class MBlanketPermissionPolicy;

/**
 *
 * An abstract operational interface to an 'extended' protection domain.
 *
 */

class MExtendedProtectionDomain: public MProtectionDomain
{
public:

    /**
     *
     * Returns the connection filters defined for this protection domain.
     *
     */

    virtual void ConnectionFiltersL(RPointerArray<const MConnectionFilter>&) const = 0;

    /**
     *
     * Returns the function group bindings defined for this protection domain.
     *
     */

    virtual void FunctionGroupBindingsL(RPointerArray<const MFunctionGroupBinding>&) const = 0;

    /**
     *
     * Returns the content access policy defined for this domain.
     *
     */

    virtual const MContentAccessPolicy& ContentAccessPolicyL(void) const = 0;

    /**
     *
     * Returns the file system access policy defined for this domain.
     *
     */

    virtual const MFileSystemAccessPolicy& FileSystemAccessPolicyL(const TDesC& aMIDletSuiteHome) const = 0;

    /**
     *
     * Returns the name of this domain.
     *
     */
    virtual const TDesC& Name(void) const = 0;

    /**
     *
     * Returns the permission downgrade policy for function groups, for this domain.
     * MIDP2 Security recommendations state that blanket permissions for certain function
     * groups are mutually exclusive to certain other function groups with blanket permissions.
     * For eg. Net access and Read User data cannot have blanket permissions at the same time
     * for midlets in third party trusted or untrusted domain.
     * Ownership is retained by this object.
     */

    virtual const MBlanketPermissionPolicy* BlanketPermissionPolicy(void) const = 0;

};

} // end of namespace MIDP

#endif // MEXTENDEDPROTECTIONDOMAIN_H
