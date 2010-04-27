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


#ifndef LCDWINDOWSURFACE_H
#define LCDWINDOWSURFACE_H

#include <w32std.h>
#include "LcdSurface.h"

typedef CFbsBitGc::TGraphicsOrientation TOrientation;

/**
 * Coordinate transform from logical device coords to
 * phyical device coords is determined by the scale,
 * origin and orientation.
 */
struct TCoordinateSystem
{
    TSize        iSize;
    TSize        iScale;
    TPoint       iOrigin;
    TOrientation iOrientation;
};

NONSHARABLE_CLASS(CLcdWindowSurface)
        : public CLcdSurface
        , private MDirectContent
        , private MDirectScreenAccess
{
public:
    /* We make the CONE dependency explicit here by requiring a CCoeControl for the window */
    static CLcdWindowSurface* NewL(CCoeControl& aWindow, MDirectContainer& aContainer, TBool aUpdate);
    ~CLcdWindowSurface();

    RWindow*     Window() const;
    void         GetCoordinateSystem(TCoordinateSystem& aCoords) const;

    virtual TSurfaceType Type() const;
    virtual void Update(const TRect& aRect);
    virtual void Begin(TAcceleratedBitmapInfo& aInfo, TBitmapLockCount& aCount);
    virtual void End(TBitmapLockCount& aCount);
    virtual TRect Bounds() const;
    virtual RRegion* VisibleRegion() const;
    virtual CBitmapContext* Context() const;
    virtual TImageType ImageType() const;

private:
    virtual void MdcContainerWindowRectChanged(const TRect& /*aRect*/) {};

    /**
    Notify content that container visiblity has changed. The content must not
    be displayed when it's container is invisible.
    @param "aVisible" ETrue if the container is visible, EFalse if it is invisible.
    */
    virtual void MdcContainerVisibilityChanged(TBool aVisible);

    /**
    Notify content that the container bounds have changed.
    @param "aRect" Maximum area the content can occupy in screen co-ordinates.
    The content should be clipped to this area.
    */
    virtual void MdcContentBoundsChanged(const TRect& aRect);

    /**
    Notify content that its display area has changed.
    This should only be called when the content is displayed on an Item.
    @param "aDisplayRect" Display area in Item co-ordinates.
    @param "aScreenRect" Display area in screen co-ordinates.
    */
    virtual void MdcItemContentRectChanged(const TRect& aContentRect,const TRect& aScreenRect);

    virtual void MdcContainerDestroyed();

    /**
     * Notify content that any drawing
     * via direct screen access must be aborted
     */
    virtual void MdcAbortDSA();

    /**
     * Allow the content to draw
     * via direct screen access after MdcAbortDSA
     */
    virtual void MdcResumeDSA();

private:
    virtual void AbortNow(RDirectScreenAccess::TTerminationReasons);
    virtual void Restart(RDirectScreenAccess::TTerminationReasons);

private:
    CLcdWindowSurface(MDirectContainer& aContainer, TBool aUpdate);
    void ConstructL(CCoeControl& aWindow);
    void ActivateL(RWindow* aWindow);
    void Deactivate();
    TBool Start();
    TInt DoUpdate();

private:
    MDirectContainer*   iContainer;
    RWsSession&         iWsSession;
    CWsScreenDevice&    iWsDevice;
    TBool               iUpdate;
    TImageType          iImageType;
    RWindow*            iWsWindow;
    CWindowGc*          iWsContext;     // backup context
    CDirectScreenAccess* iWsAccess;     // primary context, region and update device
    TAcceleratedBitmapInfo iScreen;
    TCoordinateSystem iScreenCoords;
    TBool iEnabled;
    TBool iStarted;
    TBool iVisible;
    TRect iBounds;
    RRegion iEmptyRegion;
    RRegion iUpdateRegion;
};

#endif // LCDWINDOWSURFACE_H
