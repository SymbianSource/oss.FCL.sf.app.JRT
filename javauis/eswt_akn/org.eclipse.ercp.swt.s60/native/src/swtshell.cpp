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


#include <AknDef.h>
#include <AknsBasicBackgroundControlContext.h>
#include <swtlaffacade.h>
#ifdef RD_JAVA_S60_RELEASE_9_2
#include <gfxtranseffect/gfxtranseffect.h>
#include <akntransitionutils.h>
#endif // RD_JAVA_S60_RELEASE_9_2
#include <coreuiavkoneswt.h>

#include "eswtmobileextensions.h"
#include "swtuiutils.h"
#include "swtshell.h"


// Main pane varieties
static const TInt KSwtMainPainVarietyClassic = 3;
static const TInt KSwtMainPainVarietySmallSpLandscape = 9;
static const TInt KSwtMainPainVarietySmallSpLandscapePen = 4;

#ifdef RD_JAVA_S60_RELEASE_9_2
static const TInt KSwtRoundCornerBgColorDiff = 50;
#endif // RD_JAVA_S60_RELEASE_9_2


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtShell::NewL
// ---------------------------------------------------------------------------
//
CSwtShell* CSwtShell::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtShell* aParent, TInt aStyle)
{
    CSwtShell* self = new(ELeave) CSwtShell(aDisplay, aPeer,
                                            aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtShell::CSwtShell
// ---------------------------------------------------------------------------
//
CSwtShell::CSwtShell(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                     MSwtShell* aParent, TInt aStyle)
        : CSwtDecorations(aDisplay, aPeer,
                          aParent ? aParent->Composite() : NULL, aStyle, EFalse, EFalse)
        , iMinSize(-1, -1)
        , iSavedStyle(aStyle)
        , iHasCba(aParent == NULL)
{
    SetFocusing(EFalse);
    // Top-level Shells are considered as always maximised
    if (!iParent)
    {
        iIsMaximized = ETrue;
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::~CSwtShell
// ---------------------------------------------------------------------------
//
CSwtShell::~CSwtShell()
{
    // Do all the destruction in DoDelete().
}

// ---------------------------------------------------------------------------
// CSwtShell::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtShell::ConstructL()
{
    // Create the window
    //
    // When created, the window does not inherit the control's visibility.
    // The workaround is to make the control visible, create the window and
    // finally make the control invisible again.

    // WARNING! Avkon expects window owning parent control to be always ready
    // to draw! See CCoeControl::DrawNow(). This might cause various problems!
    CCoeControl::MakeVisible(ETrue);
    CreateWindowL();
    CCoeControl::MakeVisible(EFalse);

    // Shells are by default invisible
    iCtrlFlags |= MSwtControl::EFlagInvisible;

    // Essential for clearing up tactile feedback when Button gets invisible.
    SetMopParent(iEikonEnv->EikAppUi());

    // The rect must be in window coordinates, not screen
    CSwtDecorations::ConstructL();

    // Set priority based on the shell style. See TSwtWinPriority.
    // 0 ordinal position means highest z position.
    TInt ordinalPos(1);
    if (!iParent)
    {
        ordinalPos = 0;
    }
    RWindow& window = Window();
    if (iStyle & KSwtStyleOnTop)
    {
        iWindowPriority = ESwtWinPriorityDefault;
    }
    else
    {
        iWindowPriority = ESwtWinPriorityShell;
    }
    window.SetOrdinalPosition(ordinalPos, iWindowPriority);

#ifdef RD_SCALABLE_UI_V2
    window.SetPointerGrab(ETrue);
#endif //RD_SCALABLE_UI_V2

    DoSetRect(DefaultBounds());
    SetMinimumSize(TSize());
    UiUtils().RegisterShellL(*this);

#ifdef RD_SCALABLE_UI_V2
    if ((iStyle & KSwtStyleSystemModal) != 0 ||
            (iStyle & KSwtStyleApplicationModal) != 0)
    {
        CCoeControl::SetGloballyCapturing(ETrue);
        CCoeControl::SetPointerCapture(ETrue);
    }
#endif //RD_SCALABLE_UI_V2

#ifdef RD_JAVA_S60_RELEASE_9_2
    GfxTransEffect::Enable();
    GfxTransEffect::Register(this,
                             iParent ? KGfxOptionsMenuControlUid : KGfxInformationNoteControlUid, EFalse);

    // Window transparency cannot be enabled as it breaks the async drawing.
    // Since transparency cannot be enabled, set the initial background
    // color to something closer to the theme's background.
    if (iParent)
    {
        TRgb bgColor = iDisplay.UiUtils().GetSystemColor(ESwtColorWidgetBackground);

        TInt r = bgColor.Green();
        r -= KSwtRoundCornerBgColorDiff;
        if (r < 0)
            r = 0;

        TInt g = bgColor.Green();
        g -= KSwtRoundCornerBgColorDiff;
        if (g < 0)
            g = 0;

        TInt b = bgColor.Blue();
        b -= KSwtRoundCornerBgColorDiff;
        if (b < 0)
            b = 0;

        bgColor.SetRed(b);
        bgColor.SetGreen(b);
        bgColor.SetBlue(b);

        OverrideColorL(EColorControlBackground, bgColor);
    }
#endif // RD_JAVA_S60_RELEASE_9_2
}

// ---------------------------------------------------------------------------
// CSwtShell::GetTopShell
// ---------------------------------------------------------------------------
//
MSwtShell& CSwtShell::GetTopShell() const
{
    return UiUtils().GetControlTopShell(*this);
}

// ---------------------------------------------------------------------------
// CSwtShell::RemoveAndRememberFocus
// ---------------------------------------------------------------------------
//
void CSwtShell::RemoveAndRememberFocus()
{
    ASSERT(iFocusControl && iFocusControl->CoeControl().IsFocused());
    ASSERT(!iFocusMemory);

    iFocusMemory = iFocusControl;
    // This sets iFocusControl to NULL
    iFocusControl->CoeControl().SetFocus(EFalse);

    ASSERT(!iFocusControl);
}

// ---------------------------------------------------------------------------
// CSwtShell::DoSetRect
// Note that calling SetRect on the Shell does not cause a PositionChanged!
// ---------------------------------------------------------------------------
//
void CSwtShell::DoSetRect(const TRect& aRect)
{
    // Divert the job to UiUtils if this is the Shell of an editor open for
    // split view editing.
    MSwtUiUtils& utils = iDisplay.UiUtils();
    MSwtControl* splitInputView = utils.SplitInputView();
    if (splitInputView && (&(splitInputView->GetShell())) == this)
    {
        utils.SetSplitInputShellPos(aRect.iTl);
        CCoeControl::SetSize(aRect.Size());
    }
    else
    {
        CCoeControl::SetRect(aRect);
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SwtHandleResourceChangeL
// From CSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtShell::SwtHandleResourceChangeL(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        if (!iParent)
        {
            CAknsBasicBackgroundControlContext* bg =
                static_cast<CAknsBasicBackgroundControlContext*>(GetControlContext());
            if (bg)
            {
                bg->SetRect(iDisplay.Device().Bounds());
            }
            DoSetRect(DefaultBounds());
        }
    }

    CSwtDecorations::SwtHandleResourceChangeL(aType);
}

// ---------------------------------------------------------------------------
// CSwtShell::CSwtShell
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
void CSwtShell::DoPaint(const TRect& aRect) const
{
    // Set Draw called by eSWT drawing framework flag
    const_cast<CSwtShell *>(this)->iDrawCalledByOurFrwk = ETrue;

    // Restore title visibility so that it is drawn. See Draw method
    if (AsyncPainting())
    {
        if (iTitle)
        {
            iTitle->MakeVisible(this->IsVisible());
        }
    }
    CoeControl().DrawBackground(aRect);

    CSwtComposite::Draw(aRect);

    // Additional drawing code is to be added here instead of Draw()

    // Unset Draw called by eSWT drawing framework flag
    const_cast<CSwtShell *>(this)->iDrawCalledByOurFrwk = EFalse;
}

// ---------------------------------------------------------------------------
// CSwtShell::BorderInnerRect
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
TRect CSwtShell::BorderInnerRect() const
{
    if (!iParent)
    {
        return CSwtComposite::BorderInnerRect();
    }
    else
    {
        return CSwtDecorations::BorderInnerRect();
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::BorderOuterRect
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
TRect CSwtShell::BorderOuterRect(const TRect& aRect) const
{
    if (!iParent)
    {
        return CSwtComposite::BorderOuterRect(aRect);
    }
    else
    {
        return CSwtDecorations::BorderOuterRect(aRect);
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::DoDelete
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
void CSwtShell::DoDelete()
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    GfxTransEffect::Deregister(this);
#endif // RD_JAVA_S60_RELEASE_9_2

    CSwtUiUtils& uiUtils = UiUtils();
    if (uiUtils.GetActiveShell() == this)
    {
        iDisplay.MenuArranger().ActiveShellLost();
        // Notify Command arranger
        MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
        // commandArranger may be null as Mobile Extensions is optional
        if (commandArranger)
        {
            commandArranger->ActiveShellLost();
        }
    }

    uiUtils.UnregisterShell(*this);
    iFullTabList.Close();
    delete iTitleText;
    iTitleText = NULL;

    iFocusControl = NULL;
    iPrevFocusControl = NULL;
    iFocusMemory = NULL;
    iDefaultCommand = NULL;
    iControlGoingToStack= NULL;
    iControlGainingFocus= NULL;
    iTitleText = NULL;

    // Last step must always be this!
    UiUtils().GarbageCollect(*this);
}

// ---------------------------------------------------------------------------
// CSwtShell::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtShell::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                       TEventCode aType)
{
    // Offer key event to command arranger first
    TKeyResponse keyResponse = OfferKeyEventToCommandAndMenuArrangersL(aKeyEvent, aType);
    if (keyResponse != EKeyWasConsumed)
    {
        // Now to the tab group.
        // Tab groups are usually handled in HandleHeyL but this call
        // is needed for cases where there are no focusable controls
        keyResponse = OfferKeyEventToTabGroupL(aKeyEvent, aType);
    }
    if (keyResponse == EKeyWasConsumed)
    {
        return EKeyWasConsumed;
    }

    return CSwtDecorations::OfferKeyEventL(aKeyEvent, aType);
}

// ---------------------------------------------------------------------------
// CSwtShell::HandlePointerEventL
// This is the entry point for all pointer events targeted at any of the children
// of this Shell except for Browser's case.
// From CCoeControl
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtShell::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    CSwtUiUtils& utils = UiUtils();

    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        utils.SetNaviKeyInput(EFalse);

        // Ignore pointer events outside modal Shells.
        MSwtShell *activeShell = utils.GetActiveShell();
        if (!activeShell || (activeShell && activeShell != this &&
                             !((activeShell->Control()->Style() & KSwtStylePrimaryModal) != 0
                               && activeShell->Control()->IsDescentOf(*this))))
        {
            utils.SetActiveShell(*this, ETrue);
        }
    }

    MSwtControl* capturingControl = utils.PointerCaptureControl();
    if (capturingControl && capturingControl != this)
    {
        if (aPointerEvent.iType == TPointerEvent::EButton1Down
                && capturingControl->IsFocusable())
        {
            capturingControl->SetSwtFocus(KSwtFocusByPointer);
        }
        capturingControl->HandlePointerEventL(aPointerEvent);
        iDisplay.TryDetectLongTapL(aPointerEvent);
        return;
    }

    if (!iDisplay.RevertPointerEvent())
    {
        CSwtComposite::HandlePointerEventL(aPointerEvent);
    }

    iDisplay.TryDetectLongTapL(aPointerEvent);
}
#else
void CSwtShell::HandlePointerEventL(const TPointerEvent& /*aPointerEvent*/)
{
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtShell::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtShell::MakeVisible(TBool aVisible)
{
    // Remove focus from its owner before making it invisible
    if (!aVisible && iFocusControl)
    {
        RemoveAndRememberFocus();
    }

    if (aVisible && !iBgContext)
    {
        TRAP_IGNORE(CreateBackgroundL());
    }

    CSwtUiUtils& utils = UiUtils();

#ifdef RD_JAVA_S60_RELEASE_9_2
    TBool doEffect = (IsVisible() != aVisible) && (iDisplay.IsUiReady() || iParent);
    if (doEffect)
    {
        GfxTransEffect::Begin(this, aVisible ? KGfxControlAppearAction : KGfxControlDisappearAction);
        GfxTransEffect::SetDemarcation(this, Rect());
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    CSwtDecorations::MakeVisible(aVisible);

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (doEffect)
    {
        GfxTransEffect::End(this);
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    utils.ShellActivabilityChanged(*this);

    // Done already if the Shell became active
    if (aVisible && utils.GetActiveShell() != this)
    {
        if (!iFullScreen && HasCba())
        {
            utils.SetCbaVisible(ETrue);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtShell::SetDimmed(TBool aDimmed)
{
    CSwtDecorations::SetDimmed(aDimmed);
    UiUtils().ShellActivabilityChanged(*this);
}

// ---------------------------------------------------------------------------
// CSwtShell::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtShell::FocusChanged(TDrawNow /*aDrawNow*/)
{
    // Shells cannot gain ( nor lose ) focus, they can only be active but we may
    // still end up here because of the app UI's AddToStackL().
    if (IsFocused())
    {
        CCoeControl::SetFocus(EFalse);
        if (iControlGoingToStack)
        {
            // This control child is added to control stack,
            // the Symbian framework will manages its focus.
            // When the SWT application goes to background,
            // Symbian will unfocus the control child.
            // The SWT application still runs,
            // and the fact that the control child is unfocused could perhaps
            // cause some conflicts in this SWT native implementation.
            TRAP_IGNORE(iControlGoingToStack->AddToControlStackL(
                            ECoeStackPriorityDefault));
        }
        if (iControlGainingFocus)
        {
            iControlGainingFocus->SetSwtFocus();
            iControlGainingFocus = NULL;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::Draw
// From CCoeControl
// Draw (additional drawing must be placed in DoPaint(), cannot be put here)
// Areas inside aRect which are not drawn will turn black or transparent in 9.2!
// ---------------------------------------------------------------------------
//
void CSwtShell::Draw(const TRect& aRect) const
{
    if (iCtrlFlags & EFlagDoNotDraw)
    {
        return;
    }

    if (iUrgentPaintControl)
    {
        const TRect rect(iUrgentPaintControl->CoeControl().Rect());
        if (aRect.iTl.iX < rect.iTl.iX
                || aRect.iTl.iY < rect.iTl.iY
                || aRect.iBr.iX > rect.iBr.iX
                || aRect.iBr.iY > rect.iBr.iY)
        {
            // We cannot afford not to draw around the urgently painted control,
            // therefore draw the whole Shell urgently.
            ASwtControlBase::PaintUrgently(aRect, EFalse);
        }
        else
        {
            // The urgently painted control and its children are drawn exclusively.
            iUrgentPaintControl->PaintUrgently(aRect, EFalse);
        }
        FinishRedraw();
    }
    else if (CoeControl().DrawableWindow()->IsFaded())
    {
        // This is to fix horible flickering in faded lists,
        // when menus or modal Shells are open partially on top.
        ASwtControlBase::PaintUrgently(aRect, EFalse);
        FinishRedraw();
    }
    else
    {
        if (AsyncPainting())
        {
            // Wait for eSWT drawing framework before drawing title so that everything
            // is drawn at the same time (don't draw title when it is Symbian drawing
            // framework that is calling Draw method )
            if (iTitle && this->IsVisible())
            {
                iTitle->MakeVisible(EFalse);
            }

            // Post a paint event
            TRect paintRect(Rect());
            paintRect.Intersection(aRect);   // In window coords
            if (!paintRect.IsEmpty())
            {
                TBool mergeable = !(iStyle & KSwtStyleNoMergePaints);
                paintRect.Move(iPosition);   // In screen coords
                TRAP_IGNORE(iDisplay.PostPaintEventL(iPeer, iPeer, paintRect, mergeable));
            }
        }
        else
        {
            // Extra attention must be payed to the fact that after ::Draw,
            // ::DrawComponentControls is called. This means that the eSWT
            // children will be drawn before the "native" children, like
            // scrollbars, title panes, etc. Watch out for overlapping.
            ASwtControlBase::PaintUrgently(aRect, EFalse);
            FinishRedraw();
        }
    }
}

void CSwtShell::SizeChanged()
{
    CSwtDecorations::SizeChanged();
    MSwtUiUtils& utils = iDisplay.UiUtils();
    MSwtControl* splitInputView = utils.SplitInputView();
    if (splitInputView && (&(splitInputView->GetShell())) == this)
    {
        utils.AdjustSplitInputShellPos();
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::Draw
// From MCoeControlBackground
// ---------------------------------------------------------------------------
//
void CSwtShell::Draw(CWindowGc& aGc, const CCoeControl& aControl,
                     const TRect& aRect) const
{
    // Wait for eSWT drawing framework before drawing background so that
    // everything is drawn at the same time (don't draw background when it is
    // Symbian drawing framework that is calling Draw method)
    if (AsyncPainting())
    {
        if (iUrgentPaintControl || iDrawCalledByOurFrwk)
        {
            CSwtDecorations::Draw(aGc, aControl, aRect);
        }
    }
    else
    {
        CSwtDecorations::Draw(aGc, aControl, aRect);
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::IsFocusable
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtShell::IsFocusable(TInt /*aReason*/) const
{
    // Shells never get the CoeControl focus, just active
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtShell::SetSwtFocus
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtShell::SetSwtFocus(TInt aReason /*=KSwtFocusByApi*/)
{
    if (aReason != KSwtFocusByForce)
    {
        // Pass focus to children
        CSwtComposite::SetSwtFocus(aReason);
    }

    if (UiUtils().SetActiveShell(*this, ETrue))
    {
        if ((aReason == KSwtFocusByForce) && iFocusControl)
        {
            RemoveAndRememberFocus();
        }
        return ETrue;
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtShell::GetVisible
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtShell::GetVisible() const
{
    return CCoeControl::IsVisible();
}

// ---------------------------------------------------------------------------
// CSwtShell::GetBounds
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRect CSwtShell::GetBounds() const
{
    // A Shell must return screen coordinates. A Shell is window owning and, as
    // such, iPosition is its window's position in screen coordinates.
    return TRect(iPosition, iSize);
}

// ---------------------------------------------------------------------------
// CSwtShell::GetLocation
// From MSwtControl
// ---------------------------------------------------------------------------
//
TPoint CSwtShell::GetLocation() const
{
    // A Shell must return screen coordinates. A Shell is window owning and,
    // as such, iPosition is its window's position in screen coordinates.
    return iPosition;
}

// ---------------------------------------------------------------------------
// CSwtShell::Redraw
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::Redraw()
{
    if (!(iCtrlFlags & EFlagDoNotDraw))
    {
        CSwtDecorations::Redraw();
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetLocation
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::SetLocation(const TPoint& aPoint)
{
    // Maximised Shells cannot be moved
    if (iIsMaximized || !iParent)
    {
        return;
    }

    DoSetLocation(aPoint);
}

// ---------------------------------------------------------------------------
// CSwtShell::SetWidgetSize
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::SetWidgetSize(const TSize& aSize)
{
    // Maximised Shells cannot be moved
    if (iIsMaximized || !iParent)
    {
        return;
    }

    TSize checkedSize(Max(aSize.iWidth, iMinSize.iWidth),
                      Max(aSize.iHeight, iMinSize.iHeight));

    if (Size() != checkedSize)
    {
        SetSize(checkedSize);

        // Drawing only after everything else.
        if (!(iStyle & KSwtStyleNoRedrawResize))
        {
            Redraw();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetBounds
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::SetBounds(const TRect& aRect)
{
    // Maximised Shells cannot be moved
    if (iIsMaximized || !iParent)
    {
        return;
    }

    TBool changed = EFalse;
    TSize checkedSize(Max(aRect.Size().iWidth, iMinSize.iWidth),
                      Max(aRect.Size().iHeight, iMinSize.iHeight));
    if (Size() != checkedSize)
    {
        SetSize(checkedSize);
        changed = ETrue;
    }

    // Divert the job to UiUtils if this is the Shell of an editor open for
    // split view editing.
    MSwtUiUtils& utils = iDisplay.UiUtils();
    MSwtControl* splitInputView = utils.SplitInputView();
    if (splitInputView && (&(splitInputView->GetShell())) == this)
    {
        utils.SetSplitInputShellPos(aRect.iTl);
    }
    else
    {
        if (aRect.iTl != iPosition)
        {
            iPosition = aRect.iTl;
            Window().SetPosition(aRect.iTl);
            PositionChanged();
            changed = ETrue;
        }

        // No need to recurse on the children, their positions within the
        // window do not change
    }

    if (changed && !(iStyle & KSwtStyleNoRedrawResize))
    {
        Redraw();
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::IsFocusControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtShell::IsFocusControl()
{
    if ((UiUtils().GetActiveShell() == this) && (!iFocusControl))
    {
        return ETrue;
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtShell::MoveAbove
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::MoveAbove(const MSwtControl* aControl)
{
    if (iParent)
    {
        iParent->MoveChildAbove(*this, aControl);
    }

    if (!aControl || aControl->IsShell())
    {
        TInt newPosition;
        if (aControl)
        {
            // Use OrdinalPosition() instead of FullOrdinalPosition() since
            // FullOrdinalPosition() can be called only when the window
            // priorities are not changed inside a window group. See
            // TSwtWinPriority
            TInt paramPosition = aControl->CoeControl().DrawableWindow()->
                                 OrdinalPosition();
            newPosition = (paramPosition > 0) ? paramPosition - 1 : 0;
        }
        else
        {
            newPosition = 0;
        }

        SetZPosition(newPosition);
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::MoveBelow
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::MoveBelow(const MSwtControl* aControl)
{
    if (iParent)
    {
        iParent->MoveChildBelow(*this, aControl);
    }

    if (!aControl || aControl->IsShell())
    {
        TInt newPosition;
        if (aControl)
        {
            // Use OrdinalPosition() instead of FullOrdinalPosition() since
            // FullOrdinalPosition() can be called only when the window
            // priorities are not changed inside a window group. See
            // TSwtWinPriority
            newPosition = aControl->CoeControl().DrawableWindow()->
                          OrdinalPosition() + 1;
        }
        else
        {
            newPosition = -1;
        }

        SetZPosition(newPosition);
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::HideSilently
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::HideSilently()
{
    iSilentlyHidden = ETrue;
    RWindow& window = Window();
    // It may seem odd that 0 is used here since 0 means closest to the screen.
    // Ordinal position -1 creates some flickering quirks witht the status pane.
    // It is ok to use 0 as the ordinal priority is the one that really counts.
    window.SetOrdinalPosition(0, ESwtWinPriorityInactiveTopShell);
    const TInt childCount = iChildren.Count();
    for (TInt i = 0; i < childCount; i++)
    {
        MSwtControl* child = iChildren[i];
        if (child->IsShell())
        {
            child->HideSilently();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::ShowSilently
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::ShowSilently()
{
    if (GetVisible())
    {
        iSilentlyHidden = EFalse;
        RWindow& window = Window();
        // Bring to front of the z order
        // Will cause a deferred redraw
        window.SetOrdinalPosition(0, iWindowPriority);
    }
    const TInt childCount = iChildren.Count();
    for (TInt i = 0; i < childCount; i++)
    {
        MSwtControl* child = iChildren[i];
        if (child->IsShell())
        {
            child->ShowSilently();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetVisible
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtShell::SetVisible(TBool aVisible)
{
    // A DrawNow() is absolutely mandatory if the ui is set to be ready.
    CSwtDecorations::SetVisible(aVisible);

    TBool paintingUrgently = !AsyncPainting();
    if (aVisible && paintingUrgently && !iDisplay.IsUiReady())
    {
        iDisplay.SetUiReady(iParent == NULL);
    }
    // Otherwise will be done from Java.
}

// ---------------------------------------------------------------------------
// CSwtShell::ChildFocusChanged
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtShell::ChildFocusChanged(MSwtControl& aControl)
{
    // This method's only purpose is to stop recursion, therefore nothing
    // to be done here. Ensure the shell is indeed the active one if the
    // control is focused
    ASSERT((aControl.CoeControl().IsFocused()) ?
           UiUtils().GetActiveShell() == this : true);
    (void) aControl;
}

// ---------------------------------------------------------------------------
// CSwtShell::SetMaximized
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
void CSwtShell::SetMaximized(TBool aMaximized)
{
    // Top-level or full-screen Shells cannot be maximised
    // but maximized true brings the application to foreground if it is not.
    if (!iParent || iFullScreen)
    {
        if (aMaximized && !UiUtils().IsAppFocused())
        {
            UiUtils().ChangeAppFocus(*this, ETrue);
            return;
        }
    }
    if (aMaximized == iIsMaximized)
    {
        return;
    }

    TRect newRect(TRect::EUninitialized);
    if (aMaximized)
    {
        TRect screenRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);
        TInt variety(0);
        if (CSwtLafFacade::IsLandscapeOrientation())
        {
            variety = AknLayoutUtils::PenEnabled() ?
                      KSwtMainPainVarietySmallSpLandscapePen :
                      KSwtMainPainVarietySmallSpLandscape;
        }
        else
        {
            variety = KSwtMainPainVarietyClassic;
        }
        TAknLayoutRect layoutRect = CSwtLafFacade::GetComposeAndLayoutRect(
                                        CSwtLafFacade::EMainPaneCompose, screenRect, variety);
        newRect = layoutRect.Rect();
        iNormalBounds.SetRect(iPosition, iSize);
    }
    else
    {
        newRect = iNormalBounds;
    }

    if (!aMaximized)
    {
        iIsMaximized = EFalse;
    }
    SetBounds(newRect);
    if (aMaximized)
    {
        iIsMaximized = ETrue;
    }
    Redraw();
}

void CSwtShell::SetMinimized(TBool aMinimized)
{
    if (iParent) // minimized is not supported for second level shells
    {
        return;
    }
    else
    {
        if (UiUtils().IsAppFocused() == aMinimized)  //Is there anything to do?
        {
            if (aMinimized)
            {
                if (UiUtils().GetParentTopShellOfActiveShell() == this)  // only active toplevel shells can send to bground
                {
                    UiUtils().ChangeAppFocus(*this, EFalse);
                }
            }
            else
            {
                UiUtils().ChangeAppFocus(*this, ETrue);
            }
        }
    }
}

TBool CSwtShell::GetMinimized() const
{
    if (iParent)
    {
        return EFalse;
    }
    else
    {
        return !UiUtils().IsAppFocused();
    }
}
// ---------------------------------------------------------------------------
// CSwtShell::GetText
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
const TDesC* CSwtShell::GetText() const
{
    // If this is a Top Level Shell.
    if (!GetParent())
    {
        return iTitleText;
    }
    else
    {
        return CSwtDecorations::GetText();
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetTextL
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
void CSwtShell::SetTextL(const TDesC& aText)
{
    // If this is a Top Level Shell.
    if (!GetParent())
    {
        if (iTitleText)
        {
            delete iTitleText;
            iTitleText = NULL;
        }

        iTitleText = aText.AllocL();

        // If this is the current active Shell so the name in the task List
        // is update with TitleText.
        if (UiUtils().GetActiveShell() == this)
        {
            UiUtils().UpdateStatusPaneL();
        }
    }
    else
    {
        CSwtDecorations::SetTextL(aText);
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetImageL
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
void CSwtShell::SetImageL(const MSwtImage* aImage)
{
    CSwtDecorations::SetImageL(aImage);
    if (!iParent)
    {
        // If this is the current active Shell so the image in the Context pane
        // is updated.
        if (UiUtils().GetActiveShell() == this)
        {
            UiUtils().UpdateStatusPaneL();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetMenuBar
// From MSwtDecorations
// ---------------------------------------------------------------------------
//
void CSwtShell::SetMenuBar(const MSwtMenu* aMenuBar)
{
    if (aMenuBar)
    {
        InstallCba();
    }
    CSwtDecorations::SetMenuBar(aMenuBar);
}

// ---------------------------------------------------------------------------
// CSwtShell::Decorations
// From MSwtShell
// ---------------------------------------------------------------------------
//
MSwtDecorations* CSwtShell::Decorations()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtShell::GetParentShell
// From MSwtShell
// ---------------------------------------------------------------------------
//
MSwtShell* CSwtShell::GetParentShell()
{
    return static_cast<CSwtShell*>(iParent);
}

// ---------------------------------------------------------------------------
// CSwtShell::ForceActive
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::ForceActive()
{
    UiUtils().SetActiveShell(*this, ETrue);
}

// ---------------------------------------------------------------------------
// CSwtShell::GetChildShellsPeersL
// From MSwtShell
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtShell::GetChildShellsPeersL()
{
    const TInt childCount = iChildren.Count();
    if (iChildren.Count() == 0)
    {
        return NULL;
    }

    CSwtPeerArray* array = new(ELeave) CSwtPeerArray(KChildrenGranularity);
    CleanupStack::PushL(array);

    for (TInt i = 0; i < childCount; ++i)
    {
        MSwtControl* child = iChildren[i];
        if (child->IsShell())
        {
            array->AppendL(child->JavaPeer());
        }
    }

    CleanupStack::Pop(array);
    return array;
}

// ---------------------------------------------------------------------------
// CSwtShell::Display
// From MSwtShell
// ---------------------------------------------------------------------------
//
MSwtDisplay& CSwtShell::Display() const
{
    return iDisplay;
}

// ---------------------------------------------------------------------------
// CSwtShell::FullScreenMode
// From MSwtShell
// ---------------------------------------------------------------------------
//
TBool CSwtShell::FullScreenMode() const
{
    return iFullScreen;
}

// ---------------------------------------------------------------------------
// CSwtShell::GetImeInputMode
// From MSwtShell
// IME stands for Input Method Editor. This is Microsoft's method of allowing
// to enter the thousands of different characters used in East Asian languages
// using a standard 101-key keyboard.
// There are four IME types: Japanese, Korean, Chinese Simplified and
// Chinese Traditional.
// IMEs use different ways of writing Eastern characters: roman, katakana,
// hiragana or the numeric codes ( the unicode values ).
// There are different flags used to control how a key stroke is converted into
// an asian character. The flats are:
// - NONE if set means the IME is not in use. This is the only behaviour we
//   support on non APAC variants of eSWT. We do this by returning 0 in SWT.
// - ROMAN if set means that the roman conversion method is used.
// - FULLSHAPE (known as DBCS in SWT) if set means that two bytes are used
//   instead of one.
// - KATAKANA  (known as phonetic in SWT) if set means that katakana is used.
// If not set it means that hiragana is used.
// - NATIVE if set means that one of the native methods is used. If not set
// it means that alpha numeric codes are used.
// ---------------------------------------------------------------------------
//
TInt CSwtShell::GetImeInputMode() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CSwtShell::GetMinimumSize
// From MSwtShell
// ---------------------------------------------------------------------------
//
TSize CSwtShell::GetMinimumSize() const
{
    // Top-level Shells have always the same minimum size: the application area
    if (!iParent)
    {
        return iEikonEnv->EikAppUi()->ClientRect().Size();
    }

    // If there is a user-specified minimum size: use it
    if (iMinSize != TSize(-1,-1))
    {
        return iMinSize;
    }

    // The minimum size if the size of the trimmings
    TRect trim(0, 0, 0, 0);
    ComputeTrim(trim);
    return trim.Size();
}


// ---------------------------------------------------------------------------
// CSwtShell::SetImeInputMode
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetImeInputMode(TInt /*aMode*/)
{
    //Do nothing
}

// ---------------------------------------------------------------------------
// CSwtShell::SetMinimumSize
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetMinimumSize(const TSize& aMinimumSize)
{
    // The minimum size of top-level Shells cannot be modified
    if (!iParent)
    {
        return;
    }

    // The absolute minimum size: the size of the trimmings
    TRect trim(0, 0, 0, 0);
    ComputeTrim(trim);
    const TSize trimSize(trim.Size());

    // Adjust the specified minimum size to be no less than the absolute minimum
    iMinSize.iWidth = Max(aMinimumSize.iWidth,  trimSize.iWidth);
    iMinSize.iHeight = Max(aMinimumSize.iHeight, trimSize.iHeight);

    // Adjust current size to be at least as big as the minimum size
    TSize newSize(Max(iMinSize.iWidth,iSize.iWidth),
                  Max(iMinSize.iHeight,iSize.iHeight));
    if (newSize != iSize)
    {
        SetSize(newSize);
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetStyle
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetStyle(TInt aStyle)
{
    iStyle = aStyle;
}

// ---------------------------------------------------------------------------
// CSwtShell::GetFullTabListL
// From MSwtShell
// ---------------------------------------------------------------------------
//
const RSwtControlArray& CSwtShell::GetFullTabListL()
{
    if (iFullTabList.Count() == 0)
    {
        ComputeTabListL(iFullTabList);   // codescanner::leave
    }

#ifdef _DEBUG
    // Make sure the list contains no Shell
    const TInt count = iFullTabList.Count();
    for (TInt i=0; i < count; ++i)
    {
        ASSERT(!iFullTabList[i]->IsShell());
    }
#endif

    return iFullTabList;
}

// ---------------------------------------------------------------------------
// CSwtShell::CSwtShell
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::ResetTabList()
{
    iFullTabList.Reset();
}

// ---------------------------------------------------------------------------
// CSwtShell::FindTraversalTargetL
// From MSwtShell
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtShell::FindTraversalTargetL(TInt aDetail,
        MSwtControl& aFocusControl, RSwtControlArray* aExcludedControls)
{
    MSwtControl* ctrl = &aFocusControl;
    MSwtControl* newFocus = NULL;

    // Tab traversal, use the tab list
    if (aDetail == ESwtTraverseTabNext || aDetail == ESwtTraverseTabPrevious ||
            aDetail == ESwtTraverseArrowNext || aDetail == ESwtTraverseArrowPrevious)
    {
        // If Shell itself force focused, start from the first child.
        if (ctrl->ShellInterface() == this)
        {
            ctrl = NULL;
            if (iChildren.Count())
            {
                ctrl = iChildren[0];
            }
        }

        if (!ctrl)
        {
            return this;
        }

        const RSwtControlArray& tabList = GetFullTabListL();
        const TInt oldIndex = tabList.Find(ctrl);

        if (oldIndex == KErrNotFound)
        {
            // A control can be excluded from tablist. If so, just set traverse
            // target to the nearest control which is in the tablist
            MSwtControl* control = ctrl;
            MSwtComposite* parent = control->GetParent();
            const RSwtControlArray* children;
            while (control && parent)
            {
                children = parent->Children();
                ASSERT(children);
                TInt controlIndex = children->Find(control);
                if (controlIndex < 0)
                {
                    // The control has a parent and its parent does not know this child,
                    // which means the parents should not be used as a composite
                    break;
                }
                // find the nearest control in the tablist from sibling
                const TInt count = children->Count();
                const TInt inc = (aDetail == ESwtTraverseTabNext || aDetail == ESwtTraverseArrowNext) ? 1 : count-1;
                TInt i = controlIndex;
                while ((i = (i + inc) % count) != controlIndex)
                {
                    MSwtControl* ctl = tabList[i];
                    if ((tabList.Find(ctl) != KErrNotFound)
                            && (ctl->IsFocusable())
                            && ((!aExcludedControls)
                                || (aExcludedControls->Find(ctl) == KErrNotFound)))
                    {
                        newFocus = ctl;
                        break;
                    }
                }
                // goes to the parent level
                control = parent->Control();
                parent = control->GetParent();
            }
        }
        else
        {
            //ASSERT( oldIndex != KErrNotFound );
            const TInt count = tabList.Count();
            const TInt inc   = (aDetail == ESwtTraverseTabNext
                                || aDetail == ESwtTraverseArrowNext) ? 1 : count - 1;

            TInt i = oldIndex;
            while ((i = (i + inc) % count) != oldIndex)
            {
                MSwtControl* ctl = tabList[i];
                if ((ctl->IsFocusable()) && ((!aExcludedControls)
                                             || (aExcludedControls->Find(ctl) == KErrNotFound)))
                {
                    newFocus = ctl;
                    break;
                }
            }
        }
    }
    else if (aDetail == ESwtTraverseParent)
    {
        MSwtControl* parentCtrl = NULL;
        MSwtComposite* parent = aFocusControl.GetParent();
        while (parent)
        {
            parentCtrl = parent->Control();

            RSwtControlArray tabList;
            parentCtrl->ComputeTabListL(tabList);
            TInt count = tabList.Count();
            for (TInt i = 0; i < count; i++)
            {
                MSwtControl* ctrl = tabList[i];
                if (ctrl != &aFocusControl && ctrl->IsFocusable())
                {
                    newFocus = ctrl;
                    break; // for
                }
            }
            tabList.Close();

            if (newFocus)
            {
                break; // while
            }

            if (parentCtrl->IsFocusable())
            {
                newFocus = parentCtrl;
                break; // while
            }

            if (parentCtrl->IsShell())
            {
                break;
            }

            parent = parentCtrl->GetParent();
        }
    }

    if (newFocus == &aFocusControl)
    {
        newFocus = NULL;
    }

    return newFocus;
}

// ---------------------------------------------------------------------------
// CSwtShell::SetFocusControl
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetFocusControl(MSwtControl* aControl)
{
    // If this Shell is active, then the specified control must be either NULL
    // or already focused
    ASSERT(UiUtils().GetActiveShell() != this || !aControl
           || aControl->CoeControl().IsFocused());
    // Old focus control must first be removed before setting new one
    ASSERT(!iFocusControl || !aControl);
    ASSERT(aControl != iFocusControl || !aControl);
    ASSERT(aControl != this);
    ASSERT(!aControl || !aControl->IsShell());

    if (aControl)
    {
        iFocusMemory = NULL;
    }

    // The control which gain the Focus.
    if (aControl)
    {
        // The Control which gain he Focus is a Push Button
        MSwtButton* focusedButton = aControl->ButtonInterface();
        if (focusedButton && (aControl->Style() & KSwtStylePush))
        {
            // Set the Focused Button to Default.
            SetFocusedDefaultButton(focusedButton);
        }
        else
        {
            // If the focused Control is not a Push Button,
            // then set the DefaultButton on the Shell to DefaultButton.
            if (OriginalDefaultButton())
            {
                const_cast<MSwtButton*>(OriginalDefaultButton())->SetDefault(ETrue);
            }
        }
    }
    // The control which lost Focus.
    else if (iFocusControl)
    {
        // The Control which lost he Focus is a Push Button
        if (iFocusControl->ButtonInterface()
                && (iFocusControl->Style() & KSwtStylePush))
        {
            // So remove the DefaultButton status on it.
            SetFocusedDefaultButton(NULL);
        }
    }

    DoSetFocusControl(aControl);
}

// ---------------------------------------------------------------------------
// CSwtShell::HasModalChild
// From MSwtShell
// ---------------------------------------------------------------------------
//
TBool CSwtShell::HasModalChild() const
{
    ASSERT(EFalse);
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtShell::ActivityChanged
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::ActivityChanged(TBool aIsActive, TBool aSetFocus)
{
    iControlGainingFocus = NULL;
    iControlGoingToStack = NULL;

    // Becoming active
    if (aIsActive)
    {
        ASSERT(!iFocusControl);

        TRAP_IGNORE(AddToControlStackL(ECoeStackPriorityDefault));

        // Set the top level Shell title in the task list.
        const TDesC* titleText = GetText();

        // Find a focus control
        MSwtControl* newFocusControl = NULL;

        if (aSetFocus)
        {
            if (!iFocusMemory)
            {
                // First focusable one in the tab list
                const RSwtControlArray* tabList = NULL;
                TRAP_IGNORE((tabList=&GetFullTabListL()));
                if (tabList)
                {
                    const TInt count= tabList->Count();
                    for (TInt i=0; i<count; ++i)
                    {
                        MSwtControl* ctrl = (*tabList)[i];
                        if (ctrl->IsFocusable())
                        {
                            newFocusControl = ctrl;
                            break;
                        }
                    }
                }
            }
            else if (!iFocusMemory->IsFocusable())
            {
                TRAP_IGNORE((newFocusControl = FindTraversalTargetL(
                                                   ESwtTraverseTabPrevious, *iFocusMemory)));
            }
            else
            {
                newFocusControl = iFocusMemory;
            }
        }

        iFocusMemory = NULL;

        TRAP_IGNORE(iDisplay.PostShellEventL(iPeer, ESwtEventActivate));

        if (newFocusControl)
        {
            // Give focus to the focus control
            ASSERT(!newFocusControl->IsShell());
            CCoeControl& coeFocusCtrl = newFocusControl->CoeControl();
            if (!coeFocusCtrl.IsFocused())
            {
                // This will set iFocusControl to the correct value
                coeFocusCtrl.SetFocus(ETrue);
                ASSERT(&iFocusControl->CoeControl() == &coeFocusCtrl);
            }
            ASSERT(coeFocusCtrl.IsFocused());
        }
        else
        {
            // No focusable Control found, the Shell is active on its own.
            // Clear the next focusable Control in UiUtils. If this is not
            // done, when returning from screen saver, the app will jump
            // to the parent Shell of the next focusable control
            UiUtils().SetNextFocusedControl(NULL);
        }

        MSwtUiUtils& utils = iDisplay.UiUtils();
        if (!iFullScreen)
        {
            if (HasCba())
            {
                // Shell has CBA, force CBA visible
                utils.SetCbaVisible(ETrue);
            }
            else
            {
                if (iParent)
                {
                    // Child Shell does not have CBA.
                    // It can be made invisible if topmost shell is fullscreen.
                    MSwtShell* topMostShell = utils.TopMostTopShell();
                    if (topMostShell && topMostShell->FullScreenMode())
                    {
                        utils.SetCbaVisible(EFalse);
                    }
                }
                // Top Shells which are not fullscreen always have CBA
            }
        }
        else
        {
            utils.SetCbaVisible(EFalse);
        }
    }
    // Becoming inactive
    else
    {
        TRAP_IGNORE(iDisplay.PostShellEventL(iPeer, ESwtEventDeactivate));
        if (iFocusControl)
        {
            RemoveAndRememberFocus();
        }
        RemoveFromControlStack();

        // Just in case we were in the middle of kinetic action
        SetUrgentPaintControl(NULL);
    }

    // Notify Menu arranger
    if (aIsActive)
    {
        iDisplay.MenuArranger().ActiveShellGained(*this);
    }
    else
    {
        iDisplay.MenuArranger().ActiveShellLost();
    }

    // Notify Command arranger
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    if (commandArranger)
    {
        if (aIsActive)
        {
            commandArranger->ActiveShellGained(*this);
        }
        else
        {
            commandArranger->ActiveShellLost();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetZPosition
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetZPosition(TInt aZPosition)
{
    Window().SetOrdinalPosition(aZPosition);

    // Set our children at the same position ( to avoid having them behind us )
    const TInt count = iChildren.Count();
    // The foreground child is the first one, hence our looping backwards
    for (TInt i = count - 1; i >= 0; --i)
    {
        MSwtShell* childShell = iChildren[i]->ShellInterface();
        if (childShell)
        {
            childShell->SetZPosition(aZPosition);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::FocusControl
// From MSwtShell
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtShell::FocusControl() const
{
    return iFocusControl;
}

// ---------------------------------------------------------------------------
// CSwtShell::SetDefaultCommand
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetDefaultCommand(const MSwtCommand* aCommand)
{
    iDefaultCommand = aCommand;
}

// ---------------------------------------------------------------------------
// CSwtShell::DefaultCommand
// From MSwtShell
// ---------------------------------------------------------------------------
//
const MSwtCommand* CSwtShell::DefaultCommand() const
{
    return iDefaultCommand;
}

// ---------------------------------------------------------------------------
// CSwtShell::SetFullScreen
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetFullScreen(TBool aFullScreen)
{
    if (aFullScreen == iFullScreen)
    {
        return;
    }
    iFullScreen = aFullScreen;
    CSwtUiUtils& utils = UiUtils();
    SetRedraw(EFalse);
    MSwtShell* activeTopShell = utils.GetParentTopShellOfActiveShell();
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    if (aFullScreen)
    {
        if (activeTopShell == this)
        {
            if (commandArranger)
            {
                commandArranger->ActiveShellGained(*this);
            }
            utils.SetCbaVisible(EFalse);
        }
        iStyle &= ~KSwtStyleTitle;
    }
    else
    {
        if (activeTopShell == this)
        {
            if (commandArranger)
            {
                commandArranger->ActiveShellGained(*this);
            }
            if (HasCba())
            {
                utils.SetCbaVisible(ETrue);
            }
        }
        iStyle = iSavedStyle;
    }

    // Update status pane
    if (utils.GetActiveShell() == this)
    {
        TRAP_IGNORE(utils.UpdateStatusPaneL());
    }

    DoSetRect(DefaultBounds());

    SetRedraw(ETrue);
}

// ---------------------------------------------------------------------------
// CSwtShell::SetControlGoingToStack
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetControlGoingToStack(MSwtControl* aControl)
{
    iControlGoingToStack = aControl;
}

// ---------------------------------------------------------------------------
// CSwtShell::SetControlGainingFocus
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetControlGainingFocus(MSwtControl* aControl)
{
    iControlGainingFocus = aControl;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::DefaultBounds
// From MSwtShell
// ---------------------------------------------------------------------------
//
TRect CSwtShell::DefaultBounds() const
{
    TRect rect;
    TInt variety(0);
    if (iParent)
    {
        // Dialog shells
        AknLayoutUtils::TAknCbaLocation cbaLocation(AknLayoutUtils::CbaLocation());

        // variety numbers for right CBA are
        if (cbaLocation == AknLayoutUtils::EAknCbaLocationRight)
        {
            variety = 9; // 13 - 7 (highest)
        }
        else if (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
        {
            variety = 16; // 20 - 14 (highest)
        }
        else
        {
            variety = 4; // 0 - 6 (highest)
        }

        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EPopupParent, mainPane);
        rect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EPopupWindowGeneral, mainPane, variety).Rect();

        if (HasTitleStyle())
        {
            TInt h(CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EHeadingPane, mainPane).Rect().Height());
            if (cbaLocation == AknLayoutUtils::EAknCbaLocationRight
                    || cbaLocation == AknLayoutUtils::EAknCbaLocationLeft)
            {
                rect.iTl.iY -= h / 2;
                rect.iBr.iY += h / 2;
            }
            else
            {
                rect.iTl.iY -= h;
            }
        }
    }
    else
    {
        // Top shells
        TRect screenRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);
        if (FullScreenMode())
        {
            return screenRect;
        }
        if (CSwtLafFacade::IsLandscapeOrientation())
        {
            variety = AknLayoutUtils::PenEnabled() ?
                      KSwtMainPainVarietySmallSpLandscapePen :
                      KSwtMainPainVarietySmallSpLandscape;
        }
        else
        {
            variety = KSwtMainPainVarietyClassic;
        }

        TAknLayoutRect layoutRect = CSwtLafFacade::GetComposeAndLayoutRect(
                                        CSwtLafFacade::EMainPaneCompose, screenRect, variety);
        rect = layoutRect.Rect();
        if (!HasCba())
        {
            rect.iBr = screenRect.iBr;
        }
    }

    return rect;
}

// ---------------------------------------------------------------------------
// CSwtShell::SetToBeFocusedControl
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetToBeFocusedControl(MSwtControl* aControl)
{
    ASSERT(!iFocusControl || !iFocusControl->CoeControl().IsFocused());

    if (iFocusControl)
    {
        DoSetFocusControl(NULL);
    }

    iFocusMemory = aControl;
}

// ---------------------------------------------------------------------------
// CSwtShell::CheckFocusMemory
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::CheckFocusMemory(MSwtControl& aUnregisteredControl)
{
    // if shell still keeps reference to control, that is going to be
    // unregistered, that set the reference to another control
    if (iFocusMemory == &aUnregisteredControl)
    {
        iFocusMemory = NULL;
        TRAP_IGNORE((iFocusMemory=FindTraversalTargetL(
                                      ESwtTraverseTabPrevious, aUnregisteredControl)));
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::SetUrgentPaintControl
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetUrgentPaintControl(MSwtControl* aControl)
{
    if (iUrgentPaintControl != aControl)
    {
        if (aControl)
        {
            iUrgentPaintControl = aControl;
        }
        else
        {
            iUrgentPaintControl = NULL;

            // Request a full redraw from Window Server.
            // Mandatory at least for PaintListeners which call redraw()
            DrawDeferred();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::UrgentPaintControl
// From MSwtShell
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtShell::UrgentPaintControl() const
{
    return iUrgentPaintControl;
}

// ---------------------------------------------------------------------------
// CSwtShell::AsyncPainting
// From MSwtShell
// ---------------------------------------------------------------------------
//
TBool CSwtShell::AsyncPainting() const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    return iAsyncPainting;
#else
    return ETrue;
#endif
}

// ---------------------------------------------------------------------------
// CSwtShell::SetAsyncPainting
// From MSwtShell
// ---------------------------------------------------------------------------
//
#ifdef RD_JAVA_S60_RELEASE_9_2
void CSwtShell::SetAsyncPainting(TBool aStatus)
{
    iAsyncPainting = aStatus;
}
#else
void CSwtShell::SetAsyncPainting(TBool)
{
}
#endif

// ---------------------------------------------------------------------------
// CSwtShell::InstallCba
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::InstallCba()
{
    if (iHasCba)
    {
        return;
    }
    iHasCba = ETrue;
    MSwtUiUtils& utils = iDisplay.UiUtils();
    if (utils.GetActiveShell() == this)
    {
        utils.SetCbaVisible(ETrue);
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::HasCba
// From MSwtShell
// ---------------------------------------------------------------------------
//
TBool CSwtShell::HasCba() const
{
    return iHasCba;
}

// ---------------------------------------------------------------------------
// CSwtShell::SetTaskTip
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::SetTaskTip()
{
    iIsTaskTip = ETrue;
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iParent && CAknEnv::Static()->TransparencyEnabled())
    {
        Window().SetRequiredDisplayMode(EColor16MA);
        if (Window().SetTransparencyAlphaChannel() == KErrNone)
        {
            Window().SetBackgroundColor(~0);
        }
    }
    GfxTransEffect::Deregister(this); // already registered in ConstructL
    GfxTransEffect::Register(this, KGfxDiscreetPopupControlUid, EFalse);
#endif
}

// ---------------------------------------------------------------------------
// CSwtShell::IsTaskTip
// From MSwtShell
// ---------------------------------------------------------------------------
//
TBool CSwtShell::IsTaskTip() const
{
    return iIsTaskTip;
}

// ---------------------------------------------------------------------------
// CSwtShell::DoSetLocation
// From MSwtShell
// ---------------------------------------------------------------------------
//
void CSwtShell::DoSetLocation(const TPoint& aPoint)
{
    if (aPoint == iPosition)
    {
        return;
    }

    // Divert the job to UiUtils if this is the Shell of an editor open for
    // split view editing.
    MSwtUiUtils& utils = iDisplay.UiUtils();
    MSwtControl* splitInputView = utils.SplitInputView();
    if (splitInputView && (&(splitInputView->GetShell())) == this)
    {
        utils.SetSplitInputShellPos(aPoint);
        return;
    }

    iPosition = aPoint;
    Window().SetPosition(aPoint);

    // Post the MoveEvent if the location changed.
    PositionChanged();

    // Drawing after everything else.
    if (!(iStyle & KSwtStyleNoRedrawResize))
    {
        Redraw();
    }
}

// ---------------------------------------------------------------------------
// CSwtShell::HandleStatusPaneSizeChange
// From MEikStatusPaneObserver
// ---------------------------------------------------------------------------
//
void CSwtShell::HandleStatusPaneSizeChange()
{
    if (iIsMaximized  && (!iParent))
    {
        DoSetRect(DefaultBounds());
    }
}

void CSwtShell::FinishRedraw() const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    iDisplay.CoeEnv()->WsSession().Finish();
#endif
}

// ---------------------------------------------------------------------------
// CSwtShell::UpdateHighlight
// ---------------------------------------------------------------------------
//
void CSwtShell::UpdateHighlight(TBool aDrawNow /*= EFalse*/)
{
    // Turn off highlight for the previously focused control and its eventual
    // nearest captioned control parent.
    if (iPrevFocusControl)
    {
        MSwtCaptionedControl* captParent = iPrevFocusControl->GetNearestCaptionedControl(EFalse);
        MSwtControl* captParentCtrl = NULL;
        if (captParent)
        {
            MSwtComposite* captParentComp = captParent->Composite();
            if (captParentComp)
            {
                captParentCtrl = captParentComp->Control();
                DoSetHighlight(*captParentCtrl, EFalse);
            }
        }

        DoSetHighlight(*iPrevFocusControl, EFalse);

        if (aDrawNow)
        {
            if (captParentCtrl)
                captParentCtrl->Redraw();
            else
                iPrevFocusControl->Redraw();
        }
        else
        {
            if (captParentCtrl)
                captParentCtrl->CoeControl().DrawDeferred();
            else
                iPrevFocusControl->CoeControl().DrawDeferred();
        }

        iPrevFocusControl = NULL;
    }

    // Turn on highlight for the current focused control and its eventual
    // nearest captioned control parent.
    if (iFocusControl)
    {
        TBool ctrlHighlight = EFalse;
        TBool captHighlight = EFalse;

        if (iFocusControl->Pressed())
        {
            // Always represent pressed state with highlight
            TInt pressPolicy = iFocusControl->PressBackgroundPolicy();
            ctrlHighlight = pressPolicy == EPressBackground;
            captHighlight = pressPolicy == EPressBackground
                            || pressPolicy == EEmbeddedPressBackground;
        }
        else
        {
            if (iDisplay.UiUtils().NaviKeyInput())
            {
                TBool noOtherFocusableCtrls(CountFocusableChildren(1, iFocusControl) == 0);
                if (noOtherFocusableCtrls)
                {
                    // No highlight if there's only one focusable control
                    ctrlHighlight = EFalse;
                    captHighlight = EFalse;
                }
                else
                {
                    captHighlight = ETrue;
                    ctrlHighlight = ETrue;

                    TInt policy = iFocusControl->FocusBackgroundPolicy();
                    if ((policy == ENoFocusBackground)
                            || (policy == EEmbeddedFocusBackground)
                            || (policy ==ENoFocusBackgroundInCaptionedControl))
                    {
                        ctrlHighlight = EFalse;
                    }
                }
            }
            else
            {
                // No focus highlight with touch input
                ctrlHighlight = EFalse;
                captHighlight = EFalse;
            }
        }

        TBool directlyCaptioned(EFalse);
        MSwtCaptionedControl* captParent = iFocusControl->GetNearestCaptionedControl(EFalse);
        MSwtControl* captParentCtrl = NULL;
        if (captParent)
        {
            MSwtComposite* captParentComp = captParent->Composite();
            if (captParentComp)
            {
                directlyCaptioned = iFocusControl->GetParent() == captParentComp;
                // When pressing, only the direct caption control gets "pressed" also.
                if (!directlyCaptioned && iFocusControl->Pressed())
                    captHighlight = EFalse;
                captParentCtrl = captParentComp->Control();
                DoSetHighlight(*captParentCtrl, captHighlight);
            }
        }

        if (!directlyCaptioned)
        {
            DoSetHighlight(*iFocusControl, ctrlHighlight);
        }

        if (aDrawNow)
        {
            if (captParentCtrl)
                captParentCtrl->Redraw();
            else
                iFocusControl->Redraw();
        }
        else
        {
            if (captParentCtrl)
                captParentCtrl->CoeControl().DrawDeferred();
            else
                iFocusControl->CoeControl().DrawDeferred();
        }
    }
}

void CSwtShell::DoSetFocusControl(MSwtControl* aControl)
{
    iPrevFocusControl = iFocusControl;

    // This must always be the only place to assign to iFocusControl!
    iFocusControl = aControl;
}

void CSwtShell::DoSetHighlight(MSwtControl& aControl, TBool aEnabled)
{
    if (aControl.HasHighlight(EFalse) != aEnabled) // not checking parents
    {
        aControl.SetHighlight(aEnabled);
    }
}

void CSwtShell::ControlDisposing(const MSwtControl& aControl)
{
    const MSwtControl* control = &aControl;

    if (control == iPrevFocusControl)
        iPrevFocusControl = NULL;

    if (control == iControlGoingToStack)
        iControlGoingToStack = NULL;

    if (control == iControlGainingFocus)
        iControlGainingFocus = NULL;

    if (control== iFocusControl)
        iFocusControl = NULL;

    if (control == iFocusMemory)
        iFocusMemory = NULL;

    if (control == iUrgentPaintControl)
        iUrgentPaintControl = NULL;
}
