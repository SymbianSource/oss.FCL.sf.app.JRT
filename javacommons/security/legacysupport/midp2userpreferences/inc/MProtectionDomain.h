/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
@file
@publishedPartner
@released
*/

#ifndef MPROTECTIONDOMAIN_H
#define MPROTECTIONDOMAIN_H

#include <e32base.h>

namespace MIDP
{
/**
 * Abstraction to manage the runtime use of Protection Domains.
 */
class MProtectionDomain
{
public:

    /**
     * Returns the list of permissions for this Protection Domain.
     *
     */
    virtual const RPointerArray<MPermission>& Permissions(void) const = 0;
};


}

#endif // MPROTECTIONDOMAIN_H
