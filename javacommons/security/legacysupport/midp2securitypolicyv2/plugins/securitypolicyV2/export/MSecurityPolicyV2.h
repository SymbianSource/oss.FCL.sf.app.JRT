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
 * @file    MSecurityPolicyV2.h
 *
 * @internalComponent
 *
 */

#ifndef MSECURITYPOLICYV2_H
#define MSECURITYPOLICYV2_H

#include <e32std.h>
#include "j2me/midp2/core/mversionedinterface.h"
#include "j2me/systemams/module/security/policy/msecuritypolicy.h"

namespace MIDP
{
class MExtendedProtectionDomain;
class MFileSystemAccessPolicy;

/**
 *
 * An abstract operational interface to a security policy
 *
 */

class MSecurityPolicyV2: public MSecurityPolicy, public MVersionedInterface
{
public:

    virtual ~MSecurityPolicyV2() = 0;

    /**
     *
     * Returns the named protection domain defined by this security policy.
     *
     * Leaves with KErrNotFound if the named protection domain is not defined
     * by this policy.
     *
     * @param aName the name of the protection domain.
     *
     */

    virtual const MExtendedProtectionDomain& ProtectionDomainL(const TDesC& aName) const = 0;

    /**
     *
     * Returns the untrusted protection domain defined by this security policy.
     *
     * Leaves with KErrNotFound if the untrusted protection domain is not defined
     * by this policy.
     *
     */

    virtual const MExtendedProtectionDomain& UntrustedProtectionDomainL(void) const = 0;
};

inline MSecurityPolicyV2::~MSecurityPolicyV2()
{
}

} // end of namespace MIDP

#endif // MSECURITYPOLICYV2_H
