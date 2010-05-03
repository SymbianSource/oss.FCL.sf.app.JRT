/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file SecurityPolicy.cpp
 *
 * @internal
 *
 */

#include <f32file.h>
#include <centralrepository.h>
#include "FunctionGroup.h"
#include "ProtectionDomain.h"
#include "XPFParser.h"
#include "SecurityPolicy.h"

/**
 *
 * @internal
 *
 * A concrete implementation of the MSecurityPolicyV2 'interface'
 *
 * @see MSecurityPolicyV2
 *
 */

namespace MIDP
{

CSecurityPolicy* CSecurityPolicy::NewL()
{
    CSecurityPolicy* sp = new(ELeave) CSecurityPolicy;

    CleanupStack::PushL(sp);
    sp->ConstructL();
    CleanupStack::Pop(sp);
    return sp;
}

CSecurityPolicy::CSecurityPolicy()
{
}

// Java Security Policy Central Repository UID
static const TUid KCRUidJavaSecurityPolicy = { 0x2001B289 };

// IDs for security folder and file keys
static const TUint32 KPolicyFolder = 0x02;
static const TUint32 KPolicyFile = 0x01;

_LIT(KPolicyFileSuffix, ".xpf");

void CSecurityPolicy::ConstructL()
{

    // opening Central Repository and reading keys for security policy folder and file
    CRepository* repository = CRepository::NewLC(KCRUidJavaSecurityPolicy);

    HBufC* folderBuf = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr secPolicyFolder(folderBuf->Des());

    HBufC* fileBuf = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr secPolicyFile(fileBuf->Des());

    // Load the security policy file path and name
    User::LeaveIfError(repository->Get(KPolicyFolder, secPolicyFolder));
    User::LeaveIfError(repository->Get(KPolicyFile, secPolicyFile));

    RFs fs;

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // constructing path to security policy file
    HBufC* policyFileName    = HBufC::NewLC(secPolicyFolder.Length() + secPolicyFile.Length() + KPolicyFileSuffix().Length());
    TPtr   policyFileNamePtr = policyFileName->Des();

    policyFileNamePtr += secPolicyFolder;
    policyFileNamePtr += secPolicyFile;
    policyFileNamePtr += KPolicyFileSuffix();

    // opening the .xpf file
    RFile policyFile;

    User::LeaveIfError(policyFile.Open(fs, *policyFileName, EFileShareReadersOnly));

    // parsing the .xpf file and constructing SecurityPolicy object
    CXPFParser* xpfp = MIDP::CXPFParser::NewLC(policyFile, *this);

    xpfp->ParseL();

    // cleanup
    CleanupStack::PopAndDestroy(xpfp);
    CleanupStack::PopAndDestroy(policyFileName);
    CleanupStack::PopAndDestroy();  // fs
    CleanupStack::PopAndDestroy(fileBuf);
    CleanupStack::PopAndDestroy(folderBuf);
    CleanupStack::PopAndDestroy(repository);

}

CSecurityPolicy::~CSecurityPolicy()
{
    iProtectionDomains.ResetAndDestroy();
    iProtectionDomains.Close();
    delete iUntrustedProtectionDomain;
    iFunctionGroups.ResetAndDestroy();
    iFunctionGroups.Close();
    iPermissions.ResetAndDestroy();
    iPermissions.Close();
}

// MVersionedInterface implementation

TUint32 CSecurityPolicy::InterfaceVersion(void) const
{
    return 0x0200;
}

// End of MVersionedInterface implementation

// MSecurityPolicyV2 implementation

/**
 *
 * @internal
 *
 * Returns the permissions associted with the untrusted protection domain
 * defined by this security policy.
 *
 * @return an array of pointers to MPermission.
 *
 * Leaves with KErrNotFound if an untrusted protection domain is not defined.
 *
 */

const RPointerArray<MPermission>& CSecurityPolicy::UntrustedPermissionsL(void)
{
    return UntrustedProtectionDomainL().Permissions();
}

/**
 *
 * @internal
 *
 * Returns the permissions associated with the named protection domain defined
 * by this security policy.
 *
 * @return an array of pointers to MPermission.
 *
 * Leaves with KErrNotFound if a protection domain with the given name is not
 * defined.
 *
 */

const RPointerArray<MPermission>& CSecurityPolicy::PermissionsL(const TDesC& aName)
{
    return ProtectionDomainL(aName).Permissions();
}

/**
 *
 * @internal
 *
 * Returns the protection domain with the given name defined by this security
 * policy.
 *
 * @return a reference to the named protection domain.
 *
 * Leaves with KErrNotFound if a protection domain with the given name has
 * not been defined.
 *
 */

const MExtendedProtectionDomain& CSecurityPolicy::ProtectionDomainL(
    const TDesC& aName) const
{
    CProtectionDomain* pd = FindProtectionDomain(aName);

    if (pd == NULL)
    {
        User::Leave(KErrNotFound);
    }
    return *pd;
}

/**
 *
 * @internal
 *
 * Returns the untrusted protection domain specified by this security policy,
 *
 * @return a reference to the untrusted protection domain.
 *
 * Leaves with KErrNotFound if an untrusted protection domain is not defined.
 *
 */

const MExtendedProtectionDomain& CSecurityPolicy::UntrustedProtectionDomainL(void) const
{
    if (iUntrustedProtectionDomain == NULL)
    {
        User::Leave(KErrNotFound);
    }
    return *iUntrustedProtectionDomain;
}

// End of MSecurityPolicyV2 implementation

/**
 *
 * @internal
 *
 * Adds the named protection domain to this security policy.
 *
 * @return a pointer to the added protection domain.
 *
 * Leaves with KErrAlreadyExists if a protectiuon domain with this name has
 * already been added.
 *
 */

CProtectionDomain* CSecurityPolicy::AddProtectionDomainL(const TDesC& aName)
{
    if (FindProtectionDomain(aName) != NULL)
    {
        User::Leave(KErrAlreadyExists);
    }

    CProtectionDomain* pd = CProtectionDomain::NewLC(*this, ++iLastId, aName);

    User::LeaveIfError(iProtectionDomains.Append(pd));
    CleanupStack::Pop(pd);
    return pd;
}

const TInt KUntrustedId = -1;

_LIT(KUntrustedName, "");

/**
 *
 * @internal
 *
 * Adds the untrusted protection domain to this security policy.
 *
 * @return a pointer to the added protection domain.
 *
 * Leaves with KErrAlreadyExists if the untrusted protection domain has
 * already been added.
 *
 */

CProtectionDomain* CSecurityPolicy::AddUntrustedProtectionDomainL(void)
{
    if (iUntrustedProtectionDomain != NULL)
    {
        User::Leave(KErrAlreadyExists);
    }
    iUntrustedProtectionDomain = CProtectionDomain::NewLC(*this, KUntrustedId, KUntrustedName);
    CleanupStack::Pop(iUntrustedProtectionDomain);
    return iUntrustedProtectionDomain;
}

/**
 *
 * @internal
 *
 * Adds the named function group to this security policy
 *
 * @param aName the name of the function group to add.
 *
 * @return a pointer to the added function group.
 *
 * Leaves with KErrAleadyExists if a function group with the given name
 * has already been added
 *
 */

CFunctionGroup* CSecurityPolicy::AddFunctionGroupL(const TDesC& aName)
{
    if (FindFunctionGroup(aName) != NULL)
    {
        User::Leave(KErrAlreadyExists);
    }

    CFunctionGroup* fg = CFunctionGroup::NewLC(aName);

    User::LeaveIfError(iFunctionGroups.Append(fg));
    CleanupStack::Pop(fg);
    return fg;
}

/**
 *
 * @internal
 *
 * Adds the permission to the function group.
 *
 * @param aFunctionGroup    the function group to add the permission to,
 *
 * @param aPermission       the name of the permission to add.
 *
 * Leaves with KErrAlreadyExists if the named permission has already been
 * added to a function group.
 *
 */

void CSecurityPolicy::AddPermissionL(CFunctionGroup& aFunctionGroup, const TDesC& aPermission)
{
    TInt permissionCount = iPermissions.Count();

    for (TInt i = 0; i < permissionCount; i++)
    {
        if ((*iPermissions[i]) == aPermission)
        {
            User::Leave(KErrAlreadyExists);
        }
    }

    HBufC* permission = aPermission.AllocLC();

    User::LeaveIfError(iPermissions.Append(permission));
    CleanupStack::Pop(permission);
    aFunctionGroup.AddPermissionL(permission);
}


#define CONST_SAFE_RETURN(t, v) (v)


/**
 *
 * @internal
 *
 * Finds the named function group.
 *
 * @param aName the name of the function group to find.
 *
 * @return a pointer to the function group, or NULL if not found.
 *
 */

CFunctionGroup* CSecurityPolicy::FindFunctionGroup(const TDesC& aName) const
{
    TInt count = iFunctionGroups.Count();

    for (TInt i = 0; i < count; i++)
    {
        if (iFunctionGroups[i]->Name() == aName)
        {
            return CONST_SAFE_RETURN(CFunctionGroup*, iFunctionGroups[i]);
        }
    }
    return NULL;
}

/**
 *
 * @internal
 *
 * Finds the named protection domain.
 *
 * @param aName the name of the protectiomn domain to find.
 *
 * @return a pointer to the protection domain, or NULL if not found.
 *
 */

CProtectionDomain* CSecurityPolicy::FindProtectionDomain(const TDesC& aName) const
{
    TInt count = iProtectionDomains.Count();

    for (TInt i = 0; i < count; i++)
    {
        if (iProtectionDomains[i]->Name() == aName)
        {
            return CONST_SAFE_RETURN(CProtectionDomain*, iProtectionDomains[i]);
        }
    }
    return NULL;
}

} // end of namespace MIDP
