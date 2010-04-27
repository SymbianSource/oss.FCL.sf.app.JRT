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


#ifndef LCDTRANSFORM_H
#define LCDTRANSFORM_H

#include <gdi.h>
#include <lcdgdrv.h>

/**
 * Simple symmetry group + translation transformation.
 *
 *@internal
 */
class TLcdTransform
{
public:
    IMPORT_C TPoint operator()(const TPoint&) const;
    IMPORT_C TLcdTransform operator*(const TLcdTransform& aTransform) const;
    IMPORT_C TBool operator == (const TLcdTransform&) const;
    IMPORT_C TLcdTransform Inverse() const;

    TInt iDuDx;
    TInt iDuDy;
    TInt iDvDx;
    TInt iDvDy;
    TInt iU0;
    TInt iV0;
};

/**
 * Construct transform for given transform type.
 */
IMPORT_C TLcdTransform Transform(TTransformType aType, const TSize& aSize);

/**
 * Construct identity transformation.
 */
IMPORT_C TLcdTransform Identity();

/**
 * Construct vertical reflection.
 *
 * Maps from source (image) coords to destination (graphics) coords.
 */
IMPORT_C TLcdTransform ReflectVert(const TSize& aSize);

/**
 * Construct horizontal reflection.
 *
 * Maps from source (image) coords to destination (graphics) coords.
 */
IMPORT_C TLcdTransform ReflectHorz(const TSize& aSize);

/**
 * Construct diagonal reflection.
 *
 * Maps from source (image) coords to destination (graphics) coords.
 */
IMPORT_C TLcdTransform ReflectDiag();

/**
 * Construct translation.
 *
 * Maps from source (image) coords to destination (graphics) coords.
 *
 */
IMPORT_C TLcdTransform Translate(const TPoint& aPoint);

#endif // LCDTRANSFORM_H
