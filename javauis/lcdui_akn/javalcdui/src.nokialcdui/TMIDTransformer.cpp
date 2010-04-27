/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used to get basic manipulations to the bitmaps.
*
*/


//  INCLUDE FILES
#include <e32std.h>
#include "TMIDTransformer.h"

void TMIDTransformer::Transform(TRect& aDrawRect, TInt aTransform)
{
    iSize = aDrawRect.Size();
    iTransform = aTransform;
    TInt rotates(aTransform & KRotates);
    if (rotates == KRotate90 || rotates == KRotate270)
    {
        aDrawRect.SetSize(TSize(aDrawRect.Height(), aDrawRect.Width()));
    }
}



TInt TMIDTransformer::Transform(const TPoint& aMovement)
{
    TInt error(KErrNone);
    TInt hFlip(1);
    TInt vFlip(1);
    TInt flip(iTransform & KFlips);

    if ((flip & KFlipVertical) != 0)
    {
        vFlip = -1;
    }

    if ((flip & KFlipHorizontal) != 0)
    {
        hFlip = -1;
    }

    TInt rotates(iTransform & KRotates);

    switch (rotates)
    {
    case KRotate90:
    {
        iXMove.SetXY(0, 1 * hFlip);
        iYMove.SetXY(-1 * vFlip, 0);
        if (vFlip == -1)
        {
            iBitPosition.iX = aMovement.iY;
        }
        else
        {
            iBitPosition.iX = iSize.iWidth - aMovement.iY - 1;
        }

        if (hFlip == -1)
        {
            iBitPosition.iY = iSize.iHeight - aMovement.iX - 1;
        }
        else
        {
            iBitPosition.iY = aMovement.iX;
        }
        break;
    }
    case KRotate180:
    {
        iXMove.SetXY(-1 * hFlip, 0);
        iYMove.SetXY(0, -1 * vFlip);

        if (hFlip == -1)
        {
            iBitPosition.iX = aMovement.iX;
        }
        else
        {
            iBitPosition.iX = iSize.iWidth - aMovement.iX - 1;
        }

        if (vFlip == -1)
        {
            iBitPosition.iY = aMovement.iY;
        }
        else
        {
            iBitPosition.iY = iSize.iHeight - aMovement.iY - 1;
        }

        break;
    }
    case KRotate270:
    {
        iXMove.SetXY(0, -1 * hFlip);
        iYMove.SetXY(1 * vFlip, 0);

        if (vFlip == -1)
        {
            iBitPosition.iX = iSize.iWidth - aMovement.iY - 1;
        }
        else
        {
            iBitPosition.iX = aMovement.iY;
        }

        if (hFlip == -1)
        {
            iBitPosition.iY = aMovement.iX;
        }
        else
        {
            iBitPosition.iY = iSize.iHeight - aMovement.iX - 1;
        }

        break;
    }
    // No rotate, so rotate angle is 0
    case 0:
    {
        iXMove.SetXY(1 * hFlip, 0);
        iYMove.SetXY(0, 1 * vFlip);
        if (hFlip == -1)
        {
            iBitPosition.iX = iSize.iWidth - aMovement.iX - 1;
        }
        else
        {
            iBitPosition.iX = aMovement.iX;
        }

        if (vFlip == -1)
        {
            iBitPosition.iY = iSize.iHeight - aMovement.iY - 1;
        }
        else
        {
            iBitPosition.iY = aMovement.iY;
        }

        break;
    }
    default:
    {
        error = KErrGeneral;
        break;
    }
    }

    iPoint = iBitPosition;
    return error;
}




