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


#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <eiklbi.h>
#include <eikenv.h>
#include <AknsUtils.h>
#include <aknconsts.h>
#include <AknLayoutFont.h>
#include <AknBidiTextUtils.h>
#include <swtlaffacade.h>
#include <avkon.mbg>
#include "swtlabel.h"
#include "swtcontrolbase.h"
#include "swtfont.h"


const TInt KBorderMargin = 1;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtLabel::CSwtLabel
// ---------------------------------------------------------------------------
//
CSwtLabel::CSwtLabel(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                     MSwtComposite& aParent, TInt aStyle)
        : ASwtControlBase(aDisplay, aPeer, &aParent, aStyle)
        , iOriginalText(NULL)
{
    SetFocusing(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtLabel::~CSwtLabel
// ---------------------------------------------------------------------------
//
CSwtLabel::~CSwtLabel()
{
    delete iEikLabel;
    delete iSeparatorHead;
    delete iSeparatorHeadMask;
    delete iSeparatorCenter;
    delete iSeparatorCenterMask;
    delete iSeparatorTail;
    delete iSeparatorTailMask;
    delete iOriginalText;

    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
    }

    if (iImage != NULL)
        iImage->RemoveRef();

#ifdef _lint
    iDefaultFont = NULL;
    iImage       = NULL;
    iForegroundColor = NULL;
#endif
}

// ---------------------------------------------------------------------------
// CSwtLabel::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtLabel::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();

    SetContainerWindowL(coeParent);

    CAknControl::MakeVisible(coeParent.IsVisible());
    CAknControl::SetDimmed(coeParent.IsDimmed());

    if (iStyle & KSwtStyleSeparator)
    {
        CreateSeparatorL();
    }
    else
    {
        CreateTextL();
    }

    SetBackground(this);   // Back will be drawn by ASwtControlBase::Draw
    ActivateL();
}

// ---------------------------------------------------------------------------
// CSwtLabel::CreateTextL
// ---------------------------------------------------------------------------
//
void CSwtLabel::CreateTextL()
{
    ASSERT(!(iStyle & KSwtStyleSeparator));
    ASSERT(!iEikLabel);

    iEikLabel = new(ELeave) CEikLabel();
    iEikLabel->SetFocusing(EFalse);
    iEikLabel->SetContainerWindowL(*this);
    iEikLabel->MakeVisible(EFalse);
    iEikLabel->SetDimmed(IsDimmed());
    iEikLabel->SetTextL(KNullDesC);
    SetAlignment(iStyle & KSwtAlignmentMask);
    RetrieveDefaultFontL();
    DoSetFontL(&iDefaultFont->Font());
    UpdateTextColor();

    // Correct position of CEikLabel will be set in PositionChanged()
}

// ---------------------------------------------------------------------------
// CSwtLabel::CreateSeparatorL
// ---------------------------------------------------------------------------
//
void CSwtLabel::CreateSeparatorL()
{
    ASSERT(!iEikLabel);
    ASSERT(!iImage);

    delete iSeparatorHead;
    iSeparatorHead = NULL;
    delete iSeparatorHeadMask;
    iSeparatorHeadMask = NULL;
    delete iSeparatorCenter;
    iSeparatorCenter = NULL;
    delete iSeparatorCenterMask;
    iSeparatorCenterMask = NULL;
    delete iSeparatorTail;
    iSeparatorTail = NULL;
    delete iSeparatorTailMask;
    iSeparatorTailMask = NULL;

    // Get skin and bitmap ids
    TAknsItemID separatorHeadSkinId;
    TAknsItemID separatorCenterSkinId;
    TAknsItemID separatorTailSkinId;

    TInt separatorCenterId;
    TInt separatorCenterMaskId;

    if (iStyle & KSwtStyleVertical)
    {

        if (iStyle & KSwtStyleShadowIn)
        {
            separatorHeadSkinId.Set(KAknsIIDQsnFrSeparatorVTShdwin);
            separatorCenterSkinId.Set(KAknsIIDQsnFrSeparatorVMShdwin);
            separatorTailSkinId.Set(KAknsIIDQsnFrSeparatorVBShdwin);
        }
        else if (iStyle & KSwtStyleShadowOut)
        {
            separatorHeadSkinId.Set(KAknsIIDQsnFrSeparatorVTShdwout);
            separatorCenterSkinId.Set(KAknsIIDQsnFrSeparatorVMShdwout);
            separatorTailSkinId.Set(KAknsIIDQsnFrSeparatorVBShdwout);
        }
        else
        {
            separatorHeadSkinId.Set(KAknsIIDQsnFrSeparatorVTShdwnone);
            separatorCenterSkinId.Set(KAknsIIDQsnFrSeparatorVMShdwnone);
            separatorTailSkinId.Set(KAknsIIDQsnFrSeparatorVBShdwnone);
        }
    }
    else
    {

        if (iStyle & KSwtStyleShadowIn)
        {
            separatorHeadSkinId.Set(KAknsIIDQsnFrSeparatorHLShdwin);
            separatorCenterSkinId.Set(KAknsIIDQsnFrSeparatorHMShdwin);
            separatorTailSkinId.Set(KAknsIIDQsnFrSeparatorHRShdwin);
        }
        else if (iStyle & KSwtStyleShadowOut)
        {
            separatorHeadSkinId.Set(KAknsIIDQsnFrSeparatorHLShdwout);
            separatorCenterSkinId.Set(KAknsIIDQsnFrSeparatorHMShdwout);
            separatorTailSkinId.Set(KAknsIIDQsnFrSeparatorHRShdwout);
        }
        else
        {
            separatorHeadSkinId.Set(KAknsIIDQsnFrSeparatorHLShdwnone);
            separatorCenterSkinId.Set(KAknsIIDQsnFrSeparatorHMShdwnone);
            separatorTailSkinId.Set(KAknsIIDQsnFrSeparatorHRShdwnone);
        }
    }


    MAknsSkinInstance* skin = AknsUtils::SkinInstance();


    // first, try to get separator specific images from skin
    _LIT(KNoneFile," ");
    TInt err(KErrNone);

    TRAP(err, AknsUtils::CreateIconL(
             skin,
             separatorHeadSkinId,
             iSeparatorHead,
             iSeparatorHeadMask,
             KNoneFile,
             0,
             0));


    if (err == KErrNone)
    {

        TRAP(err, AknsUtils::CreateIconL(
                 skin,
                 separatorCenterSkinId,
                 iSeparatorCenter,
                 iSeparatorCenterMask,
                 KNoneFile,
                 0,
                 0));
    }

    if (err == KErrNone)
    {

        TRAP(err, AknsUtils::CreateIconL(
                 skin,
                 separatorTailSkinId,
                 iSeparatorTail,
                 iSeparatorTailMask,
                 KNoneFile,
                 0,
                 0));
    }

    if (err != KErrNone)
        // if separator specific images are not available from skin
        // try to use dashed line image from skin, or from image file
    {

        if (iStyle & KSwtStyleVertical)
        {
            separatorCenterSkinId = KAknsIIDQgnGrafLinePrimaryVerticalDashed;
            separatorCenterId = EMbmAvkonQgn_graf_line_primary_vertical_dashed;
            separatorCenterMaskId = EMbmAvkonQgn_graf_line_primary_vertical_dashed_mask;

        }
        else
        {
            separatorCenterSkinId = KAknsIIDQgnGrafLinePrimaryHorizontalDashed;
            separatorCenterId = EMbmAvkonQgn_graf_line_primary_horizontal_dashed;
            separatorCenterMaskId = EMbmAvkonQgn_graf_line_primary_horizontal_dashed_mask;
        }

        AknsUtils::CreateIconL(
            skin,
            separatorCenterSkinId,
            iSeparatorCenter,
            iSeparatorCenterMask,
            KAvkonBitmapFile,
            separatorCenterId,
            separatorCenterMaskId);

        delete iSeparatorHead;
        iSeparatorHead = NULL;
        delete iSeparatorHeadMask;
        iSeparatorHeadMask =NULL;
        delete iSeparatorTail;
        iSeparatorTail = NULL;
        delete iSeparatorTailMask;
        iSeparatorTailMask = NULL;
        iDashedLineSeparatorInUse =  ETrue;
    }


    // Get separator thickness
    TAknLayoutRect layoutRect;
    const TInt KBigEnough(50);
    if (iStyle & KSwtStyleVertical)
    {
        layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorVerticalPane,
                     TRect(0, 0, KBigEnough, KBigEnough), 0);
        iSeparatorThickness = layoutRect.Rect().Size().iWidth;
    }
    else
    {
        layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorHorizontalPane,
                     TRect(0, 0, KBigEnough, KBigEnough), 0);
        iSeparatorThickness = layoutRect.Rect().Size().iHeight;
    }

    if (iDashedLineSeparatorInUse)
    {
        iSeparatorThickness = 1;
    }

    ResizeSeparatorImages();

}

// ---------------------------------------------------------------------------
// CSwtLabel::NewL
// ---------------------------------------------------------------------------
//
CSwtLabel* CSwtLabel::NewL(MSwtDisplay& aDisplay,
                           TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle)
{
    CSwtLabel* self = new(ELeave) CSwtLabel(aDisplay, aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtLabel::ClipTextL
// ---------------------------------------------------------------------------
//
void  CSwtLabel::ClipTextL(TInt aWHint)
{
    if (iOriginalText == NULL || aWHint <= 0 || !iEikLabel)
    {
        return;
    }

    RBuf textToClip;
    textToClip.Create(*iOriginalText);
    textToClip.CleanupClosePushL();

    const CFont* font = iEikLabel->Font();
    if (iOriginalTextLineCount == 1)
    {
        if (aWHint > font->CharWidthInPixels(CEditableText::EEllipsis))
        {
            AknTextUtils::ClipToFit(textToClip, *font, aWHint);
        }
    }
    iEikLabel->SetTextL(textToClip);

    CleanupStack::PopAndDestroy(); // textToClip
}

// ---------------------------------------------------------------------------
// CSwtLabel::ComputeTextSizeL
// ---------------------------------------------------------------------------
//
TSize CSwtLabel::ComputeTextSizeL(TInt aWHint, TInt aHHint)
{
    // Return non default hints
    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return TSize(aWHint, aHHint);
    }

    // Compute size
    TSize prefSize(0, 0);
    if ((iStyle & KSwtStyleWrap) && !(aWHint == KSwtDefault && aHHint == KSwtDefault))
    {
        // Wrap text with a default hint
        if (aWHint != KSwtDefault)
        {
            prefSize.iWidth = aWHint;
            prefSize.iHeight = CalcWrappedTextHeightL(aWHint);
        }
        else
        {
            prefSize.iHeight = aHHint;
            prefSize.iWidth = CalcWrappedTextWidth(aHHint);
        }
    }
    else
    {
        // This will return the width of the longer line and height that fits all lines
        prefSize = MinimumSize();
    }

    // Avoid text touching the border
    prefSize += TPoint(2 * KBorderMargin, 2 * KBorderMargin);

    if (aHHint != KSwtDefault)
    {
        prefSize.iHeight = aHHint;
    }

    if (aWHint != KSwtDefault)
    {
        prefSize.iWidth = aWHint;
    }

    return prefSize;
}

// ---------------------------------------------------------------------------
// CSwtLabel::CalcWrappedTextHeightL
// ---------------------------------------------------------------------------
//
TInt CSwtLabel::CalcWrappedTextHeightL(TInt aWidth)
{
    if (iOriginalText == NULL || aWidth <= 0 || !iEikLabel)
    {
        return 0;
    }

    // Calculate the number of lines when wrapping text to the given rectangle
    const TInt ARRAY_GRANULARITY = 4;
    CArrayFixFlat<TPtrC>* wrappedArray = new(ELeave) CArrayFixFlat<TPtrC>(ARRAY_GRANULARITY);
    CleanupStack::PushL(wrappedArray);
    const CFont* font(iEikLabel->Font());

    // NOTE!!! AknTextUtils::WrapToArrayL behaves strange for strings that
    // contain '-' (dash) char. Sometimes return one lesser line if several
    // dashes are encountered.

    AknTextUtils::WrapToArrayL(*iOriginalText, aWidth, *font, *wrappedArray);
    TInt nbStrings = wrappedArray->Count();
    if (nbStrings < 1)
    {
        // It happens when aWidth < Char's width
        // here just set nBStrings to 1
        // so something can be visible.
        nbStrings = 1;
    }

    // Calculate text height
    const TInt result(MinimumHeight(nbStrings));

    wrappedArray->Reset();
    CleanupStack::PopAndDestroy(wrappedArray);

    return result;
}

// ---------------------------------------------------------------------------
// CSwtLabel::CalcWrappedTextWidth
// ---------------------------------------------------------------------------
//
TInt CSwtLabel::CalcWrappedTextWidth(TInt aHeight)
{
    if (iOriginalText == NULL || aHeight <= 0)
    {
        return 0;
    }
    // This will return the width of the longer line and height that fits all lines
    return MinimumWidth();
}

// ---------------------------------------------------------------------------
// CSwtLabel::WrapTextL
// ---------------------------------------------------------------------------
//
void CSwtLabel::WrapTextL(TSize aSize)
{
    WrapTextL(aSize.iWidth, aSize.iHeight);
}

// ---------------------------------------------------------------------------
// CSwtLabel::WrapTextL
// ---------------------------------------------------------------------------
//
void CSwtLabel::WrapTextL(TInt aWHint, TInt aHHint)
{
    if (iOriginalText == NULL || aWHint <= 0 || aHHint <= 0 || !iEikLabel)
    {
        return;
    }

    const CFont* font = iEikLabel->Font();

    // Estimate how many text rows within given rectangle
    TInt textPaneHeight = GetFontHeight(font);
    const TInt gap = iEikLabel->PixelGapBetweenLines();

    // The last row in label does not add a gap after it. So...
    // aHHint = rows * textPaneHeight + (rows - 1) * gap; and therefore...
    TInt rows = (aHHint + gap) / (textPaneHeight + gap);
    if (rows < 1)
    {
        // It happens when hint height < Char's height
        // here just set nBStrings to 1
        // so something can be visible.
        rows = 1;
    }

    // Setup width arrray
    CArrayFixFlat< TInt >* widthsArray = new(ELeave) CArrayFixFlat< TInt >(rows);
    CleanupStack::PushL(widthsArray);
    widthsArray->SetReserveL(rows);
    TInt i;
    for (i = 0; i < rows; ++i)
    {
        widthsArray->AppendL(aWHint);
    }

    // Convert text into wrapped text with AKnTextUtils
    RBuf wrappedBuffer;
    const TInt KDouble(2);
    wrappedBuffer.CreateL(KDouble *(iOriginalText->Length()));
    wrappedBuffer.CleanupClosePushL();
    AknTextUtils::WrapToStringAndClipL(*iOriginalText, *widthsArray, *font, wrappedBuffer);

    // Set wrapped text
    iEikLabel->SetTextL(wrappedBuffer);

    CleanupStack::PopAndDestroy(); // wrappedBuffer
    CleanupStack::PopAndDestroy(widthsArray);
}

// ---------------------------------------------------------------------------
// CSwtLabel::RetrieveDefaultFontL
// ---------------------------------------------------------------------------
//
void CSwtLabel::RetrieveDefaultFontL()
{
    ASSERT(iEikLabel);
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }
    // Same as Button's
    iDefaultFont = CSwtFont::NewL(iDisplay.Device(), iDisplay.Device().GetSystemFont()->Font());
}

// ---------------------------------------------------------------------------
// CSwtLabel::MinimumSize
// Replaces CEikLabel's MinimumSize to fix some texts getting ellipsed and clipped
// ---------------------------------------------------------------------------
//
TSize CSwtLabel::MinimumSize() const
{
    const TInt height = MinimumHeight();
    const TInt width = MinimumWidth();
    return TSize(width, height);
}

// ---------------------------------------------------------------------------
// CSwtLabel::MinimumWidth
// Replaces CEikLabel's MinimumWidth to fix some texts getting ellipsed.
// ---------------------------------------------------------------------------
//
TInt CSwtLabel::MinimumWidth() const
{
    if (!iEikLabel || !iOriginalText)
    {
        return 0;
    }

    TInt result = 0;
    TPtrC ptr(iOriginalText->Des());
    const CFont* font = iEikLabel->Font();
    CFont::TMeasureTextInput::TFlags order = CFont::TMeasureTextInput::EFVisualOrder;
    if (AknLayoutUtils::LayoutMirrored())
    {
        order = CFont::TMeasureTextInput::EFVisualOrderRightToLeft;
    }

    FOREVER
    {
        const TInt pos = ptr.Locate('\n');
        TPtrC left = ptr;
        if (pos >= 0)
        {
            left.Set(ptr.Left(pos));
        }
        TInt lineW = AknBidiTextUtils::MeasureTextBoundsWidth(*font, left, order);
        if (lineW > result)
        {
            result = lineW;
        }
        if (pos < 0 || pos == ptr.Length() - 1)
        {
            break;
        }
        ptr.Set(ptr.Mid(pos + 1));
    }

    return result;
}

// ---------------------------------------------------------------------------
// CSwtLabel::MinimumHeight
// Replaces CEikLabel's MinimumHeight to fix some texts getting clipped
// ---------------------------------------------------------------------------
//
TInt CSwtLabel::MinimumHeight() const
{
    return(MinimumHeight(iOriginalTextLineCount));
}

// ---------------------------------------------------------------------------
// CSwtLabel::MinimumHeight
// Replaces CEikLabel's MinimumHeight to fix some texts getting clipped
// ---------------------------------------------------------------------------
//
TInt CSwtLabel::MinimumHeight(TInt aLineCount) const
{
    if (!iEikLabel)
    {
        return 0;
    }

    // Compared to CEikLabel's MinimumHeight() we add one more gap.
    // This is to fix some fonts getting clipped from above.
    // Do not change this ever, as wrapping will surely brake.
    return aLineCount * iEikLabel->PixelGapBetweenLines()
           + aLineCount * GetFontHeight(iEikLabel->Font());
}

// ---------------------------------------------------------------------------
// CSwtLabel::DoSetFontL
// ---------------------------------------------------------------------------
//
void CSwtLabel::DoSetFontL(const CFont* aFont)
{
    if (!iEikLabel)
    {
        return;
    }
    iEikLabel->SetFont(aFont);
    if (aFont)
    {
        // The gap must be set exaclty in the same way as in AknLayoutUtils::LayoutLabel().
        // So, do not modify this unless CEikLabel does too!
        iEikLabel->SetPixelGapBetweenLines(
            Max(aFont->FontLineGap() - aFont->HeightInPixels(), KSwtMinLinePadding));

        // This is to fix some fonts getting to much space after last line.
        iEikLabel->iMargin.iTop = aFont->FontMaxDescent();

        TRAP_IGNORE(ClipTextL(iEikLabel->Size().iWidth));
    }
}

// ---------------------------------------------------------------------------
// CSwtLabel::GetFontHeight
// ---------------------------------------------------------------------------
//
TInt CSwtLabel::GetFontHeight(const CFont* aFont) const
{
    if (!aFont)
    {
        return 0;
    }

#ifdef RD_JAVA_S60_RELEASE_9_2
    return aFont->FontMaxHeight();
#else
    return aFont->HeightInPixels();
#endif
}

// ---------------------------------------------------------------------------
// CSwtLabel::Draw
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::Draw(const TRect& aRect) const
{
    ASSERT(!(iCtrlFlags & EFlagDoNotDraw));

    const TRect clientRect(BorderInnerRect());
    TRect dirtyClientRect(clientRect);
    dirtyClientRect.Intersection(aRect);

    CWindowGc& gc = SystemGc();

    // Draw the image if we have one
    if (iImage != NULL && !iEikLabel->IsVisible())   //lint !e613 iEikLabel cannot be NULL if iImage is not
    {
        const TSize imgSize(iImage->GetBounds().Size());
        TPoint imgPos(TPoint::EUninitialized);


        const TBool leftToRight((iStyle & KSwtStyleRightToLeft)
                                || AknLayoutUtils::LayoutMirrored());
        if (iStyle & KSwtStyleCenter)
        {
            imgPos.iX = clientRect.iTl.iX + (clientRect.Width() - imgSize.iWidth) / 2;
        }
        else if (((iStyle & KSwtStyleTrail) && (!leftToRight))
                 || ((iStyle &  KSwtStyleLead) && leftToRight))
        {
            imgPos.iX = clientRect.iBr.iX - imgSize.iWidth;
        }
        else
        {
            imgPos.iX = clientRect.iTl.iX;
        }

        // Vertical alignment ( always centered )
        imgPos.iY = clientRect.iTl.iY + (clientRect.Height() - imgSize.iHeight) / 2;

        TRect drawRect(imgPos, imgSize);
        drawRect.Intersection(dirtyClientRect);

        if (!drawRect.IsEmpty())
        {
            TRect imgRect(drawRect);
            imgRect.Move(-imgPos);

            const CFbsBitmap& bitmap = iImage->Bitmap();
            const CFbsBitmap* mask   = iImage->MaskBitmap();
            if (mask == NULL)
                gc.BitBlt(drawRect.iTl, &bitmap, imgRect);
            else
                gc.BitBltMasked(drawRect.iTl, &bitmap, imgRect, mask, EFalse);
        }
    }
    else if (iSeparatorCenter)
    {
        DrawSeparator(gc);
    }
}

// ---------------------------------------------------------------------------
// CSwtLabel::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::FocusChanged(TDrawNow aDrawNow)
{
    // Labels are not supposed to gain focus on their own, but they may still
    // get it through ForceFocus().
    TBool focused = IsFocused();
    if (iEikLabel)
    {
        iEikLabel->SetFocus(focused);
    }

    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtLabel::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::SetDimmed(TBool aDimmed)
{
    CAknControl::SetDimmed(aDimmed);
    if (iEikLabel)
    {
        iEikLabel->SetDimmed(aDimmed);
    }
}

// ---------------------------------------------------------------------------
// CSwtLabel::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtLabel::ComponentControl(TInt /*aIndex*/) const
{
    if (iEikLabel)
    {
        return static_cast< CCoeControl* >(iEikLabel);
    }
    else
    {
        return NULL;
    }
} //lint !e1763

// ---------------------------------------------------------------------------
// CSwtLabel::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtLabel::CountComponentControls() const
{
    if (iEikLabel)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// ---------------------------------------------------------------------------
// CSwtLabel::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtLabel::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    return HandleKeyL(aKeyEvent, aType, ETrue);
}

// ---------------------------------------------------------------------------
// CSwtLabel::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}

// ---------------------------------------------------------------------------
// CSwtLabel::SwtHandleResourceChangeL
// ---------------------------------------------------------------------------
//
void CSwtLabel::SwtHandleResourceChangeL(TInt aType)
{
    CAknControl::HandleResourceChange(aType);
    if (aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch)
    {
        if (iEikLabel)
        {
            RetrieveDefaultFontL();
            if (!iFont)
            {
                iEikLabel->SetFont(&iDefaultFont->Font());
            }
        }
        else if (iStyle & KSwtStyleSeparator)
        {
            CreateSeparatorL();
        }

        if (aType == KAknsMessageSkinChange)
        {
            UpdateTextColor();
        }
    }
    else if (aType == KEikDynamicLayoutVariantSwitch)
    {
        SetAlignment(iStyle & KSwtAlignmentMask);
    }
}

// ---------------------------------------------------------------------------
// CSwtLabel::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::PositionChanged()
{
    if (iEikLabel)
    {
        TRect clientRect(BorderInnerRect());
        if (HasBorderStyle())
        {
            // The text should not touch the border
            iEikLabel->SetPosition(clientRect.iTl + TPoint(KBorderMargin, KBorderMargin));
        }
        else
        {
            iEikLabel->SetPosition(clientRect.iTl);
        }
    }
    else if (iSeparatorCenter)
    {
        ResizeSeparatorImages();
    }

    HandlePositionChanged();
}

// ---------------------------------------------------------------------------
// CSwtLabel::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::SizeChanged()
{
    if (iEikLabel)
    {
        const TRect clientRect(BorderInnerRect());
        if (HasBorderStyle())
        {
            // The text should not touch the border
            iEikLabel->SetSize(clientRect.Size() - TPoint(2 * KBorderMargin, 2 * KBorderMargin));
        }
        else
        {
            iEikLabel->SetSize(clientRect.Size());
        }

        if (iStyle & KSwtStyleWrap)
        {
            TRAP_IGNORE(WrapTextL(iEikLabel->Size()));
        }
        else
        {
            TRAP_IGNORE(ClipTextL(iEikLabel->Size().iWidth));
        }
    }
    else if (iSeparatorCenter)
    {
        ResizeSeparatorImages();
    }

    HandleSizeChanged();
}

// ---------------------------------------------------------------------------
// CSwtLabel::MopSupplyObject
// From CCoeControl
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtLabel::MopSupplyObject(TTypeUid aId)
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

// ---------------------------------------------------------------------------
// CSwtLabel::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtLabel::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtLabel::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtLabel::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtLabel::IsFocusable
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtLabel::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    const CCoeControl& coeCtrl = CoeControl();

    // Invisible or disabled controls can never get the focus ( not even forced )
    if (!coeCtrl.IsVisible() || coeCtrl.IsDimmed())
        return EFalse;
    else
        return aReason == KSwtFocusByForce;
}

// ---------------------------------------------------------------------------
// CSwtLabel::ProcessKeyEventL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::ProcessKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
{
    // Do nothing
}

// ---------------------------------------------------------------------------
// CSwtLabel::ComputeSizeL
// From MSwtControl
// ---------------------------------------------------------------------------
//
TSize CSwtLabel::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return TSize(aWHint, aHHint);
    }

    TSize prefSize(0, 0);
    if (iSeparatorCenter)
    {
        if (aHHint != KSwtDefault)
        {
            prefSize.iHeight = aHHint;
        }
        else if (!(iStyle & KSwtStyleVertical))
        {
            prefSize.iHeight = Min(iSeparatorThickness, prefSize.iHeight);
        }

        if (aWHint != KSwtDefault)
        {
            prefSize.iWidth = aWHint;
        }
        else if (iStyle & KSwtStyleVertical)
        {
            prefSize.iWidth = Min(iSeparatorThickness, prefSize.iWidth);
        }
    }

    if (iImage)
    {
        prefSize = iImage->GetBounds().Size();
    }
    else
    {
        prefSize = ComputeTextSizeL(aWHint, aHHint);
    }

    if (!iSeparatorCenter)
    {
        prefSize += BorderSizeDelta();
    }

    if (aWHint != KSwtDefault)
    {
        prefSize.iWidth = aWHint;
    }
    if (aHHint != KSwtDefault)
    {
        prefSize.iHeight = aHHint;
    }

    return prefSize;
}

// ---------------------------------------------------------------------------
// CSwtLabel::DrawSeparator
// ---------------------------------------------------------------------------
//
void CSwtLabel::DrawSeparator(CWindowGc& aGc) const
{
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

    if (iSeparatorHead && iSeparatorHeadMask)
    {
        aGc.BitBltMasked(iSeparatorHeadRect.iTl, iSeparatorHead,
                         iSeparatorHeadRect.Size(), iSeparatorHeadMask, ETrue);
    }
    else if (iSeparatorHead)
    {
        aGc.BitBlt(iSeparatorHeadRect.iTl, iSeparatorHead, iSeparatorHeadRect.Size());
    }

    if (iSeparatorCenter && iSeparatorCenterMask)
    {
        aGc.BitBltMasked(iSeparatorCenterRect.iTl, iSeparatorCenter,
                         iSeparatorCenterRect.Size(), iSeparatorCenterMask, ETrue);
    }
    else if (iSeparatorCenter)
    {
        aGc.BitBlt(iSeparatorCenterRect.iTl, iSeparatorCenter, iSeparatorCenterRect.Size());
    }

    if (iSeparatorTail && iSeparatorTailMask)
    {
        aGc.BitBltMasked(iSeparatorTailRect.iTl, iSeparatorTail,
                         iSeparatorTailRect.Size(), iSeparatorTailMask, ETrue);
    }
    else if (iSeparatorTail)
    {
        aGc.BitBlt(iSeparatorTailRect.iTl, iSeparatorTail, iSeparatorTailRect.Size());
    }
}

// ---------------------------------------------------------------------------
// CSwtLabel::CalcAndUpdateSeparatorRect
// ---------------------------------------------------------------------------
//
TRect CSwtLabel::CalcAndUpdateSeparatorRect()
{
    TRect rect(Rect());

    TRect separatorRect;
    TAknLayoutRect layoutRect;
    if (iStyle & KSwtStyleVertical)
    {
        layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorVerticalPane, rect, 0);
        separatorRect =  layoutRect.Rect();
        if (iDashedLineSeparatorInUse)
        {
            separatorRect.SetWidth(iSeparatorThickness);
        }
        // adjust seperator's width according to the width of the parent
        if (separatorRect.Width() > rect.Width())
        {
            separatorRect.iTl.iX = rect.iTl.iX;
            separatorRect.iBr.iX = rect.iBr.iX;
        }

        // move to center
        TInt xPos = rect.iTl.iX + Max(0, (rect.Width() - separatorRect.Width()) / 2);
        separatorRect.SetRect(TPoint(xPos, separatorRect.iTl.iY), separatorRect.Size());

        if (iDashedLineSeparatorInUse)
        {
            iSeparatorCenterRect = separatorRect;
        }
        else
        {
            layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorVerticalPaneG1, separatorRect, 0);
            iSeparatorHeadRect = layoutRect.Rect();
            layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorVerticalPaneG2, separatorRect, 0);
            iSeparatorCenterRect = layoutRect.Rect();
            layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorVerticalPaneG3, separatorRect, 0);
            iSeparatorTailRect = layoutRect.Rect();

            // adjust seperator's height according to the height of the parent
            if ((iSeparatorHeadRect.Height()
                    + iSeparatorTailRect.Height()) > separatorRect.Height())
            {
                iSeparatorHeadRect = separatorRect;
                iSeparatorHeadRect.iBr.iY  = separatorRect.Center().iY;
                iSeparatorTailRect = separatorRect;
                iSeparatorTailRect.iTl.iY  = iSeparatorHeadRect.iBr.iY;
            }
        }

    }
    else
    {
        layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorHorizontalPane, rect, 0);
        separatorRect =  layoutRect.Rect();
        if (iDashedLineSeparatorInUse)
        {
            separatorRect.SetHeight(iSeparatorThickness);
        }
        // adjust seperator's height according to height of the parent
        if (separatorRect.Height() > rect.Height())
        {
            separatorRect.iTl.iY = rect.iTl.iY;
            separatorRect.iBr.iY = rect.iBr.iY;
        }

        // move to center
        TInt yPos = rect.iTl.iY + Max(0,
                                      (rect.Height() - separatorRect.Height()) / 2);
        separatorRect.SetRect(TPoint(separatorRect.iTl.iX, yPos), separatorRect.Size());

        if (iDashedLineSeparatorInUse)
        {
            iSeparatorCenterRect = separatorRect;
        }
        else
        {
            layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorHorizontalPaneG1, separatorRect, 0);
            iSeparatorHeadRect = layoutRect.Rect();
            layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorHorizontalPaneG2, separatorRect, 0);
            iSeparatorCenterRect = layoutRect.Rect();
            layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::ESeparatorHorizontalPaneG3, separatorRect, 0);
            iSeparatorTailRect = layoutRect.Rect();

            // adjust seperator's width according to the width of the parent
            if ((iSeparatorHeadRect.Width()
                    + iSeparatorTailRect.Width()) > separatorRect.Width())
            {
                iSeparatorHeadRect = separatorRect;
                iSeparatorHeadRect.iBr.iX  = separatorRect.Center().iX;
                iSeparatorTailRect = separatorRect;
                iSeparatorTailRect.iTl.iX  = iSeparatorHeadRect.iBr.iX;
            }
        }

    }

    return separatorRect;
}

// ---------------------------------------------------------------------------
// CSwtLabel::ResizeSeparatorImages
// ---------------------------------------------------------------------------
//
void  CSwtLabel::ResizeSeparatorImages()
{
    CalcAndUpdateSeparatorRect();
    AknIconUtils::SetSize(iSeparatorHead,
                          iSeparatorHeadRect.Size(), EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iSeparatorHeadMask,
                          iSeparatorHeadRect.Size(), EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iSeparatorCenter,
                          iSeparatorCenterRect.Size(), EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iSeparatorCenterMask,
                          iSeparatorCenterRect.Size(), EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iSeparatorTail,
                          iSeparatorTailRect.Size(), EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iSeparatorTailMask,
                          iSeparatorTailRect.Size(), EAspectRatioNotPreserved);
}

// ---------------------------------------------------------------------------
// CSwtLabel::DefaultFont
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::SetFontL(const MSwtFont* aFont)
{
    ASwtControlBase::DoSetFontL(aFont);

    if (iEikLabel)
    {
        const MSwtFont* swtFont = GetFont();
        ASSERT(swtFont != NULL);
        const CFont& font = (swtFont->Font());
        DoSetFontL(&font);
    }

    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtLabel::UpdateTextColor
// ---------------------------------------------------------------------------
//
void CSwtLabel::UpdateTextColor()
{
    // This method is used only when label has parent, that has gained focus
    // highlight. In such case label gets focus highlight and text color
    // has to be updated
    if (iEikLabel)
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
            TRAP_IGNORE(iEikLabel->OverrideColorL(EColorLabelText, color));
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtLabel::HandleHighlightChange
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::HandleHighlightChange()
{
    UpdateTextColor();
}

// ---------------------------------------------------------------------------
// CSwtLabel::SetForegroundL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtLabel::SetForegroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetForegroundL(aColor);
    SetColorL(EColorLabelText, aColor);
    iCustomFg = aColor;
    UpdateTextColor();
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtLabel::DefaultFont
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
const MSwtFont& CSwtLabel::DefaultFont() const
{
    if (iDefaultFont != NULL)   // Implies iEikLabel != NULL
        return *iDefaultFont;

    return *iDisplay.Device().GetSystemFont();
}

// ---------------------------------------------------------------------------
// CSwtLabel::Control
// From MSwtLabel
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtLabel::Control()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtLabel::GetAlignment
// From MSwtLabel
// ---------------------------------------------------------------------------
//
TInt CSwtLabel::GetAlignment() const
{
    return iStyle & KSwtAlignmentMask;
}

// ---------------------------------------------------------------------------
// CSwtLabel::SetAlignment
// From MSwtLabel
// ---------------------------------------------------------------------------
//
void CSwtLabel::SetAlignment(TInt aAlignment)
{
    ASSERT(!(iStyle & KSwtStyleSeparator));

    TGulAlignment alignmentValueOS;
    if (aAlignment == KSwtStyleCenter)         // Center
    {
        alignmentValueOS = EHCenterVCenter;
    }
    else if (aAlignment == KSwtStyleTrail)     // Trail
    {
        if ((iStyle & KSwtStyleRightToLeft)
                || AknLayoutUtils::LayoutMirrored())
            alignmentValueOS = EHLeftVCenter;
        else
            alignmentValueOS = EHRightVCenter;
    }
    else                                    // Lead ( default )
    {
        ASSERT(aAlignment==KSwtStyleLead);
        if (iStyle & KSwtStyleRightToLeft
                || AknLayoutUtils::LayoutMirrored())
            alignmentValueOS = EHRightVCenter;
        else
            alignmentValueOS = EHLeftVCenter;
    }

    iStyle &= ~KSwtAlignmentMask;
    iStyle |= aAlignment;

    if (iEikLabel != NULL)
    {
        iEikLabel->SetAlignment(alignmentValueOS);
    }

    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtLabel::SetImageL
// From MSwtLabel
// ---------------------------------------------------------------------------
//
void CSwtLabel::SetImageL(const MSwtImage* aImage)
{
    ASSERT(!(iStyle & KSwtStyleSeparator));
    ASSERT(!iSeparatorCenter);

    if (iImage != NULL)
        iImage->RemoveRef();

    iImage = aImage;

    TBool showText;
    if (iImage != NULL)
    {
        iImage->AddRef();
        showText = EFalse;
    }
    else
    {
        showText = ETrue;
    }

    iEikLabel->MakeVisible(showText);   //lint !e613

    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtLabel::SetTextL
// From MSwtLabel
// NOTE! The text was already set on the Java side and that Label.getText() returns.
// ---------------------------------------------------------------------------
//
void CSwtLabel::SetTextL(const TDesC& aText)
{
    ASSERT(!(iStyle & KSwtStyleSeparator));

    // Replace local copy of the text
    delete iOriginalText;
    iOriginalText = NULL;
    iOriginalText = aText.AllocL();
    iEikLabel->MakeVisible(ETrue);

    // Count lines and cache result
    TInt numberOfLines = 1;
    TPtrC ptr = aText;
    FOREVER
    {
        const TInt offset = ptr.Locate('\n');
        if (offset < 0 || offset == ptr.Length() - 1)
        {
            break;
        }
        numberOfLines++;
        ptr.Set(ptr.Mid(offset + 1));
    }
    iOriginalTextLineCount = numberOfLines;

    // Wrap or clip text and set the result to CEikLabel
    if (iStyle & KSwtStyleWrap)
    {
        WrapTextL(iEikLabel->Size());
    }
    else
    {
        ClipTextL(iEikLabel->Size().iWidth);
    }

    Redraw();
}
