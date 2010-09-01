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


#include "swtbitmapgc.h"


// ======== MEMBER FUNCTIONS ========


inline CSwtBitmapGc::CSwtBitmapGc(MSwtBitmapDrawable& aDrawable, CFbsBitGc* aGc, MSwtGc::TDestructor aDestructor,
                                  const TRgb& aPenCol, const TRgb& aBrushCol, const MSwtFont& aFont, const TSize& aSize)
        : CSwtGcBase(aDrawable, aGc, aDestructor, aPenCol, aBrushCol, aFont, TRect(aSize), aSize)
{
}


/**
 * Constructs a GC suitable for the font & bitmap server's devices.
 *
 * @param aDrawable   The drawable that created <code>aGc</code>.
 * @param aGc         The native GC to use, ownership is transferred.
 * @param aDestructor The function that will take care of destroying <code>iGc</code>.
 * @param aPenCol     The pen's colour.
 * @param aBrushCol   The brush's colour.
 * @param aFont       The font to use
 */
CSwtBitmapGc* CSwtBitmapGc::NewL(MSwtBitmapDrawable& aDrawable, CFbsBitGc* aGc, MSwtGc::TDestructor aDestructor,
                                 const TRgb& aPenCol, const TRgb& aBrushCol, const MSwtFont& aFont)
{
    // We intentionally don't use new(ELeave) so as to allow cleanup of aGc
    TSize size(aDrawable.GraphicsDevice().SizeInPixels());
    CSwtBitmapGc* self = new CSwtBitmapGc(aDrawable, aGc, aDestructor, aPenCol, aBrushCol, aFont, size);
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

void CSwtBitmapGc::CopyAreaL(MSwtImage& aImage, const TPoint& aPoint) const
{
    const CFbsBitGc* srceGc     = static_cast<CFbsBitGc*>(iGc);
    CFbsBitmap&      destBitmap = aImage.Bitmap();

    // Create a GC for the destination
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(&destBitmap);
    CleanupStack::PushL(device);
    CFbsBitGc* destGc = CFbsBitGc::NewL();
    destGc->Activate(device);

    // Copy from GC to GC
    destGc->BitBlt(TPoint(0,0), *srceGc, TRect(aPoint,destBitmap.SizeInPixels()));

    // Cleanup
    delete destGc;
    CleanupStack::PopAndDestroy(device);

    aImage.HandleUpdate();
}
