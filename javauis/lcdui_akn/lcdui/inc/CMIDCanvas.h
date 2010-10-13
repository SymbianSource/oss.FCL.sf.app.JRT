/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMIDCanvas - Implements the native part of the Canvas class.
*
*/



#ifndef CMIDCANVAS_H
#define CMIDCANVAS_H

/**
 * Enables platform support for accelerated GDI operations.
 *
 * Used for creating of hardware bitmap (e.g. in LcdFbsImage.cpp) which can be
 * used in hardware graphics accelerator if available.
 *
 * FIX for BITGDI failing to transparently support drawing to hardware
 * bitmaps. Certain accelerator operations fail silently rather than falling
 * back to software routines. If the licencee can be sure that their hardware
 * accelerator supports all BITGDI operations then it may be worth
 * uncommenting the following line.
 */
#define ACCELERATED_BITGDI 1

/**
 * Enables deffered BitBlting. With this flag on, CMIDCanvas counts BitBlt
 * cycles and tries to optimize BitBlt process.
 */
#define DEFER_BITBLT 1

/**
 * Enables update invalidate method of drawing. Use RWindow::BeginRedraw and
 * RWindow::EndRedraw to tell the window server when the window is about to
 * respond to the last redraw event by redrawing.
 */
#define CANVAS_UPDATE_INVALIDATE 1

/**
 * Used double buffer for Canvas. Enabled by MIDP 2.1 specification.
 *
 * Define to switch on native double buffering in canvas. Canvas may still be
 * double buffered in java even if this is switched off, however this has not
 * been tested without JSR_135_SUPPORT.
 */
#define CANVAS_DOUBLE_BUFFER 1

/**
 * Enable direct screen access for Canvas when not using double buffering.
 * Define to switch on direct access in canvas. Cannot be used without
 * CANVAS_DOUBLE_BUFFER.
 */
#ifndef RD_JAVA_NGA_ENABLED
#define CANVAS_DIRECT_ACCESS 1
#endif // !RD_JAVA_NGA_ENABLED


/**
 * If canvas is using direct access itself, it must provide a native
 * framebuffer.
 */
#ifdef CANVAS_DIRECT_ACCESS
#ifndef CANVAS_DOUBLE_BUFFER
#define CANVAS_DOUBLE_BUFFER
#endif
#endif

/**
 * Without a native framebuffer we cannot handle redraw events natively, but
 * will have to generate a java repaint.
 */
#ifndef CANVAS_DOUBLE_BUFFER
#define CANVAS_ASYNC_REPAINT
#endif


#include <w32std.h>
#include <gulgcmap.h>
#include <coemain.h>
#include <coecntrl.h>

#ifdef RD_JAVA_NGA_ENABLED
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <platform/mw/alf/alfcompositionclient.h>
#include "monitor.h"
#endif // RD_JAVA_NGA_ENABLED


#include <lcdui.h>
#include <reflcdui.h>
#include <aknpointereventsuppressor.h>
#include <MMIDCustomComponentContainer.h>


/** MMIDMediaKeysListener in declaration. */
#include "MMIDMediaKeysListener.h"

/** Used CMIDDisplayable as member iDisplayable. */
#include "CMIDDisplayable.h"


class CMIDKeyDecoder;
class CMIDScaler;
class CMIDCallIndicator;
class CMIDNetworkIndicator;
class CMIDRemConObserver;
class CMIDTactileFeedbackExtension;


/**
 * Drawable canvas control.
 * Can be used as drawing surface of MIDP Canvas.
 * Key events, game actions, pointer events and size change events are
 * generated in the CMIDCanvas class. Whenever a key is pressed, that key is
 * translated and an event is posted to the Java side.
 *
 * @lib    lcdui.dll
 * @since  S60 v3.0
 */
NONSHARABLE_CLASS(CMIDCanvas) :
        public CCoeControl,
        public MMIDCanvas,
        public MMIDBufferProcessor,
        public MDirectContainer,
        public MMIDLcduiEventConsumer,
#ifdef CANVAS_DIRECT_ACCESS
        public MDirectScreenAccess,
#endif
        public MMIDMediaKeysListener,
#ifdef RD_TACTILE_FEEDBACK
        public MMIDTactileFeedbackComponent,
#endif
#ifdef RD_JAVA_NGA_ENABLED
        public MAlfBufferProvider,
#endif // RD_JAVA_NGA_ENABLED 
        public MMIDCustomComponentContainer
{
    /** CMIDNetworkIndicator is using protected function PostEvent. */
    friend class CMIDNetworkIndicator;

public:

    /**
     * Two-phased constructor.
     *
     * @param  aEnv Environment object.
     * @param  aComponentType The component type, can be ECanvas
     *         or EGameCanvas.
     * @param  aUpdateRequired Update is requied.
     */
    static CMIDCanvas* NewL(
        MMIDEnv& aEnv,
        CCoeControl& aWindow,
        MMIDComponent::TType aComponentType = MMIDComponent::ECanvas,
        TBool aUpdateRequired = EFalse);

    /**
     * Destructor.
     */
    virtual ~CMIDCanvas();

    /**
     * Set fullscreen mode on/off.
     *
     * @since  S60 v3.0
     * @param  aFullScreen The fullscreen mode state to be set.
     */
    void FullScreen(TBool aFullScreen);

    /**
     * Tells whether scaling is on or off.
     *
     * @since  S60 v3.0
     * @return ETrue if scaling is on, otherwise returns EFalse.
     */
    TBool IsScalingOn();

    /**
     * Tells whether application attribute
     * Nokia-MIDlet-S60-Selection-Key-Compatibility is set true or not.
     *
     * @since  S60 v5.0
     * @return ETrue if the attribute value is true, otherwise returns EFalse.
     */
    TBool SelectionKeyCompatibility();


// from base class CCoeControl

    /**
     * Counts the number of compound controls in this CCoeControl.
     *
     * @return The number of compound controls in this CCoeControl
     * @since S60 5.0
     */
    TInt CountComponentControls() const;

    /**
     * Returns the compound control in the specified index.
     *
     * @param aIndex The index of the control to be retrieved.
     * @return The control in the specified index or NULL if there
     *         is no control in that index.
     * @since S60 5.0
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * From CCoeControl.
     * Process a key event.
     *
     * @since  S60 v3.0
     * @param  aKeyEvent Key event.
     * @param  aType Event code.
     * @return Key response.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType);


// from base class MMIDCanvas

    /**
     * From MMIDComponent.
     * Invoked by the framework to clean up any resources used by the
     * implementation. Deletes this.
     *
     * @since  S60 v3.0
     */
    void Dispose();

    /**
     * From MMIDComponent.
     * Returns the component type as a MMIDComponent::TType enum.
     *
     * @since  S60 v3.0
     * @return Always returns MMIDComponent::ECanvas.
     */
    MMIDComponent::TType Type() const;

    /**
     * From MMIDComponent.
     * Returns buffer processor associated with this component.
     *
     * @since  S60 v3.0
     * @return Always returns this.
     */
    MMIDBufferProcessor* Processor();

    /**
     * From MMIDCancas.
     * Returns a reference to the CCoeControl corresponding to this canvas.
     *
     * @since  S60 v3.0
     * @return Reference to the CCoeControl corresponding to this canvas.
     */
    CCoeControl& Control();

    /**
     * From MMIDCancas.
     * Double buffering support.
     *
     * Returns the frame buffer.
     *
     * @since  S60 v3.0
     * @return A pointer to the frame buffer.
     */
    CFbsBitmap* FrameBuffer() const;

    /**
     * From MMIDCancas.
     * Returns the size of the canvas content.
     *
     * @since  S60 v3.0
     * @return The size of the displayable content.
     */
    TSize ContentSize() const;

    /**
     * From MMIDCancas.
     * Direct screen access support.
     *
     * Disables temporarily direct screen access.
     *
     * @since  S60 v3.0
     */
    void PauseDirectAccess();

    /**
     * From MMIDCancas.
     * Re-enables direct screen access.
     *
     * @since  S60 v3.0
     */
    void ResumeDirectAccess();

    /**
     * From MMIDCancas.
     * Switch off key event posting. Game keys will still be tracked, but
     * keyPressed()/keyReleased() callbacks will not be called.
     *
     * @since  S60 v3.0
     */
    void SuppressKeys();

    /**
     * From MMIDCancas.
     * Returns the latched state of the game action keys.
     *
     * @since  S60 v3.0
     * @return The latched state of the game action keys.
     */
    TUint32 GameActions();

    /**
     * From MMIDCancas.
     * Returns a direct container on which video can be rendered.
     *
     * @since  S60 v3.0
     * @return A container on which video can be rendered
     */
    MDirectContainer& DirectContainer();

    /**
     * From MMIDCancas.
     * Draw a background image.
     *
     * @since  S60 v3.2
     * @param  aGc Target graphical context
     * @param  aPosition Position
     * @param  aSize Size
     */
    void DrawBackground(
        CBitmapContext& aGc,
        const TPoint& aPosition,
        const TSize& aSize);

    /**
     * From MMIDCancas.
     * Returns the type of Canvas object.
     *
     * @since  S60 v3.2
     * @return ETrue if the type is
     *         MMIDComponent::EGameCanvas and EFalse
     *         when the type is MMIDComponent::ECanvas.
     */
    TBool IsGameCanvas() const;

    /**
     * From MMIDCancas.
     * Gets a network indicator location and size on fullscreen Canvas.
     *
     * @since  S60 v5.0
     * @param  aPosition Returned top left position of network indicator
     * @param  aSize Returned size of nw indicator
     * @return EFalse if Canvas is not able to display nw indicator (not
     *         fullscreen), otherwise ETrue.
     */
    TBool NetworkIndicatorLocation(TPoint& aPosition, TSize& aSize) const;


#ifdef RD_TACTILE_FEEDBACK
    /**
     * From MMIDCancas.
     * Get tactile feedback component.
     *
     * @since  S60 v5.0
     * @return this as tactile feedback component.
     */
    MMIDTactileFeedbackComponent* TactileFeedbackComponent();
#endif // RD_TACTILE_FEEDBACK

    TBool ReadyToBlit() const;

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
    TBool ProcessL(
        const TMIDBufferOp*& aRead,
        const TMIDBufferOp* aEnd,
        TInt& aCycles,
#ifdef RD_JAVA_NGA_ENABLED
        java::util::Monitor* aMonitor);
#else
        TRequestStatus* aStatus = NULL);
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


#ifdef CANVAS_DIRECT_ACCESS
// from base class MDirectScreenAccess

    /**
     * From MAbortDirectScreenAccess.
     * This function is called by the window server when direct screen access
     * must stop (for example because a dialogue is moved in front of the area
     * where direct screen access is taking place).
     *
     * @since  S60 v3.0
     * @param  aReason The reason why direct screen access was terminated.
     */
    void AbortNow(RDirectScreenAccess::TTerminationReasons aReasons);

    /**
     * From MDirectScreenAccess.
     * This function is called by the window server as soon as direct screen
     * access can resume.
     *
     * @since  S60 v3.0
     * @param  aReason Provides the reason why direct screen access was
     *         terminated.
     */
    void Restart(RDirectScreenAccess::TTerminationReasons aReasons);
#endif // CANVAS_DIRECT_ACCESS


// from base class MDirectContainer

    /**
     * From MDirectContainer.
     * Set the content of this direct container.
     *
     * @since  S60 v3.0
     * @param  aContent Content which is displayed on this container.
     */
    void MdcAddContent(MDirectContent* aContent);

    /**
     * From MDirectContainer.
     * Remove the content from this direct container. This will called when
     * the Player is closed.
     *
     * @since  S60 v3.0
     * @param  aContent Content which is displayed on this container.
     */
    void MdcRemoveContent(MDirectContent* aContent);

    /**
     * From MDirectContainer.
     * Add content bound to this direct container.
     * Container will not allow painting in this region.
     *
     * @since  S60 v3.0
      * @param  aRect Content bounds.
     */
    void MdcAddContentBounds(const TRect& aRect);

    /**
     * From MDirectContainer.
     * Removes one content bound from this direct container.
     *
     * @since  S60 v3.2
     * @param  aRect Content bound to be removed.
     */
    void MdcRemoveContentBounds(const TRect& aRect);

    /**
     * From MDirectContainer.
     * Get rect of a window on which content of this direct container can be
     * displayed.
     *
     * @since  S60 v3.2
     * @return Rect of a window.
     */
    TRect MdcContainerWindowRect() const;

    /**
     * From MDirectContainer.
     * Get the visiblity of this direct container.
     *
     * @since  S60 v3.0
     * @return ETrue if the container is visible, EFalse if it is
     * invisible.
     */
    TBool MdcContainerVisibility() const;

    /**
     * From MDirectContainer.
     * Get the bounds of the area that the content of this direct container
     * can occupy.
     *
     * @since  S60 v3.0
     * @return Maximum area that the content can occupy in screen
     *         co-ordinates.
     */
    TRect MdcContentBounds() const;

    /**
     * From MDirectContainer.
     * Get the area on the Item on which content can be displayed.
     *
     * @since  S60 v3.0
     * @param  aContentRect Return parameter for content area in Item
     *         co-ordinates.
     * @param  aScreenRect Return parameter for content area in screen
     *         co-ordinates.
     */
    void MdcItemContentRect(TRect& aContentRect, TRect& aScreenRect) const;

    /**
     * From MDirectContainer.
     * Allows a control created for direct container content display to pass
     * on pointer events.
     *
     * @since  S60 v3.0
     * @param  aPointerEvent The pointer event. iPosition will be reset to the
     *         parent window position. iParentPosition will be invalid for the
     *         container.
     */
    void MdcHandlePointerEventL(TPointerEvent& aPointerEvent);

    /**
     * From MDirectContainer.
     * Flush the direct container content.
     *
     * @since  S60 v3.2
     * @param  aContentRect The content area to be proceeded.
     */
    void MdcFlushContainer(const TRect& aRect);

    /**
     * From MDirectContainer.
     * Invokes callback aConsumer->MdcDSAResourcesCallback in LCDUI thread.
     * Used in case when MMAPI needs to start DSA within LCDUI thread.
     *
     * @since  S60 v5.0
     * @param "aConsumer" A consumer of callback
     */
    void MdcGetDSAResources(MUiEventConsumer& aConsumer);

    /**
     * From MDirectContainer.
     * Invokes callback aConsumer->MdcUICallback in LCDUI thread.
     * Used in case when MMAPI needs to run some operation
     * within LCDUI thread.
     *
     * @since  S60 v5.0
     * @param "aConsumer" A consumer of callback
     * @param "aCallbackId" A number identifying the callback
     */
    void MdcGetUICallback(MUiEventConsumer& aConsumer, TInt aCallbackId);

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * From MDirectContainer.
     * Notification about added content in LCDUI thread.
     * @since  S60 v9.2
     */
    void MdcNotifyContentAdded();
#endif

    /**
     * From MDirectContainer.
     *
     * @since  S60 v9.2
     * @param TBool aEnableFix
     */
    void MdcFixUIOrientation(TBool aEnableFix);

// From MMIDLcduiEventConsumer
public:
    void HandleLcduiEvent(int aType);

// from base class MMIDMediaKeysListener

    /**
     * From MMIDMediaKeysListener.
     * This method is called when a media key has been pressed.
     *
     * @since  S60 3.2
     * @param  TMIDKeyEvent The media key event that has occured.
     */
    void HandleMediaKeyEvent(TMIDKeyEvent& aKeyEvent);


#ifdef RD_TACTILE_FEEDBACK
// from base class MMIDTactileFeedbackComponent

    /**
     * From MMIDTactileFeedbackComponent.
     * Update tactile feedback areas.
     *
     * @since S60 v5.0
     */
    void UpdateTactileFeedback();

    /**
     * From MMIDTactileFeedbackComponent.
     * Register feedback area.
     *
     * @since  S60 v5.0
     * @param  aId The area Id.
     * @param  aRect The area rectangle.
     * @param  aStyle The area style.
     */
    void RegisterFeedbackArea(TInt aId, TRect aRect, TInt aStyle);

    /**
     * From MMIDTactileFeedbackComponent.
     * Unregister feedback area.
     *
     * @since  S60 v5.0
     * @param  aId The area Id.
     */
    void UnregisterFeedbackArea(TInt aId);

    /**
     * From MMIDTactileFeedbackComponent.
     * Unregister feedback for the control.
     *
     * @since  S60 v5.0
     */
    void UnregisterFeedbackForControl();

    /**
     * From MMIDTactileFeedbackComponent.
     * Move area to first priority.
     *
     * @since  S60 v5.0
     * @param  aId The area Id.
     */
    void MoveAreaToFirstPriority(TInt aId);
#endif // RD_TACTILE_FEEDBACK

    // From MMIDCustomComponentContainer

    /**
     * Registers a new custom component to this container.
     *
     * The ownership of the component is NOT transfferred. Nothing happens
     * if the component has already been registered. The component is added
     * on top of the stack in this container. Index can be changed later.
     *
     * @param aComponent The component to be registered.
     * @since S60 5.0
     */
    void RegisterComponentL(MMIDCustomComponent* aComponent);

    /**
     * Unregisters an existing custom component from this container.
     *
     * Nothing happens if the component is not found from this container.
     *
     * @param aComponent The component to be unregistered.
     * @since S60 5.0
     */
    void UnregisterComponent(MMIDCustomComponent* aComponent);

    /**
     * Changes the index of the specified custom component.
     *
     * @param aComponent The component which index will be changed
     * @param aNewIndex The new index of the component.
     * @since S60 5.0
     */
    void SetComponentIndexL(
        MMIDCustomComponent* aComponent,
        TInt aNewIndex);

    /**
     * Stores the index of focused component in parent.
     *
     * @param aComponent The component which gained focus
     * @since S60 5.0
     */
    void SetFocusedComponent(MMIDCustomComponent* aComponent);

    /**
     * Returns the index of the given component in this container.
     *
     * @return The index of the given component in this container. KErrNotFound
     *         is returned if this container does not have the specified component.
     * @since S60 5.0
     */
    TInt ComponentIndex(MMIDCustomComponent* aComponent) const;

    /**
     * Gets the full screen status of this component container.
     *
     * Typically the container is the same object which is returned when
     * calling <code>Control()</code>.
     *
     * @return <code>ETrue</code> if this component container is set to full screen
     *         and <code>EFalse</code> if not.
     * @since S60 5.0
     */
    TBool IsFullScreen() const;

    /**
    * Returns the drawing rectangle.
    *
    * @since  java 2.1
    * @return The rectangle where canvas draws.
    */
    inline const TRect ViewRect() const;

#ifdef RD_JAVA_S60_RELEASE_9_2
    /**
     * Gets a Displayable from the container.
     *
     * @return The Displayable from container.
     * @since S60 5.0
     */
    CMIDDisplayable* GetDisplayable() const;
#endif // RD_JAVA_S60_RELEASE_9_2

protected:

    /**
     * Post an event to Java side.
     *
     * @since  S60 v3.0
     * @param  aType Event type.
     * @param  aData0 Event data.
     * @param  aData1 Event data.
     * @return ETrue on success, otherwise returns EFalse .
     */
    TBool PostEvent(TEventType aType, TInt aData0, TInt aData1) const;

    /**
     * Update screen from frame buffer.
     *
     * @since  S60 v3.0
     * @param  aRect Source rectangle in frame buffer co-ordinates.
     */
#ifdef RD_JAVA_NGA_ENABLED
    void UpdateL(const TRect& aRect);
#else
    void Update(const TRect& aRect);
#endif

    /**
     * Prepare draw operation.
     *
     * @since  S60 v5.0
     * @param  aGc Graphics context for drawing.
     * @param  aWinRect Source rectangle in frame buffer co-ordinates.
     */
    inline void PrepareDraw(CGraphicsContext& aGc, TRect& aWinRect) const;

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Draws canvas content using
     *       - pixel source when canvas has plain 2D content.
     *       - EGL surface when M3G content has been drawn.
     * This method is not used if canvas has direct content.
     * In this case drawing is done via CCoeControl framework.
     *
     * @since  S60 v9.2
     * @param  aRect Rectangle to be drawn in frame buffer co-ordinates.
     */
    void DrawWindowNgaL(const TRect& aRect);
#endif

#ifdef CANVAS_DOUBLE_BUFFER
    /**
     * Draw through Window server using native frame buffer.
     *
     * @since  S60 v3.0
     * @param  aRect Source rectangle in frame buffer co-ordinates.
     */
    void DrawWindow(const TRect& aRect) const;
#endif // CANVAS_DOUBLE_BUFFER

#ifdef CANVAS_DIRECT_ACCESS
    /**
     * Start Direct Screen Access.
     *
     * @since  S60 v3.0
     */
    void StartDirectAccess();

    /**
     * Stop Direct Screen Access.
     *
     * @since  S60 v3.0
     */
    void StopDirectAccess();

    /**
     * Restart Direct Screen Access.
     *
     * @since  S60 v3.0
     */
    void RestartL();

    /**
     * Draw using Direct Screen Access.
     *
     * @since  S60 v3.0
     * @param  aRect Source rectangle in frame buffer co-ordinates.
     */
    void DrawDirect(const TRect& aRect) const;
#endif // CANVAS_DIRECT_ACCESS

// from base class CCoeControl

    /**
     * From CCoeControl.
     * Resource change handling.
     *
     * @since  S60 v5.0
     * @param  aType The message type code.
     */
    void HandleResourceChange(TInt aType);

    /**
     * Scale II (Pen and Touch) support to S60 release 5.0 builds.
     * Conditional compilation required by the implementation.
     */
#ifdef RD_SCALABLE_UI_V2
    /**
     * From CCoeControl.
     * Pointer events handling.
     *
     * @since  S60 v5.0
     * @param  aEvent The received pointer event.
     */
    virtual void HandlePointerEventL(const TPointerEvent& aEvent);
#endif // RD_SCALABLE_UI_V2

    /**
     * From CCoeControl.
     * Change of focus.
     *
     * @since  S60 v3.0
     * @param  aDrawNow The redraw of the control is or is not required
     *         immediatelly.
     */
    virtual void FocusChanged(TDrawNow aDrawNow);

    /**
     * From CCoeControl.
     * Change of size.
     *
     * @since  S60 v3.0
     */
    virtual void SizeChanged();

    /**
     * From CCoeControl.
     * Change of position.
     *
     * @since  S60 v3.0
     */
    virtual void PositionChanged();

    /**
     * From CCoeControl.
     * Drawing.
     *
     * @since  S60 v3.0
     * @param  aRect The clipping rectangle.
     */
    virtual void Draw(const TRect& aRect) const;


private:

#ifdef RD_SCALABLE_UI_V2
    /**
     * Pointer events handling in controls.
     *
     * @param  aPointerEvent The received pointer event on canvas.
     *
     * @return ETrue if event was consumed by any control, Efalse otherwise.
     * @since  S60 v5.0
     */
    TBool HandlePointerEventInControlsL(const TPointerEvent& aPointerEvent);

    /**
     * Pointer event pressed in the control.
     *
     * @param  aControl The control to be checked.
     * @param  apoint The received pointer event transformed position.
     *
     * @return ETrue if event is in the control, Efalse otherwise.
     * @since  S60 v5.0
     */
    TBool PointerEventInControl(MMIDCustomComponent* aControl,
                                TPoint aPoint);

#endif // RD_SCALABLE_UI_V2

    /**
     * Constructor.
     * @see CMIDCanvas::NewL()
     */
    CMIDCanvas(
        MMIDEnv& aEnv,
        MMIDComponent::TType aCanvasType,
        TBool aUpdateRequired);

    /**
     * Second phase construction.
     *
     * @since  S60 v3.0
     * @param  aWindow Container window.
     */
    void ConstructL(CCoeControl& aWindow);

    /**
     * Set the container window.
     *
     * @since  S60 v3.0
     * @param  aWindow Container window.
     */
    void SetContainerWindowL(const CCoeControl& aWindow);

    /**
     * Send key event to Java side.
     *
     * @since  S60 v3.2
     * @param  aEvent Key event.
     * @param  aType Event code.
     */
    void SendKeyEventToJavaSideL(const TKeyEvent& aEvent, TEventCode aType);


#ifdef CANVAS_DOUBLE_BUFFER
    /**
     * Create frame buffer.
     *
     * @since  S60 v3.0
     * @param  aSize The size of a frame buffer.
     */
    void CreateFrameBufferL(const TSize& aSize);
#endif // CANVAS_DOUBLE_BUFFER

#ifdef DEFER_BITBLT
    /**
     * Get BitBlt cycles count.
     *
     * @since  S60 v3.0
     * @param  aSize The size of a pixel region.
     * @return Cycles count.
     */
    TInt BitBltCycles(const TSize& aSize) const;
#endif // DEFER_BITBLT

    /**
     * Layout Canvas control.
     *
     * @since  S60 v3.0
     */
    void Layout();

#ifdef CANVAS_DIRECT_ACCESS
    /**
     * Tells whether Direct Screen Access is enabled or disabled.
     *
     * @since   S60 v3.0
     * @return  ETrue if DSA is enabled, otherwise returns EFalse.
     */
    inline TBool DirectEnabled() const;

    /**
     * Tells whether Direct Screen Access is paused or not.
     *
     * @since   S60 v3.0
     * @return  ETrue if DSA is paused, otherwise returns EFalse.
     */
    inline TBool DirectPaused() const;

    /**
     * Tells whether screen device update is required or not.
     *
     * @since   S60 v3.0
     * @return  ETrue if update is required, otherwise returns EFalse.
     */
    inline TBool UpdateRequired() const;
#endif // CANVAS_DIRECT_ACCESS

    /**
     * Tells whether posting of key events is enabled or disabled.
     *
     * @since   S60 v3.0
     * @return  ETrue if posting of key events is enabled, otherwise returns
     *          EFalse.
     */
    inline TBool PostKeyEvents() const;

    /**
     * Process game press action.
     *
     * @since  S60 v3.0
     * @param  aAction Game action.
     */
    void GamePress(TInt aAction);

    /**
     * Process game release action.
     *
     * @since  S60 v3.0
     * @param  aAction Game action.
     */
    void GameRelease(TInt aAction);

    /**
     * Sets MIDlet's original size and target size resolutions if those are
     * defined in JAD or manifest.  If attribute is missing or it's not
     * defined correctly, the value will be 0,0. This value is later used as
     * an "error value".
     *
     * @since  S60 v3.0
     */
    void SetScalingFactors();

    /**
     * Returns original MIDlet screen size with regards to display
     * orientation.
     *
     * When "MIDlet scaling on orientation switch" is allowed and Canvas
     * scaling is enabled then this function reflects display orientation
     * when accessing original MIDlet screen size. This means that it
     * eventually swaps original MIDlet size orientation if it does not
     * correspond to actual display orientation.
     *
     * @since   S60 v3.2
     * @return  Original MIDlet screen size with regards to display
     *          orientation.
     */
    TSize OrientedOrgMIDletScrSize() const;

    /**
     * Check if the network indicator should be shown in Canvas depending on
     * the midlet's domain:
     * - in manufacturer domain the indicator is not shown
     * - in operator domain a central reposotory key defines whether the
     *   indicator is shown
     * - in trusted/untrusted 3rd patry domain the indicator is shown
     *
     * @since   S60 3.2
     * @return  ETrue, if network indicator is shown in Canvas, otherwise
     *          returns EFalse.
     */
    TBool IsNetworkIndicatorEnabledL() const;
#ifdef RD_JAVA_NGA_ENABLED
    TBool IsDownScaling(const TSize& aSourceSize, const TRect& aDestRect,TBool aM3GContent) const;
#else
    TBool IsDownScaling(const TSize& aSourceSize, const TRect& aDestRect) const;
#endif //RD_JAVA_NGA_ENABLED
public:
    /**
     * Handles switching from foreground to
     * background and vice versa. Mainly rendering
     * context and surfaces need to be relased.
     */
    void HandleForeground(TBool aForeground);

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Motification about MIDlet's changed full or partial foreground status.
     * @param aFullOrPartialFg  ETrue, when full or partial foreground was gained.
     *                          EFalse, when foreground was fully lost.
     * @param aCurrentDisplayable   ETrue, if canvas is the current displayable
     */
    void HandleFullOrPartialForegroundL(TBool aFullOrPartialFg, TBool aCurrentDisplayable);

    /**
     * Frees all GPU memory that is reserved by Canvas.
     * @param aForced   If ETrue, memory is freed without checking window visibility
     *                  If EFalse, memory is freed if canvas window is invisible.
     */
    void FreeGraphicsMemory(TBool aForced);
#endif

private:
    /** States of the first paint */
    enum TFirstPainState {
        EFirstPaintNeverOccurred = 0,
        EFirstPaintInitiated,
        EFirstPaintPrepared,
        EFirstPaintOccurred
    };

#ifdef RD_JAVA_NGA_ENABLED

private:
    /** EGL surface types */
    typedef enum
    {
        EEglNone = 0,
        EEglWindow,
        EEglPbuffer
    }  TEglType;

// from MAlfBufferProvider
public:
    /**
     * Called by framework when the next buffer is ready and available for the client to draw.
     *
     * @since  S60 v9.2
     * @param aVisibleRegion is currently visible surface region
     * @return ETrue to continue drawing of frames. EFalse to pause drawing.
     *         After pausing drawing client needs to call CAlfCompositionPixelSource::ActivateL to
     *         continue drawing of frames
     */
    TBool ProduceNewFrameL(const TRegion& aVisibleRegion, TUint8*& aBuffer);

    /**
     * Called by framework when new buffer is been created.
     * Client should provide desired creation attributes.
     *
     * @since  S60 v9.2
     */
    TBufferCreationAttributes& BufferAttributes();

    /**
     * Notifications from Alf composition FW
     *
     * @since  S60 v9.2
     */
    void ContextAboutToSuspend();
    void OnActivation();

// From MMIDComponentNgaExtension
public:
    /**
     * @see MMIDComponentNgaExtension#UpdateEglContent()
     * @since S60 9.2
     */
    void UpdateEglContent();

    /**
     * @see MMIDComponentNgaExtension#IsEglAvailable()
     * @since S60 9.2
     */
    TBool IsEglAvailable() const;

    /**
     * @see MMIDComponentNgaExtension#BindEglSurface()
     * @since S60 9.2
     */
    EGLSurface BindEglSurface();

    /**
     * @see MMIDComponentNgaExtension#ReleaseEglSurface()
     * @since S60 9.2
     */
    void ReleaseEglSurface();

    /**
     * @see MMIDComponentNgaExtension#UpdateOffScreenBitmapL()
     * @since S60 9.2
     */
    virtual void UpdateOffScreenBitmapL(TBool aForced);

    /**
     * @see MMIDComponentNgaExtension#FillEglSurface()
     * @since S60 9.2
     */
    virtual TBool FillEglSurface(const TRect& aRect, const TRgb& aColor);

    /**
     * Clear whole OpenGL ES surface (color buffer).
     * @param aSurfaceType A surface to be cleared. If the value is
     * <code>EEglNone</code> then the defualt surface is used
     * (e.g. Pbuffer in case of scaling).
     * @param aRgba A clear color. If null then a default color will be used:
     * black in case of scaling and otherwise the clear color is white.
     * @return <code>ETrue</code> if succeeded.
     * @since S60 9.2
     */
    virtual TBool ClearEglSurface(
        TEglType aSurfaceType = EEglWindow, const TRgb* aRgba = NULL);

// From MMIDCanvas
public:
    /**
     * @see MMIDCanvas::UpdateRect(const TRect&)
     */
    void UpdateRect(const TRect& aRect);

    /**
     * @see MMIDCanvas::MidletExiting()
     */
    void MidletExiting();
private:
    /**
     * Blits pbuffer surface scaled to window surface
     * @since S60 9.2
     */
    void BlitPBufferScaledToWindowSurface();

    /**
     * Initializes pixel source
     * @since S60 9.2
     */
    void InitPixelSourceL();

    /**
    * Releases resources occupied by the pixel source
    * @since S60 9.2
    */
    void DisposePixelSource();

    /**
     * Creates an EGL pbuffer surface and context
     * for openGL based scaling
     * @since S60 9.2
     */
    void CreatePBufferSurfaceL();

    /**
     * Disposes pbuffer surface and context
     * created with CreatePBufferSurface()
     * @since S60 9.2
     */
    void DisposePBufferSurface();

    /*
     * Initializes EGL resources
     * @since S60 9.2
     */
    void OpenEglL();

    /**
     * Releases EGL resources
     * @param aReadback     Defines if surface content is copied to bitmap before
     *                      destroying surface
     * @since S60 9.2
     */
    void CloseEgl(TBool aReadback);

    /**
     * Creates EGL context
     * @since S60 9.2
     */
    void CreateEglContext();

    /**
     * Sets current EGL
     * @param aType The EGL type that is set active: Window, PBuffer or NULL.
     * @return ETrue if succeeded.
     * @since S60 9.2
     */
    TBool SetCurrentEglType(TEglType aType = EEglNone);

    /**
     * Gets current EGL
     * @since S60 9.2
     */
    TEglType GetCurrentEglType();

    /**
     * Reads EGL surface size
     * @param aSurface Egl surface. If the value is <code>EGL_NO_SURFACE</code>
     * then the defualt surface is used (e.g. Pbuffer in case of scaling).
     * @return Surface size. Returns TSize(0, 0) if failed.
     * @since S60 9.2
     */
    TSize GetEglSurfaceSize(EGLSurface aSurface = EGL_NO_SURFACE);

    /**
     * Handles size changed. EGL window surface needs to be recreated
     * if window size increases.
     * @param aOrientationChange    ETrue, method is called because of orientation change
     * @since S60 9.2
     */
    void HandleSizeChanged(TBool aOrientationChange);

    /**
     * Draws frame buffer 2D content as OpenGL texture in case of
     * M3G + 2D mixing.
     * @since S60 9.2
     */
    void BlitFrameBufferPixels();

    /**
     * Initializes the OpenGL ES content before 2d rendering
     * @param aTopLeftOrigo If false then the origo is viewport's
     * bottom left corner.
     * @param aRect Scissor rectangle. If NULL then the scissor
     * will be the same as the viewport rectangle.
     * @since S60 9.2
     */
    TSize InitOpenGl(TBool aTopLeftOrigo = EFalse, const TRect* aRect = NULL);

    /**
     * Draws a rectangular area of frame buffer as OpenGL texture.
     * aRect Part of frame buffer that is drawn.
     * @since S60 9.2
     */
    void BlitSubRect(const TRect& aRect);

    /**
     * Creates OpenGL texture out of aPixels and draws it to
     * EGL surface. Parameters define the texture location on EGL surface.
     * @since S60 9.2
     */
    void BlitSubRectTexture(
        TInt aXOffset, TInt aYOffset, TInt aWidth, TInt aHeight,
        TInt aStride, const TUint8* aPixels);
    /**
     * Clears the frame buffer with transparent color.
     * Clearing is needed in M3G + 2D mixing case in order
     * 2D content can be rendered as an OpenGL texture.
     * @since S60 9.2
     */
    void ClearFrameBuffer();

    /**
     * Pixel source is activated when canvas has a frame ready to be passed
     * to Alf composition.
     * @param aDrawingOngoing Flag if system drawing is on now.
     * @since S60 9.2
     */
    void ActivatePixelSourceL(TBool aDrawingOngoing);

    /**
     * Suspending pixel source releases its resources on graphics HW.
     * @since S60 9.2
     */
    void SuspendPixelSource();

    /**
     * Notifies monitor omwned by CMIDBuffer after async ProcessL()
     * is completed (i.e. in normal case when ProduceNewFrameL() is called).
     * @since S60 9.2
     */
    void NotifyMonitor();

    /**
     * Check for OpenGL errors.
     * @since S60 9.2
     */
    void AssertGL();

    /**
     * @since S60 9.2
     * Converts pixels from ARGB to RGBA.
     */
    inline void ConvertPixels(
        const TUint32* aSrc, TUint8* aDst, TInt aPixelCount);

    /**
     * EGL error
     * @since S60 9.2
     */
    const TDesC8& GetEglError(EGLint aErr);

    /**
     * Clears UI surface to transparent color
     * @since S60 9.2
     */
    void ClearUiSurface(TBool aDrawing);

    /**
     * Draws current canvas content to CWindowGc.
     * @since S60 9.2
     */
    void DrawToWindowGc();

    /**
     * Checks if canvas window is currently really visible on screen.
     * Used for checking when canvas should destroy its surfaces.
     * @since S60 9.2
     */
    TBool IsWindowVisible() const;

    /**
     * Posts forced paint event to Java side. Forced paint causes Canvas.paint() to be
     * called, event if Displayable.isShown() would return false in Java. See also Canvas.java.
     * @since S60 9.2
     */
    void PostForcedPaint();
#endif // RD_JAVA_NGA_ENABLED

private: // data

    /** CMIDCanvas flags datatype */
    enum TFlags
    {
        EDirectEnabled  = 1,
        EDirectPaused   = 2,
        EPostKeyEvents  = 4,
        EUpdateRequired = 8
    };

    /**
     * A MIDlet environment.
     */
    MMIDEnv& iEnv;

#ifdef CANVAS_DOUBLE_BUFFER
    /**
     * Frame buffer.
     * Not own.
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
#endif // CANVAS_DOUBLE_BUFFER

    /**
     * Stores information whether this control is the game canvas or not.
     */
    TBool iIsGameCanvas;

    /**
     * View rectangle.
     */
    TRect iViewRect;

    /**
     * Content size.
     */
    TSize iContentSize;

#ifdef CANVAS_DIRECT_ACCESS
    /**
     * An active object used to start direct screen access.
     * Own.
     */
    CDirectScreenAccess* iDirectAccess;

    /**
     * Direct screen access bitmapped graphics context.
     * Not own.
     */
    CFbsBitGc* iDirectGc;

    /**
     * Update region.
     */
    mutable RRegion iUpdateRegion;
#endif // CANVAS_DIRECT_ACCESS

    /**
     * Status flags.
     */
    TUint32 iFlags;

    /**
     * Game key state.
     */
    TUint32 iGameKeyState;

    /**
     * Game key latch.
     */
    TUint32 iGameKeyLatch;

    /**
     * Displayable.
     * Not own.
     */
    CMIDDisplayable* iDisplayable;

    /**
     * Fullscreen - status flag.
     */
    TBool iFullScreen;

    /**
     * Scaling on - status flag.
     */
    TBool iScalingOn;

    /**
     * Nokia-MIDlet-Original-Display-Size JAD attribute value.
     */
    TSize iOrgMIDletScrSize;

    /**
     * Nokia-MIDlet-Target-Display-Size JAD attribute value.
     */
    TSize iTargetMIDletScrSize;

    /**
     * Nokia-MIDlet-allow-scaling-on-orientation-switch JAD attribute value.
     */
    TBool iScaleMIDletOnOrientSwitch;

    /**
     * Nokia-MIDlet-S60-Selection-Key-Compatibility JAD attribute value.
     */
    TBool iS60SelectionKeyCompatibility;

    /**
     * Network indicator.
     * Own.
     */
    CMIDNetworkIndicator* iNetworkIndicator;

    /**
     * Call indicator.
     * Own.
     */
    CMIDCallIndicator* iCallIndicator;

    /**
     * Scaler.
     * Not own.
     */
    CMIDScaler* iScaler;

    /**
     * PointerEventSuppressor.
     * Own.
     */
    CAknPointerEventSuppressor* iPointerEventSuppressor;
    TPoint iPointerPressedCoordinates;
    TBool iPointerEventSuppressionOngoing;
    TInt iPESPointerMovementInTwips;
    TInt iPESTimeInMilliseconds;

    /**
     * For drag event filtering.
     */
    TTime iPreviousDragTimeStamp;

    /**
     * Direct content listeners.
     * Not own.
     */
    RPointerArray< MDirectContent > iDirectContents;

    /**
     * Restore content when unfaded - status flag.
     */
    TBool iRestoreContentWhenUnfaded;

    /**
     * Position in screen co-ordinates.
     * Stored in member variable because CCoeControl::PositionRelativeToScreen
     * cannot be called from another thread. Value is set in
     * SetContainerWindowL and in SizeChanged methods. Used in
     * MdcContentBounds method.
     */
    TPoint iPositionRelativeToScreen;

    /**
     * Key decoder.
     * Not own.
     */
    CMIDKeyDecoder* iKeyDecoder;

    /**
     * Remote connection observer.
     * Not own.
     */
    CMIDRemConObserver* iRemConObserver;

    /**
     * The last fade message.
     */
    TInt iLastFadeMessage;

#ifdef RD_TACTILE_FEEDBACK
    /**
     * To handle tactile feedback areas.
     * Own.
     */
    CMIDTactileFeedbackExtension* iTactileFeedback;
#endif // RD_TACTILE_FEEDBACK

    /**
     * Stores information about screen orientation, if screen
     * in landscape mode, variable iLandscape is True
     */
    mutable TBool iLandscape;

    /**
     * Stores information about screen need of screen refresh
     * after orientation change to avoid artifacting
     */
    mutable TBool iWndUpdate;

#ifdef CANVAS_DIRECT_ACCESS
    /**
     * Stores information that DirectContent
     * needs to be informed about DSA restart
     */
    TBool iDcDsaToStart;
#endif // CANVAS_DIRECT_ACCESS


    /**
     * Stores old bounds of the direct content
     * used before SizeChanged is called
     */
    TRect iOldContentBounds;

    // The list of registered custom components.
    RPointerArray< MMIDCustomComponent > iCustomComponents;

    // The count of custom components.
    TInt iCustomComponentsControlCount;

    // The index of focused custom component
    TInt iFocusedComponent;

    // Stores the control on which was press event generated
    MMIDCustomComponent* iPressedComponent;

    /**
     * Flag incdicating the foreground status of canvas.
     * Set to EFalse when some other displayable is made current
     * or when MIDlet goes to background.
     */
    TBool iForeground;

#ifdef RD_JAVA_S60_RELEASE_9_2
    // Indicates state of partial VKB
    TBool iPartialVKBOpen;
#endif

#ifdef RD_JAVA_NGA_ENABLED

    /**
     * CAlfCompositionPixelSource is used for drawing canvas
     * content when only basic content is drawn
     * (no M3G or direct content).
     * Own.
     */
    CAlfCompositionPixelSource* iAlfCompositionPixelSource;

    /**
     * Flag indicating if pixel source is in suspended state.
     * When pixel source is suspended, it releases its surface
     * on Graphics HW.
     */
    TBool iPixelSourceSuspended;

    /**
     * Buffer attributes define the size of ARGB data that is
     * passed to CAlfCompositionPixelSource.
     * Read by CAlfCompositionPixelSource in BufferAttributes().
     */
    TBufferCreationAttributes iAlfBufferAttributes;

    /**
     * Indicates if frame buffer contains a complete frame that
     * should be passed to Alf compostion.
     */
    mutable TBool iFrameReady;

    /**
     * Monitor that needs to be signalled after
     * ProduceNewFrameL callback is received.
     * Not own.
     */
    java::util::Monitor* iAlfMonitor;

    /**
     * Flag indicating whether device has 3D HW accelerator.
     */
    TBool i3DAccelerated;

    /**
     * Flag indicating whether midlet is drawing M3G content.
     * Reset to EFalse in CloseEgl().
     */
    TBool iM3GContent;
    /**
     * Previously Flag of iM3GContent
     */
    mutable TBool iPrevM3GContent;

    /**
     * ETrue, while M3G has bound to canvas EGL surface.
     */
    TBool iEglOccupied;

    /**
     * ETrue, when canvas is drawing the first frame
     * that has M3G content. Reset to EFalse in CloseEgl().
     */
    TBool iM3GStart;

    /**
     * Set to ETrue, if canvas is resized while M3G was bound to
     * EGL surface.
     */
    TBool iEglPendingResize;

    /**
     * Set to ETrue, if canvas canvas goes to background
     * (i.e. canvas needs to release resources on graphics HW)
     * while M3G was bound to EGL surface.
     */
    TBool iEglPendingDispose;

    /**
     * Handles to the EGL display, surface, context and config.
     */
    EGLDisplay iEglDisplay;
    EGLSurface iEglWindowSurface;
    EGLContext iEglWindowSurfaceContext;
    EGLConfig  iEglConfig;

    /**
     * Handles to the EGL pbuffer surface, context and texture in case of
     * OpenGL based scaling
     */
    EGLSurface iEglPbufferSurface;
    EGLContext iEglPbufferSurfaceContext;
    GLuint iPbufferTexture;

    /**
     * These rects define the framebuffer areas that
     * need to be blitted to EGL surface as OpenGL texture.
     * Canvas is devided vertically into 2 parts, but it is
     * a common case that iUpperUpdateRect covers the full canvas
     * (E.g. if fillRect() is used in MIDlet for full canvas area).
     * Updated by UpdateRects().
     */
    TRect iUpperUpdateRect;
    TRect iLowerUpdateRect;

    /**
     * Pointer to converted pixel data that is given to
     * OpenGL when creating the frame buffer texture.
     * Added the member variable because this is needed continuosly.
     * Own.
     */
    TUint8* iTexturePixels;

    /**
     * OpenGL coordinate arrays used fo r rendering filled rectangles
     */
    GLshort* iVertexArray;
    GLubyte* iTriangleStrip;

    /**
     * ETrue, when midlet is exiting and this canvas is the current displayable
     */
    TBool iExiting;
#endif // RD_JAVA_NGA_ENABLED

    /**
     * If dragging was started (by pointer down) inside the canvas,
     * all drag events and release event should go to canvas
     * even if it is outside of canvas.
     * All events, which started outside the canvas have to be ignored.
     */
    TBool iDragEventsStartedInside;

    /**
     * Switched after any graphics have been sent to screen.
     * Those graphics should be really drawn on the screen.
     */
    TInt iFirstPaintState;
};

#ifdef RD_JAVA_NGA_ENABLED
// ------------------------------------
// Converts pixels from ARGB to RGBA
// ------------------------------------
//
inline void CMIDCanvas::ConvertPixels(
    const TUint32* aSrc, TUint8* aDst, TInt aPixelCount)
{
    while (aPixelCount--)
    {
        TUint argb = *aSrc++;
        *aDst++ = (argb >> 16) & 0xFFu;
        *aDst++ = (argb >>  8) & 0xFFu;
        *aDst++ =  argb        & 0xFFu;
        *aDst++ = (argb >> 24) & 0xFFu;
    }
}

inline const TRect CMIDCanvas::ViewRect() const
{
    return iViewRect;
}
#endif // RD_JAVA_NGA_ENABLED

#endif // CMIDCANVAS_H
