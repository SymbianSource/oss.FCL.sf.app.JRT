/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTWINDOWGC_H
#define SWTWINDOWGC_H


#include "swtgcbase.h"


/**
 * CSwtWindowGc
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtWindowGc)
        : public CSwtGcBase
{
public:
    static CSwtWindowGc* NewL(MSwtWindowDrawable& aDrawable, CWindowGc* aGc,
    TDestructor aDestructor, const TRgb& aPenCol, const TRgb& aBrushCol,
    const MSwtFont& aFont, const TRect& aBoundaryRect, const TRect& aGcScreenRect);

private:
    inline CSwtWindowGc(MSwtWindowDrawable& aDrawable, CWindowGc* aGc,
                        TDestructor aDestructor, const TRgb& aPenCol, const TRgb& aBrushCol,
                        const MSwtFont& aFont, const TRect& aBoundaryRect, const TRect& aGcScreenRect);

// From MSwtGc
public:
    void CopyAreaL(MSwtImage& aImage, const TPoint& aPoint) const;
#ifdef RD_JAVA_NGA_ENABLED
    void CopyArea(const TRect& aSource, const TPoint& aDestination);
#endif

private:
    const TPoint iOriginPosition; // Position of the origin of the GC; in screen coordinate, in pixels.
};


#endif // SWTWINDOWGC_H
