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


#include "swtwindowgc.h"


// ======== MEMBER FUNCTIONS ========


inline CSwtWindowGc::CSwtWindowGc(MSwtWindowDrawable& aDrawable, CWindowGc* aGc, TDestructor aDestructor,
                                  const TRgb& aPenCol, const TRgb& aBrushCol, const MSwtFont& aFont,
                                  const TRect& aBoundaryRect, const TRect& aGcScreenRect)
        : CSwtGcBase(aDrawable, aGc, aDestructor, aPenCol, aBrushCol, aFont, aBoundaryRect, aGcScreenRect.Size())
        , iOriginPosition(aGcScreenRect.iTl)
{
}


/**
 * Constructs a GC suitable for the window server's devices.
 *
 * @param aDrawable     The drawable that created <code>aGc</code>.
 * @param aGc           The native GC to use, ownership is transferred.
 * @param aDestructor   The function that will take care of destroying <code>iGc</code>.
 * @param aPenCol       The pen's colour.
 * @param aBrushCol     The brush's colour.
 * @param aFont         The font to use
 * @param aBoundaryRect The rectangle drawing is restricted to, in GC coordinates.
 * @param aGcScreenRect The GC's rectangle in screen coordinates
 */
CSwtWindowGc* CSwtWindowGc::NewL(MSwtWindowDrawable& aDrawable, CWindowGc* aGc, TDestructor aDestructor,
                                 const TRgb& aPenCol, const TRgb& aBrushCol, const MSwtFont& aFont,
                                 const TRect& aBoundaryRect, const TRect& aGcScreenRect)
{
    // We intentionally don't use new(ELeave) so as to allow cleanup of aGc
    CSwtWindowGc* self = new CSwtWindowGc(aDrawable, aGc, aDestructor, aPenCol, aBrushCol, aFont, aBoundaryRect, aGcScreenRect);
    if (self == NULL)
    {
        aDestructor(aGc);
        User::LeaveNoMemory();
    }
    return self;
}


//
// Virtual methods from MSwtGc
//

void CSwtWindowGc::CopyAreaL(MSwtImage& aImage, const TPoint& aPoint) const
{
    CFbsBitmap& bitmap = aImage.Bitmap();

    TRect rect((aPoint + iOriginPosition), bitmap.SizeInPixels());
    static_cast<CWsScreenDevice*>(iGc->Device())->CopyScreenToBitmap(&bitmap, rect);
    aImage.HandleUpdate();
}

#ifdef RD_JAVA_NGA_ENABLED
void CSwtWindowGc::CopyArea(const TRect& aSource, const TPoint& aDestination)
{
    TRect rect(aSource);
    rect.Move(iOriginPosition);
    CSwtGcBase::CopyArea(rect, aDestination);
}
#endif
