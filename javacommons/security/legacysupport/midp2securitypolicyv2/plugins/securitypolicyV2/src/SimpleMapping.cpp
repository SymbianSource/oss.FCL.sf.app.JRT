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
 * @file SimpleMapping.cpp
 *
 * @internal
 *
 */

#include "SimpleMapping.h"

/*
 *
 * A 'Simple Mapping' is used to represent the mapping between the current
 * interaction mode for a function group and the corresponding interaction
 * mode for a permission within that function group.
 *
 */

namespace MIDP
{

CSimpleMapping* CSimpleMapping::NewLC(const TDesC&                aPermission,
                                      RArray<MPermission::TMode>& aMapping)
{
    CSimpleMapping* sm = new(ELeave) CSimpleMapping;

    CleanupStack::PushL(sm);
    sm->ConstructL(aPermission, aMapping);
    return sm;
}

CSimpleMapping::CSimpleMapping()
{
}

void CSimpleMapping::ConstructL(const TDesC& aPermission, RArray<MPermission::TMode>& aMapping)
{
    iPermission = aPermission.AllocL();

    TInt count = aMapping.Count();

    ASSERT((count % 2) == 0);

    for (TInt i = 0; i < count; i++)
    {
        iMapping.Append(aMapping[i]);
    }
}

CSimpleMapping::~CSimpleMapping()
{
    delete iPermission;
    iMapping.Close();
}

/*
 *
 * Returns the name of the permission to which this mapping applies.
 *
 */

const TDesC& CSimpleMapping::Permission(void) const
{
    return *iPermission;
}

/*
 *
 * Returns the interaction mode of the permission to which this mapping
 * applies, given the current interaction mode for the function group.
 *
 */

MPermission::TMode CSimpleMapping::Mapping(MPermission::TMode aCurrent) const
{
    TInt count = iMapping.Count();

    for (TInt i = 0; i < count; i += 2)
    {
        if (iMapping[i] == aCurrent)
        {
            return iMapping[i+1];
        }
    }
    return aCurrent;
}

} // end of namespace MIDP

