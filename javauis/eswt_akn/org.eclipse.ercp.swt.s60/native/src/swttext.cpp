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


#include <EIKCOCTL.rsg>
#include <AknUtils.h>
#include <eikedwin.h>
#include <AknNumEdwin.h>
#include <eikfpne.h>
#include "swttext.h"
#include "swtcontrolhelper.h"
#include "swtscrollbar.h"
#include "swtedwin.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tagma_internal.h>
#endif

static const TText KDefaultSecretChar = '*';

#define KInputModesRequiringFeedbackWithSingleChar (EAknEditorTextInputMode | EAknEditorKatakanaInputMode | EAknEditorFullWidthTextInputMode | EAknEditorFullWidthKatakanaInputMode | EAknEditorHiraganaKanjiInputMode | EAknEditorHiraganaInputMode | EAknEditorHalfWidthTextInputMode)
#define KInputModesRequiringFeedbackWithString (EAknEditorHiraganaInputMode | EAknEditorHiraganaKanjiInputMode)

const TInt KPasswordCharTimeout = 700000; // 700 msecs


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::CPasswordText
// ---------------------------------------------------------------------------
//
inline CSwtText::CPasswordText::CPasswordText(
    const CParaFormatLayer* aParaLayer, const CCharFormatLayer* aCharLayer,
    CEikEdwin* aEdwin, TBool aPswdEnable)
        : CGlobalText(aParaLayer, aCharLayer)
        , iEchoChar(0), iEditor(aEdwin), iClearCharPos(-1), iPswdEnable(aPswdEnable)
{
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::~CPasswordText
// ---------------------------------------------------------------------------
//
CSwtText::CPasswordText::~CPasswordText()
{
    delete iTimer;
    delete iClearText;
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::ConstructL
// ---------------------------------------------------------------------------
//
inline void CSwtText::CPasswordText::ConstructL()
{
    CGlobalText::ConstructL(ESegmentedStorage, EDefaultTextGranularity);

    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iClearText = HBufC::NewL(CEditableText::EDefaultTextGranularity);
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::NewL
// ---------------------------------------------------------------------------
//
CSwtText::CPasswordText* CSwtText::CPasswordText::NewL(
    const CParaFormatLayer* aParaLayer,
    const CCharFormatLayer* aCharLayer, CEikEdwin* aEdwin, TBool aPswdEnable)
{
    CPasswordText* self = new(ELeave) CPasswordText(
        aParaLayer, aCharLayer, aEdwin, aPswdEnable);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::SaveClearTextL
// We save our clear text. We reallocate if not enough memory.
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::SaveClearTextL(TInt aPos, const TDesC& aBuf)
{
    ASSERT(iClearText);
    TPtr ptr(iClearText->Des());

    TInt totalLength = ptr.Length() + aBuf.Length();
    //
    if (totalLength > ptr.MaxLength())
    {
        iClearText = iClearText->ReAllocL(totalLength);
        ptr.Set(iClearText->Des());
    }

    ptr.Insert(aPos,aBuf);
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::Reset
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::Reset()
{
    if (iPswdEnable)
    {
        if (iTimer->IsActive())
        { //commit any previous character before deleting or else we
            // may crash in the timer callback when trying to replace
            // an uncommited but already deleted char
            TRAP_IGNORE(DoTimerCallbackL(EFalse));
        }
    }
    // clear local (hidden) copy
    TPtr ptr = iClearText->Des();
    ptr.SetLength(0);
    CGlobalText::Reset();
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::InsertL
// Insert a single character. First we save the character in the clear text by
// calling SaveClearTextL(). Then if the character is a paragraph delimiter
// insert a space.Otherwise if we are in alpha mode insert the clear
// character and start the timer. Otherwise insert the asterisks.
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::InsertL(TInt aPos, const TChar& aChar)
{
    TBuf<1> buf;
    buf.Append(aChar);
    SaveClearTextL(aPos, buf);
    if (iPswdEnable)
    {
        if (aChar == CEditableText::EParagraphDelimiter)
        {
            CGlobalText::InsertL(aPos, ' ');
        }
        else if (iEditor->AknEditorCurrentInputMode() &
                 KInputModesRequiringFeedbackWithSingleChar)
        {
            if (iTimer->IsActive() && iClearCharPos != aPos)
            { //commit any previous character in a different position
                DoTimerCallbackL(ETrue);
            }
            else
            { //or restart the timer if the char is in the same
                //position (multi-tap looping)
                iTimer->Cancel();
            }

            CGlobalText::InsertL(aPos, aChar);
            iClearCharPos = aPos;
            iTimer->Start(KPasswordCharTimeout, KPasswordCharTimeout,
                          TCallBack(TimerCallback, this));
        }
        else
        {
            CGlobalText::InsertL(aPos, iEchoChar);
        }
    }
    else
    {
        if (iEchoChar)
        {
            CGlobalText::InsertL(aPos, iEchoChar);
        }
        else
        {
            CGlobalText::InsertL(aPos, aChar);
        }
    }

}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::InsertL
// Insert one or more characters. If inserting only one character rely
// on InsertL() accepting only one char. Otherwise, save the clear text by
// calling SaveClearTextL(). Then for every character either insert an asterisk
// or a space if the character is a space delimiter. Note: this method is
// called at start-up so it makes no sense to show a temporary clear character.
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::InsertL(TInt aPos,const TDesC& aBuf)
{
    if (iPswdEnable)
    {

        if (aBuf.Length() == 1)
        {
            CSwtText::CPasswordText::InsertL(aPos, aBuf[0]);
        }
        else if (aBuf.Length() > 1 && (iEditor->AknEditorCurrentInputMode() &
                                       KInputModesRequiringFeedbackWithString))
        {
            // When writing in kanji or hiragana input mode, the characters are
            // entered in sequences. Insert the sequence char by char so that
            // the user will see the last character temporarily for feedback.
            for (TInt i = 0; i < aBuf.Length(); i++)
            {
                CSwtText::CPasswordText::InsertL(aPos + i, aBuf[i]);
            }
        }
        else
        {
            SaveClearTextL(aPos, aBuf);

            for (TInt i = 0; i < aBuf.Length(); i++)
            {
                if (aBuf[i] == CEditableText::EParagraphDelimiter)
                {
                    CGlobalText::InsertL(aPos + i, ' ');
                }
                else
                {
                    CGlobalText::InsertL(aPos + i, iEchoChar);
                }
            }
        }
    }
    else
    {
        if (iEchoChar)
        {
            SaveClearTextL(aPos, aBuf);
            for (TInt i = 0; i < aBuf.Length(); i++)
            {
                if (aBuf[i] == CEditableText::EParagraphDelimiter)
                {
                    CGlobalText::InsertL(aPos + i, ' ');
                }
                else
                {
                    CGlobalText::InsertL(aPos + i, iEchoChar);
                }
            }
        }
        else
        {
            for (TInt i = 0; i < aBuf.Length(); i++)
            {
                CSwtText::CPasswordText::InsertL(aPos + i, aBuf[i]);
            }
        }

    }

}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::SetEchoCharL
// Support Changin all characters to masked character when we set the
// SetEchoCharater
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::SetEchoCharL()
{
    TInt length = iClearText->Des().Length();
    if (length > 0)
    {
        CGlobalText::DeleteL(0,length);
        if (iEchoChar)
        {
            for (int i = 0; i < length; i++)
            {
                CGlobalText::InsertL(i, iEchoChar);
            }
        }
        else
        {
            CGlobalText::InsertL(0 , iClearText->Des());
        }
        iEditor->NotifyNewFormatL();
    }
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::PasteFromStoreL
// Support for pasting in the Text with EchoCharacter. The normal insertion
// mechanism is not used in the paste operation, so it must be handled separately
// here. This is an overriden method that stores the pasted chars to the local
// store and puts masked chars (*) to the visible store (in super class).
// ---------------------------------------------------------------------------
//
TInt CSwtText::CPasswordText::PasteFromStoreL(const CStreamStore& aStore,
        const CStreamDictionary& aDictionary,
        TInt aPos)
{
    // 1. Paste clipboard text (to visible text)
    TInt numCharsPasted = CGlobalText::PasteFromStoreL(aStore, aDictionary, aPos);
    // 2. Insert the pasted text to the local clear text store
    TPtr content(SwtControlHelper::GetClipboardTextContentLC());
    TPtrC ptr = content.Ptr();
    SaveClearTextL(aPos, ptr);

    if (iEchoChar)
    {
        // 3. Mask the visible text
        for (int i = 0; i < numCharsPasted; i++)
        {
            TPtrC charToProcess = Read(aPos+i, 1);
            TChar replaceChar   = iEchoChar;
            if (charToProcess[0] == CEditableText::EParagraphDelimiter)
            {
                replaceChar = ' ';
            }
            CGlobalText::DeleteL(aPos+i,1);
            CGlobalText::InsertL(aPos+i, replaceChar);
        }
    }
    if (content.Ptr())
    {
        CleanupStack::PopAndDestroy();
    }
    return numCharsPasted;
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::CopyToStoreL
// Support for copying Text with EchoCharacter constraint. The normal mechanism
// is not used in the copy operation, so it must be handled separately here.
// This is an overriden method that stores the copied chars to the local store
// and puts masked chars (*) to the visible store (in super class).
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::CopyToStoreL(CStreamStore& aStore,
        CStreamDictionary& aDictionary,
        TInt aPos,
        TInt aLength) const
{
    if (!iEchoChar)
    {
        // this allow to copy in single line text without
        // Echo Character set
        CGlobalText::CopyToStoreL(aStore, aDictionary,
                                  aPos, aLength);
    }
    else
    {
        // here we should handle the situation that it shouldn't
        // delete already existing clipboard contents.
    }
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::Delete
// Delete one or more chars, this makes sure our clear text stays
// in sync.
// ---------------------------------------------------------------------------
//
TBool CSwtText::CPasswordText::DeleteL(TInt aPos,TInt aLength)
{
    if (iPswdEnable)
    {
        if (iTimer->IsActive())
        { //commit any previous character before deleting or else we
            // may crash in the timer callback when trying to replace
            // an uncommited but already deleted char
            DoTimerCallbackL(EFalse);
        }
    }
    TPtr ptr = iClearText->Des();
    ptr.Delete(aPos, aLength);

    return CGlobalText::DeleteL(aPos, aLength);
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::Extract
// This method is called by edwin when returning text to the user via
// a GetText() call. We make sure we return the clear text instead of the
// masking used by edwin.
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::Extract(TDes& aBuf,TInt aPos) const
{
    if (iPswdEnable || iEchoChar)
    {
        aBuf.SetLength(0);
        aBuf.Append(iClearText->Mid(aPos));
    }
    else
    {
        CGlobalText::Extract(aBuf, aPos);
    }
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::Extract
// This method is called by edwin when returning text to the user via
// a GetText() call. We make sure we return the clear text instead of the
// masking used by edwin.
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::Extract(TDes& aBuf,
                                      TInt aPos,
                                      TInt aLength) const
{
    if (iPswdEnable || iEchoChar)
    {
        aBuf.SetLength(0);
        aBuf.Append(iClearText->Mid(aPos, aLength));
    }
    else
    {
        CGlobalText::Extract(aBuf, aPos, aLength);
    }
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::TimerCallbackL
// ---------------------------------------------------------------------------
//
TInt CSwtText::CPasswordText::TimerCallback(TAny* aThis)
{
    CPasswordText* self = STATIC_CAST(CPasswordText*, aThis);
    ASSERT(self);

    TRAPD(err, self->DoTimerCallbackL(ETrue));
    return err;
}

// ---------------------------------------------------------------------------
// CSwtText::CPasswordText::DoTimerCallbackL
// It is time to remove the temporary clear character. Do a delete at
// iClearCharPos and then insert an asterisk at this position. Then we need to
// call something that signals the edwin to update its characters,
// NotifyNewFormatL() seems to do the job. Note: this is only needed when inse-
// rting a char from the special char table or when holding a key down so that
// a number is inserted even if in alpha input mode. Otherwise you do not notice
// the differece as edwin update its text anyway when commiting a fep transaction
// after a timeout that seems just about longer than our timeout.
// ---------------------------------------------------------------------------
//
void CSwtText::CPasswordText::DoTimerCallbackL(TBool aNotifyFormat)
{

    iTimer->Cancel();

    ASSERT(iClearCharPos != -1 && (DocumentLength() > iClearCharPos));
    // here checking iEchochar is for null character in iEchoChar that does mask
    // the characters so we dont need to insert or delete.
    if (iClearCharPos != -1 && (DocumentLength() > iClearCharPos) && iEchoChar)
    {
        CGlobalText::DeleteL(iClearCharPos,1);
        CGlobalText::InsertL(iClearCharPos, iEchoChar);

        if (aNotifyFormat && iEditor->TextView())
        {
            iEditor->NotifyNewFormatL();
        }
        iEditor->DrawDeferred();
    }
}

// CSwtText::~CSwtText
// ---------------------------------------------------------------------------
//
CSwtText::~CSwtText()
{
    //iiBufferToVerify.Close();
}

// ---------------------------------------------------------------------------
// CSwtText::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtText::ConstructL()
{
    CSwtTextBase::ConstructL();
    TUint aknFlags = 0;
    // Compute edwin flags from our style
    TUint flags = CEikEdwin::ESegmentedStorage | CEikEdwin::ELineCursor
                  | CEikEdwin::EInclusiveSizeFixed | CEikEdwin::ENoAutoSelection
                  | CEikEdwin::EResizable  | CEikEdwin::EWidthInPixels;
    if (!(iStyle & KSwtStyleWrap))
    {
        flags |= CEikEdwin::ENoWrap;
    }

    if (iStyle & KSwtStyleReadOnly)
    {
        flags |= CEikEdwin::EReadOnly;
    }

    // Single/multi - line
    TInt lineCount;
    if (iStyle & KSwtStyleSingle)
    {
        flags |= CEikEdwin::ENoLineOrParaBreaks | CEikEdwin::ENoWrap;
        lineCount = 1;
    }
    else
    {
        lineCount = 0;
    }

    // Construct the editor
    CSwtEdwin* editor = new(ELeave) CSwtEdwin;
    SetEditor(editor);
    editor->ConstructL(flags, 0, ETextLimit, lineCount);
    editor->SetContainerWindowL(*this);

    if (iStyle & KSwtStyleSingle)
    {
        CParaFormatLayer* paraLayer = iEikonEnv->SystemSingleLineParaFormatLayerL();
        CCharFormatLayer* charLayer = iEikonEnv->SystemCharFormatLayerL();
        CPasswordText* passwordText = CPasswordText::NewL(
                                          paraLayer, charLayer, editor, iStyle & KSwtStylePassword);
        delete editor->Text();
        // Ownership of passwordText is transferred
        editor->SetDocumentContentL(*passwordText, CEikEdwin::EUseText);

    }
    editor->CreateTextViewL();
    editor->SetDimmed(IsDimmed());
    editor->AddEdwinObserverL(this);

    ActivateL();

    // Password: default to echo char = '*'
    if (iStyle & KSwtStylePassword)
    {
        ASSERT(iStyle & KSwtStyleSingle);
        static_cast < CPasswordText* >(Editor().Text())->
        iEchoChar = KDefaultSecretChar;
        editor->SetAknEditorCase(EAknEditorLowerCase);
        editor->SetAknEditorCurrentCase(EAknEditorLowerCase);
        editor->EnableCcpuSupportL(EFalse);
        aknFlags |= EAknEditorFlagNoT9;
        // Based on MIDP constraints, checks if the text is a PASSWORD text
        TCoeInputCapabilities inputCapabilities = editor->InputCapabilities();
        if (inputCapabilities != TCoeInputCapabilities::ENone)
        {
            CSwtLafFacade::SetPasswordCapability(inputCapabilities);
        }
    }
#ifdef RD_SCALABLE_UI_V2
    aknFlags |= EAknEditorFlagDeliverVirtualKeyEventsToApplication;
#endif // RD_SCALABLE_UI_V2 

    editor->SetAknEditorFlags(aknFlags);

    // Scrollbars
    if (iStyle & (KSwtStyleHScroll | KSwtStyleVScroll))
    {
        editor->CreatePreAllocatedScrollBarFrameL();

        CEikScrollBarFrame* sbFrame = editor->ScrollBarFrame();
#ifdef RD_SCALABLE_UI_V2
        // WARNING!!! The expanded touch area does not move correctly togehter with the scrollbars!
        sbFrame->SetScrollBarFrameFlags(CEikScrollBarFrame::EDisableExpandedTouchArea);
#endif // RD_SCALABLE_UI_V2

        sbFrame->CreateDoubleSpanScrollBarsL(EFalse, EFalse);
        CEikScrollBarFrame::TScrollBarVisibility horizVis =
            /*( iStyle & KSwtStyleHScroll ) ?
            CEikScrollBarFrame::EOn : */
            CEikScrollBarFrame::EOff;
        CEikScrollBarFrame::TScrollBarVisibility vertVis =
            (iStyle & KSwtStyleVScroll) ?
            CEikScrollBarFrame::EOn :
            CEikScrollBarFrame::EOff;
        sbFrame->SetScrollBarVisibilityL(horizVis, vertVis);
    }
    //To avoid editor cursor to blink
    //if the user doesnt set size and position
    editor->SetPosition(TPoint(0,0));
    editor->SetSize(TSize(0,0));
    SetAlignmentL();
    RetrieveDefaultFontL();
    FixPadding();
}

// ---------------------------------------------------------------------------
// CSwtText::NewL
// ---------------------------------------------------------------------------
//
CSwtText* CSwtText::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                         MSwtComposite& aParent, TInt aStyle)
{
    CSwtText* self = new(ELeave) CSwtText(
        aDisplay, aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtText::Draw
// ---------------------------------------------------------------------------
//
void CSwtText::Draw(const TRect& /*aRect*/) const
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
        ((CSwtEdwin*)&editor)->SetClippingRect(clipRect);
        iLastViewVisibleRect = clipRect;
    }
}

// ---------------------------------------------------------------------------
// CSwtText::ComputeAlignment
// ---------------------------------------------------------------------------
//
CParaFormat::TAlignment CSwtText::ComputeAlignment() const
{
    CParaFormat::TAlignment result;
    const TInt alignment = iStyle  & KSwtAlignmentMask;

    if (alignment == KSwtStyleCenter)
    {
        result = CParaFormat::ECenterAlign;
    }
    else if (alignment == KSwtStyleTrail)
    {
        if ((iStyle & KSwtOrientationMask) == KSwtStyleRightToLeft
                || AknLayoutUtils::LayoutMirrored())
        {
            result = CParaFormat::ELeftAlign;
        }
        else
        {
            result = CParaFormat::ERightAlign;
        }
    }
    else
    {
        ASSERT(alignment == KSwtStyleLead);
        if ((iStyle & KSwtOrientationMask) == KSwtStyleRightToLeft
                || AknLayoutUtils::LayoutMirrored())
        {
            result = CParaFormat::ERightAlign;
        }
        else
        {
            result = CParaFormat::ELeftAlign;
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// CSwtText::SetAlignmentL
// ---------------------------------------------------------------------------
//
void CSwtText::SetAlignmentL()
{
    CParaFormat* paraFormat = CParaFormat::NewL();
    CleanupStack::PushL(paraFormat);
    paraFormat->iHorizontalAlignment = ComputeAlignment();
    TParaFormatMask paraMask;
    paraMask.SetAttrib(EAttAlignment);
    static_cast < CGlobalText* >(Editor().Text())->
    ApplyParaFormatL(paraFormat, paraMask, 0, 0);
    CleanupStack::PopAndDestroy(paraFormat);
}

// ---------------------------------------------------------------------------
// CSwtText::CropToFirstLine
// ---------------------------------------------------------------------------
//
void CSwtText::CropToFirstLine(TDes& aText)
{
    const TInt length = aText.Length();
    const TText* ptr = aText.Ptr();
    TInt i = 0;

    // As of Unicode 4.0.1 there is only one character in each of the line
    // or paragraph separator categories. Therefore, if speed was an issue,
    // the below code could be replaced by a simpler and faster test against
    // those two characters. The current code, however, is more future proof.
    for (i = 0; i < length; i++)
    {
        const TText ch = ptr[i];

        // CR or LF
        if (ch == 0x0D || ch == 0x0A)
        {
            break;
        }

        const TChar::TCategory category = TChar(ch).GetCategory();
        if (category == TChar::EZlCategory
                || category == TChar::EZpCategory)
        {
            break;
        }
    }

    aText.SetLength(i);
}


// ---------------------------------------------------------------------------
// CSwtText::ConvertToUnicodeBreaks
// ---------------------------------------------------------------------------
//
void CSwtText::ConvertToUnicodeBreaks(TDes& aText)
{
    const TText KCr = 0x0D; // CR
    const TText KLf = 0x0A; // LF

    const TInt length = aText.Length();
    TText* ptr = const_cast < TText* >(aText.Ptr());

    // First loop: just scans for CR & LF and stops at the first occurrence.
    // This loop is faster than the second one.
    TInt inputIndex  = 0;
    for (; inputIndex < length;  ++ inputIndex)
    {
        if (ptr[inputIndex] == KCr || ptr[inputIndex] == KLf)
        {
            break;
        }
    }

    // Second loop: starts at the first CR or LF and does the actual replacement.
    // It will only be triggered if the first loop didn't reach the end.
    TText prevChar    = 0;
    TInt  outputIndex = inputIndex;
    for (; inputIndex < length;  ++ inputIndex)
    {
        TText ch = ptr[inputIndex];
        if (ch != KCr && ch != KLf)
        {
            ptr[outputIndex ++ ] = ch;
        }
        else
        {
            // Don't replace LF if preceded by CR, just skip it.
            if (!(ch == KLf && prevChar == KCr))
            {
                ptr[outputIndex ++ ] = CEditableText::EParagraphDelimiter;
            }
        }

        prevChar = ch;
    }

    aText.SetLength(outputIndex);
}

// ---------------------------------------------------------------------------
// CSwtText::ProcessLineBrakes
// ---------------------------------------------------------------------------
//
void CSwtText::ProcessLineBrakes(TDes& aText) const
{
    if (iStyle & KSwtStyleSingle)
    {
        CropToFirstLine(aText);
    }
    else
    {
        ConvertToUnicodeBreaks(aText);
    }
}

// ---------------------------------------------------------------------------
// CSwtText::PostVerifyEventL
// ---------------------------------------------------------------------------
//
void CSwtText::PostVerifyEventL(
    TInt aStart, TInt aEnd, TDes& aText)
{
    iDisplay.PostVerifyEventL(
        *this, iPeer, aStart, aEnd, aText);
}

// ---------------------------------------------------------------------------
// CSwtText::PostVerifyEventL
// ---------------------------------------------------------------------------
//
void CSwtText::PostVerifyEventL(TInt aStart, TInt aEnd, TInt aChar)
{
    TBuf<1> charString;
    charString.Append(aChar);
    PostVerifyEventL(aStart, aEnd, charString);
}

// ---------------------------------------------------------------------------
// CSwtText::SendCharacterL
// ---------------------------------------------------------------------------
//
void CSwtText::SendCharacterL(TText aChar)
{
    TKeyEvent keyEvent;

    // For some reason TChar::IsPrint() returns false for line separator and
    // paragraph separator, so we have to emulate them.
    if (aChar == CEditableText::EParagraphDelimiter)
    {
        keyEvent.iCode = EKeyEnter;
        keyEvent.iModifiers = 0;
    }
    else if (aChar == CEditableText::ELineBreak)
    {
        keyEvent.iCode = EKeyEnter;
        keyEvent.iModifiers = EModifierShift;
    }
    else
    {
        keyEvent.iCode = aChar;
        keyEvent.iModifiers = 0;
    }

    keyEvent.iScanCode = 0;
    keyEvent.iRepeats = 0;
    ForwardKeyEventL(keyEvent, EEventKey);
    iDisplay.PostModifyEventL(iPeer);
}

// ---------------------------------------------------------------------------
// CSwtText::HandleResourceChange
// From CSwtTextBase
// ---------------------------------------------------------------------------
//
void CSwtText::HandleResourceChange(TInt aType)
{
    CSwtTextBase::HandleResourceChange(aType);

    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        TRAP_IGNORE(
            TInt topIndex = 0;
            if (iStyle & KSwtStyleMulti)
    {
        // SetOrientationL method changes the TopIndex, we need to save it and set it again.
        topIndex = GetTopIndexL();
        }
        // Update text orientation (left-to-right / right-to-left)
        SetOrientationL(iStyle & KSwtOrientationMask);
        if (iStyle & KSwtStyleMulti)
    {
        SetTopIndexL(topIndex);
        }
        );
    }
}

// ---------------------------------------------------------------------------
// CSwtText::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtText::SizeChanged()
{
    CSwtTextBase::SizeChanged();
    Editor().TextLayout()->RestrictScrollToTopsOfLines(EFalse);
}

// ---------------------------------------------------------------------------
// CSwtText::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtText::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    TBool traversalDoIt = ETrue;
    CEikEdwin& editor = Editor();

#ifdef RD_SCALABLE_UI_V2
    // If focus gets changed by keys, iIgnorePointerDown is set to ETrue. We
    // don't want this since the next pointer event should be ignored only
    // when focus gets changed by tapping on the screen.
    iIgnorePointerDown = EFalse;
#endif //RD_SCALABLE_UI_V2

    switch (aKeyEvent.iCode)
    {
    case EKeyEnter:
        traversalDoIt = iStyle & KSwtStyleSingle;
        break;

    case EKeyEscape:
        traversalDoIt = EFalse;
        break;

    case EKeyTab:
        traversalDoIt = (iStyle & KSwtStyleMulti)
                        && (aKeyEvent.iModifiers & ECtrlFuncShiftMask);
        break;

    case EKeyDownArrow:
        traversalDoIt = (!(aKeyEvent.iModifiers & EModifierShift))
                        && (GetCaretLineNumber() == GetLineCount() - 1)
                        && (GetShell().FindTraversalTargetL(
                                ESwtTraverseArrowNext, *this));
        break;

    case EKeyUpArrow:
        traversalDoIt = (!(aKeyEvent.iModifiers & EModifierShift))
                        && (GetCaretLineNumber() == 0)
                        && (GetShell().FindTraversalTargetL(
                                ESwtTraverseArrowPrevious, *this));
        break;

    case EKeyLeftArrow:
    case EKeyRightArrow:
        traversalDoIt = EFalse;
        break;

    default:
        // traversalDoIt remains ETrue
        break;
    }

    /*
     * This is a workaround for issue with traversing between Text if cursor
     * was on first position and LEFT key was pressed, the event was
     * consumed somewhere in AknFep. The same problem was in case RIGHT key
     * and end cursor position. In this case we don't recieve EEventKey, so
     * we must simulate it.
     */
    if (aType == EEventKeyDown)
    {
        if (aKeyEvent.iScanCode == EStdKeyLeftArrow &&
                editor.CursorPos() == 0)
        {
            HandleKeyL(aKeyEvent, EEventKey, EFalse);
        }
        else if (aKeyEvent.iScanCode == EStdKeyRightArrow &&
                 editor.CursorPos() == editor.TextLength())
        {
            HandleKeyL(aKeyEvent, EEventKey, EFalse);
        }
    }

    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

// ---------------------------------------------------------------------------
// CSwtText::ProcessKeyEventL
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtText::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    CEikEdwin& editor = Editor();

    if (aType != EEventKey)
    {
        return;
    }

    // Read - only: CEikEdwin displays a message and leaves. We want the message
    // but not the leaving, hence the trap.
    if (iStyle & KSwtStyleReadOnly)
    {
        TRAP_IGNORE(ForwardKeyEventL(aKeyEvent, aType));
        return;
    }

    // Single - line: enter not consumed, translated to default selection event
    if ((iStyle & KSwtStyleSingle) && aKeyEvent.iCode == EKeyEnter)
    {
        iDisplay.PostDefaultSelectionEventL(iPeer);
        return;
    }

    const TInt modifiers = aKeyEvent.iModifiers & ECtrlFuncShiftMask;
    const TCursorSelection selection(editor.Selection());
    const TInt selStart = selection.LowerPos();
    const TInt selEnd = selection.HigherPos();
    const TInt selLength = selEnd  -  selStart;
    const TInt length = editor.TextLength();
    const TInt hotkey = HotKeyL(aKeyEvent);

    // Cut
    if (hotkey == CEikEdwin::EHotKeyCut)
    {
        TText echoChar = static_cast < CPasswordText* >(
                             Editor().Text())->iEchoChar;
        if (!(iStyle & KSwtStylePassword) || !(echoChar))
        {
            Editor().ClipboardL(CEikEdwin::ECopy);
            if (selLength != 0)
            {
                TPtr text(NULL, 0, 0);
                PostVerifyEventL(selStart, selEnd, text);
            }
        }
    }
    // Copy
    else if (hotkey == CEikEdwin::EHotKeyCopy)
    {
        TText echoChar = static_cast < CPasswordText* >(
                             Editor().Text())->iEchoChar;
        if (!(iStyle & KSwtStylePassword) || !(echoChar))
        {
            editor.ClipboardL(CEikEdwin::ECopy);
        }
    }
    // Paste
    else if (hotkey == CEikEdwin::EHotKeyPaste)
    {
        TPtr content(SwtControlHelper::GetClipboardTextContentLC());
        if (content.Length() == 0)
        {
            // Nothing to paste: CEikEdwin displays a message and leaves. We want
            // the message but not the leaving, hence the trap.
            TRAP_IGNORE(ForwardKeyEventL(aKeyEvent, aType));
        }
        else
        {
            PostVerifyEventL(selStart, selEnd, content);
        }
        if (content.Ptr())
        {
            CleanupStack::PopAndDestroy();
        }
    }
    // Undo
    else if (hotkey == CEikEdwin::EHotKeyUndo)
    {
        // Do nothing
    }
    // Delete
    else if (aKeyEvent.iCode == EKeyDelete && modifiers == 0)
    {
        TPtr text(NULL, 0, 0);
        if (selLength != 0)
        {
            PostVerifyEventL(selStart, selEnd, text);
        }
        else if (selection.iCursorPos < length)
        {
            PostVerifyEventL(selection.iCursorPos, selection.iCursorPos + 1, text);
        }
    }
    // Backspace
    else if (aKeyEvent.iCode == EKeyBackspace && modifiers == 0)
    {
        TPtr text(NULL, 0, 0);
        if (selLength != 0)
        {
            PostVerifyEventL(selStart, selEnd, text);
        }
        else if (selection.iCursorPos > 0)
        {
            PostVerifyEventL(selection.iCursorPos - 1, selection.iCursorPos, text);
        }
    }
    // Enter: paragraph separator
    else if (aKeyEvent.iCode == EKeyEnter && modifiers == 0)
    {
        PostVerifyEventL(selStart, selEnd, CEditableText::ELineBreak);
    }
    // Shift + Enter: line separator
    else if (aKeyEvent.iCode == EKeyEnter && (modifiers & EModifierShift))
    {
        PostVerifyEventL(selStart, selEnd, CEditableText::ELineBreak);
    }
    // Ctrl + Minus: non - breaking hyphen
    else if (aKeyEvent.iCode == '-' && (modifiers & EModifierCtrl))
    {
        PostVerifyEventL(selStart, selEnd, CEditableText::ENonBreakingHyphen);
    }
    // Shift + Space: non - breaking space
    else if (aKeyEvent.iCode == ' ' && (modifiers & EModifierShift))
    {
        PostVerifyEventL(selStart, selEnd, CEditableText::ENonBreakingSpace);
    }
    // Regular character
    else if (aKeyEvent.iCode == EKeyTab || TChar(aKeyEvent.iCode).IsPrint())
    {
        PostVerifyEventL(selStart, selEnd, aKeyEvent.iCode);
    }
    // Other
    else
    {
        ForwardKeyEventL(aKeyEvent, aType);
    }
}

// ---------------------------------------------------------------------------
// CSwtText::Scrollable
// From MSwtText
// ---------------------------------------------------------------------------
//
MSwtScrollable* CSwtText::Scrollable()
{
    return this;
}

// ---------------------------------------------------------------------------
// CSwtText::GetTextL
// From MSwtText
// ---------------------------------------------------------------------------
//
HBufC* CSwtText::GetTextL() const
{
    return Editor().GetTextInHBufL();
}

// ---------------------------------------------------------------------------
// CSwtText::GetTextRangeL
// From MSwtText
// ---------------------------------------------------------------------------
//
HBufC* CSwtText::GetTextRangeL(TInt aStart, TInt aEnd) const
{
    ASSERT(aStart <=aEnd);

    const TInt length = Editor().TextLength();
    if (length == 0)
    {
        return NULL;
    }

    TInt pos1 = Max(0,    aStart);
    TInt pos2 = Min(aEnd, length - 1);
    TInt count = pos2 - pos1 + 1;
    if (pos1 >= length || pos2 < 0 || count <= 0)
    {
        return NULL;
    }

    HBufC* buffer = HBufC::NewL(count);
    TPtr des(buffer->Des());
    Editor().Text()->Extract(des, pos1, count);
    return buffer;
}

// ---------------------------------------------------------------------------
// CSwtText::SetTextL
// From MSwtText
// ---------------------------------------------------------------------------
//
TBool CSwtText::SetTextL(TDes& aText)
{
    ProcessLineBrakes(aText);
    return CSwtTextBase::SetTextL(aText);
}

// ---------------------------------------------------------------------------
// CSwtText::AppendL
// From MSwtText
// ---------------------------------------------------------------------------
//
TBool CSwtText::AppendL(TDes& aText)
{
    ProcessLineBrakes(aText);
    return CSwtTextBase::AppendL(aText);
}

// ---------------------------------------------------------------------------
// CSwtText::InsertL
// From MSwtText
// ---------------------------------------------------------------------------
//
TBool CSwtText::InsertL(TDes& aText)
{
    ProcessLineBrakes(aText);
    return CSwtTextBase::InsertL(aText);
}

// ---------------------------------------------------------------------------
// CSwtText::GetCharCount
// From MSwtText
// ---------------------------------------------------------------------------
//
TInt CSwtText::GetCharCount() const
{
    return Editor().TextLength();
}

// ---------------------------------------------------------------------------
// CSwtText::SetTextLimit
// From MSwtText
// ---------------------------------------------------------------------------
//
TBool CSwtText::SetTextLimitL(TInt aLimit)
{
    iTextLimit = aLimit;
    return CSwtTextBase::SetLimitL(aLimit);
}

// ---------------------------------------------------------------------------
// CSwtText::GetCaretLineNumber
// From MSwtText
// ---------------------------------------------------------------------------
//
TInt CSwtText::GetCaretLineNumber() const
{
    TTmLineInfo info;
    TPoint origin(TPoint::EUninitialized);
    TInt width;
    TInt ascent;
    TInt descent;
    TTmDocPos curPos;
    Editor().TextView()->GetCursorPos(curPos);
    TTmDocPosSpec curPosSpec(curPos);
    Editor().TextLayout()->TagmaTextLayout().GetCursor(
        curPosSpec, ECursorVertical, info, origin, width, ascent, descent);
    return info.iLineNumber;
}

// ---------------------------------------------------------------------------
// CSwtText::GetCaretLocationL
// From MSwtText
// ---------------------------------------------------------------------------
//
TPoint CSwtText::GetCaretLocationL() const
{
    CTextView* textView = Editor().TextView();
    CTextLayout* layout   = Editor().TextLayout();

    TTmLineInfo info;
    TPoint curLoc;
    TInt width;
    TInt ascent;
    TInt descent;
    TTmDocPos curPos;
    textView->GetCursorPos(curPos);
    TTmDocPosSpec curPosSpec(curPos);
    layout->TagmaTextLayout().GetCursor(
        curPosSpec, ECursorVertical, info, curLoc, width, ascent, descent);
    TPoint result(TPoint::EUninitialized);
    layout->GetOrigin(result);
    result += curLoc;
    result += textView->ViewRect().iTl;
    result -= ClientRect().iTl;
    result.iY += GetTopPixelL();
    return result;
}

// ---------------------------------------------------------------------------
// CSwtText::GetCaretPosition
// From MSwtText
// ---------------------------------------------------------------------------
//
TInt CSwtText::GetCaretPosition() const
{
    return Editor().CursorPos();
}

// ---------------------------------------------------------------------------
// CSwtText::GetSelection
// From MSwtText
// ---------------------------------------------------------------------------
//
TPoint CSwtText::GetSelection() const
{
    const TCursorSelection selection(Editor().Selection());
    return TPoint(selection.LowerPos(), selection.HigherPos());
}

// ---------------------------------------------------------------------------
// CSwtText::GetSelectionTextL
// From MSwtText
// ---------------------------------------------------------------------------
//
HBufC* CSwtText::GetSelectionTextL() const
{
    const TCursorSelection selection(Editor().Selection());
    const TInt length = selection.Length();
    if (length == 0)
    {
        return NULL;
    }

    HBufC* buffer = HBufC::NewL(length);
    TPtr des(buffer->Des());
    Editor().Text()->Extract(des, selection.LowerPos(), length);
    return buffer;
}

// ---------------------------------------------------------------------------
// CSwtText::SetSelectionL
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::SetSelectionL(TInt aStart, TInt aEnd)
{
    const TInt length = Editor().TextLength();
    const TInt pos1 = Max(0, Min(aStart, length));
    const TInt pos2 = Max(0, Min(aEnd, length));
    const TInt cursor = Min(pos1, pos2);
    const TInt anchor = Max(pos1, pos2);
    Editor().SetSelectionL(cursor, anchor);
    // This is neccessary, since CEikEdwin doesn't really set the selection
    // until drawing occurs and clipboard operations can be
    // broken in such a case
    Editor().TextView()->SetSelectionL(TCursorSelection(cursor, anchor));
}

// ---------------------------------------------------------------------------
// CSwtText::ClearSelectionL
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::ClearSelectionL()
{
    TCursorSelection selection(Editor().Selection());
    Editor().SetSelectionL(selection.iCursorPos, selection.iCursorPos);
    Editor().TextView()->SetSelectionL(TCursorSelection(
                                           selection.iCursorPos, selection.iCursorPos));
}

// ---------------------------------------------------------------------------
// CSwtText::SelectAllL
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::SelectAllL()
{
    const TInt length = Editor().TextLength();
    Editor().SetSelectionL(length, 0);
    Editor().TextView()->SetSelectionL(TCursorSelection(length, 0));
}

// ---------------------------------------------------------------------------
// CSwtText::ShowSelectionL
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::ShowSelectionL()
{
    TCursorSelection selection(Editor().Selection());
    Editor().SetSelectionL(selection.iCursorPos, selection.iAnchorPos);
}

// ---------------------------------------------------------------------------
// CSwtText::CopyL
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::CopyL()
{
    if (!(iStyle & KSwtStylePassword))
    {
        Editor().ClipboardL(CEikEdwin::ECopy);
    }
}

// ---------------------------------------------------------------------------
// CSwtText::PasteL
// From MSwtText
// ---------------------------------------------------------------------------
//
TBool CSwtText::PasteL()
{
    TPtrC content(SwtControlHelper::GetClipboardTextContentLC());
    TInt length = content.Length();
    if (content.Ptr() != NULL)
    {
        CleanupStack::PopAndDestroy(); // content
    }
    if (length == 0)
    {
        // Nothing to paste: CEikEdwin displays a message and leaves.
        // We want the message but not the leaving, hence the trap.
        TRAP_IGNORE(Editor().ClipboardL(CEikEdwin::EPaste));
        return EFalse;
    }

    Editor().ClipboardL(CEikEdwin::EPaste);
    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtText::GetEditable
// From MSwtText
// ---------------------------------------------------------------------------
//
TBool CSwtText::GetEditable() const
{
    return CSwtTextBase::GetEditable();
}

// ---------------------------------------------------------------------------
// CSwtText::SetEditable
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::SetEditable(TBool aEditable)
{
    CSwtTextBase::SetEditable(aEditable);
}

// ---------------------------------------------------------------------------
// CSwtText::GetLineCount
// From MSwtText
// ---------------------------------------------------------------------------
//
TInt CSwtText::GetLineCount() const
{
    return Editor().TextLayout()->NumFormattedLines();
}

// ---------------------------------------------------------------------------
// CSwtText::GetLineHeightL
// From MSwtText
// ---------------------------------------------------------------------------
//
TInt CSwtText::GetLineHeightL() const
{
    CTextLayout* layout = Editor().TextLayout();
    TPoint xyPos(TPoint::EUninitialized);
    layout->DocPosToXyPosL(0, xyPos);
    TRect lineRect(TRect::EUninitialized);
    layout->GetLineRect(xyPos.iY, lineRect);
    return lineRect.Height();
}

// ---------------------------------------------------------------------------
// CSwtText::GetTopIndexL
// From MSwtText
// ---------------------------------------------------------------------------
//
TInt CSwtText::GetTopIndexL() const
{
    ASSERT(iStyle & KSwtStyleMulti);

    CTextLayout* layout = Editor().TextLayout();

    // First fully visible line
    TInt firstLine = layout->FirstLineInBand();

    // Adjust ( if needed ) to return the first partially visible line
    // FirstCharOnLine() counts lines from 1
    TInt lineStart = layout->FirstCharOnLine(firstLine + 1);
    TRect lineRect(layout->GetLineRectL(lineStart, lineStart));
    if (lineRect.iTl.iY != 0)
    {
        --firstLine;
    }

    return firstLine;
}

// ---------------------------------------------------------------------------
// CSwtText::SetTopIndexL
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::SetTopIndexL(TInt aIndex)
{
    ASSERT(iStyle & KSwtStyleMulti);

    if (aIndex < 0)
    {
        aIndex = 0;
    }
    Editor().TextView()->SetViewLineAtTopL(aIndex + 1);

    // Update vertical scroll - bar
    CEikScrollBarFrame* sbFrame = Editor().ScrollBarFrame();
    if (sbFrame)
    {
        CEikScrollBar* vsb = sbFrame->GetScrollBarHandle(
                                 CEikScrollBar::EVertical);
        if (vsb)
        {
            Editor().HandleScrollEventL(
                vsb, EEikScrollThumbReleaseVert);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtText::GetTopPixelL
// From MSwtText
// ---------------------------------------------------------------------------
//
TInt CSwtText::GetTopPixelL() const
{
    if (iStyle & KSwtStyleSingle)
    {
        return 0;
    }

    CTextLayout* layout = Editor().TextLayout();
    TInt topLine = layout->FirstLineInBand();
    TInt lineStart = layout->FirstCharOnLine(topLine + 1);
    TRect lineRect(layout->GetLineRectL(lineStart, lineStart));
    TInt lineHeight = lineRect.Height();
    if (lineRect.iTl.iY != 0)
    {
        --topLine;
    }
    TInt topPixel = (lineHeight * topLine)  +  lineRect.iTl.iY;
    return topPixel;
}

// ---------------------------------------------------------------------------
// CSwtText::GetEchoChar
// From MSwtText
// ---------------------------------------------------------------------------
//
TText CSwtText::GetEchoChar() const
{
    return static_cast < CPasswordText* >(Editor().Text())->iEchoChar;
}

// ---------------------------------------------------------------------------
// CSwtText::SetEchoChar
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::SetEchoCharL(TText aEcho)
{
    if (aEcho!=0 && !TChar(aEcho).IsPrint())
    {
        aEcho = KDefaultSecretChar;
    }
    static_cast < CPasswordText* >(Editor().Text())->iEchoChar = aEcho;
    static_cast < CPasswordText* >(Editor().Text())->SetEchoCharL();
    Redraw();
}

// ---------------------------------------------------------------------------
// CSwtText::SetDoubleClickEnabled
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::SetDoubleClickEnabled(TBool /*aEnabled*/)
{
    // No pointer, nothing to do
}

// ---------------------------------------------------------------------------
// CSwtText::GetOrientation
// From MSwtText
// ---------------------------------------------------------------------------
//
TInt CSwtText::GetOrientation() const
{
    return iStyle & KSwtOrientationMask;
}

// ---------------------------------------------------------------------------
// CSwtText::SetOrientationL
// From MSwtText
// ---------------------------------------------------------------------------
//
void CSwtText::SetOrientationL(TInt aOrientation)
{
    ASSERT(aOrientation == KSwtStyleLeftToRight
           || aOrientation == KSwtStyleRightToLeft);
    iStyle &= ~KSwtOrientationMask;
    iStyle |= aOrientation;
    SetAlignmentL();
    Editor().HandleTextChangedL();
}

// ---------------------------------------------------------------------------
// CSwtText::ProcessVerifiedTextL
// From MSwtVerifyEventObserver
// ---------------------------------------------------------------------------
//
void CSwtText::ProcessVerifiedTextL(
    TInt aStart, TInt aEnd, TDes& aText)
{
    ProcessLineBrakes(aText);
    Editor().CancelFepTransaction();
    const TCursorSelection oldSelection(Editor().Selection());

    // Special treatment for the most common cases, we simply send a key
    // instead of replacing the text. This helps reduce flicker.

    // Single printable character
    if (aText.Length() == 1
            && oldSelection.LowerPos() == aStart
            && oldSelection.HigherPos() == aEnd)
    {
        TText ch = aText[0];
        if (TChar(ch).IsPrint()
                || ch == CEditableText::EParagraphDelimiter
                || ch == CEditableText::ELineBreak)
        {
            SendCharacterL(aText[0]);
            return;
        }
    }

    if (aText.Length() == 0 && oldSelection.Length() == 0)
    {
        // Delete
        if (aStart == oldSelection.iCursorPos
                && aEnd == oldSelection.iCursorPos + 1)
        {
            SendCharacterL(EKeyDelete);
            return;
        }

        // Backspace
        if (aStart == oldSelection.iCursorPos - 1
                && aEnd == oldSelection.iCursorPos)
        {
            SendCharacterL(EKeyBackspace);
            return;
        }
    }

    Editor().ClearSelectionL();

    // Delete old range and insert new text
    CPlainText* plainText = Editor().Text();
    TInt textLength = plainText->DocumentLength();
    if (aStart > textLength)
    {
        aStart = textLength;
    }
    if (aEnd > textLength)
    {
        aEnd = textLength;
    }

    plainText->DeleteL(aStart, aEnd - aStart);

    textLength = aText.Length();
    if (textLength > 0)
    {
        TInt nbExtraChar = ((iTextLimit > 0)
                            && (iTextLimit < (plainText->DocumentLength() + textLength))) ?
                           (plainText->DocumentLength() + textLength - iTextLimit) : 0;
        if (nbExtraChar > 0)
        {
            aText.Delete(textLength - nbExtraChar, nbExtraChar);
            iEikonEnv->InfoMsg(R_EIK_TBUF_MAX_CHARACTERS_REACHED);
        }
        plainText->InsertL(aStart, aText);
    }
    Editor().HandleTextChangedL();
    TInt newCursorPos;
    if (oldSelection.HigherPos()>=aStart && oldSelection.LowerPos()<=aEnd)
    {
        newCursorPos=aStart+aText.Length();
    }
    else if (oldSelection.iCursorPos>aEnd)
    {
        newCursorPos=oldSelection.iCursorPos-(aEnd-aStart)+aText.Length();
    }
    else
    {
        newCursorPos=oldSelection.iCursorPos;
    }

    Editor().SetSelectionL(newCursorPos,newCursorPos);
    iDisplay.PostModifyEventL(iPeer);
}


// ---------------------------------------------------------------------------
// CSwtText::HandleEdwinEventL
// From MEikEdwinObserver
// ---------------------------------------------------------------------------
//
void CSwtText::HandleEdwinEventL(CEikEdwin*, TEdwinEvent aEventType)
{
    if (aEventType == EEventTextUpdate)
    {
        iDisplay.PostModifyEventL(iPeer);
    }
}

// ---------------------------------------------------------------------------
// CSwtText::SetVisibleScrollBar
// From MEikEdwinObserver
// ---------------------------------------------------------------------------
//
void CSwtText::SetVisibleScrollBar(
    TInt aStyle, TBool aVisible)
{
    // Get frame
    CEikScrollBarFrame* sbFrame = SbFrame();
    if (!sbFrame)
    {
        return;
    }

    // Calculate new visibility states
    if (aStyle & KSwtStyleHScroll)
    {
        iHScroll->SetVisible(aVisible);
    }

    if (aStyle & KSwtStyleVScroll)
    {
        iVScroll->SetVisible(aVisible);
    }

    // Resize Scrollable, as the client area has changed
    CCoeControl& coeCtrl = CoeControl();
    coeCtrl.SetSize(coeCtrl.Size());

    // Force a redraw here as some Scrollables do not update the screen after
    // a scrollbar as been switched on or off.
    Redraw();
}

