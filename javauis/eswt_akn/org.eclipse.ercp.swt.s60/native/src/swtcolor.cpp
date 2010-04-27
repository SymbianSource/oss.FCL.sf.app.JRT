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


#include <gdi.h>
#include "swtcolor.h"


// ======== MEMBER FUNCTIONS ========


/**
 * Drops the least significant bits of a colour's 8-bit channels in
 *        order to simulate the colour at a lesser bit depth.
 */
TRgb CSwtColor::ScaleColor(TInt aRedBits, TInt aGreenBits, TInt aBlueBits) const
{
    return ScaleColor(iOriginalColor, aRedBits, aGreenBits, aBlueBits);
}


TRgb CSwtColor::ScaleColor(const TRgb& aOriginalColor,
                           const TInt& aRedBits,
                           const TInt& aGreenBits,
                           const TInt& aBlueBits)
{
    ASSERT(aRedBits>0);
    ASSERT(aGreenBits>0);
    ASSERT(aBlueBits>0);

    TUint scaledRed   = ScaleChannel(aOriginalColor.Red(),   aRedBits);
    TUint scaledGreen = ScaleChannel(aOriginalColor.Green(), aGreenBits);
    TUint scaledBlue  = ScaleChannel(aOriginalColor.Blue(),  aBlueBits);
    return TRgb(scaledRed, scaledGreen, scaledBlue);
}


/**
 * Computes the gray equivalent to a colour.
 *
 * The gray is computed using the following luminance formula:
 * L = 30% red + 59% green + 11% blue
 */
TRgb CSwtColor::Gray(TInt aBitsInChannel) const
{
    return Gray(iOriginalColor, aBitsInChannel);
}

TRgb CSwtColor::Gray(const TRgb& aOriginalColor, const TInt& aBitsInChannel)
{
    // For faster computation we divide by 256, proportions are scaled accordingly.
    TUint luminance = (aOriginalColor.Red()*77 + aOriginalColor.Green()*151 + aOriginalColor.Blue()*28) / 256;
    luminance = ScaleChannel(luminance, aBitsInChannel);
    return TRgb(luminance, luminance, luminance);
}


/**
 * Returns the 8-bit value a colour channel would have if it were scaled
 *        down to some lesser bit resolution and then back up to 8 bits.
 *
 * For example, the 8-bit value 243 (11110011 binary) when scaled down to
 * 5 bits becomes 30 (11110 binary). Now if we scaled up the latter value to
 * 8 bits, it yelds 246.
 *
 * The operation is not about simply dropping the last bits (if we had done so,
 * the final result would have been 240).The purpose of this method is to
 * simulate lesser resolutions, not just to decrease resolution.
 *
 * @param aValue         The original value of the colour channel, no more than 255.
 * @param aBitsInChannel The number of bits to simulate, no more than 8.
 */
inline TUint CSwtColor::ScaleChannel(TUint aValue, TInt aBitsInChannel)
{
    ASSERT(aValue<=255u);
    ASSERT(aBitsInChannel<=8);
    return ((aValue>>(8-aBitsInChannel))*255u) / ((1u<<aBitsInChannel)-1u);
}




//
// Virtual methods from MSwtColor
//


void CSwtColor::Dispose()
{
    delete this;
}


TRgb CSwtColor::RgbValue() const
{
    const CGraphicsDevice& graphDev = const_cast<MSwtDevice&>(iDevice).GraphicsDevice();
    return RgbValue(graphDev, iOriginalColor);
}


TRgb CSwtColor::RgbValue(const CGraphicsDevice& aGraphDev, const TRgb& aOriginalColor)
{
    // First try and map using a palette
    CPalette* palette = NULL;
    TInt error = aGraphDev.GetPalette(palette);
    if (!error && palette!=NULL)
    {
        return palette->NearestEntry(aOriginalColor);
    }
    // Now, map depending on the display mode
    switch (aGraphDev.DisplayMode())
    {
        // Gray modes
    case EGray2:    // 1 bit
        return Gray(aOriginalColor, 1);
    case EGray4:    // 2 bits
        return Gray(aOriginalColor, 2);
    case EGray16:   // 4 bits
        return Gray(aOriginalColor, 4);
    case EGray256:  // 8 bits
        return Gray(aOriginalColor, 8);

        // Direct colour modes
    case EColor64K: // 16 bits (5,6,5)
        return ScaleColor(aOriginalColor, 5, 6, 5);
    case EColor4K:  // 12 bits (4,4,4)
        return ScaleColor(aOriginalColor, 4, 4, 4);

        // 24 bits: leave as is
    case EColor16M:
        return aOriginalColor;

        // U32 bits: leave as is
    case EColor16MU:
        return aOriginalColor;

    case EColor16MA:
        return aOriginalColor;

    case EColor16MAP:
        return aOriginalColor;

        // Unhandled: leave as is
    default:
        __DEBUGGER(); // Not an error, but it's nonetheless worth being noted
        return aOriginalColor;
    }
}
