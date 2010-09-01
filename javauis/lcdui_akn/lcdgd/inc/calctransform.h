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


#ifndef CALCTRANSFORM_H
#define CALCTRANSFORM_H

#include <gdi.h>
#include <lcdgdrv.h>
#include "lcdgdrvif.h"
#include "lcdtransform.h"


/**
 * Compute the TLcdTransform mapping between aSrcRect and aDstRect with transform type
 * aSrcTransform.
 */
IMPORT_C TLcdTransform CalcTransform
(
    const TRect&   aDstRect,            // unclipped
    const TRect&   aSrcRect,            // unclipped
    TTransformType aSrcTransform
);

/**
 * Calculate clipped source and destination rectangles for
 * src to dst transform aSrcTransform. This function calculates
 * the largest corresponding subrectangles of aDstRect and aSrcRect
 * that also lie within aDstClipRect and aSrcClipRect respectively.
 */
IMPORT_C void ClipTransformRect
(
    TRect& aDstRect,                    // in/out
    TRect& aSrcRect,                    // in/out
    const TRect& aDstClipRect,          // input
    const TRect& aSrcClipRect,          // input
    TLcdTransform& aSrcTransform        // src to target transform
);

/**
 * Clip source and dst rects against source and dst clip rects, maintaining mapping (i.e. no stretch)
 */
IMPORT_C void ClipRects(TRect& aDstRect, const TRect& aDstClip, TRect& aSrcRect, const TRect& aSrcClip);

/**
 *@return ETrue if aSrcRect and aDstRect correspond under transformation aTransform
 */
IMPORT_C TBool CheckTransform(const TRect& aDstRect, const TRect& aSrcRect, const TLcdTransform aTransform);

/**
 *@return ETrue if aDstRect is completely contained within rectangle of size aDstSize, and
 * aDstRect transformed by aTransform is completely contained within rectangle of size aSrcSize.
 */
IMPORT_C TBool CheckBounds(const TSize& aDstSize, const TSize& aSrcSize, const TRect& aDstRect, const TLcdTransform& aTransform);

#endif // CALCTRANSFORM_H
