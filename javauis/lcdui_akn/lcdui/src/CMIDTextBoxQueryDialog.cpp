/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Popu-up TextBox dialog implementation for S60
*
*/

#include <eikenv.h>
#include "CMIDTextBoxQueryDialog.h"

#include "lcdui.hrh"
#include <lcdui.rsg>
#include <EIKCOCTL.rsg>

#include "CMIDEdwinUtils.h"

#include <eikcapc.h>
#include <avkon.rsg>
#include <avkon.hrh>

#include <aknenv.h>
#include <AknSettingCache.h>

// FEP CR
// constants for FEP
#include <AknFepInternalCRKeys.h>
#include <centralrepository.h>

// CAknExtendedInputCapabilities for extended input capabilities
#include <aknextendedinputcapabilities.h>

// sound files
#include <aknappui.h>
#include <aknsoundsystem.h>

// aiw used to invoke phonebook
#include <AiwContactSelectionDataTypes.h>
// phone client API classes for starting phone call
#include <AiwCommon.h>
#include <aiwdialdata.h>
#include <aiwdialdataext.h>


#include <j2me/jdebug.h>

CMIDTextBoxQueryDialog* CMIDTextBoxQueryDialog::NewLC(TInt aConstraints,
        TDes& aResult,
        TInt aMaxSize,
        CMIDDisplayable* aDisplayable,
        const TDesC& aText)
{
    CMIDTextBoxQueryDialog* self = new(ELeave) CMIDTextBoxQueryDialog(aResult);
    CleanupStack::PushL(self);
    self->ConstructL(aConstraints, aMaxSize, aDisplayable, aText);
    return self;
}

CMIDTextBoxQueryDialog::~CMIDTextBoxQueryDialog()
{
    delete iAiwPbkClient;
    delete iEdwinUtils;
    RemoveNonMidletCommands();
}

void CMIDTextBoxQueryDialog::ConstructL(TInt aConstraints,
                                        TInt aMaxSize,
                                        CMIDDisplayable* aDisplayable,
                                        const TDesC& aText)
{
    TLocale loc;
    iDecimalSeparator = loc.DecimalSeparator();
    if (iDecimalSeparator != ',')
    {
        iDecimalSeparator = '.';
    }

    iEdwinUtils = CMIDEdwinUtils::NewL(this, iDecimalSeparator);

    iConstraints = aConstraints;
    iStrict = ETrue;

    // if text is invalid according to constraints, throw IllegalArgumentException,
    // except for a PHONENUMBER which only has the invalid characters removed
    if ((iConstraints & MMIDTextField::EConstraintMask) != MMIDTextField::EPhoneNumber
            && !iEdwinUtils->ConstraintsValidForText(aText,iConstraints,ETrue))
    {
        User::Leave(KErrArgument);
    }

    iMaxSize = aMaxSize;
    iDisplayable = aDisplayable;

    // This is aprotected method call must be inside the class itself.
    // ConstructSleepingDialogL is replacing the call of ReadResourceLC
    // in CMIDTextBoxDialogControl::CreateTextBoxQueryDialogL
    ConstructSleepingDialogL(R_MIDP_POPUP_TEXTBOX);
}

CMIDTextBoxQueryDialog::CMIDTextBoxQueryDialog(TDes& aResult)
        : CAknTextQueryDialog(aResult, CAknQueryDialog::ENoTone)
{
}

TInt CMIDTextBoxQueryDialog::RouseSleepingDialog()
{
    return CAknTextQueryDialog::RouseSleepingDialog();
}

void CMIDTextBoxQueryDialog::ExitSleepingDialog()
{
    CAknTextQueryDialog::ExitSleepingDialog();
}

void CMIDTextBoxQueryDialog::TryExitL(TInt aButtonId)
{
    iDisplayable->MenuHandler()->ProcessCommandL(aButtonId);
}

TBool CMIDTextBoxQueryDialog::ProcessCommandL(CMIDCommand* aCommand)
{
    ASSERT(aCommand);
    TBool ret = EFalse;

    switch (aCommand->Id())
    {
    case CMIDEdwinUtils::EMenuCommandFetchPhoneNumber:
        // for fetching data from phonebook
        if (!iAiwPbkClient)
        {
            iAiwPbkClient = CMIDAiwPbk2Client::NewL(*this);
        }
        iAiwPbkClient->FetchFromPhoneBookL(EAiwPhoneNumberSelect);
        ret = ETrue;
        break;
    case CMIDEdwinUtils::EMenuCommandFetchEmailAddress:
        // for fetching data from phonebook
        if (!iAiwPbkClient)
        {
            iAiwPbkClient = CMIDAiwPbk2Client::NewL(*this);
        }
        iAiwPbkClient->FetchFromPhoneBookL(EAiwEMailSelect);
        ret = ETrue;
        break;
    case CMIDEdwinUtils::EMenuCommandCreatePhoneCall:
        CreatePhoneCallL();
        ret = ETrue;
        break;
    default:
        break;
    }

    return ret;
}

void CMIDTextBoxQueryDialog::DeleteTextL(TInt aOffset,TInt aLength)
{
    // get all initial values before deletion
    TInt oldSize = Size();
    HBufC* oldText = iEditor->GetTextInHBufL();
    CleanupStack::PushL(oldText);

    TInt cursorPos = GetCaretPosition();

    TCursorSelection selection = TCursorSelection(aOffset, aOffset + aLength);
    _LIT(KEmptyText, "");
    iEditor->InsertDeleteCharsL(aOffset, KEmptyText, selection);

    // Check that new content is valid.
    // If not, return old content and throw exception.
    TBool valid = iEdwinUtils->ConstraintsValidForText(Read(), iConstraints, ETrue);
    if (!valid)
    {
        iEditor->SetTextL(oldText);
    }
    CleanupStack::PopAndDestroy(oldText);

    iEditor->HandleTextChangedL();

    if (!valid)
    {
        User::Leave(KErrArgument);
    }

    // Handle cursor moving

    // Cursor is in the middle of delete area
    if ((cursorPos > aOffset) && (cursorPos < (aOffset + aLength)))
    {
        SetCursorPositionL(aOffset);
    }
    // cursor is on the right side of delete area
    else if (cursorPos > (aOffset + aLength))
    {
        TInt newPosition = cursorPos - aLength;
        if (newPosition < 0)
        {
            newPosition = 0;
        }
        SetCursorPositionL(newPosition);
    }
    // cursor is on the left side of delete area
    else
    {
        SetCursorPositionL(cursorPos);
    }
}

void CMIDTextBoxQueryDialog::SetTextL(const TDesC& aText)
{
    // if text is invalid according to constraints, throw IllegalArgumentException,
    //  except for a PHONENUMBER which only has the invalid characters removed
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
    {
    }
    else if (!iEdwinUtils->ConstraintsValidForText(aText, iConstraints, (iStrict ? ETrue : EFalse)))
    {
        User::Leave(KErrArgument);
    }
    iStrict = ETrue;

    HBufC* buf = iEdwinUtils->ConvertToLocalizedLC(aText, iConstraints);

    if (IsNumberConversionNeeded())
    {
        TPtr ptr = buf->Des();
        AknTextUtils::LanguageSpecificNumberConversion(ptr);
    }

    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
    {
        iEdwinUtils->RemoveNonPhoneNumberChars(buf);
    }
    iEditor->SetTextL(buf);
    iEditor->HandleTextChangedL();
    CleanupStack::PopAndDestroy(buf);

    // move cursor to end of the new text
    SetCursorPositionL(Size());
}

void CMIDTextBoxQueryDialog::InsertTextL(const TDesC& aText,TInt aPosition)
{
    //VALIDATE EXISTING TEXT
    HBufC* temp = NULL;
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EDecimal)
    {
        // Convert decimal content to unlocalized form before testing validity
        // of insertion.
        temp = GetTextL();
        if (temp)
        {
            CleanupStack::PushL(temp);
            iEdwinUtils->ConvertFromLocalizedDecimal(temp);
        }
    }

    if (aPosition < 0)
        aPosition = 0;
    //
    TInt size = Size();
    if (aPosition > size)
        aPosition = size;
    //
    if (!iEdwinUtils->ConstraintsValidForInsertedTextL((temp ? temp->Des() : Read()),
            aText,
            aPosition,
            iConstraints,
            ETrue))
        User::Leave(KErrArgument);
    if (temp)
    {
        CleanupStack::PopAndDestroy(temp);
        temp = NULL;
    }
    //
    //TEXT INSERTION STARTS HERE
    if (IsReadyToDraw())
    {
        iEditor->ClearSelectionL();
    }

    temp = iEdwinUtils->ConvertToLocalizedLC(aText, iConstraints);

    if (IsNumberConversionNeeded())
    {
        TPtr ptr = temp->Des();
        AknTextUtils::LanguageSpecificNumberConversion(ptr);
    }
    iEditor->Text()->InsertL(aPosition, temp->Des());

    iEditor->HandleTextChangedL();

    // Get the cursor position and move it forward along with inserter characters, if
    // characters were inserted on the left side of the cursor.
    TInt cursorPos = GetCaretPosition();
    TInt numberOfAddedCharacters = Size() - size;

    if (cursorPos >= aPosition)
    {
        if ((cursorPos + numberOfAddedCharacters) < iMaxSize)
        {
            SetCursorPositionL(cursorPos + numberOfAddedCharacters);
        }
    }

    CleanupStack::PopAndDestroy(temp);
}

void CMIDTextBoxQueryDialog::SetConstraintsL(TUint /*aConstraints*/)
{
}

void CMIDTextBoxQueryDialog::SetFEPModeAndCharFormat()
{
    iEdwinUtils->SetFEPModeAndCharFormat(iConstraints, iEditor);
}

TInt CMIDTextBoxQueryDialog::SetMaxSizeL(TInt aSize)
{
    TInt oldSize = Size();
    if (aSize < oldSize)
    {
        DeleteTextL(aSize, oldSize - aSize);
    }
    iMaxSize = aSize;
    iEditor->SetTextLimit(iMaxSize);
    iMaxSize = GetMaxSize();
    return iMaxSize;
}

TInt CMIDTextBoxQueryDialog::GetMaxSize()
{
    return iEditor->MaxLength();
}

TInt CMIDTextBoxQueryDialog::Size()
{
    return iEditor->TextLength();
}

TInt CMIDTextBoxQueryDialog::GetCaretPosition()
{
    return iEditor->CursorPos();
}

HBufC* CMIDTextBoxQueryDialog::GetTextL()
{
    // getting of native text
    HBufC* buf = iEditor->GetTextInHBufL();

    // If there is no text in iEditor, this set empty buf.
    if (!buf)
    {
        buf = HBufC::NewL(0);
    }

    // convert decimal from localized to MIDlet-visible representation
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EDecimal)
    {
        iEdwinUtils->ConvertFromLocalizedDecimal(buf);
    }

    // convert digits, if needed
    if (IsNumberConversionNeeded())
    {
        TPtr ptr = buf->Des();
        AknTextUtils::ConvertDigitsTo(ptr, EDigitTypeWestern);
    }

    // Return a copy of the contents where newlines etc. are
    // mapped to their Java representations.
    CMIDUtils::MapETextToJavaChars(buf);


    return buf;
}

void CMIDTextBoxQueryDialog::SetInitialInputModeL(const TDesC& aCharacterSubset)
{
    iEdwinUtils->SetInitialInputModeL(aCharacterSubset,
                                      iConstraints,
                                      iInitialCurrentCase,
                                      iInitialCurrentInputMode,
                                      iInitialCurrentLanguage);

    TUint constraint = iConstraints & MMIDTextField::EConstraintMask;

    // Set permitted case modes for TextBox in lowercase or uppercase mode.
    if (aCharacterSubset.Compare(KMidpUppercaseLatin) == 0 ||
            aCharacterSubset.Compare(KMidpLowercaseLatin) == 0)
    {
        // MIDP_UPPERCASE_LATIN or MIDP_LOWERCASE_LATIN are ignored if
        // INITIAL_CAPS_SENTENCE or INITIAL_CAPS_WORD modifier in ANY.
        if (!(iConstraints & MMIDTextField::EInitialCapsWordSentence ||
                iConstraints & MMIDTextField::EInitialCapsWord) ||
                constraint != MMIDTextField::EAny)
        {
            // If initial input mode is uppercase or lowercase then permit
            // only explicit case mode changes, automatic changes are not
            // allowed.
            iEditor->SetAknEditorPermittedCaseModes(EAknEditorUpperCase |
                                                    EAknEditorLowerCase);
        }
    }
    else
    {
        iEditor->SetAknEditorPermittedCaseModes(EAknEditorAllCaseModes);
    }
}

void CMIDTextBoxQueryDialog::SetTitleL(const TDesC* aString)
{
    if (aString)
    {
        SetPromptL(*aString);
    }
    else
    {
        SetPromptL(KNullDesC);
    }
}

void CMIDTextBoxQueryDialog::SetCursorPositionL(TInt aPosition)
{
    TInt textLength = iEditor->TextLength();
    if (textLength < aPosition)
    {
        aPosition = textLength;
    }
    if (textLength >0)
    {
        iEditor->SetCursorPosL(aPosition, EFalse);
    }
    else
    {
        iEditor->SetCursorPosL(0, EFalse);
    }
}

TSize CMIDTextBoxQueryDialog::ContentSize() const
{
    return CCoeControl::Size();
}

void CMIDTextBoxQueryDialog::FocusChanged(TDrawNow aDrawNow)
{
    CAknTextQueryDialog::FocusChanged(aDrawNow);
}
//
// We do not want to become visible if we are not showing
//
void CMIDTextBoxQueryDialog::MakeVisible(TBool aVisible)
{
    if (!aVisible)
    {
        CAknTextQueryDialog::MakeVisible(aVisible);
    }
    else if (iShowing)
    {
        if (aVisible)
        {
            //re-layouting done just before dialog is shown
            //to be sure layout is correct in CAknPopupForm
            CAknTextQueryDialog::SizeChanged();
        }
        CAknTextQueryDialog::MakeVisible(aVisible);
    }
}

void CMIDTextBoxQueryDialog::SizeChanged()
{
    if (iShowing)
    {
        CAknTextQueryDialog::SizeChanged();
    }
    TRAP_IGNORE(UpdateScrollBarPositionL());
}

TKeyResponse CMIDTextBoxQueryDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    // If midlet is closed from applications menu or by pressing end key, then
    // 'esc' key event is generated. That event is not handled here.
    // See OfferKeyEventL function of CMIDTextBoxDialogControl.
    if (aKeyEvent.iCode == EKeyEscape)
    {
        return EKeyWasNotConsumed;
    }
    //
    //  Enter Key:
    //
    //
    if (aType == EEventKey)
    {
        iusedKeyEvent = &aKeyEvent;
    }

    // If TextBox have constraints below params
    // Any and Password
    // Numeric or Numeric and Password or
    // Decimal or Decimal and Password  or
    // PhoneNumber or PhoneNumber and Password or
    // MailAddr or MailAddr and Password or
    // PhoneNumber Uneditable or MailAddr Uneditable or
    // Url or Url and Password
    // then show menu or activate one command.
    if (((aType == EEventKey && aKeyEvent.iCode == EKeyEnter) ||
            (aType == EEventKeyUp && aKeyEvent.iScanCode ==   EStdKeyEnter &&
             aKeyEvent.iCode == EKeyEnter && iusedKeyEvent!=&aKeyEvent)))
    {

        if ((iConstraints == MMIDTextField::EAny + MMIDTextField::EPassword) ||

                (iConstraints == MMIDTextField::ENumeric ||
                 (iConstraints == MMIDTextField::ENumeric  + MMIDTextField::EPassword) ||
                 iConstraints == MMIDTextField::EDecimal ||
                 (iConstraints == MMIDTextField::EDecimal + MMIDTextField::EPassword)) ||

                (iConstraints == MMIDTextField::EPhoneNumber ||
                 (iConstraints == MMIDTextField::EPhoneNumber + MMIDTextField::EPassword) ||
                 iConstraints == MMIDTextField::EMailAddr ||
                 (iConstraints == MMIDTextField::EMailAddr  + MMIDTextField::EPassword))  ||

                ((iConstraints == MMIDTextField::EPhoneNumber + MMIDTextField::EUneditable) ||
                 (iConstraints == MMIDTextField::EMailAddr  + MMIDTextField::EUneditable)) ||

                (iConstraints == MMIDTextField::EUrl ||
                 (iConstraints == MMIDTextField::EUrl + MMIDTextField::EPassword)))
        {

            // counter OK and Item elements from options menu
            TInt numOpt = iDisplayable->NumCommandsForOkOptionsMenu();

            //open menu
            if (numOpt > 1)
            {
                iDisplayable->MenuHandler()->ShowMenuL(CMIDMenuHandler::EOkMenu);
            }
            else
            {
                CMIDCommand* cmdTmp = NULL;

                // counter of all elements from options menu
                TInt numItemCommands = iDisplayable->MainCommandList()->Count();

                for (TInt i = 0; i < numItemCommands; i++)
                {
                    // for each command from menu
                    // count offset index only for commands
                    // without FetchPhoneNumber, EmailAddress, PhoneCall
                    cmdTmp = iDisplayable->MainCommandList()->At(i).iCommand;

                    if (cmdTmp)
                    {
                        if ((cmdTmp->Id() != CMIDEdwinUtils::EMenuCommandFetchPhoneNumber) &&
                                (cmdTmp->Id() != CMIDEdwinUtils::EMenuCommandFetchEmailAddress) &&
                                (cmdTmp->Id() != CMIDEdwinUtils::EMenuCommandCreatePhoneCall))
                        {
                            break;
                        }
                    }
                }

                // execute command
                if (cmdTmp)
                {
                    if ((cmdTmp->CommandType() == MMIDCommand::EOk ||
                            cmdTmp->CommandType() == MMIDCommand::EItem ||
                            cmdTmp->CommandType() == MMIDCommand::EScreen))
                    {
                        // get command index from option menu
                        TInt offsetIdx = iDisplayable->MainCommandList()->FindCommandIndex(cmdTmp);

                        if (offsetIdx != KErrNotFound &&
                                iDisplayable->MainCommandList()->At(offsetIdx).iCommand == cmdTmp)
                        {
                            iDisplayable->ProcessCommandL(iDisplayable->MainCommandList()->CommandOffset() + offsetIdx);
                        }

                    }
                }
            }
            return EKeyWasConsumed;
        }
    }

    //
    //  End Enter Key
    //

    // msk: this is needed if MSK is not enabled on the device
    if ((aType == EEventKey) && (aKeyEvent.iScanCode == EStdKeyDevice3))
    {
        if (iDisplayable && iDisplayable->ShowOkOptionsMenuL())
        {
            SetFocus(EFalse);
        }
        return EKeyWasConsumed;
    }

    // store the key pressed last time, used for warning beeps while reaching size limit
    if (aType == EEventKeyDown)
    {
        iLastMultitapKey = iThisMultitapKey;
        iThisMultitapKey = aKeyEvent.iScanCode;
    }

    if (aType == EEventKeyDown)
    {
        iKeyEventsPending = ETrue;
    }
    else if (aType == EEventKeyUp)
    {
        iKeyEventsPending = EFalse;
    }
    //
    // Post ItemStateChangedEvent before a command is delivered to the Form's CommandListener
    //
    TUint scanCode = aKeyEvent.iScanCode;

    //
    // ignore up and down arrows on PASSWORD TextField to avoid funny behaviour
    // with line breaks
    if (iDisplayable && (iConstraints & MMIDTextField::EPassword)
            && (aKeyEvent.iCode == EKeyDownArrow || aKeyEvent.iCode == EKeyUpArrow))
    {
        return EKeyWasConsumed;
    }

    // when pressing the send key on a phonenumber-type box, call the number
#ifdef RD_SCALABLE_UI_V2
    if ((aType == EEventKey) && (scanCode == EStdKeyYes))
#else
    if ((aType == EEventKeyUp) && (scanCode == EStdKeyYes))
#endif // RD_SCALABLE_UI_V2
    {
        if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
        {
            CreatePhoneCallL();
            return EKeyWasConsumed;
        }
    }

#ifdef RD_SCALABLE_UI_V2
    if (IsConstraintSet(MMIDTextField::ENumeric) && (aType == EEventKey || aType == EEventKeyUp) && !iEditor->IsReadOnly() &&
#else
    if (IsConstraintSet(MMIDTextField::ENumeric) && (aType == EEventKeyUp) && !iEditor->IsReadOnly() &&
#endif // RD_SCALABLE_UI_V2
            ((scanCode==EStdKeyNkpAsterisk) || (scanCode == EStdKeyMinus) || (scanCode==EStdKeyNkpMinus) || (scanCode == 0x2A) || (scanCode == 0x2D) ||
             (TChar(aKeyEvent.iCode) == TChar('-') && scanCode != EStdKeyMinus)))
    {
        HandleMinusCharEventL(MEikEdwinObserver::EEventTextUpdate);
        return EKeyWasConsumed;
    }

    //handling of qwerty keypad "-" or "." pressing in Decimal mode
#ifdef RD_SCALABLE_UI_V2
    if (IsConstraintSet(MMIDTextField::EDecimal) && (aType == EEventKey) && !iEditor->IsReadOnly() &&
#else
    if (IsConstraintSet(MMIDTextField::EDecimal) && (aType == EEventKeyUp) && !iEditor->IsReadOnly() &&
#endif // RD_SCALABLE_UI_V2
            ((scanCode==EStdKeyMinus) || (scanCode==EStdKeyFullStop)))
    {
        CPlainText* res = iEditor->Text();
        TInt textLength = Size();
        if (scanCode == EStdKeyMinus && textLength < iMaxSize)
        {
            res->InsertL(GetCaretPosition(), KMinusChar);
        }
        else if (scanCode == EStdKeyFullStop && textLength < iMaxSize)
        {
            res->InsertL(GetCaretPosition(), KFullStopChar);
        }

        iEditor->HandleTextChangedL(); // notify editor about the text changes
        TInt cursorPos = GetCaretPosition();

        if (Size() < iMaxSize)
        {
            SetCursorPositionL(cursorPos + 1);
        }
        else if (cursorPos == (iMaxSize - 1) && cursorPos == textLength && scanCode==EStdKeyFullStop)
        {
            SetCursorPositionL(iMaxSize);
        }

        HandleTextUpdateL(MEikEdwinObserver::EEventTextUpdate);
    }
    //Error tone playing case1:
    //Play error tone if TextBox/TextField is read-only or maximum length has been reached.
    //Here is handling of keys 0...9 for full keyboard and itu-t, but NOT virtual keyboard.
    if (aType == EEventKeyDown
            && ((scanCode >= KKeyQwerty0 && scanCode <= KKeyQwerty9)
                || scanCode == EStdKeyHash
                || scanCode == EStdKeyNkpAsterisk
                || scanCode == KMidpKeyNkpAsteriskHW))
    {
        if ((scanCode == EStdKeyNkpAsterisk || scanCode == KMidpKeyNkpAsteriskHW)
                && !iEditor->IsReadOnly() && IsConstraintSet(MMIDTextField::ENumeric))
        {
            //Do nothing here in case of minus char toggle
        }
        else if (iEditor->IsReadOnly())
        {
            iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
            return EKeyWasConsumed;
        }

        // All possible sources of warning beep are separated to be easy to read
        else if (Size() >= iMaxSize)
        {
            TInt ret = KErrNone;
            TInt inputModeValue = iEditor->AknEditorCurrentInputMode();
            if (inputModeValue != EAknEditorTextInputMode)
            {
                iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
                return EKeyWasConsumed;
            }

            TInt aknRepositoryValue = 0;
            CRepository* aknFepRepository = NULL;
            TRAP(ret, aknFepRepository = CRepository::NewL(KCRUidAknFep));
            if ((ret != KErrNone) || (!aknFepRepository))
            {
                // if we cannot get the repository (something is wrong), play nothing
                return EKeyWasConsumed;
            }
            aknFepRepository->Get(KAknFepPredTxtFlag, aknRepositoryValue);

            delete aknFepRepository;
            aknFepRepository = NULL;

            if (aknRepositoryValue == 1)    // 1 means the predictive input is on
            {
                iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
                return EKeyWasConsumed;
            }
            // now only nonpredictive textinput is left
            if (iThisMultitapKey != iLastMultitapKey) // different key tapped -> no multitap
            {
                iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
                iThisMultitapKey = 0;
                iLastMultitapKey = 0;
                return EKeyWasConsumed;
            }
        }
    }

    if (aType != EEventKey)
    {
        return EKeyWasConsumed;
    }


    TKeyResponse response = EKeyWasNotConsumed;
    TBool isFocused = IsFocused();

    // If there is no focus textbox should not consume keys
    if (isFocused)
    {
        //Error tone playing case2:
        //Play error tone if TextBox/TextField is read-only or maximum length has been reached.
        //Here is handling of full keyboard keys(NOT 0...9) and all virtual keyboard keys.
        //(Note: Virtual keyboard sends only EEventKey type events, not up or down events)
        //(Note: Error tone is played when there is no text to be replaced i.e. no text has been painted)
        if (!iEdwinUtils->IsNavigationKey(aKeyEvent) && !iEdwinUtils->IsHotKeyL(aKeyEvent, iCoeEnv) && !aKeyEvent.iCode == EKeyYes &&
                (!iKeyEventsPending || (scanCode < KKeyQwerty0  || scanCode > KKeyQwerty9)))
        {
            if (iEditor->IsReadOnly() || (Size() >= iMaxSize && aKeyEvent.iCode != EKeyBackspace))
            {
                //SelectionLength() > 0 if text has been selected/painted
                if (iEditor->SelectionLength() == 0)
                {
                    iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
                }
                response = iEditor->OfferKeyEventL(aKeyEvent,aType);
                return response;
            }
        }

        TBool valid = EFalse;

        if (iEdwinUtils->IsNavigationKey(aKeyEvent) || iEdwinUtils->IsHotKeyL(aKeyEvent, iCoeEnv))
        {
            HBufC* oldText = GetTextL();
            CleanupStack::PushL(oldText);
            TCursorSelection sel = iEditor->Selection();

            response = CAknTextQueryDialog::OfferKeyEventL(aKeyEvent,aType);
            valid = iEdwinUtils->ConstraintsValidForText(iEditor->Text()?Read():TPtrC(),iConstraints,EFalse);

            if (!valid)
            {
                SetTextL(*oldText);
                iEditor->HandleTextChangedL();
                iEditor->SetSelectionL(sel.iCursorPos,sel.iAnchorPos);
                response = EKeyWasConsumed;
            }
            CleanupStack::PopAndDestroy(oldText);

            return response;
        }
        else
        {
            TBuf<1> key;
            key.Append(TChar(aKeyEvent.iCode));
            valid = iEdwinUtils->ConstraintsValidForInsertedTextL(iEditor->Text()?Read():TPtrC(),
                    key,
                    GetCaretPosition(),
                    iConstraints,
                    EFalse);
            if (valid)
            {
                response = iEditor->OfferKeyEventL(aKeyEvent,aType);
            }
            else
            {
                // If minus char was entered in full querty editor mode
                if (IsConstraintSet(MMIDTextField::EDecimal) &&
                        (aType == EEventKey) && iEditor && !iEditor->IsReadOnly() &&
                        (TChar(aKeyEvent.iCode) == TChar('-') && scanCode != EStdKeyMinus))
                {
                    CPlainText* res = iEditor->Text();

                    if (res && TChar(aKeyEvent.iCode) == TChar('-') &&
                            Size() < iMaxSize)
                    {
                        res->InsertL(GetCaretPosition(), KMinusChar);
                        // notify editor about the text changes
                        iEditor->HandleTextChangedL();

                        if (Size() < iMaxSize)
                        {
                            SetCursorPositionL(GetCaretPosition() + 1);
                        }

                        //Prevent changes that would result in an illegal string
                        HandleTextUpdateL(MEikEdwinObserver::EEventTextUpdate);
                    }
                }

            }
            return response;
        }
    }
    return response;
}

#ifdef RD_SCALABLE_UI_V2
void CMIDTextBoxQueryDialog::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled() && Rect().Contains(aPointerEvent.iPosition))
    {
        TBool consumed = STATIC_CAST(CMIDDisplayable*, iDisplayable)->TryDetectLongTapL(aPointerEvent);
        if (!consumed)
        {
            CAknTextQueryDialog::HandlePointerEventL(aPointerEvent);
        }
    }
}
#endif // RD_SCALABLE_UI_V2

TCoeInputCapabilities CMIDTextBoxQueryDialog::InputCapabilities() const
{
    TCoeInputCapabilities inputCapabilities(TCoeInputCapabilities::ENone, NULL,
                                            const_cast<CMIDTextBoxQueryDialog*>(this));
    inputCapabilities.MergeWith(iEditor->InputCapabilities());
    return inputCapabilities;
}

void CMIDTextBoxQueryDialog::HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType)
{
    if (aEdwin != iEditor)
    {
        return;
    }

    if (aEventType == MEikEdwinObserver::EEventTextUpdate)
    {
        // handles clearing multitap counter
        if ((iLastMultitapKey != 0) && (iLastMultitapKey != iThisMultitapKey))
        {
            iLastMultitapKey = 0;
        }
        else
        {
            iThisMultitapKey = 0;
            iLastMultitapKey = 0;
        }

        HandleTextUpdateL(MEikEdwinObserver::EEventTextUpdate);

    }
}

void CMIDTextBoxQueryDialog::HandleTextUpdateL(TEdwinEvent aEventType)
{
    if (aEventType == MEikEdwinObserver::EEventTextUpdate)
    {
        TBool textChanged = EFalse;
        HBufC* res = NULL;
        TRAPD(err, { res = GetTextL();});
        if (err != KErrNone || !res)
        {
            return;
        }
        CleanupStack::PushL(res);

        // in DECIMAL mode, don't allow changes that would result
        // in an illegal string;
        //  1) if there is a minus, it must be the first character
        //  2) there can be at most one decimal separator
        if ((aEventType == EEventTextUpdate) && ((iConstraints
                & MMIDTextField::EConstraintMask) == MMIDTextField::EDecimal))
        {
            TInt illegalCharPos = -1;
            TPtr16 ptr = res->Des();
            TInt minusPos = ptr.LocateReverse(TChar('-'));
            TInt pointPosL = ptr.Locate(iDecimalSeparator);
            TInt pointPosR = ptr.LocateReverse(iDecimalSeparator);
            TInt cursorPos = GetCaretPosition();
            TInt endCursorPos = cursorPos;

            // check if minus sign is inserted on incorrect place
            // (not at the beginning)
            if ((minusPos != KErrNotFound) && (minusPos != 0))
            {
                // check if minus sign isn't inserted twice
                if (minusPos != ptr.Locate(TChar('-')))
                {
                    illegalCharPos = minusPos;
                }
                // insert minus char at the beginning of decimal field
                else
                {
                    ptr.Delete(minusPos, 1);
                    ptr.Insert(0, KMinusChar);
                    minusPos = 0;
                    iStrict = EFalse;

                    // Decimal separator was inserted twice and minus
                    // was next character in text. Minus was just set as first
                    // character - so in fact we have two decimal separators
                    // after minus character - this is not allowed,
                    // delete extra character
                    if (pointPosL != pointPosR && pointPosL == 0 &&
                            minusPos == pointPosL)
                    {
                        // Delete both decimal separator characters
                        // and place new decimal separator in correct position
                        ptr.Delete(pointPosR, 1);
                        ptr.Delete((pointPosL + 1), 1);
                        ptr.Insert(1, KFullStopChar);
                    }
                    SetTextL(*res);
                    textChanged = ETrue;

                    // Set correct cusros possition
                    if (pointPosL == 0 && minusPos == pointPosL)
                    {
                        endCursorPos = (pointPosL + 1);
                    }
                }
            }

            // Locate decimal separator again
            pointPosL = ptr.Locate(iDecimalSeparator);
            pointPosR = ptr.LocateReverse(iDecimalSeparator);

            if ((minusPos != KErrNotFound) && (pointPosL == 0))
            {
                illegalCharPos = pointPosL;
            }
            else if (pointPosL != pointPosR)
            {
                illegalCharPos = pointPosR;
            }

            // if minus or dot/comma char is displayed more than once
            if (illegalCharPos >= 0)
            {
                // deleting second minus or dot/comma char
                // and set cursor position on the right place
                ptr.Delete(illegalCharPos, 1);
                iStrict = EFalse;
                SetTextL(*res);
                textChanged = ETrue;
                if (cursorPos >= 1)
                {
                    if (cursorPos == (illegalCharPos + 1))
                    {
                        cursorPos--;
                    }

                    endCursorPos = cursorPos;
                }
            }
            SetCursorPositionL(endCursorPos);
        }
        else if (((iConstraints &
                   MMIDTextField::EConstraintMask) == MMIDTextField::EMailAddr) ||
                 ((iConstraints &
                   MMIDTextField::EConstraintMask) == MMIDTextField::EUrl))
        {
            // In EMAIL and URL mode, don't allow changes that would result
            // in an illegal string; if there is '/n' or '/f', and line breaks
            // are not supported, chars need to be changed to space
            // SetText function before actual text change checks if line
            // breaks are not supported
            TPtr16 text = res->Des();
            TInt tmpPos;

            if ((text.Locate(TChar('\n'))) >=0 ||
                    (text.Locate(TChar('\f'))) >=0)
            {
                tmpPos = GetCaretPosition();
                SetTextL(*res);
                SetCursorPositionL(tmpPos);
            }

            textChanged = ETrue;
        }
        CleanupStack::Pop(res);
        delete res;

        // if there were any modifications the text is updated
        if (textChanged)
        {
            iEditor->HandleTextChangedL();
        }
    }
}

void CMIDTextBoxQueryDialog::GetCaptionForFep(TDes& aCaption) const
{
    aCaption = Prompt();
}

void CMIDTextBoxQueryDialog::ProcessModifiers()
{
    // set general edwin attributes according to TextBox constraints
    iEditor->SetReadOnly(iConstraints & MMIDTextField::EUneditable);
}

void CMIDTextBoxQueryDialog::ShowL(TBool aShow)
{
    if (aShow != iShowing)
    {
        if (aShow)
        {
            iShowing = ETrue;

#ifdef RD_SCALABLE_UI_V2
            SetPointerCapture(ETrue);
#endif
            RouseSleepingDialog();

            UpdateInitialInputModes();
        }
        else
        {
            ExitSleepingDialog();
#ifdef RD_SCALABLE_UI_V2
            SetPointerCapture(EFalse);
#endif

            iShowing = EFalse; //attention: iShowing must be true when dismissing
            // a dialog or else MakeVisible() won't execute and
            // we won't update the fader's stack, @see MakeVisible()

        }

        UpdateCbasL();
    }
}

void CMIDTextBoxQueryDialog::SetTextWithNewConstraintsL(HBufC* aText)
{
    // If the current contents of the TextBox do not match the new constraints,the
    // contents are set to empty, except for a PHONENUMBER which only has the
    // invalid characters removed. Otherwise, set the text as given.
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
    {
        iEdwinUtils->RemoveNonPhoneNumberChars(aText);
        SetTextL(*aText);
    }
    else if (!iEdwinUtils->ConstraintsValidForText(aText->Des(),
             iConstraints & MMIDTextField::EConstraintMask,
             ETrue))
    {
        SetTextL(KNullDesC);
    }
    else
    {
        SetTextL(*aText);
    }
}

void  CMIDTextBoxQueryDialog::UpdateLeftSoftKeyL()
{
}

TBool CMIDTextBoxQueryDialog::OkToExitL(TInt aCommandId)
{
    TInt offset = iDisplayable->MainCommandList()->CommandOffset();
    if (aCommandId ==EEikBidCancel)
    {
        return ETrue;
    }
    if (aCommandId == EAknSoftkeyOptions)   // 3000
    {
        iDisplayable->MenuHandler()->ProcessCommandL(EAknSoftkeyOptions);
        SetFocus(EFalse);
    }
    else if (aCommandId - offset < 0)   // aCommandId value is over 3000 - used by MSK
    {
        iDisplayable->ProcessCommandL(aCommandId);
    }
    else // aCommandId value is 7000 and over - used by LSK, RSK and EnterKey
    {
        CMIDCommand* tempCmd = NULL;
        CMIDCommand* command = NULL;
        TBool isMaped = EFalse;
        TInt mapedCnt = ENone;
        TInt mapedId = ENone;

        // number of all commands
        TInt numberOfCommands = iDisplayable->CommandCount();
        // number of Ok and Item commands
        TInt numberOfOkOptionsCommands = iDisplayable->NumCommandsForOkOptionsMenu();
        // command ID which is get from aCommandId
        TInt commandId = aCommandId - offset;

        if (numberOfCommands > 0)
        {
            // Go through every midlet command, and save its ID if maped
            // and also count maped commands.
            for (TInt i = 0; i < numberOfCommands; i++)
            {
                tempCmd = iDisplayable->MainCommandList()->At(i).iCommand;
                if (tempCmd->IsMappedToSoftKey())
                {
                    mapedId = i;
                    isMaped = ETrue;
                    mapedCnt++;
                }
            }

            // If there are more than one maped command (LSK & RSK maped)
            // or maped command is different than commandId (EnterKey used),
            // set CMIDCommand command with this command, which index
            // is the same as commandId value.
            if (mapedCnt > 1 || mapedId != commandId)
            {
                command = iDisplayable->MainCommandList()->At(commandId).iCommand;
            }
            else // Else if there are one or zero command maped.
            {
                if (isMaped)   // One command maped.
                {
                    command = iDisplayable->MainCommandList()->At(mapedId).iCommand;
                }
                else // Zero commands maped.
                {
                    command = iDisplayable->MainCommandList()->At(0).iCommand;
                }
            }

            // type of the command
            TInt commandType = command->CommandType();

            // If there aren't any EExit, EBack, ECancel and EStop
            // command mapped and there are still some commands
            // show menu, else process with maped command.
            if (commandType != MMIDCommand::EExit &&
                    commandType != MMIDCommand::EBack &&
                    commandType != MMIDCommand::ECancel &&
                    commandType != MMIDCommand::EStop &&
                    numberOfOkOptionsCommands > 1)
            {
                iDisplayable->MenuHandler()->ShowMenuL(CMIDMenuHandler::EOkMenu);
            }
            else
            {
                iDisplayable->ProcessCommandL(aCommandId);
            }
        }
    }

    //The dialog can only be dismissed java side or else we'll end
    //up with the TextBox displayed and no dialog
    return EFalse;
}

void CMIDTextBoxQueryDialog::SetEditorL()
{
    iEditor = (CEikEdwin*)QueryControl()->ControlByLayoutOrNull(EDataLayout);
    iEditor->SetAknEditorFlags(CEikEdwin::EZeroEnumValue);

    TBool singleLineEditor = IsConstraintSet(MMIDTextField::EMailAddr)
                             || IsConstraintSet(MMIDTextField::EUrl)
                             || (iConstraints & MMIDTextField::EPassword);

    if (((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EAny)
            && !(iConstraints & MMIDTextField::EPassword))
    {
        iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars | iEditor->AknEdwinFlags());
    }

    AknEditUtils::ConstructEditingL(iEditor,                        // aEdwin
                                    iMaxSize,                      // aEditingSpace
                                    (singleLineEditor ? 1 : 0),    // aEditingWindow
                                    EAknEditorCharactersLowerCase, // aCharacterCase
                                    EAknEditorAlignLeft,           // aJustification
                                    ETrue,                         // aAllowedToMoveInsertionPoint
                                    ETrue,                         // aCursorYesNo
                                    EFalse);                       // aOverflowYesNo


    if (iConstraints & MMIDTextField::EPassword)
    {
        TUint32 userFlags = iEditor->UserFlags();
        iEditor->SetAknEditorFlags(userFlags |= CEikEdwin::EUserSuppliedText);
        CreatePasswordTextL();
    }

    HBufC* buf = GetTextL();
    CleanupStack::PushL(buf);
    if (buf)
    {
        CMIDUtils::MapJavaToETextChars(buf);
        if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
        {
            iEdwinUtils->RemoveNonPhoneNumberChars(buf);
        }
        SetTextL(*buf);
    }
    CleanupStack::PopAndDestroy(buf);

    iEditor->CreateTextViewL();
    ProcessModifiers();
    SetFEPModeAndCharFormat();
    iEditor->AddEdwinObserverL(this);

    CreateNonMidletCommandsL();

    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::ENonPredictive)
    {
        SetPredictiveTextInputPermitted(EFalse);
    }
    else
    {
        SetPredictiveTextInputPermitted(ETrue);
    }
    // Publish MIDP constraints via input capabilities of the CEikEdwin
    // The information is needed by chinese VKB/HWR system that adjusts its behavior
    // based on MIDP constraints (most notably by checking if the field is a PASSWORD field).
    TCoeInputCapabilities inputCapabilities = InputCapabilities();
    if (inputCapabilities != TCoeInputCapabilities::ENone)
    {
        MObjectProvider* mop = inputCapabilities.ObjectProvider();
        if (mop)
        {
            CAknExtendedInputCapabilities* extendedInputCapabilities = NULL;
            extendedInputCapabilities = mop->MopGetObject(extendedInputCapabilities);
            if (extendedInputCapabilities)
            {
                extendedInputCapabilities->SetMIDPConstraints(iConstraints);
            }
        }
    }

    iThisMultitapKey = 0;   // reset key counter
    iLastMultitapKey = 0;

    if (iEditor->ScrollBarFrame())
    {
        iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);
    }
}

TBool CMIDTextBoxQueryDialog::Showing()
{
    return iShowing;
}

TInt CMIDTextBoxQueryDialog::Constraints()
{
    return iConstraints;
}

void CMIDTextBoxQueryDialog::UpdateCbasL()
{
    if (iShowing)
    {
        iDisplayable->SetCba(&ButtonGroupContainer());
        iDisplayable->InitializeCbasL();
        CCoeControl* cba = ButtonGroupContainer().ButtonGroup()->AsControl();
        cba->DrawableWindow()->SetOrdinalPosition(0);
        cba->MakeVisible(ETrue);
        cba->DrawNow();
    }
    else
    {
        iDisplayable->SetCba(iDisplayable->MenuHandler()->Cba());
    }
}

TBool CMIDTextBoxQueryDialog::IsNumberConversionNeeded()
{
    return iEdwinUtils->IsNumberConversionNeeded(GetConstraints());
}

void CMIDTextBoxQueryDialog::HandleMinusCharEventL(MEikEdwinObserver::TEdwinEvent aEventType)
{
    if ((aEventType == MEikEdwinObserver::EEventTextUpdate) && ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::ENumeric))
    {
        CPlainText* res = iEditor->Text();
        TBuf<1> firstChar;
        if (Size() != 0)
        {
            res->Extract(firstChar,  0, 1);
        }
        TInt cursorPos = GetCaretPosition();
        //toggle '-' char in the first position
        TInt pos = firstChar.Locate(TChar('-'));
        if (pos == KErrNotFound)
        {
            if (Size() < iMaxSize)
            {
                res->InsertL(0, KMinusChar);
                cursorPos++;
            }
            else
            {
                //play error sound if text limit is to be exceeded
                iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
            }
        }
        else
        {
            TBool deletion = res->DeleteL(0,1);
            if (cursorPos >= 1)
            {
                cursorPos--;
            }
        }
        iEditor->HandleTextChangedL(); // notify editor about the text changes
        SetCursorPositionL(cursorPos);
    }
}

TPtrC CMIDTextBoxQueryDialog::Read() const
{
    if (iConstraints & MMIDTextField::EPassword)
    {
        return static_cast<CPasswordBoxGlobalText*>(iEditor->Text())->ClearText();
    }
    else
    {
        return iEditor->Text()->Read(0,iEditor->TextLength());
    }
}

/** Create non-midlet commands according to the constraints and add them
to either the displayable or to the item. Commands will be processed in ProcessCommandL(). */
void CMIDTextBoxQueryDialog::CreateNonMidletCommandsL()
{
    if (((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber) && !(iConstraints & MMIDTextField::EUneditable))
    {
        AddCommandToEdwinL(R_MIDP_PB_FETCH_NUMBER_SHORT_COMMAND_TEXT, R_MIDP_PB_FETCH_NUMBER_COMMAND_TEXT, CMIDEdwinUtils::EMenuCommandFetchPhoneNumber);
        AddCommandToEdwinL(R_MIDP_CREATE_CALL_SHORT_COMMAND_TEXT, R_MIDP_CREATE_CALL_COMMAND_TEXT, CMIDEdwinUtils::EMenuCommandCreatePhoneCall);
    }

    if (((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber) && (iConstraints & MMIDTextField::EUneditable))
    {
        AddCommandToEdwinL(R_MIDP_CREATE_CALL_SHORT_COMMAND_TEXT, R_MIDP_CREATE_CALL_COMMAND_TEXT, CMIDEdwinUtils::EMenuCommandCreatePhoneCall);
    }

    if (((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EMailAddr) && !(iConstraints & MMIDTextField::EUneditable))
    {
        AddCommandToEdwinL(R_MIDP_PB_FETCH_EMAIL_SHORT_COMMAND_TEXT, R_MIDP_PB_FETCH_EMAIL_COMMAND_TEXT, CMIDEdwinUtils::EMenuCommandFetchEmailAddress);
    }
}

/**
Creates and adds new command to edwin based on shot & long label resource ids
and other parameters.
*/
void CMIDTextBoxQueryDialog::AddCommandToEdwinL(TInt aCommandResIdShort,
        TInt aCommandResIdLong,
        TInt aCommandId)
{
    TBuf<64> shortLabel;
    iEikonEnv->ReadResourceL(shortLabel, aCommandResIdShort);
    TBuf<64> longLabel;
    iEikonEnv->ReadResourceL(longLabel, aCommandResIdLong);

    MMIDCommand* cmd = CMIDCommand::NewL(shortLabel, longLabel, MMIDCommand::EItem, 0, aCommandId);
    CleanupStack::PushL(cmd);

    STATIC_CAST(CMIDCommand*,cmd)->SetObserver(this);

    AddCommandToEdwinL(*cmd);
    CleanupStack::Pop(cmd);
}

/**
Creates and adds new command to edwin, short label is the same as long label.
*/
void CMIDTextBoxQueryDialog::AddCommandToEdwinL(TInt aCommandResId,
        TInt aCommandId)
{
    AddCommandToEdwinL(aCommandResId, aCommandResId, aCommandId);
}

/** This method is called from the destructor and removes
    the commands added by CreateNonMidletCommandsL(). To do
    this we examine cmds on the list (either of the displayable
    or the item as applicable) and remove those whose observer is us.
    */
void CMIDTextBoxQueryDialog::RemoveNonMidletCommands()
{
    CMIDCommandList* list = NULL;
    if (iDisplayable)
    {
        list = STATIC_CAST(CMIDDisplayable*, iDisplayable)->MainCommandList();
    }

    if (!list)
    {
        return;
    }

    for (TInt i = 0; i < list->Count(); i++)
    {
        CMIDCommand* cmd = list->At(i).iCommand;
        if (cmd && (cmd->Observer() == this))
        {
            list->Remove(cmd);
            cmd->Dispose();
            i--;
        }
    }
}

void CMIDTextBoxQueryDialog::AddCommandToEdwinL(MMIDCommand& aCommand)
{
    STATIC_CAST(CMIDDisplayable*, iDisplayable)->AddCommandL(&aCommand);
}

void CMIDTextBoxQueryDialog::CreatePhoneCallL()
{
    iEdwinUtils->CreatePhoneCallL(Size(), CCoeEnv::Static()->WsSession(), iEditor);
}

void CMIDTextBoxQueryDialog::CreatePasswordTextL()
{
    CGlobalText* oldGText = STATIC_CAST(CGlobalText*,iEditor->Text());
    CPasswordBoxGlobalText* text = new(ELeave) CPasswordBoxGlobalText
    (oldGText->GlobalParaFormatLayer(),oldGText->GlobalCharFormatLayer(),*iEditor);
    CleanupStack::PushL(text);
    text->ConstructL();
    CleanupStack::Pop(text);

    CPlainText*  editorText = iEditor->Text();
    delete editorText;
    editorText = NULL;

    iEditor->SetDocumentContentL(*text,CEikEdwin::EUseText);
}

void CMIDTextBoxQueryDialog::UpdateInitialInputModes()
{
    // when becoming the current Displayable, set editor input mode
    // to the one set by SetInitialInputModeL
    if (((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EAny ||
            (iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EMailAddr ||
            (iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EUrl))
    {
        if (iInitialCurrentLanguage)
        {
            iEditor->SetAknEditorLocalLanguage(iInitialCurrentLanguage);
            if (iInitialCurrentInputMode == 0)
            {
                // clears any number mode used previously
                iEditor->SetAknEditorCurrentInputMode(EAknEditorNullInputMode);
            }
        }

        if (iInitialCurrentInputMode)
        {
            iEditor->SetAknEditorCurrentInputMode(iInitialCurrentInputMode);
        }

        if (iInitialCurrentCase)
        {
            if ((iConstraints & MMIDTextField::EPassword ||
                    iConstraints & MMIDTextField::EMailAddr ||
                    iConstraints & MMIDTextField::EUrl) &&
                    iInitialCurrentCase == EAknEditorTextCase)
            {
                // Text case is not used in passwords, emailaddrs and urls
                iInitialCurrentCase = EAknEditorLowerCase;
            }
            iEditor->SetAknEditorCurrentCase(iInitialCurrentCase);
            iEditor->SetAknEditorCase(iInitialCurrentCase);
        }
    }
}

TTypeUid::Ptr CMIDTextBoxQueryDialog::MopSupplyObject(TTypeUid aId)
{
    TTypeUid::Ptr ret = TTypeUid::Null();

    if (aId.iUid == CEikMenuBar::ETypeId)
    {
        if (iDisplayable && iDisplayable->MenuHandler())
        {
            // number of all commands
            TInt numberOfCommands = iDisplayable->CommandCount();

            // If there are no commands we should not call SupplyMopObject
            if (numberOfCommands > 0)
            {
                ret = SupplyMopObject(aId,
                                      iDisplayable->MenuHandler()->Cba(),
                                      iDisplayable->MenuHandler()->MenuBar());
            }
        }
    }

    if (!ret.Pointer())
    {
        ret = CAknTextQueryDialog::MopSupplyObject(aId);
    }
    return ret;
}

/* HandleResourceChange
 *
 * This method is called after a resource change event, for example after
 * screen dynamic resolution change.
 */
void CMIDTextBoxQueryDialog::HandleResourceChange(TInt aType)
{
    CAknTextQueryDialog::HandleResourceChange(aType);
    TRAP_IGNORE(UpdateScrollBarPositionL());
}

/* UpdateScrollBarPositionL
 *
 * This method is called for placing scrollbar to correct place in edwin
 */
void CMIDTextBoxQueryDialog::UpdateScrollBarPositionL()
{
    if (iEditor && iEditor->TextLayout())
    {
        TInt numLines = iEditor->TextLayout()->NumFormattedLines();
        if (numLines == iEditor->MaximumHeightInLines())
        {
            if (iEditor->ScrollBarFrame())
            {
                iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);
            }
        }
        else if (numLines > iEditor->MaximumHeightInLines())
        {
            if (!iEditor->ScrollBarFrame())
            {
                iEditor->CreatePreAllocatedScrollBarFrameL();
                iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
            }
            TRect editorRect = iEditor->Rect();
            if (iEditor->ScrollBarFrame()->VerticalScrollBar())
            {
                editorRect.SetWidth(editorRect.Width() - iEditor->ScrollBarFrame()->VerticalScrollBar()->ScrollBarBreadth());
                iEditor->SetRect(editorRect);
            }
        }
    }
}

void CMIDTextBoxQueryDialog::PreLayoutDynInitL()
{
    CAknQueryDialog::PreLayoutDynInitL();
    UpdateLeftSoftKeyL();
}
// End of file
