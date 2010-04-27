/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MBLANKETPERMISSIONPOLICYELEMENT_H
#define MBLANKETPERMISSIONPOLICYELEMENT_H

/**
@file
@publishedPartner
@see MIDP::MBlanketPermissionPolicyElement
*/

#include <e32base.h>
#include "j2me/midp2/core/mversionedinterface.h"


namespace MIDP
{

/**
Describes
*/
class MBlanketPermissionPolicyElement : public MVersionedInterface
{
public:

    virtual ~MBlanketPermissionPolicyElement() = 0;

    // MBlanketPermissionPolicyElement interface
    virtual const RPointerArray<const HBufC>& FunctionGroupsA(void) const = 0;

    virtual const RPointerArray<const HBufC>& FunctionGroupsB(void) const = 0;

    //Get the function group at index
    virtual TPtrC GetDowngradeFunctionGroup(TInt index) const = 0;

    virtual TPtrC GetDowngradeMode(TInt index) const = 0;

    virtual TInt GetDowngradeOptionsCount(void) const = 0;

    virtual void AppendFunctionGroupAL(const HBufC*) = 0;

    virtual void AppendFunctionGroupBL(const HBufC*) = 0;

    //Stored in the format "function group:new permission setting"
    virtual void AppendDowngradeFunctionGroupListL(const HBufC*) = 0;

};

inline MBlanketPermissionPolicyElement::~MBlanketPermissionPolicyElement()
{
}

} // end of namespace MIDP

#endif // MBLANKETPERMISSIONPOLICYELEMENT_H
