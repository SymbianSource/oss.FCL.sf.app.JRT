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
 * @file FunctionGroupBinding.cpp
 *
 * @internalComponent
 *
 */

#include "FunctionGroup.h"
#include "SimpleMapping.h"
#include "FunctionGroupBinding.h"

/**
 *
 * A concrete implementation of the MFunctionGroupBinding interface
 *
 * @see MFunctionGroupBinding
 *
 */

namespace MIDP
{

/**
 *
 * CFunctionGroupBinding constructor.
 *
 * @param aProtectionDomain the protection domain to which this binding applies.
 *
 * @param aFunctionGroup    the function group to which this binding applies.
 *
 * @param aId               the id of this binding.
 *
 * @param aType             the type of all permissions associated with this binding.
 *
 * @param aDefaultMode      the default interaction mode for all permissions
 *                          associated with this binding.
 *
 * @param aMaximumMode      the 'maximum' interaction mode for all permissions
 *                          associated with this binding.
 *
 */

CFunctionGroupBinding::CFunctionGroupBinding(
    CProtectionDomain& aProtectionDomain,
    CFunctionGroup&    aFunctionGroup,
    TUint32            aId,
    MPermission::TType aType,
    MPermission::TMode aDefaultMode,
    MPermission::TMode aMaximumMode):
        iProtectionDomain(aProtectionDomain),
        iFunctionGroup(aFunctionGroup),
        iId(aId),
        iType(aType),
        iDefaultMode(aDefaultMode)
{
    switch (aMaximumMode)
    {
    case MPermission::EBlanket:

        iInteractionModesMap |= (1 << MPermission::EBlanket);

        // FALL-THROUGH

    case MPermission::ESession:

        iInteractionModesMap |= (1 << MPermission::ESession);

        // FALL-THROUGH

    case MPermission::EOneshot:

        iInteractionModesMap |= (1 << MPermission::EOneshot);

        // FALL-THROUGH

    default:

        iInteractionModesMap |= (1 << MPermission::EDenied);
        break;
    }
}

/**
 *
 * CFunctionGroupBinding constructor.
 *
 * @param aProtectionDomain the protection domain to which this binding applies.
 *
 * @param aFunctionGroup    the function group to which this binding applies.
 *
 * @param aId               the id of this binding.
 *
 * @param aType             the type of all permissions associated with this binding.
 *
 * @param aDefaultMode      the default interaction mode for all permissions
 *                          associated with this binding.
 *
 * @param aInteractionModes ...
 *
 *
 */

CFunctionGroupBinding::CFunctionGroupBinding(
    CProtectionDomain& aProtectionDomain,
    CFunctionGroup&    aFunctionGroup,
    TUint32            aId,
    MPermission::TType aType,
    MPermission::TMode aDefaultMode,
    TUint32            aInteractionModes):
        iProtectionDomain(aProtectionDomain),
        iFunctionGroup(aFunctionGroup),
        iId(aId),
        iType(aType),
        iDefaultMode(aDefaultMode),
        iInteractionModesMap(aInteractionModes)
{
}

CFunctionGroupBinding::~CFunctionGroupBinding()
{
    iMappings.ResetAndDestroy();
}

// MFunctionGroupBinding interface implementation

const MFunctionGroup& CFunctionGroupBinding::FunctionGroup(void) const
{
    return iFunctionGroup;
}

TUint32 CFunctionGroupBinding::Id(void) const
{
    return iId;
}

MPermission::TType CFunctionGroupBinding::PermissionType(void) const
{
    return iType;
}

MPermission::TMode CFunctionGroupBinding::DefaultInteractionMode(void) const
{
    return iDefaultMode;
}

void CFunctionGroupBinding::AllowedInteractionModesL(RArray<MPermission::TMode>& aModes) const
{
    if (iInteractionModesMap & (1 << MPermission::EBlanket))
    {
        User::LeaveIfError(aModes.Append(MPermission::EBlanket));
    }
    if (iInteractionModesMap & (1 << MPermission::ESession))
    {
        User::LeaveIfError(aModes.Append(MPermission::ESession));
    }
    if (iInteractionModesMap & (1 << MPermission::EOneshot))
    {
        User::LeaveIfError(aModes.Append(MPermission::EOneshot));
    }
    if (iInteractionModesMap & (1 << MPermission::EDenied))
    {
        User::LeaveIfError(aModes.Append(MPermission::EDenied));
    }
}

MPermission::TMode CFunctionGroupBinding::PermissionInteractionModeL(
    const TDesC&       aPermission,
    MPermission::TMode aCurrent) const
{
    CSimpleMapping* sm = FindSimpleMapping(aPermission);

    return (sm != NULL) ? sm->Mapping(aCurrent) : aCurrent;
}

// End of MFunctionGroupBinding interface implementation

/**
 *
 * Adds a Simple mapping to this binding.
 *
 * @param aPermission   the name of the permission to which this binding applies.
 *
 * @param aMapping      the mapping in p-list format (key1, value1, key2, value2, ...)
 *
 * Leaves with KErrAlreadyExists if a Simple mapping has already been added for the
 * given permission.
 *
 */

void CFunctionGroupBinding::AddSimpleMappingL(
    const TDesC&                aPermission,
    RArray<MPermission::TMode>& aMapping)
{
    if (FindSimpleMapping(aPermission) != NULL)
    {
        User::Leave(KErrAlreadyExists);
    }

    CSimpleMapping* sm = CSimpleMapping::NewLC(aPermission, aMapping);

    User::LeaveIfError(iMappings.Append(sm));
    CleanupStack::Pop(sm);
}


#define CONST_SAFE_RETURN(t, v) (v)


/**
 *
 * Finds the Simple mapping for the given permission.
 *
 * @param aPermission   the name of the permission to find the mapping for.
 *
 * @return a pointer to a Simple mapping, or NULL if not found.
 *
 */

CSimpleMapping* CFunctionGroupBinding::FindSimpleMapping(const TDesC& aPermission) const
{
    TInt count = iMappings.Count();

    for (TInt i = 0; i < count; i++)
    {
        if (iMappings[i]->Permission() == aPermission)
        {
            return CONST_SAFE_RETURN(CSimpleMapping*, iMappings[i]);
        }
    }
    return NULL;
}

} // end of namespace MIDP
