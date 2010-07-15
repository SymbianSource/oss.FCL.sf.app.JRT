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


#include <biditext.h>
#include <aknappui.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <coecntrl.h>
#include <eikon.hrh>
#include <AknsDrawUtils.h>
#include <swtlaffacade.h>
#include <coreuiavkoneswt.h>
#include "eswtmobileextensions.h"
#include "swtcontrolbase.h"
#include "swtuiutils.h"
#include "swtjavabufferdrawer.h"


// ======== MEMBER FUNCTIONS ========


EXPORT_C ASwtControlBase::ASwtControlBase(MSwtDisplay& aDisplay, TSwtPeer aPeer,
        MSwtComposite* aParent, TInt aStyle)
        : iDisplay(aDisplay)
        , iPeer(aPeer)
        , iParent(aParent)
        , iCtrlFlags(0)
        , iStyle(aStyle)
        , iFont(NULL)
        , iPopupMenu(NULL)
        , iIsOnControlStack(EFalse)
        , iStackedSetRedrawFalseNb(0)
        , iBackgrdDefaultUsed(ETrue)
        , iFocusFrame(NULL)
        , iFocusChanged(EFalse)
{
    ASSERT(iPeer!=NULL);

    // Orientation not specified, find default
    TInt orientation = iStyle & KSwtOrientationMask;
    if (orientation!=KSwtStyleLeftToRight && orientation!=KSwtStyleRightToLeft)
    {
        if (iParent != NULL) // Inherit from parent
            orientation = iParent->Control()->Style() & KSwtOrientationMask;
        else                 // Based on user's locale
            orientation = (TBidiText::ScriptDirectionality(User::Language())==TBidiText::ELeftToRight) ?
                          KSwtStyleLeftToRight : KSwtStyleRightToLeft;

        iStyle &= ~KSwtOrientationMask;
        iStyle |= orientation;
    }
}

void ASwtControlBase::CreateFocusBackgroundL()
{
    ASSERT(!iFocusFrame);

    TRect rect(CoeControl().Rect());
    TRect innerRect;
    innerRect.iTl = CSwtLafFacade::GetLayoutRect(
                        CSwtLafFacade::EInputFieldSkinPlacingGeneralLine2, rect, 0).Rect().iBr;
    innerRect.iBr = CSwtLafFacade::GetLayoutRect(
                        CSwtLafFacade::EInputFieldSkinPlacingGeneralLine5, rect, 0).Rect().iTl;

    iFocusFrame = CAknsFrameBackgroundControlContext::NewL(
#ifdef RD_JAVA_S60_RELEASE_9_2
                      KAknsIIDQsnFrPopupPreview,
#else
                      KAknsIIDQsnFrInput,
#endif // RD_JAVA_S60_RELEASE_9_2
                      rect, innerRect, EFalse /*parent absolute*/);

    if (iBackgrdDefaultUsed)
    {
        iFocusFrame->SetParentContext(SwtMopSupplyCtrlContext(EFalse));
    }
}

CAknsBasicBackgroundControlContext* ASwtControlBase::FocusBackgroundNow() const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (!iDisplay.UiUtils().NaviKeyInput() && !iPressed)
    {
        return NULL;
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    // Pressed down state shows the highlight no matter what.
    if (!iPressed)
    {
        TInt policy = FocusBackgroundPolicy();

        // CaptionedControls that have a custom back color set provide a highlight with
        // no parent context and that can be a problem for semi transparent highligts.
        // The remedy for this is to let the child control draw its own highlight for
        // as long as the parent CaptionedControl has the custom background color set.
        if ((policy == ENoFocusBackground)
                || (policy == EEmbeddedFocusBackground)
                || (iParent && iParent->Control()->CaptionedControlInterface() && iParent->Control()->IsDefaultBackgroundUse())
                || (policy == ECaptionedCtrlFocusBackground && !IsFocusedOrChildIsFocused())
                || (policy == EDefaultFocusBackground && GetShell().FocusControl() != this)
                || (GetShell().Composite()->CountFocusableChildren(1, this) == 0))
        {
            return NULL;
        }
    }

    // If we got here, it means we have to draw a focus background.
    if (!iFocusFrame)
    {
        TRAP_IGNORE(const_cast<ASwtControlBase*>(this)->CreateFocusBackgroundL());
    }

    return iFocusFrame;
}

void ASwtControlBase::RedrawFocusBackground()
{
    MSwtCaptionedControl* captCtrl = GetTopCaptionedControl();
    if (captCtrl)
    {
        captCtrl->Composite()->Control()->Redraw();
    }
    else
    {
        Redraw();
    }
}

void ASwtControlBase::InitControlBaseL()
{
    RegisterWithParentL();
}

EXPORT_C ASwtControlBase::~ASwtControlBase()
{
    delete iFocusFrame;

    // RemoveRef here in case it goes through destruction before being
    // registered (and Dispose() is not called)
    if (iFont != NULL)
        iFont->RemoveRef();

#ifdef _lint
    //lint -esym(1740, ASwtControlBase::iPeer)
    //lint -esym(1740, ASwtControlBase::iParent)
    iFont      = NULL;
    iPopupMenu = NULL;
#endif
}

// From class MCoeControlBackground.
// Draw the Background. This method was called by the eSWT and Symbian drawing framework.
// If the control is a Composite then make sure not to draw the background to
// the areas that will be painted by the children. This is to avoid painting
// over them as this would cause flickering.
//
EXPORT_C void ASwtControlBase::Draw(CWindowGc& aGc, const CCoeControl& aControl,
                                    const TRect& aRect) const
{
    RRegion* clippingRegion = NULL;
    const MSwtComposite* composite = CompositeInterface();
    if (composite)
    {
        clippingRegion = SetupChildClippingRegion(composite, aGc, aRect);
    }

    TBool isDialogShell = IsShell() && iParent;
    TBool isTaskTip = isDialogShell && ShellInterface()->IsTaskTip();

    // Draw the background.
    if (!IsDefaultBackgroundUse()
#ifdef RD_JAVA_S60_RELEASE_9_2
            // Fill the round corners
            || (isDialogShell && !isTaskTip)
#endif // RD_JAVA_S60_RELEASE_9_2
       )
    {
        aGc.SetBrushColor(GetBackground());
        aGc.Clear(aRect);
    }

    // Hopefully the highlight back is always semi transparent,
    // otherwise the back color is lost.
    MAknsControlContext* cc = SwtMopSupplyCtrlContext();
    AknsDrawUtils::Background(AknsUtils::SkinInstance(), cc, &aControl, aGc, aRect);

    // Cancel & destroy the clipping region
    CancelChildClippingRegion(clippingRegion, aGc);
}

EXPORT_C void ASwtControlBase::DrawBorder(const TRect& /*aRect*/) const
{
    TInt borderWidth = GetBorderWidth();
    if (borderWidth > 0)
    {
        CWindowGc& gc = CoeControl().SystemGc();
        gc.SetBrushStyle(CGraphicsContext::ENullBrush);
        gc.SetPenColor(UiUtils().GetSystemColor(ESwtColorWidgetBorder));
        gc.SetPenStyle(CGraphicsContext::ESolidPen);
        gc.SetPenSize(TSize(borderWidth, borderWidth));
        gc.DrawRect(CoeControl().Rect());
    }
}

// In eSWT all border should have the same width/height.
EXPORT_C TInt ASwtControlBase::GetBorderWidth() const
{
    if (HasBorderStyle())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Window coords
EXPORT_C TRect ASwtControlBase::ClientRect() const
{
    return BorderInnerRect();
}

TBool ASwtControlBase::HasBorderStyle() const
{
    TBool res = (iStyle & KSwtStyleBorder);
    const MSwtShell* shell = ShellInterface();
    if (shell)
    {
        // TaskTip shells have the border.
        res = shell->IsTaskTip() || (res && !(iStyle & KSwtStyleNoTrim) && iParent);
    }
    return res;
}

TSize ASwtControlBase::BorderSizeDelta() const
{
    return TSize(GetBorderWidth()*2, GetBorderWidth()*2) ;
}

// Window coords
EXPORT_C TRect ASwtControlBase::BorderInnerRect() const
{
    TRect borderInnerRect = CoeControl().Rect();

    if (borderInnerRect.Height() != 0 && borderInnerRect.Width() != 0)
    {
        TInt w(GetBorderWidth());
        borderInnerRect.Shrink(w, w);
    }

    return borderInnerRect;
}

EXPORT_C TRect ASwtControlBase::BorderOuterRect(const TRect& aRect) const
{
    TRect borderOuterRect(aRect);
    borderOuterRect.Grow(GetBorderWidth(), GetBorderWidth());

    return borderOuterRect;
}

TKeyResponse ASwtControlBase::HandleDiagonalKeysL(const TKeyEvent& aKeyEvent, TEventCode aType,
        TBool aTraversalDoIt, const TUint key1, const TStdScanCode scanCode1,
        const TUint key2, const TStdScanCode scanCode2)
{
    TKeyEvent keyTemp;
    // send first event
    keyTemp.iRepeats = 0;
    keyTemp.iScanCode = scanCode1;
    keyTemp.iModifiers = 32768;
    keyTemp.iCode = 0;
    HandleKeyL(keyTemp, EEventKeyDown, aTraversalDoIt);     //DOWN
    keyTemp = aKeyEvent;
    keyTemp.iCode = key1;
    keyTemp.iScanCode = scanCode1;
    TKeyResponse firstResponse = HandleKeyL(keyTemp, aType, aTraversalDoIt);     // KEYCODE
    keyTemp.iRepeats = 0;
    keyTemp.iScanCode = scanCode1;
    keyTemp.iModifiers = 32768;
    keyTemp.iCode = 0;
    HandleKeyL(keyTemp, EEventKeyUp, aTraversalDoIt);      // UP

    // send second event
    keyTemp.iRepeats = 0;
    keyTemp.iScanCode = scanCode2;
    keyTemp.iModifiers = 32768;
    keyTemp.iCode = 0;
    HandleKeyL(keyTemp, EEventKeyDown, aTraversalDoIt);     // DOWN
    keyTemp = aKeyEvent;
    keyTemp.iCode = key2;
    keyTemp.iScanCode = scanCode2;
    TKeyResponse secondResponse = HandleKeyL(keyTemp, aType, aTraversalDoIt);     // KEYCODE
    keyTemp.iRepeats = 0;
    keyTemp.iScanCode = scanCode2;
    keyTemp.iModifiers = 32768;
    keyTemp.iCode = 0;
    HandleKeyL(keyTemp, EEventKeyUp, aTraversalDoIt);       // UP

    TKeyResponse response = EKeyWasNotConsumed;
    if (EKeyWasConsumed == firstResponse && EKeyWasConsumed == secondResponse)
    {
        response = EKeyWasConsumed;
    }

    return response;
}

void ASwtControlBase::DrawComponentControls(const CCoeControl& aControl,
        const TRect& aRect, CWindowGc& aGc, TBool aCanDrawOutsideRect)
{
    const TInt count = aControl.CountComponentControls();
    for (TInt i=0; i<count; ++i)
    {
        CCoeControl& compCtrl = *aControl.ComponentControl(i);
        if (compCtrl.IsVisible())
        {
            if (compCtrl.OwnsWindow())
            {
            }
            else
            {
                TRect drawRect(compCtrl.Rect());
                drawRect.Intersection(aRect);
                if (!drawRect.IsEmpty())
                {
                    aGc.Reset();
                    aGc.SetClippingRect(drawRect);
                    compCtrl.DrawBackground(drawRect);
                    compCtrl.DrawForeground(drawRect);
                }

                if (aCanDrawOutsideRect)
                {
                    DrawComponentControls(compCtrl, aRect, aGc, aCanDrawOutsideRect);
                }
                else if (!drawRect.IsEmpty())
                {
                    DrawComponentControls(compCtrl, drawRect, aGc, aCanDrawOutsideRect);
                }
            }
        }
    }
}

TBool ASwtControlBase::RectContainsRect(const TRect& aContainer, const TRect& aContainee)
{
    return (aContainer.iTl.iX <= aContainee.iTl.iX
            && aContainer.iTl.iY <= aContainee.iTl.iY
            && aContainer.iBr.iX >= aContainee.iBr.iX
            && aContainer.iBr.iY >= aContainee.iBr.iY);
}

void ASwtControlBase::FlushJavaBuffer(MSwtGc* aGc, TInt* aIntParams, TInt aIntCount, HBufC* aStrParams, RWindow* aWindow, const TRect& aDrawRect) const
{
    iDisplay.JavaBufferDrawer().Draw(aGc, aIntParams, aIntCount, aStrParams, aWindow, aDrawRect);
}

EXPORT_C void ASwtControlBase::PostMouseEventL(const TPointerEvent& aPointerEvent)
{
    TPoint eventPosition(aPointerEvent.iPosition);

    // Is is essential that the coordinates posted to the queue are in screen
    // coordinates to ensure the case where late mouse events are handled
    // after the control has moved away from the current position.
    TPoint pos(WindowToScreen(eventPosition));

    // Compute the state mask
    TInt stateMask = UiUtils().GetPointerEventStateMask(aPointerEvent.iModifiers);

    // Handle double click (must be issued between Down and Up)
    if (aPointerEvent.iModifiers & EModifierDoubleClick)
        iDisplay.PostMouseEventL(iPeer, ESwtEventMouseDoubleClick, 0, pos, stateMask);

    // Compute the event type and button number
    TSwtEventType type;
    TInt          button = 0;
    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EButton1Down:
        type   = ESwtEventMouseDown;
        button = KSwtMouseButton1;
        break;
    case TPointerEvent::EButton1Up:
        type   = ESwtEventMouseUp;
        button = KSwtMouseButton1;
        break;
    case TPointerEvent::EButton2Down:
        type   = ESwtEventMouseDown;
        button = KSwtMouseButton2;
        break;
    case TPointerEvent::EButton2Up:
        type   = ESwtEventMouseUp;
        button = KSwtMouseButton2;
        break;
    case TPointerEvent::EButton3Down:
        type   = ESwtEventMouseDown;
        button = KSwtMouseButton3;
        break;
    case TPointerEvent::EButton3Up:
        type   = ESwtEventMouseUp;
        button = KSwtMouseButton3;
        break;
    case TPointerEvent::EDrag:
    case TPointerEvent::EMove:
        type = ESwtEventMouseMove;
        break;
    default:
        return;
    }

    iDisplay.PostMouseEventL(iPeer, type, button, pos, stateMask);
}

EXPORT_C void ASwtControlBase::EnableFocusHighlight(TBool /*aEnable*/)
{
    // Method is used only on 9.2 for list controls (Lists, ListView,
    // ListBox and Table)
}

TPoint ASwtControlBase::ComputeLocationToSet(const TPoint& aLocation)
{
    ASSERT(!IsShell());
    return aLocation + iParent->Scrollable()->Control()->ClientRect().iTl; //lint !e613
}

EXPORT_C MSwtShell& ASwtControlBase::GetShell() const
{
    if (!iShell)
    {
        MSwtShell* selfAsShell = const_cast<MSwtShell*>(ShellInterface());
        if (selfAsShell)
            iShell = selfAsShell;
        else
            iShell = &iParent->Control()->GetShell();
    }
    ASSERT(iShell);
    return *iShell;
}

void ASwtControlBase::Invalidate(const TRect& aRect)
{
    if (!aRect.IsEmpty() && CoeControl().IsVisible() && !(iCtrlFlags & MSwtControl::EFlagDoNotDraw))
        ForceInvalidate(aRect);
}

void ASwtControlBase::ForceInvalidate(const TRect& aRect)
{
    if (aRect.IsEmpty())
    {
        return;
    }

    MSwtShell& shell = GetShell();
    MSwtControl* urgentPaintControl = shell.UrgentPaintControl();
    if (urgentPaintControl)
    {
        if (IsDescentOf(*urgentPaintControl))
        {
            urgentPaintControl->PaintUrgently(aRect);
        }
    }
    else
    {
        if (shell.AsyncPainting())
        {
            TRect dirtyRect(aRect);
            MSwtControl* controlShell = shell.Control();

            // Convert to Screen Coordinate
            dirtyRect.Move(controlShell->CoeControl().PositionRelativeToScreen());

            TBool mergeable = !(iStyle & KSwtStyleNoMergePaints);
            TRAP_IGNORE(iDisplay.PostPaintEventL(JavaPeer(), controlShell->JavaPeer(), dirtyRect, mergeable));
        }
        else
        {
            static_cast<RWindow*>(shell.Control()->CoeControl().DrawableWindow())->Invalidate(aRect);
        }
    }
}

EXPORT_C void ASwtControlBase::DoPaint(const TRect& aRect) const
{
    ASSERT(!IsShell());
    const CCoeControl& ctrl = CoeControl();
    ctrl.DrawBackground(aRect);
    ctrl.DrawForeground(aRect);
}

EXPORT_C void ASwtControlBase::SetColorL(TLogicalColor aLogicalColor, const MSwtColor* aColor)
{
    CCoeControl& coeCtrl = CoeControl();
    TRgb color((aColor!=NULL) ? aColor->RgbValue() : iDisplay.CoeEnv()->Color(aLogicalColor));
    coeCtrl.OverrideColorL(aLogicalColor, color);
}

EXPORT_C CSwtUiUtils& ASwtControlBase::UiUtils() const
{
    return static_cast<CSwtUiUtils&>(iDisplay.UiUtils());
}

EXPORT_C void ASwtControlBase::DoDelete()
{
    delete this;
}

EXPORT_C TRect ASwtControlBase::VisibleRect(TBool aVisibleBounds) const
{
    // WINDOW COORDINATES!
    if (!CoeControl().IsVisible())
        return TRect(); // This is (0,0,0,0)

    TRect rect((aVisibleBounds)? CoeControl().Rect() : ClientRect());

    if (!IsShell())
    {
        rect = ClipToVisibleRect(rect);
    }

    return rect;
}

TRect ASwtControlBase::ClipToVisibleRect(const TRect& aRect) const
{
    // WINDOW COORDINATES!
    TRect rect = aRect;
    MSwtComposite* composite = iParent;
    while (composite != NULL)
    {
        MSwtControl* ctrl = composite->Control();
        rect.Intersection(ctrl->ClientRect());
        composite = (!ctrl->IsShell()) ? ctrl->GetParent() : NULL;
    }
    return rect;
}

// Hurts the performance of non overlapping drawing when there are a lot
// of control sibblings and does not really work for overlapping drawing
#if 0
void ASwtControlBase::VisibleRegion(RRegion& aRegion, TBool aVisibleBounds) const
{
    // SCREEN COORDINATES!
    aRegion.AddRect(TRect(CoeControl().PositionRelativeToScreen(),
                          aVisibleBounds ? CoeControl().Size() : ClientRect().Size()));
    MSwtComposite* parent = iParent;
    const MSwtControl*  children = this;

    while (parent && !children->IsShell())
    {
        const RSwtControlArray* siblings = parent->Children();

        for (TInt i = siblings->Find(children)-1; i >= 0; i--)
        {
            MSwtControl* control = (*siblings)[i];

            if (!control->IsShell() && control->CoeControl().IsVisible())
            {
                aRegion.SubRect(TRect(control->CoeControl().PositionRelativeToScreen(), control->CoeControl().Size()));
            }
        }

        children = parent->Control();
        parent = parent->Control()->GetParent();
    }
}
#endif

TPoint ASwtControlBase::WindowToScreen(const TPoint& aPoint) const
{
    const CCoeControl& coeControl = CoeControl();
    const TPoint posInScreen(coeControl.PositionRelativeToScreen());
    const TPoint posInWindow((coeControl.OwnsWindow()) ? TPoint(0,0) : coeControl.Position());

    TInt x = (aPoint.iX - posInWindow.iX) + posInScreen.iX;
    TInt y = (aPoint.iY - posInWindow.iY) + posInScreen.iY;
    return TPoint(x,y);
}

void ASwtControlBase::FocusabilityChanged()
{
    CCoeControl& coeCtrl = CoeControl();
    MSwtShell& shell = GetShell();

    if (!IsFocusable(EFalse))
    {
        if (coeCtrl.IsFocused())
        {
            // Composite losing focusability can mean that it has received a new child.
            // Otherwise it means that the Composite itself has been disabled or hidden.
            // If a Control loses focusability, let the parent find a new focus target.
            // If a Composite loses focusability, find new target first in children then ancestors.
            MSwtControl* newFocus = NULL;
            MSwtComposite* comp = CompositeInterface();
            if (comp)
            {
                TRAP_IGNORE((newFocus = shell.FindTraversalTargetL(ESwtTraverseArrowNext, *this)));
            }
            if (!newFocus)
            {
                TRAP_IGNORE((newFocus = shell.FindTraversalTargetL(ESwtTraverseParent, *this)));
            }
            if (newFocus)
            {
                newFocus->CoeControl().SetFocus(ETrue);
            }
            else
            {
                // This control was focused but now there's nothing focusable anymore
                coeCtrl.SetFocus(EFalse);
                shell.SetControlGainingFocus(NULL);
                shell.SetControlGoingToStack(NULL);
                if (UiUtils().NextFocusedControl() == this)
                {
                    UiUtils().SetNextFocusedControl(NULL);
                }
            }
        }
        ASSERT(!coeCtrl.IsFocused());
    }
    else
    {
        if (UiUtils().GetActiveShell() == &shell && shell.FocusControl() == NULL)
            coeCtrl.SetFocus(ETrue);
    }
}

EXPORT_C void ASwtControlBase::HandleFocusChanged(TDrawNow /*aDrawNow*/)
{
    CCoeControl&  coeCtrl     = CoeControl();
    TBool         focused     = coeCtrl.IsFocused();
    CSwtUiUtils&  uiUtils     = UiUtils();
    MSwtShell*    activeShell = uiUtils.GetActiveShell();
    MSwtControl*  oldFocus    = (activeShell!=NULL) ? activeShell->FocusControl() : NULL;
    TBool         appFocus    = uiUtils.IsAppFocused();

    // Detect when SetFocus() has been called twice with the same parameter
    if (oldFocus && ((focused && oldFocus == this)
                     || (!focused && oldFocus != this)))
    {
        return;
    }

    MSwtShell& shell = GetShell();

    // Make sure that UiUtils' information of the control that most
    // recently had focus is up-to-date. This way the focus will be
    // returned to the correct control once we regain application
    // focus after losing it.
    if (oldFocus == this)
    {
        uiUtils.SetNextFocusedControl(oldFocus);
    }

    // if app fails on next line check this issue:
    // caller should call MSwtControl::SetFocus() or MSwtControl::ForceFocus()
    // or Overridden function if posssible,
    // instead of CCoeControl::SetFoucus directly
    ASSERT(!(focused && !appFocus));

    if (!focused)
    {
        // Remove current control from ControlStack since the control becomes unfocused
        RemoveFromControlStack();
    }

    // Update the focus control
    if (focused)
    {
        // Clear the urgent paint control if focus goes outside it.
        // There cannot be an urgently painted control that is not
        // focused or does not have a focused child.
        MSwtControl* urgentPaintControl = shell.UrgentPaintControl();
        if (urgentPaintControl && urgentPaintControl != this
                && !IsDescentOf(*urgentPaintControl))
        {
            shell.SetUrgentPaintControl(NULL);
        }

        // Activate our shell and remove focus from current owner
        if (activeShell != &shell)
        {
            // This removes focus from oldFocus
            uiUtils.SetActiveShell(shell, EFalse);
        }
        else
        {
            if (oldFocus)
            {
                // first add control before removing the control from controlstack
                // so that it stop calling focusChanged of shell.
                TRAP_IGNORE(AddToControlStackL(ECoeStackPriorityDefault));
                oldFocus->CoeControl().SetFocus(EFalse);
            }
        }

        ASSERT(!oldFocus || !oldFocus->CoeControl().IsFocused());

        // Set ourselves as the new focus control
        // Can't register a Shell with itself
        if (shell.Control() != this)
        {
            shell.SetFocusControl(this);
        }
    }
    else
    {
        shell.SetFocusControl(NULL);
    }

    // Notify parent of our new focus
    MSwtComposite* parent = GetParent();
    if (parent)
    {
        parent->ChildFocusChanged(*this);
    }


    // Notify Menu arranger
    if (focused)
    {
        iDisplay.MenuArranger().ControlFocusGained(*this);
    }
    else
    {
        iDisplay.MenuArranger().ControlFocusLost();
    }

    // Notify Command arranger
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    if (commandArranger)   // commandArranger may be null as Mobile Extensions is optional
    {
        if (focused)
        {
            commandArranger->ControlFocusGained(*this);
        }
        else
        {
            commandArranger->ControlFocusLost();
        }
    }

    // Post the focus event
    TSwtEventType type((focused) ? ESwtEventFocusIn : ESwtEventFocusOut);
    TRAP_IGNORE(iDisplay.PostFocusEventL(JavaPeer(), type));

    if (!(iCtrlFlags & MSwtControl::EFlagDoNotDraw))
    {
        Redraw();
    }

    if (focused)
    {
        // Add to ControlStack since the control becomes focused
        TRAP_IGNORE(AddToControlStackL(ECoeStackPriorityDefault));
    }
}

TKeyResponse ASwtControlBase::OfferKeyEventToCommandAndMenuArrangersL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    if ((aType == EEventKeyDown) || (aType==EEventKeyUp))
    {
        return EKeyWasNotConsumed;
    }

    MSwtShell* activeTopShell = iDisplay.UiUtils().GetParentTopShellOfActiveShell();
    MSwtMenuArranger& menuArranger = iDisplay.MenuArranger();
    if (activeTopShell
            && activeTopShell->FullScreenMode()
            && menuArranger.IsOptionsCommandAdded())
    {
        if (aKeyEvent.iCode == EKeyCBA1 || aKeyEvent.iCode == EKeyCBA2)
        {
            menuArranger.TryDisplayMenuBarL(EFalse);
            return EKeyWasConsumed;
        }
    }

    // If the OK and backspace keys are not used by the control, HandleKeyL() returns EKeyWasNotConsumed.
    // Thus, this allows the command arranger calling the operations associated to these keys
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    if (commandArranger != NULL)
    {
        // Clear command is called when backspace key is pressed
        if (aKeyEvent.iCode == EKeyBackspace &&
                !IsKeyUsed(EKeyBackspace) &&
                commandArranger->IsClearOperationSet())
        {
            if (commandArranger->DoClearOperationL())
            {
                return EKeyWasConsumed;
            }
        }
        else if (!AknLayoutUtils::MSKEnabled() ||
                 (activeTopShell && activeTopShell->FullScreenMode()))
        {
            // When the selection key is pressed, the default command is called otherwise
            // a menu with the OK and SELECT commands is displayed
            if (aKeyEvent.iCode == EKeyOK
                    && !IsKeyUsed(EKeyOK)
                    && commandArranger->IsContextSensitiveOperationSet())
            {
                if (commandArranger->DoContextSensitiveOperationL())
                {
                    return EKeyWasConsumed;
                }
            }
        }
        else if (aKeyEvent.iCode == EKeyEnter && !IsKeyUsed(EKeyEnter))
        {
            if (commandArranger->HandleEnterKeyL())
            {
                return EKeyWasConsumed;
            }
        }
    }

    return EKeyWasNotConsumed;
}

TKeyResponse ASwtControlBase::HandleKeyL(const TKeyEvent& aKeyEvent,
        TEventCode aType, TBool aTraversalDoIt)
{
    // Follows implementation of diagonal Keys. Native diagonal keys are in eSWT
    // replaced by two corresponding key events check if key is diagonal, if yes,
    // change the traversalDoIt state
    if (aKeyEvent.iCode == EKeyLeftUpArrow
            || aKeyEvent.iCode == EKeyRightUpArrow
            || aKeyEvent.iCode == EKeyLeftDownArrow
            || aKeyEvent.iCode == EKeyRightDownArrow)
    {
        aTraversalDoIt = EFalse;
    }

    switch (aKeyEvent.iCode)
    {
    case EKeyLeftUpArrow:
        return HandleDiagonalKeysL(aKeyEvent, aType, aTraversalDoIt, EKeyLeftArrow,
                                   EStdKeyLeftArrow, EKeyUpArrow, EStdKeyUpArrow);
    case EKeyLeftDownArrow:
        return HandleDiagonalKeysL(aKeyEvent, aType, aTraversalDoIt, EKeyLeftArrow,
                                   EStdKeyLeftArrow, EKeyDownArrow, EStdKeyDownArrow);
    case EKeyRightUpArrow:
        return HandleDiagonalKeysL(aKeyEvent, aType, aTraversalDoIt, EKeyRightArrow,
                                   EStdKeyRightArrow, EKeyUpArrow, EStdKeyUpArrow);
    case EKeyRightDownArrow:
        return HandleDiagonalKeysL(aKeyEvent, aType, aTraversalDoIt, EKeyRightArrow,
                                   EStdKeyRightArrow, EKeyDownArrow, EStdKeyDownArrow);
    }

    if (aKeyEvent.iScanCode == EStdKeyDownArrow
            || aKeyEvent.iScanCode == EStdKeyUpArrow
            || aKeyEvent.iScanCode == EStdKeyRightArrow
            || aKeyEvent.iScanCode == EStdKeyLeftArrow)
    {
        iDisplay.UiUtils().SetNaviKeyInput(ETrue);
    }

    // Offer key event to command arranger first
    TKeyResponse keyResponse = OfferKeyEventToCommandAndMenuArrangersL(aKeyEvent, aType);
    if (keyResponse == EKeyWasConsumed)
    {
        iTraverse = EFalse;
        return EKeyWasConsumed;
    }

    // Traversal only on key event, not key down nor key up
    if (aType != EEventKey)
    {
        if (!iTraverse

#ifndef RD_JAVA_S60_RELEASE_9_2
                && !(AknLayoutUtils::MSKEnabled()
                     && aKeyEvent.iScanCode == EStdKeyDevice3
                     && iDisplay.CommandArranger()->IsDefaultCommandSet())
#endif
           )
        {
            iDisplay.PostKeyEventL(*this, aKeyEvent, aType);
        }
        iTraverse = EFalse;
        return EKeyWasConsumed;
    }

    // In any case cancel the traverse state
    iTraverse = EFalse;

    TSwtTraversal detail(ESwtTraverseNone);

    const TInt modifiers = aKeyEvent.iModifiers & ECtrlFuncShiftMask;

    // If we press left or right arrow key and if there is a TabGroup displayed,
    // we let the TabGroup manage the keys.
    if (aTraversalDoIt && modifiers == 0
            && (aKeyEvent.iCode == EKeyRightArrow || aKeyEvent.iCode == EKeyLeftArrow))
    {
        keyResponse = OfferKeyEventToTabGroupL(aKeyEvent, aType);
        if (keyResponse == EKeyWasConsumed)
        {
            return EKeyWasConsumed;
        }
    }

    // Tab
    if (aKeyEvent.iCode == EKeyTab)
    {
        switch (modifiers)
        {
        case 0:                                // Tab
        case EModifierCtrl:                    // Ctrl+Tab
            detail = ESwtTraverseTabNext;
            break;
        case EModifierShift:                   // Shift+Tab
        case EModifierCtrl|EModifierShift:     // Ctrl+Shift+Tab
            detail = ESwtTraverseTabPrevious;
            break;
        default:
            break;
        }
    }
    else if (aKeyEvent.iCode == EKeyRightArrow ||
             aKeyEvent.iCode == EKeyDownArrow)
    {
        detail = ESwtTraverseArrowNext;
    }
    else if (aKeyEvent.iCode == EKeyLeftArrow ||
             aKeyEvent.iCode == EKeyUpArrow)
    {
        detail = ESwtTraverseArrowPrevious;
    }

    /*
     * These two conditions are because workaround in Text. We simulate
     * EEventKey from EEventKeyDown, so in this moment we haven't
     * aKeyEvent.iCode value, only iScanCode.
     */
    if (aType == EEventKey && aKeyEvent.iCode == 0)
    {
        if (aKeyEvent.iScanCode == EStdKeyLeftArrow)
        {
            detail = ESwtTraverseArrowPrevious;
        }
        if (aKeyEvent.iScanCode == EStdKeyRightArrow)
        {
            detail = ESwtTraverseArrowNext;
        }
    }

    // Return (only if there is a default button or a default command)
    if ((aKeyEvent.iCode==EKeyEnter || aKeyEvent.iCode==EKeyOK) && modifiers==0)
    {
        const MSwtShell& shell = GetShell();
        if ((!ButtonInterface() || ((Style() & KSwtStylePush) == 0))
                && const_cast<MSwtShell&>(shell).Decorations()->DefaultButton())
        {
            detail = ESwtTraverseReturn;
        }

        if (shell.DefaultCommand())
        {
            detail = ESwtTraverseReturn;
        }
    }

    // Escape (only for non top-level Shells)
    if (aKeyEvent.iCode==EKeyEscape && modifiers==0)
    {
        if (GetShell().GetParentShell() != NULL)
            detail = ESwtTraverseEscape;
    }

    // Send event
    if (detail != ESwtTraverseNone)
    {
        iTraverse = aTraversalDoIt;
        MSwtComposite* parent = this->GetParent();
        if (parent)
        {
            // if the parent does not have a tablist.
            // it means the parent should not be used as a composite.
            // So don't post traverse event
            CSwtPeerArray* tabList = parent->GetTabListL();
            if (tabList)
            {
                delete tabList;
                tabList = NULL;
                iDisplay.PostTraverseEventL(*this, detail, aKeyEvent, aType, aTraversalDoIt);
            }
        }
        else
        {
            iDisplay.PostTraverseEventL(*this, detail, aKeyEvent, aType, aTraversalDoIt);
        }
    }
    else
    {
        iDisplay.PostKeyEventL(*this, aKeyEvent, aType);
    }

    return EKeyWasConsumed;
}

TKeyResponse ASwtControlBase::OfferKeyEventToTabGroupL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    CEikStatusPane *sp = ((CAknAppUi*)iDisplay.CoeEnv()->EikAppUi())->StatusPane();
    ASSERT(sp != NULL);
    CAknNavigationControlContainer* naviCtrlCtnr = static_cast<CAknNavigationControlContainer*>
            (sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    ASSERT(naviCtrlCtnr != NULL);
    CAknNavigationDecorator* navi = naviCtrlCtnr->Top();
    if (navi != NULL && navi->ControlType() == CAknNavigationDecorator::ETabGroup) // if there is a tab group
    {
        CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>(navi->DecoratedControl());
        if (tabGroup != NULL && tabGroup->OfferKeyEventL(aKeyEvent, aType) == EKeyWasConsumed)
        {
            TSwtTraversal detail(ESwtTraverseNone);
            if (AknLayoutUtils::LayoutMirrored())
            {
                detail = (aKeyEvent.iCode == EKeyRightArrow ?
                          ESwtTraversePagePrevious : ESwtTraversePageNext);
            }
            else
            {
                detail = (aKeyEvent.iCode == EKeyRightArrow ?
                          ESwtTraversePageNext : ESwtTraversePagePrevious);
            }
            iDisplay.PostTraverseEventL(*this, detail, aKeyEvent, aType, ETrue);
            // The tab has consumed the key event.
            return EKeyWasConsumed;
        }
    }
    return EKeyWasNotConsumed;
}

EXPORT_C void ASwtControlBase::RegisterWithParentL()
{
    if (iParent != NULL)
    {
        // eSWT hierarchy controls.
        iParent->RegisterChildL(*this);
    }

    // Set our DoNotDraw flag according to our parent flag
    if (iParent != NULL)
    {
        UpdateDoNotDrawFlag();
    }
}

EXPORT_C TTypeUid::Ptr ASwtControlBase::SwtMopSupplyObject(TTypeUid aId)
{
    // MAknsControlContext is ask.
    if (aId.iUid == MAknsControlContext::ETypeId)
    {
        MAknsControlContext* bckgd = SwtMopSupplyCtrlContext();

        // If a parent have is own background context and the color background was not set on it.
        // Return the Control Context of his background.
        if (bckgd != NULL)
        {
            return MAknsControlContext::SupplyMopObject(aId, bckgd);
        }
    }

    // Else return Null
    return TTypeUid::Null();
}

MAknsControlContext* ASwtControlBase::SwtMopSupplyCtrlContext(TBool aBeginSearchToThis) const
{
    MAknsControlContext* background = NULL;
    if (aBeginSearchToThis)
    {
        background = FocusBackgroundNow();
        if (background)
        {
            return background;
        }
    }

    MSwtComposite* parent = GetParent();
    if (IsDefaultBackgroundUse())
    {
        background = aBeginSearchToThis ? GetControlContext() : NULL;
        while (!background && parent)
        {
            background = parent->Control()->GetControlContext();

            // If the parent is a Shell we stop on it.
            if (parent->Control()->IsShell())
            {
                parent = NULL;
            }
            else
            {
                parent = parent->Control()->GetParent();
            }
        }
    }

    return background;
}

RRegion* ASwtControlBase::SetupChildClippingRegion(const MSwtComposite* aComposite,
        CWindowGc& aGc,
        const TRect& aDrawRect,
        const TPoint& aOffset /*= TPoint( 0, 0 )*/) const
{
    ASSERT(aComposite);

    TBool clippingRegionSet(EFalse);
    RRegion* clippingRegion = NULL;
    if (aComposite)
    {
        TInt childCount(0);
        const RSwtControlArray* children = aComposite->Children();
        if (children)
        {
            childCount = children->Count();
        }
        // If this control has children then proceed with setting
        // a clipping region to exclude them.
        if (childCount > 0)
        {
            clippingRegion = new RRegion(aDrawRect, childCount + 1);
            if (clippingRegion)
            {
                for (TInt i = 0; i < childCount; ++i)
                {
                    MSwtControl* child = (*children)[ i ];
                    // Shells are window owning so they will get clipped anyway.
                    // No point in adding overhead by handling them manually here.
                    // Also controls that are not visible should not be clipped.
                    if (child && !child->IsShell() && child->CoeControl().IsVisible()
                            && !child->HasDoNotDrawFlag())
                    {
                        clippingRegion->SubRect(child->CoeControl().Rect());
                    }
                }
                if (!clippingRegion->CheckError())
                {
                    if (aOffset != TPoint(0, 0))
                    {
                        clippingRegion->Offset(aOffset);
                    }
                    if (aGc.SetClippingRegion(*clippingRegion) == KErrNone)
                    {
                        clippingRegionSet = ETrue;
                    }
                }
                if (!clippingRegionSet)
                {
                    if (clippingRegion)
                    {
                        clippingRegion->Destroy();
                    }
                    clippingRegion = NULL;
                }
            }
        }
    }
    return clippingRegion;
}

void ASwtControlBase::CancelChildClippingRegion(RRegion* aClippingRegion,
        CWindowGc& aGc) const
{
    // Cancel & destroy the clipping region
    if (aClippingRegion)
    {
        aGc.CancelClippingRegion();
        aClippingRegion->Destroy();
    }
}

TPoint ASwtControlBase::ClientToWindow(const TPoint& aPoint) const
{
    return aPoint + ClientRect().iTl;
}

TRect ASwtControlBase::ClientToWindow(const TRect& aRect)  const
{
    const TRect clientRect(ClientRect());
    return TRect(aRect.iTl+clientRect.iTl, aRect.iBr+clientRect.iTl);
}

TPoint ASwtControlBase::WindowToClient(const TPoint& aPoint) const
{
    return aPoint - ClientRect().iTl;
}

EXPORT_C const MSwtFont& ASwtControlBase::DefaultFont() const
{
    return *(iDisplay.Device().GetSystemFont());
}

void ASwtControlBase::HandleRectChanged()
{
    // Rebound the focus highlight
    if (iFocusFrame)
    {
        TRect rect(CoeControl().Rect());
        TRect innerRect;
        innerRect.iTl = CSwtLafFacade::GetLayoutRect(
                            CSwtLafFacade::EInputFieldSkinPlacingGeneralLine2, rect, 0).Rect().iBr;
        innerRect.iBr = CSwtLafFacade::GetLayoutRect(
                            CSwtLafFacade::EInputFieldSkinPlacingGeneralLine5, rect, 0).Rect().iTl;
        iFocusFrame->SetFrameRects(rect, innerRect);
    }
}

EXPORT_C void ASwtControlBase::HandleSizeChanged()
{
    if (iPrevCoeSize == CoeControl().Size())
    {
        return;
    }

    if (iPrevCoeSize != CoeControl().Size())
    {
        iPrevCoeSize = CoeControl().Size();
        TRAP_IGNORE(iDisplay.PostResizeEventL(iPeer));
        HandleRectChanged();
    }
}

EXPORT_C void ASwtControlBase::HandlePositionChanged()
{
    if (iPrevSwtPos == GetLocation() && iPrevCoePos == CoeControl().Position())
    {
        return;
    }

    if (iPrevSwtPos != GetLocation())
    {
        iPrevSwtPos = GetLocation();
        TRAP_IGNORE(iDisplay.PostMoveEventL(iPeer));
    }

    if (iPrevCoePos != CoeControl().Position())
    {
        iPrevCoePos = CoeControl().Position();
        HandleRectChanged();
    }
}

//
// Virtual methods from MSwtDrawable
//
EXPORT_C MSwtGc* ASwtControlBase::NewGcL()
{
    CCoeControl& coeCtrl = CoeControl();

    // Create the native GC
    CWsScreenDevice& device = static_cast<CWsScreenDevice&>(GraphicsDevice());
    CWindowGc* nativeGc = new(ELeave) CWindowGc(&device);
    TInt error = nativeGc->Construct();
    if (error)
    {
        delete nativeGc;
        User::Leave(error);
    }
    nativeGc->Activate(*coeCtrl.DrawableWindow());

    // Set the GC's origin to the top-left of the client area
    const TRect clientRect(ClientRect()); // in window coords
    nativeGc->SetOrigin(clientRect.iTl);

    // Compute the boundary rect (in GC coords)
    TRect boundaryRect(clientRect);
    if (!IsShell())
    {
        MSwtComposite* parent = GetParent();
        if (parent != NULL)
            boundaryRect.Intersection(static_cast<ASwtControlBase*>(parent->Control())->VisibleRect());
    }
    boundaryRect.Move(-clientRect.iTl);

    // Compute the GC rect in screen coords
    const TRect gcScreenRect(WindowToScreen(clientRect.iTl), clientRect.Size());

    // Get the colours & the font
    CEikonEnv* eikenv = iDisplay.CoeEnv();
    TRgb penCol(eikenv->ControlColor(EColorControlText,       coeCtrl));
    TRgb brushCol(eikenv->ControlColor(EColorControlBackground, coeCtrl));
    const MSwtFont* font = GetFont();
    ASSERT(font!=NULL);

    return iDisplay.Factory().NewWindowGcL(*this, nativeGc, DestroyNativeGc,
                                           penCol, brushCol, *font, boundaryRect, gcScreenRect);
}

EXPORT_C CBitmapDevice& ASwtControlBase::GraphicsDevice()
{
    return *static_cast<CWsScreenDevice*>(iDisplay.CoeEnv()->SystemGc().Device());
}

EXPORT_C void ASwtControlBase::HandleUpdate()
{
    // Nothing to do for controls
}

void ASwtControlBase::DestroyNativeGc(CBitmapContext* aGc)
{
    static_cast<CWindowGc*>(aGc)->Deactivate();
    delete aGc;
}

//
// Virtual methods from MSwtControl
//
EXPORT_C TSwtPeer ASwtControlBase::Dispose()
{
    // Note that unregistration is done here rather than in the destructor.
    // The reason for it is that an object may go through destruction before
    // being registered.
    if (iParent)   // This may be NULL because of top-level Shells
    {
        iParent->UnregisterChild(*this);
    }
    // If the control has a bad parent, such as combo, table or caption control
    // UnregisterChild does not kill the focus
    if (CoeControl().IsFocused())
    {
        CoeControl().SetFocus(EFalse);
        ASSERT(!CoeControl().IsFocused());
    }

    // RemoveRef here in case iFont would be pointing to an MSwtFont
    // that is disposed in destructor of the object (e.g. iDefaultFont)
    if (iFont)
    {
        iFont->RemoveRef();
        iFont = NULL;
    }

    Invalidate(CoeControl().Rect());

    if (GetShell().ControlGoingToStack() == this)
    {
        GetShell().SetControlGoingToStack(NULL);
    }
    if (GetShell().ControlGainingFocus() == this)
    {
        GetShell().SetControlGainingFocus(NULL);
    }
    if (UiUtils().PointerCaptureControl() == this)
    {
        UiUtils().SetPointerCaptureControl(NULL);
    }
    if (UiUtils().NextFocusedControl() == this)
    {
        UiUtils().SetNextFocusedControl(NULL);
    }
    if (iDisplay.LongTapAnimationControl() == this)
    {
        iDisplay.CancelLongTapAnimation();
    }

    RemoveFromControlStack();

    TSwtPeer peer(iPeer);
    DoDelete();
    return peer;
}

EXPORT_C TSwtPeer ASwtControlBase::JavaPeer() const
{
    return iPeer;
}

EXPORT_C TUint ASwtControlBase::Flags() const
{
    return iCtrlFlags;
}

EXPORT_C void ASwtControlBase::Paint(const TRect& aRect,
                                     TBool aPaintNatively /*= ETrue*/,
                                     MSwtGc* aGc /*= NULL*/,
                                     TInt* aIntParams /*= NULL*/,
                                     TInt aIntCount /*= 0*/,
                                     HBufC* aStrParams /*= NULL*/) const
{
    // While emergency paints are required, painting is done by PaintUrgently
    // It is essential that paint events coming from Java are not served as
    // they can slow down considerably the kinetic scrolling.
    // The only reason for letting non-urgent paint controls still paint is
    // that there might be late paint event arriving after the urgent paint
    // has started as in the case of focus changes.
    MSwtShell& shell = GetShell();
    if (shell.UrgentPaintControl() == this)
    {
        return;
    }

    if (iCtrlFlags & MSwtControl::EFlagDoNotDraw)
    {
        return;
    }

    // Except Shells, Controls cannot draw over invisible parents.
    if (!GetVisibleRecursively())
    {
        return;
    }

    const CCoeControl& coeCtrl = CoeControl();
    RWindow* window = static_cast<RWindow*>(coeCtrl.DrawableWindow());
    ASSERT(window);

    TRect dirtyRect(aRect);                     // screen coordinates
    dirtyRect.Move(-window->Position());        // window coordinates
    dirtyRect.Intersection(VisibleRect(ETrue));
    if (dirtyRect.IsEmpty())
    {
        return;
    }

    CWindowGc& systemGc = coeCtrl.SystemGc();
    MSwtComposite* comp = CompositeInterface();

    window->Invalidate(dirtyRect);
    window->BeginRedraw(dirtyRect);

    // Native controls draw here
    if (aPaintNatively)
    {
        systemGc.Activate(*window);
        systemGc.Reset();
        systemGc.SetClippingRect(dirtyRect);
        DoPaint(dirtyRect);

        TBool isChildShell(EFalse);
        if (comp)
        {
            // GC intentionally reset only once for both scrollbar back and border.
            systemGc.Reset();
            systemGc.SetClippingRect(dirtyRect);
            comp->DrawScrollBarBackground(dirtyRect);

            // For child Shells, the border is drawn right after the background.
            isChildShell = IsShell() && iParent;
            if (isChildShell)
            {
                if (HasBorderStyle())
                {
                    DrawBorder(dirtyRect);
                }
            }
        }

        // GC is reset inside DrawComponentControls
        DrawComponentControls(coeCtrl, dirtyRect, systemGc, CanDrawOutsideRect());

        if (!isChildShell)
        {
            if (HasBorderStyle())
            {
                systemGc.Reset();
                systemGc.SetClippingRect(dirtyRect);
                DrawBorder(dirtyRect);
            }
        }
    }

    // Java paint listener draws here
    if (aGc)
    {
        // Workaround, corrects the clipping region. Currently without this workaround
        // if we do a BitBlt from the command buffer and the GCs origin has been set to > (0, 0)
        // then clipping region will be wrong. Origin is applied to it twice. Adding some
        // operation here, before the potential BitBlt, will correct the problem. This is
        // a platform issue in 3.2 with pre-activated graphics contexts.
#ifndef RD_SCALABLE_UI_V2
        aGc->BitmapContext().Plot(TPoint(-1,-1));
#endif //RD_SCALABLE_UI_V2

        // Clip Java GC so that it does not need to draw under children of the composite.
        CWindowGc& javaGc= static_cast<CWindowGc&>(aGc->BitmapContext());
        RRegion* javaGcClipRegion = NULL;
        TRect javaGcRect(dirtyRect);             // window coordinates
        javaGcRect.Intersection(ClientRect());   // window coordinates
        if (comp)
        {
            javaGcClipRegion = SetupChildClippingRegion(
                                   comp,
                                   javaGc,
                                   javaGcRect,
                                   -ClientRect().iTl);              // Java GC coordinates (relative to client area)
        }

        // Flush Java GC
        FlushJavaBuffer(aGc, aIntParams, aIntCount, aStrParams, window, javaGcRect);
        CancelChildClippingRegion(javaGcClipRegion, javaGc);
    }

    window->EndRedraw();

    if (aPaintNatively)
    {
        systemGc.Deactivate();
    }

#ifdef RD_JAVA_S60_RELEASE_9_2
    iDisplay.CoeEnv()->WsSession().Finish();
#endif
}

EXPORT_C void ASwtControlBase::PaintUrgently(const TRect& aRect, TBool aInitDraw /*= ETrue*/) const
{
    if (iCtrlFlags & MSwtControl::EFlagDoNotDraw)
    {
        return;
    }

    // Except Shells, Controls cannot draw over invisible parents.
    if (!GetVisibleRecursively())
    {
        return;
    }

    TRect dirtyRect(aRect);   // window coordinates
    dirtyRect.Intersection(VisibleRect(ETrue));
    if (dirtyRect.IsEmpty())
    {
        return;
    }

    const CCoeControl& coeCtrl = CoeControl();
    RWindow* window = static_cast<RWindow*>(coeCtrl.DrawableWindow());
    ASSERT(window);
    CWindowGc& systemGc = coeCtrl.SystemGc();

    if (aInitDraw)
    {
        systemGc.Activate(*window);
        window->Invalidate(dirtyRect);
        window->BeginRedraw(dirtyRect);
    }

    systemGc.Reset();
    systemGc.SetClippingRect(dirtyRect);
    DoPaint(dirtyRect);

    TBool isChildShell(EFalse);
    MSwtComposite* comp = CompositeInterface();
    if (comp)
    {
        // GC intentionally reset only once for both scrollbar back and border.
        systemGc.Reset();
        systemGc.SetClippingRect(dirtyRect);
        comp->DrawScrollBarBackground(dirtyRect);

        // For child Shells, the border is drawn right after the background.
        isChildShell = IsShell() && iParent;
        if (isChildShell)
        {
            if (HasBorderStyle())
            {
                DrawBorder(dirtyRect);
            }
        }
    }

    // GC is reset inside DrawComponentControls
    DrawComponentControls(coeCtrl, dirtyRect, systemGc, CanDrawOutsideRect());

    if (!isChildShell)
    {
        if (HasBorderStyle())
        {
            systemGc.Reset();
            systemGc.SetClippingRect(dirtyRect);
            DrawBorder(dirtyRect);
        }
    }

    if (comp)
    {
        const RSwtControlArray* children = comp->Children();
        const TInt count = children->Count();
        for (TInt i = 0; i < count; i++)
        {
            MSwtControl* child = (*children)[i];
            if (!child->IsShell())
                child->PaintUrgently(dirtyRect, EFalse);
        }
    }

    if (aInitDraw)
    {
        window->EndRedraw();
        systemGc.Deactivate();
#ifdef RD_JAVA_S60_RELEASE_9_2
        iDisplay.CoeEnv()->WsSession().Finish();
#endif
    }
}

EXPORT_C TBool ASwtControlBase::IsPaintingUrgently() const
{
    MSwtControl* urgentPaintControl = GetShell().UrgentPaintControl();
    return urgentPaintControl
           && (urgentPaintControl == this || IsDescentOf(*urgentPaintControl));
}

EXPORT_C TBool ASwtControlBase::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    const CCoeControl& coeCtrl = CoeControl();

    // Invisible or disabled control can never get the focus (not even forced)
    if (!coeCtrl.IsVisible() || coeCtrl.IsDimmed())
        return EFalse;

    MSwtComposite* parent = GetParent();

    // Go through the whole ancestor chain, if any of the ancestors is
    // disabled or invisible, return EFalse.
    while (parent)
    {
        MSwtControl* parentCtrl = parent->Control();

        if (!parentCtrl->GetEnabled() || !parentCtrl->GetVisible())
        {
            return EFalse;
        }

        if (!parentCtrl->IsShell())
        {
            parent = parentCtrl->GetParent();
        }
        else
        {
            parent = NULL;
        }
    }

    return ((aReason == KSwtFocusByForce) || !coeCtrl.IsNonFocusing());
}

EXPORT_C TBool ASwtControlBase::SetSwtFocus(TInt aReason /*=KSwtFocusByApi*/)
{
    ASSERT(!IsShell());

    CCoeControl& coeCtrl = CoeControl();
    if (coeCtrl.IsFocused())
    {
        return ETrue;
    }

    MSwtShell& shell = GetShell();
    if (!IsFocusable(aReason))
    {
        // remember the control which is not focusible because its parent shell is invisible currently
        // the control can get the focus later whenever the shell becomes visible.

        if (shell.Control()->GetVisible())
        {
            // don't need to remember the control when the control itself is a shell
            // or its parent shell is visible.
            return EFalse;
        }

        const CCoeControl& coeCtrl = CoeControl();
        if ((aReason != KSwtFocusByForce) && coeCtrl.IsNonFocusing())
        {
            // don't remember non-focusing control
            return EFalse;
        }

        // Go through the whole ancestor chain, if any of the ancestors is
        // disabled, don't remember the control.
        TBool rememberIt(ETrue);
        MSwtComposite* parent = GetParent();
        while (parent)
        {
            MSwtControl* parentCtrl = parent->Control();
            if (!parentCtrl->GetEnabled())
            {
                rememberIt = EFalse;
                break;
            }

            if (!parentCtrl->IsShell())
            {
                parent = parentCtrl->GetParent();
            }
            else
            {
                break;
            }
        }

        if (rememberIt)
        {
            shell.SetToBeFocusedControl(this);
        }

        return EFalse;
    }


    CSwtUiUtils& uiUtils = UiUtils();

    if (uiUtils.GetActiveShell()==&shell || uiUtils.CanBeActive(shell))
    {
        if (uiUtils.IsAppFocused())
        {
            // Focus if we can (Shell must be able to be active)
            coeCtrl.SetFocus(ETrue);
        }
        else
        {
            // Set next focused control
            uiUtils.SetNextFocusedControl(this);
        }
    }

    return coeCtrl.IsFocused();
}

EXPORT_C TInt ASwtControlBase::FocusBackgroundPolicy() const
{
    return EDefaultFocusBackground;
};

EXPORT_C TBool ASwtControlBase::GetEnabled() const
{
    return (CoeControl().IsDimmed()) ? EFalse : ETrue;
}

EXPORT_C TBool ASwtControlBase::GetVisible() const
{
    return !(iCtrlFlags & MSwtControl::EFlagInvisible);
}

EXPORT_C TBool ASwtControlBase::GetVisibleRecursively() const
{
    if (!IsShell() && iParent && !iParent->Control()->GetVisibleRecursively())
    {
        return EFalse;
    }
    return GetVisible();
}

EXPORT_C TBool ASwtControlBase::IsDefaultBackgroundUse() const
{
    return iBackgrdDefaultUsed;
}

EXPORT_C MAknsControlContext* ASwtControlBase::GetControlContext() const
{
    // A focus highlight without parent context is no good since the highlight
    // graphic may be semi transparent, so return CaptionedControl's highlight
    // only if no back color has been set.
    if (CaptionedControlInterface() && IsDefaultBackgroundUse())
    {
        return FocusBackgroundNow();
    }
    else
    {
        return NULL;
    }
}

EXPORT_C TRgb ASwtControlBase::GetBackground() const
{
    return iDisplay.CoeEnv()->ControlColor(EColorControlBackground, CoeControl());
}

EXPORT_C TRgb ASwtControlBase::GetForeground() const
{
    return iDisplay.CoeEnv()->ControlColor(EColorControlText, CoeControl());
}

EXPORT_C void ASwtControlBase::SetBackgroundL(const MSwtColor* aColor)
{
    DoSetBackgroundL(aColor);
    Redraw();
}

void ASwtControlBase::DoSetBackgroundL(const MSwtColor* aColor)
{
    SetColorL(EColorControlBackground, aColor);
    if (aColor)
    {
        iBackgrdDefaultUsed = EFalse;
        // Not using FocusBackgroundNow() intentionally
        if (iFocusFrame)
        {
            iFocusFrame->SetParentContext(NULL);
        }
    }
    else
    {
        iBackgrdDefaultUsed = ETrue;
        // Not using FocusBackgroundNow() intentionally
        if (iFocusFrame)
        {
            iFocusFrame->SetParentContext(SwtMopSupplyCtrlContext(EFalse));
        }
    }
}

EXPORT_C void ASwtControlBase::SetForegroundL(const MSwtColor* aColor)
{
    DoSetForegroundL(aColor);
    Redraw();
}

void ASwtControlBase::DoSetForegroundL(const MSwtColor* aColor)
{
    SetColorL(EColorControlText, aColor);
}

EXPORT_C const MSwtFont* ASwtControlBase::GetFont() const
{
    return (iFont!=NULL) ? iFont : &DefaultFont();
}

EXPORT_C TRect ASwtControlBase::GetBounds() const
{
    return TRect(GetLocation(), CoeControl().Size());
}

EXPORT_C TPoint ASwtControlBase::GetLocation() const
{
    TRect parentClientRect(iParent->Scrollable()->Control()->ClientRect()); //lint !e613
    return CoeControl().Position() - parentClientRect.iTl;
}

EXPORT_C const MSwtMenu* ASwtControlBase::GetPopupMenu() const
{
    return iPopupMenu;
}

EXPORT_C const MSwtMenu* ASwtControlBase::GetControlMenu() const
{
    return NULL;
}

EXPORT_C MSwtComposite* ASwtControlBase::GetParent() const
{
    return iParent;
}

EXPORT_C TBool ASwtControlBase::IsDescentOf(const MSwtControl& aControl) const
{
    MSwtComposite* aParent = aControl.CompositeInterface();
    if (!aParent)
    {
        return EFalse;
    }

    MSwtComposite* parent = GetParent();
    while (parent)
    {
        if (parent == aParent)
        {
            return ETrue;
        }
        parent = parent->Control()->GetParent();
    }

    return EFalse;
}

EXPORT_C MSwtShell& ASwtControlBase::GetTopShell() const
{
    const MSwtControl* ctrl = this;
    while (ctrl && ctrl->GetParent())
    {
        ctrl = ctrl->GetParent()->Control();
    }
    MSwtShell* shell = const_cast<MSwtShell*>(ctrl->ShellInterface());
    ASSERT(shell);
    return *shell;
}

EXPORT_C MSwtCaptionedControl* ASwtControlBase::GetTopCaptionedControl() const
{
    MSwtCaptionedControl* res = NULL;
    const MSwtControl* ctrl = this;
    while (ctrl)
    {
        if (ctrl->CaptionedControlInterface())
        {
            res = ctrl->CaptionedControlInterface();
        }
        ctrl = ctrl->GetParent() ? ctrl->GetParent()->Control() : NULL;
    }
    return res;
}

EXPORT_C TSize ASwtControlBase::GetWidgetSize() const
{
    return CoeControl().Size();
}

EXPORT_C void ASwtControlBase::SetWidgetSize(const TSize& aSize)
{
    ASSERT(!IsShell());

    CCoeControl& coeCtrl = CoeControl();
    TRect oldRect(coeCtrl.Rect());
    TRect newRect(coeCtrl.Position(), aSize);

    if (newRect == oldRect)
    {
        return;
    }

    MSwtShell& shell = GetShell();
    if (shell.UrgentPaintControl() == this)
    {
        shell.SetUrgentPaintControl(NULL);
    }

    TBool urgent = EFalse;
    if (iParent->ScrolledCompositeContent() == this
            && shell.UrgentPaintControl() == iParent->Control())
    {
        if (iParent->ScrolledCompositePysicsAction() != KSwtPhysicsNoAction)
        {
            // Cannot change the bounds of the content while flicking or panning
            return;
        }
        urgent = ETrue;
    }

    if (!urgent)
    {
        // Do not invalidate outside parent's visible client area.
        if (iParent)
        {
            TRect parentVisibleRect(static_cast<ASwtControlBase*>(
                                        iParent->Control())->VisibleRect());
            oldRect.Intersection(parentVisibleRect);
            newRect.Intersection(parentVisibleRect);
        }

        // Invalidate uncovered regions.
        RRegion invalidRegion;
        invalidRegion.Clear();
        invalidRegion.AddRect(oldRect);
        invalidRegion.SubRect(newRect);
        if (!invalidRegion.IsEmpty())
        {
            ASSERT(invalidRegion.Count() <= 2);
            for (TInt i = 0; i < invalidRegion.Count(); i++)
            {
                Invalidate(invalidRegion[i]);
            }
        }
        invalidRegion.Close();
    }

    // Set and redraw new rect
    coeCtrl.SetSize(aSize);
    if (!(iStyle & KSwtStyleNoRedrawResize))
    {
        if (urgent)
        {
            iParent->Control()->PaintUrgently(iParent->Control()->CoeControl().Rect());
        }
        else
        {
            Redraw();
        }
    }
}

EXPORT_C TBool ASwtControlBase::IsFocusControl()
{
    ASSERT(!IsShell());
    return CoeControl().IsFocused();
}

EXPORT_C void ASwtControlBase::MoveAbove(const MSwtControl* aControl)
{
    ASSERT(!IsShell());
    ASSERT(iParent!=NULL);
    iParent->MoveChildAbove(*this, aControl);
}

EXPORT_C void ASwtControlBase::MoveBelow(const MSwtControl* aControl)
{
    ASSERT(!IsShell());
    ASSERT(iParent!=NULL);
    iParent->MoveChildBelow(*this, aControl);
}

EXPORT_C void ASwtControlBase::Redraw()
{
    RedrawRegion(TRect(TPoint(0, 0), CoeControl().Size()));
}

// aRect is in Control coords / relative to itself!
EXPORT_C void ASwtControlBase::RedrawRegion(const TRect& aRect)
{
    if (!GetVisibleRecursively() || (iCtrlFlags & MSwtControl::EFlagDoNotDraw))
    {
        return;
    }

    CCoeControl& coeCtrl = CoeControl();
    TRect rectInWindow(aRect);
    if (!coeCtrl.OwnsWindow())
    {
        rectInWindow.Move(coeCtrl.Position());   // In window coords
    }
    TRect dirtyRect(VisibleRect(ETrue));     // In window coords
    dirtyRect.Intersection(rectInWindow);    // In window coords
    if (dirtyRect.IsEmpty())
    {
        return;
    }

    MSwtShell& shell = GetShell();
    MSwtControl* urgentPaintControl = shell.UrgentPaintControl();
    if (urgentPaintControl)
    {
        // Redraw requests outside urgently painted controls are ignored.
        if (urgentPaintControl == this || IsDescentOf(*urgentPaintControl))
        {
            PaintUrgently(dirtyRect);
        }
    }
    else
    {
        if (shell.AsyncPainting())
        {
            TRect ctrlRect(coeCtrl.Rect());
            MSwtControl* controlShell = shell.Control();
            TSwtPeer peerOfTheTargetControl(controlShell->JavaPeer());
            if (RectContainsRect(ctrlRect, dirtyRect))
            {
                peerOfTheTargetControl = JavaPeer();
            }

            dirtyRect.Move(controlShell->CoeControl().PositionRelativeToScreen());   // Screen coords
            TBool mergeable = !(iStyle & KSwtStyleNoMergePaints);
            TRAP_IGNORE(iDisplay.PostPaintEventL(JavaPeer(), peerOfTheTargetControl, dirtyRect, mergeable));
        }
        else
        {
            PaintUrgently(dirtyRect);
        }
    }
}

EXPORT_C void ASwtControlBase::SetRedraw(TBool aRedraw)
{
    if (aRedraw)
    {
        if (iStackedSetRedrawFalseNb > 0)
        {
            --iStackedSetRedrawFalseNb;
        }
    }
    else
    {
        ++iStackedSetRedrawFalseNb;
    }

    UpdateDoNotDrawFlag();

    if (!(iCtrlFlags & MSwtControl::EFlagDoNotDraw))
    {

        //For Shell the drawing are doing in Java Side.
        if (!IsShell())
        {
            // We mark this Control as needed to be redraw.
            // Redraw() is prefered instead DrawDeferred() because it directly post a
            // PaintEvent on this Control whereas DrawDeferred
            // notify the Windows server that the windows container need to be redraw.
            Redraw();
        }
    }
}

TBool ASwtControlBase::ParentHasDoNotDrawFlag()
{
    MSwtControl* parent = NULL;

    if (iParent!=NULL)
        parent = iParent->Control();

    while (parent != NULL)
    {
        if (parent->Flags() & MSwtControl::EFlagDoNotDraw)
        {
            return ETrue;
        }
        else if (!parent->GetParent() || parent->IsShell())
        {
            return EFalse;
        }
        else
        {
            parent = parent->GetParent()->Control();
        }
    }

    return EFalse;
}

EXPORT_C TBool ASwtControlBase::HasDoNotDrawFlag() const
{
    return (iCtrlFlags & MSwtControl::EFlagDoNotDraw);
}

EXPORT_C void ASwtControlBase::UpdateDoNotDrawFlag()
{
    if (iStackedSetRedrawFalseNb == 0 && !ParentHasDoNotDrawFlag())
    {
        iCtrlFlags &= ~MSwtControl::EFlagDoNotDraw;
    }
    else
    {
        iCtrlFlags |= MSwtControl::EFlagDoNotDraw;
    }
}

EXPORT_C void ASwtControlBase::SetFontL(const MSwtFont* aFont)
{
    DoSetFontL(aFont);
    Redraw();
}

void ASwtControlBase::DoSetFontL(const MSwtFont* aFont)
{
    if (iFont != NULL)
        iFont->RemoveRef();
    iFont = aFont;
    if (iFont != NULL)
        iFont->AddRef();
}

EXPORT_C void ASwtControlBase::SetVisible(TBool aVisible)
{
    CCoeControl& coeCtrl = CoeControl();
    MSwtShell& shell = GetShell();
    TBool redrawFocusedCtrl = (&shell == UiUtils().GetActiveShell())
                              && (shell.FocusControl() != this);
    TInt oldCount = 0;
    TInt newCount = 0;

    if (redrawFocusedCtrl)
    {
        oldCount = shell.Composite()->CountFocusableChildren(2);
    }

    if (aVisible)
    {
        iCtrlFlags &= ~MSwtControl::EFlagInvisible;
    }
    else
    {
        if (shell.UrgentPaintControl() == this)
        {
            shell.SetUrgentPaintControl(NULL);
        }
    }

    coeCtrl.MakeVisible(aVisible);

    if (!aVisible)
        iCtrlFlags |= MSwtControl::EFlagInvisible;

    if (aVisible)
    {
        Redraw();
    }
    else
    {
        Invalidate(coeCtrl.Rect());
    }

    if (!IsShell())
    {
        FocusabilityChanged();
    }

    if (redrawFocusedCtrl)
    {
        newCount = shell.Composite()->CountFocusableChildren(2);
    }

    if (redrawFocusedCtrl
            && ((oldCount >= 2 && newCount == 1)
                || (oldCount == 1 && newCount >= 2)))
    {
        MSwtControl* focusedCtrl = GetShell().FocusControl();
        if (focusedCtrl)
        {
            static_cast<ASwtControlBase*>(focusedCtrl)->RedrawFocusBackground();
        }
    }
}

EXPORT_C void ASwtControlBase::SetLocation(const TPoint& aPoint)
{
    ASSERT(!IsShell());

    TPoint oldLocation(GetLocation());
    TPoint newLocation = ComputeLocationToSet(aPoint);
    CCoeControl& coeCtrl = CoeControl();
    TRect oldRect(coeCtrl.Rect());
    TRect newRect(newLocation, coeCtrl.Size());

    if (oldLocation == newLocation)
    {
        return;
    }

    MSwtShell& shell = GetShell();
    if (shell.UrgentPaintControl() == this)
    {
        shell.SetUrgentPaintControl(NULL);
    }

    TBool urgent = EFalse;
    if (iParent->ScrolledCompositeContent() == this
            && shell.UrgentPaintControl() == iParent->Control())
    {
        if (iParent->ScrolledCompositePysicsAction() != KSwtPhysicsNoAction)
        {
            // Cannot change the bounds of the content while flicking or panning
            return;
        }
        urgent = ETrue;
    }

    if (!urgent)
    {
        // Do not invalidate outside parent's visible client area.
        if (iParent)
        {
            TRect parentVisibleRect(static_cast<ASwtControlBase*>(
                                        iParent->Control())->VisibleRect());
            oldRect.Intersection(parentVisibleRect);
            newRect.Intersection(parentVisibleRect);
        }

        // Invalidate uncovered regions.
        if (newRect.Intersects(oldRect))
        {
            RRegion invalidRegion;
            invalidRegion.Clear();
            invalidRegion.AddRect(oldRect);
            invalidRegion.SubRect(newRect);
            ASSERT(invalidRegion.Count() <= 2);
            for (TInt i = 0; i < invalidRegion.Count(); i++)
            {
                Invalidate(invalidRegion[i]);
            }
            invalidRegion.Close();
        }
        else
        {
            Invalidate(oldRect);
        }
    }

    // Set and redraw new rect
    coeCtrl.SetPosition(newLocation);
    if (!(iStyle & KSwtStyleNoRedrawResize))
    {
        if (urgent)
        {
            iParent->Control()->PaintUrgently(iParent->Control()->CoeControl().Rect());
        }
        else
        {
            Redraw();
        }
    }
}

EXPORT_C void ASwtControlBase::SetEnabled(TBool aEnabled)
{
    CCoeControl& coeCtrl = CoeControl();
    MSwtShell& shell = GetShell();
    TBool redrawFocusedCtrl = (&shell == UiUtils().GetActiveShell())
                              && (shell.FocusControl() != this);
    TInt oldCount = 0;
    TInt newCount = 0;

    if (redrawFocusedCtrl)
    {
        oldCount = shell.Composite()->CountFocusableChildren(2);
    }

    if (aEnabled)
    {
        iCtrlFlags &= ~MSwtControl::EFlagDisabled;
    }

    coeCtrl.SetFocusing(aEnabled);
    coeCtrl.SetDimmed(!aEnabled);

    if (!aEnabled)
        iCtrlFlags |= MSwtControl::EFlagDisabled;

    if (!IsShell())
    {
        // If we got disabled this will find a new focused item.
        FocusabilityChanged();
    }

    Redraw();

    if (redrawFocusedCtrl)
    {
        newCount = shell.Composite()->CountFocusableChildren(2);
    }

    if (redrawFocusedCtrl
            && ((oldCount >= 2 && newCount == 1)
                || (oldCount == 1 && newCount >= 2)))
    {
        MSwtControl* focusedCtrl = GetShell().FocusControl();
        if (focusedCtrl)
        {
            static_cast<ASwtControlBase*>(focusedCtrl)->RedrawFocusBackground();
        }
    }
}

EXPORT_C void ASwtControlBase::SetBounds(const TRect& aRect)
{
    // Must NOT call SetExtent if we want component controls to be relocated.
    ASSERT(!IsShell());

    CCoeControl& coeCtrl = CoeControl();
    TSize oldSize(coeCtrl.Size());
    TSize newSize(aRect.Size());
    TPoint oldLocation(GetLocation());
    TPoint newLocation(ComputeLocationToSet(aRect.iTl));
    TRect oldRect(coeCtrl.Rect());
    TRect newRect(newLocation, newSize);

    if (oldLocation == newLocation && oldSize == newSize)
    {
        return;
    }

    MSwtShell& shell = GetShell();
    if (shell.UrgentPaintControl() == this)
    {
        shell.SetUrgentPaintControl(NULL);
    }

    TBool urgent = EFalse;
    if (iParent->ScrolledCompositeContent() == this
            && shell.UrgentPaintControl() == iParent->Control())
    {
        if (iParent->ScrolledCompositePysicsAction() != KSwtPhysicsNoAction)
        {
            // Cannot change the bounds of the content while flicking or panning
            return;
        }
        urgent = ETrue;
    }

    if (!urgent)
    {
        // Do not invalidate outside parent's visible client area.
        if (iParent)
        {
            TRect parentVisibleRect(static_cast<ASwtControlBase*>(
                                        iParent->Control())->VisibleRect());
            oldRect.Intersection(parentVisibleRect);
            newRect.Intersection(parentVisibleRect);
        }

        // Invalidate uncovered regions.
        if (newRect.Intersects(oldRect))
        {
            RRegion invalidRegion;
            invalidRegion.Clear();
            invalidRegion.AddRect(oldRect);
            invalidRegion.SubRect(newRect);
            ASSERT(invalidRegion.Count() <= 4);
            for (TInt i = 0; i < invalidRegion.Count(); i++)
            {
                Invalidate(invalidRegion[i]);
            }
            invalidRegion.Close();
        }
        else
        {
            Invalidate(oldRect);
        }
    }

    // Set and invalidate new rect
    if (oldLocation != newLocation)
    {
        coeCtrl.SetPosition(newLocation);
    }
    if (oldSize != newSize)
    {
        coeCtrl.SetSize(newSize);
    }

    if (!(iStyle & KSwtStyleNoRedrawResize))
    {
        if (urgent)
        {
            iParent->Control()->PaintUrgently(iParent->Control()->CoeControl().Rect());
        }
        else
        {
            Redraw();
        }
    }
}

EXPORT_C void ASwtControlBase::SetPopupMenuL(const MSwtMenu* aMenu)
{
    if (aMenu)
    {
        GetShell().InstallCba();
    }

    const MSwtMenu* oldMenu = iPopupMenu;
    iPopupMenu = aMenu;

    if (iPopupMenu == NULL && oldMenu != NULL)
    {
        iDisplay.MenuArranger().HandleMenuChangedL(*oldMenu, MSwtMenuArranger::ESwtMenuRemoved);
    }
    else if (iPopupMenu != NULL)
    {
        iDisplay.MenuArranger().HandleMenuChangedL(*iPopupMenu, MSwtMenuArranger::ESwtMenuAdded);
    }
}

EXPORT_C void ASwtControlBase::SetCapture(TBool aCapture)
{
    if (aCapture)
    {
        UiUtils().SetPointerCaptureControl(this);
    }
    else
    {
        if (UiUtils().PointerCaptureControl() == this)
        {
            UiUtils().SetPointerCaptureControl(NULL);
        }
    }
}

EXPORT_C TInt ASwtControlBase::Style() const
{
    return iStyle;
}

EXPORT_C void ASwtControlBase::Update() const
{
    if (iCtrlFlags & MSwtControl::EFlagDoNotDraw)
        return;
}

EXPORT_C void ASwtControlBase::ComputeTabListL(RSwtControlArray& aList)
{
    User::LeaveIfError(aList.Append(this));
}

EXPORT_C TPoint ASwtControlBase::ToControl(const TPoint& aPoint) const
{
    const TPoint clientOriginInScreen(WindowToScreen(ClientRect().iTl));
    return aPoint - clientOriginInScreen;
}

EXPORT_C TPoint ASwtControlBase::ToDisplay(const TPoint& aPoint) const
{
    const TPoint clientOriginInScreen(WindowToScreen(ClientRect().iTl));
    return aPoint + clientOriginInScreen;
}

EXPORT_C TBool ASwtControlBase::CbaButtonIsUsed(TInt /*aCbaButtonIndex*/) const
{
    return EFalse;
}

EXPORT_C TBool ASwtControlBase::IsKeyUsed(TUint /*aKeyCode*/) const
{
    // By default, we assume that the control doesn't use key
    return EFalse;
}

EXPORT_C TBool ASwtControlBase::MSKSelCmdEnabled() const
{
    return EFalse;
}

EXPORT_C TBool ASwtControlBase::ForceTraverseEvent(TSwtTraversal aDetail, TBool aDoIt)
{
    TRAPD(ignore, iDisplay.ForceTraverseEventL(*this, aDetail, aDoIt));
    return (ignore == KErrNone);
}

EXPORT_C void ASwtControlBase::AddToControlStackL(TInt aPriority)
{
    if (!iIsOnControlStack)
    {
        iIsOnControlStack = ETrue;
        TRAPD(error, iDisplay.CoeEnv()->EikAppUi()->AddToStackL(&CoeControl(), aPriority));
        if (error!=KErrNone)
        {
            iIsOnControlStack = EFalse;
            User::Leave(error);
        }
        if (!IsShell())
        {
            GetShell().SetControlGoingToStack(this);
            GetShell().SetControlGainingFocus(NULL);
        }
    }
}
EXPORT_C HBufC* ASwtControlBase::MSKLabelL() const
{
    return KNullDesC().Alloc();
}

EXPORT_C void ASwtControlBase::RemoveFromControlStack()
{
    if (iIsOnControlStack)
    {
        iDisplay.CoeEnv()->EikAppUi()->RemoveFromStack(&CoeControl());
        iIsOnControlStack = EFalse;
        if (!IsShell())
        {
            if (
                iDisplay.MenuArranger().EikMenuBar()->IsDisplayed() ||
                iDisplay.CoeEnv()->EikAppUi()->IsDisplayingControlBetweenPriorities(
                    ECoeStackPriorityMenu, ECoeStackPriorityCba))
            {
                GetShell().SetControlGainingFocus(this);
            }
        }
    }
}

EXPORT_C TBool ASwtControlBase::IsOnControlStack() const
{
    return iIsOnControlStack;
}

#ifdef RD_SCALABLE_UI_V2
EXPORT_C void ASwtControlBase::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    CoeControl().HandlePointerEventL(aPointerEvent);
    PostMouseEventL(aPointerEvent);
}
#else
EXPORT_C void ASwtControlBase::HandlePointerEventL(const TPointerEvent& /*aPointerEvent*/)
{
    ASSERT(EFalse);
}
#endif

#ifdef RD_SCALABLE_UI_V2
EXPORT_C TBool ASwtControlBase::IsLongTapAnimationCandidate(const TPointerEvent& aPointerEvent) const
{
    return (ClientRect().Contains(aPointerEvent.iPosition)
            && iDisplay.CommandArranger()->ControlHasStylusPopupMenu(*this));
}
#else
EXPORT_C TBool ASwtControlBase::IsLongTapAnimationCandidate(const TPointerEvent& /*aPointerEvent*/) const
{
    return EFalse;
}
#endif

TBool ASwtControlBase::IsFocusedOrChildIsFocused() const
{
    return (GetShell().FocusControl() == this) || ChildIsFocused();
}

TBool ASwtControlBase::ChildIsFocused() const
{
    MSwtControl* focusedControl = GetShell().FocusControl();
    return focusedControl && focusedControl->IsDescentOf(*this);
}

EXPORT_C void ASwtControlBase::DoControlSpecificFeedback(
    const TBool& /*aFirstTap*/, const TBool& /*aTappedToChildRect*/,
    const TPointerEvent& /*aPointerEvent*/) const
{
    //Do nothing here. Each child class will implement
    //this function according to its needs.
    //Note that this function is not called by platform.
}
