/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for TextField and TextBox
*
*/


#include <eikenv.h>
#include <eikappui.h>
#include <coecntrl.h>
// macros for resources
#include <EIKCOCTL.rsg>

#include <AknUtils.h>
// usied for playing error sounds when text input
#include <aknsoundsystem.h>
#include <aknenv.h>
// CAknSettingCache used for obtaining input language
#include <AknSettingCache.h>
#include <AknIndicatorContainer.h>
#include <aknEditStateIndicator.h>

#include <applayout.cdl.h>              // LAF
#include <aknlayoutscalable_apps.cdl.h> // LAF
#include <aknlayoutscalable_avkon.cdl.h> // LAF
#include <AknLayoutFont.h>
// CAknExtendedInputCapabilities for extended input capabilities
#include <aknextendedinputcapabilities.h>

#include <lcdui.rsg>
// Avkon dialog for info notes
#include <aknnotewrappers.h>

// for fetching contacts from phonebook
// CPbk2StoreConfiguration for contact manager
#include <CPbk2StoreConfiguration.h>
// used in CMIDAiwPbk2Client::VPbkSingleContactOperationComplete to retrieving single contact
#include <MVPbkStoreContact.h>
// MVPbkContactLinkArray* iFetchedLinks
#include <CVPbkContactLinkArray.h>
// used in CMIDAiwPbk2Client::VPbkSingleContactOperationComplete
#include <MVPbkContactFieldTextData.h>
// used for open all contact stores
#include <MVPbkContactStoreList.h>
// API for iContactManager
#include <CVPbkContactManager.h>

// aiw used to invoke phonebook
#include <AiwContactSelectionDataTypes.h>

// phone client API classes for starting phone call
#include <AiwCommon.h>
#include <aiwdialdata.h>
// using AIWDialDataExt::KMaximumPhoneNumberLength
#include <aiwdialdataext.h>

// loading string from resource
#include <StringLoader.h>

// FEP CR
// constants for FEP
#include <AknFepInternalCRKeys.h>
#include <centralrepository.h>

#include <j2me/jdebug.h>

#include "CMIDEdwin.h"
#include "CMIDTextFieldItem.h"
#include "CMIDComponentFactory.h"
#include "CMIDUtils.h"
#include "CMIDUIManager.h"

#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDEdwin.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);

// class CMIDEdwin

void CMIDEdwin::ConstructL(const TDesC& aText,TInt aMaxSize)
{

    // Set decimal separator according to the current locale.
    // Note: decimal separators other than ',' and '.' are currently not supported
    TLocale loc;
    iDecimalSeparator = loc.DecimalSeparator();
    if (iDecimalSeparator != ',')
    {
        iDecimalSeparator = '.';
    }

    iEdwinUtils = CMIDEdwinUtils::NewL(this, iDecimalSeparator);

    // if text is invalid according to constraints, throw IllegalArgumentException,
    // except for a PHONENUMBER which only has the invalid characters removed
    if ((iConstraints & MMIDTextField::EConstraintMask) != MMIDTextField::EPhoneNumber
            && !iEdwinUtils->ConstraintsValidForText(aText,iConstraints,ETrue))
    {
        User::Leave(KErrArgument);
    }

    iMaxSize = aMaxSize;
    if (iDisplayable)
    {
        iUIManager = iDisplayable->GetUIManager();
        SetContainerWindowL(*iDisplayable);
        SetAknEditorFlags(EAknEditorFlagEnableScrollBars | AknEdwinFlags());
    }
    else
    {
        iUIManager = iTextFieldItem->GetUIManager();
        // TextField does not have any displayable yet available (before Form.Append())
        // and EikEdwin needs to have a proper container window set, otherwise it may crash in singleLineEditor mode
        CCoeControl::SetContainerWindowL(*(iUIManager->OpenMenuHandlerL()->GetDisplayable()));
    }

    TBool singleLineEditor = (iConstraints & MMIDTextField::EPassword);

    AknEditUtils::ConstructEditingL(this,                           // aEdwin
                                    aMaxSize,                      // aEditingSpace
                                    (singleLineEditor ? 1 : 0),    // aEditingWindow
                                    EAknEditorCharactersLowerCase, // aCharacterCase
                                    EAknEditorAlignLeft,           // aJustification
                                    ETrue,                         // aAllowedToMoveInsertionPoint
                                    ETrue,                         // aCursorYesNo
                                    EFalse);                       // aOverflowYesNo

    if (iConstraints & MMIDTextField::EPassword)
    {
        iEdwinUserFlags |= CEikEdwin::EUserSuppliedText;
        CreatePasswordTextL();
    }

    HBufC* buf = aText.AllocLC();
    CMIDUtils::MapJavaToETextChars(buf);
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
    {
        iEdwinUtils->RemoveNonPhoneNumberChars(buf);
    }
    CEikEdwin::SetTextL(buf);
    CreateTextViewL();
    ProcessModifiers();
    CleanupStack::PopAndDestroy(buf);
    SetFEPModeAndCharFormat();
    AddEdwinObserverL(this);    // see HandleEdwinEventL
    CreateNonMidletCommandsL();


    iUIManager->OpenNaviPaneControllerL()->AddTickerObserverL(this);

    // set the cursor position
    CEikEdwin::SetCursorPosL(TextLength(), EFalse);

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
}

CMIDEdwin::CMIDEdwin(
    TUint aConstraints, MMIDDisplayable* aDisplayable, CMIDTextFieldItem* aTextFieldItem)
        : iDisplayable(static_cast<CMIDDisplayable*>(aDisplayable)),
        iTextFieldItem(aTextFieldItem),
        iConstraints(aConstraints),
        iStateChanged(EFalse),
        iKeyEventsPending(EFalse),
        iInitialCurrentCase(0),
        iInitialCurrentInputMode(0),
        iInitialCurrentLanguage(ELangTest)
{
}

CMIDEdwin::~CMIDEdwin()
{
    //phonebook fetch
    delete iAiwPbkClient;
    //edwin utils
    delete iEdwinUtils;
    if (iUIManager && iUIManager->GetNaviPaneController())
    {
        iUIManager->GetNaviPaneController()->RemoveTickerObserver(this);
    }
    RemoveNonMidletCommands();
}

void CMIDEdwin::DeleteTextL(TInt aOffset,TInt aLength)
{
    // get all initial values before deletion
    TInt oldSize = Size();
    HBufC* oldText = GetTextInHBufL();
    CleanupStack::PushL(oldText);
    // get cursor position before deletion
    TInt cursorPos = CEikEdwin::CursorPos();

    if (IsReadyToDraw())
        ClearSelectionL();

    TInt withoutFEPSize = Size();
    // if size is different then FEP transaction was canceled and some letters
    // are missing - must work with original text
    if (withoutFEPSize != oldSize)
    {
        CEikEdwin::SetTextL(oldText);
    }

    //
    // Do the deletion, then roll back if it leaves invalid text
    //
    iText->DeleteL(aOffset,aLength);
    //
    TBool valid = iEdwinUtils->ConstraintsValidForText(Read(), iConstraints, ETrue);
    if (!valid)
    {
        CEikEdwin::SetTextL(oldText);
    }
    CleanupStack::PopAndDestroy(oldText);

    HandleTextChangedL();
    if (IsReadyToDraw())
        SetSelectionL(aOffset,aOffset);

    if (!valid)
    {
        User::Leave(KErrArgument);
    }

    // Handle cursor moving
    // Cursor is in the middle of delete area
    if ((cursorPos > aOffset) && (cursorPos < (aOffset + aLength)))
    {
        CEikEdwin::SetCursorPosL(aOffset, EFalse);
    }
    // cursor is on the right side of delete area
    else if (cursorPos > (aOffset + aLength))
    {
        TInt newPosition = cursorPos - aLength;
        if (newPosition < 0)
        {
            newPosition = 0;
        }
        CEikEdwin::SetCursorPosL(newPosition, EFalse);
    }
    // cursor is on the left side of delete area
    else
    {
        CEikEdwin::SetCursorPosL(cursorPos, EFalse);
    }
}

void CMIDEdwin::SetTextL(const TDesC& aText)
{
    // if text is invalid according to constraints, throw IllegalArgumentException,
    //  except for a PHONENUMBER which only has the invalid characters removed
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
    {
    }
    else if (!iEdwinUtils->ConstraintsValidForText(aText, iConstraints, ETrue))
    {
        User::Leave(KErrArgument);
    }

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
    CEikEdwin::SetTextL(buf);
    HandleTextChangedL();
    CleanupStack::PopAndDestroy(buf);

    // move cursor to end of the new text
    CEikEdwin::SetCursorPosL(TextLength(), EFalse);
}

/** The first part of this method validates existing text + new text.
Then it carries on with text insertion. */
void CMIDEdwin::InsertTextL(const TDesC& aText, TInt aPosition)
{
    //VALIDATE EXISTING TEXT
    HBufC* temp = NULL;
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EDecimal)
    {
        // Convert decimal content to unlocalized form before testing validity
        // of insertion.
        temp = GetTextInHBufL();
        if (temp)
        {
            CleanupStack::PushL(temp);
            iEdwinUtils->ConvertFromLocalizedDecimal(temp);
        }
    }

    if (aPosition < 0)
        aPosition = 0;

    TInt size = TextLength();
    if (aPosition > size)
        aPosition = size;

    // if text is invalid according to constraints, throw IllegalArgumentException,
    //  except for a PHONENUMBER which only has the invalid characters removed
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
    {
    }
    else if (!iEdwinUtils->ConstraintsValidForInsertedTextL((temp ? temp->Des() : Read()), aText, aPosition, iConstraints, ETrue))
    {
        User::Leave(KErrArgument);
    }
    if (temp)
    {
        CleanupStack::PopAndDestroy(temp);
        temp = NULL;
    }
    //
    //TEXT INSERTION STARTS HERE
    if (IsReadyToDraw())
    {
        ClearSelectionL();
    }

    temp = iEdwinUtils->ConvertToLocalizedLC(aText, iConstraints);


    if (IsNumberConversionNeeded())
    {
        TPtr ptr = temp->Des();
        AknTextUtils::LanguageSpecificNumberConversion(ptr);
    }

    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
    {
        iEdwinUtils->RemoveNonPhoneNumberChars(temp);
    }
    iText->InsertL(aPosition, temp->Des());

    HandleTextChangedL();

    // Get the cursor position and move it forward along with inserter characters, if
    // characters were inserted on the left side of the cursor.
    TInt cursorPos = CEikEdwin::CursorPos();
    TInt numberOfAddedCharacters = TextLength() - size;

    if (cursorPos >= aPosition)
    {
        if ((cursorPos + numberOfAddedCharacters) < iMaxSize)
        {
            CEikEdwin::SetCursorPosL(cursorPos + numberOfAddedCharacters, EFalse);
        }
    }

    UpdateTextCapacityIndicatorValueL();

    CleanupStack::PopAndDestroy(temp);
}

/** Not used in current implementation; functionality
    provided in CMIDEdwinControl and CMIDTextFieldItem
   */
void CMIDEdwin::SetConstraintsL(TUint /*aConstraints*/)
{
}

void CMIDEdwin::SetTextWithNewConstraintsL(HBufC* aText)
{
    // If the current contents of the TextBox do not match the new constraints,the
    // contents are set to empty, except for a PHONENUMBER which only has the
    // invalid characters removed. Otherwise, set the text as given.
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EPhoneNumber)
    {
        iEdwinUtils->RemoveNonPhoneNumberChars(aText);
        CEikEdwin::SetTextL(aText);
    }
    else if (!iEdwinUtils->ConstraintsValidForText(aText->Des(), iConstraints & MMIDTextField::EConstraintMask, ETrue))
    {
        CEikEdwin::SetTextL(&KNullDesC);
    }
    else
    {
        CEikEdwin::SetTextL(aText);
    }
    HandleTextChangedL();
}


TInt CMIDEdwin::SetMaxSizeL(TInt aMaxSize)
{
    TInt size = TextLength();
    if (aMaxSize < size)
    {
        DeleteTextL(aMaxSize,size-aMaxSize);
    }
    iMaxSize = aMaxSize;
    SetTextLimit(iMaxSize);
    return iMaxSize;
}

TInt CMIDEdwin::GetMaxSize()
{
    return iMaxSize;
}


TInt CMIDEdwin::Size()
{
    TRAP_IGNORE(ResetNumericContentL());
    return TextLength();
}

TInt CMIDEdwin::GetCaretPosition()
{
    TRAP_IGNORE(ResetNumericContentL());
    return CursorPos();
}

/** For a NUMERIC TextBox, clear the contents if the "displayed contents" are
    in a state that is not valid "actual contents" for the current constraints.
*/
void CMIDEdwin::ResetNumericContentL()
{
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::ENumeric)
    {
        HBufC* buf = GetTextInHBufL();
        if (buf != NULL)
        {
            CleanupStack::PushL(buf);
            if (!iEdwinUtils->ConstraintsValidForText(buf->Des(), iConstraints, ETrue))
            {
                (buf->Des()).SetLength(0);
                CEikEdwin::SetTextL(buf);
            }
            CleanupStack::PopAndDestroy(buf);
        }
    }
}

HBufC* CMIDEdwin::GetTextL()
{
    HBufC* buf = GetTextInHBufL();
    if (!buf)
    {
        buf = HBufC::NewL(0);
    }
    // convert decimal from localized to MIDlet-visible representation
    if ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EDecimal)
    {
        iEdwinUtils->ConvertFromLocalizedDecimal(buf);
    }

    if (IsNumberConversionNeeded())
    {
        TPtr ptr = buf->Des();
        AknTextUtils::ConvertDigitsTo(ptr, EDigitTypeWestern);
    }

    if (!iEdwinUtils->ConstraintsValidForText(buf->Des(), iConstraints, ETrue))
    {
        // The "displayed contents" of the TextBox may be in a state that is
        // not valid "actual contents" for the current constraints; in that
        // case, return an empty string and clear the TextBox contents.
        (buf->Des()).SetLength(0);
        CEikEdwin::SetTextL(buf);
    }
    else
    {
        // Otherwise, return a copy of the contents where newlines etc. are
        // mapped to their Java representations.
        CMIDUtils::MapETextToJavaChars(buf);
    }
    return buf;
}


void CMIDEdwin::CreatePasswordTextL()
{
    CGlobalText* oldGText = STATIC_CAST(CGlobalText*,iText);
    CPasswordBoxGlobalText* text=new(ELeave) CPasswordBoxGlobalText
    (oldGText->GlobalParaFormatLayer(),oldGText->GlobalCharFormatLayer(),*this);
    CleanupStack::PushL(text);
    text->ConstructL();
    CleanupStack::Pop(text);

    delete iText;
    iText = NULL;

    SetDocumentContentL(*text,CEikEdwin::EUseText);
}

/**
 * Need to recreate TextView here such that it uses the right window
 */
void CMIDEdwin::SetContainerWindowL(const CCoeControl& aContainer)
{
    SetObserver(this); //Prevent MCoeControlObserver::EEventStateChanged going to Form

    CEikEdwin::SetContainerWindowL(aContainer);
}

TKeyResponse CMIDEdwin::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
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
    TBool losingFocus = EFalse;

    CMIDMenuHandler* menuHandler = iUIManager->GetMenuHandler();
    // Enter Key
    //
    //TextFieldItem
    if (iTextFieldItem)
    {
        // QWERTY and VKB Enter Key
        if (scanCode == EStdKeyEnter && aType == EEventKeyDown && !useEnterKey)
        {
            useEnterKey=true;
        }
        if ((scanCode ==    EStdKeyEnter && aType == EEventKeyUp && useEnterKey) ||
                (aType == EEventKey && aKeyEvent.iCode == EKeyEnter))
            //TextFieldItem with NUMERIC,DECIMAL and PHONENUMBER constraint
            if (IsConstraintSet(MMIDTextField::ENumeric) ||
                    IsConstraintSet(MMIDTextField::EDecimal) ||
                    IsConstraintSet(MMIDTextField::ENumeric + MMIDTextField::EPassword))
            {
                useEnterKey=false;
                //Activate default command
                if (iTextFieldItem->DefaultCommand())
                {
                    menuHandler->ProcessCommandL(
                        iTextFieldItem->CommandList()->CommandOffset() +
                        iTextFieldItem->CommandList()->FindCommandIndex(iTextFieldItem->DefaultCommand()));
                }
                // Show ctx menu or activate command
                else
                {
                    menuHandler->ShowMenuL(CMIDMenuHandler::EOkMenu);
                    if (menuHandler->MenuItemsCount() <= 1)
                        menuHandler->ProcessCommandL(iTextFieldItem->CommandList()->CommandOffset());
                }

                return EKeyWasConsumed;

            }
            else
            {
                //TextFieldItem with PHONENUMBER constraint
                if (IsConstraintSet(MMIDTextField::EPhoneNumber) ||
                        IsConstraintSet(MMIDTextField::EPhoneNumber + MMIDTextField::EPassword))
                {
                    useEnterKey = false;
                    //Activate default command
                    if (iTextFieldItem->DefaultCommand())
                    {
                        menuHandler->ProcessCommandL(
                            iTextFieldItem->CommandList()->CommandOffset() +
                            iTextFieldItem->CommandList()->FindCommandIndex(iTextFieldItem->DefaultCommand()));
                    }
                    // Show options menu or activate command
                    else
                    {
                        menuHandler->ShowMenuL(CMIDMenuHandler::EOkMenu);
                        if (menuHandler->MenuItemsCount() == 1)
                        {
                            menuHandler->ProcessCommandL(iTextFieldItem->CommandList()->CommandOffset());
                        }
                    }
                    return EKeyWasConsumed;
                }
                else
                {
                    //TextFieldItem with ANY,PASSWORD  constraint
                    if (iConstraints == MMIDTextField::EPassword)
                    {
                        useEnterKey=false;
                        //Activate default command
                        if (iTextFieldItem->DefaultCommand())
                        {
                            menuHandler->ProcessCommandL(
                                iTextFieldItem->CommandList()->CommandOffset() +
                                iTextFieldItem->CommandList()->FindCommandIndex(iTextFieldItem->DefaultCommand()));
                        }
                        // Show options menu or activate command
                        else
                        {
                            menuHandler->ShowMenuL(CMIDMenuHandler::EOkMenu);
                            if (menuHandler->MenuItemsCount() == 1)
                            {
                                menuHandler->ProcessCommandL(
                                    iTextFieldItem->CommandList()->CommandOffset());
                            }
                        }
                        return EKeyWasConsumed;

                    }

                }
            }
        if (aKeyEvent.iCode ==  EKeyEnter && aType == EEventKey)
        {
            //TextFieldItem with EMAILADDR constraint
            if (IsConstraintSet(MMIDTextField::EMailAddr) ||
                    IsConstraintSet(MMIDTextField::EUrl))
            {
                //Show menu or activate command
                if (!iTextFieldItem->DefaultCommand())
                {
                    menuHandler->ShowMenuL(CMIDMenuHandler::EOkMenu);
                    if (menuHandler->MenuItemsCount() <= 1)
                    {
                        menuHandler->ProcessCommandL(
                            iTextFieldItem->CommandList()->CommandOffset());
                    }
                    return EKeyWasConsumed;
                }

            }
        }
    }
    // End Enter Key
    //

    //
    // keyboard menu
    //
    switch (scanCode)
    {
    case EStdKeyF1:
    case EStdKeyMenu:
        losingFocus=ETrue;
    }
    //
    // Pause the ticker for five sec if # key received
    if (aKeyEvent.iScanCode == EStdKeyHash && !IsReadOnly()) //0x7f
    {
        iUIManager->OpenNaviPaneControllerL()->PauseTickerL(TICKER_PAUSE_INTERVAL, this);
    }
    //
    // Non keyboard zoom,menu,etc.
    //
    if (scanCode>=ESpecialKeyBase && scanCode<(ESpecialKeyBase+ESpecialKeyCount))
        losingFocus=ETrue;
    //
    // CBA buttons:
    //
    if (scanCode>=EStdKeyDevice0 && scanCode<=EStdKeyDeviceF)
        losingFocus=ETrue;
    //
    if (iTextFieldItem && losingFocus && iStateChanged)
    {
        iTextFieldItem->HandleControlEventL(this,MCoeControlObserver::EEventStateChanged);
        iStateChanged=EFalse;
    }

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
    if ((aType == EEventKey) && (iConstraints & MMIDTextField::EPassword)
#else
    if ((aType == EEventKeyDown) && (iConstraints & MMIDTextField::EPassword)
#endif // RD_SCALABLE_UI_V2
            && iTextFieldItem) // type == TextField
    {
        static_cast<CPasswordBoxGlobalText*>(iText)->RejectNextKey(scanCode == EStdKeyEnter);
    }

#ifdef RD_SCALABLE_UI_V2
    if (IsConstraintSet(MMIDTextField::ENumeric) && (aType == EEventKey || aType == EEventKeyUp) && !IsReadOnly() &&
#else
    if (IsConstraintSet(MMIDTextField::ENumeric) && (aType == EEventKeyUp) && !IsReadOnly() &&
#endif // RD_SCALABLE_UI_V2
            ((scanCode==EStdKeyNkpAsterisk) || (scanCode == EStdKeyMinus) || (scanCode==EStdKeyNkpMinus) || (scanCode == 0x2A) || (scanCode == 0x2D) ||
             (TChar(aKeyEvent.iCode) == TChar('-') && scanCode != EStdKeyMinus)))
    {
        HandleMinusCharEventL(EEventTextUpdate);
        CEikEdwin::ReportEdwinEventL(EEventTextUpdate);
        UpdateTextCapacityIndicatorValueL();
        return EKeyWasConsumed;
    }

    //handling of qwerty keypad "-" or "." pressing in Decimal mode
#ifdef RD_SCALABLE_UI_V2
    if (IsConstraintSet(MMIDTextField::EDecimal) && (aType == EEventKey) && !IsReadOnly() &&
#else
    if (IsConstraintSet(MMIDTextField::EDecimal) && (aType == EEventKeyUp) && !IsReadOnly() &&
#endif // RD_SCALABLE_UI_V2
            ((scanCode==EStdKeyMinus) || (scanCode==EStdKeyFullStop)))
    {
        CPlainText* res = CEikEdwin::Text();
        TInt textLength = CEikEdwin::TextLength();
        if (scanCode == EStdKeyMinus && textLength < iMaxSize)
        {
            res->InsertL(CEikEdwin::CursorPos(), KMinusChar);
        }
        else if (scanCode == EStdKeyFullStop && textLength < iMaxSize)
        {
            res->InsertL(CEikEdwin::CursorPos(), KFullStopChar);
        }

        HandleTextChangedL(); // notify editor about the text changes
        CEikEdwin::ReportEdwinEventL(EEventTextUpdate);
        TInt cursorPos = CEikEdwin::CursorPos();

        if (CEikEdwin::TextLength() < iMaxSize)
        {
            CEikEdwin::SetCursorPosL(cursorPos + 1, EFalse);
        }
        else if (cursorPos == (iMaxSize - 1) && cursorPos == textLength &&
                 (scanCode == EStdKeyFullStop || scanCode == EStdKeyMinus))
        {
            CEikEdwin::SetCursorPosL(iMaxSize, EFalse);
        }
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
                && !IsReadOnly() && IsConstraintSet(MMIDTextField::ENumeric))
        {
            //Do nothing here in case of minus char toggle
        }
        else if (IsReadOnly())
        {
            iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
            UpdateTextCapacityIndicatorValueL();
            return EKeyWasConsumed;
        }

        // All possible sources of warning beep are separated to be easy to read
        else if (TextLength() >= iMaxSize)
        {
            TInt ret = KErrNone;
            TInt inputModeValue = AknEditorCurrentInputMode();
            if (inputModeValue != EAknEditorTextInputMode)
            {
                iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
                UpdateTextCapacityIndicatorValueL();
                return EKeyWasConsumed;
            }

            TInt aknRepositoryValue = 0;
            CRepository* aknFepRepository = NULL;
            TRAP(ret, aknFepRepository = CRepository::NewL(KCRUidAknFep));
            if ((ret != KErrNone) || (!aknFepRepository))
            {
                // if we cannot get the repository (something is wrong), play nothing
                UpdateTextCapacityIndicatorValueL();
                return EKeyWasConsumed;
            }
            aknFepRepository->Get(KAknFepPredTxtFlag, aknRepositoryValue);

            delete aknFepRepository;
            aknFepRepository = NULL;

            if (aknRepositoryValue == 1)    // 1 means the predictive input is on
            {
                iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
                UpdateTextCapacityIndicatorValueL();
                return EKeyWasConsumed;
            }
            // now only nonpredictive textinput is left
            if (iThisMultitapKey != iLastMultitapKey) // different key tapped -> no multitap
            {
                iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
                UpdateTextCapacityIndicatorValueL();
                iThisMultitapKey = 0;
                iLastMultitapKey = 0;
                return EKeyWasConsumed;
            }
        }
    }

    if (aType != EEventKey && aKeyEvent.iScanCode != EStdKeyApplication0)
    {
        UpdateTextCapacityIndicatorValueL();
        return EKeyWasConsumed;
    }


    TKeyResponse response = EKeyWasNotConsumed;

    // Error tone playing case2:
    // Play error tone if TextBox/TextField is read-only or maximum length has been reached.
    // Here is handling of full keyboard keys(NOT 0...9) and all virtual keyboard keys
    // (camera and menu key not included).
    // (Note: Virtual keyboard sends only EEventKey type events, not up or down events)
    // (Note: Error tone is played when there is no text to be replaced i.e. no text has been painted)
    if (!iEdwinUtils->IsNavigationKey(aKeyEvent) && !iEdwinUtils->IsHotKeyL(aKeyEvent, iCoeEnv) && aKeyEvent.iCode != EKeyYes &&
            (!iKeyEventsPending || (scanCode < KKeyQwerty0  || scanCode > KKeyQwerty9)) &&
            (aKeyEvent.iCode != EKeyApplication0 && scanCode != EStdKeyApplication0 &&
             aKeyEvent.iCode != EKeyApplication19 && scanCode != EStdKeyApplication19))
    {
        if (IsReadOnly() || (TextLength() >= iMaxSize && aKeyEvent.iCode != EKeyBackspace))
        {
            //SelectionLength() > 0 if text has been selected/painted
            if (SelectionLength() == 0)
            {
                iAvkonAppUi->KeySounds()->PlaySound(EAvkonSIDErrorTone);
            }
            response = CEikEdwin::OfferKeyEventL(aKeyEvent,aType);
            UpdateTextCapacityIndicatorValueL();
            return response;
        }
    }

    TBool valid = EFalse;
    //
    if (iEdwinUtils->IsNavigationKey(aKeyEvent) || iEdwinUtils->IsHotKeyL(aKeyEvent, iCoeEnv))
    {
        HBufC* oldText = GetTextInHBufL();
        CleanupStack::PushL(oldText);
        TCursorSelection sel = Selection();
        //
        response = CEikEdwin::OfferKeyEventL(aKeyEvent,aType);
        valid = iEdwinUtils->ConstraintsValidForText(iText?Read():TPtrC(),iConstraints,EFalse);
        //
        if (!valid)
        {
            CEikEdwin::SetTextL(oldText);
            HandleTextChangedL();
            SetSelectionL(sel.iCursorPos,sel.iAnchorPos);
            response = EKeyWasConsumed;
        }
        //
        CleanupStack::PopAndDestroy(oldText);
        UpdateTextCapacityIndicatorValueL();
        return response;
    }
    else
    {
        TBuf<1> key;
        key.Append(TChar(aKeyEvent.iCode));
        valid = iEdwinUtils->ConstraintsValidForInsertedTextL(iText?Read():TPtrC(), key, CursorPos(), iConstraints, EFalse);
        if (valid)
        {
            if (iTextFieldItem && (aKeyEvent.iCode == EKeyEnter) &&
                    (((iConstraints & MMIDTextField::EConstraintMask) !=  MMIDTextField::EAny) ||
                     (iConstraints & MMIDTextField::EPassword)))
            {//enter key for NON ANY or ANY | PASSWORD editors
                return EKeyWasNotConsumed; //must do command triggering in this case
            }

            response = CEikEdwin::OfferKeyEventL(aKeyEvent,aType);
            UpdateTextCapacityIndicatorValueL();
        }
        else
        {
            // If minus char was entered in full querty editor mode
            if (IsConstraintSet(MMIDTextField::EDecimal) &&
                    (aType == EEventKey) && !IsReadOnly() &&
                    (TChar(aKeyEvent.iCode) == TChar('-') && scanCode != EStdKeyMinus))
            {
                CPlainText* res = CEikEdwin::Text();

                if (res && TChar(aKeyEvent.iCode) == TChar('-') &&
                        TextLength() < iMaxSize)
                {
                    TInt textLength = TextLength();
                    res->InsertL(GetCaretPosition(), KMinusChar);
                    HandleTextChangedL(); // notify editor about the text changes
                    CEikEdwin::ReportEdwinEventL(EEventTextUpdate);
                    TInt cursorPos = GetCaretPosition();

                    if (TextLength() < iMaxSize)
                    {
                        CEikEdwin::SetCursorPosL(GetCaretPosition() + 1, EFalse);
                    }
                    else if (cursorPos == (iMaxSize - 1) && cursorPos == textLength)

                    {
                        CEikEdwin::SetCursorPosL(iMaxSize, EFalse);
                    }
                }
            }

        }
        return response;
    }
}

void CMIDEdwin::FocusChanged(TDrawNow aDrawNow)
{
    CEikEdwin::FocusChanged(aDrawNow);
    TBool isFocused = IsFocused();
    TInt  ret = KErrNone;

    // in case of TextFieldItem iDisplayable is not valid do not use it.
    if (!iDisplayable && iTextFieldItem->Form() == NULL)
    {
        return;
    }

    if (!IsReadOnly())
    {
        if (isFocused)
        {
            TRAP(
                ret,
                iUIManager->OpenNaviPaneControllerL()->PauseTickerL(TICKER_PAUSE_INTERVAL, this));
        }
        else
        {
            TRAP(
                ret,
                iUIManager->OpenNaviPaneControllerL()->PauseTickerL(0, this));
        }
    }
    //
    // Post ItemStateChangedEvent if focus moves away
    //
    if (iTextFieldItem && !isFocused && (iStateChanged))
    {
        TRAP(ret,iTextFieldItem->HandleControlEventL(this,MCoeControlObserver::EEventStateChanged));
        iStateChanged=EFalse;
    }

    // Show text input indicator is the last thing we want to do
    TRAP(ret,UpdateTextCapacityIndicatorValueL());
}

void CMIDEdwin::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent aEventType)
{
    switch (aEventType)
    {
    case MCoeControlObserver::EEventStateChanged:
        iStateChanged=ETrue;
        break;
    default:
        break;
    }
}

void CMIDEdwin::ProcessModifiers()
{
    // set general edwin attributes according to TextBox constraints
    SetReadOnly(iConstraints & MMIDTextField::EUneditable);
}

void CMIDEdwin::HandleMinusCharEventL(TEdwinEvent aEventType)
{

    if ((aEventType == EEventTextUpdate) && ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::ENumeric))
    {
        ClearSelectionL();
        CPlainText* res = CEikEdwin::Text();
        TBuf<1> firstChar;
        if (CEikEdwin::TextLength() != 0)
        {
            res->Extract(firstChar,  0, 1);
        }
        TInt cursorPos = CEikEdwin::CursorPos();
        //toggle '-' char in the first position
        TInt pos = firstChar.Locate(TChar('-'));
        if (pos == KErrNotFound)
        {
            if (CEikEdwin::TextLength() < iMaxSize)
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
        HandleTextChangedL(); // notify editor about the text changes
        CEikEdwin::SetCursorPosL(cursorPos, EFalse);
    }
}

void CMIDEdwin::HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType)
{
    if (iTextFieldItem && aEventType == MEikEdwinObserver::EEventTextUpdate)
    {
        // notify Java that text has changed with itemStateChanged()
        iTextFieldItem->HandleControlEventL(aEdwin, MCoeControlObserver::EEventStateChanged);
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

        TBool textChanged = EFalse;

        // in DECIMAL mode, don't allow changes that would result
        // in an illegal string;
        //  1) if there is a minus, it must be the first character
        //  2) there can be at most one decimal separator
        if ((iConstraints &
                MMIDTextField::EConstraintMask) == MMIDTextField::EDecimal)
        {
            HBufC* res = NULL;
            TRAPD(err, { res = GetTextInHBufL();});
            if (err != KErrNone || !res)
            {
                return;
            }
            CleanupStack::PushL(res);

            TInt illegalCharPos = -1;
            TPtr16 ptr = res->Des();
            TInt minusPos = ptr.LocateReverse(TChar('-'));
            TInt endCursorPos = CEikEdwin::CursorPos();

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
                    CEikEdwin::SetTextL(res);
                    textChanged = ETrue;
                }
            }

            TInt pointPosL = ptr.Locate(iDecimalSeparator);
            TInt pointPosR = ptr.LocateReverse(iDecimalSeparator);
            TInt cursorPos = CEikEdwin::CursorPos();
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
                CEikEdwin::SetTextL(res);
                textChanged = ETrue;
                if (cursorPos >= 1)
                {
                    if (cursorPos == illegalCharPos)
                    {
                        cursorPos--;
                    }
                    endCursorPos = cursorPos;
                }
            }
            CEikEdwin::SetCursorPosL(endCursorPos, EFalse);
            CleanupStack::Pop(res);
            delete res;
        }
        else if (((iConstraints &
                   MMIDTextField::EConstraintMask) == MMIDTextField::EMailAddr)||
                 ((iConstraints &
                   MMIDTextField::EConstraintMask) == MMIDTextField::EUrl))
        {
            HBufC* res = NULL;
            TRAPD(err, { res = GetTextL();});
            if (err != KErrNone || !res)
            {
                return;
            }
            CleanupStack::PushL(res);

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
                tmpPos = CEikEdwin::CursorPos();
                SetTextL(*res);
                CEikEdwin::SetCursorPosL(tmpPos, EFalse);
            }

            textChanged = ETrue;
            CleanupStack::Pop(res);
            delete res;
        }

        // if there were any modifications the text is updated
        if (textChanged)
        {
            HandleTextChangedL();
        }
    }


}

void CMIDEdwin::SetInitialInputModeL(const TDesC& aCharacterSubset)
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
        //Setting permited case mode for TextField in ANY do panic
        //when TextField loses focus.
        if (iTextFieldItem && iConstraints == MMIDTextField::EAny)
        {
            return;
        }
        // MIDP_UPPERCASE_LATIN or MIDP_LOWERCASE_LATIN are ignored if
        // INITIAL_CAPS_SENTENCE or INITIAL_CAPS_WORD modifier in ANY.
        if (!(iConstraints & MMIDTextField::EInitialCapsWordSentence ||
                iConstraints & MMIDTextField::EInitialCapsWord) ||
                constraint != MMIDTextField::EAny)
        {
            // If initial input mode is uppercase or lowercase then permit
            // only explicit case mode changes, automatic changes are not
            // allowed.
            SetAknEditorPermittedCaseModes(EAknEditorUpperCase |
                                           EAknEditorLowerCase);
        }
    }
    else
    {
        SetAknEditorPermittedCaseModes(EAknEditorAllCaseModes);
    }

}


TBool CMIDEdwin::IsNumberConversionNeeded()
{
    return iEdwinUtils->IsNumberConversionNeeded(GetConstraints());
}

void CMIDEdwin::SetFEPModeAndCharFormat()
{
    iEdwinUtils->SetFEPModeAndCharFormat(iConstraints, this);
}

/** Create non-midlet commands according to the constraints and add them
to either the displayable or to the item. Commands will be processed in ProcessCommandL(). */
void CMIDEdwin::CreateNonMidletCommandsL()
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
void CMIDEdwin::AddCommandToEdwinL(TInt aCommandResIdShort,
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
void CMIDEdwin::AddCommandToEdwinL(TInt aCommandResId, TInt aCommandId)
{
    AddCommandToEdwinL(aCommandResId, aCommandResId, aCommandId);
}

/** This method is called from the destructor and removes
    the commands added by CreateNonMidletCommandsL(). To do
    this we examine cmds on the list (either of the displayable
    or the item as applicable) and remove those whose observer is us.
    */
void CMIDEdwin::RemoveNonMidletCommands()
{
    CMIDCommandList* list = NULL;
    if (iDisplayable)
    {
        list = STATIC_CAST(CMIDDisplayable*, iDisplayable)->MainCommandList();
    }
    else if (iTextFieldItem)
    {
        list = STATIC_CAST(CMIDTextFieldItem*, iTextFieldItem)->CommandList();
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

/** Process non-midlet commands according to their unique negative id. See
MMIDCommandObserver, FetchFromPhoneBoolL() and CreateNonMidletCommandsL(). */
TBool CMIDEdwin::ProcessCommandL(CMIDCommand* aCommand)
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

void CMIDEdwin::CreatePhoneCallL()
{
    iEdwinUtils->CreatePhoneCallL(TextLength(), CCoeEnv::Static()->WsSession(), this);
}

/** From MTickerObserver: If the ticker is showing disable the displaying of
edit indicators in the navi pane.  **/
void CMIDEdwin::TickerShowing(TBool aShowing)
{
    TInt flags = aShowing ? EAknEditorFlagNoEditIndicators | AknEdwinFlags() :
                 ~EAknEditorFlagNoEditIndicators & AknEdwinFlags();

    SetAknEditorFlags(flags);
}

void CMIDEdwin::HandleCurrentL(TBool aActivate)
{
    // when becoming the current Displayable, set editor input mode
    // to the one set by SetInitialInputModeL
    if (aActivate &&
            ((iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EAny ||
             (iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EMailAddr ||
             (iConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EUrl))
    {
        if (iInitialCurrentLanguage)
        {
            SetAknEditorLocalLanguage(iInitialCurrentLanguage);
            if (iInitialCurrentInputMode == 0)
            {
                // clears any number mode used previously
                SetAknEditorCurrentInputMode(EAknEditorNullInputMode);
            }
        }

        if (iInitialCurrentInputMode)
        {
            SetAknEditorCurrentInputMode(iInitialCurrentInputMode);
        }

        if (iInitialCurrentCase)
        {
            TInt initialCurrentCase = iInitialCurrentCase;
            if ((iConstraints & MMIDTextField::EPassword ||
                    iConstraints & MMIDTextField::EMailAddr ||
                    iConstraints & MMIDTextField::EUrl) &&
                    iInitialCurrentCase == EAknEditorTextCase)
            {
                // Text case is not used in passwords, emailaddrs and urls
                initialCurrentCase = EAknEditorLowerCase;
            }
            SetAknEditorCurrentCase(initialCurrentCase);
            SetAknEditorCase(initialCurrentCase);
        }
    }
    if (aActivate)
    {
        UpdateTextCapacityIndicatorValueL();
    }
}

/** Update the text capacity indicator if we have focus and are not read only.*/
void CMIDEdwin::UpdateTextCapacityIndicatorValueL()
{
    TBool isActive = iDisplayable ? iDisplayable->IsActive() :
                     (iTextFieldItem->Form() ? iTextFieldItem->Form()->CurrentDisplayable().IsActive() : EFalse);

    if (IsFocused() && isActive && !(iConstraints & MMIDTextField::EUneditable))
    {
        // Navi pane controller is opened on constructor phase so it should exists
        iUIManager->OpenNaviPaneControllerL()->UpdateTextInputIndicatorL(
            iMaxSize - TextLength());
        iDisplayingCapacityIndic = ETrue;
    }
    else if (iDisplayingCapacityIndic)
    {
        // Navi pane controller is opened on constructor phase so it should exists
        iUIManager->OpenNaviPaneControllerL()->UpdateTextInputIndicatorL(-1);
        iDisplayingCapacityIndic = EFalse;
    }
}

/** Updates capacity indicator when text is pasted. */
void CMIDEdwin::HandleTextPastedL(TInt aStartPos,TInt& aLength)
{
    // When TextBox field is NUMERIC, PHONENUMBER or DECIMAL new line
    // characters if exists in copied text must be filtered out before pasting

    if ((IsConstraintSet(MMIDTextField::ENumeric) ||
            IsConstraintSet(MMIDTextField::EPhoneNumber) ||
            IsConstraintSet(MMIDTextField::EDecimal)) != EFalse)
    {
        HBufC* hbuf = GetTextInHBufL();
        TPtr strPtr = hbuf->Des();
        TInt firstPos = strPtr.Find(KLineSeparator);

        if (firstPos != KErrNotFound)
        {
            CleanupStack::PushL(hbuf);
            while (firstPos != KErrNotFound)
            {
                strPtr.Delete(firstPos, KLineSeparator().Length());
                firstPos = strPtr.Find(KLineSeparator);
            }
            CEikEdwin::SetTextL(hbuf);
            CleanupStack::Pop(hbuf);
        }
        delete hbuf;
    }
    UpdateTextCapacityIndicatorValueL();
    CEikEdwin::HandleTextPastedL(aStartPos, aLength);
}

TPtrC CMIDEdwin::Read() const
{
    if (iConstraints & MMIDTextField::EPassword)
    {
        return static_cast<CPasswordBoxGlobalText*>(iText)->ClearText();
    }
    else
    {
        return iText->Read(0,TextLength());
    }
}
