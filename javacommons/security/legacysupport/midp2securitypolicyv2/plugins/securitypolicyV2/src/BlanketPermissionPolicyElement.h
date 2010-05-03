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
 *
 * @file BlanketPermissionPolicyElement.h
 *
 * @internal
 *
 */

#ifndef BLANKETPERMISSIONPOLICYELEMENT_H
#define BLANKETPERMISSIONPOLICYELEMENT_H

//#include <e32base.h>
//#include <f32file.h>

#include "MBlanketPermissionPolicyElement.h"

namespace MIDP
{


class CBlanketPermissionPolicyElement: public CBase, public MBlanketPermissionPolicyElement
{
public:
    static CBlanketPermissionPolicyElement* NewL();
    virtual ~CBlanketPermissionPolicyElement();


    // MVersionedInterface interface
    inline virtual TUint32 InterfaceVersion(void) const;

    // MBlanketPermissionPolicyElement interface
    virtual const RPointerArray<const HBufC>& FunctionGroupsA(void) const;

    virtual const RPointerArray<const HBufC>& FunctionGroupsB(void) const;

    //Get the function group at index
    virtual TPtrC GetDowngradeFunctionGroup(TInt index) const;

    virtual TPtrC GetDowngradeMode(TInt index) const;

    virtual TInt GetDowngradeOptionsCount(void) const;

    virtual void AppendFunctionGroupAL(const HBufC*);

    virtual void AppendFunctionGroupBL(const HBufC*);

    //Stored in the format function group:new permission setting
    virtual void AppendDowngradeFunctionGroupListL(const HBufC*);


private:
    CBlanketPermissionPolicyElement();

private:
    //Function group set A
    RPointerArray<const HBufC>  iFunctionGroupA;

    //Function group set B
    RPointerArray<const HBufC>  iFunctionGroupB;

    //Downgrade priority list
    RPointerArray<const HBufC>  iDowngradeFunctionGroupList;
};

inline TUint32 CBlanketPermissionPolicyElement::InterfaceVersion(void) const
{
    return 0x0100;
}

} // end of namespace MIDP

#endif // BLANKETPERMISSIONPOLICYELEMENT_H
