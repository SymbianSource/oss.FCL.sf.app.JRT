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
 * @file    ProetctionDomain.cpp
 *
 * @internal
 *
 */

#include "j2me/midp2/security/MFunctionGroup.h"
#include "ConnectionFilter.h"
#include "ContentAccessPolicy.h"
#include "FileSystemAccessPolicy.h"
#include "FunctionGroupBinding.h"
#include "Permission.h"
#include "SecurityPolicy.h"
#include "ProtectionDomain.h"
#include "BlanketPermissionPolicy.h"


/**
 *
 * @internal
 *
 * A concrete implementation of the MExtendedProtectionDomain interface
 *
 * @see MExtendedProtectionDomain
 *
 */

namespace MIDP
{

/*
 *
 * Returns a new instance of CProtectionDomain
 *
 * @param aPolicy   the policy which defines the protection domain.
 *
 * @param aId       the id of the protection domain
 *
 * @param aName     the name of the protection domain
 *
 */

CProtectionDomain* CProtectionDomain::NewLC(
    CSecurityPolicy& aPolicy,
    TInt             aId,
    const TDesC&     aName)
{
    CProtectionDomain* pd = new(ELeave) CProtectionDomain(aPolicy, aId);

    CleanupStack::PushL(pd);
    pd->ConstructL(aName);
    return pd;
}

CProtectionDomain::CProtectionDomain(CSecurityPolicy& aPolicy, TInt aId):
        iPolicy(aPolicy),
        iId(aId)
{
}

void CProtectionDomain::ConstructL(const TDesC& aName)
{
    iName = aName.AllocL();
}

CProtectionDomain::~CProtectionDomain()
{
    iPermissions.ResetAndDestroy();
    iPermissions.Close();
    iBindings.ResetAndDestroy();
    iBindings.Close();
    iConnectionFilters.ResetAndDestroy();
    iConnectionFilters.Close();
    delete iName;
    delete iFileSystemAccessPolicy;
    delete iBlanketPermissionPolicy;
}

TInt CProtectionDomain::Id(void) const
{
    return iId;
}

const TDesC& CProtectionDomain::Name(void) const
{
    return *iName;
}

void CProtectionDomain::ConnectionFiltersL(RPointerArray<const MConnectionFilter>& aFilters) const
{
    TInt count = iConnectionFilters.Count();

    for (TInt i = 0; i < count; i++)
    {
        User::LeaveIfError(aFilters.Append(iConnectionFilters[i]));
    }
}

void CProtectionDomain::FunctionGroupBindingsL(RPointerArray<const MFunctionGroupBinding>& aBindings) const
{
    TInt bindingCount = iBindings.Count();

    for (TInt i = 0; i < bindingCount; i++)
    {
        User::LeaveIfError(aBindings.Append(iBindings[i]));
    }
}

const MContentAccessPolicy& CProtectionDomain::ContentAccessPolicyL(void) const
{
    return *iContentAccessPolicy;
}

const MFileSystemAccessPolicy& CProtectionDomain::FileSystemAccessPolicyL(const TDesC&) const
{

    User::LeaveIfNull(iFileSystemAccessPolicy);
    return *iFileSystemAccessPolicy;
}

//Ownership is retained (not transferred to caller)
const MBlanketPermissionPolicy* CProtectionDomain::BlanketPermissionPolicy(void) const
{
    return iBlanketPermissionPolicy;
}
//Ownership is transferred to this object
void CProtectionDomain::SetBlanketPermissionPolicy(
    CBlanketPermissionPolicy* aBlanketPermissionPolicy)
{
    delete iBlanketPermissionPolicy;
    iBlanketPermissionPolicy = aBlanketPermissionPolicy;
}

const RPointerArray<MPermission>& CProtectionDomain::Permissions(void) const
{
    return iPermissions;
}


void CProtectionDomain::AddConnectionFilterL(
    const TDesC&          aTypeName,
    TInt                  aType,
    const TDesC&          aScheme,
    RPointerArray<HBufC>& aParams)
{
    CConnectionFilter* cf = CConnectionFilter::NewLC(aTypeName, aType, aScheme, aParams);

    User::LeaveIfError(iConnectionFilters.Append(cf));
    CleanupStack::Pop(cf);
}

/*
 * Adds a new Function group binding to this protection domain.
 *
 * @param aFunctionGroupName    the name of the function group this binding
 *                              applies to.
 *
 * @param
 */


CFunctionGroupBinding* CProtectionDomain::AddFunctionGroupBindingL(
    const TDesC&          aFunctionGroupName,
    MPermission::TType    aPermissionType,
    MPermission::TMode    aDefaultInteractionMode,
    MPermission::TMode    aMaximumInteractionMode)
{
    CFunctionGroup* fg = iPolicy.FindFunctionGroup(aFunctionGroupName);

    if (fg == NULL)
    {
        User::LeaveIfError(KErrNotFound);
    }

    CFunctionGroupBinding* fgb = new(ELeave) CFunctionGroupBinding(
        *this,
        *fg,
        ++iLastBindingId,
        aPermissionType,
        aDefaultInteractionMode,
        aMaximumInteractionMode);

    CleanupStack::PushL(fgb);
    User::LeaveIfError(iBindings.Append(fgb));
    CleanupStack::Pop(fgb);
    AddPermissionsL(*fgb);
    return fgb;
}

CFunctionGroupBinding* CProtectionDomain::AddFunctionGroupBindingL(
    const TDesC&          aFunctionGroupName,
    MPermission::TType    aPermissionType,
    MPermission::TMode    aDefaultInteractionMode,
    TUint32               aInteractionModes)
{
    CFunctionGroup* fg = iPolicy.FindFunctionGroup(aFunctionGroupName);

    if (fg == NULL)
    {
        User::LeaveIfError(KErrNotFound);
    }

    CFunctionGroupBinding* fgb = new(ELeave) CFunctionGroupBinding(
        *this,
        *fg,
        ++iLastBindingId,
        aPermissionType,
        aDefaultInteractionMode,
        aInteractionModes);

    CleanupStack::PushL(fgb);
    User::LeaveIfError(iBindings.Append(fgb));
    CleanupStack::Pop(fgb);
    AddPermissionsL(*fgb);
    return fgb;
}

void CProtectionDomain::AddPermissionsL(CFunctionGroupBinding& aBinding)
{
    const RPointerArray<const HBufC>& permissions = aBinding.FunctionGroup().Permissions();
    TInt                              count       = permissions.Count();
    MPermission::TMode                mode        = aBinding.DefaultInteractionMode();
    MPermission::TType                type        = aBinding.PermissionType();

    for (TInt i = 0; i < count; i++)
    {
        CPermission* permission = new(ELeave) CPermission(*permissions[i], type, mode);

        CleanupStack::PushL(permission);
        User::LeaveIfError(iPermissions.Append(permission));
        CleanupStack::Pop(permission);
    }
}

void CProtectionDomain::SetFileSystemAccessPolicy(
    CFileSystemAccessPolicy* aFileSystemAccessPolicy)
{
    delete iFileSystemAccessPolicy;     // In case it has been set once already.
    iFileSystemAccessPolicy = aFileSystemAccessPolicy;
}

} // end of namespace MIDP

