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
#include <AknNumEdwin.h>
#include <eikfpne.h>
#include <eswtcore.rsg>
#include <avkon.hrh>
#include "swtcontrolhelper.h"
#include "swtconstrainedtext.h"
#include "swtedwin.h"

// --------------------------------------------------------------------------------
// CSwtConstrainedText::CSwtConstrainedText
// --------------------------------------------------------------------------------
//
inline CSwtConstrainedText::CSwtConstrainedText(MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle)
        : CSwtTextBase(aDisplay, aPeer, aParent, aStyle)
        , iTextLimit(MSwtText::ETextLimit)
{
    // Set decimal separator according to the current locale.
    // Note: decimal separators other than ',' and '.' are currently not supported
    TLocale loc;
    iDecimalSeparator = loc.DecimalSeparator();
    if (iDecimalSeparator != ',')
    {
        iDecimalSeparator = '.';
    }
}

// --------------------------------------------------------------------------------
// CSwtConstrainedText::~CSwtConstrainedText
// --------------------------------------------------------------------------------
//
CSwtConstrainedText::~CSwtConstrainedText()
{
}

// --------------------------------------------------------------------------------
// CSwtConstrainedText::NewL
// --------------------------------------------------------------------------------
//
CSwtConstrainedText* CSwtConstrainedText::NewL(MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle)
{
    CSwtConstrainedText* self = new(ELeave) CSwtConstrainedText(aDisplay,
            aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtConstrainedText::Draw
// ---------------------------------------------------------------------------
//
void CSwtConstrainedText::Draw(const TRect& /*aRect*/) const
{
    if (GetShell().UrgentPaintControl() == this)
        return;

    CEikEdwin& editor(Editor());
    if (editor.TextView() == NULL)
    {
        return;
    }
    
    // Update editor's visibility, otherwise text will be drawn
    // when performing clipboard operations on invisible control.
    const TBool visible = GetVisibleRecursively();
    if (visible != editor.IsVisible())
    {
        editor.MakeVisible(visible);
    }

    // Update custom clipping, otherwise edwin will draw outside of
    // its parent control.
    TRect clipRect(ClipToVisibleRect(editor.TextView()->ViewRect()));
    if (clipRect != iLastViewVisibleRect)
    {
        switch (iConstraint)
        {
        case ENumeric:
            ((CSwtIntegerEdwin*)&editor)->SetClippingRect(clipRect);
            break;
        case EDecimal:
            ((CSwtFloatingPointEdwin*)&editor)->SetClippingRect(clipRect);
            break;
        case EPhoneNumber:
            ((CSwtEdwin*)&editor)->SetClippingRect(clipRect);
            break;
        default:
            ASSERT(EFalse);
            break;
        }
        iLastViewVisibleRect = clipRect;
    }
}

// --------------------------------------------------------------------------------
//  Checks the content will remain valid if the specified text replaces
//       the given range.
// --------------------------------------------------------------------------------
//
inline TInt CSwtConstrainedText::LooseCheckReplacementL(TInt aStart,
        TInt aEnd,
        const TDesC& aText) const
{
    return CheckReplacementL(aStart, aEnd, aText, EFalse);
}


// --------------------------------------------------------------------------------
//   Checks the content will remain valid if the specified character replaces
//         the given range.
// --------------------------------------------------------------------------------
TInt CSwtConstrainedText::LooseCheckReplacementL(TInt aStart,
        TInt aEnd,
        TInt aChar) const
{
    TBuf<1> buf;
    buf.Append(aChar);
    return LooseCheckReplacementL(aStart, aEnd, buf);
}


// --------------------------------------------------------------------------------
//   Checks the content will remain valid if the specified text replaces
//         the current selection.
// --------------------------------------------------------------------------------
inline TInt CSwtConstrainedText::StrictCheckReplacementL(const TDesC& aText) const
{
    const TCursorSelection sel(Editor().Selection());
    return CheckReplacementL(sel.LowerPos(), sel.HigherPos(), aText, ETrue);
}


// --------------------------------------------------------------------------------
//  Checks the content will remain valid if the specified text replaces
//         the given range.
// --------------------------------------------------------------------------------
TInt CSwtConstrainedText::CheckReplacementL(TInt aStart,
        TInt aEnd,
        const TDesC& aText,
        TBool aStrict) const
{

    ASSERT(aStart <= aEnd);

    const TInt selLength = aEnd - aStart;

    const TInt bufLength = Editor().TextLength() + Max(0, aText.Length() - selLength);
    HBufC* buf = HBufC::NewLC(bufLength);
    TPtr ptr(buf->Des());
    Editor().GetText(ptr);
    ptr.Delete(aStart, selLength);
    ptr.Insert(aStart, aText);
    TInt result = (aStrict) ? StrictValidateText(*buf) : LooseValidateText(*buf);
    CleanupStack::PopAndDestroy(buf);
    return result;
}


// --------------------------------------------------------------------------------
// CSwtConstrainedText::LooseValidateText
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::LooseValidateText(const TDesC& aText) const
{
    switch (iConstraint)
    {
    case ENumeric:
        return LooseValidateNumeric(aText);
    case EDecimal:
        return LooseValidateDecimal(aText);
    case EPhoneNumber:
        return ValidatePhoneNumber(aText);
    default:
        ASSERT(EFalse);
        return KErrNone;
    }
}

// --------------------------------------------------------------------------------
// CSwtConstrainedText::StrictValidateText
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::StrictValidateText(const TDesC& aText) const
{
    switch (iConstraint)
    {
    case ENumeric:
        return StrictValidateNumeric(aText);
    case EDecimal:
        return StrictValidateDecimal(aText);
    case EPhoneNumber:
        return ValidatePhoneNumber(aText);
    default:
        ASSERT(EFalse);
        return KErrNone;
    }
}

// --------------------------------------------------------------------------------
//  Checks whether a given text is an acceptable NUMERIC. It must be
//  made of digits only and an optional leading minus sign. If the
//  maximum text size is 1, then no minus sign is allowed.
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::LooseValidateNumeric(const TDesC& aText) const
{
    const TText* ptr = aText.Ptr();

    // Likely text will be appended, so we perform checking backwards for faster execution
    for (TInt pos=aText.Length(); pos>0;)
    {
        TChar ch(ptr[--pos]);
        if (!ch.IsDigit() && !(ch == '-' && (pos == 0 && iTextLimit != 1)))
        {
            return KErrArgument;
        }
    }

    return KErrNone;
}

// --------------------------------------------------------------------------------
//  Strict version of the NUMERIC check. Same as the loose one except a "-" is
//  no longer accepted.
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::StrictValidateNumeric(const TDesC& aText) const
{
    _LIT(KMinus, "-");
    if (aText.Compare(KMinus) == 0)
    {
        return KErrArgument;
    }
    return LooseValidateNumeric(aText);
}

// --------------------------------------------------------------------------------
//  Checks whether a text is an acceptable DECIMAL. It consists of an
//  optional minus sign '-', followed by one or more whole-number
//  decimal digits, followed by an optional fraction separator,
//  followed by zero or more decimal fraction digits. The whole-number
//  decimal digits may be omitted if the fraction separator and one or
//  more decimal fraction digits are present.
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::LooseValidateDecimal(const TDesC& aText) const
{
    TInt len = aText.Length();
    TInt pos = 0;
    const TText* ptr = aText.Ptr();

    if (len == 0)
    {
        return KErrNone;
    }

    // '-' is allowed only in the begining of the text and limit>1
    if (ptr[0] == '-')
    {
        pos = 1;
        //'.' is not allowed when first character is '-' and limit is 2
        if ((pos<len && (TChar(ptr[pos]) == iDecimalSeparator) && iTextLimit == 2))
        {
            return KErrArgument;
        }
    }

    for (TInt pos1=aText.Length(); pos1>0;)
    {
        TChar ch(ptr[--pos1]);
        //'-', '.' is not allowed when limit is 1
        if (((ch == '-' || ch == iDecimalSeparator) && (pos1 == 0 && iTextLimit == 1)))
        {
            return KErrArgument;
        }
    }

    // allows all digits, and allow only one '.'
    TBool valid = ETrue;
    while (pos<len && (TChar(ptr[pos]).IsDigit() ||
                       (TChar(ptr[pos]) == iDecimalSeparator && valid)))
    {
        if (TChar(ptr[pos]) == iDecimalSeparator)
        {
            valid = EFalse;
        }
        ++pos;
    }
    return (pos == len) ? KErrNone : KErrArgument;
}


// --------------------------------------------------------------------------------
//  Strict version of the DECIMAL check. Same as the loose one except "-", "."
//  and "-." are no longer accepted.
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::StrictValidateDecimal(const TDesC& aText) const
{
    _LIT(KMinus, "-");

    TBuf<1> point;
    point.Append(iDecimalSeparator);

    TBuf<2> minusPoint;
    minusPoint.Append(KMinus);
    minusPoint.Append(iDecimalSeparator);

    if (aText.Compare(KMinus) == 0 || aText.Compare(point) == 0 ||
            aText.Compare(minusPoint) == 0)
    {
        return KErrArgument;
    }
    return LooseValidateDecimal(aText);
}


// --------------------------------------------------------------------------------
//  Checks if a character is acceptable in a phone number
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::IsPhoneNumberChar(const TChar &aChar,
        const TDesC& aAllowedChars)
{
    // Well, digits and space: of course
    if (aChar.IsDigit() || aChar==' ')
    {
        return ETrue;
    }


    // Is it one of the other allowed chars
    TInt len = aAllowedChars.Length();
    for (TInt i = 0; i<len; ++i)
    {
        if (aChar == TChar(aAllowedChars[i]))
        {
            return ETrue;
        }
    }

    // Not a phone number character
    return EFalse;
}


// --------------------------------------------------------------------------------
//  Checks if a text is a correct PHONENUMBER. Checks it contains only
//  allowed characters, no syntax checking.
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::ValidatePhoneNumber(const TDesC& aText)
{

    _LIT(KDefaultSpecialChars, "*#+ABCD,T!@;IiGg>()wpWP");

    // Load the list of allowed chars from resource
    const TDesC* allowed = &KDefaultSpecialChars;

    // Likely text will be appended, so we perform checking backwards for faster execution
    const TText* ptr = aText.Ptr();
    for (TInt i=aText.Length(); i>0;)
    {
        if (!IsPhoneNumberChar(ptr[--i], *allowed))
        {
            return KErrArgument;
        }
    }

    return KErrNone;
}

// --------------------------------------------------------------------------------
// Converts decimal point, if any, to a localized decimal separator.
// --------------------------------------------------------------------------------
//
HBufC* CSwtConstrainedText::ConvertToLocalizedDecimalLC(const TDesC& aText)
{
    HBufC* buf = aText.AllocLC();
    TPtr text = buf->Des();
    TInt pos = buf->Length();
    for (; pos>0;)
    {
        TText& ch = text[--pos];
        if (ch == '.')
            ch = (TText)iDecimalSeparator;
    }
    return buf;
}

// --------------------------------------------------------------------------------
// Converts localized decimal separator, if any, to a decimal point.
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::ConvertFromLocalizedDecimal(HBufC* buf) const
{
    TPtr text = buf->Des();
    TInt pos = buf->Length();
    for (; pos>0;)
    {
        TText& ch = text[--pos];
        if (ch == (TText)iDecimalSeparator)
            ch = '.';
    }
}


// --------------------------------------------------------------------------------
//    CSwtConstrainedText::IsNavigationKey
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::IsNavigationKey(const TKeyEvent& aKeyEvent) const
{
    switch (aKeyEvent.iScanCode)
    {
    case EStdKeyDownArrow:
    case EStdKeyUpArrow:
    case EStdKeyLeftArrow:
    case EStdKeyRightArrow:
    case EStdKeyEscape:
    case EStdKeyPageUp:
    case EStdKeyPageDown:
    case EStdKeyHome:
    case EStdKeyEnd:
        return ETrue;
    default:
        return EFalse;
    }
}

// --------------------------------------------------------------------------------
//    CSwtConstrainedText::IsHotKeyL
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::IsHotKeyL(const TKeyEvent& aKeyEvent) const
{

    switch (HotKeyL(aKeyEvent))
    {
    case CEikEdwin::EHotKeyCut:
    case CEikEdwin::EHotKeyCopy:
    case CEikEdwin::EHotKeyPaste:
    case CEikEdwin::EHotKeyUndo:
    case CEikEdwin::EHotKeyFind:
    case CEikEdwin::EHotKeyInsertChar:
        return ETrue;
    default:
        return EFalse;
    }
}

// --------------------------------------------------------------------------------
// Virtual methods from CCoeControl
// --------------------------------------------------------------------------------
//
TKeyResponse CSwtConstrainedText::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    TBool traversalDoIt = ETrue;

    // Set the traversal do-it default to false for the following cases:

    // Escape
    if (aKeyEvent.iCode == EKeyEscape)
    {
        traversalDoIt = EFalse;
    }
    // Left and Right arrows : do not allow traversal
    else if (aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow)
    {
        traversalDoIt = EFalse;
    }
    return CSwtTextBase::HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

// --------------------------------------------------------------------------------
// Virtual methods from MSwtControl
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::ProcessKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    if (aType != EEventKey)
    {
        return;
    }


    // Read-only: CEikEdwin displays a message and leaves. We want the message
    // but not the leaving, hence the trap.
    if (iStyle & KSwtStyleReadOnly)
    {
        TRAP_IGNORE(ForwardKeyEventL(aKeyEvent, aType));
        return;
    }

    // We're single-line: enter not consumed, translated to default selection event
    if (aKeyEvent.iCode == EKeyEnter)
    {
        iDisplay.PostDefaultSelectionEventL(iPeer);
        return;
    }


    const TInt modifiers = aKeyEvent.iModifiers & ECtrlFuncShiftMask;
    const TCursorSelection selection(Editor().Selection());
    const TInt selStart  = selection.LowerPos();
    const TInt selEnd    = selection.HigherPos();
    const TInt selLength = selEnd - selStart;
    const TInt length    = Editor().TextLength();
    const TInt hotkey    = HotKeyL(aKeyEvent);

    TBool notify   = ETrue;
    TInt  validity;

    // Cut
    if (hotkey == CEikEdwin::EHotKeyCut)
    {
        validity = LooseCheckReplacementL(selStart, selEnd, KNullDesC);
    }
    // Paste
    else if (hotkey == CEikEdwin::EHotKeyPaste)
    {
        TPtrC content(SwtControlHelper::GetClipboardTextContentLC());
        if (content.Length() == 0)
        {
            // Nothing to paste: CEikEdwin displays a message and leaves. We want
            // the message but not the leaving, hence the trap.
            TRAP_IGNORE(ForwardKeyEventL(aKeyEvent, aType));
            return;
        }

        validity = LooseCheckReplacementL(selStart, selEnd, content);
        if (content.Ptr())
        {
            CleanupStack::PopAndDestroy(&content);
        }
    }
    // Undo
    else if (hotkey == CEikEdwin::EHotKeyUndo)
    {
        validity = KErrNone;
    }
    // Delete
    else if (aKeyEvent.iCode == EKeyDelete && modifiers == 0)
    {
        if (selLength != 0)
        {
            validity = LooseCheckReplacementL(selStart, selEnd, KNullDesC);
        }
        else if (selection.iCursorPos < length)
        {
            validity = LooseCheckReplacementL(selection.iCursorPos,
                                              selection.iCursorPos+1, KNullDesC);
        }

        else
        {
            validity = KErrArgument;
        }
    }
    // Backspace
    else if (aKeyEvent.iCode == EKeyBackspace && modifiers == 0)
    {
        if (selLength != 0)
        {
            validity = LooseCheckReplacementL(selStart, selEnd, KNullDesC);
        }
        else if (selection.iCursorPos > 0)
        {
            validity = LooseCheckReplacementL(selection.iCursorPos-1, selection.iCursorPos, KNullDesC);
        }
        else
        {
            validity = KErrArgument;
        }
    }
    // Ctrl+Minus: non-breaking hyphen
    else if (aKeyEvent.iCode=='-' && (modifiers & EModifierCtrl))
    {
        validity = LooseCheckReplacementL(selStart, selEnd, CEditableText::ENonBreakingHyphen);
    }
    // Shift+Space: non-breaking space
    else if (aKeyEvent.iCode == ' ' && (modifiers & EModifierShift))
    {
        validity = LooseCheckReplacementL(selStart, selEnd, CEditableText::ENonBreakingSpace);
    }
    // Regular character
    else if (aKeyEvent.iCode == EKeyTab || TChar(aKeyEvent.iCode).IsPrint())
    {
        validity = LooseCheckReplacementL(selStart, selEnd, aKeyEvent.iCode);
    }
    // Other (navigation mostly)
    else
    {
        validity = KErrNone;
        notify   = EFalse;
    }

    if (validity == KErrNone)
    {
        ForwardKeyEventL(aKeyEvent, aType);
        if (notify)
        {
            iDisplay.PostModifyEventL(iPeer);
        }

    }
}

// --------------------------------------------------------------------------------
// Virtual methods from MSwtConstrainedText
// --------------------------------------------------------------------------------
//
MSwtControl* CSwtConstrainedText::Control()
{
    return this;
}

// --------------------------------------------------------------------------------
//  This is actually the 3rd-time constructor. This is where the actual editor
//  is constructed (couldn't be before we set the constraint).
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::SetConstraintL(TConstraint aConstraint)
{
    ASSERT(iConstraint == 0);
    ASSERT(aConstraint != 0);


    // Construct the control
    CEikEdwin* editor = NULL;

    switch (aConstraint)
    {
    case ENumeric:
    {
        CSwtIntegerEdwin* ed = new(ELeave)CSwtIntegerEdwin;
        editor = ed;
        SetEditor(editor);
        ed->ConstructL(-KMaxTInt, KMaxTInt, iTextLimit);
        Editor().SetTextLimit(iTextLimit);
        Editor().SetAknEditorCase(EAknSCTNumeric);
        Editor().SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
        Editor().SetAknEditorInputMode(EAknEditorNumericInputMode);
        Editor().SetAknEditorCurrentInputMode(EAknEditorNumericInputMode);
        Editor().SetAknEditorFlags(EAknEditorFlagFixedCase | EAknEditorFlagNoT9);
        Editor().SetAknEditorNumericKeymap(EAknEditorPlainNumberModeKeymap);
#ifdef RD_SCALABLE_UI_V2
        Editor().SetAknEditorFlags(EAknEditorFlagUseSCTNumericCharmap
                                   | EAknEditorFlagDeliverVirtualKeyEventsToApplication);
#else
        Editor().SetAknEditorFlags(EAknEditorFlagUseSCTNumericCharmap);
#endif // RD_SCALABLE_UI_V2 
        Editor().SetAknEditorSpecialCharacterTable(R_ESWT_CONSTRAINEDTEXT_NUMERIC_MODE_DIALOG);
    }
    break;
    case EDecimal:
    {
        CSwtFloatingPointEdwin* fpe = new(ELeave) CSwtFloatingPointEdwin;
        editor = fpe;
        SetEditor(editor);
        fpe->ConstructL(-KMaxTInt, KMaxTInt, iTextLimit);
        Editor().SetAknEditorCase(EAknSCTNumeric);
        Editor().SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
        Editor().SetAknEditorInputMode(EAknEditorNumericInputMode);
        Editor().SetAknEditorCurrentInputMode(EAknEditorNumericInputMode);
        Editor().SetAknEditorFlags(EAknEditorFlagFixedCase | EAknEditorFlagNoT9);
        Editor().SetAknEditorNumericKeymap(EAknEditorPlainNumberModeKeymap);
#ifdef RD_SCALABLE_UI_V2
        Editor().SetAknEditorFlags(EAknEditorFlagUseSCTNumericCharmap
                                   | EAknEditorFlagDeliverVirtualKeyEventsToApplication);
#else
        Editor().SetAknEditorFlags(EAknEditorFlagUseSCTNumericCharmap);
#endif // RD_SCALABLE_UI_V2 
        if (iDecimalSeparator == ',')
        {
            Editor().SetAknEditorSpecialCharacterTable(R_ESWT_CONSTRAINEDTEXT_DECIMAL_MODE_WITH_COMMA_DIALOG);
        }
        else
        {
            Editor().SetAknEditorSpecialCharacterTable(R_ESWT_CONSTRAINEDTEXT_DECIMAL_MODE_DIALOG);
        }
    }
    break;
    case EPhoneNumber:
    {
        const TUint flags = CEikEdwin::ESegmentedStorage | CEikEdwin::ELineCursor
                            | CEikEdwin::EInclusiveSizeFixed | CEikEdwin::ENoAutoSelection
                            | CEikEdwin::EResizable  | CEikEdwin::EWidthInPixels
                            | CEikEdwin::ENoWrap;
        editor = new(ELeave) CSwtEdwin;
        SetEditor(editor);
        editor->ConstructL(flags, 0, iTextLimit, 1);
        editor->SetAknEditorCase(EAknSCTNumeric);
        editor->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
        editor->SetAknEditorInputMode(EAknEditorNumericInputMode);
        editor->SetAknEditorNumericKeymap(EAknEditorAlphanumericNumberModeKeymap);
#ifdef RD_SCALABLE_UI_V2
        editor->SetAknEditorFlags(EAknEditorFlagUseSCTNumericCharmap
                                  | EAknEditorFlagDeliverVirtualKeyEventsToApplication);
#else
        editor->SetAknEditorFlags(EAknEditorFlagUseSCTNumericCharmap);
#endif // RD_SCALABLE_UI_V2            
        editor->SetAknEditorSpecialCharacterTable(R_ESWT_CONSTRAINEDTEXT_PHONENUMBER_MODE_DIALOG);
    }
    break;
    default:
        ASSERT(EFalse);
        break;
    }

    editor->SetContainerWindowL(*this);

    editor->CreateTextViewL();
    editor->MakeVisible(IsVisible());
    editor->SetDimmed(IsDimmed());
    editor->SetPosition(iPosition);
    editor->AddEdwinObserverL(this);
    ActivateL();

    RetrieveDefaultFontL();
    FixPadding();

    iConstraint = aConstraint;

    // Needs to be done after the ActivateL method.
    InitControlBaseL();
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::GetTextL
// --------------------------------------------------------------------------------
//
HBufC* CSwtConstrainedText::GetTextL() const
{
    HBufC* buf = Editor().GetTextInHBufL();
    if (!buf)
    {
        buf = HBufC::NewL(0);
    }
    // convert decimal from localized to MIDlet-visible representation
    if (iConstraint & EDecimal)
    {
        ConvertFromLocalizedDecimal(buf);
    }
    return buf;
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::SetTextL
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::SetTextL(const TDesC& aText)
{
    TBool val;
    if (iConstraint & EDecimal)
    {
        HBufC* buf = ConvertToLocalizedDecimalLC(aText);
        TPtr ptr = buf->Des();
        User::LeaveIfError(StrictValidateText(ptr));
        val = CSwtTextBase::SetTextL(ptr);
        CleanupStack::PopAndDestroy(buf);
    }
    else
    {
        User::LeaveIfError(StrictValidateText(aText));
        val = CSwtTextBase::SetTextL(aText);
    }
    return val;
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::AppendL
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::AppendL(const TDesC& aText)
{
    const TInt len = Editor().TextLength();

    // Ensure content will be valid
    HBufC* buf = HBufC::NewLC(len + aText.Length());
    TPtr ptr(buf->Des());
    Editor().GetText(ptr);

    TBool val;
    if (iConstraint & EDecimal)
    {
        HBufC* afterLocDec = ConvertToLocalizedDecimalLC(aText);
        TPtr locPtr = afterLocDec->Des();
        ptr.Append(locPtr);

        User::LeaveIfError(StrictValidateText(*buf));

        val = CSwtTextBase::AppendL(locPtr);
        CleanupStack::PopAndDestroy(afterLocDec);
    }
    else
    {
        ptr.Append(aText);
        User::LeaveIfError(StrictValidateText(*buf));
        val = CSwtTextBase::AppendL(aText);
    }
    CleanupStack::PopAndDestroy(buf);

    return val;
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::InsertL
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::InsertL(const TDesC& aText)
{
    TBool val;
    if (iConstraint & EDecimal)
    {
        HBufC* buf = ConvertToLocalizedDecimalLC(aText);
        TPtr ptr = buf->Des();
        User::LeaveIfError(StrictCheckReplacementL(ptr));
        val = CSwtTextBase::InsertL(ptr);
        CleanupStack::PopAndDestroy(buf);
    }
    else
    {
        User::LeaveIfError(StrictCheckReplacementL(aText));
        val = CSwtTextBase::InsertL(aText);
    }
    return val;
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::GetCharCount
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::GetCharCount() const
{
    return Editor().TextLength();
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::SetTextLimit
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::SetTextLimitL(TInt aLimit)
{
    iTextLimit = aLimit;
    return CSwtTextBase::SetLimitL(aLimit);
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::GetCaretPosition
// --------------------------------------------------------------------------------
//
TInt CSwtConstrainedText::GetCaretPosition() const
{
    return Editor().CursorPos();
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::SetSelectionL
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::SetSelectionL(TInt aStart, TInt aEnd)
{
    const TInt len = Editor().TextLength();
    if (aStart>len || aEnd>len)
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    Editor().SetSelectionL(aEnd, aStart);
    // This is neccessary, since CEikEdwin doesn't really set the selection
    // until drawing occurs and clipboard operations can be
    // broken in such a case
    Editor().TextView()->SetSelectionL(TCursorSelection(aEnd, aStart));
}

// --------------------------------------------------------------------------------
// CSwtConstrainedText::ClearSelectionL
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::ClearSelectionL()
{
    TCursorSelection selection(Editor().Selection());
    Editor().SetSelectionL(selection.iCursorPos, selection.iCursorPos);
    Editor().TextView()->SetSelectionL(TCursorSelection(selection.iCursorPos,
                                       selection.iCursorPos));
}

// --------------------------------------------------------------------------------
//   CSwtConstrainedText::SelectAllL
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::SelectAllL()
{
    const TInt length = Editor().TextLength();
    Editor().SetSelectionL(length, 0);
    Editor().TextView()->SetSelectionL(TCursorSelection(length, 0));
}

// --------------------------------------------------------------------------------
//   CSwtConstrainedText::ShowSelectionL
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::ShowSelectionL()
{
    TCursorSelection selection(Editor().Selection());
    Editor().SetSelectionL(selection.iCursorPos, selection.iAnchorPos);
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::CutL
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::CutL()
{
    const TCursorSelection selection(Editor().Selection());
    const TInt selLength = selection.Length();

    // Ensure content will be valid
    if (selLength != 0)
    {
        HBufC* text = Editor().GetTextInHBufL();
        TPtr ptr(text->Des());
        ptr.Delete(selection.LowerPos(), selLength);
        TInt error = StrictValidateText(ptr);
        delete text;
        User::LeaveIfError(error);
    }

    return CSwtTextBase::CutL();
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::CopyL
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::CopyL()
{
    Editor().ClipboardL(CEikEdwin::ECopy);
}

// --------------------------------------------------------------------------------
//CSwtConstrainedText::PasteL
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::PasteL()
{
    // Ensure content will be valid
    TPtrC content(SwtControlHelper::GetClipboardTextContentLC());
    TInt clipboardLength = content.Length();
    User::LeaveIfError(StrictCheckReplacementL(content));
    if (content.Ptr())
    {
        CleanupStack::PopAndDestroy();
    }


    if (clipboardLength == 0)
    {
        // Nothing to paste: CEikEdwin displays a message and leaves. We want
        // the message but not the leaving, hence the trap.
        TRAP_IGNORE(Editor().ClipboardL(CEikEdwin::EPaste));
        return EFalse;
    }

    Editor().ClipboardL(CEikEdwin::EPaste);
    return ETrue;
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::GetEditable
// --------------------------------------------------------------------------------
//
TBool CSwtConstrainedText::GetEditable() const
{
    return CSwtTextBase::GetEditable();
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::SetEditable
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::SetEditable(TBool aEditable)
{
    CSwtTextBase::SetEditable(aEditable);
}

// --------------------------------------------------------------------------------
//  CSwtConstrainedText::HandleEdwinEventL
// --------------------------------------------------------------------------------
//
void CSwtConstrainedText::HandleEdwinEventL(CEikEdwin */*aEdwin*/, TEdwinEvent aEventType)
{
    switch (aEventType)
    {
    case EEventTextUpdate:
    {
        TInt textLength = Editor().TextLength();
        TInt caretPos = GetCaretPosition();
        if ((iConstraint & ENumeric ||
                iConstraint & EDecimal) && (textLength > 0 && caretPos > 0))
        {

            CPlainText* editorText = Editor().Text();
            TPtrC ptr(editorText->Read(0));

            TBool valid = ptr[caretPos - 1] == iDecimalSeparator;
            //'-' is not allowed when limit is 1 or in the middle of the text in
            // both Numeric and Decimal
            if (ptr[caretPos - 1] == '-'
                    && (caretPos > 1 || iTextLimit == 1))
            {
                editorText->DeleteL(caretPos - 1, 1);
                caretPos--;
            }
            //'.' is not allowed when more than one dot or limit is 1 or
            // first character is '-' and limit is 2 in Decimal style
            if (iConstraint & EDecimal)
            {
                if (valid)
                {
                    if (iTextLimit == 1 || (ptr[0] == '-' && iTextLimit == 2) ||
                            caretPos-1 != ptr.LocateF(iDecimalSeparator) ||
                            caretPos-1 != ptr.LocateReverseF(iDecimalSeparator))
                    {
                        editorText->DeleteL(caretPos - 1, 1);
                        caretPos--;
                    }
                }
            }
            Editor().HandleTextChangedL();
            Editor().SetSelectionL(caretPos, caretPos);
        }
    }
    break;

    default:
        break;
    }
}
