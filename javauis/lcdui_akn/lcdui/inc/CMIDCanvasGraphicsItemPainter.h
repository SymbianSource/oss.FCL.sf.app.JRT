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
* Description:  CMIDCanvasGraphicsItem - Implements the native part Canvas Graphics Item Painter class.

*
*/


#ifndef CMIDCANVASGRAPHICSITEMPAINTER_H
#define CMIDCANVASGRAPHICSITEMPAINTER_H

// EXTERNAL INCLUDES
#include <e32base.h>
#include <MMIDCanvasGraphicsItemPainter.h>
#include <MMIDCustomComponent.h>

#include <reflcdui.h>
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CCoeControl;
class MMIDCustomComponentContainer;
class CMIDCanvasGraphicsItem;


// CLASS DESCRIPTION
/**
 * Defines the Canvas Graphics Item Painter native side component.
 */
NONSHARABLE_CLASS(CMIDCanvasGraphicsItemPainter) :
        public CCoeControl,
        public MMIDCanvasGraphicsItemPainter,
        public MMIDBufferProcessor,
        public MDirectContent
{
public: // Type definitions

    class TCtorParams
    {
    public:
        // The width of the graphics item in pixels.
        TInt iWidth;

        // The height of the graphics item in pixels or in rows.
        TInt iHeight;

        // Environment instance
        MMIDEnv* iEnv;

        TCtorParams::TCtorParams() :
                iWidth(0),
                iHeight(0),
                iEnv(NULL)
        {}
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @param aParams The construction parameters of this class.
     * @return New instance from this class
     */
    static CMIDCanvasGraphicsItemPainter* NewL(const TCtorParams& aParams);

    /**
     * Destructor.
     */
    virtual ~CMIDCanvasGraphicsItemPainter();

// from base class MMIDCanvasGraphicsItemPainter

    /**
     * From MMIDComponent
     * Invoked by the framework to clean up any  resources used by the
     * implementation.
     *
     * Typically, the implementation will do "delete this" but is clearly
     * not restricted to
     *
     * @since S60 5.0
     */
    void Dispose();

    /**
     * From MMIDComponent.
     * Returns buffer processor associated with this component.
     *
     * @since  S60 v3.0
     * @return Always returns this.
     */
    MMIDBufferProcessor* Processor();

    /**
     * From MMIDCanvasGraphicsItemPainter
     * Sets the position of this canvas graphics item component.
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    void SetPosition(const TInt aX, const TInt aY);

    /**
     * From MMIDCanvasGraphicsItemPainter
     * Sets the size of this graphics item component.
     *
     * @param aWidth The width value of the anchor point.
     * @param aHeight The height value of the anchor point.
     * @since S60 5.0
     */
    void SetItemSizeL(const TInt aWidth, const TInt aHeight);

    /**
     * From MMIDCanvasGraphicsItemPainter
     * Sets the direct container of this canvas graphics item component.
     *
     * Note that the canvas graphics item does not necessary need to have
     * direct container. If the parent component is responsible for somekind
     * of custom drawing using direct screena access, this method can be used
     * to register the direct container.
     *
     * The canvas graphics item adds itself to the direct container so that it
     * is not able to draw on top of the canvas graphics item when direct
     * content is added to the screen.
     *
     * @param aDirectContainer The direct container of this compoennt if
     *        any. <code>NULL</code> removes the current container.
     * @since S60 5.0
     */
    void SetDirectContainerL(MDirectContainer* aDirectContainer);

    /**
     * Propagates the parent to set window of this
     * canvas graphics item component.
     *
     * @param aContainer The parent MIDP custom component
     *        container.
     * @since S60 5.0
     */
    void SetParentL(MMIDCustomComponentContainer* aComponentContainer);

    /**
     * Sets this canvas graphics item component visible if it is hidden.
     *
     * Depending on the current status of the canvas graphics item, this
     * operation is no-op if there is nothing to do. (i.e. the canvas graphics
     * item is set hidden when it is already hidden).
     *
     * @param aVisible Indicates the visibility status of the canvas graphics
     *        item.
     * @since S60 5.0
     */
    void SetVisibleL(TBool aVisible);

    /**
     * Returns painter's frame buffer
     *
     * @since S60 5.0
     */
    CFbsBitmap* FrameBuffer() const;

    /**
     * Resets frame buffer bitmap before paint method is called.
     */
    void ResetFrameBuffer(const TPoint& aPosition, const TSize& aSize);

// from base class MMIDBufferProcessor

    /**
     * From MMIDBufferProcessor.
     * Process a block of commands, updating read pointer as you go.
     *
     * @since  S60 v3.0
     * @param  aRead The address of the first command in the buffer for this
     *         processor.
     * @param  aEnd The address of the command *after* the last command in the
     *         buffer for this processor.
     * @param  aCycles The amount of 'compute effort' remaining in the current
     *         AO RunL.
     * @param  aStatus The request status.
     * @return Returns ETrue if processing will be async.
     */
#ifdef RD_JAVA_NGA_ENABLED
    virtual TBool ProcessL(
        const TMIDBufferOp*& aRead, const TMIDBufferOp* aEnd,
        TInt& aCycles, java::util::Monitor* aMonitor);
#else
    virtual TBool ProcessL(
        const TMIDBufferOp*& aRead, const TMIDBufferOp* aEnd,
        TInt& aCycles, TRequestStatus* aStatus = NULL);
#endif

    /**
     * From MMIDBufferProcessor.
     * Called by framework if the processor is doing some async operation
     * when the MIDlet is closed down. Processor should cancel the async
     * operation and clean up any resources.
     *
     * @since  S60 v3.0
     */
    void AbortAsync();


// From basic class MDirectContent

    /**
     * Notify content that container window has changed.
     * This should only be called when the content is displayed on an
     * Item.
     *
     * @param aWindow New window on which content can be displayed or NULL
     *        if the Item has been removed from the Form
     * @since S60 5.0
     */
    void MdcContainerWindowRectChanged(const TRect& /*aRect*/);

    /**
     * Notify content that container visiblity has changed. The content
     * must not be displayed when it's container is invisible.
     *
     * @param aVisible ETrue if the container is visible, EFalse if it is
     *        invisible.
     * @since S60 5.0
     */
    void MdcContainerVisibilityChanged(TBool aVisible);

    /**
     * Notify content that the container bounds have changed.
     *
     * @param aRect Maximum area the content can occupy in screen
     *        co-ordinates. The content should be clipped to this area.
     * @since S60 5.0
     */
    void MdcContentBoundsChanged(const TRect& aRect);

    /**
     * Notify content that its display area has changed.
     * This should only be called when the content is displayed on an
     * Item.
     *
     * @param aDisplayRect Display area in Item co-ordinates.
     * @param aScreenRect Display area in screen co-ordinates.
     * @since S60 5.0
     */
    void MdcItemContentRectChanged(
        const TRect& aContentRect, const TRect& aScreenRect);

    /**
     * Notify content that container is about to be destoryed.
     *
     * @since S60 5.0
     */
    void MdcContainerDestroyed();

    /**
     * Aborts DSA.
     *
     * @since S60 5.0
     */
    void MdcAbortDSA();

    /**
     * Resumes DSA.
     *
     * @since S60 5.0
     */
    void MdcResumeDSA();

private: // Construtors

    /**
     * C++ constructor.
     */
    CMIDCanvasGraphicsItemPainter(MMIDEnv& aEnv);

    /**
     * Second phase constructor
     *
     * @param aParams The construction parameters of this class.
     * @since S60 5.0
     */
    void ConstructL(const TCtorParams& aParams);

    /**
     * Resets buffers of painter.
     *
     * @since S60 5.0
     */
    void ResetBuffers();

    /**
     * Creates buffers for painter.
     *
     * Creates the frame buffer with its associated device and context and the
     * offscreen buffer with its device and context. They are both of the same size,
     * the size of the content rect. The skin background is then printed on the
     * frame buffer via the frame context.
     *
     * @since S60 5.0
     */
    void CreateBuffersL();

    /**
     * Draws painter.
     *
     * If painter is visible, blits his frame buffer.
     *
     * @since S60 5.0
     */
    void Draw(const TRect& aRect) const;

private: // Data

    // Canvas graphics item control. Owned.
    CMIDCanvasGraphicsItem* iItem;

    // Container for the direct content. Not owned.
    MDirectContainer* iDirectContainer;

    /**
     * Frame buffer.
     * Own.
     */
    CFbsBitmap* iFrameBuffer;

    /**
     * Frame bitmapped graphics context.
     * Own.
     */
    CFbsBitGc* iFrameContext;

    /**
     * Frame bitmap graphics device.
     * Own.
     */
    CFbsBitmapDevice* iFrameDevice;

    /**
     * Canvas graphics item rect
     */
    TRect iContentRect;

    /**
     * Canvas graphics item painter inner visibility
     */
    TBool iVisible;

    // A MIDlet environment.
    MMIDEnv& iEnv;
};

#endif // CMIDCANVASGRAPHICSITEMPAINTER_H

// End of file
