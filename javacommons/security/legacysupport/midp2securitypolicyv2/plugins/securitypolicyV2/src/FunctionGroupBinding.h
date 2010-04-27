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
 * @file FunctionGroupBinding.h
 *
 * @internal
 *
 */

#ifndef FUNCTIONGROUPBINDING_H
#define FUNCTIONGROUPBINDING_H

#include <e32base.h>
#include "j2me/midp2/security/MPermission.h"
#include "MFunctionGroupBinding.h"

namespace MIDP
{
class CFunctionGroup;
class CProtectionDomain;
class CSimpleMapping;

class MFunctionGroup;

class CFunctionGroupBinding: public CBase, public MFunctionGroupBinding
{
public:

    CFunctionGroupBinding(
        CProtectionDomain&,
        CFunctionGroup&,
        TUint32,
        MPermission::TType,
        MPermission::TMode,
        MPermission::TMode);

    CFunctionGroupBinding(
        CProtectionDomain&  aProtectionDomain,
        CFunctionGroup&     aFunctionGroup,
        TUint32             aId,
        MPermission::TType  aPermissionType,
        MPermission::TMode  aDefaultMode,
        TUint32             aInteractionModes);

    virtual ~CFunctionGroupBinding();

    // MFunctionGroupBinding interface

    virtual const MFunctionGroup&   FunctionGroup(void) const;
    virtual TUint32                 Id(void) const;
    virtual MPermission::TType      PermissionType(void) const;
    virtual MPermission::TMode      DefaultInteractionMode(void) const;
    virtual void                    AllowedInteractionModesL(
        RArray<MPermission::TMode>&) const;
    virtual MPermission::TMode      PermissionInteractionModeL(
        const TDesC&,
        MPermission::TMode) const;

    // End of MFunctionGroupBinding interface

    void AddSimpleMappingL(const TDesC&, RArray<MPermission::TMode>&);

private:

    CSimpleMapping* FindSimpleMapping(const TDesC&) const;

private:

    CProtectionDomain&              iProtectionDomain;
    CFunctionGroup&                 iFunctionGroup;
    TUint32                         iId;
    MPermission::TType              iType;
    MPermission::TMode              iDefaultMode;
    TUint32                         iInteractionModesMap;
    RPointerArray<CSimpleMapping>   iMappings;
};

} // end of namespace MIDP

#endif // FUNCTIONGROUPBINDING_H
