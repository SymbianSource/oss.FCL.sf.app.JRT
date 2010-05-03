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


#include <swtlaffacade.h>
#include <swtbrowserschemehandler.h>
#include <AknBidiTextUtils.h>
#include <AknsUtils.h>
#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK
#include "swtfont.h"
#include "swthyperlink.h"


// The scheme prefixes used by the scheme handler to recognise the content type
_LIT(KSwtHyperLinkMail, "mailto:");
_LIT(KSwtHyperLinkTel, "tel:");
_LIT(KSwtHyperLinkHttp, "http://");


// ======== MEMBER FUNCTIONS ========


CSwtHyperLink* CSwtHyperLink::NewL(MSwtDisplay&   aDisplay,
                                   TSwtPeer       aPeer,
                                   MSwtComposite& aParent,
                                   TInt           aStyle)
{
    CSwtHyperLink* self = new(ELeave) CSwtHyperLink(aDisplay,
            aPeer,
            aParent,
            aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}


CSwtHyperLink::~CSwtHyperLink()
{
#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->RemoveFeedbackForControl(this);
    }
#endif // RD_TACTILE_FEEDBACK
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }
    iFormattedText.Close();
    iOriginalText.Close();
}


CSwtHyperLink::CSwtHyperLink(MSwtDisplay&   aDisplay,
                             TSwtPeer       aPeer,
                             MSwtComposite& aParent,
                             TInt           aStyle)
        : ASwtControlBase(aDisplay, aPeer, &aParent, aStyle)
        , iFormat(MSwtHyperLink::EInvalid)
{
}


void CSwtHyperLink::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();

    SetContainerWindowL(coeParent);
    CAknControl::MakeVisible(coeParent.IsVisible());
    CAknControl::SetDimmed(coeParent.IsDimmed());

    UpdateDefaultFontL();
    iFormattedText.CreateL(KNullDesC);
    iOriginalText.CreateL(KNullDesC);
    UpdateSkinColor();
    SetBackground(this);   // Back will be drawn by ASwtControlBase::Draw

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif // RD_TACTILE_FEEDBACK

    ActivateL();
}


void CSwtHyperLink::SwtHandleResourceChangeL(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        UpdateDefaultFontL();
    }
    else if (aType == KAknsMessageSkinChange)
    {
        if (!iCustomTextColor)
        {
            UpdateSkinColor();
        }
    }
}


void CSwtHyperLink::DrawText(CWindowGc& aGc,
                             const CFont* aFont,
                             const TDesC& aText,
                             const TRect& aTextRect,
                             const CGraphicsContext::TTextAlign& aAlignment) const
{
    if (!aFont)
    {
        return;
    }

    // Vertically center the text inside the text rect
    const TInt excessSpace = aTextRect.Height() - aFont->FontMaxHeight();
    const TInt baseLineY = (excessSpace / 2) + aFont->FontMaxAscent();

    TPtrC textToDraw(aText.Ptr(), aText.Length());
    RBuf convertedText;

    TInt error = convertedText.Create(aText.Length() + KAknBidiExtraSpacePerLine);
    if (error == KErrNone)
    {
        TInt availableWidth(aTextRect.Width());
        TBool clipped = AknBidiTextUtils:: ConvertToVisualAndClip(
                            aText,
                            convertedText,
                            *aFont,
                            availableWidth,
                            availableWidth);

        textToDraw.Set(convertedText.Ptr(), convertedText.Length());
    }

    CFont::TMeasureTextInput::TFlags order = CFont::TMeasureTextInput::EFVisualOrder;
    if (AknLayoutUtils::LayoutMirrored())
    {
        order = CFont::TMeasureTextInput::EFVisualOrderRightToLeft;
    }

    TInt textWidth = AknBidiTextUtils::MeasureTextBoundsWidth(*aFont,
                     textToDraw,
                     order);
    TInt alignmentSpace = aTextRect.Width() - textWidth;
    TPoint textLocation(aTextRect.iTl.iX, aTextRect.iTl.iY + baseLineY);
    if (aAlignment == CGraphicsContext::ECenter)
    {
        textLocation.iX += (alignmentSpace / 2);
    }
    else if (aAlignment == CGraphicsContext::ERight)
    {
        textLocation.iX += alignmentSpace;
    }

    TRgb textColor = iLinkColor;
    // Same background highlight as that of Link
    if (iPressed || (iDisplay.UiUtils().NaviKeyInput() && IsFocused()))
    {
        textColor = iHighlightedLinkColor;
        aGc.SetPenStyle(CGraphicsContext::ENullPen);
        aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        aGc.SetBrushColor(iHighlightColor);
        aGc.DrawRect(TRect(TPoint(textLocation.iX, textLocation.iY - aFont->FontMaxAscent()),
                           TSize(textWidth, aFont->FontLineGap())));
    }

    aGc.UseFont(aFont);
    aGc.SetPenStyle(CGraphicsContext::ESolidPen);
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
    aGc.SetUnderlineStyle(EUnderlineOn);
    aGc.SetPenColor(textColor);

    aGc.DrawText(textToDraw, textLocation);
    convertedText.Close();
}


void CSwtHyperLink::UpdateSkinColor()
{
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              iLinkColor,
                              KAknsIIDQsnHighlightColors,
                              EAknsCIQsnHighlightColorsCG3);
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              iHighlightColor,
                              KAknsIIDQsnHighlightColors,
                              EAknsCIQsnHighlightColorsCG2);
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                              iHighlightedLinkColor,
                              KAknsIIDQsnTextColors,
                              EAknsCIQsnTextColorsCG24);
}


void CSwtHyperLink::UpdateDefaultFontL()
{
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }
    iDefaultFont = CSwtFont::NewL(iDisplay.Device(), iDisplay.Device().GetSystemFont()->Font());
}


void CSwtHyperLink::LaunchHandler()
{
    TPtrC scheme;
    scheme.Set(Scheme());

    RBuf textForHandler;
    TInt err = textForHandler.Create(scheme.Length() + iFormattedText.Length());
    if (err != KErrNone)
    {
        ASSERT(EFalse);
        return;
    }
    textForHandler.Append(scheme);
    textForHandler.Append(iFormattedText);

    CSwtBrowserSchemeHandler* handler = NULL;
    TRAP(err, handler = CSwtBrowserSchemeHandler::NewL(textForHandler));
    if (err != KErrNone)
    {
        ASSERT(EFalse);
    }
    else
    {
        TRAP(err, handler->HandleUrlStandaloneL());
        if (err != KErrNone)
        {
            ASSERT(EFalse);
        }
        delete handler;
        handler = NULL;
    }
    textForHandler.Close();
}


TPtrC CSwtHyperLink::Scheme()
{
    TPtrC scheme;
    switch (iFormat)
    {
    case MSwtHyperLink::EPhone:
        scheme.Set(KSwtHyperLinkTel);
        break;
    case MSwtHyperLink::EUrl:
        scheme.Set(KSwtHyperLinkHttp);
        break;
    case MSwtHyperLink::EEmail:
        scheme.Set(KSwtHyperLinkMail);
        break;
    default:
        ASSERT(EFalse);
        break;
    }
    return scheme;
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtHyperLink::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode       aType)
{
    TBool traversalDoIt(ETrue);
    if (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
    {
        traversalDoIt = EFalse;
    }
    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::HandleResourceChange(TInt aType)
{
    CAknControl::HandleResourceChange(aType);
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtHyperLink::MopSupplyObject(TTypeUid aId)
{
    return ASwtControlBase::SwtMopSupplyObject(aId);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::SizeChanged()
{
    HandleSizeChanged();
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::PositionChanged()
{
    HandlePositionChanged();
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::FocusChanged(TDrawNow aDrawNow)
{
    HandleFocusChanged(aDrawNow);
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::Draw(const TRect& /*aRect*/) const
{
    CGraphicsContext::TTextAlign alignment;
    if (iStyle & KSwtStyleCenter)
    {
        alignment = CGraphicsContext::ECenter;
    }
    else if (iStyle & KSwtStyleTrail)
    {
        if (AknLayoutUtils::LayoutMirrored())
        {
            alignment = CGraphicsContext::ELeft;
        }
        else
        {
            alignment = CGraphicsContext::ERight;
        }
    }
    else // default is left
    {
        if (AknLayoutUtils::LayoutMirrored())
        {
            alignment = CGraphicsContext::ERight;
        }
        else
        {
            alignment = CGraphicsContext::ELeft;
        }
    }

    TRect textRect(Rect());
    TMargins8 padding = iDisplay.UiUtils().InlineReferencePadding();

    textRect.iTl.iX += padding.iLeft;
    textRect.iTl.iY += padding.iTop;
    textRect.iBr.iX -= padding.iRight;
    textRect.iBr.iY -= padding.iBottom;

    DrawText(SystemGc(),
             &GetFont()->Font(),
             iFormattedText,
             textRect,
             alignment);
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtHyperLink::CoeControl()
{
    return *this;
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtHyperLink::CoeControl() const
{
    return *this;
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::ProcessKeyEventL(const TKeyEvent& aKeyEvent,
                                     TEventCode       aType)
{
    if (aType == EEventKey &&
            (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter))
    {
        LaunchHandler();
    }
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EButton1Down:
    {
        iPressed = ETrue;
#ifdef RD_TACTILE_FEEDBACK
        if (iFeedback)
        {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            iFeedback->InstantFeedback(ETouchFeedbackSensitiveButton);
#else
            iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        }
#endif //RD_TACTILE_FEEDBACK
        Redraw();
        break;
    }

    case TPointerEvent::EDrag:
    {
        TBool pressed = iPressed;
        if (Rect().Contains(aPointerEvent.iPosition))
        {
            iPressed = ETrue;
        }
        else
        {
            iPressed = EFalse;
        }
        if (pressed != iPressed)
        {
            Redraw();
        }
        break;
    }

    case TPointerEvent::EButton1Up:
    {
        TBool pressed = iPressed;
        iPressed = EFalse;
        if (!iDisplay.RevertPointerEvent() && Rect().Contains(aPointerEvent.iPosition))
        {
            LaunchHandler();
        }
        if (pressed != iPressed)
        {
            Redraw();
        }
        break;
    }

    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::SetForegroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetForegroundL(aColor);
    aColor ? iCustomTextColor = ETrue : iCustomTextColor = EFalse;
    if (iCustomTextColor)
    {
        TRgb rgb;
        TBool overrideColorSet(GetColor(EColorControlText, rgb));
        ASSERT(overrideColorSet);
        iLinkColor = rgb;
    }
    else
    {
        UpdateSkinColor();
    }
    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
TSize CSwtHyperLink::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    const MSwtFont* font = GetFont();
    ASSERT(font);
    TSize res(font->TextWidthInPixels(iFormattedText), font->Font().FontMaxHeight());
    if (font == &DefaultFont())
    {
        res.iHeight = Max(iDisplay.UiUtils().InlineReferenceFontHeight(), res.iHeight);
    }

    res += iDisplay.UiUtils().InlineReferencePadding().SizeDelta();
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
// From class ASwtControlBase.
// ---------------------------------------------------------------------------
//
const MSwtFont& CSwtHyperLink::DefaultFont() const
{
    return *iDefaultFont;
}


// ---------------------------------------------------------------------------
// From class ASwtControlBase.
// ---------------------------------------------------------------------------
//
HBufC* CSwtHyperLink::MSKLabelL() const
{
    return iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
}


// ---------------------------------------------------------------------------
// From class ASwtControlBase.
// ---------------------------------------------------------------------------
//
TBool CSwtHyperLink::IsKeyUsed(TUint aKeyCode) const
{
    TBool keyUsed(EFalse);
    if (aKeyCode == EKeyOK || aKeyCode == EKeyEnter)
    {
        keyUsed = ETrue;
    }
    return keyUsed;
}


// ---------------------------------------------------------------------------
// From class MSwtHyperLink.
// Note that this function is called already before SetFormat has been
// called.
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtHyperLink::Control()
{
    return this;
}


// ---------------------------------------------------------------------------
// From class MSwtHyperLink.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::SetTextL(const TDesC& aText)
{
    ASSERT(iFormat != MSwtHyperLink::EInvalid);

    iFormattedText.Close();
    iOriginalText.Close();
    iFormattedText.CreateL(aText);
    iOriginalText.CreateL(aText);

    iFormattedText.TrimAll();
    iFormattedText.LowerCase();

    TInt pos(KErrNotFound);
    TPtrC scheme;
    scheme.Set(Scheme());
    pos = iFormattedText.Find(scheme);

    TBool schemeAlreadyValid(pos == 0);
    if (schemeAlreadyValid)
    {
        RBuf tmp;
        tmp.CreateL(iFormattedText.Length() - scheme.Length());
        tmp.CleanupClosePushL();
        tmp.Append(iFormattedText.Mid(scheme.Length()));
        iFormattedText.Close();
        iFormattedText.CreateL(tmp);
        CleanupStack::PopAndDestroy(&tmp);
        iFormattedText.Trim();
    }

    Redraw();
}


// ---------------------------------------------------------------------------
// From class MSwtHyperLink.
// ---------------------------------------------------------------------------
//
const TDesC* CSwtHyperLink::GetText()
{
    ASSERT(iFormat != MSwtHyperLink::EInvalid);
    return &iOriginalText;
}


// ---------------------------------------------------------------------------
// From class MSwtHyperLink.
// This is called right after the ConstructL via another JNI call.
// This is called only once.
// ---------------------------------------------------------------------------
//
void CSwtHyperLink::SetFormat(TInt aFormat)
{
    ASSERT(iFormat == MSwtHyperLink::EInvalid);

    // Only these formats are recognised by this implementation.
    ASSERT(aFormat == MSwtHyperLink::EUrl ||
           aFormat == MSwtHyperLink::EEmail ||
           aFormat == MSwtHyperLink::EPhone);
    iFormat = static_cast<MSwtHyperLink::TFormat>(aFormat);
}
