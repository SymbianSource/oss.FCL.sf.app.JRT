/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
@file
@publishedPartner
@released
*/

#ifndef MSECURITYPOLICY_H
#define MSECURITYPOLICY_H

#include <e32base.h>

namespace MIDP
{
class MPermission;

/**
 * Interface class used to return permission lists.
 * The static security policy module is used by the SystemAMS to
 * obtain the set of permissions associated with a given protection domain.
 */
class MSecurityPolicy
{
public:

    /** Destructor */
    virtual ~MSecurityPolicy() = 0;

    /**
     * Returns a list of the permissions granted or potentially granted to an
     * untrusted MIDlet suite.
     * @return  Reference to an array containing the list of untrusted persmissions.
     *          The array will not be modified, but it and its contents, must remain
     *          valid idefinitely once it has been returned to the SystemAMS.
     */
    virtual const RPointerArray<MPermission>& UntrustedPermissionsL(void) = 0;

    /**
     * Returns a list of the permissions granted or potentially granted to a trusted
     * MIDlet suite bound to the given Protection Domain.
     * @param   aProtectionDomainId
     *          Descriptor reference containing a Protection Domain isd.
     *          @see MProtectionDomainBinding.
     * @return  Reference to an array containing the list of trusted persmissions.
     *          The array will not be modified, but it and its contents, must remain
     *          valid idefinitely once it has been returned to the SystemAMS.
     */
    virtual const RPointerArray<MPermission>& PermissionsL(const TDesC& aProtectionDomainId) = 0;
};

inline MSecurityPolicy::~MSecurityPolicy()
{
}

} // end of namespace MIDP

#endif // MSECURITYPOLICY_H
