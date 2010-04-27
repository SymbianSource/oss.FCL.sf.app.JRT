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
 * @file BlanketPermissionPolicy.h
 *
 * @internal
 *
 */

#ifndef BLANKETPERMISSIONPOLICY_H
#define BLANKETPERMISSIONPOLICY_H

//#include <e32base.h>
//#include <f32file.h>

#include "MBlanketPermissionPolicy.h"

namespace MIDP
{

class MBlanketPermissionPolicyElement;
class CBlanketPermissionPolicyElement;

class CBlanketPermissionPolicy: public CBase, public MBlanketPermissionPolicy
{

public:
    // MVersionedInterface interface
    inline virtual TUint32 InterfaceVersion(void) const;

    virtual ~CBlanketPermissionPolicy();
    static CBlanketPermissionPolicy* NewL();


    void Add(const CBlanketPermissionPolicyElement *);

    // MBlanketPermissionPolicy
    virtual TInt GetPolicyElementCount() const ;

    virtual const MBlanketPermissionPolicyElement* GetPolicyElement(TInt index) const ;

private:
    CBlanketPermissionPolicy();

    void ConstructL();

private:
    RPointerArray<const CBlanketPermissionPolicyElement> iPolicyElements;
};

inline TUint32 CBlanketPermissionPolicy::InterfaceVersion(void) const
{
    return 0x0100;
}

} // end of namespace MIDP


#endif // BLANKETPERMISSIONPOLICY_H
