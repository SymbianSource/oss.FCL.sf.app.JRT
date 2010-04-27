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

#ifndef MPROTECTIONDOMAINTABLE_H
#define MPROTECTIONDOMAINTABLE_H

#include <e32base.h>

namespace MIDP
{

class MProtectionDomain;

/**
 * Abstraction to allow retrieval of named protection domains.
 */
class MProtectionDomainTable
{
public:

    /** Destructor */
    virtual ~MProtectionDomainTable() = 0;

    /**
     * Return the protection domain corresponding to the supplied name.
     * @param   aName
     *          Descriptor references containing the protection domain name.
     * @return  A pointer to the protection domain which the user is responsible
     *          for deleting.
     */
    virtual const MProtectionDomain* ProtectionDomain(const TDesC& aName) = 0;
};

inline MProtectionDomainTable::~MProtectionDomainTable()
{
}

}

#endif // MPROTECTIONDOMAINTABLE_H
