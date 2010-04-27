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
The reference implementation of the Blanket permission policy .
@file
@internalComponent
@see MIDP::MBlanketPermissionPolicy
*/

#include <e32base.h>

#include "BlanketPermissionPolicyElement.h"
#include "BlanketPermissionPolicy.h"

namespace MIDP
{

/**
Creates a new blanket permission policy
@return The blanket permission policy
*/
CBlanketPermissionPolicy* CBlanketPermissionPolicy::NewL()
{
    CBlanketPermissionPolicy* bpe = new(ELeave) CBlanketPermissionPolicy();
    CleanupStack::PushL(bpe);
    bpe->ConstructL();
    CleanupStack::Pop(bpe);
    return bpe;
}

CBlanketPermissionPolicy::CBlanketPermissionPolicy()
{
}


CBlanketPermissionPolicy::~CBlanketPermissionPolicy()
{
    //Delete contents of RPointerArray
    iPolicyElements.ResetAndDestroy();
}

void CBlanketPermissionPolicy::ConstructL()
{

}

void CBlanketPermissionPolicy::Add(const CBlanketPermissionPolicyElement *aElement)
{
    iPolicyElements.Append(aElement);
}


TInt CBlanketPermissionPolicy::GetPolicyElementCount() const
{
    return iPolicyElements.Count();
}

const MBlanketPermissionPolicyElement* CBlanketPermissionPolicy::GetPolicyElement(TInt index) const
{
    return iPolicyElements[index];
}


// End of MBlanketPermissionPolicy implementation

} // end of namespace MIDP

