/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file    ProtectionDomain.h
 *
 * @internal
 *
 */

#ifndef PROTECTIONDOMAIN_H
#define PROTECTIONDOMAIN_H

#include <e32base.h>
#include "j2me/midp2/security/MPermission.h"
#include "MExtendedProtectionDomain.h"

namespace MIDP
{
class CConnectionFilter;
class CContentAccessPolicy;
class CFileSystemAccessPolicy;
class CFunctionGroupBinding;
class CSecurityPolicy;
class MConnectionFilter;
class MFileSystemAccessPolicy;
class CBlanketPermissionPolicy;
class MBlanketPermissionPolicy;


class CProtectionDomain: public CBase, public MExtendedProtectionDomain
{
public:

    static CProtectionDomain* NewLC(CSecurityPolicy&, TInt, const TDesC&);

public:

    virtual ~CProtectionDomain();

    virtual TInt Id(void) const;

    virtual const TDesC& Name(void) const;

    virtual void ConnectionFiltersL(RPointerArray<const MConnectionFilter>&) const;

    virtual void FunctionGroupBindingsL(RPointerArray<const MFunctionGroupBinding>&) const;

    virtual const MContentAccessPolicy& ContentAccessPolicyL(void) const;

    virtual const MFileSystemAccessPolicy& FileSystemAccessPolicyL(const TDesC&) const;

    //Ownership is retained (not transferred to caller)
    virtual const MBlanketPermissionPolicy* BlanketPermissionPolicy(void) const;

    //Ownership is transferred to this class
    void SetBlanketPermissionPolicy(CBlanketPermissionPolicy*);

    virtual const RPointerArray<MPermission>& Permissions(void) const;

    void AddConnectionFilterL(const TDesC&, TInt, const TDesC&, RPointerArray<HBufC>&);


    CFunctionGroupBinding* AddFunctionGroupBindingL(
        const TDesC&         aName,
        MPermission::TType   aPermissionType,
        MPermission::TMode   aDefaultMode,
        MPermission::TMode   aMaximumMode);

    CFunctionGroupBinding* AddFunctionGroupBindingL(
        const TDesC&         aName,
        MPermission::TType   aPermissionType,
        MPermission::TMode   aDefaultMode,
        TUint32              aInteractionModes);

    void SetFileSystemAccessPolicy(CFileSystemAccessPolicy*);

private:

    CProtectionDomain(CSecurityPolicy&, TInt);

    void ConstructL(const TDesC&);

    void AddPermissionsL(CFunctionGroupBinding&);

private:

    CSecurityPolicy&                        iPolicy;
    TInt                                    iId;
    HBufC*                                  iName;
    TUint32                                 iLastBindingId;
    RPointerArray<CFunctionGroupBinding>    iBindings;
    RPointerArray<CConnectionFilter>        iConnectionFilters;
    RPointerArray<MPermission>              iPermissions;
    CContentAccessPolicy*                   iContentAccessPolicy;
    CFileSystemAccessPolicy*                iFileSystemAccessPolicy;
    CBlanketPermissionPolicy*               iBlanketPermissionPolicy;
};

} // end of namespace MIDP

#endif // PROTECTIONDOMAIN_H
