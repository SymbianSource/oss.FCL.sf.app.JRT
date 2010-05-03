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


#ifndef SECURITYPOLICY_H
#define SECURITYPOLICY_H

/**
 *
 * @file    SecurityPolicy.h
 *
 * @internal
 *
 */

#include <e32base.h>
#include "j2me/midp2/security/MPermission.h"
#include "MSecurityPolicyV2.h"

namespace MIDP
{
class CFunctionGroup;
class CProtectionDomain;
class MExtendedProtectionDomain;

class CSecurityPolicy: public CBase, public MSecurityPolicyV2
{
public:

    static CSecurityPolicy* NewL();

public:

    virtual ~CSecurityPolicy();

    virtual TUint32 InterfaceVersion(void) const;

    // MSecurityPolicyV2 interface

    virtual const RPointerArray<MPermission>&   UntrustedPermissionsL(void);
    virtual const RPointerArray<MPermission>&   PermissionsL(const TDesC&);
    virtual const MExtendedProtectionDomain&    ProtectionDomainL(const TDesC&) const;
    virtual const MExtendedProtectionDomain&    UntrustedProtectionDomainL(void) const;

    // End of MSecurityPolicyV2 interface

    CProtectionDomain* AddProtectionDomainL(const TDesC&);
    CProtectionDomain* AddUntrustedProtectionDomainL(void);
    CFunctionGroup*    AddFunctionGroupL(const TDesC&);

    void AddPermissionL(CFunctionGroup&, const TDesC&);

    CFunctionGroup*    FindFunctionGroup(const TDesC&) const;
    CProtectionDomain* FindProtectionDomain(const TDesC&) const;

private:

    CSecurityPolicy();

    void ConstructL();

private:

    RPointerArray<CProtectionDomain>    iProtectionDomains;
    CProtectionDomain*                  iUntrustedProtectionDomain;
    RPointerArray<CFunctionGroup>       iFunctionGroups;
    RPointerArray<HBufC>                iPermissions;
    TInt                                iLastId;
};

} // end of namespace MIDP

#endif // SECURITYPOLICY_H
