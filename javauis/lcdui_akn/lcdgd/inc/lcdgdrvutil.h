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

#ifndef LCDGDRVUTIL_H
#define LCDGDRVUTIL_H

#include "lcdgdrvif.h"

const TUint KTransNoneMask =  ETransNoneBit;
const TUint KTransAllMask  =  ETransNoneBit
                              | ETransMirrorRot180Bit
                              | ETransMirrorBit
                              | ETransRot180Bit
                              | ETransMirrorRot270Bit
                              | ETransRot90Bit
                              | ETransRot270Bit
                              | ETransMirrorRot90Bit;
// These two transformations can both be handled more easily in some cases,
// where pixels are in the same horizontal order, so Mem::Move() can be used.
const TUint KTransSimpleMask =  ETransNoneBit
                                | ETransMirrorRot180Bit;

#define IMAGE_TYPE(CM,AM,TT) { CM, AM, TT }

#define BLITTER_ENTRY(DT,ST,F) { DT, ST, KTransAllMask, ECompositeSrcCopy, &F }
#define SIMPLE_BLITTER_ENTRY(DT,ST,F) { DT, ST, KTransSimpleMask, ECompositeSrcCopy, &F }
#define RENDER_ENTRY(DT,ST,TM,F) { DT, ST, TM, ECompositeSrcOver, &F }
#define COLLISION_DETECTOR_ENTRY(M1,T1,M2,T2,F) { M1,T1,M2,T2,&F }

#define JAVA_OPAQUE_IMAGE IMAGE_TYPE(EColorARGB8888, ENone, ETransparencyIgnoreChannel)
#define JAVA_ALPHA_IMAGE  IMAGE_TYPE(EColorARGB8888, ENone, ETransparencyAlphaChannel)

#define UNUSED(X)   (void)X

#ifndef _DEBUG
#define INLINE inline
#else
#define INLINE
#endif

struct TEdge
{
    TInt    iDx;        // difference in x coords of end points
    TUint32 iAbsDeltaX;
    TInt32  iSignDeltaX;
    TInt    iDy;        // difference in y coords of end points - always positive due to end point order.
    TUint32 iAbsDeltaY;
    TInt32  iSignDeltaY;
    TInt    iX;         // normalized integer part of x coordinate.
    TInt    iF;         // numerator of normalized fractional part of x coordinate
    TInt    iD;         // denominator of normalized fractional part of x coordinate
    TInt    iIx;        // integer increment (normalized)
    TInt    iIf;        // fractional increment (normalized)
    TInt    iDe;        // Sign(iDx)
};

struct TEdge64
{
    TInt64  iDx;        // difference in x coords of end points
    TUint32 iAbsDeltaX;
    TInt32  iSignDeltaX;
    TInt64  iDy;        // difference in y coords of end points - always positive due to end point order.
    TUint32 iAbsDeltaY;
    TInt32  iSignDeltaY;
    TInt64  iX;         // normalized integer part of x coordinate.
    TInt64  iF;         // numerator of normalized fractional part of x coordinate
    TInt64  iD;         // denominator of normalized fractional part of x coordinate
    TInt64  iIx;        // integer increment (normalized)
    TInt64  iIf;        // fractional increment (normalized)
    TInt64  iDe;        // Sign(iDx)
};

/**
 * Calculate rational increment stepping in positive y direction.
 *
 *@param aDx difference in x coordinates of edge end points
 *@param aDy difference in y coordinates of edge end points, always positive (y sorted vertices)
 *@return aIx integer increment
 *@return aIf positive fractional increment with denominator aDy
 *
 */
void Increment(TInt& aIx, TInt& aIf, TInt aDx, TInt aDy);

void Increment(TInt64& aIx, TInt64& aIf, TInt64 aDx, TInt64 aDy);

/**
 *@return sign of aValue
 */
inline TInt Sign(TInt aValue)
{
    if (aValue > 0)
    {
        return 1;
    }
    if (aValue < 0)
    {
        return -1;
    }
    return 0;
}

/**
 * Porter-Duff src over destination alpha blend for channels of up to
 * 7 bits, with alpha premultiplied by 257
 */
inline TUint32 Blend7PreScaled(TUint32 aAlpha, TUint32 aSrc, TUint32 aDst)
{
    return (((aSrc - aDst)*aAlpha >> 16) + aDst) & 0xff;
}

/**
 * Porter-Duff src over destination alpha blend for channels of up to
 * 8 bits, with alpha premultiplied by 257
 */
inline TUint32 Blend8PreScaled(TUint32 aAlpha, TUint32 aSrc, TUint32 aDst)
{
    return ((aSrc - aDst)*aAlpha + (aDst << 16) - aDst + 255) >> 16;
}

/**
 * 16bpp routines use Blend16
 */
#define Blend16 Blend7PreScaled

/**
 * 32bpp routines use Blend32 (for RGB888 Blend7PreScaled roundoff works againts us)
 */
#define Blend32 Blend8PreScaled
#endif // LCDGDRVUTIL_H
