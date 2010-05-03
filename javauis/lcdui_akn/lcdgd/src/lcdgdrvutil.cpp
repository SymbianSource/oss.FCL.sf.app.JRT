/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include "lcdgdrvutil.h"

/**
 * Calculate rational increment stepping in positive y direction.
 *
 *@param aDx difference in x coordinates of edge end points
 *@param aDy difference in y coordinates of edge end points, always positive (y sorted vertices)
 *@return aIx integer increment
 *@return aIf positive fractional increment with denominator aDy
 *
 */
void Increment(TInt& aIx, TInt& aIf, TInt aDx, TInt aDy)
{
    ASSERT(aDy > 0);

    TInt ip = aDx / aDy;
    TInt fp = aDx % aDy;

    if (fp < 0)
    {
        aIx = ip - 1;
        aIf = fp + aDy;
    }
    else
    {
        aIx = ip;
        aIf = fp;
    }
}

void Increment(TInt64& aIx, TInt64& aIf, TInt64 aDx, TInt64 aDy)
{
    ASSERT(aDy > 0);

    TInt64 ip = aDx / aDy;
    TInt64 fp = aDx % aDy;

    if (fp < 0)
    {
        aIx = ip - 1;
        aIf = fp + aDy;
    }
    else
    {
        aIx = ip;
        aIf = fp;
    }
}

