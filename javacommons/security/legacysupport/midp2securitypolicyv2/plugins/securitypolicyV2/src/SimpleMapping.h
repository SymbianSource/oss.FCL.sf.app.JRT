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
 * @file SimpleMapping.h
 *
 * @internal
 *
 */

#ifndef SIMPLEMAPPING_H
#define SIMPLEMAPPING_H

#include <e32base.h>
#include "j2me/midp2/security/MPermission.h"

namespace MIDP
{

class CSimpleMapping: public CBase
{
public:

    static CSimpleMapping* NewLC(const TDesC&, RArray<MPermission::TMode>&);

public:

    virtual ~CSimpleMapping();

    const TDesC& Permission(void) const;

    MPermission::TMode Mapping(MPermission::TMode aCurrent) const;

private:

    CSimpleMapping();

    void ConstructL(const TDesC&, RArray<MPermission::TMode>&);

private:

    HBufC*                      iPermission;
    RArray<MPermission::TMode>  iMapping;
};

} // end of namespace MIDP

#endif // SIMPLEMAPPING_H
