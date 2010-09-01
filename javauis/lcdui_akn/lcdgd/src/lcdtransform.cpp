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

#include "lcdtransform.h"

EXPORT_C TPoint TLcdTransform::operator()(const TPoint& aPoint) const
{
    return TPoint
           (
               aPoint.iX*iDuDx + aPoint.iY*iDuDy + iU0,
               aPoint.iX*iDvDx + aPoint.iY*iDvDy + iV0
           );
}

EXPORT_C TLcdTransform TLcdTransform::operator *(const TLcdTransform& aT) const
{
    TLcdTransform t;

    t.iDuDx = iDuDx*aT.iDuDx + iDuDy*aT.iDvDx;
    t.iDuDy = iDuDx*aT.iDuDy + iDuDy*aT.iDvDy;
    t.iDvDx = iDvDx*aT.iDuDx + iDvDy*aT.iDvDx;
    t.iDvDy = iDvDx*aT.iDuDy + iDvDy*aT.iDvDy;
    t.iU0   = iDuDx*aT.iU0   + iDuDy*aT.iV0 + iU0;
    t.iV0   = iDvDx*aT.iU0   + iDvDy*aT.iV0 + iV0;

    ASSERT(Abs(t.iDuDx) <= 1);
    ASSERT(Abs(t.iDuDy) <= 1);
    ASSERT(Abs(t.iDvDx) <= 1);
    ASSERT(Abs(t.iDvDy) <= 1);
    ASSERT(Abs(t.iDuDx*t.iDvDy - t.iDuDy*t.iDvDx) == 1);

    return t;
}

EXPORT_C TBool TLcdTransform::operator == (const TLcdTransform& aT) const
{
    return (iDuDx == aT.iDuDx) &&
           (iDuDy == aT.iDuDy) &&
           (iDvDx == aT.iDvDx) &&
           (iDvDy == aT.iDvDy) &&
           (iU0   == aT.iU0) &&
           (iV0   == aT.iV0)  ;
}

EXPORT_C TLcdTransform TLcdTransform::Inverse() const
{
    TLcdTransform t;

    TInt det = iDuDx*iDvDy - iDuDy*iDvDx;

    ASSERT(Abs(det) == 1);

    TInt a = iDuDx;
    TInt b = iDuDy;
    TInt c = iDvDx;
    TInt d = iDvDy;
    TInt u = iU0;
    TInt v = iV0;

    t.iDuDx = d * det;
    t.iDuDy = - b * det;
    t.iDvDx = - c * det;
    t.iDvDy = a * det;

    t.iU0 = (b * v - d * u) * det;
    t.iV0 = (c * u - a * v) * det;

    return t;
}

EXPORT_C TLcdTransform Transform(TTransformType aType, const TSize& aSize)
{
    TLcdTransform t = TLcdTransform();

    switch (aType)
    {
    case ETransNone:
    {
        t.iDuDx = 1;
        t.iDvDx = 0;
        t.iDuDy = 0;
        t.iDvDy = 1;
        t.iU0   = 0;
        t.iV0   = 0;
    }
    break;
    case ETransMirrorRot180:    // EReflectVert
    {
        t.iDuDx = 1;
        t.iDvDx = 0;
        t.iDuDy = 0;
        t.iDvDy = -1;
        t.iU0   = 0;
        t.iV0   = aSize.iHeight - 1;
    }
    break;
    case ETransMirror:          // EReflectHorz
    {
        t.iDuDx = -1;
        t.iDvDx = 0;
        t.iDuDy = 0;
        t.iDvDy = 1;
        t.iU0   = aSize.iWidth - 1;
        t.iV0   = 0;
    }
    break;
    case ETransRot180:          // EReflectVert|EReflectHorz
    {
        t.iDuDx = -1;
        t.iDvDx = 0;
        t.iDuDy = 0;
        t.iDvDy = -1;
        t.iU0 = aSize.iWidth - 1;
        t.iV0 = aSize.iHeight - 1;
    }
    break;
    case ETransMirrorRot270:    // EReflectDiag
    {
        t.iDuDx = 0;
        t.iDvDx = 1;
        t.iDuDy = 1;
        t.iDvDy = 0;
        t.iU0   = 0;
        t.iV0   = 0;
    }
    break;
    case ETransRot90:           // EReflectVert|EReflectDiag
    {
        t.iDuDx = 0;
        t.iDvDx = 1;
        t.iDuDy = -1;
        t.iDvDy = 0;
        t.iU0   = aSize.iHeight-1;
        t.iV0   = 0;
    }
    break;

    case ETransRot270:          // EReflectHorz|EReflectDiag
    {
        t.iDuDx = 0;
        t.iDvDx = -1;
        t.iDuDy = 1;
        t.iDvDy = 0;
        t.iU0   = 0;
        t.iV0   = aSize.iWidth - 1;
    }
    break;

    case ETransMirrorRot90:     // EReflectVert|EReflectHorz|EReflectDiag
    {
        t.iDuDx = 0;
        t.iDvDx = -1;
        t.iDuDy = -1;
        t.iDvDy = 0;
        t.iU0 = aSize.iHeight - 1;
        t.iV0 = aSize.iWidth  - 1;
    }
    break;
    default:
        ASSERT(aType != aType);     // unknown type.
        break;
    }

    return t;
}


EXPORT_C TLcdTransform Identity()
{
    TLcdTransform t;
    t.iDuDx = 1;
    t.iDuDy = 0;
    t.iDvDx = 0;
    t.iDvDy = 1;
    t.iU0   = 0;
    t.iV0   = 0;
    return t;
}

/**
 * Compose the vertical reflection.
 *
 * Maps from source (image) coords to destination (graphics) coords.
 */
EXPORT_C TLcdTransform ReflectVert(const TSize& aSize)
{
    return Transform((TTransformType)EReflectVert, aSize);
}

/**
 * Compose the horizontal reflection.
 *
 * Maps from source (image) coords to destination (graphics) coords.
 */
EXPORT_C TLcdTransform ReflectHorz(const TSize& aSize)
{
    return Transform((TTransformType)EReflectHorz, aSize);
}

/**
 * Compose the diagonal reflection.
 *
 * Maps from source (image) coords to destination (graphics) coords.
 */
EXPORT_C TLcdTransform ReflectDiag()
{
    return Transform((TTransformType)EReflectDiag, TSize(0,0));
}

/**
 * Compose translation.
 *
 * Maps from source (image) coords to destination (graphics) coords.
 *
 */
EXPORT_C TLcdTransform Translate(const TPoint& aPoint)
{
    TLcdTransform t;
    t.iDuDx = 1;
    t.iDuDy = 0;
    t.iDvDx = 0;
    t.iDvDy = 1;
    t.iU0   = aPoint.iX;
    t.iV0   = aPoint.iY;
    return t;
}
