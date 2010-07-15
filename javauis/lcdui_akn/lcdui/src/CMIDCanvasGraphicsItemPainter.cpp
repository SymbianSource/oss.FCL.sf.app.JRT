/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements the native part of Canvas Graphics Item painter.
*
*/


// EXTERNAL INCLUDES
#include <MMIDCustomComponentContainer.h>
#include <lcdui.h>
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>
#include <j2me/jdebug.h>

// INTERNAL INCLUDES
#include "CMIDCanvasGraphicsItem.h"
#include "CMIDCanvasGraphicsItemPainter.h"

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::NewL
// ---------------------------------------------------------------------------
//
CMIDCanvasGraphicsItemPainter* CMIDCanvasGraphicsItemPainter::NewL(
    const TCtorParams& aParams)
{
    CMIDCanvasGraphicsItemPainter* self =
        new(ELeave) CMIDCanvasGraphicsItemPainter(*aParams.iEnv);

    CleanupStack::PushL(self);
    self->ConstructL(aParams);
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::ConstructL
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::ConstructL(
    const TCtorParams& aParams)
{
    iContentRect = TRect(0, 0, aParams.iWidth, aParams.iHeight);
    SetFocusing(EFalse);
    SetSize(TSize(aParams.iWidth, aParams.iHeight));
    CreateBuffersL();
    iVisible = EFalse;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::CMIDCanvasGraphicsItemPainter
// ---------------------------------------------------------------------------
//
CMIDCanvasGraphicsItemPainter::CMIDCanvasGraphicsItemPainter(MMIDEnv& aEnv) :
        iEnv(aEnv)
{
    // No implementation.
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::~CMIDCanvasGraphicsItemPainter
// ---------------------------------------------------------------------------
//
CMIDCanvasGraphicsItemPainter::~CMIDCanvasGraphicsItemPainter()
{
    DEBUG("CMIDCanvasGraphicsItemPainter::~CMIDCanvasGraphicsItemPainter +");

    // Release buffers
    ResetBuffers();

    // Remove association from direct container.
    if (iDirectContainer)
    {
        iDirectContainer->MdcRemoveContent(this);
    }

    iItem = NULL;

    DEBUG("CMIDCanvasGraphicsItemPainter::~CMIDCanvasGraphicsItemPainter -");
}

enum TOpCode
{
    ESync,
    ESyncRect
};

/** Copy the content (iFrameBuffer) into iOffScreenBuffer and then draw.
Either do a full copy or copy only the clipped area.
*/
#ifdef RD_JAVA_NGA_ENABLED
TBool CMIDCanvasGraphicsItemPainter::ProcessL(
    const TMIDBufferOp*& aOp, const TMIDBufferOp* aEnd,
    TInt& /*aCycles*/, java::util::Monitor* /*aMonitor*/)
#else
TBool CMIDCanvasGraphicsItemPainter::ProcessL(
    const TMIDBufferOp*& aOp, const TMIDBufferOp* aEnd,
    TInt& /*aCycles*/, TRequestStatus* /*aStatus*/)
#endif
{
    DEBUG("< CMIDCanvasGraphicsItemPainter::ProcessL");

    while (aOp < aEnd)
    {
        switch (aOp->OpCode())
        {
        case ESync:
            // We redraw the proper drawing rect.
            // The top-left point is set to PositionRelativeToScreen
            // in case that scaling is on.
            DrawNow(
                TRect(
                    iContentRect.Size() != iViewSize ?
                        PositionRelativeToScreen() : iPosition,
                    iViewSize));
            break;

        case ESyncRect:
            TRect clip = *static_cast<const TRect*>(aOp->Data());

            clip.Move(iContentRect.iTl);
            clip.Intersection(Rect());

            ActivateGc();
            Draw(clip);
            DeactivateGc();

            break;
        }
        aOp += aOp->Size();
    }

    DEBUG("> CMIDCanvasGraphicsItemPainter::ProcessL");
    return EFalse;
}

void CMIDCanvasGraphicsItemPainter::AbortAsync()
{
    // nope
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::SetDirectContainerL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::SetDirectContainerL(
    MDirectContainer* aDirectContainer)
{
    // Remove association from the previous direct container.
    if (iDirectContainer)
    {
        iDirectContainer->MdcRemoveContent(this);
    }

    // Associate this component to the new direct container. Note that this
    // needs to be done in order to disable the direct screen access of
    // the MID parent component. This way it does not draw on top of
    // the canvas graphics item when repaint etc. methods are called.
    if (aDirectContainer)
    {
        aDirectContainer->MdcAddContent(this);
    }

    // Store new container. Or NULL if passed.
    iDirectContainer = aDirectContainer;
}

void CMIDCanvasGraphicsItemPainter::SetParentL(MMIDCustomComponentContainer* aComponentContainer)
{
    CCoeControl& control = aComponentContainer->Control();
    SetContainerWindowL(control);
    ActivateL();
    if (iVisible)
    {
        // Paint the control
        iEnv.PostJavaEvent(*this, ECanvasGraphicsItemPainterEvent,
                           ECanvasGraphicsItemRepaint, 0);
    }
    else
    {
        MakeVisible(iVisible);
    }
}

void CMIDCanvasGraphicsItemPainter::SetVisibleL(TBool aVisible)
{
    if (iVisible != aVisible)
    {
        if (aVisible)
        {
            DEBUG("CMIDCanvasGraphicsItemPainter::SetVisibleL, setting painter visible");

            // Show the graphics item when it is hidden.
            MakeVisible(aVisible);
            iVisible = aVisible;
            // Paint the control
            iEnv.PostJavaEvent(*this, ECanvasGraphicsItemPainterEvent,
                               ECanvasGraphicsItemRepaint, 0);
        }
        else if (!aVisible)
        {
            DEBUG("CMIDCanvasGraphicsItemPainter::SetVisibleL, setting painter hidden");

            // Hide the graphics item when it is visible.
            MakeVisible(aVisible);
            iVisible = aVisible;
        }
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::SetPosition
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::SetPosition(const TInt aX, const TInt aY)
{
    DEBUG("CMIDCanvasGraphicsItemPainter::SetPosition +");

    //Previous position is moved to new location. New location
    //is relative to canvas point of origin (0,0).
    TInt moveX = aX - iContentRect.iTl.iX;
    TInt moveY = aY - iContentRect.iTl.iY;
    iContentRect.Move(moveX, moveY);

    SetRect(iContentRect);

    if (IsVisible())
    {
        iEnv.PostJavaEvent(*this, ECanvasGraphicsItemPainterEvent,
                           ECanvasGraphicsItemRepaint, 0);
    }

    DEBUG("CMIDCanvasGraphicsItemPainter::SetPosition -");
}


// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::SetItemSize
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::SetItemSizeL(
    const TInt aWidth,
    const TInt aHeight)
{
    DEBUG_INT2(
        "CMIDCanvasGraphicsItemPainter::SetItemSize +, aWidth=%d, aHeight=%d",
        aWidth,
        aHeight);

    if (iContentRect.Size().iWidth != aWidth ||
            iContentRect.Size().iHeight != aHeight)
    {
        iContentRect.SetSize(TSize(aWidth, aHeight));

        SetSize(iContentRect.Size());

        // Recreate buffers with new size
        CreateBuffersL();

        if (IsVisible())
        {
            iEnv.PostJavaEvent(*this, ECanvasGraphicsItemPainterEvent,
                               ESizeChanged, aWidth, aHeight, 0);
        }
    }

    DEBUG("CMIDCanvasGraphicsItemPainter::SetItemSize -");
}


// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::Dispose
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::Dispose()
{
    DEBUG("CMIDCanvasGraphicsItemPainter::Dispose +");

    delete this;

    DEBUG("CMIDCanvasGraphicsItemPainter::Dispose -");
}

// ---------------------------------------------------------------------------
// From class MMIDComponent.
// CMIDCanvas::Processor
// Always returns this-> as buffer processor associated with this component.
// ---------------------------------------------------------------------------
//
MMIDBufferProcessor* CMIDCanvasGraphicsItemPainter::Processor()
{
    return this;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::MdcContainerWindowRectChanged
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//

void CMIDCanvasGraphicsItemPainter::MdcContainerWindowRectChanged(
    const TRect& /*aRect*/)
{
    DEBUG("CMIDCanvasGraphicsItemPainter::MdcContainerWindowRectChanged +");

    // Not used at the moment.

    DEBUG("CMIDCanvasGraphicsItemPainter::MdcContainerWindowRectChanged -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::MdcContainerVisibilityChanged
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::MdcContainerVisibilityChanged(
    TBool /*aVisible*/)
{
    DEBUG("CMIDCanvasGraphicsItemPainter::MdcContainerVisibilityChanged +");

    // Not used at the moment.

    DEBUG("CMIDCanvasGraphicsItemPainter::MdcContainerVisibilityChanged -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::MdcContentBoundsChanged
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::MdcContentBoundsChanged(
    const TRect& /*aRect*/)
{
    DEBUG("CMIDCanvasGraphicsItemPainter::MdcContentBoundsChanged +");

    // Not used at the moment.

    DEBUG("CMIDCanvasGraphicsItemPainter::MdcContentBoundsChanged -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::MdcItemContentRectChanged
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::MdcItemContentRectChanged(
    const TRect& /*aContentRect*/,
    const TRect& /*aScreenRect*/)
{
    DEBUG("CMIDCanvasGraphicsItemPainter::MdcItemContentRectChanged +");

    // Not used at the moment.

    DEBUG("CMIDCanvasGraphicsItemPainter::MdcItemContentRectChanged -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::MdcContainerDestroyed
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::MdcContainerDestroyed()
{
    DEBUG("CMIDCanvasGraphicsItemPainter::MdcContainerDestroyed +");

    iDirectContainer = NULL;

    DEBUG("CMIDCanvasGraphicsItemPainter::MdcContainerDestroyed -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::MdcAbortDSA
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::MdcAbortDSA()
{
    DEBUG("CMIDCanvasGraphicsItemPainter::MdcAbortDSA +");

    // Not used at the moment.

    DEBUG("CMIDCanvasGraphicsItemPainter::MdcAbortDSA -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::MdcResumeDSA
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::MdcResumeDSA()
{
    DEBUG("CMIDCanvasGraphicsItemPainter::MdcResumeDSA +");

    // Not used at the moment.

    DEBUG("CMIDCanvasGraphicsItemPainter::MdcResumeDSA -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::CreateBuffersL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::CreateBuffersL()
{
    DEBUG("< CMIDCanvasGraphicsItemPainter::CreateBuffersL");
    ResetBuffers();

    TRect rect = TRect(
                     0,
                     0,
                     iContentRect.Size().iWidth,
                     iContentRect.Size().iHeight);

    iFrameBuffer = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iFrameBuffer->Create(rect.Size(), iEnv.DisplayMode()));

    iFrameDevice = CFbsBitmapDevice::NewL(iFrameBuffer);
    User::LeaveIfError(iFrameDevice->CreateContext(iFrameContext));

    DEBUG("> CMIDCanvasGraphicsItemPainter::CreateBuffersL");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::ResetBuffers
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::ResetBuffers()
{
    delete iFrameBuffer;
    delete iFrameContext;
    delete iFrameDevice;

    iFrameBuffer  = NULL;
    iFrameContext = NULL;
    iFrameDevice  = NULL;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::FrameBuffer
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
CFbsBitmap* CMIDCanvasGraphicsItemPainter::FrameBuffer() const
{
    return iFrameBuffer;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::ResetFrameBuffer
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::ResetFrameBuffer(
    const TPoint& aPosition, const TSize& aSize)
{
    if (iFrameBuffer)
    {
        //calculate stride: scanline should have the same width as frame buffer,
        //size of one pixel is defined by frame buffer display mode
        TInt stride = iFrameBuffer->ScanLineLength(
                          iFrameBuffer->SizeInPixels().iWidth, iFrameBuffer->DisplayMode());

        iFrameBuffer->LockHeap();

        for (TInt i = aPosition.iY; i < aSize.iHeight; i++)
        {
            const void *srcAddr =
                ((const char *) iFrameBuffer->DataAddress()) + i * stride;
            unsigned int *src = (unsigned int *) srcAddr;
            //starting adress should be shifted to clipping x-position
            src += aPosition.iX;
            for (TInt j = aPosition.iX; j < aSize.iWidth; j++)
            {
                //reset pixel to fully transparent
                *src++ = 0x00000000;
            }
        }

        iFrameBuffer->UnlockHeap();
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItemPainter::Draw
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItemPainter::Draw(const TRect& /*aRect*/) const
{
    DEBUG("+ CMIDCanvasGraphicsItemPainter::Draw");
    ASSERT(iFrameBuffer);

    // This verifies if canvas graphics item can be drawn.
    if (iItem && IsVisible())
    {
        // Gets a size of content.
        TSize nonScaledSize = iContentRect.Size();

        CWindowGc& gc = SystemGc();

        if (nonScaledSize == iViewSize)
        {
            // It draws non-scaled canvas graphics item.
            DEBUG("BitBlt - non-scaled - DrawBitmap");
            gc.BitBlt(iContentRect.iTl, iFrameBuffer);
        }
        else
        {
            // Scaling.

            // When scaling is on, move drawing position is needed.
            TRect windowRect = TRect(iViewSize);
            windowRect.Move(PositionRelativeToScreen());

            // It sets clip of system gc to current canvas size.
            gc.SetClippingRect(iOnScreenCanvasRect);

            // It draws scaled canvas graphics item.
            DEBUG("DrawWindow - Scaling - DrawBitmap");
            gc.DrawBitmap(windowRect, iFrameBuffer, TRect(nonScaledSize));

            // Cancel of clipping rect.
            gc.CancelClippingRect();
        }
    }

    DEBUG("- CMIDCanvasGraphicsItemPainter::Draw");
}

// End of file
