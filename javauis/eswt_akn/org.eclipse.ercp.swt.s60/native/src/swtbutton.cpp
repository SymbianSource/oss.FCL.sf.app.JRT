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
#include <AknsUtils.h>
#include <aknconsts.h>
#include <avkon.mbg>
#include <swtlaffacade.h>
#include "swtcontrolhelper.h"
#include "eswtmobileextensions.h"
#include "swtbutton.h"
#include "swtbuttonproxy.h"
#include "swtfont.h"
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtButton::NewL
// ---------------------------------------------------------------------------
//
CSwtButton* CSwtButton::NewL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
{
    CSwtButton* self = new(ELeave) CSwtButton(
        aDisplay,
        aPeer,
        aParent,
        aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtButton::CSwtButton
// ---------------------------------------------------------------------------
//
CSwtButton::CSwtButton(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
        : ASwtControlBase(aDisplay, aPeer, &aParent, aStyle)
{
}

// ---------------------------------------------------------------------------
// CSwtButton::~CSwtButton
// ---------------------------------------------------------------------------
//
CSwtButton::~CSwtButton()
{
    delete iButton;

    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
    }

    if (iImage != NULL)
    {
        iImage->RemoveRef();
    }

    if (iDupFont)
    {
        iDupFont->RemoveRef();
    }

#ifdef _lint
    iDefaultFont = NULL;
    iImage = NULL;
    iCustomIcon = NULL;
    iDupFont = NULL;
#endif
}

// ---------------------------------------------------------------------------
// CSwtButton::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtButton::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);

    CreateButtonL(KNullDesC);
    RetrieveDefaultFontL();

    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());
    SetAlignment(iStyle & KSwtAlignmentMask);

    SetComponentsToInheritVisibility(ETrue);

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
    // Disabling feedback for CSwtButton.
    // When enabled - first tap on combo creates extra feedback,
    // that is not needed.
    if (iFeedback)
    {
        iFeedback->EnableFeedbackForControl(this, EFalse);
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    ActivateL();
}

// ---------------------------------------------------------------------------
// CSwtButton::RetrieveDefaultFontL
// ---------------------------------------------------------------------------
//
void CSwtButton::RetrieveDefaultFontL()
{
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }

    // Same as Label's
    iDefaultFont = CSwtFont::NewL(iDisplay.Device(), iDisplay.Device().GetSystemFont()->Font());
    if (!iFont)
    {
        ProcessFontUpdate();
    }
}
// ---------------------------------------------------------------------------------------------
// CSwtButton::ProcessFontUpdate
// ---------------------------------------------------------------------------------------------
//
void CSwtButton::ProcessFontUpdate()
{
    const MSwtFont* swtFont=GetFont();
    ASSERT(swtFont);
    const CFont* font = &(swtFont->Font());

    // Workaround for that the custom font on window server side is changed unexpectedly
    if (iFont)
    {
        if (!iDupFont)
        {
            // duplicate
            TRAP_IGNORE(iDupFont = CSwtFont::NewL(iDisplay.Device(), swtFont->Font()));
        }

        if (iDupFont)
        {
            // duplicated font will pass to the button to avoid custom font being changed
            font = &(iDupFont->Font());
        }
    }

    iButton->SetTextFont(font);
}


// ---------------------------------------------------------------------------
// CSwtButton::CreateButtonL
// ---------------------------------------------------------------------------
//
void CSwtButton::CreateButtonL(const TDesC& aText)
{
    ASSERT(iButton == NULL);

    if (iStyle & (KSwtStyleCheck | KSwtStyleRadio))
    {
        CGulIcon* icon = CreateCheckOrRadioIconL(iSelected, iStyle);
        CleanupStack::PushL(icon);

        iButton = CSwtButtonProxy::NewL(this, icon, aText,
                                        KAknButtonSizeFitText | KAknButtonNoFrame | KAknButtonPressedDownFrame);

        CleanupStack::Pop(icon);

        iCustomIcon = icon;

        TSize iconSize(TSize::EUninitialized);
        GetSizeOfCheckOrRadioIcon(iconSize);
        SetIconSize(iCustomIcon, iconSize);
        iButton->SetIconSize(iconSize);
    }
    else // KSwtStyleToggle, KSwtStylePush, KSwtStyleFlat
    {
        // Do not use KAknButtonTextInsideFrame. It clips the icons when text is present.
        iButton = CSwtButtonProxy::NewL(this, NULL, aText, KAknButtonSizeFitText);
    }

    iButton->SetObserver(this);
    iButton->MakeVisible(ETrue);
    iButton->SetFocusing(ETrue);
    iButton->SetIconScaleMode(EAspectRatioPreserved);

    iButton->SetBackground(this);   // Back will be drawn by ASwtControlBase::Draw

    // Set default color
    TRgb color;
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              color,
                              KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG63); // Messaging touch buttons
    iButton->OverrideColorL(EColorButtonText, color);
}

// ---------------------------------------------------------------------------
// CSwtButton::ChangeStateL
// ---------------------------------------------------------------------------
//
void CSwtButton::ChangeStateL(TBool aState)
{
    ASSERT(iButton);

    if (iStyle & KSwtStyleToggle)
    {
        if (aState)
        {
            iButton->SetFlags(iButton->Flags() | KAknButtonStateHasLatchedFrame);
        }
        else
        {
            iButton->SetFlags(iButton->Flags() & ~KAknButtonStateHasLatchedFrame);
        }
    }
    else if (iStyle & (KSwtStyleCheck | KSwtStyleRadio))
    {
        ChangeCheckOrRadioIconL(aState);
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::CreateCheckOrRadioIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CSwtButton::CreateCheckOrRadioIconL(
    TBool aState, TInt aStyle) const
{
    ASSERT(aStyle & (KSwtStyleCheck | KSwtStyleRadio));

    TInt bmpId(0);
    TInt maskId(0);
    TAknsItemID skinItemId;

    if (aStyle & KSwtStyleCheck)
    {
        if (aState)
        {
            skinItemId.Set(KAknsIIDQgnPropCheckboxOn);
            bmpId = EMbmAvkonQgn_prop_checkbox_on;
            maskId = EMbmAvkonQgn_prop_checkbox_on_mask;
        }
        else
        {
            skinItemId.Set(KAknsIIDQgnPropCheckboxOff);
            bmpId = EMbmAvkonQgn_prop_checkbox_off;
            maskId = EMbmAvkonQgn_prop_checkbox_off_mask;
        }
    }
    else // KSwtStyleRadio
    {
        if (aState)
        {
            skinItemId.Set(KAknsIIDQgnPropRadiobuttOn);
            bmpId = EMbmAvkonQgn_prop_radiobutt_on;
            maskId = EMbmAvkonQgn_prop_radiobutt_on_mask;
        }
        else
        {
            skinItemId.Set(KAknsIIDQgnPropRadiobuttOff);
            bmpId = EMbmAvkonQgn_prop_radiobutt_off;
            maskId = EMbmAvkonQgn_prop_radiobutt_off_mask;
        }
    }

    return AknsUtils::CreateGulIconL(
               AknsUtils::SkinInstance(),
               skinItemId,
               AknIconUtils::AvkonIconFileName(),
               bmpId,
               maskId);
}

// ---------------------------------------------------------------------------
// CSwtButton::ChangeCheckOrRadioIconL
// ---------------------------------------------------------------------------
//
void CSwtButton::ChangeCheckOrRadioIconL(TBool aState)
{
    ASSERT(iStyle & (KSwtStyleCheck | KSwtStyleRadio));

    CGulIcon* icon = CreateCheckOrRadioIconL(aState, iStyle);
    if (iButton)
    {
        iButton->SetIcon(icon);
    }
    iCustomIcon = icon;

    TSize iconSize(TSize::EUninitialized);
    GetSizeOfCheckOrRadioIcon(iconSize);
    SetIconSize(iCustomIcon, iconSize);
    iButton->SetIconSize(iconSize);
}

// ---------------------------------------------------------------------------
// CSwtButton::SetIconSize
// ---------------------------------------------------------------------------
//
void CSwtButton::SetIconSize(const CGulIcon* aIcon, const TSize& aSize,
                             TScaleMode aScaleMode) const
{
    if (aIcon)
    {
        if (aIcon->Bitmap())
        {
            AknIconUtils::SetSize(aIcon->Bitmap(), aSize, aScaleMode);
        }
        if (aIcon->Mask())
        {
            AknIconUtils::SetSize(aIcon->Mask(), aSize, aScaleMode);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::GetSizeOfCheckOrRadioIcon
// ---------------------------------------------------------------------------
//
void CSwtButton::GetSizeOfCheckOrRadioIcon(TSize& aSize) const
{
    aSize = TSize(0, 0);

    if (iStyle & (KSwtStyleCheck | KSwtStyleRadio))
    {
        TInt variety((iStyle & KSwtStyleRadio) ? 1 : 0);

        aSize = CSwtLafFacade::GetLayoutRect(
                    CSwtLafFacade::EControlButtonPaneG2, TRect(), variety).Rect().Size();
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::UpdateControlSize
// ---------------------------------------------------------------------------
//
void CSwtButton::UpdateControlSize()
{
    if (iButton)
    {
        iButton->SetSize(iSize);
    }

    if (iStyle & (KSwtStyleCheck | KSwtStyleRadio))
    {
        // Update check/radio icon to match the new size of the control
        TRAP_IGNORE(ChangeCheckOrRadioIconL(iSelected));
    }

    if (iButton)
    {
        iButton->SetHighlightRect(Rect());
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::SwtHandleResourceChangeL
// ---------------------------------------------------------------------------
//
void CSwtButton::SwtHandleResourceChangeL(TInt aType)
{
    CCoeControl::HandleResourceChange(aType);
    if (aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch)
    {
        if (!iFont)
        {
            RetrieveDefaultFontL();
        }
        else
        {
            // Workaround: the custom font (iDupFont) that was passed to button
            // can be changed by switching resolutions. Here, delete the duplicated
            // font and a new will be created in ProcessFontUpdate
            if (iDupFont)
            {
                // delete duplicated font (by removing reference to it)
                iDupFont->RemoveRef();
                iDupFont = NULL;
            }
            ProcessFontUpdate();
        }
        if (aType == KAknsMessageSkinChange)
        {
            // Refresh radio or check image
            if (iStyle & (KSwtStyleCheck | KSwtStyleRadio))
            {
                ChangeCheckOrRadioIconL(iSelected);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::IsFeedbackOnTouchDown
// ---------------------------------------------------------------------------
//
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
TBool CSwtButton::IsFeedbackOnTouchDownNeeded(const TPointerEvent& aPointerEvent) const
{
    if (aPointerEvent.iType != TPointerEvent::EButton1Down)
    {
        return EFalse;
    }

    return ETrue;
}
#endif

// ---------------------------------------------------------------------------
// CSwtButton::IsFeedbackOnTouchRelease
// ---------------------------------------------------------------------------
//
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
TBool CSwtButton::IsFeedbackOnTouchReleaseNeeded(const TPointerEvent& aPointerEvent) const
{
    if (aPointerEvent.iType != TPointerEvent::EButton1Up)
    {
        return EFalse;
    }

    // When pointer was dragged out of the visible area of the control,
    // ther should be no feedback.
    return VisibleRect(ETrue).Contains(aPointerEvent.iPosition);
}
#endif

// ---------------------------------------------------------------------------
// CSwtButton::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtButton::ComponentControl(TInt /*aIndex*/) const
{
    return iButton;
}

// ---------------------------------------------------------------------------
// CSwtButton::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtButton::CountComponentControls() const
{
    return (iButton) ? 1 : 0;
}

// ---------------------------------------------------------------------------
// CSwtButton::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtButton::MakeVisible(TBool aVisible)
{
    CCoeControl::MakeVisible(aVisible);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtButton::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtButton::SetDimmed(TBool aDimmed)
{
    ASSERT(iButton);

    CCoeControl::SetDimmed(aDimmed);

    if (iButton)
    {
        iButton->SetDimmed(aDimmed);
    }

    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtButton::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtButton::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TBool traversalDoIt = ETrue;

    // Enter key is used to traverse_return the Shell, which means a default
    // button can get selected. Only the Selection key is a sure thing.
    if (aKeyEvent.iCode == EKeyOK)
    {
        traversalDoIt = EFalse;
    }

    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

// ---------------------------------------------------------------------------
// CSwtButton::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtButton::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}

// ---------------------------------------------------------------------------
// CSwtButton::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtButton::PositionChanged()
{
    if (iButton)
    {
        iButton->SetPosition(iPosition);
        iButton->SetHighlightRect(Rect());
    }
    HandlePositionChanged();
}

// ---------------------------------------------------------------------------
// CSwtButton::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtButton::SizeChanged()
{
    UpdateControlSize();
    HandleSizeChanged();
}

// ---------------------------------------------------------------------------
// CSwtButton::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtButton::FocusChanged(TDrawNow aDrawNow)
{
    TBool isFocused = IsFocusControl();
    if (iButton)
    {
        if (iDisplay.UiUtils().NaviKeyInput())
        {
            iButton->SetFocus(isFocused, aDrawNow);
        }
        else
        {
            // This is to eliminate the key focus highlight indication.
            if (iButton->IsFocused())
                iButton->SetFocus(EFalse, aDrawNow);
        }
    }
    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtButton::MopSupplyObject
// From CCoeControl
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtButton::MopSupplyObject(TTypeUid aId)
{
    TTypeUid::Ptr id = ASwtControlBase::SwtMopSupplyObject(aId);

    if (id.Pointer() == NULL)
    {
        return CCoeControl::MopSupplyObject(aId);
    }
    else
    {
        return id;
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::SetFontL
// From ASwtControlbase
// ---------------------------------------------------------------------------
//
void CSwtButton::SetFontL(const MSwtFont* aFont)
{
    ASwtControlBase::DoSetFontL(aFont);

    // Workaround: the custom font (iDupFont) that was passed to button can be changed by switching resolutions.
    // So, we don't directly pass iFont to button. Instead, we pass a duplicate font, iDupFont, to button.
    if (iDupFont)
    {
        // delete the old duplicated font (by removing reference to it) and
        // a new one will be duplicated in ProcessFontUpdate
        iDupFont->RemoveRef();
        iDupFont = NULL;
    }

    if (iButton)
    {
        ProcessFontUpdate();
    }

    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtButton::SetForegroundL
// From ASwtControlbase
// ---------------------------------------------------------------------------
//
void CSwtButton::SetForegroundL(const MSwtColor* aColor)
{
    TRgb color;

    if (aColor)
    {
        color = aColor->RgbValue();
    }
    else
    {
        // Reset to default color
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                                  color,
                                  KAknsIIDQsnTextColors,
                                  EAknsCIQsnTextColorsCG63); // Messaging touch buttons
    }

    if (iButton)
    {
        iButton->OverrideColorL(EColorButtonText, color);
    }

    ASwtControlBase::DoSetForegroundL(aColor);
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtButton::DefaultFont
// From ASwtControlbase
// ---------------------------------------------------------------------------
//
const MSwtFont& CSwtButton::DefaultFont() const
{
    ASSERT(iDefaultFont!=NULL);
    return *iDefaultFont;
}

// ---------------------------------------------------------------------------
// CSwtButton::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtButton::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtButton::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtButton::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtButton::ProcessKeyEventL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtButton::ProcessKeyEventL(const TKeyEvent& aKeyEvent,
                                  TEventCode aType)
{
    // Do not pass SKs to CAknButton as it will activate.
    if (iButton && (aKeyEvent.iScanCode != EStdKeyDevice0 && aKeyEvent.iScanCode != EStdKeyDevice1))
    {        
        TBool forward(EFalse);
        if ((iStyle & KSwtStyleRadio)
                && (GetParent()->Control()->CaptionedControlInterface() == NULL)
                && ((GetParent()->Control()->Style() & KSwtStyleNoRadioGroup) == 0))
        {
            if (!GetSelection())
            {
                forward = ETrue;
            }
        }
        else
        {
            forward = ETrue;
        }
        
        if (forward)
        {
            if (aKeyEvent.iScanCode == EStdKeyNkpEnter) {
                // Workaround for CAknButton's inability to handle EKeyEnter correctly
                TKeyEvent ev;
                ev.iCode = aKeyEvent.iCode;
                ev.iModifiers = aKeyEvent.iModifiers;
                ev.iRepeats = aKeyEvent.iRepeats;
                ev.iScanCode = EStdKeyEnter;
                iButton->OfferKeyEventL(ev, aType);
            }
            else
            {
                iButton->OfferKeyEventL(aKeyEvent, aType);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::IsKeyUsed
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtButton::IsKeyUsed(TUint aKeyCode) const
{
    if (aKeyCode == EKeyBackspace)
    {
        return EFalse;
    }
    else if (aKeyCode == EKeyOK)
    {
        return ETrue;
    }
    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtButton::ComputeSizeL
// From MSwtControl
// ---------------------------------------------------------------------------
//
TSize CSwtButton::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return TSize(aWHint, aHHint);
    }

    ASSERT(iButton);

    // Icon size
    const CGulIcon* icon = iButton->Icon();
    TSize iconSize(0, 0);
    if (icon)
    {
        if (icon->Bitmap())
        {
            iconSize = icon->Bitmap()->SizeInPixels();
        }
    }
    else if (iStyle & (KSwtStyleCheck | KSwtStyleRadio))
    {
        GetSizeOfCheckOrRadioIcon(iconSize);
    }

    // Text size
    const MSwtFont* font = GetFont();
    ASSERT(font);
    TSize textSize(0, font->Font().FontMaxHeight());
    if (font == &DefaultFont())
    {
        textSize.iHeight = Max(iDisplay.UiUtils().InlineReferenceFontHeight(), textSize.iHeight);
    }
    if (iButton->Text().Length())
    {
        textSize.iWidth += font->TextWidthInPixels(iButton->Text());
    }

    // Icon & text size
    TSize res(iDisplay.UiUtils().InlineReferencePadding().SizeDelta());
    res.iWidth += iconSize.iWidth + textSize.iWidth;
    res.iHeight += Max(iconSize.iHeight, textSize.iHeight);
    res += BorderSizeDelta();

    if (aWHint != KSwtDefault)
    {
        res.iWidth = aWHint;
    }
    if (aHHint != KSwtDefault)
    {
        res.iHeight = aHHint;
    }

    return res;
}

// ---------------------------------------------------------------------------
// CSwtButton::ButtonInterface
// From MSwtControl
// ---------------------------------------------------------------------------
//
MSwtButton* CSwtButton::ButtonInterface()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtButton::FocusBackgroundPolicy
// From MSwtControl
// ---------------------------------------------------------------------------
//
TInt CSwtButton::FocusBackgroundPolicy() const
{
    return EEmbeddedFocusBackground;
};

// ---------------------------------------------------------------------------
// CSwtButton::Control
// From MSwtButton
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtButton::Control()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtButton::GetSelection
// From MSwtButton
// ---------------------------------------------------------------------------
//
TBool CSwtButton::GetSelection() const
{
    return iSelected;
}

// ---------------------------------------------------------------------------
// CSwtButton::SetSelection
// From MSwtButton
// ---------------------------------------------------------------------------
//
void CSwtButton::SetSelection(TBool aState)
{
    if (iSelected != aState)
    {
        if (!(iStyle & KSwtStylePush))
        {
            iSelected = aState;

            TRAP_IGNORE(ChangeStateL(aState));

            Redraw();
        }

        if (iSelected
                && (iStyle & KSwtStyleRadio)
                && (GetParent()->Control()->CaptionedControlInterface() == NULL)
                && ((GetParent()->Control()->Style() & KSwtStyleNoRadioGroup) == 0))
        {
            GetParent()->NotifyRadioGroup(this);
        }
    }
    //MSK label update needed.
    if (AknLayoutUtils::MSKEnabled() && ((iStyle & KSwtStyleRadio) || (iStyle & KSwtStyleCheck)))
    {
        MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
        if (commandArranger)
        {
            TRAP_IGNORE(commandArranger->UpdateMSKLabelL()) ;
        }
    }

}

// ---------------------------------------------------------------------------
// CSwtButton::GetAlignment
// From MSwtButton
// ---------------------------------------------------------------------------
//
TInt CSwtButton::GetAlignment() const
{
    return iStyle & KSwtAlignmentMask;
}

// ---------------------------------------------------------------------------
// CSwtButton::SetAlignment
// From MSwtButton
// ---------------------------------------------------------------------------
//
void CSwtButton::SetAlignment(TInt aAlignment)
{
    CGraphicsContext::TTextAlign aHorizontalAlignment;
    CSwtButtonProxy::TAlignment aIconAlignment;

    iStyle &= ~KSwtAlignmentMask;
    iStyle |= (aAlignment & KSwtAlignmentMask);

    if (aAlignment == KSwtStyleCenter)         // Center
    {
        aHorizontalAlignment = CGraphicsContext::ECenter;
        aIconAlignment = CSwtButtonProxy::ECenter;
    }
    else if (aAlignment == KSwtStyleTrail)     // Trail
    {
        if ((iStyle & KSwtOrientationMask) == KSwtStyleRightToLeft
                || AknLayoutUtils::LayoutMirrored())
        {
            aHorizontalAlignment = CGraphicsContext::ELeft;
            aIconAlignment = CSwtButtonProxy::ELeft;
        }
        else
        {
            aHorizontalAlignment = CGraphicsContext::ERight;
            aIconAlignment = CSwtButtonProxy::ERight;
        }
    }
    else                                    // Lead ( default )
    {
        ASSERT(aAlignment==KSwtStyleLead);
        if ((iStyle & KSwtOrientationMask) == KSwtStyleRightToLeft
                || AknLayoutUtils::LayoutMirrored())
        {
            aHorizontalAlignment = CGraphicsContext::ERight;
            aIconAlignment = CSwtButtonProxy::ERight;
        }
        else
        {
            aHorizontalAlignment = CGraphicsContext::ELeft;
            aIconAlignment = CSwtButtonProxy::ELeft;
        }
    }

    if (iButton)
    {
        iButton->SetTextHorizontalAlignment(aHorizontalAlignment);
        iButton->SetIconHorizontalAlignment(aIconAlignment);
    }

    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtButton::SetImageL
// From MSwtButton
// ---------------------------------------------------------------------------
//
void CSwtButton::SetImageL(const MSwtImage* aImage)
{
    if (iStyle & (KSwtStylePush | KSwtStyleToggle))
    {
        if (iImage != NULL)
        {
            iImage->RemoveRef();
        }

        iImage = NULL;
        CGulIcon* img = NULL;
        if (aImage != NULL)
        {
            CFbsBitmap* bmp = SwtControlHelper::GetCopyOfBitmapLC(
                                  &aImage->Bitmap());
            ASSERT(bmp != NULL);
            CFbsBitmap* mask   = SwtControlHelper::GetInvertedCopyOfMonoBitmapLC(
                                     aImage->MaskBitmap());
            img = CGulIcon::NewL(bmp, mask);
            if (mask)
            {
                CleanupStack::Pop(mask);
            }
            CleanupStack::Pop(bmp);

            iImage = aImage;
            iImage->AddRef();
        }

        if (iButton)
        {
            iButton->SetIcon(img);
        }
        Redraw();
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::SetTextL
// From MSwtButton
// ---------------------------------------------------------------------------
//
void CSwtButton::SetTextL(const TDesC& aText)
{
    if (iButton)
    {
        iButton->SetTextL(aText);
    }
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtButton::SetDefault
// From MSwtButton
// ---------------------------------------------------------------------------
//
void CSwtButton::SetDefault(TBool aDefault)
{
    ASSERT(iStyle & KSwtStylePush);

    if (iIsDefault != aDefault)
    {
        iIsDefault = aDefault;

        if (iButton)
        {
            iButton->SetTextUnderlineStyle(iIsDefault ? EUnderlineOn : EUnderlineOff);
        }

        Redraw();
    }
}

// ---------------------------------------------------------------------------
// CSwtButton::IsDefault
// From MSwtButton
// ---------------------------------------------------------------------------
//
TBool CSwtButton::IsDefault() const
{
    return iIsDefault;
}

// ---------------------------------------------------------------------------
// CSwtButton::ClickL
// From MSwtButton
// ---------------------------------------------------------------------------
//
void CSwtButton::ClickL()
{
    SetSwtFocus(KSwtFocusByForce);
    TKeyEvent ev;
    ev.iScanCode = EStdKeyEnter;
    ev.iCode = 0;
    ev.iRepeats = 0;
    ev.iModifiers = 0;
    iButton->OfferKeyEventL(ev, EEventKeyDown);
    iButton->OfferKeyEventL(ev, EEventKey);
    iButton->OfferKeyEventL(ev, EEventKeyUp);
}

// ---------------------------------------------------------------------------
// CSwtButton::HandleControlEventL
// From MCoeControlObserver
// ---------------------------------------------------------------------------
//
void CSwtButton::HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType)
{
    if (aEventType == MCoeControlObserver::EEventStateChanged && aControl == iButton)
    {
#ifdef RD_SCALABLE_UI_V2
        if (!iDisplay.RevertPointerEvent())
        {
            SetSelection(!GetSelection());
            // Notify the Selection
            TRAP_IGNORE(iDisplay.PostSelectionEventL(iPeer));
        }
#else
        SetSelection(!GetSelection());
        // Notify the Selection
        TRAP_IGNORE(iDisplay.PostSelectionEventL(iPeer));
#endif //RD_SCALABLE_UI_V2
    }
}

#ifdef RD_SCALABLE_UI_V2
void CSwtButton::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (!iButton)
        return;

    // This is to eliminate the key focus highlight indication.
    if (iButton->IsFocused())
        iButton->SetFocus(EFalse, ENoDrawNow);

    if ((iStyle & KSwtStyleRadio)
            && (GetParent()->Control()->CaptionedControlInterface() == NULL)
            && ((GetParent()->Control()->Style() & KSwtStyleNoRadioGroup) == 0))
    {
        if (!GetSelection())
        {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            //Because native button supports only one feedback, we
            //have to do it by ourselves here. To avoid double feedback,
            //native button has feedback disabled
            //(see CSwtButtonProxy::HandlePointerEventL).
            if (IsFeedbackOnTouchDownNeeded(aPointerEvent) ||
                    IsFeedbackOnTouchReleaseNeeded(aPointerEvent))
            {
                //The same feedback for RADIO button is given
                //on touch down and release
                iFeedback->InstantFeedback(ETouchFeedbackBasicButton);
            }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            iButton->HandlePointerEventL(aPointerEvent);
        }
    }
    else
    {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        //Do feedback for other styles
        if (iStyle & KSwtStylePush)
        {
            if (IsFeedbackOnTouchDownNeeded(aPointerEvent))
            {
                iFeedback->InstantFeedback(ETouchFeedbackBasicButton);
            }
            if (IsFeedbackOnTouchReleaseNeeded(aPointerEvent))
            {
                iFeedback->EnableFeedbackForControl(this, ETrue, EFalse);
                iFeedback->InstantFeedback(this, ETouchFeedbackBasicButton);
                iFeedback->EnableFeedbackForControl(this, EFalse);
            }
        }
        else if (iStyle & KSwtStyleCheck)
        {
            //Feedabck for CHECK button
            if (IsFeedbackOnTouchDownNeeded(aPointerEvent))
            {
                iFeedback->InstantFeedback(ETouchFeedbackSensitiveButton);
            }
            if (IsFeedbackOnTouchReleaseNeeded(aPointerEvent))
            {
                iFeedback->InstantFeedback(ETouchFeedbackCheckbox);
            }
        }
        else if (iStyle && KSwtStyleToggle)
        {
            //Feedback for TOGGLE button
            CSwtButtonStateProxy* state = iButton->State();
            if (state)
            {
                //first we need to check if button is toggled (latched)
                TInt flags = state->Flags();
                if (flags & KAknButtonStateHasLatchedFrame)
                {
                    //latched
                    if (IsFeedbackOnTouchDownNeeded(aPointerEvent))
                    {
                        iFeedback->InstantFeedback(ETouchFeedbackSensitiveButton);
                    }
                    if (IsFeedbackOnTouchReleaseNeeded(aPointerEvent))
                    {
                        iFeedback->InstantFeedback(ETouchFeedbackBasicButton);
                    }
                }
                else
                {
                    //unlatched
                    if (IsFeedbackOnTouchDownNeeded(aPointerEvent))
                    {
                        //If button is not latched, only feedback on touch down
                        //is given.
                        iFeedback->InstantFeedback(ETouchFeedbackBasicButton);
                    }
                }
            }
        }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        iButton->HandlePointerEventL(aPointerEvent);
    }
}
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtButton::MSKLabelL
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
HBufC* CSwtButton::MSKLabelL() const
{
    HBufC* label = NULL;
    if ((iStyle & KSwtStylePush))
    {
        label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
    }
    else if (iStyle & KSwtStyleToggle)
    {
        label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_CHANGE);
    }
    else if (iStyle & KSwtStyleCheck)
    {
        label = iSelected ? iEikonEnv->AllocReadResourceL(R_QTN_MSK_UNMARK) : iEikonEnv->AllocReadResourceL(R_QTN_MSK_MARK);
    }
    else if ((iStyle & KSwtStyleRadio))
    {
        if (GetSelection())
        {
            return ASwtControlBase::MSKLabelL();
        }
        label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
    }

    ASSERT(label);
    return label;
}
