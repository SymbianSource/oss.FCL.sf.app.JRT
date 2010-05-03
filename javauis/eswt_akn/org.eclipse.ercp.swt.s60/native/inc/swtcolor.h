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


#ifndef SWTCOLOR_H
#define SWTCOLOR_H


#include "eswtgraphics.h"


/**
 * CSwtColor
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtColor)
        : public CBase
        , public MSwtColor
{
// Methods
public:
    inline CSwtColor(const MSwtDevice& aDevice, const TRgb& aRgb);

    /**
     * Static overload to be called when CSwtColor object has not been created
     * and the target graphics device can be get e.g. from MSwtGc.
     */
    static TRgb RgbValue(const CGraphicsDevice& aGraphDev, const TRgb& aOriginalColor);

private:
    TRgb ScaleColor(TInt aRedBits, TInt aGreenBits, TInt aBlueBits) const;
    static TRgb ScaleColor(const TRgb& aOriginalColor,
                           const TInt& aRedBits,
                           const TInt& aGreenBits,
                           const TInt& aBlueBits);
    TRgb Gray(TInt aBitCount) const;
    static TRgb Gray(const TRgb& aOriginalColor, const TInt& aBitsInChannel);
    static inline TUint ScaleChannel(TUint aValue, TInt aBitsInChannel);

// From MSwtColor
public:
    void Dispose();
    TRgb RgbValue() const;

// Data
private:
    const MSwtDevice& iDevice;
    const TRgb        iOriginalColor;
};

inline CSwtColor::CSwtColor(const MSwtDevice& aDevice, const TRgb& aRgb)
        : iDevice(aDevice)
        , iOriginalColor(aRgb) // With this TRgb constructor leading zeroes mean fully opaque
{
}


#endif // SWTCOLOR_H
