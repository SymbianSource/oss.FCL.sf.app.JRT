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


#ifndef SWTBITMAPGC_H
#define SWTBITMAPGC_H


#include "swtgcbase.h"


/**
 * CSwtBitmapGc
 * A GC for the font & bitmap server's devices
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBitmapGc)
        : public CSwtGcBase
{
public:
    static CSwtBitmapGc* NewL(MSwtBitmapDrawable& aDrawable,
    CFbsBitGc* aGc, MSwtGc::TDestructor aDestructor,
    const TRgb& aPenCol, const TRgb& aBrushCol,
    const MSwtFont& aFont);

private:
    inline CSwtBitmapGc(MSwtBitmapDrawable& aDrawable,
                        CFbsBitGc* aGc, MSwtGc::TDestructor aDestructor,
                        const TRgb& aPenCol, const TRgb& aBrushCol,
                        const MSwtFont& aFont, const TSize& aSize);

// From MSwtGc
public:
    void CopyAreaL(MSwtImage& aImage, const TPoint& aPoint) const;
};


#endif // SWTBITMAPGC_H
