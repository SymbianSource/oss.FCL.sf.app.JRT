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


#include <gulicon.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <swtlaffacade.h>
#include <avkon.mbg>
#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK
#include "swtrotateimage.h"
#include "swtslider.h"


const TInt KSwtSliderDefaultThumb(1);

#ifdef RD_TACTILE_FEEDBACK
// Id for the only feedback area
const TInt KTouchFeedbackRectId1(0);
#endif // RD_TACTILE_FEEDBACK


// ======== MEMBER FUNCTIONS ========


/**
 * 1st & 2nd phase constructors wrapper
 */
CSwtSlider* CSwtSlider::NewL(MSwtDisplay& aDisplay,
                             TSwtPeer aPeer,
                             MSwtComposite& aParent,
                             TInt aStyle)
{
    CSwtSlider* self = new(ELeave) CSwtSlider(aDisplay, aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

CSwtSlider::CSwtSlider(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                       MSwtComposite& aParent, TInt aStyle)
        : ASwtControlBase(aDisplay, aPeer, &aParent, aStyle)
        , iMaximum(10)
        , iIncrement(1)
        , iPageIncrement(5)
{
    SetComponentsToInheritVisibility(ETrue);
}


CSwtSlider::~CSwtSlider()
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->RemoveFeedbackForControl(this);
    }
#endif // RD_TACTILE_FEEDBACK
    delete iImagesRotator;
    delete iSliderImageMask;
    delete iSliderImage;
}

/**
 * 2nd phase constructor
 */
void CSwtSlider::ConstructL()
{
    // Get parent
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);

    if (iStyle & KSwtStyleVertical)
    {
        iImagesRotator = CAORotateImage::NewL();

        iIncreaseKey = EKeyUpArrow;
        iDecreaseKey = EKeyDownArrow;
    }
    else if (iStyle & KSwtStyleHorizontal)
    {
        if (AknLayoutUtils::LayoutMirrored())
        {
            iIncreaseKey = EKeyLeftArrow;
            iDecreaseKey = EKeyRightArrow;
        }
        else
        {
            iIncreaseKey = EKeyRightArrow;
            iDecreaseKey = EKeyLeftArrow;
        }
    }

    SetPosition(TPoint(0, 0));
    SetSize(TSize(0, 0));

    // Synchronize with parent's state
    CAknControl::MakeVisible(coeParent.IsVisible());
    CAknControl::SetDimmed(coeParent.IsDimmed());

    SetBackground(this);   // Back will be drawn by ASwtControlBase::Draw

    // Ready to be drawn
    ActivateL();

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    // Disabling feedback for CSwtSlider.
    // When enabled - first tap on any slider position creates extra
    // feedback, that is not needed.
    {
        iFeedback->EnableFeedbackForControl(this, EFalse);
    }
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#endif // RD_TACTILE_FEEDBACK
}


/**
 * Slider image is composed manually here because we want to be able to
 * scale it to fit the control's size.
 */
CGulIcon* CSwtSlider::CreateSliderIconL(TInt aValue, TInt aMinValue, TInt aMaxValue)
{
    CGulIcon* icon = CGulIcon::NewLC();


    // The function used to get the layout data
    TAknLayoutRect(&Laf)(CSwtLafFacade::TSwtLafFacadeRectId,
                         const TRect&,
                         TInt,
                         TInt,
                         TInt) = CSwtLafFacade::GetLayoutRect;

    // Get slider pane layout
    TRect dummyParentRect(TPoint(0, 0), GetPreferredSliderSize());
    // Scale the layout to the size of the control.
    // We refuse to make it smaller than the Min( layoutwidth, layoutheight ).
    TRect clientRect(ClientRect());
    if (iStyle & KSwtStyleVertical)
    {
        // We always build horizontal image, it will be rotated later.
        dummyParentRect.SetWidth(Max(clientRect.Height(), dummyParentRect.Height()));
    }
    else
    {
        dummyParentRect.SetWidth(Max(clientRect.Width(), dummyParentRect.Height()));
    }
    TRect sliderPane(Laf(CSwtLafFacade::ESliderSetPaneCP3, dummyParentRect, 0, 0, 0).Rect());

    // Get background graphic layout
    TRect topLeftRect(Laf(CSwtLafFacade::EBgSetOptPaneG2, sliderPane, 0, 0, 0).Rect());
    TRect bottomRightRect(Laf(CSwtLafFacade::EBgSetOptPaneG5, sliderPane, 0, 0, 0).Rect());
    TRect outerRect(dummyParentRect);
    TRect innerRect(topLeftRect.Width(),
                    topLeftRect.Height(),
                    outerRect.Width() - bottomRightRect.Width(),
                    outerRect.Height() - bottomRightRect.Height());

    // Get line graphic layout
    TRect lineRect(Laf(CSwtLafFacade::ESliderSetPaneG1, sliderPane, 0, 0, 0).Rect());

    // Get marker graphic layout (size only actually)
    TRect markerRect(Laf(CSwtLafFacade::ESliderSetPaneG2, sliderPane, 0, 0, 0).Rect());

    // Get marker graphic
    CFbsBitmap* marker = NULL;
    CFbsBitmap* markerMask = NULL;
    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(),
                            KAknsIIDQgnIndiSliderSet,
                            marker, markerMask,
                            AknIconUtils::AvkonIconFileName(),
                            EMbmAvkonQgn_indi_slider_edit,
                            EMbmAvkonQgn_indi_slider_edit_mask);
    AknIconUtils::SetSize(marker, markerRect.Size());

    // Get line graphic
    CFbsBitmap* line = NULL;
    CFbsBitmap* lineMask = NULL;
    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(),
                            KAknsIIDQgnGrafLinePrimaryHorizontal,
                            line, lineMask,
                            AknIconUtils::AvkonIconFileName(),
                            EMbmAvkonQgn_graf_line_primary_horizontal,
                            EMbmAvkonQgn_graf_line_primary_horizontal_mask);
    AknIconUtils::SetSize(line,
                          lineRect.Size(),
                          EAspectRatioNotPreserved);

    // Create a bitmap to draw the slider components on to
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Create(
                           dummyParentRect.Size(), iDisplay.CoeEnv()->ScreenDevice()->DisplayMode()));

    // Create a gc for drawing to the bitmap
    CFbsBitGc* fbsBitGc = CFbsBitGc::NewL();
    CleanupStack::PushL(fbsBitGc);
    CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(bitmap);
    CleanupStack::PushL(bmpDevice);
    fbsBitGc->Activate(bmpDevice);

    // The actual drawing:

    // Draw the background graphic
    const TAknsItemID *frameId = &KAknsIIDQsnFrSetOptFoc;
    CAknsFrameBackgroundControlContext* cc =
        CAknsFrameBackgroundControlContext::NewL(*frameId, outerRect, innerRect, EFalse);
    AknsDrawUtils::DrawBackground(AknsUtils::SkinInstance(), cc, NULL, *fbsBitGc,
                                  outerRect.iTl, outerRect, KAknsDrawParamDefault);
    delete cc;

    // Draw the line graphic
    fbsBitGc->BitBltMasked(lineRect.iTl,
                           line,
                           lineRect.Size(),
                           lineMask,
                           EFalse);

    // Draw the marker graphic
    TInt bitmapRun = lineRect.Width() - marker->SizeInPixels().iWidth;
    TInt range = aMaxValue - aMinValue;
    TInt pos = ((bitmapRun * (aValue - aMinValue)) / range);
    TPoint markerPos = TPoint(pos + markerRect.iTl.iX, markerRect.iTl.iY);
    TRect srcRect(0, 0, markerRect.Width(), markerRect.Height());
    fbsBitGc->BitBltMasked(markerPos, marker, srcRect,
                           markerMask, EFalse);

    CleanupStack::PopAndDestroy(bmpDevice);
    CleanupStack::PopAndDestroy(fbsBitGc);

    icon->SetBitmap(bitmap);

    // Then create the mask
    CFbsBitmap* mask = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(mask);

    TDisplayMode mode = lineMask->DisplayMode();
    if (mode == ENone)
    {
        mode = EGray256;
    }
    User::LeaveIfError(mask->Create(bitmap->SizeInPixels(), mode));

    // A gc for drawing the mask
    fbsBitGc = CFbsBitGc::NewL();
    CleanupStack::PushL(fbsBitGc);
    bmpDevice = CFbsBitmapDevice::NewL(mask);
    fbsBitGc->Activate(bmpDevice);

    // Write mask data
    fbsBitGc->SetPenStyle(CGraphicsContext::ENullPen);
    fbsBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    fbsBitGc->SetBrushColor(KRgbWhite);
    fbsBitGc->DrawRect(TRect(mask->SizeInPixels()));
    AknsDrawUtils::DrawFrame(AknsUtils::SkinInstance(), *fbsBitGc,
                             outerRect, innerRect,
                             KAknsIIDQsnFrSetOptFoc,
                             KAknsIIDQsnFrSetOptFocCenter,
                             KAknsSDMAlphaOnly);
    delete bmpDevice;
    CleanupStack::PopAndDestroy(fbsBitGc);

    icon->SetMask(mask);

    CleanupStack::Pop(mask);
    CleanupStack::Pop(bitmap);
    CleanupStack::PopAndDestroy(4);   // line, lineMask, marker, markerMask
    CleanupStack::Pop(icon);

    return icon;
}


TSize CSwtSlider::GetPreferredSliderSize() const
{
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    rect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListScrollGenPane, rect, 0).Rect();
    rect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListGenPane, rect, 0).Rect();
    rect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListSettingNumberPane, rect, 0).Rect();
    rect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESetValuePane, rect, 0).Rect();
    return rect.Size();
}

void CSwtSlider::UpdateTouchFeedbackRect() const
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->SetFeedbackArea(this, KTouchFeedbackRectId1, Rect(),
                                   ETouchFeedbackBasic, ETouchEventStylusDown);
    }
#endif //RD_TACTILE_FEEDBACK
}

void CSwtSlider::UpdateSliderImageL()
{
    if (iStyle & KSwtStyleVertical)
    {
        iImagesRotator->RemoveAllImages();
    }

    if (iSliderImage)
    {
        delete iSliderImage;
    }

    if (iSliderImageMask)
    {
        delete iSliderImageMask;
    }

    TInt nativeSliderMax = iMaximum - KSwtSliderDefaultThumb;

    // If native slider maximum value is the same as minimum value, increment the
    // value by one to prevent icon creation from failing.
    if (iMinimum == nativeSliderMax)
    {
        nativeSliderMax++;
    }

    CGulIcon* icon = CreateSliderIconL(iSelection, iMinimum, nativeSliderMax);
    CleanupStack::PushL(icon);

    icon->SetBitmapsOwnedExternally(ETrue);
    iSliderImage = icon->Bitmap();
    iSliderImageMask = icon->Mask();

    CleanupStack::PopAndDestroy(); // icon

    iImageSize = iSliderImage->SizeInPixels();

    if (iStyle & KSwtStyleVertical)
    {
        // Update the image size before the rotation of the image.
        iImageSize.SetSize(iImageSize.iHeight, iImageSize.iWidth);

        iImagesRotator->AddImageL(iSliderImage);
        iImagesRotator->AddImageL(iSliderImageMask);

        iImagesRotator->Start(this);
    }
    else
    {
        if (!(iCtrlFlags  & MSwtControl::EFlagDoNotDraw))
        {
            if (IsPaintingUrgently())
            {
                PaintUrgently(Rect()) ;
            }
            else
            {
                RWindow* window = static_cast<RWindow*>(DrawableWindow());
                TRect dirtyRect(Rect());
                dirtyRect.Move(window->Position());
                Paint(dirtyRect);
            }
        }
    }
}

void CSwtSlider::AdjustSelectionValue()
{
    TInt value = iSelection;

    if (value > (iMaximum-KSwtSliderDefaultThumb))
    {
        value = iMaximum - KSwtSliderDefaultThumb;
    }
    else if (value < iMinimum)
    {
        value = iMinimum;
    }

    iSelection = value;
}

void CSwtSlider::ClipAndDrawImage(CWindowGc& aGC, const TRect& /*aRect*/) const
{
    aGC.SetOpaque(EFalse);
    TRect clientRect(ClientRect());
    TPoint imagePosition(clientRect.iTl);
    TRect rectImageSrc(0, 0, iImageSize.iWidth, iImageSize.iHeight);

    if (clientRect.Width() >= iImageSize.iWidth)
    {
        imagePosition.iX += (clientRect.Width() - iImageSize.iWidth) / 2;
    }
    else
    {
        rectImageSrc.iTl.iX = iImageSize.iWidth / 2 - clientRect.Width() / 2;
        rectImageSrc.SetWidth(clientRect.Width());
    }

    if (clientRect.Height() >= iImageSize.iHeight)
    {
        imagePosition.iY += (clientRect.Height() - iImageSize.iHeight) / 2;
    }
    else
    {
        rectImageSrc.iTl.iY = iImageSize.iHeight / 2 - clientRect.Height() / 2;
        rectImageSrc.SetHeight(clientRect.Height());
    }

    aGC.BitBltMasked(imagePosition, iSliderImage, rectImageSrc, iSliderImageMask, EFalse);
}

//
// Virtual methods from CCoeControl
//
TSize CSwtSlider::MinimumSize()
{
    return BorderOuterRect(iImageSize).Size();
}

void CSwtSlider::MakeVisible(TBool aVisible)
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->EnableFeedbackForControl(this, aVisible);
    }
#endif // RD_TACTILE_FEEDBACK
    CAknControl::MakeVisible(aVisible);

    FocusabilityChanged();
}

void CSwtSlider::SetDimmed(TBool aDimmed)
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->EnableFeedbackForControl(this, !aDimmed);
    }
#endif // RD_TACTILE_FEEDBACK
    CAknControl::SetDimmed(aDimmed);

    FocusabilityChanged();
}

TKeyResponse CSwtSlider::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TKeyResponse keyReponse = EKeyWasNotConsumed;

    if (aType == EEventKey)
    {
        if ((aKeyEvent.iCode == iIncreaseKey) || (aKeyEvent.iCode == iDecreaseKey))
            keyReponse = EKeyWasConsumed;

    }

    if (keyReponse == EKeyWasNotConsumed)
        return HandleKeyL(aKeyEvent, aType, ETrue);
    else
        return HandleKeyL(aKeyEvent, aType, EFalse);
}

void CSwtSlider::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}

void CSwtSlider::SwtHandleResourceChangeL(TInt aType)
{
    CAknControl::HandleResourceChange(aType);
    // Check all type which can modify the Slider
    //
    if (aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch)
    {
        UpdateSliderImageL();
    }
}

void CSwtSlider::Draw(const TRect& aRect) const
{
    if (iStyle & KSwtStyleVertical)
    {
        if (!iImagesRotator->IsFinish())
        {
            iImagesRotator->SetRedrawAfterRotation(ETrue);
            return;
        }
    }

    ClipAndDrawImage(SystemGc(), aRect);
}

void CSwtSlider::FocusChanged(TDrawNow aDrawNow)
{
    CAknControl::FocusChanged(ENoDrawNow);
    // Spread focus information
    HandleFocusChanged(aDrawNow);
}

void CSwtSlider::SizeChanged()
{
    CAknControl::SizeChanged();
    UpdateTouchFeedbackRect();
    TRAP_IGNORE(UpdateSliderImageL());
    HandleSizeChanged();
}

void CSwtSlider::PositionChanged()
{
    UpdateTouchFeedbackRect();
    HandlePositionChanged();
}

TTypeUid::Ptr CSwtSlider::MopSupplyObject(TTypeUid aId)
{
    TTypeUid::Ptr id = ASwtControlBase::SwtMopSupplyObject(aId);

    if (id.Pointer() == NULL)
    {
        return CAknControl::MopSupplyObject(aId);
    }
    else
    {
        return id;
    }
}

#ifdef RD_SCALABLE_UI_V2

void CSwtSlider::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if ((aPointerEvent.iType == TPointerEvent::EButton1Down ||
            aPointerEvent.iType == TPointerEvent::EDrag))
    {
        TInt betweenSteps;
        TInt value = CalcAlignedValue(aPointerEvent.iPosition - ClientRect().iTl);

        // Calculate if we have ended at a valid slider step
        betweenSteps = value % iIncrement;

        if (betweenSteps != 0)
        {
            value = value - betweenSteps;

            // if click was nearer or middle of values, then move it to
            // next possible value
            if (betweenSteps > (iIncrement / 2))
            {
                value = value + iIncrement;
            }
        }

#ifdef RD_TACTILE_FEEDBACK
        if (iFeedback && aPointerEvent.iType == TPointerEvent::EDrag &&
                value != iSelection)
        {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            iFeedback->InstantFeedback(ETouchFeedbackSlider);
#else
            // Give sensitive feedback when selection changes by dragging.
            iFeedback->InstantFeedback(ETouchFeedbackSensitive);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        }

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        if (iFeedback && aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            iFeedback->InstantFeedback(ETouchFeedbackSlider);
        }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

#endif //RD_TACTILE_FEEDBACK
        SetSelection(value);
    }

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    if (iFeedback && aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        iFeedback->InstantFeedback(ETouchFeedbackSlider);
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    PostMouseEventL(aPointerEvent);
}

TInt CSwtSlider::CalcAlignedValue(const TPoint& aPoint)
{
    TInt bitmapRun = 0;
    TInt returnValue = iMinimum;
    TInt sliderOrigin = 0;

    // Take into account the marker on the slider and extra space on the each side
    // of the slider
    TInt markerSize = CSwtLafFacade::GetLayoutRect(
                          CSwtLafFacade::ESliderSetPaneG2, TRect()).Rect().Size().iWidth;

    TAknLayoutRect layoutRect =
        CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESliderSetPaneCP3, TRect());
    TSize lineSize = CSwtLafFacade::GetLayoutRect(
                         CSwtLafFacade::ESliderSetPaneG1, layoutRect.Rect()).Rect().Size();

    if (lineSize.iWidth < 0)
    {
        lineSize.iWidth = -lineSize.iWidth;
    }

    markerSize += lineSize.iWidth;

    if (iStyle & KSwtStyleHorizontal)
    {
        // Calculate the start coordinate of the actual slider inside the
        // control
        sliderOrigin = (ClientRect().Size().iWidth - iImageSize.iWidth) / 2;

        // Size of the active part of the slider
        bitmapRun = iImageSize.iWidth - markerSize;

        if (bitmapRun == 0)
        {
            bitmapRun = 1;
        }

        // Calculate which slider selection is the closest to the pointer location
        returnValue = (((iMaximum-iMinimum) * (aPoint.iX - sliderOrigin -
                                               markerSize / 2)) / bitmapRun) + iMinimum;
    }
    else
    {
        sliderOrigin = (ClientRect().Size().iHeight - iImageSize.iHeight) / 2;

        bitmapRun = iImageSize.iHeight - markerSize;

        if (bitmapRun == 0)
        {
            bitmapRun = 1;
        }

        returnValue = (((iMaximum-iMinimum) * (aPoint.iY - sliderOrigin -
                                               markerSize / 2)) / bitmapRun) + iMinimum;

        returnValue = iMaximum - returnValue - 1;
    }

    return returnValue;
}

#endif // RD_SCALABLE_UI_V2

//
// Virtual methods from MSwtControl
//

CCoeControl& CSwtSlider::CoeControl()
{
    return *this;
}

const CCoeControl& CSwtSlider::CoeControl() const
{
    return *this;
}

void CSwtSlider::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (aType != EEventKey)
    {
        return;
    }

    TInt increment = (aKeyEvent.iRepeats == 1)?iPageIncrement : iIncrement;

    const TInt code(aKeyEvent.iCode);

    if (code == iDecreaseKey)
    {
        SetSelection(iSelection - increment);
    }
    else if (code == iIncreaseKey)
    {
        SetSelection(iSelection + increment);
    }
}


TSize CSwtSlider::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return TSize(aWHint, aHHint);
    }

    TSize preferredSize(GetPreferredSliderSize());
    if (iStyle & KSwtStyleVertical)
    {
        TInt tmp(preferredSize.iHeight);
        preferredSize.iHeight = preferredSize.iWidth;
        preferredSize.iWidth = tmp;
    }
    TSize borderDelta(BorderSizeDelta());

    TInt preferredHeight = preferredSize.iHeight + borderDelta.iHeight;
    TInt preferredWidth = preferredSize.iWidth + borderDelta.iWidth;

    preferredSize.iWidth = (aWHint == KSwtDefault) ? preferredWidth : aWHint;
    preferredSize.iHeight = (aHHint == KSwtDefault) ? preferredHeight : aHHint;

    // Leave space around the slider so that control focus highlight can be
    // seen even when slider graphics are not transparent.
    TRect innerRect(CoeControl().Rect());
    TRect outerRect(innerRect);
    TAknLayoutRect topLeft = CSwtLafFacade::GetLayoutRect(
                                 CSwtLafFacade::EInputFieldSkinPlacingGeneralLine2, CoeControl().Rect(), 0);
    TRect rectTopLeft(topLeft.Rect());
    innerRect.iTl = rectTopLeft.iBr;
    TAknLayoutRect layoutRect = CSwtLafFacade::GetLayoutRect(
                                    CSwtLafFacade::EInputFieldSkinPlacingGeneralLine5, CoeControl().Rect(), 0);
    TRect rectLayout(layoutRect.Rect());
    innerRect.iBr = rectLayout.iTl;
    if (iStyle & KSwtStyleVertical)
    {
        preferredSize.iWidth += ((innerRect.iTl.iX - outerRect.iTl.iX) * 2);
    }
    else
    {
        preferredSize.iHeight += ((innerRect.iTl.iY - outerRect.iTl.iY) * 2);
    }

    if (aWHint != KSwtDefault)
    {
        preferredSize.iWidth = aWHint;
    }
    if (aHHint != KSwtDefault)
    {
        preferredSize.iHeight = aHHint;
    }

    return preferredSize;
}

//
// Virtual methods from MSwtSlider
//

MSwtControl* CSwtSlider::Control()
{
    return this;
}

TInt CSwtSlider::GetMaximum() const
{
    return iMaximum;
}

TInt CSwtSlider::GetMinimum() const
{
    return iMinimum;
}

TInt CSwtSlider::GetPageIncrement() const
{
    return iPageIncrement;
}

TInt CSwtSlider::GetIncrement() const
{
    return iIncrement;
}

TInt CSwtSlider::GetSelection() const
{
    return iSelection;
}

void CSwtSlider::SetMaximum(TInt aValue)
{
    if (aValue != iMaximum && aValue > iMinimum)
    {
        iMaximum = aValue;

        AdjustSelectionValue();
        TRAP_IGNORE(UpdateSliderImageL());
    }
}

void CSwtSlider::SetPageIncrement(TInt aValue)
{
    if (aValue > 0)
    {
        iPageIncrement = aValue;
    }
}

void CSwtSlider::SetMinimum(TInt aValue)
{
    if (aValue != iMinimum && aValue >= 0 && aValue < iMaximum)
    {
        iMinimum = aValue;

        AdjustSelectionValue();
        TRAP_IGNORE(UpdateSliderImageL());
    }
}

void CSwtSlider::SetIncrement(TInt aValue)
{
    iIncrement = (aValue < 1) ? 1 : aValue;
}

void CSwtSlider::SetSelection(TInt aValue)
{
    TInt previousSelection = iSelection;
    iSelection = aValue;

    AdjustSelectionValue();

    if (iSelection != previousSelection)
    {
        TRAP_IGNORE(UpdateSliderImageL());
        TRAP_IGNORE(iDisplay.PostSelectionEventL(iPeer));
    }
}

void CSwtSlider::SetValues(TInt aSelection,
                           TInt aMinimum,
                           TInt aMaximum,
                           TInt aIncrement,
                           TInt aPageIncrement)
{
    if (aMinimum >= 0 && aMinimum < aMaximum)
    {
        iMinimum = aMinimum;
        iMaximum = aMaximum;
    }

    SetIncrement(aIncrement);
    SetPageIncrement(aPageIncrement);

    SetSelection(aSelection);
}

TBool CSwtSlider::SetSwtFocus(TInt aReason /*= KSwtFocusByApi*/)
{
    TBool prevFocused = IsFocusControl();
    TBool res = ASwtControlBase::SetSwtFocus(aReason);

    // Gaines focus by pointer
    if (IsFocusControl() && !prevFocused)
    {
        iFocusChanged = ETrue;
    }

    return res;
}
