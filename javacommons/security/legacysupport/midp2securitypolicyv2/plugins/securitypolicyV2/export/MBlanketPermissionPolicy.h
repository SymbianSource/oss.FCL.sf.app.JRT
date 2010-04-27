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



#ifndef MBLANKETPERMISSIONPOLICY_H
#define MBLANKETPERMISSIONPOLICY_H

/**
 * @file
 * @publishedPartner
 * @interim
 */

#include "j2me/midp2/core/mversionedinterface.h"

namespace MIDP
{

class MBlanketPermissionPolicyElement;

/**
An abstract operational interface to a policy for administration of blanket permissions.
*/

class MBlanketPermissionPolicy: public MVersionedInterface
{
public:

    virtual ~MBlanketPermissionPolicy() = 0;

    virtual TInt GetPolicyElementCount() const =0;

    virtual const MBlanketPermissionPolicyElement* GetPolicyElement(TInt index) const =0 ;

};

inline MBlanketPermissionPolicy::~MBlanketPermissionPolicy()
{
}

} // end of namespace MIDP

#endif // MBLANKETPERMISSIONPOLICY_H
