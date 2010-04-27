/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
The reference implementation of the Blanket permission policy element.
@file
@internalComponent
@see MIDP::MBlanketPermissionPolicyElement
*/

#include <e32base.h>
#include "BlanketPermissionPolicyElement.h"

namespace MIDP
{

/**
Creates a new blanket permission policy element
@return The blanket permission policy element
*/
CBlanketPermissionPolicyElement* CBlanketPermissionPolicyElement::NewL()
{
    CBlanketPermissionPolicyElement* pe = new(ELeave) CBlanketPermissionPolicyElement();
    return pe;
}

CBlanketPermissionPolicyElement::CBlanketPermissionPolicyElement()
{
}


CBlanketPermissionPolicyElement::~CBlanketPermissionPolicyElement()
{
    iDowngradeFunctionGroupList.ResetAndDestroy();
    iFunctionGroupA.ResetAndDestroy();
    iFunctionGroupB.ResetAndDestroy();
}


const RPointerArray<const HBufC>& CBlanketPermissionPolicyElement::FunctionGroupsA(void) const
{
    return iFunctionGroupA;
}

const RPointerArray<const HBufC>& CBlanketPermissionPolicyElement::FunctionGroupsB(void) const
{
    return iFunctionGroupB;
}

//Assumption that index is always valid.
TPtrC CBlanketPermissionPolicyElement::GetDowngradeFunctionGroup(TInt index) const
{
    const HBufC* fgMode = iDowngradeFunctionGroupList[index];
    TInt loc = fgMode->Locate(TChar(':'));
    return fgMode->Left(loc);
}

//Assumption that index is always valid.
TPtrC16 CBlanketPermissionPolicyElement::GetDowngradeMode(TInt index) const
{
    const HBufC* fgMode = iDowngradeFunctionGroupList[index];
    TInt loc = fgMode->Locate(TChar(':'));
    return fgMode->Right(fgMode->Length()-loc-1);
}

TInt CBlanketPermissionPolicyElement::GetDowngradeOptionsCount(void) const
{
    return iDowngradeFunctionGroupList.Count();
}

void CBlanketPermissionPolicyElement::AppendFunctionGroupAL(
    const HBufC* aFunctionGroup)
{
    iFunctionGroupA.AppendL(aFunctionGroup);
}

void CBlanketPermissionPolicyElement::AppendFunctionGroupBL(
    const HBufC* aFunctionGroup)
{
    iFunctionGroupB.AppendL(aFunctionGroup);
}

void CBlanketPermissionPolicyElement::AppendDowngradeFunctionGroupListL(
    const HBufC* aFGMode)
{
    iDowngradeFunctionGroupList.AppendL(aFGMode);
}


// End of MBlanketPermissionPolicyElement implementation

} // end of namespace MIDP

