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


#include <eikimage.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <swtlaffacade.h>
#include "swtcontrolhelper.h"
#include "swtfont.h"
#include "swtcomposite.h"
#include "swtcaptionedcontrol.h"


// Default preferred size for an empty CaptionedControl
const TInt KSwtCaptionedCtrlDefPrefSize = 50;
const TInt KSwtDefaultCaptionMargin = 3;
const TInt KSwtDefaultTrailMargin = 3;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CControlProxy::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CSwtCaptionedControl::CControlProxy::CountComponentControls() const
{
    return iChild ? 1 : 0;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CControlProxy::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtCaptionedControl::CControlProxy::ComponentControl(
    TInt /*aIndex*/) const
{
    ASSERT(iChild);
    return &(iChild->CoeControl());
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CControlProxy::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CSwtCaptionedControl::CControlProxy::MinimumSize()
{
    TSize size(0, 0);
    if (iChild)
    {
        // Use the computed size from the Java side. This way cover also
        // those components that compute their size on the Java side.
        size = iPreferredSize;
    }
    return size;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CControlProxy::PositionChanged
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::CControlProxy::PositionChanged()
{
    iPrevPos = iPosition;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CControlProxy::SizeChanged
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::CControlProxy::SizeChanged()
{
    if (iPosition != iPrevPos)
    {
        if (iChild)
        {
            iChild->CoeControl().SetPosition(iPosition);
        }
        iPrevPos = iPosition;
    }

    if (iChild)
    {
        // Divert the job to UiUtils if this is an editor open for split view editing.
        MSwtUiUtils& utils = iChild->GetShell().Display().UiUtils();
        if (utils.SplitInputView() == iChild)
        {
            utils.SetSplitInputViewSize(iSize);
        }
        else
        {
            iChild->CoeControl().SetSize(iSize);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::NewL
// ---------------------------------------------------------------------------
//
CSwtCaptionedControl* CSwtCaptionedControl::NewL(MSwtDisplay& aDisplay,
        TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle)
{
    CSwtCaptionedControl* self = new(ELeave) CSwtCaptionedControl(aDisplay,
            aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CSwtCaptionedControl
// ---------------------------------------------------------------------------
//
CSwtCaptionedControl::CSwtCaptionedControl(MSwtDisplay& aDisplay,
        TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle)
        : ASwtControlBase(aDisplay, aPeer, &aParent, aStyle)
{
    CCoeControl& coeParent = aParent.Control()->CoeControl();
    CEikCaptionedControl::MakeVisible(coeParent.IsVisible());
    CEikCaptionedControl::SetDimmed(coeParent.IsDimmed());
    SetComponentsToInheritVisibility(ETrue);
    SetFocusing(ETrue);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::~CSwtCaptionedControl
// ---------------------------------------------------------------------------
//
CSwtCaptionedControl::~CSwtCaptionedControl()
{
    // Own
    ASSERT(iChildren.Count() == 0);
    iChildren.Close();
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::ConstructL()
{
    SetContainerWindowL(GetParent()->Control()->CoeControl());

    // Workaround - We need to set that now, so as the
    // ConstructExtensionL is called inside the CaptionnedControl.
    SetEditableL(EFalse);

    // Retrieve default font
    iDefaultFont = CSwtFont::NewL(iDisplay.Device(), iDisplay.Device().GetSystemFont()->Font());

    iControl = new(ELeave) CControlProxy;
    iControl->SetComponentsToInheritVisibility(ETrue);
    iControl->SetContainerWindowL(*this); // Absolutely necessary
    iControl->SetMopParent(this); // Absolutely necessary
    iControl->SetFocusing(ETrue);
    iControl->ActivateL();

    SetBackground(this);    // Back will be drawn by ASwtControlBase::Draw
    ActivateL();

    UpdateTextColor();
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::Child
// ---------------------------------------------------------------------------
//
inline const MSwtControl* CSwtCaptionedControl::Child() const
{
    ASSERT(iControl);
    return static_cast<CControlProxy*>(iControl)->iChild;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::Child
// ---------------------------------------------------------------------------
//
inline MSwtControl* CSwtCaptionedControl::Child()
{
    ASSERT(iControl);
    return static_cast<CControlProxy*>(iControl)->iChild;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CoeChild
// ---------------------------------------------------------------------------
//
inline const CCoeControl* CSwtCaptionedControl::CoeChild() const
{
    const MSwtControl* child = Child();
    return (child) ? &child->CoeControl() : NULL;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CoeChild
// ---------------------------------------------------------------------------
//
inline CCoeControl* CSwtCaptionedControl::CoeChild()
{
    MSwtControl* child = Child();
    return (child) ? &child->CoeControl() : NULL;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetChild
// ---------------------------------------------------------------------------
//
inline void CSwtCaptionedControl::SetChild(MSwtControl* aChild)
{
    ASSERT(iControl);
    static_cast<CControlProxy*>(iControl)->iChild = aChild;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ComputeInnerRect
// ---------------------------------------------------------------------------
//
TRect CSwtCaptionedControl::ComputeInnerRect(const TRect& aRect) const
{
    TRect borderInnerRect(aRect);
    TRect innerRect(aRect);

    // Compute rect inside borders
    TInt borderWidth = GetBorderWidth();
    borderInnerRect.Shrink(borderWidth, borderWidth);

    // Compute rect inside frame
    innerRect.iTl = CSwtLafFacade::GetLayoutRect(
                        CSwtLafFacade::EInputFieldSkinPlacingGeneralLine2, aRect).Rect().iBr;
    innerRect.iBr = CSwtLafFacade::GetLayoutRect(
                        CSwtLafFacade::EInputFieldSkinPlacingGeneralLine5, aRect).Rect().iTl;

    // Merge computed rects
    if (innerRect.iTl.iX < borderInnerRect.iTl.iX)
    {
        innerRect.iTl.iX = borderInnerRect.iTl.iX;
    }
    if (innerRect.iTl.iY < borderInnerRect.iTl.iY)
    {
        innerRect.iTl.iY = borderInnerRect.iTl.iY;
    }
    if (innerRect.iBr.iX > borderInnerRect.iBr.iX)
    {
        innerRect.iBr.iX = borderInnerRect.iBr.iX;
    }
    if (innerRect.iBr.iY > borderInnerRect.iBr.iY)
    {
        innerRect.iBr.iY = borderInnerRect.iBr.iY;
    }

    innerRect.Normalize();

    return innerRect;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::DoSetSize
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::DoSetSize(const TSize& aSize)
{
    // Compute new inner rect
    TRect newRect(Rect());
    newRect.SetSize(aSize);
    iInnerRect = ComputeInnerRect(newRect);

    // Give iInnerRect to CEikCaptionedControl SetSize
    TPoint realPosition(iPosition);
    iPosition = iInnerRect.iTl;

    // Save aSize before calling SetSize because it will modify iSize and aSize
    // may be a reference to iSize. In this case, aSize would be modified
    // whereas we want to use it as parameter of SetSizeWithoutNotification.
    TSize realSizeToSet(aSize);
    CEikCaptionedControl::SetSize(iInnerRect.Size());

    // Set our rect to its real value
    iPosition = realPosition;
    CEikCaptionedControl::SetSizeWithoutNotification(realSizeToSet);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::GetRecursiveChildrenL
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::GetRecursiveChildrenL(RSwtControlArray& aAllChildren,
        MSwtComposite& aParent)
{
    const RSwtControlArray* children = aParent.Children();
    if (children)
    {
        for (TInt i = 0; i < children->Count(); ++i)
        {
            MSwtControl* child = (*children)[i];
            ASSERT(child);
            User::LeaveIfError(aAllChildren.Append(child));
            MSwtComposite* composite = child->CompositeInterface();
            if (composite)
            {
                GetRecursiveChildrenL(aAllChildren, *composite);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::RealFontHeight
// ---------------------------------------------------------------------------
//
TBool CSwtCaptionedControl::RealFontHeight(TInt& aHeight) const
{
    const MSwtFont* swtFont = GetFont();
    ASSERT(swtFont);
    const CFont& font = swtFont->Font();
    const CFbsFont* fbsFont = NULL;
    if (font.TypeUid() == KCFbsFontUid)
    {
        fbsFont = static_cast<const CFbsFont*>(&font);
    }

    if (fbsFont && fbsFont->IsOpenFont())
    {
        // font.AscentInPixels() and font.DescentInPixels() ( used by iCaption
        // and iTrailer ( CEikLabel ) ) don't include the height of accents etc.
        // Use the maximum possible ascent/decent instead.
        aHeight = font.FontMaxHeight();
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::DefaultLeftMargin
// ---------------------------------------------------------------------------
//
TInt CSwtCaptionedControl::DefaultLeftMargin() const
{
#if 0
    TRect dummyRect(0, 0, KSwtCaptionedCtrlDefPrefSize,
                    KSwtCaptionedCtrlDefPrefSize);
    return CSwtLafFacade::GetLayoutText(
               CSwtLafFacade::EFormDataWideGraphicFieldTextsLine2, dummyRect).
           TextRect().iTl.iX;
#endif
    return 0;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtCaptionedControl::CountComponentControls() const
{
    // We don't want iControl to be counted as a component control
    CCoeControl* controls[] = { iBitmap, iCaption, iTrailer };
    TInt count = 0;
    TInt nrOfCtrls = sizeof(controls) / sizeof(CCoeControl*);
    for (TUint i = 0; i < nrOfCtrls; i++)
    {
        if (controls[i])
        {
            count++;
        }
    }
    return count;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtCaptionedControl::ComponentControl(TInt aIndex) const
{
    // We don't want iControl to be counted as a component control
    CCoeControl* controls[] = { iBitmap, iCaption, iTrailer };
    TInt nrOfCtrls = sizeof(controls) / sizeof(CCoeControl*);
    for (TUint i = 0; i < nrOfCtrls; i++)
    {
        if (controls[i] && aIndex-- == 0)
        {
            return controls[i];
        }
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::MakeVisible(TBool aVisible)
{
    CEikCaptionedControl::MakeVisible(aVisible);
    MSwtControl* child = Child();

    // This prevents making visible controls invisible on their own
    if (child && !(child->Flags() & EFlagInvisible))
    {
        child->CoeControl().MakeVisible(aVisible);
    }

    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetDimmed(TBool aDimmed)
{
    CEikCaptionedControl::SetDimmed(aDimmed);
    MSwtControl* child = Child();

    // This prevents undimming controls dimmed on their own
    if (child && !(child->Flags() & EFlagDisabled))
    {
        child->CoeControl().SetDimmed(aDimmed);
    }

    if (iCaption)
    {
        iCaption->SetDimmed(aDimmed);
    }
    if (iTrailer)
    {
        iTrailer->SetDimmed(aDimmed);
    }

    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtCaptionedControl::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType)
{
    return HandleKeyL(aEvent, aType, ETrue);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::MinimumSize
// From CCoeControl
// ---------------------------------------------------------------------------
//
TSize CSwtCaptionedControl::MinimumSize()
{
    // Default left margin for caption and control
    TInt leftMargin = DefaultLeftMargin();

    // First line
    TSize firstLineSize(0, 0);

    // Bitmap
    if (iBitmap)
    {
        TSize bitmapSize(iBitmap->MinimumSize());
        firstLineSize = bitmapSize;
        firstLineSize.iHeight += KSwtDefaultCaptionMargin;
    }

    // Left margin
    if (firstLineSize.iWidth < leftMargin)
    {
        firstLineSize.iWidth = leftMargin;
    }

    // Caption
    if (iCaption && !(iCaption->Text()->Length()))
    {
        delete iCaption;
        iCaption = NULL;
    }

    if (iCaption)
    {
        TSize capSize(iCaption->MinimumSize());

        // Fix is needed for minimum height in case used font is an open font
        TInt captionRealMinHeight;
        if (RealFontHeight(captionRealMinHeight))
        {
            if (capSize.iHeight < captionRealMinHeight)
            {
                capSize.iHeight = captionRealMinHeight;
            }
        }
        firstLineSize.iWidth += capSize.iWidth;
        if (capSize.iHeight + KSwtDefaultCaptionMargin > firstLineSize.iHeight)
        {
            firstLineSize.iHeight = capSize.iHeight + KSwtDefaultCaptionMargin;
        }
    }

    // Second line
    // Control
    TSize secondLineSize(iControl->MinimumSize());

    // Left margin
    secondLineSize.iWidth += leftMargin;

    // Trailing
    if (iTrailer)
    {
        TSize trailSize(iTrailer->MinimumSize());

        // Fix is needed for minimum height in case used font is an open font
        TInt captionRealMinHeight;
        if (RealFontHeight(captionRealMinHeight))
        {
            if (trailSize.iHeight < captionRealMinHeight)
            {
                trailSize.iHeight = captionRealMinHeight;
            }
        }
        secondLineSize.iWidth += trailSize.iWidth;
        secondLineSize.iWidth += KSwtDefaultTrailMargin;
        if (trailSize.iHeight > secondLineSize.iHeight)
        {
            secondLineSize.iHeight = trailSize.iHeight;
        }
    }

    // Total size
    TSize size(firstLineSize);
    if (secondLineSize.iWidth > size.iWidth)
    {
        size.iWidth = secondLineSize.iWidth;
    }
    size.iHeight += secondLineSize.iHeight;

    return size;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CSwtCaptionedControl::HandlePointerEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    MSwtControl* child = Child();
    if (child)
    {
        TPointerEvent pointerEvent(aPointerEvent);
        TRect childRect(child->CoeControl().Rect());
        TBool childRectContainsPoint(childRect.Contains(pointerEvent.iPosition));

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        if (!IsFocusedOrChildIsFocused())
        {
            //feedback on first tap
            child->DoControlSpecificFeedback(ETrue, childRectContainsPoint, aPointerEvent);
        }
        else
        {
            //feedback on second tap
            child->DoControlSpecificFeedback(EFalse, childRectContainsPoint, aPointerEvent);
        }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

        // It is important here to check first if the child is focusable.
        // If the child is a Composite that is not focusable (has focusable children),
        // the child will pass the focus to its children in HandlePointerEventL.
        // So this check here is needed to avoid calling SetSwtFocus twice, which
        // can lead in some situations to Texts opening VKB when gaining focus.
        if (child->IsFocusable())
        {
            child->SetSwtFocus(KSwtFocusByPointer);
        }

        if (!childRectContainsPoint)
        {
            childRect.Shrink(2, 2);   // well inside the edges
            if (pointerEvent.iPosition.iX < childRect.iTl.iX)
            {
                pointerEvent.iPosition.iX = childRect.iTl.iX;
            }
            if (pointerEvent.iPosition.iY < childRect.iTl.iY)
            {
                pointerEvent.iPosition.iY = childRect.iTl.iY;
            }
            if (pointerEvent.iPosition.iX > childRect.iBr.iX)
            {
                pointerEvent.iPosition.iX = childRect.iBr.iX;
            }
            if (pointerEvent.iPosition.iY > childRect.iBr.iY)
            {
                pointerEvent.iPosition.iY = childRect.iBr.iY;
            }
        }

        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
            iPointerGrabbingControl = NULL;
        else
            iPointerGrabbingControl = child;

        child->CoeControl().HandlePointerEventL(pointerEvent);

        // It is possible that the pointer position did not intersect the children.
        if (!IsFocusedOrChildIsFocused())
        {
            SetSwtFocus(KSwtFocusByPointer);
        }
    }
    else
    {
        iPointerGrabbingControl = NULL;
        ASwtControlBase::SetSwtFocus(KSwtFocusByPointer);
        PostMouseEventL(aPointerEvent);
    }
}
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SwtHandleResourceChangeL
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SwtHandleResourceChangeL(TInt aType)
{
    CEikCaptionedControl::HandleResourceChange(aType);

    CCoeControl* coeChild = CoeChild();
    if (coeChild)
    {
        coeChild->HandleResourceChange(aType);
    }

    if (aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch)
    {
        if (iDefaultFont)
        {
            iDefaultFont->RemoveRef();
            iDefaultFont = NULL;
        }

        const CFont* font = &iDisplay.Device().GetSystemFont()->Font();
        iDefaultFont = CSwtFont::NewL(iDisplay.Device(), *font);

        // If captionedcontrol uses default font, then update components
        // fonts because of resource change
        if (!iFont)
        {
            if (iCaption)
            {
                iCaption->SetFont(&iDefaultFont->Font());
                iCaption->SetAllMarginsTo(0);
            }
            if (iTrailer)
            {
                iTrailer->SetFont(&iDefaultFont->Font());
                iTrailer->SetAllMarginsTo(0);
            }

            TInt captionRealFontHeight;
            if (RealFontHeight(captionRealFontHeight))
            {
                if (iCaption)
                {
                    iCaption->iMargin.iTop = captionRealFontHeight
                                             - font->HeightInPixels();
                }
                if (iTrailer)
                {
                    iTrailer->iMargin.iTop = captionRealFontHeight
                                             - font->HeightInPixels();
                }
            }
        }

        if (aType == KAknsMessageSkinChange)
        {
            UpdateTextColor();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetForegroundL
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetForegroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetForegroundL(aColor);
    iCustomFg = aColor;
    UpdateTextColor();
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::UpdateTextColor
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::UpdateTextColor()
{
    TRgb color(0);
    TInt err(KErrNone);

    if (HasHighlight())
    {
        // Highlighted foreground color, overrides all others.
        err = AknsUtils::GetCachedColor(AknsUtils::SkinInstance()
                                        , color
                                        , KAknsIIDQsnTextColors
                                        , KHighlightedTextColor);
    }
    else if (iCustomFg)
    {
        // Custom foreground color, overrides the default.
        color = iCustomFg->RgbValue();
    }
    else
    {
        // Default foreground color.
        err = AknsUtils::GetCachedColor(AknsUtils::SkinInstance()
                                        , color
                                        , KAknsIIDQsnTextColors
                                        , KNonHighlightedTextColor);
    }

    if (err == KErrNone)
    {
        TRAP_IGNORE(OverrideColorL(EColorLabelText, color));
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::HandleHighlightChange
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::HandleHighlightChange()
{
    UpdateTextColor();
    MSwtControl* child = Child();
    if (child)
    {
        child->HandleHighlightChange();
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::FocusChanged(TDrawNow aDrawNow)
{
    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SizeChanged()
{
    TRect rect(Rect());

    // Default left margin for caption and control
    TInt leftMargin = DefaultLeftMargin();

    // First line
    TInt firstLineHeight = 0;
    // Initial position for caption after bitmap layout or
    // gap between border and caption if there is no bitmap
    TInt captionLeftMargin = leftMargin;

    // Bitmap
    if (iBitmap)
    {
        TSize bitmapSize(iBitmap->MinimumSize());
        if (bitmapSize.iWidth > rect.Width())
        {
            bitmapSize.iWidth = rect.Width();
        }

        if (bitmapSize.iHeight > rect.Height() - KSwtDefaultCaptionMargin)
        {
            bitmapSize.iHeight = rect.Height() - KSwtDefaultCaptionMargin;
        }

        if (AknLayoutUtils::LayoutMirrored())
        {
            TPoint bitmapPos(0, 0);
            // in the mirrored layout bitmap is on right side
            bitmapPos.iX = rect.iTl.iX + rect.Width() - bitmapSize.iWidth;
            bitmapPos.iY = rect.iTl.iY;

            iBitmap->SetExtent(bitmapPos, bitmapSize);
        }
        else
        {
            iBitmap->SetExtent(rect.iTl, bitmapSize);
        }

        firstLineHeight = bitmapSize.iHeight;
        if (captionLeftMargin < bitmapSize.iWidth)
        {
            captionLeftMargin = bitmapSize.iWidth;
        }
    }

    // Caption
    if (iCaption)
    {
        if (captionLeftMargin > rect.Width())
        {
            captionLeftMargin = rect.Width();
        }
        TInt captionPosX = rect.iTl.iX + captionLeftMargin;
        TInt availableWidth = rect.iBr.iX - captionPosX;
        TSize capSize(iCaption->MinimumSize());

        if (capSize.iHeight > rect.Height() - KSwtDefaultCaptionMargin)
        {
            capSize.iHeight = rect.Height() - KSwtDefaultCaptionMargin;
        }

        if (capSize.iHeight > firstLineHeight)
        {
            firstLineHeight = capSize.iHeight;
        }

        if (AknLayoutUtils::LayoutMirrored())
        {
            // in mirrored layout caption is on right side
            iCaption->SetExtent(rect.iTl, TSize(availableWidth, firstLineHeight));
        }
        else
        {
            iCaption->SetExtent(TPoint(captionPosX, rect.iTl.iY),
                                TSize(availableWidth, firstLineHeight));
        }
    }
    //Increase size of first line by margin
    if (iBitmap || iCaption)
    {
        firstLineHeight += KSwtDefaultCaptionMargin;
    }

    // Second Line
    rect.iTl.iY += firstLineHeight;

    // Trailing
    if (iTrailer)
    {
        TInt availableWidth = Max(0, rect.Width() - iControl->MinimumSize().iWidth -
                                  leftMargin - KSwtDefaultTrailMargin);
        TSize trailSize(iTrailer->MinimumSize());
        if (trailSize.iWidth > availableWidth)
        {
            trailSize.iWidth = availableWidth;
        }
        if (trailSize.iHeight > rect.Height())
        {
            trailSize.iHeight = rect.Height();
        }

        if (AknLayoutUtils::LayoutMirrored())
        {
            // in mirrored layout trailing text is on left side
            iTrailer->SetExtent(TPoint(rect.iTl.iX, rect.iTl.iY), trailSize);
            rect.iTl.iX += trailSize.iWidth;
            rect.iTl.iX += KSwtDefaultTrailMargin;
            rect.iBr.iX -= leftMargin;
        }
        else
        {
            rect.iTl.iX += leftMargin;
            TInt trailPosX = rect.iBr.iX - trailSize.iWidth;
            iTrailer->SetExtent(TPoint(trailPosX, rect.iTl.iY), trailSize);
            rect.iBr.iX = trailPosX;
            rect.iBr.iX = Max(0, rect.iBr.iX - KSwtDefaultTrailMargin);
        }
    }
    else
    {
        if (AknLayoutUtils::LayoutMirrored())
        {
            // in mirrored layout margin is on rightside
            rect.iBr.iX -= leftMargin;
        }
        else
        {
            rect.iTl.iX += leftMargin;
        }
    }

    rect.Normalize();

    // Control
    iControl->SetRect(rect);

    // Crop
    if (iCaption)
    {
        TRAP_IGNORE(iCaption->SetTextL(*iCaptionText));
        iCaption->CropText();
    }

    HandleSizeChanged();
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::PositionChanged()
{
    const TPoint oldPos(iPrevPos);
    const TRect newRect(Rect());

    // Replace old position for future use
    iPrevPos = newRect.iTl;

    // Refresh inner rect
    iInnerRect = ComputeInnerRect(newRect);

    // Forward to base
    CEikCaptionedControl::PositionChanged();

    // Forward new position to child
    if (newRect.iTl != oldPos)
    {
        iControl->SetPosition(iControl->Position() + newRect.iTl - oldPos);
    }

    // The moved event is sent from ASwtControlBase::SetLocation
    HandlePositionChanged();
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtCaptionedControl::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtCaptionedControl::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::IsFocusable
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtCaptionedControl::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    if (aReason == KSwtFocusByForce)
    {
        return ASwtControlBase::IsFocusable(KSwtFocusByForce);
    }
    else
    {
        // The captioned control itself is focusable if there is no focusable child
        return ASwtControlBase::IsFocusable()
               && (CountFocusableChildren(1, NULL, EFalse) == 0);
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::FocusBackgroundPolicy
// From MSwtControl
// ---------------------------------------------------------------------------
//
TInt CSwtCaptionedControl::FocusBackgroundPolicy() const
{
    return ECaptionedCtrlFocusBackground;
};

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ProcessKeyEventL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::ProcessKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    MSwtControl* child = Child();
    if (child)
    {
        CEikCaptionedControl::OfferKeyEventL(aKeyEvent, aType);
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetSwtFocus
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtCaptionedControl::SetSwtFocus(TInt aReason /*=KSwtFocusByApi*/)
{
    MSwtControl* child = Child();

    if ((aReason != KSwtFocusByForce) && child && child->SetSwtFocus(aReason))
    {
        return ETrue;
    }

    return ASwtControlBase::SetSwtFocus(aReason);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ClientRect
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRect CSwtCaptionedControl::ClientRect() const
{
    TRect r(ComputeInnerRect(Rect()));

    TInt firstLineHeight = 0;
    if (iBitmap)
    {
        firstLineHeight = iBitmap->Size().iHeight;
    }
    if (iCaption)
    {
        TInt captionHeight = iCaption->Size().iHeight;
        if (captionHeight > firstLineHeight)
        {
            firstLineHeight = captionHeight;
        }
    }

    r.iTl.iY += firstLineHeight;
    r.iTl.iY += KSwtDefaultCaptionMargin;

    if (iTrailer)
    {
        if (AknLayoutUtils::LayoutMirrored())
        {
            r.iTl.iX += iTrailer->Size().iWidth;
            r.iTl.iX += KSwtDefaultTrailMargin;
        }
        else
        {
            r.iBr.iX -= iTrailer->Size().iWidth;
            r.iBr.iX = Max(0, r.iBr.iX - KSwtDefaultTrailMargin);
        }
    }

    TInt leftMargin = DefaultLeftMargin();

    if (AknLayoutUtils::LayoutMirrored())
    {
        r.iBr.iX -= leftMargin;
    }
    else
    {
        r.iTl.iX += leftMargin;
    }


    if (r.iBr.iX < r.iTl.iX)
    {
        r.iBr.iX = r.iTl.iX;
    }
    if (r.iTl.iY > r.iBr.iY)
    {
        r.iTl.iY = r.iBr.iY;
    }

    return r;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ComputeSizeL
// From MSwtControl
// ---------------------------------------------------------------------------
//
TSize CSwtCaptionedControl::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    ResetMinimumSizes();

    // Save caption and trailer size as they may be modified when calling
    // MinimumSize() because CEikLabel::MinimumSize is used and this does set
    // its iSize
    TSize capSize(TSize::EUninitialized);
    TSize trailSize(TSize::EUninitialized);
    if (iCaption)
    {
        capSize = iCaption->Size();
    }
    if (iTrailer)
    {
        trailSize = iTrailer->Size();
    }

    // Get CEikCaptionedControl preferred size
    TSize size(MinimumSize());

    // Restore caption and trailer size if needed
    if ((iCaption) && (iCaption->Size()  !=  capSize))
    {
        iCaption->SetSize(capSize);
    }
    if ((iTrailer) && (iTrailer->Size()  !=  trailSize))
    {
        iTrailer->SetSize(trailSize);
    }

    // Add frame and border size
    TRect outerRect(0, 0, KSwtCaptionedCtrlDefPrefSize,
                    KSwtCaptionedCtrlDefPrefSize);
    TRect innerRect = ComputeInnerRect(outerRect);
    size += outerRect.Size() - innerRect.Size();

    if (aWHint != KSwtDefault)
    {
        size.iWidth = aWHint;
    }
    if (aHHint != KSwtDefault)
    {
        size.iHeight = aHHint;
    }

    return size;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetWidgetSize
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetWidgetSize(const TSize& aNewSize)
{
    TRect oldRect(Rect());
    TRect newRect(Position(), aNewSize);

    // Do not invalidate outside parent's visible client area.
    if (iParent)
    {
        TRect parentVisibleRect(iParent->Control()->CoeControl().Rect());
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
            // Invalidate our old rect
            Invalidate(invalidRegion[i]);
        }
    }
    invalidRegion.Close();

    DoSetSize(aNewSize);

    if (!(iStyle & KSwtStyleNoRedrawResize))
    {
        // Invalidate our new rect
        Redraw();
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetFontL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetFontL(const MSwtFont* aFont)
{
    ASwtControlBase::DoSetFontL(aFont);

    const MSwtFont* swtFont = GetFont();
    ASSERT(swtFont);
    const CFont& font = (swtFont->Font());

    if (iCaption)
    {
        iCaption->SetFont(&font);
        iCaption->SetAllMarginsTo(0);
    }

    if (iTrailer)
    {
        iTrailer->SetFont(&font);
        iTrailer->SetAllMarginsTo(0);
    }

    TInt captionRealFontHeight;
    if (RealFontHeight(captionRealFontHeight))
    {
        if (iCaption)
        {
            iCaption->iMargin.iTop = captionRealFontHeight
                                     - font.HeightInPixels();
        }
        if (iTrailer)
        {
            iTrailer->iMargin.iTop = captionRealFontHeight
                                     - font.HeightInPixels();
        }
    }

    // We force a call to ( private ) SizeChanged to resize the components.
    ResetMinimumSizes();
    SetWidgetSize(iSize);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetBounds
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetBounds(const TRect& aRect)
{
    // Must NOT call SetExtent if we want component controls to be relocated.

    TSize oldSize(Size());
    TPoint oldLocation(GetLocation());
    TPoint newLocation(iParent->Scrollable()->Control()->ClientRect().iTl + aRect.iTl);
    TRect oldRect(Rect());
    TRect newRect(newLocation, aRect.Size());

    // Do not invalidate outside parent's visible client area.
    if (iParent)
    {
        TRect parentVisibleRect(iParent->Control()->CoeControl().Rect());
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

    // Set and invalidate new rect
    if (oldLocation != newLocation)
    {
        CEikCaptionedControl::SetPosition(newLocation);
    }
    if (oldSize != aRect.Size())
    {
        DoSetSize(aRect.Size());
    }
    if (!(iStyle & KSwtStyleNoRedrawResize))
    {
        Redraw();
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CompositeInterface
// From MSwtControl
// ---------------------------------------------------------------------------
//
MSwtComposite* CSwtCaptionedControl::CompositeInterface() const
{
    const MSwtComposite* composite = this;
    return const_cast<MSwtComposite*>(composite);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ComputeTabListL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::ComputeTabListL(RSwtControlArray& aList)
{
    ASwtControlBase::ComputeTabListL(aList);
    MSwtControl* child = Child();
    if (child)
    {
        child->ComputeTabListL(aList);
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CaptionedControlInterface
// From MSwtControl
// ---------------------------------------------------------------------------
//
MSwtCaptionedControl* CSwtCaptionedControl::CaptionedControlInterface() const
{
    const MSwtCaptionedControl* composite = this;
    return const_cast<MSwtCaptionedControl*>(composite);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::UpdateDoNotDrawFlag
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::UpdateDoNotDrawFlag()
{
    ASwtControlBase::UpdateDoNotDrawFlag();
    MSwtControl* child = Child();
    if (child)
    {
        child->UpdateDoNotDrawFlag();
    }
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::DefaultFont
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
const MSwtFont& CSwtCaptionedControl::DefaultFont() const
{
    ASSERT(iDefaultFont);
    return *iDefaultFont;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::Control
// From MSwtScrollable
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtCaptionedControl::Control()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ComputeTrim
// From MSwtScrollable
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::ComputeTrim(TRect& aRect) const
{
    TInt leftMargin = DefaultLeftMargin();
    aRect.iTl.iX -= leftMargin;

    if (iTrailer)
    {
        aRect.iBr.iX += iTrailer->Size().iWidth;
        TInt deltaHeight = iTrailer->Size().iHeight - aRect.Height();
        if (deltaHeight > 0)
        {
            aRect.iBr.iY += deltaHeight;
        }
    }

    TSize firstLineSize(0, 0);
    if (iBitmap)
    {
        firstLineSize = iBitmap->Size();
    }
    if (firstLineSize.iWidth < leftMargin)
    {
        firstLineSize.iWidth = leftMargin;
    }
    if (iCaption)
    {
        firstLineSize.iWidth += iCaption->Size().iWidth;
        TInt captionHeight = iCaption->Size().iHeight;
        if (captionHeight > firstLineSize.iHeight)
        {
            firstLineSize.iHeight = captionHeight;
        }
    }

    aRect.iTl.iY -= firstLineSize.iHeight;

    TInt deltaWidth = firstLineSize.iWidth - aRect.Width();
    if (deltaWidth > 0)
    {
        aRect.iBr.iX += deltaWidth;
    }

    // Add frame and border size
    TRect outerRect(0, 0, KSwtCaptionedCtrlDefPrefSize,
                    KSwtCaptionedCtrlDefPrefSize);
    TRect innerRect = ComputeInnerRect(outerRect);
    aRect.iTl -= innerRect.iTl - outerRect.iTl;
    aRect.iBr += outerRect.iBr - innerRect.iBr;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::CreateScrollBarL
// From MSwtScrollable
// ---------------------------------------------------------------------------
//
MSwtScrollBar* CSwtCaptionedControl::CreateScrollBarL(TSwtPeer /*aPeer*/,
        TInt /*aStyle*/)
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetVisibleScrollBar
// From MSwtScrollable
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetVisibleScrollBar(TInt /*aStyle*/,
        TBool /*aVisible*/)
{
    // Should never be called
    ASSERT(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ScrollBarDestroyed
// From MSwtScrollable
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::ScrollBarDestroyed(
    const MSwtScrollBar* /*aScrollbar*/)
{
    // Should never be called
    ASSERT(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::Scrollable
// From MSwtComposite
// ---------------------------------------------------------------------------
//
MSwtScrollable* CSwtCaptionedControl::Scrollable()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::RegisterChildL
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::RegisterChildL(MSwtControl& aChild)
{
    MSwtControl* oldChild = Child();

    ASSERT(oldChild != &aChild);
    ASSERT(!aChild.CoeControl().IsFocused());
    ASSERT(!aChild.IsShell());

    MSwtShell& shell = GetShell();

    // Remove previous child
    if (oldChild)
    {
        UnregisterChild(*oldChild);
    }

    // Add child
    User::LeaveIfError(iChildren.Append(&aChild));
    SetChild(&aChild);

    // Reset iPreferredSize as we have a new child
    SetChildPreferredSize(TSize(0, 0));

    // Take care of tab stopping
    shell.ResetTabList();

    // Take care of focus
    if (iDisplay.UiUtils().GetActiveShell() == &shell)
    {
        MSwtControl* focusedCtrl = shell.FocusControl();
        if ((!focusedCtrl || focusedCtrl == this) && aChild.IsFocusable())
        {
            aChild.CoeControl().SetFocus(ETrue);
        }
    }

    // Set the child at our client area's origin
    aChild.SetLocation(TPoint(0, 0));

    // We force a call to ( private ) SizeChanged to resize the components.
    ResetMinimumSizes();
    SetWidgetSize(iSize);

    aChild.CoeControl().SetMopParent(this);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::UnregisterChild
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::UnregisterChild(MSwtControl& aChild)
{
    // If the child to unregister is not our current child, it means it is an
    // old child ( means another control was added to the captionedcontrol
    // after this old child, this can be done as captionedcontrol inherits
    // from composite ). This happens in the dispose of composite that
    // disposes all its children. As captionedcontrol doesn't use this old
    // child anymore in the native side, nothing has to be done.
    if (Child() != &aChild)
    {
        return;
    }

    // Unregister default buttons, focus memory, etc.
    MSwtShell& shell = GetShell();
    if (shell.UrgentPaintControl() == &aChild)
    {
        shell.SetUrgentPaintControl(NULL);
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

    // Remove child
    TInt index = iChildren.Find(&aChild);
    ASSERT(index != KErrNotFound);
    iChildren.Remove(index);
    SetChild(NULL);
    ResetMinimumSizes();

    // Take care of tab stopping
    shell.ResetTabList();
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::GetChildrenPeersL
// From MSwtComposite
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtCaptionedControl::GetChildrenPeersL()
{
    MSwtControl* child = Child();
    if (!child)
    {
        return NULL;
    }

    CSwtPeerArray* array = new(ELeave) CSwtPeerArray(1);
    CleanupStack::PushL(array);
    array->AppendL(child->JavaPeer());
    CleanupStack::Pop(array);
    return array;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::Children
// From MSwtComposite
// ---------------------------------------------------------------------------
//
const RSwtControlArray* CSwtCaptionedControl::Children() const
{
    return &iChildren;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ChildFocusChanged
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::ChildFocusChanged(MSwtControl& aControl)
{
    GetParent()->ChildFocusChanged(aControl);
}

// ---------------------------------------------------------------------------
// CSwtComposite::CountFocusableChildren
// From MSwtComposite
// ---------------------------------------------------------------------------
//
TInt CSwtCaptionedControl::CountFocusableChildren(TInt aTryBreakAfter /*= -1 */,
        const MSwtControl* aExclude /*= NULL */,
        TBool aVisualFocusCount /*= ETrue*/) const
{
    TInt res = 0;
    const MSwtControl* ctrl = Child();
    if (!ctrl)
    {
        return res;
    }

    if (ctrl->IsFocusable()
            && ((!aVisualFocusCount) || (aVisualFocusCount && ctrl->FocusBackgroundPolicy() != ENoFocusBackground))
            && ((!aExclude) || (aExclude && ctrl != aExclude && !ctrl->IsDescentOf(*aExclude))))
    {
        res++;
        if (res >= aTryBreakAfter)
        {
            return res;
        }
    }

    if (ctrl->CompositeInterface())
    {
        res += ctrl->CompositeInterface()->CountFocusableChildren(aTryBreakAfter - res,
                aExclude, aVisualFocusCount);
    }

    return res;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetTabListL
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetTabListL(MSwtControl** /*aControls*/,
                                       TInt /*aCount*/)
{
    // There can be only one control, so nothing to do here
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::GetTabListL
// From MSwtComposite
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtCaptionedControl::GetTabListL() const
{
    CSwtPeerArray* array = new(ELeave) CSwtPeerArray(1);
    CleanupStack::PushL(array);

    const MSwtControl* child = Child();
    if (child && child->IsFocusable())
    {
        array->AppendL(child->JavaPeer());
    }

    CleanupStack::Pop(array);
    return array;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::MoveChildAbove
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::MoveChildAbove(MSwtControl& /*aChild1*/,
        const MSwtControl* /*aChild2*/)
{
    // Will never be called, a CaptionedControl has at most one child
    ASSERT(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::MoveChildBelow
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::MoveChildBelow(MSwtControl& /*aChild1*/,
        const MSwtControl* /*aChild2*/)
{
    // Will never be called, a CaptionedControl has at most one child
    ASSERT(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::NotifyRadioGroup
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::NotifyRadioGroup(MSwtButton* /*aSelectedButton*/)
{
    // The CaptionedControl have only one children so don't manage RadioGroupButton.
    ASSERT(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::GetPointerGrabbingControl
// From MSwtComposite
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtCaptionedControl::GetPointerGrabbingControl() const
{
    return iPointerGrabbingControl;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetScrolledCompositeContentL
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetScrolledCompositeContentL(MSwtControl* /*aContent*/)
{
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ScrolledCompositeContent
// From MSwtComposite
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtCaptionedControl::ScrolledCompositeContent() const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::ScrolledCompositePysicsAction
// From MSwtComposite
// ---------------------------------------------------------------------------
//
TInt CSwtCaptionedControl::ScrolledCompositePysicsAction() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::DrawScrollBarBackground
// From MSwtComposite
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::DrawScrollBarBackground(const TRect& /*aRect*/) const
{
    // Do nothing
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::Composite
// From MSwtCaptionedControl
// ---------------------------------------------------------------------------
//
MSwtComposite* CSwtCaptionedControl::Composite() const
{
    return CompositeInterface();
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetTextL
// From MSwtCaptionedControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetTextL(const TDesC& aString)
{
    CEikCapCLabel* oldCaption = iCaption;

    SetCaptionL(aString);

    delete iCaptionText;
    iCaptionText=0;
    iCaptionText = aString.AllocL();

    if (iCaption && iCaption != oldCaption)
    {
        iCaption->SetContainerWindowL(*this); // Absolutely necessary
        iCaption->SetMopParent(this); // Absolutely necessary
        iCaption->MakeVisible(IsVisible());
        iCaption->SetDimmed(IsDimmed());
        if (AknLayoutUtils::LayoutMirrored())
        {
            iCaption->SetAlignment(EHRightVCenter);
        }
        else
        {
            iCaption->SetAlignment(EHLeftVCenter);
        }
        iCaption->ActivateL();
    }

    if (iCaption)
    {
        const MSwtFont* swtFont = GetFont();
        ASSERT(swtFont);
        const CFont& font = (swtFont->Font());
        iCaption->SetFont(&font);

        iCaption->SetAllMarginsTo(0);
        TInt captionRealFontHeight;
        if (RealFontHeight(captionRealFontHeight))
        {
            iCaption->iMargin.iTop = captionRealFontHeight - font.HeightInPixels();
        }

        UpdateTextColor();
    }

    // We force a call to ( private ) SizeChanged to resize the components.
    ResetMinimumSizes();
    SetWidgetSize(iSize);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetTrailingTextL
// From MSwtCaptionedControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetTrailingTextL(const TDesC& aString)
{
    CEikLabel* oldTrailer = iTrailer;

    if (!aString.Length())
    {
        delete iTrailer;
        iTrailer = NULL;
    }
    else
    {
        SetTrailerL(aString);
    }

    if (iTrailer && iTrailer != oldTrailer)
    {
        iTrailer->SetContainerWindowL(*this); // Absolutely necessary
        iTrailer->SetMopParent(this); // Absolutely necessary
        iTrailer->MakeVisible(IsVisible());
        iTrailer->SetDimmed(IsDimmed());
        iTrailer->ActivateL();
    }

    if (iTrailer)
    {
        const MSwtFont* swtFont = GetFont();
        ASSERT(swtFont);
        const CFont& font = (swtFont->Font());
        iTrailer->SetFont(&font);
        iTrailer->SetAllMarginsTo(0);

        TInt captionRealFontHeight;
        if (RealFontHeight(captionRealFontHeight))
        {
            iTrailer->iMargin.iTop = captionRealFontHeight - font.HeightInPixels();
        }

        UpdateTextColor();
    }

    // We force a call to ( private ) SizeChanged to resize the components.
    ResetMinimumSizes();
    SetWidgetSize(iSize);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::GetText
// From MSwtCaptionedControl
// ---------------------------------------------------------------------------
//
const TDesC* CSwtCaptionedControl::GetText() const
{
    return iCaption? iCaption->Text() : NULL;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::GetTrailingText
// From MSwtCaptionedControl
// ---------------------------------------------------------------------------
//
const TDesC* CSwtCaptionedControl::GetTrailingText() const
{
    return iTrailer? iTrailer->Text() : NULL;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetImageL
// From MSwtCaptionedControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetImageL(MSwtImage* aImage)
{
    if (aImage)
    {
        CFbsBitmap* bitmap = SwtControlHelper::GetCopyOfBitmapLC(
                                 &aImage->Bitmap());
        CFbsBitmap* mask = SwtControlHelper::GetInvertedCopyOfMonoBitmapLC(
                               aImage->MaskBitmap());

        ASSERT(bitmap);
        if (!mask)
        {
            // Captioned Control drawing does not support images with a NULL mask.
            mask = new(ELeave) CFbsBitmap;
            CleanupStack::PushL(mask);
            User::LeaveIfError(mask->Create(bitmap->SizeInPixels(), EGray2));

            CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(mask);
            CleanupStack::PushL(device);
            CFbsBitGc* gc = CFbsBitGc::NewL();
            CleanupStack::PushL(gc);

            gc->Activate(device);
            gc->SetPenStyle(CGraphicsContext::ENullPen);
            gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
            gc->SetBrushColor(KRgbBlack);
            gc->DrawRect(TRect(bitmap->SizeInPixels()));

            CleanupStack::PopAndDestroy(gc);
            CleanupStack::PopAndDestroy(device);
        }

        // After SetIconL, the CEikCaptionedControl owns bitmap and mask.
        SetIconL(bitmap, mask);
        CleanupStack::Pop(mask);
        CleanupStack::Pop(bitmap);
    }
    else
    {
        SetIconL(NULL, NULL);
    }

    if (iBitmap)
    {
        iBitmap->SetContainerWindowL(*this); // Absolutely necessary
        iBitmap->SetMopParent(this); // Absolutely necessary
    }

    // We force a call to ( private ) SizeChanged to resize the components.
    ResetMinimumSizes();
    SetWidgetSize(iSize);
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::SetChildPreferredSize
// From MSwtCaptionedControl
// ---------------------------------------------------------------------------
//
void CSwtCaptionedControl::SetChildPreferredSize(const TSize& aSize)
{
    ASSERT(iControl);
    static_cast<CControlProxy*>(iControl)->iPreferredSize = aSize;
}

// ---------------------------------------------------------------------------
// CSwtCaptionedControl::GetImageControl
// From MSwtCaptionedControl
// ---------------------------------------------------------------------------
//
const CCoeControl* CSwtCaptionedControl::GetImageControl() const
{
    return iBitmap;
}
