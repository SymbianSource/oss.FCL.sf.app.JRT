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


#include <eikedwin.h>
#include <txtglobl.h>
#include <EIKCOCTL.rsg>
#include <AknsUtils.h>
#include <swtlaffacade.h>
#include <AknUtils.h>
#include <AknDef.h>
#include "swtscrollbar.h"
#include "swtfont.h"
#include "swttextbase.h"
#include "swtcaptionedcontrol.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <uikon/eikdefconst.h>
#endif

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtTextBase::CSwtTextBase
// ---------------------------------------------------------------------------
//
CSwtTextBase::CSwtTextBase(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
        : ASwtScrollableBase(aDisplay, aPeer, &aParent, aStyle)
{
}

// ---------------------------------------------------------------------------
// CSwtTextBase::~CSwtTextBase
// ---------------------------------------------------------------------------
//
CSwtTextBase::~CSwtTextBase()
{
    iEikonEnv->SyncNotifyFocusObserversOfChangeInFocus();
    // Own
    delete iEditor;
    iEditor = NULL;
    delete iIndicator;
    iIndicator = NULL;

    // Not own.
    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }

    if (iCharFormatLayer)
    {
        delete iCharFormatLayer;
        iCharFormatLayer = NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtTextBase::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtTextBase::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);
    CSwtEditIndicatorRedictor* redirector = MopGetObject(redirector);
    if (redirector)
    {
        iIndicator = CSwtEditIndicator::NewL(CoeControl());
    }
    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());
    SetComponentsToInheritVisibility(ETrue);

    // Although the background is drawn already natively, this fixes
    // problems with srollbar back when custom back color set.
    SetBackground(this);
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetEditor
// ---------------------------------------------------------------------------
//
void CSwtTextBase::SetEditor(CEikEdwin* aEditor)
{
    ASSERT(!iEditor);
    iEditor = aEditor;
}

#if defined( RD_SCALABLE_UI_V2)

// ---------------------------------------------------------------------------
// CSwtTextBase::InputCapabilities
// ---------------------------------------------------------------------------
//
TCoeInputCapabilities CSwtTextBase::InputCapabilities() const
{
    // Override CCoeControl behaviour to provide MCoeCaptionRetrieverForFep.
    const TCoeInputCapabilities& capabilities = CAknControl::InputCapabilities();

    return TCoeInputCapabilities(capabilities.Capabilities(),
                                 capabilities.FepAwareTextEditor(),
                                 (MCoeCaptionRetrieverForFep*)this);
}

// ---------------------------------------------------------------------------
// CSwtTextBase::GetCaptionForFep
// ---------------------------------------------------------------------------
//
void CSwtTextBase::GetCaptionForFep(TDes& aCaption) const
{
    // If we are inside a CaptionedControl, return the CaptionedControl text.
    // It will be shown in thumb keypad
    MSwtCaptionedControl* captionedControl = iParent->Control()->CaptionedControlInterface();

    if (captionedControl)
    {
        const TDesC* text = captionedControl->GetText();

        if (!text)
        {
            aCaption = KNullDesC;
        }
        else
        {
            const TInt maximumLength = aCaption.MaxLength();

            if (text->Length() > maximumLength)
            {
                aCaption = text->Left(maximumLength);
            }
            else
            {
                aCaption = *text;
            }
        }
    }
    else
    {
        aCaption=KNullDesC;
    }
}

void CSwtTextBase::MCoeCaptionRetrieverForFep_Reserved_1()
{}

void CSwtTextBase::MCoeCaptionRetrieverForFep_Reserved_2()
{}

#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtTextBase::RetrieveDefaultFontL
// ---------------------------------------------------------------------------
//
void CSwtTextBase::RetrieveDefaultFontL()
{
    ASSERT(iEditor);

    if (iDefaultFont)
    {
        iDefaultFont->RemoveRef();
        iDefaultFont = NULL;
    }

    // Not same but similar to DateEditor's font. Height most probably be same.
    iDefaultFont = CSwtFont::NewL(iDisplay.Device(), *AknLayoutUtils::FontFromId(
                                      CSwtLafFacade::GetFontId(CSwtLafFacade::EFont2MidpTextPaneT1Font, 0, 0), NULL));

    if (!iFont)
    {
        // to avoid the default font of CEikEdwin
        ProcessFontUpdateL();
    }
}


// ---------------------------------------------------------------------------
// CSwtTextBase::ProcessFontUpdate
// ---------------------------------------------------------------------------
//
void CSwtTextBase::ProcessFontUpdateL()
{
    TCharFormat charFormat;
    TCharFormatMask charMask;

    charFormat.iFontSpec = GetFont()->Font().FontSpecInTwips();
    charMask.SetAttrib(EAttFontTypeface);
    charMask.SetAttrib(EAttFontHeight);

    if (charFormat.iFontSpec.iFontStyle.StrokeWeight() == EStrokeWeightBold)
    {
        charMask.SetAttrib(EAttFontStrokeWeight);
    }

    if (charFormat.iFontSpec.iFontStyle.Posture() == EPostureItalic)
    {
        charMask.SetAttrib(EAttFontPosture);
    }

    // when we set the new font color also changes. So we have to set the color again.
    if (!iTextColor)
    {
        TRgb color;
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), color,
                                  KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
        charFormat.iFontPresentation.iTextColor = color;
        charMask.SetAttrib(EAttColor);
    }
    else
    {
        charFormat.iFontPresentation.iTextColor = GetForeground();
        charMask.SetAttrib(EAttColor);
    }

    if (iStyle & KSwtStyleSingle)
    {
        CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL(charFormat, charMask);
        // we are using CGlobalText for Single line. So SetGlobalCharFormat for Global text
        static_cast<CGlobalText*>(iEditor->Text())->SetGlobalCharFormat(charFormatLayer);
        if (iCharFormatLayer)
        {
            delete iCharFormatLayer;
        }
        iCharFormatLayer = charFormatLayer;
    }
    else
    {
        CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL(charFormat, charMask);
        // SetCharFormatLayer is for EText and RichText
        iEditor->SetCharFormatLayer(charFormatLayer);
    }
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetBackgroundL
// ---------------------------------------------------------------------------
//
void CSwtTextBase::SetBackgroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetBackgroundL(aColor);
    TRgb color(aColor ? aColor->RgbValue() :
               iDisplay.CoeEnv()->Color(EColorControlBackground));
    if (iEditor)
    {
        // Outside text view
        iEditor->SetBackgroundColorL(color);

        // Inside text view, outside paragraphs
        iEditor->TextView()->SetBackgroundColor(color);

        // Inside paragraphs
        CParaFormat* paraFormat = new(ELeave) CParaFormat();
        CleanupStack::PushL(paraFormat);
        paraFormat->iFillColor = color;
        TParaFormatMask paraMask;
        paraMask.SetAttrib(EAttFillColor);
        static_cast<CGlobalText*>(iEditor->Text())->ApplyParaFormatL(
            paraFormat, paraMask, 0, iEditor->TextLength());
        CleanupStack::PopAndDestroy(paraFormat);
    }
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetForegroundL
// ---------------------------------------------------------------------------
//
void CSwtTextBase::SetForegroundL(const MSwtColor* aColor)
{
    ASwtControlBase::DoSetForegroundL(aColor);
    TRgb color(aColor ? aColor->RgbValue() :
               iDisplay.CoeEnv()->Color(EColorControlText));
    if (iEditor)
    {
        TCharFormat charFormat;
        TCharFormatMask charMask;
        charFormat.iFontPresentation.iTextColor=color;
        charMask.SetAttrib(EAttColor);
        static_cast<CGlobalText*>(iEditor->Text())->
        ApplyCharFormatL(charFormat, charMask, 0, iEditor->TextLength());
        iTextColor = ETrue;
    }
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtTextBase::ComputeDelta
// ---------------------------------------------------------------------------
//
TSize CSwtTextBase::ComputeDelta() const
{
    ASSERT(iEditor);

    // Retrieve the various margins & borders
    TInt labels = 0;
    TInt cursor = 0;
    iEditor->TextView()->MarginWidths(labels, cursor);
    const TSize borderDelta(iEditor->Border().SizeDelta());
    const TSize marginDelta(iEditor->Margins().SizeDelta());
    const TSize ownBorderDelta(BorderSizeDelta());

    // The scrollbars
    const TSize sbDelta(SbOuterRect(TRect()).Size());

    // Add them all up
    // Note that marginDelta.iWidth is counted twice because CEikEdwin also
    // counts it twice ( in LayoutWidth(), margins are substracted from
    // ViewRect().Width() whereas this is already done when computing
    // ViewRect().Width() ). So this is a workaround to avoid unwanted
    // wrapping because of this defect in CEikEdwin.
    TSize totalDelta(ownBorderDelta.iWidth + borderDelta.iWidth
                     + 2 * marginDelta.iWidth  + sbDelta.iWidth
                     + labels + cursor + KEikDefaultCursorWidth,
                     ownBorderDelta.iHeight + borderDelta.iHeight
                     + marginDelta.iHeight + sbDelta.iHeight);
    if (iIndicator)
    {
        totalDelta.iHeight += KSWTEditIndicatorHeight;
    }
    return totalDelta;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::HotKeyL
// ---------------------------------------------------------------------------
//
TInt CSwtTextBase::HotKeyL(const TKeyEvent& aKeyEvent) const
{
    ASSERT(iEditor);

    if (!(aKeyEvent.iModifiers & EModifierCtrl)
            || (aKeyEvent.iCode >= 100)
            || (aKeyEvent.iCode == ' '))
    {
        return -1;
    }

    HBufC* hkBuf;
    if (aKeyEvent.iModifiers & EModifierShift)
    {
        hkBuf = iCoeEnv->AllocReadResourceL(R_EIK_EDWIN_SHIFT_CTRL_HOTKEYS);
    }
    else
    {
        hkBuf = iCoeEnv->AllocReadResourceL(R_EIK_EDWIN_CTRL_HOTKEYS);
    }
    TInt hotkey = hkBuf->Locate(TChar(aKeyEvent.iCode + 'a' - 1));
    delete hkBuf;
    return hotkey;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetEditable
// ---------------------------------------------------------------------------
//
void CSwtTextBase::SetEditable(TBool aEditable)
{
    // Update the style accordingly
    if (aEditable)
    {
        iStyle &= ~KSwtStyleReadOnly;
    }
    else
    {
        iStyle |= KSwtStyleReadOnly;
    }

    iEditor->SetReadOnly(!aEditable);
    iEditor->SetFocus(IsFocused());
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetTextL
// ---------------------------------------------------------------------------
//
TBool CSwtTextBase::SetTextL(const TDesC& aText)
{
    // This must be called before querying the length or selection as it can
    // remove the char that is just being typed.
    iEditor->CancelFepTransaction();

    // Get the remaining editing space available in editor if text limit set,
    // and return if no available space.
    TInt editorTextLen = iEditor->TextLength();
    TInt availableLen = KErrNotFound;
    if (iTextLimit)
    {
        ASSERT(editorTextLen <= iTextLimit);
        availableLen = iTextLimit;
        if (availableLen <= 0)
        {
            return EFalse;
        }
    }


    // Clear selection if any
    iEditor->ClearSelectionL();

    // If a text limit is set, append as much as the available space permits,
    // otherwise append the entire text.
    TInt textToInsertLen = aText.Length();
    if (availableLen != KErrNotFound
            && textToInsertLen > availableLen)
    {
        TPtrC des = aText.Left(availableLen);
        iEditor->SetTextL(&des);
    }
    else
    {
        iEditor->SetTextL(&aText);
    }
    iEditor->HandleTextChangedL();


    // Restore carret
    iEditor->SetSelectionL(0, 0);

    /* Workaround for junit tests. Copy and paste functions crash, when
     * Text is not visible. There is some optimalization in CEikEdwin,
     * that updates selection (and cursos position) after Text is drawn.
     */
    iEditor->TextView()->SetSelectionL(TCursorSelection(0, 0));

    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::AppendL
// ---------------------------------------------------------------------------
//
TBool CSwtTextBase::AppendL(const TDesC& aText)
{
    ASSERT(iEditor);

    // This must be called before querying the length or selection as it can
    // remove the char that is just being typed.
    iEditor->CancelFepTransaction();

    // Get the remaining editing space available in editor if text limit set,
    // and return if no available space.
    TInt textToInsertLen = aText.Length();
    TInt editorTextLen = iEditor->TextLength();
    TInt availableLen = KErrNotFound;
    if (iTextLimit)
    {
        ASSERT(editorTextLen <= iTextLimit);
        availableLen = iTextLimit - editorTextLen;
        if (availableLen <= 0)
        {
            return EFalse;
        }
    }

    // Clear selection if any
    iEditor->ClearSelectionL();

    // If a text limit is set, append as much as the available space permits,
    // otherwise append the entire text.
    CPlainText* plainText = iEditor->Text();
    if (availableLen != KErrNotFound
            && textToInsertLen > availableLen)
    {
        plainText->InsertL(editorTextLen, aText.Left(availableLen));
    }
    else
    {
        plainText->InsertL(editorTextLen, aText);
    }
    iEditor->HandleTextChangedL();

    // Restore carret
    editorTextLen = iEditor->TextLength();
    iEditor->SetSelectionL(editorTextLen, editorTextLen);

    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::InsertL
// ---------------------------------------------------------------------------
//
TBool CSwtTextBase::InsertL(const TDesC& aText)
{
    ASSERT(iEditor);

    // This must be called before querying the length or selection as it can
    // remove the char that is just being typed.
    iEditor->CancelFepTransaction();

    const TCursorSelection selection(iEditor->Selection());
    const TInt selStart = selection.LowerPos();
    TInt textToInsertLen = aText.Length();

    // Return if there is nothing to delete
    if (textToInsertLen == 0 && selection.Length() == 0)
    {
        return EFalse;
    }

    iEditor->ClearSelectionL();
    // Update selection to be sure selection is valid after text removal
    iEditor->SetSelectionL(selStart, selStart);
    iEditor->TextView()->SetSelectionL(TCursorSelection(selStart, selStart));

    // Remove selected text
    CPlainText* plainText = iEditor->Text();
    plainText->DeleteL(selStart, selection.Length());

    // Get the remaining editing space available in editor if text limit set,
    // and return if no available space.
    TInt editorTextLen = iEditor->TextLength();
    TInt availableLen = KErrNotFound;
    if (iTextLimit)
    {
        ASSERT(editorTextLen <= iTextLimit);
        availableLen = iTextLimit - editorTextLen;
        if (availableLen <= 0)
        {
            return EFalse;
        }
    }

    // If a text limit is set, append as much as the available space permits,
    // otherwise insert the entire text.
    if (availableLen != KErrNotFound
            && textToInsertLen > availableLen)
    {
        plainText->InsertL(selStart, aText.Left(availableLen));
        textToInsertLen = availableLen;
    }
    else
    {
        plainText->InsertL(selStart, aText);
    }

    iEditor->HandleTextChangedL();

    // Restore carret
    TInt newCaretPos = Min(selStart + textToInsertLen,
                           iEditor->TextLength());
    iEditor->SetSelectionL(newCaretPos, newCaretPos);

    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::CutL
// ---------------------------------------------------------------------------
//
TBool CSwtTextBase::CutL()
{
    ASSERT(iEditor);

    if (iEditor->Selection().Length() == 0)
    {
        return EFalse;
    }
    iEditor->ClipboardL(CEikEdwin::ECut);
    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetLimit
// ---------------------------------------------------------------------------
//
TBool CSwtTextBase::SetLimitL(TInt aLimit)
{
    ASSERT(iEditor);
    ASSERT(aLimit);

    const TCursorSelection selection(iEditor->Selection());
    const TInt selStart = selection.LowerPos();

    // This should be enough but it's not.
    // iEditor->SetTextLimit( aLimit );
    iTextLimit = aLimit;

    // Clear selection.
    iEditor->ClearSelectionL();

    // Ensure that the editor's text is not longer than the limit just set.
    TInt editorTextLen = iEditor->TextLength();
    if (editorTextLen > iTextLimit)
    {
        CPlainText* plainText = iEditor->Text();
        plainText->DeleteL(iTextLimit, editorTextLen - iTextLimit);
        iEditor->HandleTextChangedL();
    }
    iEditor->SetTextLimit(aLimit);
    // Not allowed to have failed here.
    editorTextLen = iEditor->TextLength();
    ASSERT(editorTextLen <= iTextLimit);

    // Ensure cursor is at the most at the end of the new text.
    if (selStart > editorTextLen)
    {
        iEditor->SetSelectionL(editorTextLen, editorTextLen);
    }

    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::ForwardKeyEventL
// NOTE: FEP may deliver the key events directly to CEikEdwin!
// ---------------------------------------------------------------------------
//
void CSwtTextBase::ForwardKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
{
    ASSERT(iEditor);
    TInt cursorPos = Editor().TextView()->Selection().iCursorPos;
    TInt textLen = Editor().TextLength();

    iEditor->OfferKeyEventL(aKeyEvent, aType);

    // This is called, because when FepManager is notified first time,
    // there are no changes in editor yet. Editor is updated later,
    // after verification on java side. So now FepManager must be notified
    // again to get a right information about changes.
    iDisplay.CoeEnv()->InputCapabilitiesChanged();

    // CEikEdwin doesn't loop when cursor is end or begining of Text
    //if edwin is readonly. this is workaround to fix.
    if ((iStyle & KSwtStyleReadOnly))
    {
        switch (aKeyEvent.iCode)
        {
        case EKeyLeftArrow:
            if (!(aKeyEvent.iModifiers & EModifierShift)
                    && (cursorPos == 0))
            {
                Editor().SetSelectionL(textLen , textLen);
            }
            break;

        case EKeyRightArrow:
            if (!(aKeyEvent.iModifiers & EModifierShift)
                    && (cursorPos == textLen))
            {
                Editor().SetSelectionL(0, 0);
            }
            break;

        default:
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtTextBase::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtTextBase::CountComponentControls() const
{
    TInt componentCount = 0;
    if (iEditor)
    {
        componentCount++;
        if (iIndicator)
        {
            componentCount++;
        }
    }
    return componentCount;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtTextBase::ComponentControl(TInt aIndex) const
{
    ASSERT(iEditor);
    if (aIndex == 1 && iIndicator)
    {
        return iIndicator->IndicatorContainer();
    }
    return iEditor;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::MakeVisible
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTextBase::MakeVisible(TBool aVisible)
{
    const TBool oldVisibility = IsVisible();
    if ((aVisible && oldVisibility) || (!aVisible && !oldVisibility))
    {
        return;
    }

    CCoeControl::MakeVisible(aVisible);
    if (iEditor)
    {
        iEditor->MakeVisible(aVisible);

        // Forward the new visibility to the scroll-bars
        CEikScrollBarFrame* sbFrame = iEditor->ScrollBarFrame();
        if (sbFrame)
        {
            //Update scrollbars
            TRAP_IGNORE(iEditor->UpdateScrollBarsL());
            const TInt count = sbFrame->CountComponentControls();
            for (TInt i = 0; i < count; ++i)
            {
                sbFrame->ComponentControl(i)->MakeVisible(aVisible);
            }
        }
    }
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetDimmed
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTextBase::SetDimmed(TBool aDimmed)
{
    CCoeControl::SetDimmed(aDimmed);
    if (iEditor)
    {
        iEditor->SetDimmed(aDimmed);
    }
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// CSwtTextBase::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTextBase::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SwtHandleResourceChangeL
// ---------------------------------------------------------------------------
//
void CSwtTextBase::SwtHandleResourceChangeL(TInt aType)
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
            ProcessFontUpdateL();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtTextBase::FixPadding
// ---------------------------------------------------------------------------
//
void CSwtTextBase::FixPadding()
{
    Editor().SetBorderViewMargins(iDisplay.UiUtils().InlineReferencePadding());
}

// ---------------------------------------------------------------------------
// CSwtTextBase::HandlePointerEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtTextBase::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    // Scrollbar tap
    CEikScrollBarFrame* sbFrame = iEditor->ScrollBarFrame();
    CEikScrollBar* vsb = sbFrame ? sbFrame->GetScrollBarHandle(CEikScrollBar::EVertical) : NULL;

    TBool hit = ETrue;

#ifdef RD_JAVA_S60_RELEASE_9_2
    hit = Rect().Contains(aPointerEvent.iPosition);
    TBool pressed = iPressed;
#endif

    // Check if we should start scrollbar grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        if (vsb && vsb->Rect().Contains(aPointerEvent.iPosition))
        {
            iVScrollBarGrabsPointerEvents = ETrue;
        }
#ifdef RD_JAVA_S60_RELEASE_9_2
        else
        {
            iPressed = ETrue;
        }
#endif
    }

    // Deliver event to scrollbar
    if (iVScrollBarGrabsPointerEvents && vsb)
    {
        vsb->HandlePointerEventL(aPointerEvent);
    }

    // Edwin tap. Button1Up after the long tap is not forwarded to the editor
    if (!(aPointerEvent.iType == TPointerEvent::EButton1Up
            && (iDisplay.RevertPointerEvent() || !hit))
            && !iVScrollBarGrabsPointerEvents
            && !iIgnorePointerDown)
    {
        TPointerEvent pointerEvent(aPointerEvent);

        // If clicking on margins, move the pointer event.
        TRect viewRect(iEditor->TextView()->ViewRect());
        TRect viewRectWithoutMargins(viewRect);
        viewRectWithoutMargins.Shrink(2, 2);    // well inside the edges

        // EButtonRepeat and EDrag events should not be moved to viewRect
        // This enables to scroll text, when dragging outside of the text
        const TBool moveEventToViewRect =
            aPointerEvent.iType != TPointerEvent::EButtonRepeat
            && aPointerEvent.iType != TPointerEvent::EDrag;

        // Checking if event is in area between margins and borders
        // When so then moving event to the rect without margins
        if (moveEventToViewRect ||
                (viewRect.Contains(aPointerEvent.iPosition) &&
                 !viewRectWithoutMargins.Contains(aPointerEvent.iPosition)))
        {
            if (pointerEvent.iPosition.iX < viewRectWithoutMargins.iTl.iX)
            {
                pointerEvent.iPosition.iX = viewRectWithoutMargins.iTl.iX;
            }
            if (pointerEvent.iPosition.iY < viewRectWithoutMargins.iTl.iY)
            {
                pointerEvent.iPosition.iY = viewRectWithoutMargins.iTl.iY;
            }
            if (pointerEvent.iPosition.iX > viewRectWithoutMargins.iBr.iX)
            {
                pointerEvent.iPosition.iX = viewRectWithoutMargins.iBr.iX;
            }
            if (pointerEvent.iPosition.iY > viewRectWithoutMargins.iBr.iY)
            {
                pointerEvent.iPosition.iY = viewRectWithoutMargins.iBr.iY;
            }
        }

        iEditor->HandlePointerEventL(pointerEvent);
    }

    // Stop scrollbar grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        iVScrollBarGrabsPointerEvents = EFalse;
#ifdef RD_JAVA_S60_RELEASE_9_2
        iPressed = EFalse;
#endif
    }
#ifdef RD_JAVA_S60_RELEASE_9_2
    else if (aPointerEvent.iType == TPointerEvent::EDrag)
    {
        iPressed = hit;
    }

    if (pressed != iPressed)
        Redraw();
#endif

#ifndef RD_JAVA_S60_RELEASE_9_2
    // We got a pointer event, so any subsequent events should not be ignored.
    iIgnorePointerDown = EFalse;
#endif // RD_JAVA_S60_RELEASE_9_2
}
#else //RD_SCALABLE_UI_V2
void CSwtTextBase::HandlePointerEventL(
    const TPointerEvent& /*aPointerEvent*/)
{
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// CSwtTextBase::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTextBase::FocusChanged(TDrawNow aDrawNow)
{
    TBool isFocused = IsFocused();

    if (iEditor)
    {
        TRAP_IGNORE(iEditor->UpdateScrollBarsL());
    }

    if (!isFocused)
    {
        if (iEditor)
        {
            iEditor->SetFocus(IsFocused());

            if (iIndicator)
            {
                iIndicator->SetState(EStateNone);
            }
        }
    }

    HandleFocusChanged(aDrawNow);

    if (isFocused)
    {
        if (iEditor)
        {
            iEditor->SetFocus(IsFocused());
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtTextBase::PositionChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTextBase::PositionChanged()
{
    if (!iEditor)
    {
        return;
    }

    // The size of scrollbars are set to ( 0,0 )
    // to be correctly updated in editor.
    CEikScrollBarFrame* sbFrame = SbFrame();
    if (sbFrame)
    {
        CEikScrollBar* verticalScrollBar = sbFrame->
                                           GetScrollBarHandle(CEikScrollBar::EVertical);
        if (verticalScrollBar)
        {
            verticalScrollBar->SetSize(TSize(0, 0));
        }
        CEikScrollBar* horizScrollBar = sbFrame->
                                        GetScrollBarHandle(CEikScrollBar::EHorizontal);
        if (horizScrollBar)
        {
            horizScrollBar->SetSize(TSize(0, 0));
        }
    }

    //Position indicator
    TRect editorRect(SbInnerRect(BorderInnerRect()));

    if (iIndicator)
    {
        const TPoint indPos(iPosition.iX + GetBorderWidth(),
                            iPosition.iY + GetBorderWidth());
        iIndicator->IndicatorContainer()->SetPosition(indPos);
        editorRect.SetRect(TPoint(editorRect.iTl.iX,
                                  indPos.iY + KSWTEditIndicatorHeight), editorRect.iBr);
    }

    // We have to call CEikEdwin::SetRect() to update the location
    // of the editor member on the screen.
    if (AknLayoutUtils::LayoutMirrored())
    {
        iEditor->SetAlignment(EAknEditorAlignRight);
        const TSize sbDelta(SbOuterRect(TRect()).Size());
        editorRect.iTl.iX += sbDelta.iWidth;
    }

    iEditor->SetRect(editorRect);
    HandlePositionChanged();
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTextBase::SizeChanged()
{
    if (!iEditor)
    {
        return;
    }
    const TSize sbDelta(SbOuterRect(TRect()).Size());
    const TSize ownBorderDelta(BorderSizeDelta());
    if (AknLayoutUtils::LayoutMirrored())
    {
        iEditor->SetAlignment(EAknEditorAlignRight);
    }
    if (iIndicator)
    {
        const TSize indicatorSize(
            BorderInnerRect().Size().iWidth,
            KSWTEditIndicatorHeight);
        iIndicator->IndicatorContainer()->SetSize(indicatorSize);
        TSize editorSize = iSize - sbDelta - ownBorderDelta;
        editorSize.iHeight -= indicatorSize.iHeight;
        iEditor->SetSize(editorSize);
    }
    else
    {
        iEditor->SetSize(iSize - sbDelta - ownBorderDelta);
    }

    if (HasBorderStyle())
    {
        iEditor->SetPosition(BorderInnerRect().iTl);
    }

    HandleSizeChanged();
}

// ---------------------------------------------------------------------------
// CSwtTextBase::MopSupplyObject
// From CCoeControl
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtTextBase::MopSupplyObject(TTypeUid aId)
{
    if (iIndicator && aId.iUid == MAknEditingStateIndicator::ETypeId)
    {
        return aId.MakePtr(static_cast<MAknEditingStateIndicator*>(iIndicator));
    }
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
// CSwtTextBase::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtTextBase::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::CoeControl
// From MSwtControl
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtTextBase::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetSwtFocus
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtTextBase::SetSwtFocus(TInt aReason /*=KSwtFocusByApi*/)
{
#ifndef RD_JAVA_S60_RELEASE_9_2
    iIgnorePointerDown = EFalse;
    if (IsFocusControl())
    {
        return ETrue;
    }
    iIgnorePointerDown = aReason == KSwtFocusByPointer;
#endif // RD_JAVA_S60_RELEASE_9_2
    return ASwtControlBase::SetSwtFocus(aReason);
}

// ---------------------------------------------------------------------------
// CSwtTextBase::FocusBackgroundPolicy
// From MSwtControl
// While background color is set, the editor's control context must be null,
// otherwise there will be ugly background drawing problems while typing.
// ---------------------------------------------------------------------------
//
TInt CSwtTextBase::FocusBackgroundPolicy() const
{
    if (!IsDefaultBackgroundUse())
    {
        return EEmbeddedFocusBackground;
    }
    else
    {
        return ASwtControlBase::FocusBackgroundPolicy();
    }
};

// ---------------------------------------------------------------------------
// CSwtTextBase::ClientRect
// From MSwtControl
// ---------------------------------------------------------------------------
//
TRect CSwtTextBase::ClientRect() const
{
    TRect rect(ASwtScrollableBase::ClientRect());
    if (iIndicator)
    {
        rect.iTl.iY = iIndicator->IndicatorContainer()->Rect().iBr.iY;
    }
    return rect;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::SetFontL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtTextBase::SetFontL(const MSwtFont* aFont)
{
    ASwtControlBase::DoSetFontL(aFont);

    // Set the font in the edwin
    if (iEditor)
    {
        ProcessFontUpdateL();
        iEditor->HandleTextChangedL();
    }

    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtTextBase::ComputeSizeL
// From MSwtControl
// ---------------------------------------------------------------------------
//
TSize CSwtTextBase::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    if (!iEditor)
    {
        // iEditor could be null if the CSwtTextBase is contained in a
        // CaptionedControl.
        return TSize(0, 0);
    }

    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return TSize(aWHint, aHHint);
    }

    const TSize delta(ComputeDelta());
    const TInt maxWidth = KMaxTInt - delta.iWidth;

    TSize prefSize(TSize::EUninitialized);

    CTextView*   textView = iEditor->TextView();
    CTextLayout* layout   = const_cast<CTextLayout*>(textView->Layout());

    if (aWHint == KSwtDefault)
    {
        layout->SetWrapWidth(maxWidth);
    }
    else
    {
        layout->SetWrapWidth(aWHint);
    }

    TViewYPosQualifier yPosQualifier;
    yPosQualifier.SetFillScreen();
    yPosQualifier.SetMakeLineFullyVisible();
    textView->HandleGlobalChangeNoRedrawL(yPosQualifier);
    TInt formatedHeight = layout->FormattedHeightInPixels();

    // if width is free, no effect for wraping. that is swt implementation
    if (aWHint == KSwtDefault)
    {
        layout->GetMinimumSizeL(maxWidth, prefSize);
    }
    else
    {
        // if width is fixed, height is free
        // if wrapping is on, the Height should be heights of all lines that all
        // characters fit in vertically ( note: VSrolling shouldn't be there )
        // if wrap is off, the Height should be heights of all lines and
        // HScrolling if any of  the line width is bigger than given width
        layout->GetMinimumSizeL(aWHint, prefSize);
        prefSize.iHeight = formatedHeight;
    }

    prefSize += delta;

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
// CSwtTextBase::IsKeyUsed
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtTextBase::IsKeyUsed(TUint aKeyCode) const
{
    // A text uses the Backspace and OK keys
    if (aKeyCode == EKeyBackspace)
    {
        return ETrue;
    }
    else if (aKeyCode == EKeyOK)
    {
        return EFalse;
    }
    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::CanDrawOutsideRect
// Needed for drawing the scrollbars in multi line texts!
// From MSwtControl
// ---------------------------------------------------------------------------
//
TBool CSwtTextBase::CanDrawOutsideRect() const
{
    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtTextBase::DefaultFont
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
const MSwtFont& CSwtTextBase::DefaultFont() const
{
    ASSERT(iDefaultFont);
    return *iDefaultFont;

}

// ---------------------------------------------------------------------------
// CSwtTextBase::SbFrame
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
CEikScrollBarFrame* CSwtTextBase::SbFrame() const
{
    return (iEditor!=NULL) ? iEditor->ScrollBarFrame() : NULL;
}

// ---------------------------------------------------------------------------
// CSwtListBase::HandleScrollEventL
// From MEikScrollBarObserver
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtTextBase::HandleScrollEventL(
    CEikScrollBar* aScrollBar,
    TEikScrollEvent aEventType)
{
    iEditor->HandleScrollEventL(aScrollBar, aEventType);
    ASwtScrollableBase::HandleScrollEventL(aScrollBar, aEventType);
}
#else //RD_SCALABLE_UI_V2
void CSwtTextBase::HandleScrollEventL(
    CEikScrollBar*,
    TEikScrollEvent)
{
}
#endif //RD_SCALABLE_UI_V2
