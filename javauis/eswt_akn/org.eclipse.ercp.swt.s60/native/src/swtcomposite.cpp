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


#include <aknappui.h>
#include <aknenv.h>
#include <AknUtils.h>
#include <eikenv.h>
#include <eiksbfrm.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsFrameBackgroundControlContext.h>
#include <centralrepository.h>
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <swtlaffacade.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include "swtcomposite.h"
#include "swtscrollbar.h"
#include "swtuiutils.h"
#include "swtshell.h"


const TInt CSwtComposite::KChildrenGranularity = 5;
const TInt KMaxScrollBarCount = 2;

// ======== MEMBER FUNCTIONS ========

CSwtComposite::CSwtComposite(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                             MSwtComposite* aParent, TInt aStyle, TBool aVisibility, TBool aDimmed)
        : ASwtScrollableBase(aDisplay, aPeer, aParent, aStyle)
{
    CAknControl::MakeVisible(aVisibility);
    SetComponentsToInheritVisibility(ETrue);
    CAknControl::SetDimmed(aDimmed);
    SetFocusing(ETrue);
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
    UpdateTactileFeedbackDensity();
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
}

CSwtComposite::~CSwtComposite()
{
    ASSERT(iChildren.Count() == 0);

    if (iPhysics)
    {
        delete iPhysics;
        iPhysics = NULL;
    }

    if (iPhysicsTimer)
    {
        iPhysicsTimer->Cancel();
    }
    delete iPhysicsTimer;
    iPhysicsTimer = NULL;

    iScrlCompContent = NULL;

    if (iPhysicsLib.Handle())
    {
        iPhysicsLib.Close();
    }

    iTabList.Close();
    iChildren.Close();
    delete iSbFrame;
}

void CSwtComposite::ConstructL()
{
    if (!IsShell())
    {
        ASSERT(iParent!=NULL);
        SetContainerWindowL(iParent->Control()->CoeControl());
    }

    CreateScrollBarFrameL();
    SetBackground(this); // Back will be drawn by ASwtControlBase::Draw
    EnableDragEvents();
    ActivateL();
}

CSwtComposite* CSwtComposite::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                   MSwtComposite& aParent, TInt aStyle)
{
    CCoeControl& parentCtrl = aParent.Control()->CoeControl();
    CSwtComposite* self = new(ELeave) CSwtComposite(aDisplay, aPeer,
            &aParent, aStyle, parentCtrl.IsVisible(), parentCtrl.IsDimmed());
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// From class MCoeControlBackground.
// Draw the Background. This method was called by the eSWT and Symbian
// drawing framework.
void CSwtComposite::Draw(CWindowGc& aGc, const CCoeControl& aControl,
                         const TRect& aRect) const
{
    if (!(iStyle & KSwtStyleNoBackground))
    {
        ASwtControlBase::Draw(aGc, aControl, aRect);
    }
}

void CSwtComposite::CreateScrollBarFrameL()
{
    ASSERT(iSbFrame==NULL);

    if (iStyle & (KSwtStyleHScroll | KSwtStyleVScroll))
    {
        iSbFrame = new(ELeave) CEikScrollBarFrame(this, this, ETrue);

#ifdef RD_SCALABLE_UI_V2
        // WARNING!!! The expanded touch area does not move correctly togehter with the scrollbars!
        iSbFrame->SetScrollBarFrameFlags(CEikScrollBarFrame::EDisableExpandedTouchArea);
#endif // RD_SCALABLE_UI_V2

        if (AknLayoutUtils::DefaultScrollBarType(iAvkonAppUi) == CEikScrollBarFrame::EDoubleSpan)
        {
            // Vertical scrollbar always needs to be created.
            iSbFrame->CreateDoubleSpanScrollBarsL(EFalse, EFalse, ETrue, iStyle & KSwtStyleHScroll);
        }

        CEikScrollBarFrame::TScrollBarVisibility horizVis = (iStyle & KSwtStyleHScroll) ?
                CEikScrollBarFrame::EOn : CEikScrollBarFrame::EOff;
        CEikScrollBarFrame::TScrollBarVisibility vertVis  = (iStyle & KSwtStyleVScroll) ?
                CEikScrollBarFrame::EOn : CEikScrollBarFrame::EOff;
        iSbFrame->SetScrollBarVisibilityL(horizVis, vertVis);

        TEikScrollBarModel vEmptyModel(0,0,0);
        TEikScrollBarModel hEmptyModel(0,0,0);
        TEikScrollBarFrameLayout layout;

        layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;
        layout.SetClientMargin(0);
        layout.SetInclusiveMargin(0);

        TRect inclusiveRect(BorderInnerRect());
        TRect clientRect(SbInnerRect(inclusiveRect));

        iSbFrame->DrawBackground(EFalse,EFalse);

        // Cannot use non double span models for double span scrollbars! Avoiding panics.
        if (iSbFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan)
        {
            TAknDoubleSpanScrollBarModel hDsSbarModel(hEmptyModel);
            TAknDoubleSpanScrollBarModel vDsSbarModel(vEmptyModel);
            iSbFrame->TileL(&hDsSbarModel, &vDsSbarModel, clientRect, inclusiveRect, layout);
        }
        else
        {
            iSbFrame->TileL(&hEmptyModel, &vEmptyModel, clientRect, inclusiveRect, layout);
        }
    }
}

void CSwtComposite::UpdateFrameL()
{
    if (iSbFrame == NULL)
        return;

    TEikScrollBarModel hModel;
    CEikScrollBar* hsb = iSbFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal);
    if (hsb)
        hModel = *hsb->Model();

    TEikScrollBarModel vModel;
    CEikScrollBar* vsb = iSbFrame->GetScrollBarHandle(CEikScrollBar::EVertical);
    if (vsb)
        vModel = *vsb->Model();

    TEikScrollBarFrameLayout layout;

    layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;
    layout.SetClientMargin(0);
    layout.SetInclusiveMargin(0);

    TRect inclusiveRect(BorderInnerRect());
    TRect clientRect(SbInnerRect(inclusiveRect));

    InvalidateScrollBars();

    // Cannot use non double span models for double span scrollbars! Avoiding panics.
    if (iSbFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan)
    {
        TAknDoubleSpanScrollBarModel hDsSbarModel(hModel);
        TAknDoubleSpanScrollBarModel vDsSbarModel(vModel);
        iSbFrame->TileL(&hDsSbarModel, &vDsSbarModel, clientRect, inclusiveRect, layout);
    }
    else
    {
        iSbFrame->TileL(&hModel, &vModel, clientRect, inclusiveRect, layout);
    }
}

// This is needed only due to problems with the base class implementation of
// updating the scrollbars. Without doing this the scrollbars won't get
// correctly updated when the control is moved around.
void CSwtComposite::InvalidateScrollBars()
{
    if (iSbFrame)
    {
        CEikScrollBar* vBar = iSbFrame->GetScrollBarHandle(CEikScrollBar::EVertical);
        CEikScrollBar* hBar = iSbFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal);
        if (vBar)
        {
            vBar->SetExtent(TPoint(0, 0), TSize(0, 0));
        }
        if (hBar)
        {
            hBar->SetExtent(TPoint(0, 0), TSize(0, 0));
        }
    }
}

void CSwtComposite::RectChanged()
{
    const TRect oldRect(iRect);
    const TRect newRect(Rect());
    if (newRect == oldRect)
        return;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    // Reset last kinetic scrolling tactile feedback position
    iLastTactileFeedbackPos = 0;
#endif

    // Take care of scroll-bars
    TRAP_IGNORE(UpdateFrameL());

    // Forward new position to children
    if (newRect.iTl != oldRect.iTl)
    {
        if (!OwnsWindow())
        {
            const TPoint delta(newRect.iTl - oldRect.iTl);
            const TInt childCount = iChildren.Count();
            for (TInt i=0; i<childCount; ++i)
            {
                MSwtControl* child = iChildren[i];
                if (!child->IsShell())
                {
                    CCoeControl& coeChild = child->CoeControl();
                    TPoint pos(coeChild.Position());
                    pos += delta;
                    coeChild.SetPosition(pos);
                }
            }
        }
    }

    iRect = newRect;
}

TInt CSwtComposite::CountComponentControls() const
{
    TInt result = 0;

    // Scroll-bar frame's components
    if (iSbFrame != NULL)
        result += iSbFrame->CountComponentControls();

    return result;
}

CCoeControl* CSwtComposite::ComponentControl(TInt aIndex) const
{
    // Scroll-bar frame's components
    if (iSbFrame != NULL)
    {
        const TInt count = iSbFrame->CountComponentControls();
        if (aIndex < count)
            return iSbFrame->ComponentControl(aIndex);
        aIndex -= count;
    }

    ASSERT(EFalse);
    return NULL;
}

void CSwtComposite::MakeVisible(TBool aVisible)
{
    CAknControl::MakeVisible(aVisible); // Scrollbars are taken care of here

    // Children
    const TInt childCount = iChildren.Count();
    for (TInt i=0; i<childCount; ++i)
    {
        MSwtControl& child = *iChildren[i];
        // This prevents making visible controls invisible on their own
        if (!(child.Flags() & MSwtControl::EFlagInvisible))
            child.CoeControl().MakeVisible(aVisible);
    }

    // Take care of scroll-bars
    TRAP_IGNORE(UpdateFrameL());

    if (!IsShell())
        FocusabilityChanged();
}

void CSwtComposite::SetDimmed(TBool aDimmed)
{
    CAknControl::SetDimmed(aDimmed);

    // Children
    const TInt childCount = iChildren.Count();
    for (TInt i=0; i<childCount; ++i)
    {
        MSwtControl& child = *iChildren[i];
        // This prevents undimming controls dimmed on their own
        if (!(child.Flags() & MSwtControl::EFlagDisabled))
            child.CoeControl().SetDimmed(aDimmed);
    }

    // Scroll-bars
    if (iSbFrame != NULL)
    {
        CEikScrollBar* vsb = iSbFrame->GetScrollBarHandle(CEikScrollBar::EVertical);
        if (vsb)
        {
            vsb->SetDimmed(aDimmed);
        }

        CEikScrollBar* hsb = iSbFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal);
        if (hsb)
        {
            hsb->SetDimmed(aDimmed);
        }
    }

    if (!IsShell())
        FocusabilityChanged();
}

TKeyResponse CSwtComposite::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    return HandleKeyL(aKeyEvent, aType, ETrue);
}

#ifdef RD_SCALABLE_UI_V2
// Physics pointer handling:
// - pointer down is stored for late pointer down deliver and higilight timer is started.
// - if the timer times out before drag or up events, the pointet control is focused.
// - drag events cancel the timer and the late pointer down delivery.
// - drag events also start the physics panning if the drag exceeds the panning treshold.
// - up events start the physics flicking if physics panning ongoing.
// - up events also perform the late pointer down delivery if physics not ongoing.
void CSwtComposite::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    CEikScrollBar* vsb = iSbFrame ?
                         iSbFrame->GetScrollBarHandle(CEikScrollBar::EVertical) : NULL;
    CEikScrollBar* hsb = iSbFrame ?
                         iSbFrame->GetScrollBarHandle(CEikScrollBar::EHorizontal) : NULL;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    iLastPointerEventType = aPointerEvent.iType;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    // Check if we should start scrollbar grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        if (!iVScrollBarGrabsPointerEvents)
        {
            if (vsb && vsb->Rect().Contains(aPointerEvent.iPosition))
            {
                iVScrollBarGrabsPointerEvents = ETrue;
            }
        }
        if (!iHScrollBarGrabsPointerEvents)
        {
            if (hsb && hsb->Rect().Contains(aPointerEvent.iPosition))
            {
                iHScrollBarGrabsPointerEvents = ETrue;
            }
        }
    }

    if (iVScrollBarGrabsPointerEvents || iHScrollBarGrabsPointerEvents)
    {
        ProcessCompositePointerEventL(aPointerEvent);
        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            iVScrollBarGrabsPointerEvents = EFalse;
            iHScrollBarGrabsPointerEvents = EFalse;
        }
    }
    else
    {
        TBool consumed(EFalse);
        if (iPhysics)
        {
            if (iPhysics->OngoingPhysicsAction() == MSwtPhysics::ESwtPhysicsActionBouncing)
            {
                GetShell().Control()->CoeControl().IgnoreEventsUntilNextPointerUp();
                return;
            }

            switch (aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
            {
                consumed = ETrue;
                iPhysicsBreaking = iPhysicsAction == KSwtPhysicsFlicking;
                iPhysics->StopPhysics(); // Now iPhysicsAction should be KSwtPhysicsNoAction
                iPhysicsAction = KSwtPhysicsGrabbing;
                iPhysicsStartPos = aPointerEvent.iPosition;
                iPhysicsPointerDown = aPointerEvent;
                iPhysicsFocusedCtrl = GetShell().FocusControl();
                PhysicsResetGeometryL();
                GetShell().SetUrgentPaintControl(this);
                // If pointer down occurs while flicking, cannot forward pointer anymore to controls.
                if (!iPhysicsBreaking)
                    PhysicsStartTimer();
                iPhysicsStartTime.HomeTime();
            }
            break;
            case TPointerEvent::EDrag:
            {
                // All drag events are used to do the physics.
                consumed = ETrue;
                if (iPhysicsAction == KSwtPhysicsPanning)
                {
                    iPhysics->RegisterPanningPosition(iPhysicsLastPos - aPointerEvent.iPosition);
                }
                else
                {
                    // Drag occurs before the physics timer timout.
                    // Check if it is long enough to start the physics.
                    TPoint drag(iPhysicsStartPos - aPointerEvent.iPosition);
                    if (Abs(drag.iX) > iPhysics->DragThreshold()
                            || Abs(drag.iY) > iPhysics->DragThreshold())
                    {
                        // Unfocus
                        MSwtControl* ctrl = GetShell().FocusControl();
                        if (ctrl)
                        {
#ifdef RD_JAVA_S60_RELEASE_9_2
                            // While panning there should be no focus highlight
                            // Highlight is enabled after pressing any key
                            ctrl->EnableFocusHighlight(EFalse);
#endif //RD_JAVA_S60_RELEASE_9_2
                            if (ctrl != iPhysicsFocusedCtrl)
                            {
                                ctrl->CoeControl().SetFocus(EFalse);
                            }
                        }

                        // Revert
                        if (iPointerGrabbingControl)
                        {
                            TPointerEvent ev(iPhysicsPointerDown);
                            ev.iType = TPointerEvent::EButton1Up;
                            // This is safe for as long as there is no parallel pointer handling.
                            iDisplay.SetRevertPointerEvent(ETrue);
                            iPointerGrabbingControl->HandlePointerEventL(ev);
                            iDisplay.SetRevertPointerEvent(EFalse);
                        }

                        iPhysicsTimer->Cancel();
                        iPhysics->RegisterPanningPosition(drag);
                        iPhysicsAction = KSwtPhysicsPanning;
                    }
                }
            }
            break;
            case TPointerEvent::EButton1Up:
            {
                TBool timerWasActive = iPhysicsTimer->IsActive();
                if (timerWasActive)
                {
                    iPhysicsTimer->Cancel();
                }
                if (iPhysicsAction != KSwtPhysicsNoAction
                        && iPhysicsAction != KSwtPhysicsGrabbing)
                {
                    consumed = ETrue;
                    const TTime time(iPhysicsStartTime);
                    TPoint drag(iPhysicsStartPos - aPointerEvent.iPosition);
                    if (iPhysics->StartPhysics(drag, time))
                        iPhysicsAction = KSwtPhysicsFlicking;
                    else
                        iPhysicsAction = KSwtPhysicsNoAction;
                }
                else
                {
                    iPhysicsAction = KSwtPhysicsNoAction;
                    GetShell().SetUrgentPaintControl(NULL);
                    if (!iPhysicsBreaking && timerWasActive)
                    {
                        // Pointer up occurs before the physics timer timout.
                        // Deliver the delayed pointer down, then this up pointer event.
                        DoHandlePointerEventL(iPhysicsPointerDown);
                    }
                    consumed = iPhysicsBreaking;
                    iPhysicsBreaking = EFalse;
                }
            }
            break;
            default:
                break;
            }

            iPhysicsLastPos = aPointerEvent.iPosition;
        }

        if (!consumed)
        {
            DoHandlePointerEventL(aPointerEvent);
        }
    }
}

void CSwtComposite::ProcessCompositePointerEventL(const TPointerEvent& aPointerEvent)
{
    MSwtControl* capturingControl = UiUtils().PointerCaptureControl();
    if (capturingControl != this)
    {
        // Call the base to handle further the pointer event.
        // This way we guarantee that eventual component controls
        // get the event also. See CSwtComposite::CountComponentControls(),
        // CSwtTable::CountComponentControls(), etc.
        CAknControl::HandlePointerEventL(aPointerEvent);
    }
    PostMouseEventL(aPointerEvent);
}

void CSwtComposite::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
    if (aScrollBar == NULL)
        return;

    if (aEventType == EEikScrollThumbDragHoriz ||
            aEventType == EEikScrollThumbDragVert ||
            aEventType == EEikScrollThumbReleaseHoriz ||
            aEventType == EEikScrollThumbReleaseVert)
    {
        if (iScrlCompContent)
        {
            switch (aEventType)
            {
            case EEikScrollThumbDragVert:
            case EEikScrollThumbDragHoriz:
                GetShell().SetUrgentPaintControl(this);
                break;
            case EEikScrollThumbReleaseVert:
            case EEikScrollThumbReleaseHoriz:
                GetShell().SetUrgentPaintControl(NULL);
                break;
            default:
                break;
            }
        }

        ASwtScrollableBase::HandleScrollEventL(aScrollBar, aEventType);
        return;
    }

    const TInt oldPos = aScrollBar->ThumbPosition();
    TInt       pos    = oldPos;

    switch (aEventType)
    {
        // Horizontal scrollbar
    case EEikScrollLeft:
        pos -= iHScroll->GetIncrement();
        break;
    case EEikScrollRight:
        pos += iHScroll->GetIncrement();
        break;
    case EEikScrollPageLeft:
        pos -= iHScroll->GetPageIncrement();
        break;
    case EEikScrollPageRight:
        pos += iHScroll->GetPageIncrement();
        break;
    case EEikScrollHome:
        pos = 0;
        break;

        // Vertical scrollbar
    case EEikScrollUp:
        pos -= iVScroll->GetIncrement();
        break;
    case EEikScrollDown:
        pos += iVScroll->GetIncrement();
        break;
    case EEikScrollPageUp:
        pos -= iVScroll->GetPageIncrement();
        break;
    case EEikScrollPageDown:
        pos += iVScroll->GetPageIncrement();
        break;
    case EEikScrollTop:
        pos = 0;
        break;

        // Either scrollbar
    case EEikScrollEnd:
    case EEikScrollBottom:
    {
        if (iSbFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan)
        {
            const TAknDoubleSpanScrollBarModel* dsModel =
                static_cast<const TAknDoubleSpanScrollBarModel*>(aScrollBar->Model());
            pos = dsModel->ScrollSpan();
        }
        else
        {
            pos = aScrollBar->Model()->iScrollSpan;
        }
    }
    break;

    default:
        return;
    }

    if (pos != oldPos)
    {
        aScrollBar->SetModelThumbPosition(pos);
    }
    //Let scrollable base dispatch the event
    ASwtScrollableBase::HandleScrollEventL(aScrollBar, aEventType);
}
#else
void CSwtComposite::HandlePointerEventL(const TPointerEvent& /*aPointerEvent*/)
{
}

void CSwtComposite::ProcessCompositePointerEventL(const TPointerEvent& /*aPointerEvent*/)
{
}

void CSwtComposite::HandleScrollEventL(CEikScrollBar* /*aScrollBar*/, TEikScrollEvent /*aEventType*/)
{
}
#endif

void CSwtComposite::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    UpdateTactileFeedbackDensity();
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
}

void CSwtComposite::SwtHandleResourceChangeL(TInt aType)
{
    CAknControl::HandleResourceChange(aType);

    const TInt childCount = iChildren.Count();
    for (TInt i=0; i<childCount; ++i)
    {
        // Forward HandleResourceChange to children (except if already informed through control stack)
        if (iChildren[i]->IsOnControlStack() == EFalse)
        {
            iChildren[i]->CoeControl().HandleResourceChange(aType);
        }
    }

    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        // Resolution may have changed, so update scrollbar frame.
        UpdateFrameL();
    }
}

void CSwtComposite::Draw(const TRect& /*aRect*/) const
{
}

void CSwtComposite::FocusChanged(TDrawNow aDrawNow)
{
    CAknControl::FocusChanged(ENoDrawNow);
    HandleFocusChanged(aDrawNow);
}

void CSwtComposite::PositionChanged()
{
    if (IsShell())
    {
        const TInt childCount = iChildren.Count();
        for (TInt i = 0; i < childCount; ++i)
        {
            MSwtControl* child = iChildren[i];
            // We reset the position of child control in order to
            // correctly display scrollbar's background
            if (!child->IsShell() && child->ScrollableInterface())
            {
                CCoeControl& coeChild = child->CoeControl();
                TPoint pos(coeChild.Position());
                coeChild.SetPosition(pos);
            }
        }
    }
    RectChanged();
    HandlePositionChanged();
}

void CSwtComposite::SizeChanged()
{
    RectChanged();
    HandleSizeChanged();
}

TTypeUid::Ptr CSwtComposite::MopSupplyObject(TTypeUid aId)
{
    if (aId.iUid == MAknsControlContext::ETypeId)
    {
        return TTypeUid::Null();
    }
    return CAknControl::MopSupplyObject(aId);
}

CCoeControl& CSwtComposite::CoeControl()
{
    return *this;
}

const CCoeControl& CSwtComposite::CoeControl() const
{
    return *this;
}

TBool CSwtComposite::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    if (aReason == KSwtFocusByForce)
    {
        return ASwtScrollableBase::IsFocusable(KSwtFocusByForce);
    }

    if (!CoeControl().IsVisible() || !GetEnabled())
    {
        return EFalse;
    }

    MSwtShell& shell = GetShell();
    if (HasFocusableControl(shell.Control()))
    {
        // if there is a focusable control in the shell scope
        // composite should not get focus
        return EFalse;
    }

    // start to handle the situation that there is not a focusable control in the shell

    const RSwtControlArray* children = Children();
    if (!children)
    {
        // composite is the only thing on this level
        // it should be able to get focus
        return ETrue;
    }

    const TInt count(children->Count());
    if (count < 1)
    {
        // composite is the only thing on this level
        // it should be able to get focus
        return ETrue;
    }


    // check if there is any child who is visible and has composite interface
    TBool foundChildComposite(EFalse);
    MSwtControl* child(NULL);
    for (TInt i =0; i < count; i++)
    {
        child = (*children)[i];
        if (child->CompositeInterface()
                && child->CoeControl().IsVisible() && child->GetEnabled())
        {
            foundChildComposite = ETrue;
            break;
        }
    }

    // if there is a visible child composite
    // the composite should not get focus.
    // otherwise, it should get focus.
    return !foundChildComposite;
}

TInt CSwtComposite::FocusBackgroundPolicy() const
{
    return ENoFocusBackground;
}

TBool CSwtComposite::HasFocusableControl(const MSwtControl* aControl) const
{
    if (!(aControl->CompositeInterface()))
    {
        return aControl->IsFocusable(EFalse);
    }
    else
    {
        // in case of composite

        if (!(aControl->CoeControl().IsVisible()) || !(aControl->GetEnabled()))
        {
            return EFalse;
        }

        const RSwtControlArray* children = aControl->CompositeInterface()->Children();

        if (!children)
        {
            // composite with no child is unfocusable
            return EFalse;
        }

        const TInt count(children->Count());
        if (count == 0)
        {
            // composite with no child is unfocusable
            return EFalse;
        }
        // search focusable controls on its children
        TBool foundFocusableControl(EFalse);
        MSwtControl* child(NULL);
        for (TInt i =0; i < count; i++)
        {
            child = (*children)[i];
            if (HasFocusableControl(child))
            {
                foundFocusableControl = ETrue;
                break;
            }
        }

        if (foundFocusableControl)
        {
            return ETrue;
        }
    }
    return EFalse;
}

void CSwtComposite::Update() const
{
    if (iCtrlFlags & MSwtControl::EFlagDoNotDraw)
        return;

    ASwtControlBase::Update();

    const TInt count = iChildren.Count();

    for (TInt i = 0; i < count; ++i)
    {
        iChildren[i]->Update();
    }
}

void CSwtComposite::ProcessKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
{
    // Nothing to do
}

TBool CSwtComposite::SetSwtFocus(TInt aReason /*=KSwtFocusByApi*/)
{
    if (aReason != KSwtFocusByForce)
    {
        // Try to set focus to a child of ours
        const TInt count = iChildren.Count();
        for (TInt i=0; i<count; ++i)
        {
            MSwtControl* child = iChildren[i];
            if (!child->IsShell())
            {
                if (child->SetSwtFocus(aReason))
                    return ETrue;
            }
        }
    }

    if (IsShell())
    {
        // Called from CSwtShell::SetSwtFocus
        return EFalse;
    }
    else
    {
        return ASwtScrollableBase::SetSwtFocus(aReason);
    }
}

TSize CSwtComposite::ComputeSizeL(TInt /*aWHint*/, TInt /*aHHint*/)
{
    // If there is a layout then CaptionedControl doesn't call this method.

    // Search for the child which has the greatest bottom-right corner location.
    const TRect clientRect(ClientRect());
    TPoint maxBr(clientRect.iTl);

    const TInt count = iChildren.Count();
    for (TInt i=0; i<count; ++i)
    {
        const MSwtControl* child = iChildren[i];
        if (child->IsShell() == NULL)
        {
            TPoint br(child->CoeControl().Rect().iBr);
            if (br.iX > maxBr.iX)
                maxBr.iX = br.iX;
            if (br.iY > maxBr.iY)
                maxBr.iY = br.iY;
        }
    }

    // Relocate in client area coordinates, this yields the client size
    TSize clientSize((maxBr-clientRect.iTl).AsSize());

    // Add trimmings
    TRect rect(clientSize);
    ComputeTrim(rect);
    return rect.Size();
}

MSwtComposite* CSwtComposite::CompositeInterface() const
{
    const MSwtComposite* composite = this;
    return const_cast<MSwtComposite*>(composite);
}

void CSwtComposite::ComputeTabListL(RSwtControlArray& aList)
{
    if (!IsShell())
        User::LeaveIfError(aList.Append(this));

    const RSwtControlArray& array = (iTabList.Count()!=0) ? iTabList : iChildren;
    const TInt count = array.Count();
    for (TInt i=0; i<count; ++i)
    {
        MSwtControl* child = array[i];
        if (!child->IsShell()) // Child Shells cannot be reached through traversal
            array[i]->ComputeTabListL(aList);
    }
}

void CSwtComposite::UpdateDoNotDrawFlag()
{
    ASwtControlBase::UpdateDoNotDrawFlag();

    for (TInt i = 0; i < iChildren.Count(); ++i)
    {
        iChildren[i]->UpdateDoNotDrawFlag();
    }
}

void CSwtComposite::SetBounds(const TRect& aRect)
{
    // Divert the job to UiUtils if this is a parent to an editor
    // open for split view editing.
    MSwtUiUtils& utils = iDisplay.UiUtils();
    if (utils.SplitInputView() == this)
    {
        utils.SetSplitInputViewSize(aRect.Size());
        SetLocation(aRect.iTl);
    }
    else
    {
        ASwtScrollableBase::SetBounds(aRect);
    }
}

void CSwtComposite::SetWidgetSize(const TSize& aSize)
{
    // Divert the job to UiUtils if this is a parent to an editor
    // open for split view editing.
    MSwtUiUtils& utils = iDisplay.UiUtils();
    if (utils.SplitInputView() == this)
    {
        utils.SetSplitInputViewSize(aSize);
    }
    else
    {
        ASwtScrollableBase::SetWidgetSize(aSize);
    }
}

void CSwtComposite::HandleHighlightChange()
{
    TInt count = iChildren.Count();

    for (TInt i = 0; i < count; ++i)
    {
        MSwtControl* child = iChildren[i];
        if (!child->IsShell())
        {
            child->HandleHighlightChange();
        }
    }
}

void CSwtComposite::DrawScrollBarBackground(const TRect& aRect) const
{
    const CCoeControl& coeControl = CoeControl();
    CWindowGc& gc = SystemGc();
    MAknsControlContext* cc = SwtMopSupplyCtrlContext();

    if (HasScrollBars())
    {
        const TInt count = coeControl.CountComponentControls();
        for (TInt i = 0; i < count; ++i)
        {
            CCoeControl& compCtrl = *coeControl.ComponentControl(i);
            if (compCtrl.IsVisible() && !compCtrl.OwnsWindow())
            {
                TRect drawRect(compCtrl.Rect());
                drawRect.Intersection(aRect);
                if (!drawRect.IsEmpty())
                {
                    AknsDrawUtils::Background(AknsUtils::SkinInstance(), cc,
                                              &coeControl, gc, drawRect);
                }
            }
        }

        // Composite has both scrollbars
        if (count == KMaxScrollBarCount)
        {
            TRect clientRect = ClientRect();
            TRect drawRect = coeControl.Rect();

            drawRect.Move(clientRect.Width(), clientRect.Height());
            drawRect.Intersection(coeControl.Rect());

            AknsDrawUtils::Background(AknsUtils::SkinInstance(), cc,
                                      &coeControl, gc, drawRect);
        }
    }
}

CEikScrollBarFrame* CSwtComposite::SbFrame() const
{
    return iSbFrame;
}

void CSwtComposite::SetVisibleScrollBar(TInt aStyle, TBool aVisible)
{
    ASwtScrollableBase::SetVisibleScrollBar(aStyle, aVisible);
    if (aVisible)
    {
        TRAP_IGNORE(UpdateFrameL());
    }
}

MSwtScrollable* CSwtComposite::Scrollable()
{
    return this;
}

void CSwtComposite::RegisterChildL(MSwtControl& aChild)
{
    ASSERT(iChildren.Find(&aChild)==KErrNotFound);
    ASSERT(!aChild.CoeControl().IsFocused());
    // Only Shells are window owners
    ASSERT(aChild.IsShell() || !aChild.CoeControl().OwnsWindow());

    MSwtShell& owningShell = GetShell();
    MSwtShell* childShell  = aChild.ShellInterface();

    // Add to child list
    User::LeaveIfError(iChildren.Append(&aChild));

    if (childShell==NULL) // Shells are not regular children
    {
        // Take care of tab stopping
        if (iTabList.Count() != 0)
        {
            TInt error = iTabList.Append(&aChild);
            if (error)
            {
                TInt index = iChildren.Count() - 1;
                ASSERT(iChildren[index]==&aChild);
                iChildren.Remove(index);
                User::Leave(error);
            }
        }
        owningShell.ResetTabList();

        // Now that we have a child, we cannot be focused any longer
        if (!IsShell())  // Shells can't be focused anyway
        {
            SetFocusing(EFalse);
            FocusabilityChanged();
            if (IsFocused())
            {
                CAknControl::SetFocus(EFalse);
            }
        }
    }

    // Take care of focus
    if (UiUtils().GetActiveShell()==&owningShell)
    {
        if (owningShell.FocusControl()==NULL && aChild.IsFocusable(EFalse))
            aChild.CoeControl().SetFocus(ETrue);
    }

    // All controls must be located at (0,0) and be of size (0,0) after construction.
    // This is a good place to enforce the former and assert the latter. Shells ,of
    // course, are an exception.

    // WARNING! Never remove this step as some controls rely on the fact that
    // PositionChanged() will be called due to this!
    if (childShell == NULL)
    {
        aChild.CoeControl().SetPosition(ClientRect().iTl); // That's (0,0) in our client rect
        ASSERT(aChild.CoeControl().Size()==TSize(0,0));
    }

    aChild.CoeControl().SetMopParent(this);
}

// WARNING! Be extra careful if you need to override this in inheriting classes.
// This applies only to children of the given Composite, not children of children.
void CSwtComposite::UnregisterChild(MSwtControl& aChild)
{
    // Unregister default buttons, focus memory, etc.
    MSwtShell& shell = GetShell();
    if (shell.UrgentPaintControl() == &aChild)
    {
        shell.SetUrgentPaintControl(NULL);
    }

    if (&aChild == iScrlCompContent)
    {
        iScrlCompContent = NULL;
    }

    MSwtDecorations* deco = shell.Decorations();
    MSwtButton* button = aChild.ButtonInterface();
    if (button && button == deco->OriginalDefaultButton())
    {
        deco->SetDefaultButton(NULL);
    }
    shell.CheckFocusMemory(aChild);

    // Pass focus to next focusable controls
    if (aChild.CoeControl().IsFocused())
    {
        MSwtControl* newFocus = NULL;
        TRAP_IGNORE((newFocus = shell.FindTraversalTargetL(ESwtTraverseParent, aChild)));
        if (newFocus)
        {
            newFocus->CoeControl().SetFocus(ETrue);
        }
        else
        {
            aChild.CoeControl().SetFocus(EFalse);
        }
    }
    ASSERT(!aChild.CoeControl().IsFocused());

    // Remove from child list
    TInt index = iChildren.Find(&aChild);
    ASSERT(index != KErrNotFound);
    iChildren.Remove(index);

    // Take care of tab stopping
    if (iTabList.Count() != 0)
    {
        // Remove from tab list
        TInt tabIndex = iTabList.Find(&aChild);
        if (tabIndex != KErrNotFound)
            iTabList.Remove(tabIndex);
    }

    if (!aChild.ShellInterface())
    {
        shell.ResetTabList();
    }

    // If that was the last child, composite can be focused again
    if (iChildren.Count() == 0 && !IsDimmed() && !IsShell())
    {
        SetFocusing(ETrue);
        FocusabilityChanged();
    }

    // Disposing control during pointer event handling case
    if (iPointerGrabbingControl == &aChild)
    {
        iPointerGrabbingControl = NULL;
    }
}

CSwtPeerArray* CSwtComposite::GetChildrenPeersL()
{
    const TInt count = iChildren.Count();
    if (count == 0)
        return NULL;

    CSwtPeerArray* array = new(ELeave) CSwtPeerArray(count);
    CleanupStack::PushL(array);

    for (TInt i=0; i<count; ++i)
    {
        MSwtControl* child = iChildren[i];
        if (!child->IsShell())
            array->AppendL(child->JavaPeer());
    }

    CleanupStack::Pop(array);
    return array;
}

void CSwtComposite::ChildFocusChanged(MSwtControl& aControl)
{
    // Overloaded in CSwtShell
    ASSERT(iParent);
    iParent->ChildFocusChanged(aControl);
}

TInt CSwtComposite::CountFocusableChildren(TInt aTryBreakAfter /*= -1 */,
        const MSwtControl* aExclude /*= NULL */,
        TBool aVisualFocusCount /*= ETrue*/) const
{
    TInt res = 0;
    const RSwtControlArray& tabList = iTabList.Count() ? iTabList : iChildren;
    const TInt tabCount = tabList.Count();
    for (TInt i = 0; i < tabCount; ++i)
    {
        MSwtControl* ctrl = tabList[i];

        if (ctrl->IsShell())
        {
            continue;
        }

        // CaptionedControls are always focusable. Either themselves or their child.
        TBool captCtrlFocusable = EFalse;
        if (aVisualFocusCount && ctrl->CaptionedControlInterface())
        {
            if (ctrl->IsFocusable(EFalse))
            {
                captCtrlFocusable = ETrue;
            }
            else
            {
                // Count any focusable children even those who cannot draw focus backgrounds.
                if (ctrl->CompositeInterface()->CountFocusableChildren(1, NULL, EFalse) > 0)
                {
                    captCtrlFocusable = ETrue;
                }
            }
        }

        if ((captCtrlFocusable || ctrl->IsFocusable(EFalse))
                && ((!aVisualFocusCount) || (aVisualFocusCount && ctrl->FocusBackgroundPolicy() != ENoFocusBackground))
                && ((!aExclude) || (aExclude && ctrl != aExclude && !ctrl->IsDescentOf(*aExclude))))
        {
            res++;
            if (res >= aTryBreakAfter)
            {
                break;
            }
        }

        if (ctrl->CompositeInterface())
        {
            if (captCtrlFocusable)
            {
                continue;
            }
            res += ctrl->CompositeInterface()->CountFocusableChildren(aTryBreakAfter - res,
                    aExclude, aVisualFocusCount);
            if (res >= aTryBreakAfter)
            {
                break;
            }
        }
    }

    return res;
}

void CSwtComposite::SetTabListL(MSwtControl** aControls, TInt aCount)
{
    TRAPD(error, DoSetTabListL(aControls, aCount));
    if (error)
    {
        iTabList.Reset();
        User::Leave(error);
    }
}

void CSwtComposite::DoSetTabListL(MSwtControl** aControls, TInt aCount)
{
    iTabList.Reset();
    if (aCount > 0)
    {
        // Append specified controls (make sure not to add Shells)
        for (TInt i=0; i<aCount; ++i)
        {
            MSwtControl* ctrl = aControls[i];
            if (ctrl && !ctrl->IsShell())
                User::LeaveIfError(iTabList.Append(ctrl));
        }
    }
}

CSwtPeerArray* CSwtComposite::GetTabListL() const
{
    CSwtPeerArray* result = new(ELeave) CSwtPeerArray(KChildrenGranularity);
    CleanupStack::PushL(result);

    const RSwtControlArray& array = (iTabList.Count()!=0) ? iTabList : iChildren;
    const TInt count = array.Count();
    for (TInt i=0; i<count; ++i)
    {
        MSwtControl* ctrl = array[i];
        if (!ctrl->IsShell())
            result->AppendL(ctrl->JavaPeer());
    }

    CleanupStack::Pop(result);
    return result;
}

void CSwtComposite::MoveChildBelow(MSwtControl& aControl1, const MSwtControl* aControl2)
{
    const TInt count = iChildren.Count();
    const TInt pos1  = iChildren.Find(&aControl1);
    const TInt pos2  = (aControl2 == NULL) ? count-1 : iChildren.Find(aControl2);

    ASSERT(pos1 != KErrNotFound);

    if (pos1 < pos2)
    {
        iChildren.Remove(pos1);
        // This will not fail, the array hasn't been compressed
        iChildren.Insert(&aControl1, pos2);

        Redraw();
    }
}

void CSwtComposite::NotifyRadioGroup(MSwtButton* aSelectedButton)
{
    const TInt childCount = iChildren.Count();
    for (TInt i=0; i<childCount; ++i)
    {
        MSwtButton* child = iChildren[i]->ButtonInterface();
        if (child && child != aSelectedButton && (child->Control()->Style() & KSwtStyleRadio))
        {
            child->SetSelection(EFalse);
            // Notify the Selection
            TRAP_IGNORE(iDisplay.PostSelectionEventL(child->Control()->JavaPeer()));
        }
    }
}

MSwtControl* CSwtComposite::GetPointerGrabbingControl() const
{
    return iPointerGrabbingControl;
}

void CSwtComposite::MoveChildAbove(MSwtControl& aControl1, const MSwtControl* aControl2)
{
    const TInt pos1 = iChildren.Find(&aControl1);
    const TInt pos2 = (aControl2 == NULL) ? 0 : iChildren.Find(aControl2);

    ASSERT(pos1 != KErrNotFound);

    if (pos1 > pos2)
    {
        iChildren.Remove(pos1);
        // This will not fail, the array hasn't been compressed
        iChildren.Insert(&aControl1, pos2);

        Redraw();
    }
}

void CSwtComposite::SetScrolledCompositeContentL(MSwtControl* aContent)
{
    if (iPhysicsTimer && !aContent)
    {
        iPhysicsTimer->Cancel();
        delete iPhysicsTimer;
        iPhysicsTimer = NULL;
    }

    if (iPhysics)
    {
        delete iPhysics;
        iPhysics = NULL;
    }

    if (aContent && aContent->GetParent() == this)
    {
        iScrlCompContent = aContent;
    }
    else
    {
        iScrlCompContent = NULL;
    }

    if (PhysicsEnabled() && iScrlCompContent)
    {
        if (!iPhysicsLib.Handle())
        {
            _LIT(KLibName, "eswtphysics.dll");
            iPhysicsLib.Load(KLibName);
        }

        if (iPhysicsLib.Handle())
        {
            TSwtPhysicsLibEntry libEntryL = (TSwtPhysicsLibEntry)iPhysicsLib.Lookup(1);
            iPhysics = libEntryL(*this, iScrlCompContent->CoeControl());
        }

        if (iPhysics)
        {
            PhysicsResetGeometryL();
            if (!iPhysicsTimer)
            {
                iPhysicsTimer = CPeriodic::NewL(CActive::EPriorityStandard);
            }
        }
    }
}

MSwtControl* CSwtComposite::ScrolledCompositeContent() const
{
    return iScrlCompContent;
}

TInt CSwtComposite::ScrolledCompositePysicsAction() const
{
    return iPhysicsAction;
}

// From MAknPhysicsObserver
void CSwtComposite::ViewPositionChanged(const TPoint& aNewPosition,
                                        TBool /*aDrawNow*/, TUint /*aFlags*/)
{
    if (!iScrlCompContent || iPhysicsViewPos == aNewPosition)
    {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        iLastTactileFeedbackPos = iPhysicsViewPos.iY;
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        return;
    }

    CCoeControl& coeCtrl(iScrlCompContent->CoeControl());
    TRect worldRect(coeCtrl.Rect());   // window coords
    TInt worldWidth(worldRect.Width());
    TRect viewRect(ClientRect());   // window coords
    TInt viewWidth(viewRect.Width());
    TInt viewHeight(viewRect.Height());

    int x = worldRect.iTl.iX; // window coords
    int y = viewRect.iTl.iY + viewHeight / 2 - aNewPosition.iY; // window coords

    // Only panning on x
    if (iPhysicsAction == KSwtPhysicsPanning)
    {
        TInt min = viewRect.iTl.iX;
        x = min + viewWidth / 2 - aNewPosition.iX; // window coords
        if (x > min)
        {
            x = min;
        }
        else
        {
            if (viewWidth >= worldWidth)
            {
                x = min;
            }
            else
            {
                TInt max = min + viewWidth - worldWidth;
                if (x < max)
                {
                    x = max;
                }
            }
        }
    }

    coeCtrl.SetPosition(TPoint(x, y));
    iPhysicsViewPos = aNewPosition;

    if (iVScroll)
    {
        TInt pos = viewRect.iTl.iY - y; // composite coords
        pos = Min(pos, iVScroll->GetMaximum());
        pos = Max(pos, 0);
        TRAP_IGNORE(iVScroll->DoSetThumbPositionL(pos));
    }
    if (iHScroll)
    {
        TInt pos = viewRect.iTl.iX - x; // composite coords
        pos = Min(pos, iHScroll->GetMaximum());
        pos = Max(pos, 0);
        TRAP_IGNORE(iHScroll->DoSetThumbPositionL(pos));
    }
    // We have to always repaint the composite, since after setPosition
    // some child controls might redraw themselves. This can cause distortions
    // while panning the composite.
    PaintUrgently();

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    // Flicking or panning the composite content should give the feedback
    if (iPhysicsAction == KSwtPhysicsPanning ||
            iPhysicsAction == KSwtPhysicsFlicking)
    {
        DoScrollingFeedback();
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
}

// From MAknPhysicsObserver
void CSwtComposite::PhysicEmulationEnded()
{
    PaintUrgently();
    iPhysicsAction = KSwtPhysicsNoAction;
    GetShell().SetUrgentPaintControl(NULL);
}

// From MAknPhysicsObserver
TPoint CSwtComposite::ViewPosition() const
{
    return iPhysicsViewPos;
}

void CSwtComposite::DoHandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EButton1Down:
    {
        MSwtControl* focusDestination = NULL;
        const TInt childCount = iChildren.Count();
        for (TInt i = 0; i < childCount; i++)
        {
            CCoeControl& ctrl = iChildren[i]->CoeControl();
            if (!ctrl.IsVisible() || ctrl.IsDimmed() || iChildren[i]->IsShell())
            {
                continue;
            }
            if (ctrl.Rect().Contains(aPointerEvent.iPosition))
            {
                focusDestination = iChildren[i];
                break;
            }
        }
        if (focusDestination)
        {
            // Never change this!
            if (focusDestination->IsFocusable())
            {
                focusDestination->SetSwtFocus(KSwtFocusByPointer);
            }
            iPointerGrabbingControl = focusDestination;
            iPointerGrabbingControl->HandlePointerEventL(aPointerEvent);
        }
        else
        {
            ProcessCompositePointerEventL(aPointerEvent);
        }
    }
    break;

    case TPointerEvent::EButton1Up:
        if (iPointerGrabbingControl)
        {
            iPointerGrabbingControl->HandlePointerEventL(aPointerEvent);

            // Only mouse up event clears the grabbing control.
            iPointerGrabbingControl = NULL;
        }
        else
        {
            ProcessCompositePointerEventL(aPointerEvent);
        }
        break;

    default:
        if (iPointerGrabbingControl)
        {
            iPointerGrabbingControl->HandlePointerEventL(aPointerEvent);
        }
        else
        {
            ProcessCompositePointerEventL(aPointerEvent);
        }
        break;
    }
}

TBool CSwtComposite::PhysicsEnabled() const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    // For getting rid of using Aknphysics central repositary keys here
    // in 9.2 kinetic scrolling is enabled by default.
    return ETrue;
#else
    // Copied from aknphysicscrkeys.h
    const TUid KCRUidAknPhysicsSettings = { 0x20021192 };
    const TInt KFeatureEnabled(0x00000009);

    CRepository* cenRep = NULL;
    TRAPD(err, cenRep = CRepository::NewL(KCRUidAknPhysicsSettings))
    if (err || !cenRep)
    {
        return EFalse;
    }
    TBool featureEnabled(EFalse);
    TInt retValue = cenRep->Get(KFeatureEnabled, featureEnabled);
    delete cenRep;

    if (retValue == KErrNone)
    {
        return featureEnabled;
    }
    else
    {
        return EFalse;
    }
#endif
}

void CSwtComposite::PhysicsResetGeometryL()
{
    if (iPhysics && iScrlCompContent)
    {
        CCoeControl& coeCtrl(iScrlCompContent->CoeControl());
        TRect viewRect(ClientRect());   // window coords
        TPoint worldLoc(coeCtrl.Position() - viewRect.iTl);   // composite coords
        TSize worldSize(coeCtrl.Size());
        iPhysicsViewPos = TPoint(viewRect.Width() / 2 - worldLoc.iX,
                                 viewRect.Height() / 2 - worldLoc.iY);
        iPhysics->InitPhysicsL(worldSize, viewRect.Size(), EFalse);
    }
}

void CSwtComposite::PhysicsStartTimer()
{
    if (iPhysics && iPhysicsTimer)
    {
        iPhysicsTimer->Cancel();
        TTimeIntervalMicroSeconds32 timeout(iPhysics->HighlightTimeout() * 1000);
        iPhysicsTimer->Start(timeout, timeout,
                             TCallBack(CSwtComposite::PhysicsTimerCallback, this));
    }
}

TInt CSwtComposite::PhysicsTimerCallback(TAny* aPtr)
{
    CSwtComposite* self = static_cast<CSwtComposite*>(aPtr);
    if (self)
    {
        self->iPhysicsTimer->Cancel();
        TRAP_IGNORE(self->DoHandlePointerEventL(self->iPhysicsPointerDown));
    }
    return 0;
}

void CSwtComposite::PaintUrgently() const
{
    if (iScrlCompContent)
    {
        ASwtScrollableBase::PaintUrgently(Rect());
    }
}

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
void CSwtComposite::UpdateTactileFeedbackDensity()
{
    TAknLayoutRect layoutRect = CSwtLafFacade::GetLayoutRect(
                                    CSwtLafFacade::EListSingleHeadingPane,
                                    TRect(), 0, 0, 0);
    iTactileFeedbackDensity = layoutRect.Rect().Height();
}

void CSwtComposite::DoScrollingFeedback()
{
    if (!iScrlCompContent)
    {
        return;
    }

    TRect scrolledRect(iScrlCompContent->CoeControl().Rect());
    TRect thisRect(CoeControl().Rect());

    // Calculate distance between current content position
    // and previous position, where last feedback happened
    TInt lastFeedbackDistanceY = Abs(iLastTactileFeedbackPos
                                     - iPhysicsViewPos.iY);
    TTouchLogicalFeedback feedbackType(ETouchFeedbackNone);
    if (lastFeedbackDistanceY >= iTactileFeedbackDensity)
    {
        if (scrolledRect.iTl.iY < thisRect.iTl.iY &&
                scrolledRect.iBr.iY > thisRect.iBr.iY)
        {
            // If top (or bottom) edge of the content is outside
            // of visible area of composite, do feedback every time
            feedbackType = ETouchFeedbackSensitiveList;
        }
        else if (iLastPointerEventType != TPointerEvent::EButton1Up)
        {
            // Begining or end of the content reached (i.e. top or bottom
            // egde of the contont is now visible): feedback is given
            // only if user do panning. No feedback, if touch release
            // happened.
            feedbackType = ETouchFeedbackSensitiveList;
        }
        // Store the position of the content
        iLastTactileFeedbackPos = iPhysicsViewPos.iY;
    }

    if (iFeedback && feedbackType != ETouchFeedbackNone)
    {
        if (iLastPointerEventType != TPointerEvent::EButton1Up)
        {
            iFeedback->EnableFeedbackForControl(this, ETrue, ETrue);
        }
        else
        {
            // Disable audion feedback on flicking (i.e. user don't pan by
            // touch input)
            iFeedback->EnableFeedbackForControl(this, ETrue, EFalse);
        }
        // Do the feedback if needed
        iFeedback->InstantFeedback(this, feedbackType);
    }
}
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
