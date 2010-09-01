/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Defines a custom editor window for Text editor component.
*
*/


// INTERNAL INCLUDES
#include "CMIDTextEditorEdwin.h"
#include "CMIDEdwinUtils.h"
#include "CMIDTextEditorEdwinCustomDraw.h"
#include "CMIDEditingStateIndicator.h"
#include "CMIDDisplayable.h"

// EXTERNAL INCLUDES
#include <MMIDTextEditor.h> // MMIDTextEditorObserver
#include <aknextendedinputcapabilities.h>
#include <AknSettingCache.h>
#include <eikedwob.h>
#include <j2me/jdebug.h>

#ifdef RD_JAVA_S60_RELEASE_9_2
#include <AknPriv.hrh>
#endif // RD_JAVA_S60_RELEASE_9_2

const TInt KColorOpaque = 255;
const TInt KCharMinus   = 0x2d;

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::CMIDTextEditorEdwin
// ---------------------------------------------------------------------------
//
CMIDTextEditorEdwin::CMIDTextEditorEdwin(CMIDEdwinUtils& aEdwinUtils)
        : CEikEdwin(),
        iMultiline(EFalse),
        iCursorPosForAction(KErrNotFound),
        iEdwinUtils(aEdwinUtils)
{
    DEBUG("CMIDTextEditorEdwin::CMIDTextEditorEdwin +");

#ifdef RD_JAVA_S60_RELEASE_9_2
    iPartialVKBOpen = EFalse;
    iDisplayable = NULL;
    iJavaAppUi = NULL;
#endif // RD_JAVA_S60_RELEASE_9_2
    // Set margins to zero
    TMargins8 margins;
    margins.SetAllValuesTo(0);
    SetBorderViewMargins(margins);
    // Set layout
    CAknEnv::Static()->GetCurrentLayoutId(iDirection);
    // Set visible content height
    iVisibleContentHeight = 0;
    // Set border to zero
    SetBorder(TGulBorder::ENone);
    SetFocusing(ETrue);
    DEBUG("CMIDTextEditorEdwin::CMIDTextEditorEdwin -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::~CMIDTextEditorEdwin
// ---------------------------------------------------------------------------
//
CMIDTextEditorEdwin::~CMIDTextEditorEdwin()
{
    DEBUG("CMIDTextEditorEdwin::~CMIDTextEditorEdwin");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::ConstructL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::ConstructL(
    TInt aEdwinFlags,
    TInt aWidthInChars,
    TInt aTextLimit,
    TInt aNumberOfLines)
{
    DEBUG("CMIDTextEditorEdwin::ConstructL +");

    CEikEdwin::ConstructL(
        aEdwinFlags, aWidthInChars, aTextLimit, aNumberOfLines);

    // Supress all background drawing of CEikEdwin so that this editor
    // can draw the correct custom color to the background.
    SetSuppressBackgroundDrawing(ETrue);

    // No background control context. This way it is possible to change the
    // background color of the text editor. By default, it is white.
    SetSkinBackgroundControlContextL(NULL);

    // Observer events in the editor. Note that this must be done in order
    // to avoid resulting incorrect content in numeric and decimal editors
    SetEdwinObserver(this);

    // To have the line where cursor is properly displayed.
    if (iLayout)
    {
        iLayout->RestrictScrollToTopsOfLines(EFalse);
    }

    DEBUG("CMIDTextEditorEdwin::ConstructL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::OfferKeyEventL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TKeyResponse CMIDTextEditorEdwin::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    DEBUG_INT("CMIDTextEditorEdwin::OfferKeyEventL +, aType=%d", aType);

    TKeyResponse response = EKeyWasNotConsumed;

    if ((aType == EEventKey) &&
            (aKeyEvent.iCode == EKeyEnter) &&
            (!iMultiline ||
             iConstraints & MMIDTextField::EPassword ||
             !IsConstraintSet(MMIDTextField::EAny)))
    {
        // Enter key for single-line editor, password and other than any
        // editors is simply passed on.
        DEBUG("CMIDTextEditorEdwin::OfferKeyEventL -, ignoring enter");

        return response;
    }

    // Offer key event to editor window if it is set focused and visible.
    // If the editor is set hidden, OfferKeyEventL will update the content
    // in the editor and draw it. So the editor content CANNOT be modified
    // if it is not visible and focused.
    if (IsFocused() && IsVisible())
    {
        DEBUG("CMIDTextEditorEdwin::OfferKeyEventL, focused=true");

        // Edwin does not report cursor movements with text updates
        // Store the current position here for future use.
        iCursorPosForAction = CursorPos();

        // Try handling special characters.
        response = HandleSpecialKeyEventsL(aKeyEvent, aType);

        // Not handled, try with CEikEdwin
        // Consume down and up type of keyevents
        if ((response == EKeyWasConsumed) || 
                (aType != EEventKey && aKeyEvent.iScanCode != 
                EStdKeyApplication0))
        {
            response = EKeyWasConsumed;
        }
        else
        {
            // Old text is needed if the new content is not valid for the
            // given set of constraints. This validation is made because
            // CEikEdwin and FEP seem to input text that is not always
            // valid for the given Java-specific constraints. No very
            // optimized solution to allocate heap here but at least this
            // solution is bulletproof and prevents illegal contents.
            // This also prevents that the implementation does not ignore
            // special characters such as navigation and hotkeys.
            HBufC* oldContent = GetTextInHBufL();

            CleanupStack::PushL(oldContent);

            response = CEikEdwin::OfferKeyEventL(aKeyEvent, aType);
            
            if (response == EKeyWasConsumed) {
                // Validate new content and undo if not valid.
                if (!iEdwinUtils.ConstraintsValidForText(
                            Read(), iConstraints, EFalse))
                {
                    CEikEdwin::SetTextL(oldContent);
                    // Notify about text change.
                    HandleTextChangedL();
                    // Restore cursor's original position.
                    SetCursorPosL(iCursorPosForAction, EFalse);
                }
            }
            else
            { 
                // Consuming the up/down arrows, because edwin does not 
                // consume them if at first/last line. 
                if ((aType == EEventKey) &&
                        (((aKeyEvent.iCode == EKeyUpArrow) ||
                          (aKeyEvent.iCode == EKeyDownArrow)) ||
                         ((aKeyEvent.iScanCode == EStdKeyUpArrow) ||
                          (aKeyEvent.iScanCode == EStdKeyDownArrow))))
                {
                    response = EKeyWasConsumed;
                }
            }
            CleanupStack::PopAndDestroy(oldContent);
        }

        // Update current selection.
        iSelection = Selection();

        // Draw now so that background gets updated. Otherwise the
        // content may not be updated (e.g. cursor may be visible
        // if a line is deleted from the editor.
        // Transparency needs whole redraw of the editor's content.
        if (IsTransparent())
        {
            iParent->DrawNow(Rect());
        }
        else
        {
            DrawDeferred();
        }
    }

    DEBUG("CMIDTextEditorEdwin::OfferKeyEventL -");

    return response;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::Traverse
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditorEdwin::Traverse(const TKeyEvent& aEvent)
{
    TInt cursorPos = 0;
    TBool traverse = EFalse;

    if ((aEvent.iCode == EKeyDownArrow) ||
            (aEvent.iScanCode == EStdKeyDownArrow))
    {
        cursorPos = CursorPos();
        if (TextLength() == cursorPos)
        {
            traverse = ETrue;
        }

    }
    else if ((aEvent.iCode == EKeyUpArrow) ||
             (aEvent.iScanCode == EStdKeyUpArrow))
    {
        cursorPos = CursorPos();
        if (cursorPos == 0)
        {
            traverse = ETrue;
        }

    }

    return traverse;
}
// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::Draw
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::Draw(const TRect& aRect) const
{
    DEBUG("CMIDTextEditorEdwin::Draw +");

    // Part of transparency workaround. Set drawing active.
    iDrawInvoked = ETrue;

    // Background color is drawn before other content.
    // Note that in order this to work, the background drawing of CEikEdwin
    // needs to be suppressed. Custom drawer cannot be used due to flickering
    // problems. Currently it just handles the selection color.
    CWindowGc& gc = SystemGc();

    // If scaling is on, clip to canvas rect is needed.
    if (iIsScalingOn)
    {
        gc.SetClippingRect(iOnScreenCanvasRect);
    }

    // Draw background with alpha.
    gc.SetBrushColor(iBackgroundColor);
    gc.Clear(aRect);

    // Now draw the content of the editor.
    CEikEdwin::Draw(aRect);

    // cancel of clip
    if (iIsScalingOn)
    {
        gc.CancelClippingRect();
    }

    // Part of transparency workaround. Set drawing not active.
    iDrawInvoked = EFalse;

    DEBUG("CMIDTextEditorEdwin::Draw -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::DrawOngoing
// Part of transparency workaround. Returns the current draw status
// of this editor window.
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditorEdwin::DrawOngoing() const
{
    return iDrawInvoked;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::Redraw
// Part of transparency workaround. Returns the current draw status
// of this editor window.
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::Redraw() const
{
    // Do not redraw if draw is already ongoing.
    if (iDrawInvoked || !iParent)
    {
        return;
    }

    // If the editor has a transaprent background, the parent
    // needs to be redrawn in that case. Note that DrawNow
    // is not applicable here because this is used from the
    // custom draw which may invoke this method during CTextView
    // redraw causing a panic to occur.
    if (IsTransparent())
    {
        iParent->DrawDeferred();
    }
    else
    {
        // No transparency. Draw this control only.
        DrawDeferred();
    }
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::IsTransparent
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditorEdwin::IsTransparent() const
{
    return (iBackgroundColor.Alpha() < KColorOpaque);
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::FocusChanged
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::FocusChanged(TDrawNow aDrawNow)
{
    DEBUG("CMIDTextEditorEdwin::FocusChanged +");

    CEikEdwin::FocusChanged(aDrawNow);

    // Apply initial input mode change when the editor has been focused.
    // Note that initial input mode change must be applied only when
    // an editing session starts (according to MIDP specification).
    // See CMIDEdwin::HandleCurrentL()

    if (IsFocused() &&
            (IsConstraintSet(MMIDTextField::EAny) ||
             IsConstraintSet(MMIDTextField::EMailAddr) ||
             IsConstraintSet(MMIDTextField::EUrl)))
    {
        // Set current language.
        if (iInitialCurrentLanguage)
        {
            DEBUG("CMIDTextEditorEdwin::FocusChanged, \
applying current language");

            SetAknEditorLocalLanguage(iInitialCurrentLanguage);

            if (iInitialCurrentInputMode == 0)
            {
                DEBUG("CMIDTextEditorEdwin::FocusChanged, null input mode");

                // Clears any number mode used previously
                SetAknEditorCurrentInputMode(EAknEditorNullInputMode);
            }
        }

        // Set current input mode.
        if (iInitialCurrentInputMode)
        {
            DEBUG("CMIDTextEditorEdwin::FocusChanged, \
applying current input mode");

            SetAknEditorCurrentInputMode(iInitialCurrentInputMode);
        }

        // Set current case.
        if (iInitialCurrentCase)
        {
            DEBUG("CMIDTextEditorEdwin::FocusChanged, \
applying current case");

            TInt initialCurrentCase = iInitialCurrentCase;

            if ((iConstraints & MMIDTextField::EPassword ||
                    IsConstraintSet(MMIDTextField::EMailAddr) ||
                    IsConstraintSet(MMIDTextField::EUrl)) &&
                    iInitialCurrentCase == EAknEditorTextCase)
            {
                // Text case is not used in passwords, emailaddrs and urls
                initialCurrentCase = EAknEditorLowerCase;
            }

            SetAknEditorCurrentCase(initialCurrentCase);
            SetAknEditorCase(initialCurrentCase);
        }
    }

    DEBUG("CMIDTextEditorEdwin::FocusChanged -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::PositionChanged
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::PositionChanged()
{
    DEBUG("CMIDTextEditorEdwin::PositionChanged +");

    CCoeControl::PositionChanged();

    // Text view may have not been created yet.
    if (iTextView)
    {
        // Adjust the view rect to the text view
        iTextView->SetViewRect(Rect());
    }

    DEBUG("CMIDTextEditorEdwin::PositionChanged -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::PositionChanged
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::OverrideColorL(TInt aLogicalColor, TRgb aColor)
{
    CCoeControl::OverrideColorL(aLogicalColor, aColor);

    if (aLogicalColor == EColorControlBackground)
    {
        iBackgroundColor = aColor;
    }

    // Suppress background drawing if the editor is transparent.
    SetSuppressBackgroundDrawing(IsTransparent());
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::CreateCustomDrawL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
CLafEdwinCustomDrawBase* CMIDTextEditorEdwin::CreateCustomDrawL()
{
    DEBUG("CMIDTextEditorEdwin::CreateCustomDrawL +");

    // First create the parent custom draw that handles the drawing of
    // text and other related components.
    const MFormCustomDraw* customDraw = CEikEdwin::CreateCustomDrawL();

    // Create text editor specific custom draw that handles the
    // setting of selection color.
    CLafEdwinCustomDrawBase* ret = new(ELeave)CMIDTextEditorEdwinCustomDraw(
        iEikonEnv->LafEnv(), *customDraw, *this);

    DEBUG("CMIDTextEditorEdwin::CreateCustomDrawL -");

    return ret;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::HandleTextPastedL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::HandleTextPastedL(TInt aStartPos, TInt& aLength)
{
    DEBUG_INT2(
        "CMIDTextEditorEdwin::HandleTextPastedL +, aStartPos=%d, aLength=%d",
        aStartPos, aLength);

    HBufC* content = GetTextInHBufL(); // May return NULL
    if (!content)
    {
        // For guaranteeing that this method does not crash in any case.
        return;
    }

    CleanupStack::PushL(content);

    // Check that the pasted text is valid for the given set of constraints.
    if (!iEdwinUtils.ConstraintsValidForText(
                content->Des(), iConstraints, ETrue))
    {
        DEBUG("CMIDTextEditorEdwin::HandleTextPastedL, \
constraints not valid");

        // Remove pasted content from the text and set it back.
        iText->DeleteL(aStartPos, aLength);
        // Notify about the change.
        HandleTextChangedL();
    }
    // Convert breaks if needed if constraints that do not allow line breaks.
    // Note that numeric, decimal and phonenumber do not allow line breaks
    // and are checked before this so no need to check here again.
    else if (!IsWrapEnabled() ||
             iConstraints & MMIDTextField::EPassword  ||
             IsConstraintSet(MMIDTextField::EUrl) ||
             IsConstraintSet(MMIDTextField::EMailAddr))
    {
        DEBUG("CMIDTextEditorEdwin::HandleTextPastedL, \
cropping to single line");

        TPtr ptr = content->Des();
        iEdwinUtils.CropToSingleLine(ptr);
        CEikEdwin::SetTextL(content);
        // Notify about content change.
        HandleTextChangedL();
    }

    CleanupStack::PopAndDestroy(content);

    DEBUG("CMIDTextEditorEdwin::HandleTextPastedL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::HandleTextChangedL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::HandleTextChangedL()
{
    DEBUG("CMIDTextEditorEdwin::HandleTextChangedL +");

    // Call edwin's method to format the text and redraw.
    CEikEdwin::HandleTextChangedL();

    // Report edwin event.
    ReportEdwinEventL(MEikEdwinObserver::EEventTextUpdate);

    DEBUG("CMIDTextEditorEdwin::HandleTextChangedL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::HandleResourceChangeL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::HandleResourceChange(TInt aType)
{
    CEikEdwin::HandleResourceChange(aType);
    // Notification about language change
    if (aType == KEikInputLanguageChange)
    {
        if (iObserver)
        {
            TInt event = MMIDTextEditorObserver::EActionLanguageChange;
            TAknLayoutId currentLayoutId;
            CAknEnv::Static()->GetCurrentLayoutId(currentLayoutId);
            if (iDirection != currentLayoutId)
            {
                iDirection = currentLayoutId;
                event |= MMIDTextEditorObserver::EActionDirectionChange;
            }
            iObserver->NotifyInputAction(event);
        }
    }
#ifdef RD_JAVA_S60_RELEASE_9_2
    if ((aType == KAknSplitInputEnabled))
    {
        // Partial screen keyboard is opened
        CEikStatusPane* pane = GetStatusPane();
        if (!iPartialVKBOpen || pane->IsVisible())
        {
            // Hide status pane and resize displayable
            iDisplayable->HandleSplitScreenKeyboard(ETrue);
            // Preventing from notification when pane is visible, but VKB is
            // already opened
            if (iObserver && !iPartialVKBOpen)
            {
                iObserver->NotifyInputAction(
                    MMIDTextEditorObserver::EActionPartialInputEnabled);
            }
            iPartialVKBOpen = ETrue;
        }
    }
    else if ((aType == KAknSplitInputDisabled) && (iPartialVKBOpen))
    {
        HandlePartialVKBDisable();
    }
#endif // RD_JAVA_S60_RELEASE_9_2
}

#ifdef RD_JAVA_S60_RELEASE_9_2
// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::FocusLost
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::FocusLost()
{
    if (iPartialVKBOpen)
    {
        DEBUG("Focus LOST - disable VKB");
        CloseVKB();
    }
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::FocusLost
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::CloseVKB()
{
    CCoeFep* fep = iCoeEnv->Fep();
    if (fep)
    {
        fep->HandleDestructionOfFocusedItem();
    }
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::DisablePartialVKB
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::HandlePartialVKBDisable()
{
    // Partial screen keyboard is closed
    iPartialVKBOpen = EFalse;
    // Show status pane if not in fullscreen and resize displayable
    iDisplayable->HandleSplitScreenKeyboard(EFalse);
    if (iObserver)
    {
        iObserver->NotifyInputAction(
            MMIDTextEditorObserver::EActionPartialInputDisabled);
    }
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::GetStatusPane
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
CEikStatusPane* CMIDTextEditorEdwin::GetStatusPane()
{
    if (!iJavaAppUi)
    {
        iJavaAppUi = java::ui::CoreUiAvkonLcdui::
                     getInstance().getJavaAknAppUi();
    }
    CEikStatusPane* pane = iJavaAppUi->StatusPane();
    return pane;
}
#endif // RD_JAVA_S60_RELEASE_9_2

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::GetStatusPane
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditorEdwin::ScrollBarEvent()
{
    TInt event = 0;
    TInt newVisiblecontentHeight = VisibleContentPosition();

    // Check if cursor has moved. This must be done because
    // currently edwin does not report cursor position movement
    // when text is changed due to user input.
    if (iVisibleContentHeight != newVisiblecontentHeight)
    {
        event |= MMIDTextEditorObserver::EActionScrollbarChange;
        // Reported, reset here to avoid multiple notifications.
        iVisibleContentHeight = newVisiblecontentHeight;
    }
    return event;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::HandleEdwinEventL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::HandleEdwinEventL(
    CEikEdwin* /*aEdwin*/,
    TEdwinEvent aEventType)
{
    DEBUG_INT("CMIDTextEditorEdwin::HandleEdwinEventL +, aEventType=%d",
              aEventType);

    if (aEventType == EEventTextUpdate && (
                IsConstraintSet(MMIDTextField::EDecimal) ||
                IsConstraintSet(MMIDTextField::ENumeric)))
    {
        // Do not report events if the content was invalid.
        if (RemoveInvalidNumericContentL())
        {
            return;
        }
    }

    // Notify observer if set.
    if (iObserver)
    {
        switch (aEventType)
        {
        case EEventNavigation:
        {
            TInt event = MMIDTextEditorObserver::EActionCaretMove;
            event |= ScrollBarEvent();
            iObserver->NotifyInputAction(event);
            break;
        }
        case EEventFormatChanged: // Fallthrough
        case EEventTextUpdate:
        {
            TInt event = MMIDTextEditorObserver::EActionContentChange;

            // Check if cursor has moved. This must be done because
            // currently edwin does not report cursor position movement
            // when text is changed due to user input.
            if (iCursorPosForAction != KErrNotFound &&
                    iCursorPosForAction != CursorPos())
            {
                event |= MMIDTextEditorObserver::EActionCaretMove;
                // Reported, reset here to avoid multiple notifications.
                iCursorPosForAction = KErrNotFound;
            }
            event |= ScrollBarEvent();
            iObserver->NotifyInputAction(event);
            break;
        }
        case EEventScroll:
        {
            break;
        }
        default:
        {
            // Not handled.
            break;
        }
        }
    }

    DEBUG("CMIDTextEditorEdwin::HandleEdwinEventL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::HandlePointerEventL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMIDTextEditorEdwin::HandlePointerEventL(
    const TPointerEvent &aPointerEvent)
{
    DEBUG("CMIDTextEditorEdwin::HandlePointerEventL +");
    if (AknLayoutUtils::PenEnabled())
    {
        CEikEdwin::HandlePointerEventL(aPointerEvent);
    }
    DEBUG("CMIDTextEditorEdwin::HandlePointerEventL -");
}
#endif // RD_SCALABLE_UI_V2


// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::HandleSpecialKEyEventsL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TKeyResponse CMIDTextEditorEdwin::HandleSpecialKeyEventsL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    DEBUG("CMIDTextEditorEdwin::HandleSpecialKeyEventsL +");

    TKeyResponse response = EKeyWasNotConsumed;

    // Do not handle anything if text's maximum lenght has been reached
    // or if the editor is set to read only state.
    if (TextLength() >= MaxLength() || IsReadOnly())
    {
        DEBUG("CMIDTextEditorEdwin::HandleSpecialKeyEventsL -, \
max size reached or is read only");
        return response;
    }

    TUint sc = aKeyEvent.iScanCode;

    DEBUG_INT2("CMIDTextEditorEdwin::HandleSpecialKeyEventsL, \
scanCode=0x%x, iConstraints=0x%x", sc, iConstraints);

    TBool dec = IsConstraintSet(MMIDTextField::EDecimal);
    TBool num = IsConstraintSet(MMIDTextField::ENumeric);

    // Try to handle minus and full stop characters separately.
    // Other than EEventKey events are ignored.

    if ((dec || num) &&
            (aType == EEventKey) &&
            (sc == EStdKeyMinus || sc == EStdKeyNkpMinus ||
             sc == EStdKeyFullStop || sc == EStdKeyNkpFullStop ||
             sc == EStdKeyNkpAsterisk || sc == KMidpKeyNkpAsteriskHW))
    {
        DEBUG("CMIDTextEditorEdwin::HandleSpecialKeyEventsL, \
handling special");

        TInt cursorPos = CursorPos();
        TPtrC content = Read();

        // Toggle minus character. Decimal editor uses SCT so do not toggle
        // from asterisk key in that case.
        if ((sc == EStdKeyMinus || sc == EStdKeyNkpMinus) ||
                ((sc == EStdKeyNkpAsterisk || sc == KMidpKeyNkpAsteriskHW)
                 && num))
        {
            DEBUG("CMIDTextEditorEdwin::HandleSpecialKeyEventsL, \
toggle minus");

            if (content.Find(KMinusChar) == KErrNotFound)
            {
                iText->InsertL(0, KMinusChar);
                cursorPos++;
            }
            else
            {
                iText->DeleteL(0, 1);
                cursorPos--;
            }

            HandleTextChangedL();
            ReportEdwinEventL(MEikEdwinObserver::EEventTextUpdate);
            SetCursorPosL(cursorPos, EFalse);

            response = EKeyWasConsumed;
        }
        else if (dec && (sc == EStdKeyFullStop ||
                         sc == EStdKeyNkpFullStop))
        {
            TLocale locale;
            TChar decimalSeparator(locale.DecimalSeparator());

            if (content.Locate(decimalSeparator) == KErrNotFound)
            {
                DEBUG("CMIDTextEditorEdwin::HandleSpecialKeyEventsL, \
inserting full stop");

                // Insert full stop sign into to cursor's current position.
                iText->InsertL(cursorPos, decimalSeparator);
                HandleTextChangedL();
                ReportEdwinEventL(MEikEdwinObserver::EEventTextUpdate);
                SetCursorPosL(cursorPos + 1, EFalse);

                response = EKeyWasConsumed;
            }
        }
    }

    DEBUG_INT("CMIDTextEditorEdwin::HandleSpecialKeyEventsL - response=%d",
              response);

    return response;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetTopParent
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetTopParent(CCoeControl* aControl)
{
    iParent = aControl;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetObserver
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetObserver(MMIDTextEditorObserver* aObserver)
{
    DEBUG("CMIDTextEditorEdwin::SetObserver +");

    iObserver = aObserver;

    DEBUG("CMIDTextEditorEdwin::SetObserver -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetEditingStateIndicator
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetEditingStateIndicator(
    CMIDEditingStateIndicator* aIndicator)
{
    DEBUG("CMIDTextEditorEdwin::SetEditingStateIndicator +");

    // Indicator can be set to NULL.
    iIndicator = aIndicator;

    DEBUG("CMIDTextEditorEdwin::SetEditingStateIndicator -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetConstraintsL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetConstraintsL(TUint aConstraints)
{
    DEBUG("CMIDTextEditorEdwin::SetConstraintsL +");

    // Note that this method cannot be 100% leave-safe due to the current
    // CEikEdwin implementation.

    __ASSERT_DEBUG(iText, User::Invariant());

    // Apply document content.
    CGlobalText* newText = NULL;
    CGlobalText* oldText = static_cast< CGlobalText* >(iText);

    const CParaFormatLayer* paraFormatLayer =
        oldText->GlobalParaFormatLayer();

    const CCharFormatLayer* charFormatLayer =
        oldText->GlobalCharFormatLayer();

    TBool enableCcpuSupport = ETrue;

    // Create password text if mode is password (non-restrictive).
    if (aConstraints & MMIDTextField::EPassword)
    {
        DEBUG(
            "CMIDTextEditorEdwin::SetConstraintsL, creating password text");

        // Password editors must not use CCPU support according to
        // S60 Editing specification.
        enableCcpuSupport = EFalse;

        CPasswordBoxGlobalText* text = new(ELeave)CPasswordBoxGlobalText(
            paraFormatLayer, charFormatLayer, *this);

        CleanupStack::PushL(text);
        text->ConstructL();
        CleanupStack::Pop(text);

        newText = text;
    }
    else // Default, no password.
    {
        // Use default global text in the editor.
        newText = CGlobalText::NewL(paraFormatLayer, charFormatLayer);
    }

    CleanupStack::PushL(newText);
    SetDocumentContentL(newText);

    // Disable or enable CCPU support here.
    EnableCcpuSupportL(enableCcpuSupport);

    // Store constraints here. Rest of the method is leave-safe
    // and RestoreDefaultInputState uses the capabilities.
    iConstraints = aConstraints;

    // Restore default input state before making any modifications. This
    // ensures that the next input constraints are set correctly and
    // that the previous constraints do not affect on the new input modes.
    // This should be done in CMIDEdwinUtils::SetFEPModeAndCharFormat()
    // but it does not reset all the necessary input parameters.
    RestoreDefaultInputState();

    // Set uneditable if set as current mode.
    SetReadOnly(aConstraints & MMIDTextField::EUneditable);

    // Use edwin utils to set the FEP mode and character format.
    iEdwinUtils.SetFEPModeAndCharFormat(aConstraints, this);

    // Publish MIDP constraints via input capabilities of CEikEdwin.
    CAknExtendedInputCapabilities* caps = AknInputCaps();
    if (caps)
    {
        DEBUG(
            "CMIDTextEditorEdwin::SetConstraintsL, setting MIDP constaints");

        caps->SetMIDPConstraints(aConstraints);
    }

    // CMIDEdwinUtils modify the multiline state of the editor
    // thus it needs the be set again here. Passwords are always
    // single line editors.
    if (!(aConstraints & MMIDTextField::EPassword))
    {
        SetWordWrapL(iMultiline);
    }
    else
    {
        SetWordWrapL(EFalse);
    }
    CleanupStack::Pop(newText);

    DEBUG("CMIDTextEditorEdwin::SetConstraintsL +");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetInitialInputModeL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetInitialInputModeL(
    const TDesC& aCharacterSubset)
{
    DEBUG_STR(
        "CMIDTextEditorEdwin::SetInitialInputModeL +, aCharacterSubset=%S",
        aCharacterSubset);

    // Set initial input mode. Note that the change is performed
    // after the next editing session starts. See MIDP specification.
    // See CMIDEdwin::SetInitialInputModeL

    iEdwinUtils.SetInitialInputModeL(
        aCharacterSubset,
        iConstraints,
        iInitialCurrentCase,
        iInitialCurrentInputMode,
        iInitialCurrentLanguage);

    // Set permitted case modes for edwin in lowercase or uppercase mode.
    if (aCharacterSubset.Compare(KMidpUppercaseLatin) == 0 ||
            aCharacterSubset.Compare(KMidpLowercaseLatin) == 0)
    {
        DEBUG("CMIDTextEditorEdwin::SetInitialInputModeL, latin");

        // MIDP_UPPERCASE_LATIN or MIDP_LOWERCASE_LATIN are ignored if
        // INITIAL_CAPS_SENTENCE or INITIAL_CAPS_WORD modifier in ANY.
        if (!(iConstraints & MMIDTextField::EInitialCapsWordSentence ||
                iConstraints & MMIDTextField::EInitialCapsWord) ||
                !IsConstraintSet(MMIDTextField::EAny))
        {
            // If initial input mode is uppercase or lowercase then permit
            // only explicit case mode changes, automatic changes are not
            // allowed.
            SetAknEditorPermittedCaseModes(
                EAknEditorUpperCase | EAknEditorLowerCase);
        }
    }
    else
    {
        SetAknEditorPermittedCaseModes(EAknEditorAllCaseModes);
    }

    DEBUG("CMIDTextEditorEdwin::SetInitialInputModeL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::RestoreDefaultInputState
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::RestoreDefaultInputState()
{
    DEBUG("CMIDTextEditorEdwin::RestoreEdwinDefaultInputState +");

    // Restore the default input state. It is not possible to directly
    // call CAknEditorState object from CEikEdwin since it is currently
    // used internally only. The default state set here is the same
    // that is set when CAknEditorState is constructed. See
    // CAknEditorState::CAknEditorState in aknedsts.cpp for details.

    SetAknEditorCase(EAknEditorTextCase);
    SetAknEditorPermittedCaseModes(EAknEditorAllCaseModes);
#ifdef RD_JAVA_S60_RELEASE_9_2
    SetAknEditorFlags(EAknEditorFlagEnablePartialScreen);
#else
    SetAknEditorFlags(EAknEditorFlagDefault);
#endif // RD_JAVA_S60_RELEASE_9_2
    SetAknEditorSpecialCharacterTable(KErrNotFound);
    SetAknEditorInputMode(EAknEditorTextInputMode);
    SetAknEditorAllowedInputModes(EAknEditorAllInputModes);
    SetAknEditorCurrentInputMode(EAknEditorTextInputMode);
    SetAknEditorCurrentCase(EAknEditorTextCase);
    SetAknEditorNumericKeymap(EAknEditorAlphanumericNumberModeKeymap);
    SetAknEditorLocalLanguage(
        CAknEnv::Static()->SettingCache().InputLanguage());

    // Enable SCT in other than numeric editors. Note that asterisk toggles
    // the minus character so the SCT must not be enabled because of that.
    if (!IsConstraintSet(MMIDTextField::ENumeric))
    {
        CAknExtendedInputCapabilities* caps = AknInputCaps();
        if (caps)
        {
            DEBUG("CMIDTextEditorEdwin::RestoreEdwinDefaultInputState,\
 disable SCT");

            caps->SetCapabilities(
                caps->Capabilities() &
                ~CAknExtendedInputCapabilities::EDisableSCT);
        }
    }

    // Reset initial input mode also.
    iInitialCurrentCase = 0;
    iInitialCurrentInputMode = 0;
    iInitialCurrentLanguage = ELangTest;

    DEBUG("CMIDTextEditorEdwin::RestoreEdwinDefaultInputState -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetCursorPosL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetCursorPosL(
    TInt aCursorPosition,
    TBool aSelect)
{
    DEBUG_INT("CMIDTextEditorEdwin::SetCursorPosL +, aCursorPosition=%d",
              aCursorPosition);

    // Set cursor if the editor has a valid parent.
    if (IsInitialized())
    {
        CEikEdwin::SetCursorPosL(aCursorPosition, aSelect);
    }
    // Do not deliver event if the position actually does not change.
    else if (CursorPos() != aCursorPosition)
    {
        // Event has to be delivered to the registered observer
        // even if the editor window has not been initialized.
        ReportEdwinEventL(MEikEdwinObserver::EEventNavigation);
    }

    if (aSelect)
    {
        // CEikEdwin does not store selection if not initialized.
        // Store it in order to avoid errors when setting selection
        // before parent is stored.
        iSelection.SetSelection(iCursorPosition, aCursorPosition);
    }
    else // Clear selection.
    {
        iSelection.SetSelection(0, 0);
    }

    // Editor window does not know the position yet because it has not
    // been activated. Store the position to a member variable. Note that
    // this must be set when the editor is activated.
    // Cursor needs to be stored all the time because change of the
    // parent object may cause the cursor to be changed in CEikEdwin.
    iCursorPosition = aCursorPosition;

    DEBUG("CMIDTextEditorEdwin::SetCursorPosL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::CursorPos
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditorEdwin::CursorPos() const
{
    DEBUG("CMIDTextEditorEdwin::CursorPos +");

    // The control may have not been initialized yet (i.e. SetParentL has
    // not been called). Do not get cursor position in that case because
    // it crashes due to missing text view in the CEikEdwin.
    TInt cursorPosition(
        (IsInitialized() ? CEikEdwin::CursorPos() : iCursorPosition));

    DEBUG_INT("CMIDTextEditorEdwin::CursorPos -, cursorPosition=%d",
              cursorPosition);

    return cursorPosition;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::Selection
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TCursorSelection CMIDTextEditorEdwin::Selection()
{
    DEBUG("CMIDTextEditorEdwin::Selection +");

    TCursorSelection selection(iSelection);

    // Selection is not returned correctly if the editor has not been
    // initialized.
    if (IsInitialized())
    {
        selection = CEikEdwin::Selection();
    }

    DEBUG("CMIDTextEditorEdwin::Selection -");

    return selection;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetWordWrapL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetWordWrapL(TBool aWrapIsOn)
{
    DEBUG_INT("CMIDTextEditorEdwin::SetWordWrapL +, aWrapIsOn=%d",
              aWrapIsOn);

    // CEikEdwin crashes when setting word wrap before it
    // has a valid container window. Multiline state is set
    // in InitializeL() method again when called.
    if (IsInitialized())
    {
        CEikEdwin::SetWordWrapL(aWrapIsOn);

        // Apply line and paragraph breaks.
        if (!aWrapIsOn)
        {
            AddFlagToUserFlags(CEikEdwin::ENoLineOrParaBreaks);
        }
        else
        {
            RemoveFlagFromUserFlags(CEikEdwin::ENoLineOrParaBreaks);
        }
    }

    iMultiline = aWrapIsOn;

    DEBUG("CMIDTextEditorEdwin::SetCursorPosL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::IsWrapEnabled
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditorEdwin::IsWrapEnabled() const
{
    return iMultiline;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetSize
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetSize(TInt aWidth, TInt aHeight)
{
    DEBUG("CMIDTextEditorEdwin::SetSize +");

    // Set the width of the editor window. Note that EWidthInPixels is set
    // so this method uses the parameter as pixels instead of chars.
    CalculateWidth(aWidth);
    // Set minimum and maximum height to the same value.
    SetMinimumHeight(aHeight);
    SetMaximumHeight(aHeight);

    TSize size(aWidth, aHeight);
    // Set size of the UI control if container has been set. Otherwise
    // this will panic the editor window at some point.
    if (IsInitialized())
    {
        CCoeControl::SetSize(size);
    }

    // Store the size for later use when set parent has been called.
    iSize = size;

    DEBUG("CMIDTextEditorEdwin::SetSize -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::Size
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TSize CMIDTextEditorEdwin::Size() const
{
    DEBUG("CMIDTextEditorEdwin::Size +");

    TSize size(iSize);

    // Check if editor has been initialized.
    if (IsInitialized())
    {
        size = CCoeControl::Size();
    }

    DEBUG("CMIDTextEditorEdwin::Size -");

    return size;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::IsInitialized
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditorEdwin::IsInitialized() const
{
    DEBUG_INT("CMIDTextEditorEdwin::IsInitialized, iInitialized=%d",
              iInitialized);

    // Currently, the text editor is considered as constructed when the text
    // view has been created and the editor window has been activated.
    return iInitialized;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::InitializeL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::InitializeL(CMIDDisplayable* aDisplayable)
{
    DEBUG("CMIDTextEditorEdwin::InitializeL +");

    if (!iInitialized)
    {

#ifdef RD_JAVA_S60_RELEASE_9_2
        iDisplayable = aDisplayable;
#else
        (void)aDisplayable;  // Just to remove a compiler warning
#endif // RD_JAVA_S60_RELEASE_9_2

        CreateTextViewL();
        ActivateL();

        // Set colors here in order to avoid crash when parent
        // has not been set by CMIDTextEditor.
        SetBackgroundColorL(iBackgroundColor);
        iTextView->SetBackgroundColor(iBackgroundColor);

        // Set selection set before initialization.
        CEikEdwin::SetCursorPosL(iCursorPosition, EFalse);

        // Selection overrides cursor's position
        if (iSelection.Length() > 0)
        {
            CEikEdwin::SetCursorPosL(iSelection.LowerPos(), EFalse);
            CEikEdwin::SetCursorPosL(iSelection.HigherPos(), ETrue);
        }

        // By default, editor is unfocused and not visible. Note
        // that hide and show cannot be called before set parent.
        SetFocus(EFalse);
        MakeVisible(EFalse);
        // Apply current size.
        CCoeControl::SetSize(iSize);
        iInitialized = true;
        // Apply current multiline status. iInitialized must be true here.
        SetWordWrapL(iMultiline);
    }

    DEBUG("CMIDTextEditorEdwin::InitializeL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::Uninitialize
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::Uninitialize()
{
    DEBUG("CMIDTextEditorEdwin::Uninitialize +");

    if (iInitialized)
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
        iDisplayable = NULL;
#endif // RD_JAVA_S60_RELEASE_9_2
        // Remove focus and hide the editor.
        SetFocus(EFalse);
        MakeVisible(EFalse);
        // Store cursor's current position.
        iCursorPosition = CEikEdwin::CursorPos();

        iInitialized = false;
        iParent = NULL;
    }

    DEBUG("CMIDTextEditorEdwin::Uninitialize -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::EditorWindowHeight
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditorEdwin::EditorWindowHeight() const
{
    DEBUG("CMIDTextEditorEdwin::EditorWindowHeight +");

    TInt minimumHeight = iSize.iHeight;
    if (iNumberOfLines == 0)
    {
        if (MinimumHeight() > 0)
        {
            minimumHeight = MinimumHeight();
        }
    }
    else
    {
        minimumHeight = (LineHeight() * iNumberOfLines)
                        + iBorder.SizeDelta().iHeight
                        + iMargins.iTop
                        + iMargins.iBottom;
    }

    DEBUG_INT("CMIDTextEditorEdwin::EditorWindowHeight - , height=%d",
              minimumHeight);

    return minimumHeight;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::NumberOfLines
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditorEdwin::NumberOfLines() const
{
    __ASSERT_DEBUG(iLayout, User::Invariant());

    return iLayout->NumFormattedLines();
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::LineHeight
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditorEdwin::LineHeight() const
{
    __ASSERT_DEBUG(iLayout, User::Invariant());

    TInt lineHeight = iLayout->FormattedHeightInPixels();
    const TInt docLength = iLayout->DocumentLength();
    if (docLength > 0)
    {
        const TInt numFormattedLines = iLayout->NumFormattedLines();
        lineHeight = lineHeight / numFormattedLines;
    }

    return lineHeight;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::VisibleContentPosition
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditorEdwin::VisibleContentPosition() const
{
    DEBUG("CMIDTextEditorEdwin::VisibleContentPosition +");

    __ASSERT_DEBUG(iLayout, User::Invariant());

    DEBUG_INT("CMIDTextEditorEdwin::VisibleContentPosition -, pos=%d",
              iLayout->PixelsAboveBand());

    return iLayout->PixelsAboveBand();
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::Read
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
const TPtrC CMIDTextEditorEdwin::Read() const
{
    if (iConstraints & MMIDTextField::EPassword)
    {
        return static_cast< CPasswordBoxGlobalText* >(iText)->ClearText();
    }

    return iText->Read(0, TextLength());
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::MopSupplyObject
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CMIDTextEditorEdwin::MopSupplyObject(TTypeUid aId)
{
    // Try supplying custom indicator.
    TTypeUid::Ptr id((
                         iIndicator ? iIndicator->SupplyIndicatorMopObject(aId)
                         : TTypeUid::Null()));

    // Indicator was not supplied. Try supplying from CEikEdwin.
    if (!(id.Pointer()))
    {
        return CEikEdwin::MopSupplyObject(aId);
    }

    return id;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::SetDocumentContentL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorEdwin::SetDocumentContentL(CGlobalText* aText)
{
    DEBUG("CMIDTextEditorEdwin::SetDocumentContentL +");

    // Even though the method takes a reference in it, the ownership
    // is still transferred to CEikEdwin. And current iText is simply
    // left ignored. Store it locally and delete after successful
    // operation
    CPlainText* temp = iText;
    TRAPD(err, CEikEdwin::SetDocumentContentL(
              *aText, CEikEdwin::EUseText));

    if (err != KErrNone)
    {
        // Failed but the ownership was transferred... State has been changed
        // but we avoid leaking any memory at this point which is better.
        // Restore the text back. This is not bulletproof, but leaves
        // the text state as it was before trying to set the new text.
        delete iText;
        iText = temp;
        User::Leave(err);
    }

    // Everything went fine. Delete the old iText.
    delete temp;

    DEBUG("CMIDTextEditorEdwin::SetDocumentContentL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::AknInputCaps
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
CAknExtendedInputCapabilities* CMIDTextEditorEdwin::AknInputCaps() const
{
    CAknExtendedInputCapabilities* caps = NULL;

    TCoeInputCapabilities inputCapabilities = InputCapabilities();
    if (inputCapabilities != TCoeInputCapabilities::ENone)
    {
        DEBUG("CMIDTextEditorEdwin::AknInputCaps, editor has input caps");

        // Get input capabilities object provider.
        MObjectProvider* provider = inputCapabilities.ObjectProvider();
        if (provider)
        {
            DEBUG(
                "CMIDTextEditorEdwin::AknInputCaps, object provider found");

            // MopGetObjects needs a reference to a pointer to an object
            // of a type that is to be retrieved. NULL is OK here.
            caps = provider->MopGetObject(caps);
        }
    }

    return caps;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::RemoveInvalidNumericContentL
// (other items were commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditorEdwin::RemoveInvalidNumericContentL()
{
    DEBUG("CMIDTextEditorEdwin::RemoveInvalidNumericContentL +");

    TBool textChanged = EFalse;
    TPtrC content = Read();

    // Handle content change in numeric and decimal editors if the
    // content is not valid for the given set of constraints.
    if (!iEdwinUtils.ConstraintsValidForText(content, iConstraints, ETrue))
    {
        DEBUG("CMIDTextEditorEdwin::RemoveInvalidNumericContentL, \
content not valid");

        TLocale locale;
        TChar decimal(locale.DecimalSeparator());
        TChar minus(KCharMinus);

        TInt cursorPos = CursorPos();
        TInt minusPos = content.Locate(minus);
        TInt decimalPos = content.Locate(decimal);

        // Validate that minus character has been entered correctly
        // First cases in which there is only minus and separator.
        if (content.Length() == 1 &&
                ((decimalPos == 0) || (minusPos == 0)))
        {
            iText->DeleteL(0, 1);
            cursorPos = 0;
        }
        else if (content.Length() == 2 &&
                 ((minusPos == 0) && (decimalPos == 1)))
        {
            iText->DeleteL(0, 2);
            cursorPos = 0;
        }
        else
        {
            // Other cases. Minus and decimal somewhere in the content.
            if (minusPos != KErrNotFound)
            {
                // The first is valid. Remove all other minus chars.
                minusPos = content.LocateReverse(minus);
                while (minusPos != 0 && minusPos != KErrNotFound)
                {
                    iText->DeleteL(minusPos, 1);
                    minusPos = content.LocateReverse(minus);
                    cursorPos--;
                }
            }

            // Decimal position may have changed.
            decimalPos = content.Locate(decimal);

            if (decimalPos != KErrNotFound)
            {
                // The first is valid. Remove all other separators.
                TInt secondPos = content.LocateReverse(decimal);
                while (secondPos != decimalPos)
                {
                    iText->DeleteL(secondPos, 1);
                    secondPos = content.LocateReverse(decimal);
                    cursorPos--;
                }
            }
        }

        // Do not report event again so call base class' method.
        CEikEdwin::HandleTextChangedL();
        // Restore cursor's original position.
        SetCursorPosL(cursorPos, EFalse);
        textChanged = ETrue;
    }

    DEBUG_INT(
        "CMIDTextEditorEdwin::RemoveInvalidNumericContentL -, textChanged=%d",
        textChanged);

    return textChanged;
}

// End of file
