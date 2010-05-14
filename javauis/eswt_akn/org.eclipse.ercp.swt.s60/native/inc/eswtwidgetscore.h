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


#ifndef ESWTWIDGETSCORE_H
#define ESWTWIDGETSCORE_H


#include <eikmenup.h>
#include <eikmenub.h>
#include <eikmobs.h>
#include <eikspane.h>
#include "eswtgraphics.h"


class CEikScrollBar;
#ifdef RD_SCALABLE_UI_V2
class CAknStylusPopUpMenu;
#endif //RD_SCALABLE_UI_V2
class MSwtButton;
class MSwtCaptionedControl;
class MSwtCommand;
class MSwtComposite;
class MSwtControl;
class MSwtMenu;
class MSwtMenuItem;
class MSwtShell;
class MSwtMobileShell;
class MSwtDecorations;
class MAknsControlContext;
class MSwtBrowser;
class MSwtListBase;
class MSwtColor;
class CSwtFontData;
class CGraphicsContext;
class MSwtInput;
class MSwtScreen;
class MSwtMultiPagePager;
class MSwtMultiPageTabGroup;
class MSwtMultiPageDialog;
class MSwtDateEditor;
class MSwtConstrainedText;
class MSwtLink;
class MSwtListBox;
class MSwtListView;
class MSwtSortedList;
class MSwtTextExtension;
class MSwtClipboard;
class MSwtTableColumn;
class MSwtTableItem;
class MSwtTable;
class MSwtTree;
class MSwtFontDialog;
class MSwtDirectoryDialog;
class MSwtHyperLink;

static const TInt KSwtCommandBaseId = 0x1000;


/**
 * Interface class for the org.eclipse.swt.widgets.ScrollBar class
 */
class MSwtScrollBar
{
protected:
    virtual ~MSwtScrollBar() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Disposes of (i.e. destroys) the scroll-bar
     * @return The Java peer associated with this object
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the scroll-bar'size
     */
    virtual TSize GetSize() const =0;

    /**
     * Enables or disables the scrollbar
     */
    virtual void SetEnabled(TBool aEnabled) =0;

    /**
     * Enables or disables the scrollbar visibility
     */
    virtual void SetVisible(TBool aVisible) =0;

    /**
     * Returns the thumb's position
     */
    virtual TInt GetThumbPosition() const =0;

    /**
     * Sets the thumb's position
     * @param aPosition The thumb's new position, if the provided value is out
     *                  of range, it must be adjusted to fit.
     */
    virtual void SetThumbPositionL(TInt aPosition) =0;

    /**
     * Returns the thumb's span
     */
    virtual TInt GetThumbSpan() const =0;

    /**
     * Sets the thumb's span
     * @param aThumbSpan The thumb's new span, will be >= 1. If the provided value
     *                   is larger than (maximum-minimum) it must be ignored.
     */
    virtual void SetThumbSpanL(TInt aThumbSpan) =0;

    /**
     * Returns the minimum value
     */
    virtual TInt GetMinimum() const =0;

    /**
     * Sets the minimum value of the scroll span
     * @param aMinimum The new minimum, will be >= 0. If the provided value is
     *                 greater than or equal to the maximum, it must be ignored.
     */
    virtual void SetMinimumL(TInt aMinimum) =0;

    /**
     * Returns the maximum value
     */
    virtual TInt GetMaximum() const =0;

    /**
     * Sets the maximum value of the scroll span
     * @param aMaximum The new maximum, wil be >= 0. If the provided value is
     *                 less than or equal to the minimum, it must be ignored.
     */
    virtual void SetMaximumL(TInt aMaximum) =0;

    /**
     * Returns the increment
     */
    virtual TInt GetIncrement() const =0;

    /**
     * Sets the increment
     * @param aIncrement The new increment, will be >= 1.
     */
    virtual void SetIncrement(TInt aIncrement) =0;

    /**
     * Returns the page increment.
     */
    virtual TInt GetPageIncrement() const =0;

    /**
     * Sets the page increment
     * @param aPageIncrement The new page increment, will be >= 1.
     */
    virtual void SetPageIncrement(TInt aPageIncrement) =0;

    /**
     * Sets all parameters in once
     */
    virtual void SetValuesL(TInt aPosition, TInt aMinimum, TInt aMaximum,
                            TInt aThumbSpan, TInt aIncrement, TInt aPageIncrement) =0;
};


/**
 * Type for an array of controls
 */
typedef RPointerArray<MSwtControl> RSwtControlArray;


/**
 * Interface class for the org.eclipse.swt.widgets.Control class
 */
class MSwtControl : public MSwtWindowDrawable
{
protected:
    virtual ~MSwtControl() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Mask values for the utility flags
     * Flags are such that the default value is 0
     */
    enum TFlagMask
    {
        EFlagDoNotDraw    = 0x01, // The control must not be drawn
        EFlagInvisible    = 0x02, // The control is invisible
        EFlagDisabled     = 0x04, // The control is disabled (grayed, dimmed)
        EFreeFlagStart    = 0x08  // The first flag available for custom use
    };

    enum TFocusBackgroundPolicy
    {
        // Composite use case, no visual indication of focus
        ENoFocusBackground = 0,

        // Normal use case, draw focus background if the Control has focus
        // and there is at least another focusable Control in same Shell.
        EDefaultFocusBackground,

        // Button use case, the Control's focus background is owned by Avkon
        EEmbeddedFocusBackground,

        // Captioned Controls use case, default focus background policy
        // applies to itself and children
        ECaptionedCtrlFocusBackground
    };

public:
    /**
     * Disposes of (i.e. destroys) the control
     * @return The Java peer associated with this object
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the Java peer associated with this object
     */
    virtual TSwtPeer JavaPeer() const =0;

    /**
     * Returns a pointer to the underlying Cone control used for the
     * actual implementation.
     */
    virtual CCoeControl& CoeControl() =0;

    /**
     * Returns a const pointer to the underlying Cone control used for the
     * actual implementation.
     */
    virtual const CCoeControl& CoeControl() const =0;

    /**
     * Returns the control's utility flags
     * <b>Note</b>: this method is a convenience aimed to be used for the
     * implementation of MSwtComposite. If the implementation on a
     * given platform does use utility flags, this method can return any value.
     */
    virtual TUint Flags() const =0;

    /**
     * Indicate if the control has the flag EFlagDoNotDraw.
     * @return True if the Control has the EFlagDoNotDraw flag, else EFalse.
     */
    virtual TBool HasDoNotDrawFlag() const =0;

    /**
     * Instructs the control to paint itself
     *
     * @param aRect The rectangle to draw, in screen coordinates.
     * @param aPaintNatively Whether the native CCoeControl::Draw should be called before flushing
     *                       the buffered operations
     * @param aGc The GC that was passed into the paint listener
     * @param aIntParam An array of buffered opcodes and integer parameters from gc operations
     *                  done in the paint listener
     * @param aIntCount The Number of items in aIntParams array
     * @param aStrParam Buffered string parameters from gc operations done in the paint listener
     */
    virtual void Paint(const TRect& aRect,
                       TBool aPaintNatively = ETrue,
                       MSwtGc* aGc = NULL,
                       TInt* aIntParams = NULL,
                       TInt aIntCount = 0,
                       HBufC* aStrParams = NULL) const =0;

    /**
     * Emergency native paint. Ignores Java GCs.
     * Window redraw must be started before calling this and stopped after.
     *
     * @param aRect The rectangle to draw, in window coordinates.
     */
    virtual void PaintUrgently(const TRect& aRect, TBool aInitDraw = ETrue) const =0;

    /**
     * Ignoring the Java paint requests and draws directly for the time being.
     */
    virtual TBool IsPaintingUrgently() const = 0;

    /**
     * Processes a key event
     */
    virtual void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType) =0;

    /**
     * Handles the pointer events.
     * @param aPointerEvent pointer event to process
     */
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent)=0;

    /**
     * Determines if a control is suitable for receiving focus.
     * Invisible or disabled controls can never be focused, not even with
     * KSwtFocusByForce. Shells cannot be focused just activated.
     */
    virtual TBool IsFocusable(TInt aReason = KSwtFocusByApi) const =0;

    /**
     * Attempts to make this control the one that receives focus.
     * If this control or a descendant takes focus, true is returned. Composites
     * attempt to assign focus to their children before taking focus themselves.
     * Some controls, such as labels, do not normally take focus. A control will
     * not take focus if it is disabled or hidden or when input is blocked due to
     * modality.
     * In case of KSwtFocusByForce, forces the control to receive keyboard events.
     * Controls that do not normally accept keyboard input will take focus
     * using this KSwtFocusByForce.
     * @return Whether this control has focus upon method exit.
     */
    virtual TBool SetSwtFocus(TInt aReason = KSwtFocusByApi) =0;

    /**
     * Returns the policy of the focus background.
     * See TFocusBackgroundPolicy.
     */
    virtual TInt FocusBackgroundPolicy() const =0;

    /**
     * Returns true if the control is selectable by the user and is not dimmed
     */
    virtual TBool GetEnabled() const =0;

    /**
     * If this function returns true it does not mean that the control should
     * be visible for the user. The parent migh be invisible in which case this
     * control should not be visible to the user or draw itself. So this function
     * returns only the internal visibility state of this controll without paying
     * attention to the combined visibility of its ancestors.
     * Use GetVisibleRecursively() to determine if the control is really visible.
     */
    virtual TBool GetVisible() const =0;

    /**
     * Except Shells, controls are shown if themeselves and their parent are visible.
     */
    virtual TBool GetVisibleRecursively() const =0;

    /**
     * Indicate if the default system colour was use for the background.
     * @return Returns Etrue if the default system colour was use for the background,
     * otherwise EFalse if a color background setted.
     */
    virtual TBool IsDefaultBackgroundUse() const = 0;

    /**
     * Returns the control context of the Control.
     * @return Returns the control context of the Control define by the control
     * or NULL or there is no specific control context..
     */
    virtual MAknsControlContext* GetControlContext() const =0;

    /**
     * Returns the control background color
     * @return The control background color
     */
    virtual TRgb GetBackground() const = 0;

    /**
     * Returns the control foreground color
     * @return The control foreground color
     */
    virtual TRgb GetForeground() const = 0;

    /**
     * Sets the color of the control's background.
     * @param aColor The colour to set the background to, if NULL
     *               the default colour will be used.
     */
    virtual void SetBackgroundL(const MSwtColor* aColor) =0;

    /**
     * Sets the color of the control's text.
     * @param aColor The colour to set the foreground to, if NULL
     *               the default colour will be used.
     */
    virtual void SetForegroundL(const MSwtColor* aColor) =0;

    /**
     * Returns the control font used for writing text
     * @return The control's font, cannot be NULL
     */
    virtual const MSwtFont* GetFont() const =0;

    /**
     * Gets the control's border width
     * @return The control's border width
     */
    virtual TInt GetBorderWidth() const =0;

    /**
     * Returns the client rectangle (i.e. the part without scroll-bars, borders
     *        and other decorations) of this Control
     * The client area returned is in window coordinates.
     */
    virtual TRect ClientRect() const =0;

    /**
     * Gets the control's bounding rectangle
     * @return The control's coordinates relative to its parent, or in screen
     *         coordinates if it has no parent. Shells always return screen
     *         coordinates.
     */
    virtual TRect GetBounds() const =0;

    /**
     * Gets the control's position.
     * @return The control's position relative to its parent, or in screen
     *         coordinates if it has no parent. Shells always return screen
     *         coordinates.
     */
    virtual TPoint GetLocation() const =0;

    /**
     * Returns the control's popup menu
     */
    virtual const MSwtMenu* GetPopupMenu() const =0;

    /**
     * Returns the control's native menu
     */
    virtual const MSwtMenu* GetControlMenu() const =0;

    /**
     * Returns the control parent
     */
    virtual MSwtComposite* GetParent() const =0;

    /**
     * Check if aParent is in the parent hierarchy.
     */
    virtual TBool IsDescentOf(const MSwtControl& aParent) const =0;

    /**
     * Get the closest parent Shell of the Control
     */
    virtual MSwtShell& GetShell() const =0;

    /**
     * Get the top Shell of the Control
     */
    virtual MSwtShell& GetTopShell() const =0;

    /**
     * Get the top CaptionedControl of the Control, if any
     */
    virtual MSwtCaptionedControl* GetTopCaptionedControl() const =0;

    /**
     * Gets the control's width and height;
     */
    virtual TSize GetWidgetSize() const =0;

    /**
     * Returns true if we have UI focus, false otherwise
     */
    virtual TBool IsFocusControl() =0;

    /**
     * Moves this control above aControl in the drawing order
     */
    virtual void MoveAbove(const MSwtControl* aControl) =0;

    /**
     * Moves this control below aControl in the drawing order
     */
    virtual void MoveBelow(const MSwtControl* aControl) =0;

    /**
     * Redraws the entire control
     */
    virtual void Redraw() =0;

    /**
     * Updates DoNotDraw flag after a call to setRedraw
     */
    virtual void UpdateDoNotDrawFlag() =0;

    /**
     * Redraws the specified area of the control
     * @param aRect The rectangle to redraw, in client area coordinates.
     */
    virtual void RedrawRegion(const TRect& aRect) =0;

    /**
     * Enables or disables future redraw requests
     */
    virtual void SetRedraw(TBool aRedraw) =0;

    /**
     * Sets the font of the control.
     * @param aFont The font to use, if NULL a default font is to be used.
     */
    virtual void SetFontL(const MSwtFont* aFont) =0;

    /**
     * Sets the visibility status of the control
     */
    virtual void SetVisible(TBool aVisible) =0;

    /**
     * Sets the Control's location
     * @param aPoint The new location; in the parent's coordinates, in screen
     *        coordinates for Shells.
     */
    virtual void SetLocation(const TPoint& aPoint) =0;

    /**
     * Sets the Control's size
     *
     * @param aSize The new size.
     */
    virtual void SetWidgetSize(const TSize& aSize) =0;

    /**
     * Compute the preferred size.
     */
    virtual TSize ComputeSizeL(TInt aWHint, TInt aHHint) = 0;

    /**
     * Sets the control as selectable by the user
     */
    virtual void SetEnabled(TBool aEnabled) =0;

    /**
     * Sets the Control's location and size
     * @param aRect The new rectangle; in the parent's coordinates, in screen
     *        coordinates for Shells.
     */
    virtual void SetBounds(const TRect& aRect) =0;

    /**
     * Sets the Control's popup menu
     * @param aMenu The popup menu.
     */
    virtual void SetPopupMenuL(const MSwtMenu* aMenu) =0;

    /**
     * Enable or disable pointer capture for this control
     */
    virtual void SetCapture(TBool aCapture) =0;

    /**
     * Returns the style of the control.
     */
    virtual TInt Style() const =0;

    /**
     * Updates the control by forcing an immediate draw
     * on itself and flushes all paint events on this Control.
     */
    virtual void Update() const =0;

    /**
     * Returns a pointer to the MSwtButton interface of this
     * Control if it is a Button.
     * Only actual Button will return a non-null pointer
     * @return The Control's MSwtButton interface, or NULL
     */
    virtual MSwtButton* ButtonInterface()
    {
        return NULL;
    }

    /**
     * Returns a pointer to the MSwtCaptionedControl interface of this
     * Control if it is a CaptionedControl.
     * Only actual CaptionedControl will return a non-null pointer
     * @return The Control's MSwtCaptionedControl interface, or NULL
     */
    virtual MSwtCaptionedControl* CaptionedControlInterface() const
    {
        return NULL;
    }

    /**
     * Returns a pointer to the MSwtShell interface of this Control.
     * Only actual Shells will return a non-null pointer
     * @return The Control's MSwtShell interface, or NULL
     */
    virtual MSwtShell* ShellInterface()
    {
        return NULL;
    }
    virtual const MSwtShell* ShellInterface() const
    {
        return NULL;
    }

    /**
     * Tests if this control is a shell
     */
    inline TBool IsShell() const
    {
        return (const_cast<MSwtControl*>(this)->ShellInterface()!=NULL);
    }

    /**
     * Returns a pointer to the MSwtDecorations interface of this Control.
     * Only actual Decorations (or Shells) will return a non-null pointer
     * @return The Control's MSwtDecorations interface, or NULL
     */
    virtual MSwtDecorations* DecorationsInterface()
    {
        return NULL;
    }

    /**
     * Tests if this control is a decorations
     */
    inline TBool IsDecorations() const
    {
        return (const_cast<MSwtControl*>(this)->DecorationsInterface()!=NULL);
    }

    /**
     * Returns a pointer to the MSwtComposite interface of this Control.
     * Only controls actually implementing MSwtComposite will return a non-null pointer
     * @return The Control's MSwtComposite interface, or NULL
     */
    virtual MSwtComposite* CompositeInterface() const
    {
        return NULL;
    }

    /**
     * Returns a pointer to the MSwtBrowser interface of this Control.
     * Only actual Browser will return a non-null pointer
     * @return The Control's MSwtBrowser interface, or NULL
     */
    virtual MSwtBrowser* BrowserInterface()
    {
        return NULL;
    }

    /**
     * Recursively adds to the provided list all the controls that can
     * be traversed using the "Tab" key.
     * Controls must be appended to the list in the same order as specified in
     * the control's own tab list. All controls are to be put in the list, even
     * non-focusable ones.
     * @param aList The list to add the controls to
     */
    virtual void ComputeTabListL(RSwtControlArray& aList) =0;

    /**
     * Converts screen coordinates to control coordinates (client area coordinates).
     * @return Control relative point
     */
    virtual TPoint ToControl(const TPoint& aPoint) const =0;

    /**
     * Converts control coordinates (client area coordinates) to screen coordinates.
     * @return Screen relative point
     */
    virtual TPoint ToDisplay(const TPoint& aPoint) const =0;

    /**
     * Indicates if control uses CBA button or if CBA button is available to user commands.
     * @param aCbaButtonIndex Index of the CBA button
     * @return ETrue if control uses CBA button, EFalse otherwise
     */
    virtual TBool CbaButtonIsUsed(TInt aCbaButtonIndex) const =0;

    /**
     * Indicates if control uses the key passed in argument.
     * @param aKeyCode Key code
     * @return ETrue if control uses the key, EFalse otherwise
     */
    virtual TBool IsKeyUsed(TUint aKeyCode) const =0;

    /**
     * Returns the MSK label to be displayed. This is used in correlation
     * with the IsKeyUsed to determine the label for the MSK.
     * @return descriptor to be displayed.
     */
    virtual HBufC* MSKLabelL() const=0;

    /**
     * Indicates if external selection command can replace
     * internal selection operation when pressing MSK.
     * @return ETrue if external selection command is allowed, EFalse otherwise
     */
    virtual TBool MSKSelCmdEnabled() const = 0;

    /**
     * Posts an asynchronous traverse event without taking into account any key event.
     * @param aDetail   The traversal detail
     * @param aDoIt     Initial value for the doit field
     * @return ETrue if the post is succesfull.
     */
    virtual TBool ForceTraverseEvent(TSwtTraversal aDetail, TBool aDoIt) =0;

    /**
     * Add this control to the Control Stack of the Eikon AppUi
     * @param aPriority The control’s stack priority.
     */
    virtual void AddToControlStackL(TInt aPriority) =0;

    /**
     * Remove this control from the Control Stack of the Eikon AppUi
     * @param aPriority The control’s stack priority.
     */
    virtual void RemoveFromControlStack()  =0;

    /**
     * Says if this control is on Control Stack of the Eikon AppUi
     * @return ETrue if control is on Control Stack of the Eikon AppUi.
     */
    virtual TBool IsOnControlStack() const =0;

    /**
     * Says if the control is allowed to draw outside its own extent.
     * @return ETrue if control is allowed to draw outside its own extent.
     */
    virtual TBool CanDrawOutsideRect() const
    {
        return EFalse;
    }

#ifdef RD_SCALABLE_UI_V2
    /**
     * Returns the control's native menu for stylus popup menu. It may contain
     * different commands than the control menu opened with MSK.
     */
    virtual const MSwtMenu* GetStylusPopupControlMenu() const
    {
        return NULL;
    }
#endif //RD_SCALABLE_UI_V2

    /**
     * Destructor wrapper. Use cautiously.
     */
    virtual void Destroy()
    {
        delete this;
    };

    /**
     * Hide / send to background the CoeControl directly without notifications.
     * For instance a Shell can be hidden without changing its focusability.
     * Does not redraw.
     */
    virtual void HideSilently() = 0;
    virtual void ShowSilently() = 0;
    virtual TBool HiddenSilently() const = 0;

    /**
     * Allows the Control to decide which pointer events are good for long tap animation.
     */
    virtual TBool IsLongTapAnimationCandidate(const TPointerEvent& aPointerEvent) const =0;

    virtual void DoControlSpecificFeedback(const TBool& aFirstTap,
                                           const TBool& aTappedToChildRect,
                                           const TPointerEvent& aPointerEvent) const = 0;

    virtual void PostMouseEventL(const TPointerEvent& aPointerEvent) = 0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Scrollable class
 * A class that implements MSwtScrollable must also implement MSwtControl.
 */
class MSwtScrollable
{
protected:
    virtual ~MSwtScrollable() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtControl interface.
     */
    virtual MSwtControl* Control() =0;

    /**
     * Computes the size needed to obtain the specified client area
     * @param aRect Must contain the client area to be obtained, this will be
     *        modified to be the needed area.
     */
    virtual void ComputeTrim(TRect& aRect) const =0;

    /**
     * Creates a ScrollBar object that represents this Scrollable's scroll-bar.
     * @param aPeer  The Java peer of the scroll-bar to be created
     * @param aStyle Can be KSwtHScroll or KSwtVScroll.
     */
    virtual MSwtScrollBar* CreateScrollBarL(TSwtPeer aPeer, TInt aStyle) =0;

    /**
     * Hides the specified scroll-bar
     * @param aStyle   The scroll-bar whose visibility to alter, must be one of
     *                 KSwtStyleHScroll and KSwtStyleVScroll.
     * @param aVisible Whether to make the specified scroll-bar visible or invisible
     */
    virtual void SetVisibleScrollBar(TInt aStyle, TBool aVisible) =0;

    /**
     * Notifies this Scrollable that one of its scroll-bars has been destroyed.
     */
    virtual void ScrollBarDestroyed(const MSwtScrollBar* aScrollBar) =0;

    /**
     * Tells if control has scrollbars
     */
    virtual TBool HasScrollBars() const
    {
        return EFalse;
    };
};


/**
 * Interface class for the org.eclipse.swt.widgets.Composite class
 * A class that implements MSwtComposite must also implement MSwtScrollable.
 */
class MSwtComposite
{
protected:
    virtual ~MSwtComposite() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtScrollable interface
     */
    virtual MSwtScrollable* Scrollable() =0;

    /**
     * Returns the MSwtControl interface
     */
    inline MSwtControl* Control()
    {
        return Scrollable()->Control();
    }

    /**
     * Registers a new child with this Composite
     * A Control may register itself only once it has been
     * <b>fully</b> constructed. Additionally, a Control shall
     * register only once with its parent.
     * @param aChild The child control to register, cannot be a Shell.
     * @sa MSwtShell::RegisterChildShellL()
     */
    virtual void RegisterChildL(MSwtControl& aChild) =0;

    /**
     * Unregisters a child from this Composite
     * A Control shall not attempt to unregister itself from a
     * parent it was not registered with.
     * @param aChild The child control to unregister, cannot be a Shell
     * @sa MSwtShell::UnregisterChildShell()
     */
    virtual void UnregisterChild(MSwtControl& aChild) =0;

    /**
     * Retrieves the Java peer of each child.
     * @return A newly allocated array containing the peers; the caller is
     *         responsible for freeing the array. NULL can be returned if
     *         the Composite has no child.
     */
    virtual CSwtPeerArray* GetChildrenPeersL() =0;

    /**
     * Returns the array containing the Composite's children.
     * @return The array containing the Composite's children.
     *         NULL can be returned if the Composite has no child.
     */
    virtual const RSwtControlArray* Children() const =0;

    /**
     * Notifies this Composite that one of its children has gained or lost focus.
     * @param aControl The child whose focus state has changed. The child may
     *        not be a direct child, it can be the child of a child, and so on.
     */
    virtual void ChildFocusChanged(MSwtControl& aControl) =0;

    /**
     * Count focusable kids.
     * @param aTryBreakAfter try to stop counting when reached, count all when < 0
     * @param aExclude count all except the composite and all its descents
     * @param aVisualFocusCount ignores controls which have ENoFocusBackground policy
     */
    virtual TInt CountFocusableChildren(TInt aTryBreakAfter = -1,
                                        const MSwtControl* aExclude = NULL,
                                        TBool aVisualFocusCount = ETrue) const =0;

    /**
     * Sets the tabbing order for the specified controls to match the
     * order that they occur in the argument list.
     * @param aControls An array of pointers to controls, they must all be
     *                  direct children of this Composite. May be NULL
     *                  but if so, aCount must be 0.
     * @param aCount    The number of controls in aControls, if 0
     *                  the tab order is reset to its default.
     */
    virtual void SetTabListL(MSwtControl** aControls, TInt aCount) =0;

    virtual CSwtPeerArray* GetTabListL() const =0;

    /**
     * Moves aChild1 such that it will lie above aChild2
     * @param aChild1 The child to move.
     * @param aChild2 The child to be placed above of. If NULL,
     *                aChild1 is moved to the top.
     */
    virtual void MoveChildAbove(MSwtControl& aChild1, const MSwtControl* aChild2) =0;

    /**
     * Moves aChild1 such that it will lie below aChild2
     * @param aChild1 The child to move.
     * @param aChild2 The child to be placed below of. If NULL,
     *                aChild1 is moved to the bottom.
     */
    virtual void MoveChildBelow(MSwtControl& aChild1, const MSwtControl* aChild2) =0;

    /**
     * When a Radio Button with a Parent which don't have a NO_RADIO_GROUP is selected
     * the other Radio Button with the same parent must be unselected.
     * So the new selected Radio Button call this method.
     * @param aSelectedButton The new selected Buttton.
     */
    virtual void NotifyRadioGroup(MSwtButton* aSelectedButton) = 0;

    /**
     * Returns the pointer grabbing control of the composite.
     * Pay attention that this returns NULL after pointer up!
     * @return Pointer to the pointer grabbing control or NULL.
     */
    virtual MSwtControl* GetPointerGrabbingControl() const
    {
        return NULL;
    }

    /**
     * ScrolledComposite content setter.
     */
    virtual void SetScrolledCompositeContentL(MSwtControl* aContent) = 0;

    /**
     * In case this is a ScrolledComposite, returns the scrolled content.
     */
    virtual MSwtControl* ScrolledCompositeContent() const = 0;

    /**
     * In case this is a ScrolledComposite, returns one of the possible physics actions defined in eswt.h
     */
    virtual TInt ScrolledCompositePysicsAction() const = 0;

    /**
     * Draw scrollbar background.
     */
    virtual void DrawScrollBarBackground(const TRect& aRect) const = 0;
};

/**
 * Interface class for the org.eclipse.swt.widgets.Canvas class
 * A class that implements MSwtCanvas must also implement MSwtComposite.
 */
class MSwtCanvas
{
protected:
    virtual ~MSwtCanvas() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtComposite interface
     */
    virtual MSwtComposite* Composite() =0;

    /**
     * Returns the MSwtScrollable interface
     */
    inline MSwtScrollable* Scrollable()
    {
        return Composite()->Scrollable();
    }

    /**
     * Returns the MSwtControl interface
     */
    inline MSwtControl* Control()
    {
        return Composite()->Control();
    }

    /**
     * Scrolls a rectangular area of this Canvas
     * @param aDest The coordinates of the destination, in client coordinates.
     * @param aRect The rectangle being scrolled, in client coordinates.
     * @param aAll  Whether to scroll the children as well. Only the children
     *              intersecting aRect are to be scrolled.
     */
    virtual void Scroll(const TPoint& aDest, const TRect& aRect, TBool aAll) =0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Decorations class
 * A class that implements MSwtDecorations must also implement MSwtCanvas.
 */
class MSwtDecorations
{
protected:
    virtual ~MSwtDecorations() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtCanvas interface
     */
    virtual MSwtCanvas* Canvas() =0;

    /**
     * Returns the MSwtComposite interface
     */
    inline MSwtComposite* Composite()
    {
        return Canvas()->Composite();
    }

    /**
     * Returns the MSwtScrollable interface
     */
    inline MSwtScrollable* Scrollable()
    {
        return Canvas()->Scrollable();
    }

    /**
     * Returns the MSwtControl interface
     */
    inline MSwtControl* Control()
    {
        return Canvas()->Control();
    }

    /**
     * Returns the decorations menu bar
     */
    virtual const MSwtMenu* MenuBar() =0;

    /**
     * Sets the images for this Decorations
     */
    virtual void SetImageL(const MSwtImage* aImage) =0;

    /**
     * Gets the images of this Decorations
     */
    virtual const MSwtImage* GetImage() const = 0;

    /**
     * Maximises the control
     */
    virtual void SetMaximized(TBool aMaximized) =0;

    /**
     * Returns if the control is currently maximised.
     */
    virtual TBool GetMaximized() const =0;

    /**
     * Minimises (iconifies) the control.
     */
    virtual void SetMinimized(TBool aMinimized) =0;

    /**
     * Returns if the control is currently maximised.
     */
    virtual TBool GetMinimized() const =0;

    /**
     * Sets the menu bar
     */
    virtual void SetMenuBar(const MSwtMenu* aMenuBar) =0;

    /**
     * Sets the <a>Decorations</a>'s title.
     * @param aText A reference to the title descriptor.
     * The method does nothing if the <a>Decorations</a> has no title.
     */
    virtual void SetTextL(const TDesC& aText) =0;

    /**
     * Returns the Decorations' title.
     * @return The title text, can be NULL.
     */
    virtual const TDesC* GetText() const =0;

    /**
     * Sets the original default button of this Decorations.
     */
    virtual void SetDefaultButton(MSwtButton* aButton) =0;

    /**
     * Sets the Button wich have the Focus.
     */
    virtual void SetFocusedDefaultButton(MSwtButton* aButton) =0;

    /**
     * Returns the default button.
     */
    virtual const MSwtButton* OriginalDefaultButton() const =0;

    /**
     * Returns the button wich have the Focus or the OriginalDefaultButton.
     */
    virtual const MSwtButton* DefaultButton() const =0;

    /**
     * Returns the button wich have the Focus or the OriginalDefaultButton.
     */
    virtual TSwtPeer GetDefaultButton() const =0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Shell class
 * A class that implements MSwtShell must also implement MSwtDecorations.
 */
class MSwtShell
        : public MEikStatusPaneObserver
{
public:
    /**
     * Possible different styles for the status pane.
     * The values have to match with what is defined in Java API for
     * MobileShell.NO_STATUS_PANE, MobileShell.SMALL_STATUS_PANE and
     * MobileShell.LARGE_STATUS_PANE.
     */
    enum TSwtStatusPaneStyle
    {
        EStyleNoStatusPane    = 1,  // No status pane at all
        EStyleSmallStatusPane = 2,  // Small status pane variant
        EStyleLargeStatusPane = 3   // Large status pane variant
    };

public:
    virtual ~MSwtShell() {}

    /**
     * Returns the MSwtDecorations interface
     */
    virtual MSwtDecorations* Decorations() =0;

    /**
     * Returns the MSwtCanvas interface
     */
    inline MSwtCanvas* Canvas()
    {
        return Decorations()->Canvas();
    }

    /**
     * Returns the MSwtComposite interface
     */
    inline MSwtComposite* Composite()
    {
        return Decorations()->Composite();
    }

    /**
     * Returns the MSwtScrollable interface
     */
    inline MSwtScrollable* Scrollable()
    {
        return Decorations()->Scrollable();
    }

    /**
     * Returns the MSwtControl interface
     */
    inline MSwtControl* Control()
    {
        return Decorations()->Control();
    }

    /**
     * Returns a pointer to the MSwtMobileShell interface of this Shell.
     * Only actual MobileShells will return a non-null pointer
     * @return The Control's MSwtMobileShell interface, or NULL
     */
    virtual MSwtMobileShell* MobileShellInterface()
    {
        return NULL;
    }
    virtual const MSwtMobileShell* MobileShellInterface() const
    {
        return NULL;
    }

    /**
     * Tests if this Shell is a MobileShell
     */
    virtual TBool IsMobileShell() const
    {
        return EFalse;
    }

    /**
     * Returns the parent Shell of this Shell.
     * Note: this method is only a more effective shortcut. The parent Shell
     * can also be retrieved using Control(), MSwtControl::GetParent()
     * and MSwtControl::ShellInterface()
     */
    virtual MSwtShell* GetParentShell() =0;

    /**
     * Causes the Shell to become the active Shell
     */
    virtual void ForceActive() =0;

    /**
     * Retrieves the Java peer of each child shell.
     * @return A newly allocated array containing the peers; the caller is
     *         responsible for freeing the array. NULL can be returned if
     *         the Composite has no child.
     */
    virtual CSwtPeerArray* GetChildShellsPeersL() =0;

    /**
     * Returns the display of the composite.
     * @return A reference to the display.
     */
    virtual MSwtDisplay& Display() const =0;

    /**
     * Returns the fullscreen mode of the MSwtShell
     */
    virtual TBool FullScreenMode() const =0;

    /**
     * @return the initial editor input mode
     */
    virtual TInt GetImeInputMode() const =0;

    /**
     * @return the minimum possible size for the shell
     */
    virtual TSize GetMinimumSize() const =0;

    /**
     * Set the initial editor input mode
     */
    virtual void SetImeInputMode(TInt aMode) =0;

    /**
     * Sets the minimum possible size for the shell
     */
    virtual void SetMinimumSize(const TSize& aMinimumSize) =0;

    /**
     * Sets the control's style
     * This method is provided in order to implement MobileShell
     * whose ChangeTrimL method impacts on the style.
     */
    virtual void SetStyle(TInt aStyle) =0;

    /**
     * Returns this Shell's full tab list
     * The full tab list contains all of the Shell's children in the order they
     * are to be traversed. It includes focusable as well as non-focusable
     * controls but excludes child Shells.
     */
    virtual const RSwtControlArray& GetFullTabListL() =0;

    /**
     * Tells the Shell to reset its tab list as it may have changed
     * Reasons for resetting the tab list are addition or removal of a child.
     */
    virtual void ResetTabList() =0;

    /**
     * Finds the new control to receive focus if the specified traversal is to be performed.
     * @param aDetail           The traversal detail; only values defined in
     *                          TSwtTraversal are valid, others are to be ignored.
     * @param aFocusControl     The control that currently has the focus.
     * @param aExcludedControls Optional list of controls that can not get the focus.
     */
    virtual MSwtControl* FindTraversalTargetL(TInt aDetail, MSwtControl& aFocusControl,
            RSwtControlArray* aExcludedControls = NULL) =0;

    /**
     * Sets this Shell's focus control
     */
    virtual void SetFocusControl(MSwtControl* aControl) =0;

    /**
     * Test whether this Shell has a modal child Shell
     * Only modal children that would prevent this Shell from being active are
     * to be considered. For example, if a child Shell is modal but invisible
     * it will not cause this method to return ETrue.
     */
    virtual TBool HasModalChild() const =0;

    /**
     * Informs this Shell that its active status has changed.
     * @param aIsActive The new active status
     * @param aSetFocus Instructs a newly activated Shell to try and set the
     *        focus on one of its children. To be ignored if aIsActive is EFalse.
     */
    virtual void ActivityChanged(TBool aIsActive, TBool aSetFocus) =0;

    /**
     * Sets the Shell's Z-order position
     * The Shell must recursively set its child Shells' Z-order positions so as
     * not to obscure them.
     * @param aZPosition The new Z-order position, 0 being the foreground and
     *        the background being >0.
     */
    virtual void SetZPosition(TInt aZPosition) =0;

    /**
     * Return the focussed control or NULL
     */
    virtual MSwtControl* FocusControl() const =0;

    /**
     * Sets the default command
     */
    virtual void SetDefaultCommand(const MSwtCommand* aCommand) =0;

    /**
     * Returns the default command
     */
    virtual const MSwtCommand* DefaultCommand() const =0;

    /**
     * Makes the Shell full-screen or not
     */
    virtual void SetFullScreen(TBool aFullScreen) =0;

    /**
     * Set the pointer to the child which should be added to Control Stack.
     */
    virtual void SetControlGoingToStack(MSwtControl* aControl) =0;

    /**
     * Return the pointer  to the child which should be added to Control Stack.
     */
    virtual MSwtControl* ControlGoingToStack() const =0;

    /**
     * Set the pointer to the child which gains the focus.
     */
    virtual void SetControlGainingFocus(MSwtControl* aControl) =0;

    /**
     * Return the pointer to the child which gains the focus.
     */
    virtual MSwtControl* ControlGainingFocus() const =0;

    /**
     * Get the current status pane style.
     */
    virtual TSwtStatusPaneStyle StatusPaneStyle() const = 0;

    /**
     * Returns the default bounds of the shell taking into consideration
     * fullscreen mode, title and shell style.
     */
    virtual TRect DefaultBounds() const =0;

    /**
     * Remember the control on which the foucus was set before activating the shell
     */
    virtual void SetToBeFocusedControl(MSwtControl* aControl) = 0;

    /**
     * Checks if shell is not keeping reference (in iFocusMemory) to Control,
     * that is going to be unregistered.
     */
    virtual void CheckFocusMemory(MSwtControl& aUnregisteredControl) =0;

    /**
     * Unfortunately we are not able to keep drawing through Java side while
     * kinetic scrolling is ongoing (panning or flicking). Shells need to
     * respond immediately to native draw requests during scrolling.
     * There can be only one Control at a time drawing urgently.
     * See CSwtShell:Draw().
     */
    virtual void SetUrgentPaintControl(MSwtControl* aControl) = 0;
    virtual MSwtControl* UrgentPaintControl() const = 0;

    /**
     * True if Java paint listeners are registered to self or children.
     */
    virtual TBool AsyncPainting() const = 0;
    virtual void SetAsyncPainting(TBool aStatus) = 0;

    /**
     * Attached CBA to the Shell. Cannot be undone.
     */
    virtual void InstallCba() = 0;
    virtual TBool HasCba() const = 0;

    /**
     * Set task tip style.
     */
    virtual void SetTaskTip() = 0;
    virtual TBool IsTaskTip() const = 0;
};

/**
 * Structure containing the data needed for the
 * org.eclipse.swt.core.Item class.
 */
NONSHARABLE_CLASS(CSwtItem)
        : public CBase
{
public:
    inline CSwtItem(HBufC* aString, const MSwtImage* aImage)
            : iString(aString)
            , iImage(aImage)
            , iImageSize(TSize::EUninitialized)
    {
        if (iImage != NULL)
            iImage->AddRef();
    }

    inline CSwtItem(HBufC* aString, const MSwtImage* aImage, const TSize& aImageSize)
            : iString(aString)
            , iImage(aImage)
            , iImageSize(aImageSize)
    {
        if (iImage != NULL)
            iImage->AddRef();
    }

    ~CSwtItem()
    {
        delete iString;
        if (iImage != NULL)
            iImage->RemoveRef();
    }

    inline const TDesC& String() const
    {
        return (iString!=NULL) ? static_cast<const TDesC&>(*iString) : KNullDesC;
    }

    inline void SetString(HBufC* aString)
    {
        delete iString;
        iString = aString;
    }

    inline const MSwtImage* Image() const
    {
        return iImage;
    }

    inline void SetImageSize(const TSize& aNewSize)
    {
        iImageSize = aNewSize;
    }

    inline TSize ImageSize() const
    {
        return iImageSize;
    }

    inline void SetImage(const MSwtImage* aImage)
    {
        if (iImage != NULL)
            iImage->RemoveRef();
        iImage = aImage;
        if (iImage != NULL)
            iImage->AddRef();
    }

private:
    HBufC*           iString; // Owned
    const MSwtImage* iImage;  // Ref counted
    TSize            iImageSize;
};


/**
 * Type for an array of titles in menu bar
 */
typedef CEikMenuBar::CTitleArray CMenuBarTitleArray;


/**
 * Interface class for the org.eclipse.swt.widgets.Menu class
 *
 */
class MSwtMenu
{
protected:
    virtual ~MSwtMenu() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Disposes of (i.e. destroys) the menu.
     * @return The Java peer associated with this object
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the Java peer associated with this object.
     */
    virtual TSwtPeer JavaPeer() const =0;

    /**
     * Registers a new menu item with this Menu
     * A MenuItem may register itself only once it has been
     * <b>fully</b> constructed. Additionally, a MenuItem shall
     * register only once with its parent.
     * @param aChild The menu item to register.
     * @param aIndex The position of the menu item.
     */
    virtual void RegisterChildL(MSwtMenuItem& aChild, TInt aIndex) =0;

    /**
     * Unregisters a menu item from this Menu
     * A MenuItem shall not attempt to unregister itself from a
     * parent it was not registered with.
     * @param aChild The menu item to unregister.
     */
    virtual void UnregisterChild(MSwtMenuItem& aChild) =0;

    /**
     * Enables or disables the menubar
     */
    virtual void SetEnabled(TBool aEnabled) =0;

    /**
     * Make menu visible or invisible
     */
    virtual void SetVisible(TBool aEnabled) =0;

    /**
     * Returns ETrue if the menu is enabled, EFalse otherwise.
     */
    virtual TBool IsEnabled() const =0;

    /**
     * Returns ETrue if the menu is set to be visible, EFalse otherwise.
     * This is just a flag setting, does not reffer to Menu's actual visibility on the screen.
     */
    virtual TBool IsVisible() const =0;

    /**
     * Set default menu item
     */
    virtual void SetDefaultItem(MSwtMenuItem* aMenuItem) =0;

    virtual MSwtMenuItem* DefaultItem() const =0;

    /**
     * Fills the menu pane control with the menu items.
     */
    virtual void FillPaneL(CEikMenuPane& aPane) const =0;

    /**
     * Asks the menu to reload resources and redisplay them on the shown menu pane.
     */
    virtual void RefreshPaneL(CEikMenuPane& aPane) const =0;

#ifdef RD_SCALABLE_UI_V2
    /**
     * Adds the menu items to the stylus popup menu. Also the submenu items are
     * added directly to the stylus popup menu as it does not support cascaded menus.
     * @param aPoupMenu     The stylus popup menu to which the menu items are added.
     */
    virtual void FillPopupL(CAknStylusPopUpMenu& aPopupMenu) const =0;
#endif //RD_SCALABLE_UI_V2

    /**
     * Processes the command associated with the command Id and the item index.
     */
    virtual void ProcessCommandL(TInt aCommandId, TInt aItemIndex, TBool aOfferToSubMenus = EFalse) const =0;

    /**
     * Returns the number of enabled items.
     * @param aStylusPopupMenu  Indicates if the displayable items are counted for
     *                          stylus popup menu. Stylus popup menu is handled differently
     *                          than the other menus.
     * @return The number of displayable items.
     */
    virtual TInt CountDisplayableItems(TBool aStylusPopupMenu = EFalse) const =0;

    virtual const MSwtMenu* GetCascadeMenu(const CEikMenuPaneItem::SData& aItemData) const =0;

    virtual TBool HasRadioGroup() const =0;

    /**
     * Return the host menu pane of this menu.
     */
    virtual CEikMenuPane* Host() const =0;

    /**
     * Set the host menu pane of this menu.
     */
    virtual void SetHost(CEikMenuPane* aPane) =0;

    /**
     * Set menu location
     */
    virtual void SetLocation(TInt aX, TInt aY) =0;

    /**
     * Get menu location
     */
    virtual TPoint GetLocation() const =0;

    /**
     * Show menu on its location
     */
    virtual TBool LocationEnabled() const =0;

    /**
     * Get reference to item list.
     */
    virtual const RPointerArray<MSwtMenuItem>* Items() const = 0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.MenuItem class
 */
class MSwtMenuItem
{
// Types
public:
    enum TSwtMenuItemData
    {
        ESwtMenuItemDataAll = 0,
        ESwtMenuItemDataText,
        ESwtMenuItemDataCascade,
        ESwtMenuItemDataSelection,
        ESwtMenuItemDataImage,
        ESwtMenuItemDataFlags
    };
protected:
    virtual ~MSwtMenuItem() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Disposes of (i.e. destroys) the menu item.
     * @return The Java peer associated with this object
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the Java peer associated with this object.
     */
    virtual TSwtPeer JavaPeer() const =0;

    /**
     * Returns the parent menu.
     */
    virtual const MSwtMenu& Parent() const =0;

    /**
     * Returns ETrue if the menu item is selected.
     */
    virtual TBool GetSelection() const =0;

    /**
     * Sets an accelerator to the menu item.
     */
    virtual void SetAcceleratorL(TInt aAccelerator) =0;

    /**
     * Enables or disables the menu item.
     */
    virtual void SetEnabled(TBool aEnabled) =0;

    /**
     * Sets the image of the menu item.
     */
    virtual void SetImageL(const MSwtImage* aImage) =0;

    /**
     * Sets the Menu of the menu item.
     */
    virtual void SetMenuL(const MSwtMenu* aMenu) =0;

    /**
     * Selects of unselects the menu item.
     */
    virtual void SetSelection(TBool aSelected) =0;

    /**
     * Sets the text of the menu item.
     */
    virtual void SetTextL(const TDesC& aText) =0;

    /**
     * Gets the accelerator of the menu item.
     */
    virtual TInt Accelerator() const =0;

    /**
     * Returns ETrue if the menu item is enabled, EFalse otherwise.
     */
    virtual TBool IsEnabled() const =0;

    /**
     * Gets the image of the menu item.
     */
    virtual const MSwtImage* Image() const =0;

    /**
     * Gets the bitmap and mask of the menu item image.
     * MenuItem keeps the ownership and its image is subreferenced according to aSize.
     */
    virtual void GetImageBitmapsL(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask,
                                  const TSize& aSize) = 0;

    /**
     * Gets the menu associated with the menu item.
     */
    virtual const MSwtMenu* Menu() const =0;

    /**
     * @ brief Gets the text of the menu item.
     */
    virtual const TDesC& Text() const =0;

    /**
     * Gets the command id.
     */
    virtual TInt CommandId() const =0;

    /**
     * Returns the style of the menu item.
     */
    virtual TInt Style() const =0;

    /**
     * Similar to Menu's FillPaneL, adds data to the native pane item.
     * @param one of TSwtMenuItemData
     */
    virtual void UpdatePaneL(CEikMenuPane& aPane, TSwtMenuItemData aDetail) =0;
};


/**
 * Interface class for the menu arranger
 */
class MSwtMenuArranger
{
public:
    typedef enum TSwtMenuEvent
    {
        ESwtMenuAdded,
        ESwtMenuRemoved,
        ESwtMenuEnableStateChanged,
        ESwtMenuItemAdded,
        ESwtMenuItemRemoved,
        ESwtMenuItemEnableStateChanged
    };

public:
    virtual ~MSwtMenuArranger() {}
    virtual void ControlFocusLost() =0;
    virtual void ControlFocusGained(const MSwtControl& aControl) =0;
    virtual void ActiveShellLost() =0;
    virtual void ActiveShellGained(const MSwtShell& aShell) =0;
    virtual void MenuBarSetOnDecorations() =0;
    virtual void SetCommandMenu(const MSwtMenu* aCommandMenu) =0;
    virtual void SetContextMenu(const MSwtMenu* aContextMenu) =0;
    virtual void HandleMenuChangedL(const MSwtMenu& aMenu, TSwtMenuEvent aType) =0;
    virtual CEikMenuBar* EikMenuBar() const =0;
    virtual void AddOptionsCommandL() =0;
    virtual void RemoveOptionsCommand() =0;
    virtual TBool IsOptionsCommandAdded() =0;
    virtual void TryDisplayMenuBarL(TInt aDisplayContextMenu) =0;
    virtual void StopDisplayingMenuBar() =0;
    virtual TBool HasMenuItems() const =0;
    virtual void HandleResolutionChangeL() =0;
    virtual void HandleWindowVisibilityChangeL(CCoeControl* aDestination) =0;
    virtual TSize ImageSize() const =0;
    virtual CEikMenuPane* ParentPane(CEikMenuPane& aPane) const =0;

#ifdef RD_SCALABLE_UI_V2
    virtual void OpenStylusPopupMenuL(const MSwtControl& aControl, const TPoint& aPoint,
                                      MSwtPopupMenuCallBack* aCallBack = NULL) =0;
    virtual void OpenStylusPopupMenuL(const MSwtMenu& aMenu, const TPoint& aPoint) =0;
    virtual void CloseStylusPopupMenuL() =0;
#endif //RD_SCALABLE_UI_V2
};


/**
 * Interface class for the org.eclipse.swt.widgets.Label class
 */
class MSwtLabel
{
protected:
    virtual ~MSwtLabel() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtControl interface.
     */
    virtual MSwtControl* Control() = 0;

    /**
     * Returns a value which describes the position of the text or image in the receiver.
     * This method will only get called if the KSwtStyleSeparator flag is <b>not</b> set.
     */
    virtual TInt GetAlignment() const = 0;

    /**
     * Controls how text and images will be displayed in the receiver.
     * This method will only get called if the KSwtStyleSeparator
     * flag is <b>not</b> set.
     * @param aAlignment The new alignment, one of KSwtStyleLead,
     *        KSwtStyleCenter or KSwtStyleTrail.
     */
    virtual void SetAlignment(TInt aAlignment) = 0;

    /**
     * Sets the receiver's image to the argument, which may be null
     * indicating that no image should be displayed.
     * This method will only get called if the KSwtStyleSeparator
     * flag is <b>not</b> set.
     */
    virtual void SetImageL(const MSwtImage* aImage) = 0;

    /**
     * Sets the receiver's text.
     * This method will only get called if the KSwtStyleSeparator
     * flag is <b>not</b> set.
     */
    virtual void SetTextL(const TDesC& aText) = 0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Button class
 */
class MSwtButton
{
protected:
    virtual ~MSwtButton() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtControl interface.
     */
    virtual MSwtControl* Control() = 0;

    /**
     * Returns a value which describes state of the button.
     */
    virtual TBool GetSelection() const = 0;

    /**
     * Gets the state of the button.
     */
    virtual void SetSelection(TBool aState) = 0;

    /**
     * Returns a value which describes the position of the text or image in the receiver.
     */
    virtual TInt GetAlignment() const = 0;

    /**
     * Controls how text and images will be displayed in the receiver.
     * @param aAlignment The new alignment, one of KSwtStyleLeft,
     *        KSwtStyleCenter or KSWtStyleRight.
     */
    virtual void SetAlignment(TInt aAlignment) = 0;

    /**
     * Sets the receiver's image to the argument, which may be null
     * indicating that no image should be displayed.
     */
    virtual void SetImageL(const MSwtImage* aImage) = 0;

    /**
     * Sets the receiver's text.
     */
    virtual void SetTextL(const TDesC& aText) = 0;

    /**
     * Sets this button as the default Button.
     */
    virtual void SetDefault(TBool aDefault) = 0;

    /**
     * Indicate if the Button is default button.
     */
    virtual TBool IsDefault() const = 0;

    /**
     * Simulate button selection.
     */
    virtual void ClickL() = 0;
};


/**
 * Interface class for Verify event observer.
 */
class MSwtVerifyEventObserver
{
public:
    /**
     * Processes the text from the verify event
     */
    virtual void ProcessVerifiedTextL(TInt aStart, TInt aEnd, TDes& aText) =0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Combo class
 * A class that implements MSwtCombo must also implement MSwtComposite.
 */
class MSwtCombo
{
protected:
    virtual ~MSwtCombo() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtComposite interface
     */
    virtual MSwtComposite* Composite() =0;

    /**
     * Returns the MSwtScrollable interface
     */
    inline MSwtScrollable* Scrollable()
    {
        return Composite()->Scrollable();
    }

    /**
     * Returns the MSwtControl interface
     */
    inline MSwtControl* Control()
    {
        return Composite()->Control();
    }

    /**
     * Appends a string element to the <a>Combo</a>.
     * @param aPtr A reference to the descriptor to be appended.
     */
    virtual void AppendL(const TDesC& aPtr) =0;

    /**
     * Inserts a string element into the <a>Combo</a> at the specified position.
     * @param aPos The position (relative to zero) where the string element is to be inserted.
     * @param aPtr A reference to the descriptor to be inserted.
     */
    virtual void InsertL(TInt aPos, const TDesC& aPtr) =0;

    /**
     * Clears the selection.
     */
    virtual void ClearSelectionL() =0;

    /**
     * Copies the selected text to the clipboard.
     */
    virtual void CopyL() =0;

    /**
     * Deselects the item indexed as aIndex from the <a>Combo</a>.
     *
     * @param aIndex    The index of the item to deselect.
     */
    virtual void DeselectItem(TInt aIndex) =0;

    /**
     * Deselects all the items from the <a>Combo</a>.
     */
    virtual void DeselectAll() =0;

    /**
     * Gets the text of the specified item.
     * @param aItemIndex The specified item.
     * @param aString    The descriptor that will hold the item's text upon return.
     *                   Must be empty upon call.
     */
    virtual void GetItemL(TInt aItemIndex, TPtrC& aString) const =0;

    /**
     * Returns the number of elements of the <a>Combo</a>.
     */
    virtual TInt GetItemCount() const =0;

    /**
     * Returns the item height.
     */
    virtual TInt GetItemHeight() const =0;

    /**
     * Returns the array of items from the <a>Combo</a>
     */
    virtual CPtrCArray* GetItemsL() const =0;

    /**
     * Returns the orientation of the receiver.
     */
    virtual TInt GetOrientation() const =0;

    /**
     * Returns the selection range
     * @return The selection in a TPoint where iX is
     *         the start and  iY is the end.
     */
    virtual TPoint GetSelection() const =0;

    /**
     * Returns the index of the currently selected item.
     */
    virtual TInt GetSelectionIndex() const =0;

    /**
     * Gets the widget's text.
     * @return The widget's text in a newly allocated buffer. The caller is
     *         responsible for deleting the returned buffer. NULL
     *         may be returned if the widget is empty.
     */
    virtual HBufC* GetTextL() const =0;

    /**
     * Returns the height of the text field.
     */
    virtual TInt GetTextHeightL() const =0;

    /**
     * Returns the maximum number of characters that the text field is capable of holding.
     */
    virtual TInt GetTextLimit() const =0;

    /**
     * Gets the number of items that are visible in the drop down portion of the list.
     */
    virtual TInt GetVisibleItemCount() const =0;

    /**
     * Searches for a text item from a given position.
     * @param aString   The string to search for.
     * @param aStart    The position from which we start.
     */
    virtual TInt IndexOf(const TDesC& aString, TInt& aStart) const =0;

    /**
     * Insert a text at the current caret position.
     * No event is posted.
     */
    virtual TBool InsertInTextControlL(const TDesC& aText) =0;

    /**
     * Pastes text from the clipboard.
     * The selected text is deleted from the widget and new text inserted from
     * the clipboard.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool PasteL() =0;

    /**
     * Removes the specified item from the <a>Combo</a>.
     * @param aIndex    The specified item.
     */
    virtual void RemoveItemL(TInt aIndex) =0;

    /**
     * Removes a range of items from the <a>Combo</a>.
     * @param aStart    The first element to remove.
     * @param aEnd      The last element to remove.
     */
    virtual void RemoveRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Removes all items from the <a>Combo</a>.
     */
    virtual void RemoveAllL() =0;

    /**
     * Selects a single item.
     * @param aIndex    The item to select.
     */
    virtual void SelectItemL(TInt aIndex) =0;

    /**
     * Sets the receiver's items to be the given array of items.
     * @param aStringArray The array to set.
     */
    virtual void SetItemsL(MDesCArray* aStringArray) =0;

    /**
     * Sets the orientation of the widget.
     */
    virtual void SetOrientationL(TInt aOrientation) =0;

    /**
     * Sets the selection.
     * Indexing is zero based. The range of a selection is from 0..N where N is
     * the number of characters in the widget. If the specified range is invalid,
     * as much of it as is possible must be set as the selection.
     * @param aStart The selection's starting point.
     * @param aEnd   The selection's ending point.
     */
    virtual void SetSelectionL(TInt aStart, TInt aEnd) =0;

    /**
     * Sets the text
     * @param aText The new text. As a convenience, the descriptor can be freely
     *        modified by the implementation if need be.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool SetTextL(TDes& aText) =0;

    /**
     * Sets the maximum number of characters that the text field is capable of holding to be the argument.
     * @param aTextLimit The limit to set.
     * @return ETrue if a modify notification must be sent
    */
    virtual TBool SetTextLimit(TInt aTextLimit) =0;

    /**
     * Sets the number of items that are visible in the drop down portion of the list.
     * @param aCount The number of items to set.
     */
    virtual void SetVisibleItemCount(TInt aCount) =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.HyperLink class.
 */
class MSwtLink
{
protected:
    virtual ~MSwtLink() {}; // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtControl interface
     */
    virtual MSwtControl* Control() = 0;

    /**
     * Sets the receiver's data.
     * @param aTextFragments Array of text fragments strings.
     * @param aLinkTargets Link targets asociated to the text fragments.
     */
    virtual void SetLinkDataL(const CDesCArray* aTextFragments,
                              const CDesCArray* aLinkTargets) = 0;
};

/**
 * Interface class for the org.eclipse.swt.widgets.List class
 * A class that implements MSwtList must also implement MSwtScrollable.
 */
class MSwtList
{
protected:
    virtual ~MSwtList() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtScrollable interface
     */
    virtual MSwtScrollable* Scrollable() =0;

    /**
     * Returns the MSwtControl interface
     */
    inline MSwtControl* Control()
    {
        return Scrollable()->Control();
    }

    /**
     * Appends a string element to the <a>List</a>.
     * @param aPtr A reference to the descriptor to be appended.
     */
    virtual void AppendL(const TDesC& aPtr) =0;

    /**
     * Inserts a string element into the <a>List</a> at the specified position.
     * @param aPos The position (relative to zero) where the string element is to be inserted.
     * @param aPtr A reference to the descriptor to be inserted.
     */
    virtual void InsertL(TInt aPos, const TDesC& aPtr) =0;

    /**
     * Deselects the items defined in the aIndices array from the <a>List</a>.
     * @param aIndices  The array of the items to deselect.
     * @param aCount    The number of items of this array.
     */
    virtual void DeselectItems(const TInt* aIndices, TInt aCount) =0;

    /**
     * Deselects the item indexed as aIndex from the <a>List</a>.
     * @param aIndex    The index of the item to deselect.
     */
    virtual void DeselectItem(TInt aIndex) =0;

    /**
     * Deselects the items indexed from aStart to aEnd from the <a>List</a>.
     * @param aStart    The first element to deselect.
     * @param aEnd      The last element to deselect.
     */
    virtual void DeselectRange(TInt aStart, TInt aEnd) =0;

    /**
     * Deselects all the items from the <a>List</a>.
     */
    virtual void DeselectAll() =0;

    /**
     * Gets the current item.
     */
    virtual TInt GetFocusIndex() const =0;

    /**
     * Gets the text of the specified item.
     * @param aItemIndex The specified item.
     * @param aString The descriptor that will hold the item's text upon return.
     *        Must be empty upon call.
     */
    virtual void GetItemL(TInt aItemIndex, TPtrC& aString) const =0;

    /**
     * Returns the number of elements of the <a>List</a>.
     */
    virtual TInt GetItemCount() const =0;

    /**
     * Returns the item height.
     */
    virtual TInt GetItemHeight() const =0;

    /**
     * Returns the number of selected elements.
     */
    virtual TInt GetSelectionCount() const =0;

    /**
     * Returns the index of the currently selected item.
     */
    virtual TInt GetSelectionIndex() const =0;

    /**
     * Get the selected items indices.
     */
    virtual const CArrayFix<TInt>* GetSelectionIndices() const =0;

    /**
     * Returns the index of the top element.
     */
    virtual TInt GetTopIndex() const =0;

    /**
     * Searches for a text item from a given position.
     * @param aString   The string to search for.
     * @param aStart    The position from which we start.
     */
    virtual TInt IndexOf(const TDesC& aString, TInt& aStart) const =0;

    /**
     * Returns if the specified item is selected.
     * @param aIndex The specified item.
     */
    virtual TBool IsSelected(TInt aIndex) const =0;

    /**
     * Removes the items defined in the aIndices array from the <a>List</a>.
     * @param aIndices  The array of the items to remove.
     * @param aCount    The number of items of this array.
     */
    virtual void RemoveItemsL(const TInt* aIndices, TInt aCount) =0;

    /**
     * Removes the specified item from the <a>List</a>.
     * @param aIndex    The specified item.
     */
    virtual void RemoveItemL(TInt aIndex) =0;

    /**
     * Removes a range of items from the <a>List</a>.
     * @param aStart    The first element to remove.
     * @param aEnd      The last element to remove.
     */
    virtual void RemoveRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Removes all items from the <a>List</a>.
     */
    virtual void RemoveAllL() =0;

    /**
     * Selects a single item.
     * @param aIndex    The item to select.
     * @param aScroll   Scrolls the list if true.
     */
    virtual void SelectItemL(TInt aIndex, TBool aScroll) =0;

    /**
     * Selects a range of items.
     * @param aStart    The first item to select.
     * @param aEnd      The last item to select.
     */
    virtual void SelectRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Selects all items.
     */
    virtual void SelectAllL() =0;

    /**
     * Sets the current focused item.
     *
     * @param aIndex    The item to focus on.
     */
    virtual void SetFocusIndex(TInt aIndex) =0;

    /**
     * Sets the receiver's items to be the given array of items.
     * @param aStringArray The array to set.
     */
    virtual void SetItemsL(MDesCArray* aStringArray) =0;

    /**
     * Sets the top item index.
     * @param aIndex The index to set as the top item.
     */
    virtual void SetTopIndex(TInt aIndex) =0;

    /**
     * Shows the selection.
     */
    virtual void ShowSelection() =0;
};



/**
 * Interface for class for the org.eclipse.swt.Text class.
 * A class that implements MSwtText must also implement MSwtScrollable.
 */
class MSwtText
{
protected:
    virtual ~MSwtText() {};

public:
    enum {ETextLimit=KMaxTInt};

public:
    virtual MSwtScrollable* Scrollable() =0;

    /**
     * Gets the widget's text.
     * @return The widget's text in a newly allocated buffer. The caller is
     *         responsible for deleting the returned buffer. NULL
     *         may be returned if the widget is empty.
     */
    virtual HBufC* GetTextL() const =0;

    /**
     * Returns a range of the widget's text.
     * If the specified values are out of range, as much text as possible is
     * returned. However, aStart can never exceed aEnd.
     * @param aStart The position of the first character in the range
     * @param aEnd   The position of the last character in the range
     * @return The widget's text in a newly allocated buffer. The caller is
     *         responsible for deleting the returned buffer. NULL
     *         may be returned if the widget is empty.
     */
    virtual HBufC* GetTextRangeL(TInt aStart, TInt aEnd) const =0;

    /**
     * Sets the text
     * @param aText The new text. As a convenience, the descriptor can be freely
     *        modified by the implementation if need be.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool SetTextL(TDes& aText) =0;

    /**
     * Appends a string.
     * @param aText The text to be appended. As a convenience, the descriptor
     *        can be freely modified by the implementation if need be.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool AppendL(TDes& aText) =0;

    /**
     * Inserts a string.
     * The old selection is replaced with the new text.
     * @param aText The text to be inserted. As a convenience, the descriptor
     *        can be freely modified by the implementation if need be.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool InsertL(TDes& aText) =0;

    /**
     * Gets the number of characters.
     */
    virtual TInt GetCharCount() const =0;

    /**
     * Sets the maximum number of characters that the receiver is capable of holding.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool SetTextLimitL(TInt aLimit) =0;

    /**
     * Gets the line number of the caret.
     */
    virtual TInt GetCaretLineNumber() const =0;

    /**
     * Gets the location of the caret.
     * @return The caret's position in pixels, in client area coordinates.
     */
    virtual TPoint GetCaretLocationL() const =0;

    /**
     * Gets the position of the caret.
     * @return The caret's position in characters.
     */
    virtual TInt GetCaretPosition() const =0;

    /**
     * Returns the selection range
     * @return The selection in a TPoint where iX is
     *         the start and  iY is the end.
     */
    virtual TPoint GetSelection() const =0;

    /**
     * Returns the selected text.
     * @return The selected text in a newly allocated buffer, may return
     *         NULL if no text is selected.
     */
    virtual HBufC* GetSelectionTextL() const =0;

    /**
     * Sets the selection.
     * Indexing is zero based. The range of a selection is from 0..N where N is
     * the number of characters in the widget. If the specified range is invalid,
     * as much of it as is possible must be set as the selection.
     * The control is scrolled so as to make the new selection visible.
     * @param aStart The selection's starting point.
     * @param aEnd   The selection's ending point.
     */
    virtual void SetSelectionL(TInt aStart, TInt aEnd) =0;

    /**
     * Clears the selection.
     */
    virtual void ClearSelectionL() =0;

    /**
     * Selects all the text in the widget.
     */
    virtual void SelectAllL() =0;

    /**
     * Makes the selction visible
     * The control is scrolled is need be such that the selection is visible.
     */
    virtual void ShowSelectionL() =0;

    /**
     * Copies the selected text to the clipboard.
     */
    virtual void CopyL() =0;

    /**
     * Pastes text from the clipboard.
     * The selected text is deleted from the widget and new text inserted from
     * the clipboard.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool PasteL() =0;

    /**
     * Gets the editable state.
     */
    virtual TBool GetEditable() const =0;

    /**
     * Sets the editable state.
     */
    virtual void SetEditable(TBool aEditable) =0;

    /**
     * Gets the number of lines.
     */
    virtual TInt GetLineCount() const =0;

    /**
     * Gets the height of a line.
     * @return The height of a line, in pixels.
     */
    virtual TInt GetLineHeightL() const =0;

    /**
     * Returns the zero-relative index of the line which is currently
     * at the top of the widget.
     */
    virtual TInt GetTopIndexL() const =0;

    /**
     * Sets the zero-relative index of the line which is currently at the top.
     */
    virtual void SetTopIndexL(TInt aIndex) =0;

    /**
     * Gets the top pixel.
     * The top pixel is the pixel position of the line that is currently at the
     * top of the widget.  On some platforms, a text widget can be scrolled by
     * pixels instead of lines so that a partial line is displayed at the top of the widget.
     * The top pixel changes when the widget is scrolled.
     * The top pixel does not include the widget trimming.
     */
    virtual TInt GetTopPixelL() const =0;

    /**
     * Gets the echo character.
     * The echo character is the character that is displayed when the user
     * enters text or the text is changed by the programmer.
     */
    virtual TText GetEchoChar() const =0;

    /**
     * Sets the echo character.
     * The echo character is the character that is displayed when the user
     * enters text or the text is changed by the programmer. Setting the echo
     * character to 0 clears the echo character and redraws the original
     * text. If for any reason the echo character is invalid, the default echo
     * character for the platform is used.
     */
    virtual void SetEchoCharL(TText aEcho) =0;

    /**
     * Sets the double click enabled flag.
     * The double click flag enables or disables the default action of the text
     * widget when the user double clicks.
     */
    virtual void SetDoubleClickEnabled(TBool aEnabled) =0;

    /**
     * Returns the orientation of the receiver.
     */
    virtual TInt GetOrientation() const =0;

    /**
     * Sets the orientation of the widget.
     */
    virtual void SetOrientationL(TInt aOrientation) =0;
};

/**
 * Interface for class for the org.eclipse.swt.ProgressBar class.
 */
class MSwtProgressBar
{
protected:
    virtual ~MSwtProgressBar() {};

public:
    virtual MSwtControl* Control() = 0;
    virtual TInt GetMaximum() const =0;
    virtual TInt GetMinimum() const =0;
    virtual TInt GetSelection() const =0;
    virtual void SetMaximum(TInt aValue) =0;
    virtual void SetMinimum(TInt aValue) =0;
    virtual void SetSelection(TInt aValue) =0;
};

/**
 * Interface for class for the org.eclipse.swt.Slider class.
 */
class MSwtSlider
{
protected:
    virtual ~MSwtSlider() {};

public:
    virtual MSwtControl* Control() = 0;
    virtual TInt GetMaximum() const =0;
    virtual TInt GetMinimum() const =0;
    virtual TInt GetIncrement() const =0;
    virtual TInt GetPageIncrement() const =0;
    virtual TInt GetSelection() const =0;
    virtual void SetMaximum(TInt aValue) =0;
    virtual void SetMinimum(TInt aValue) =0;
    virtual void SetIncrement(TInt aValue) =0;
    virtual void SetPageIncrement(TInt aValue) =0;
    virtual void SetSelection(TInt aValue) =0;
    virtual void SetValues(TInt aSelection, TInt aMinimum,
                           TInt aMaximum, TInt aIncrement, TInt aPageIncrement) =0;
};


/**
 * Interface for class for the org.eclipse.swt.FileDialog class.
 */
class MSwtFileDialog
{
protected:
    virtual ~MSwtFileDialog() {};

public:
    /**
     * Opens a File dialog, according to the style of the dialog.
     * @return A buffer containing the path and filename of the selected file,
     *         or NULL if Cancel button is used to close the dialog. The user
     *         has to release this buffer.
     */
    virtual HBufC* OpenLD() =0;
};


/**
 * Interface for a utility class
 * An instance (and only one) of the concrete class implementing this interface
 * will be created by the Display when the UI thread is started.
 * The instance will be destroyed when the UI thread is terminated.
 */
class MSwtUiUtils : public MSwtAppFocusObserver
{
public:
    /**
     * Destructor
     */
    virtual ~MSwtUiUtils() {};

    /**
     * Returns the matching standard color for the given constant.
     * @param aId SWT color constant
     * @return The Symbian OS' matching standard color. Any value other
     *         than one of the SWT color constants which is passed
     *         in will result in the color black.
     */
    virtual TRgb GetSystemColor(TSwtColorId aId) const =0;

    /**
     * Returns the font spec of a reasonable font for applications to use.
     */
    virtual TFontSpec SystemFontSpec() const =0;

    /**
     * Open a modal note
     * @param aDisplay The Display the MessageBox belongs to
     * @param aTitle Dialog's title.
     * @param aMessage The message that will be displayed.
     * @param aStyle MessageBox's style
     * @return A button id.
     */
    virtual TInt RunMessageBoxDlgLD(MSwtDisplay& aDisplay, const TDesC& aTitle,
                                    const TDesC& aMessage, TInt aStyle) const =0;

    /**
     * Returns the active shell.
     */
    virtual MSwtShell* GetActiveShell() const =0;

    /**
     * Returns the parent top level Shell of the active shell.
     */
    virtual MSwtShell* GetParentTopShellOfActiveShell() const =0;

    /**
     * Get the top most top Shell, that is the only top Shell displayed.
     */
    virtual MSwtShell* TopMostTopShell() const = 0;

    /**
     * Returns the peer of the active shell.
     */
    virtual TSwtPeer GetActiveShellPeer() const =0;

    /**
     * Returns the existing shells.
     */
    virtual const RPointerArray<MSwtShell>& Shells() const =0;

    /**
     * Returns peers of the existing shells
     */
    virtual CSwtPeerArray* GetShellPeersL() const =0;

    /**
     * Returns the focussed control, or NULL.
     */
    virtual TSwtPeer GetFocusControl() const =0;

    /**
     * Returns the Control's closest parent top level Shell
     */
    virtual MSwtShell& GetControlTopShell(const MSwtControl& aControl) const =0;

    /**
     * Draw a rectangle which has the appearance of the platform's
     * focus rectangle.
     * If there is no such thing as a focus rectangle, this method must draw
     * a rectangle with the GC's foreground colour.
     * @param aGc   The GC to draw the rectangle on
     * @param aRect The rectangle to draw
     */
    virtual void DrawFocusRect(MSwtGc& aGc, const TRect& aRect) const =0;

    /**
     * Returns the cba created by MSwtUiUtils
     */
    virtual CEikButtonGroupContainer& Cba() const =0;

    /**
     * Recreate the status pane based on the current active shell if that shell
     * is a top shell, or on the last active top shell.
     */
    virtual void UpdateStatusPaneL() =0;

    /**
     * Get the state of the application focus.
     */
    virtual TBool IsAppFocused() const =0;

    /**
     * Get localized string for the ok command title
     */
    virtual const HBufC* GetCommandOkTitleL() const =0;

    /**
     * Get localized string for the cancel command title
     */
    virtual const HBufC* GetCommandCancelTitleL() const =0;

    /**
     * Handles free ram memory event.
     * @param aEventType The type of event.
     */
    virtual void HandleFreeRamEventL(TInt aEventType) = 0;

    /**
     * Returns the mouse event state mask that can be passed to the eSWT application in MouseEvent.
     * @param aPointerEventModifiers    The modifiers of the corresponding symbian TPointerEvent.
     * @return State mask that can be sent to the eSWT application in MouseEvent
     */
    virtual TInt GetPointerEventStateMask(TUint aPointerEventModifiers) const =0;

    /**
     * Returns the eventual faded Shell.
     */
    virtual MSwtShell* FadingShell() const =0;

    /**
     * Change the name of the Title in the Status Pane.
     * @param The name for the eSwt display title in the Status Pane.
     */
    virtual void SetAppNameL(const TDesC* aAppName) = 0;

    /**
     * Gets the image maximum size from the layouts.
     * @param The type of the image passed to the layouts to get the size.
     * Returns maximum size from the layouts
     */
    virtual TSize GetBestImageSize(TInt aImageType) = 0;

    /**
     * Default padding reference for helping inline controls (Button, Label,
     * Text, DateEditor) achieving similar ideal size, height especially.
     *
     */
    virtual TMargins8 InlineReferencePadding() const = 0;

    /**
     * Default font height reference for helping inline controls (Button, Label,
     * Text, DateEditor) achieving similar ideal size, height especially.
     * Does not mean that all these controls should have same font!
     */
    virtual TInt InlineReferenceFontHeight() const = 0;

    /**
     * Update cached values, inform Shells.
     */
    virtual void HandleResourceChangedL(TInt aType) = 0;

    /**
     * Get breadth of the scrollbar. The value is cached.
     * If the scrollbar is null returns 0.
     */
    virtual TInt ScrollBarBreadth(const CEikScrollBar* aBar) = 0;

    /**
      * Returns number of active browsers in applicaition.
      */
    virtual TInt ActiveBrowserCount() const = 0;

    /**
     * Setter and getter for key input flag.
     */
    virtual TBool NaviKeyInput() const = 0;
    virtual void SetNaviKeyInput(TBool aKeyInput) = 0;

    /**
     * This should be always used to hide, show the CBA.
     */
    virtual void SetCbaVisible(TBool aVisible) = 0;

    /**
     * Returns the pointer to the control that is currently receiving the pointer events
     * because it has received EButton1Down event. This returns a valid control only between
     * EButton1Down and EButton1Up events.
     * @return Pointer to the control that is receiving the pointer events currently or NULL.
     */
    virtual MSwtControl* GetPointerGrabbingControl() = 0;

    /**
     * Default bounds for TaskTips
     */
    virtual TRect TaskTipRect() const = 0;

    /**
     * Sets the pointerGrabbingControl
     * Calling with NULL of course unsets.
     */
    virtual void SetPointerCaptureControl(MSwtControl* aControl) = 0;

    /**
     * Returns the current control that is grabbing the pointer events.
     */
    virtual MSwtControl* PointerCaptureControl() = 0;
};


/**
 * Interface class for constructing native peers.
 */
class MSwtFactory
{
public:
    /**
     * Destructor.
     */
    virtual ~MSwtFactory() {};

    /**
     * Constructs a utility object
     */
    virtual MSwtUiUtils* NewUiUtilsL(MSwtDisplay& aDisplay) const =0;

    /**
     * Constructs a Canvas
     * @param aDisplay The Display this Canvas belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtCanvas* NewCanvasL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Composite
     *
     * @param aDisplay The Display this Composite belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtComposite* NewCompositeL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Decorations
     * @param aDisplay The Display this Decorations belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtDecorations* NewDecorationsL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Shell
     * @param aDisplay     The Display this Shell belongs to
     * @param aPeer        The Java object associated with this C++ object
     * @param aParent      The parent Composite, if NULL a top level
     *                     Shell is to be created.
     * @param aStyle       The SWT style flags
     */
    virtual MSwtShell* NewShellL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtShell* aParent,
        TInt aStyle) const =0;

    virtual MSwtMenu* NewMenuL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        TInt aStyle) const =0;

    virtual MSwtMenuItem* NewMenuItemL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtMenu& aParent,
        TInt aIndex,
        TInt aStyle) const =0;

    virtual MSwtMenuArranger* NewMenuArrangerL(
        MSwtDisplay& aDisplay) const =0;

    /**
     * Constructs a Link
     * @param aDisplay The Display this List belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtLink* NewLinkL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a List
     * @param aDisplay The Display this List belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtList* NewListL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Label
     * @param aDisplay The Display this Label belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtLabel* NewLabelL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Button
     * @param aDisplay The Display this Button belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtButton* NewButtonL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Text
     * @param aDisplay The Display this Text belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtText* NewTextL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a ProgressBar
     * @param aDisplay The Display this ProgressBar belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtProgressBar* NewProgressBarL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Slider
     * @param aDisplay The Display this Slider belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtSlider* NewSliderL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Combo
     * @param aDisplay The Display this Combo belongs to
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtCombo* NewComboL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Construct a File dialog. Custom title.
     * @param aFileName Initial path and filename. A new buffer will be allocated.
     * @param aTitle    Title of the dialog.
     * @param aStyle    Style of the dialog: KSwtStyleOpen or KSwtStyleSave.
     */
    virtual MSwtFileDialog* NewFileDialogL(const TDesC& aFileName,
                                           const TDesC& aTitle, TInt aStyle) const =0;

    virtual MSwtCommand* NewCommandL(MSwtDisplay& aDisplay, TSwtPeer aPeerRef,
                                     MSwtControl& aParentControl, MSwtCommand* aParentCommand, TInt aType,
                                     TInt aPriority) const =0;
    virtual MSwtCommandArranger* NewCommandArrangerL(MSwtDisplay& aDisplay) const =0;

    /**
     * Constructs a MobileShell
     * @param aDisplay            The Display this Shell belongs to
     * @param aPeer               The Java object associated with this C++ object
     * @param aParent             The parent Composite, if NULL
     *                            a top level Shell is to be created.
     * @param aStyle              The SWT style flags
     */
    virtual MSwtMobileShell* NewMobileShellL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtShell* aParent, TInt aStyle) const =0;

    /**
     * Constructs a MobileDevice
     * @param aDisplay The Display this Device belongs to
     * @param aPeer               The Java object associated with this C++ object
     */
    virtual MSwtMobileDevice* NewMobileDeviceL(MSwtDisplay& aDisplay, TSwtPeer aPeer) const =0;

    /**
     * Constructs a Screen
     * @param aPeer               The Java object associated with this C++ object
     * @param aInputId            The identifier of the output.
     */
    virtual MSwtInput* NewInputL(TSwtPeer aPeer, TInt aInputId) const =0;

    /**
     * Constructs a Screen
     *
     * @param aDisplay The Display this HyperLink belongs to.
     * @param aPeer The Java object associated with this C++ object
     * @param aScreenId Screen identifier.
     */
    virtual MSwtScreen* NewScreenL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                   TInt aScreenId) const =0;

    /**
     * Constructs a multi page pages.
     * @param aDisplay            The Display this MultiPageDialog belongs to.
     * @param aPeer               The Java object associated with this C++ object
     * @param aParent             The parent of this control ( it must be a shell )
     */
    virtual MSwtMultiPagePager* NewMultiPagePagerL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtShell& aParent) const =0;

    /**
     * Constructs a multi page tab group. This is a control for implementing page tabs
     * @param aDisplay            The Display this MultiPageDialog belongs to.
     * @param aPeer               The Java object associated with this C++ object
     * @param aParent             The parent of this control ( it must be a shell )
     */
    virtual MSwtMultiPageTabGroup*  NewMultiPageTabGroupL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtShell& aParent) const = 0;

    /**
     * Constructs a multi page dialog. This is a control for implementing page tabs
     *
     * @param aDisplay            The Display this MultiPageDialog belongs to.
     * @param aPeer               The Java object associated with this C++ object
     * @param aParent             The parent of this control ( it must be a shell )
     */
    virtual MSwtMultiPageDialog*  NewMultiPageDialogL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtShell& aParent) const = 0;

    /**
     * Constructs a CaptionedControl
     * @param aDisplay The Display this CaptionedControl belongs to.
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtCaptionedControl* NewCaptionedControlL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const =0;

    /**
     * Constructs a ConstrainedText
     * @param aDisplay The Display this CaptionedControl belongs to.
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtConstrainedText* NewConstrainedTextL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const =0;

    /**
     * Constructs a DateEditor
     * @param aDisplay The Display this DateEditor belongs to.
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtDateEditor*  NewDateEditorL(MSwtDisplay& aDisplay,
                                            TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const =0;

    /**
     * Constructs a HyperLink
     * @param aDisplay The Display this HyperLink belongs to.
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtHyperLink* NewHyperLinkL(MSwtDisplay& aDisplay,
                                         TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const =0;

    /**
     * Constructs a ListBox
     * @param aDisplay     The Display this ListBox belongs to.
     * @param aPeer        The Java object associated with this C++ object
     * @param aParent      The parent Composite
     * @param aStyle       The SWT style flags
     */
    virtual MSwtListBox* NewListBoxL(MSwtDisplay& aDisplay,
                                     TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const =0;

    /**
     * Constructs a ListView
     * @param aDisplay     The Display this ListView belongs to.
     * @param aPeer        The Java object associated with this C++ object
     * @param aParent      The parent Composite
     * @param aStyle       The SWT style flags
     */
    virtual MSwtListView* NewListViewL(MSwtDisplay& aDisplay,
                                       TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const =0;

    /**
     * Constructs a SortedList
     * @param aDisplay     The Display this ListBox belongs to.
     * @param aPeer        The Java object associated with this C++ object
     * @param aParent      The parent Composite
     * @param aStyle       The SWT style flags
     */
    virtual MSwtSortedList* NewSortedListL(MSwtDisplay& aDisplay,
                                           TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const =0;

    /**
     * Constructs a TextExtension
     * @param aDisplay The Display this TextExtension belongs to.
     * @param aPeer    The Java object associated with this C++ object
     * @param aParent  The parent Composite
     * @param aStyle   The SWT style flags
     */
    virtual MSwtTextExtension* NewTextExtensionL(MSwtDisplay& aDisplay,
            TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle) const =0;

    /**
     * Constructs a TimedMessageBox
     * @param aTitle        The string to display as the receiver's title
     * @param aMessage      The description of the purpose for which the dialog was opened
     * @param aStyle        The SWT style flags
     * @param aImage   The image to display in the message box
     */
    virtual void RunTimedMessageBoxDlgLD(const TDesC& aTitle,
                                         const TDesC& aMessage, TInt aStyle, const MSwtImage* aImage) const=0;

    /**
     * Constructs a QueryDialog
     * @param aTitle        The dialog title
     * @param aStyle        The SWT style flags
     * @param aQueryType    The query style flags
     * @param aPromptText   The prompt text
     * @param aDefaultValue The default value string
     * @param aMinimum      The minimum of allowed characters
     * @param aMaximum      The maximum of allowed characters
     * @return The string input result
     */
    virtual HBufC* RunQueryDialogL(const TDesC& aTitle, TInt aStyle,
                                   TInt aQueryType, const TDesC& aPromptText, const TDesC& aDefaultValue,
                                   TInt aMinimum, TInt aMaximum) const=0;

    /**
     * Calls ConvertDefaultValueL of QueryDialog
     * @param aString       The string to check the format
     * @param aQueryType    The query style flags
     * @return True if the string is valid
     */
    virtual HBufC* RunQueryDialogConvertDefaultValueL(const TDesC& aString,
            TInt aQueryType) const=0;

    /**
     * Construct a Clipboard
     * @param aPeer The Java object associated with this C++ object
     */
    virtual MSwtClipboard* NewClipboardL(TSwtPeer aPeer) const =0;

    /**
     * Construct a TableColumn
     * @param aPeer         The Java object associated with this C++ object
     * @param aParent       The parent Table
     * @param aStyle        The SWT style flags
     * @param aColumnIndex  The index where to insert the column
     */
    virtual MSwtTableColumn* NewTableColumnL(
        TSwtPeer aPeer,
        MSwtTable& aParent,
        TInt aStyle,
        TInt aColumnIndex) const =0;

    /**
     * Construct a TableItem
     * @param aDisplay   The Display this TableItem belongs to
     * @param aPeer      The Java object associated with this C++ object
     * @param aParent    The parent Table
     * @param aRowIndex  The index where to insert the row inside the Table
    */
    virtual MSwtTableItem* NewTableItemL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtTable& aParent,
        TInt aRowIndex) const =0;

    /**
     * Constructs a Table
     * @param aDisplay  The Display this Table belongs to.
     * @param aPeer     The Java object associated with this C++ object
     * @param aParent   The parent Composite
     * @param aStyle    The SWT style flags
     */
    virtual MSwtTable* NewTableL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a Tree
     * @param aDisplay     The Display this Tree belongs to.
     * @param aPeer        The Java object associated with this C++ object
     * @param aParent      The parent Composite
     * @param aStyle       The SWT style flags
     */
    virtual MSwtTree* NewTreeL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle) const =0;

    /**
     * Constructs a QueryDialog
     * @param aDefaultColor The default selected color.
     * @return  The color input result, null if the user doesn't select a color
     *          (Exit from the dialog with the CANCEL button).
     *          This object must be deleted by the caller.
     */
    virtual TRgb* RunColorDialogL(TRgb* aDefaultColor) const=0;

    /**
     * Constructs a FontDialog
     */
    virtual MSwtFontDialog* NewFontDialogL() const =0;

    /**
     * Constructs a DirectoryDialog
     */
    virtual MSwtDirectoryDialog* NewDirectoryDialogL(const TDesC& aFilterPath,
            const TDesC& aTitle) const =0;

    /**
     * Consturct a browser object
     * @param aDisplay The Display the browser  belongs to.
     * @param aPeer The Java object associated with this C++ object
     * @param aParent he parent composite
     * @param aStyle The SWT style flags
     */
    virtual MSwtBrowser* NewBrowserL(MSwtDisplay& aDisplay, TSwtPeer aPeer
                                     , MSwtComposite& aParent, TInt aStyle) const = 0;

    /**
     * Constructs a Color
     * @param aDevice The device this colour is represented on.
     * @param aRgb    A RGB triplet of the colour to represent.
     */
    virtual MSwtColor* NewColorL(const MSwtDevice& aDevice, const TRgb& aRgb) const =0;

    /**
     * Constructs a GC suitable for the font & bitmap server's devices.
     * @param aDrawable   The drawable that created aGc.
     * @param aGc         The native GC to use, ownership is transferred.
     * @param aDestructor The function that will take care of destroying iGc.
     * @param aPenCol     The pen's colour.
     * @param aBrushCol   The brush's colour.
     * @param aFont       The font to use
     */
    virtual MSwtGc* NewBitmapGcL(MSwtBitmapDrawable& aDrawable, CFbsBitGc* aGc,
                                 MSwtGc::TDestructor aDestructor, const TRgb& aPenCol, const TRgb& aBrushCol,
                                 const MSwtFont& aFont) const =0;

    /**
     * Constructs a GC suitable for the window server's devices.
     * @param aDrawable     The drawable that created aGc.
     * @param aGc           The native GC to use, ownership is transferred.
     * @param aDestructor   The function that will take care of destroying iGc.
     * @param aPenCol       The pen's colour.
     * @param aBrushCol     The brush's colour.
     * @param aFont         The font to use
     * @param aBoundaryRect The rectangle drawing is restricted to, in GC coordinates.
     * @param aGcScreenRect The GC's rectangle in screen coordinates
     */
    virtual MSwtGc* NewWindowGcL(MSwtWindowDrawable& aDrawable, CWindowGc* aGc,
                                 MSwtGc::TDestructor aDestructor, const TRgb& aPenCol, const TRgb& aBrushCol,
                                 const MSwtFont& aFont,const TRect& aBoundaryRect, const TRect& aGcScreenRect) const =0;

    /**
     * Constructs a Font
     * @param aDevice       The device this font is represented on.
     * @param aTypefaceName The name of the typeface (i.e. Courrier for example).
     * @param aHeight       The height of the font.
     * @param aStyle        The style of the font.
     */
    virtual MSwtFont* NewFontL(const MSwtDevice& aDevice,
                               const TDesC& aTypefaceName, TInt aHeight, TInt aStyle) const =0;
};


#endif // ESWTWIDGETSCORE_H
