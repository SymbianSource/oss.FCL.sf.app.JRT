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
* Description:  Keymappings etc. customized for Series 60.
*
*/


// INCLUDE FILES
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>

#include <CPbk2SortOrderManager.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MPbk2ContactNameFormatter.h>

// Avkon dialog for info notes
#include <aknnotewrappers.h>

// for fetching contacts from phonebook
#include <CPbk2StoreConfiguration.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactManager.h>

// aiw used to invoke phonebook
#include <AiwContactSelectionDataTypes.h>

// phone client API classes for starting phone call
#include <AiwCommon.h>
#include <aiwdialdata.h>
#include <aiwdialdataext.h>

// FeatureManager::FeatureSupported(KFeatureIdThai) used in SetInitialInputModeL
#include <featmgr.h>
// CAknExtendedInputCapabilities for extended input capabilities
#include <aknextendedinputcapabilities.h>

#include <StringLoader.h>


#include <eikenv.h>
#include <eikappui.h>
#include <coecntrl.h>

#include <eikcoctl.rsg>

#include <aknenv.h>
#include <AknUtils.h>
#include <AknSettingCache.h>

#include <aknappui.h>

#include <AknsUtils.h>

#include <j2me/jdebug.h>

#include "CMIDKeyDecoder.h"
#include "CMIDEdwinUtils.h"

#include <lcdui.rsg>

#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDEdwin.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);

CMIDEdwinUtils* CMIDEdwinUtils::NewL(MMIDTextBox* aTextBox, const TChar aDecimalSeparator)
{
    CMIDEdwinUtils* utils = new(ELeave) CMIDEdwinUtils(aTextBox, aDecimalSeparator);
    CleanupStack::PushL(utils);
    utils->ConstructL();
    CleanupStack::Pop(utils);
    return utils;
}

CMIDEdwinUtils::CMIDEdwinUtils(MMIDTextBox* aTextBox, const TChar aDecimalSeparator)
        : iTextBox(aTextBox), iDecimalSeparator(aDecimalSeparator)
{}

void CMIDEdwinUtils::ConstructL()
{
}

/**
 * Checks whether the given text is valid within the given constraints.
 * When the contents of the TextBox are set PROGRAMMATICALLY (parameter aActualContents
 * is true), constraint validity is checked more strictly, as some constraints
 * allow certain strings while the user is typing but not as the actual contents.
 */
TBool CMIDEdwinUtils::ConstraintsValidForText(const TDesC& aText,TUint aConstraints, TBool aActualContents)
{
    TUint constraint = aConstraints & MMIDTextField::EConstraintMask;
    if (constraint == MMIDTextField::EAny)
        return ETrue;
    //
    TBool valid = ETrue;
    switch (constraint)
    {
    case MMIDTextField::EMailAddr:
    case MMIDTextField::EUrl:
        valid = IsTextValid(aText);
        break;
    case MMIDTextField::ENumeric:
        if (aActualContents)
        {
            valid = IsJavaInteger(aText);
        }
        else
        {
            valid = IsNumeric(aText);
        }
        break;
    case MMIDTextField::EPhoneNumber:
        valid = CMIDEdwinUtils::IsPhoneNumber(aText);
        break;
    case MMIDTextField::EDecimal:
        if (aActualContents)
        {
            valid = IsJavaFloatingPointLiteral(aText);
        }
        else
        {
            valid = IsDecimal(aText, iDecimalSeparator);
        }
        break;
    default:
        ASSERT(EFalse);
    }
    return valid;
}

TBool CMIDEdwinUtils::ConstraintsValidForInsertedTextL(const TDesC& aText, const TDesC& aTextToInsert, TInt aPos, TUint aConstraints, TBool aActualContents)
{
    HBufC* text = HBufC::NewL(aText.Length() + aTextToInsert.Length());
    TPtr ptr = text->Des();
    ptr.Copy(aText);
    ptr.Insert(aPos,aTextToInsert);
    TBool valid = ConstraintsValidForText(ptr, aConstraints, aActualContents);
    delete text;
    return valid;
}


TBool CMIDEdwinUtils::IsHotKeyL(const TKeyEvent& aKeyEvent, CCoeEnv* aCoeEnv)
{
    TBuf<24> buf;
    if (aKeyEvent.iModifiers & EModifierShift)
    {
        aCoeEnv->ReadResourceL(buf,R_EIK_EDWIN_SHIFT_CTRL_HOTKEYS);
    }
    else
    {
        aCoeEnv->ReadResourceL(buf,R_EIK_EDWIN_CTRL_HOTKEYS);
    }
    const TInt ret=buf.Locate(TChar(aKeyEvent.iCode+'a'-1));
    switch (ret)
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

TBool CMIDEdwinUtils::IsNavigationKey(const TKeyEvent& aKeyEvent)
{
    switch (aKeyEvent.iCode)
    {
    case EKeyDownArrow:
    case EKeyUpArrow:
    case EKeyLeftArrow:
    case EKeyRightArrow:
    case EKeyBackspace:
    case EKeyDelete:
    case EKeyEscape:
    case EKeyPageUp:
    case EKeyPageDown:
    case EKeyHome:
    case EKeyEnd:
        return ETrue;
    default:
        return EFalse;
    }
}

TBool CMIDEdwinUtils::IsPhoneNumber(const TDesC& aText)
{
    TInt pos = aText.Length();
    for (; pos>0;)
    {
        TChar ch((aText)[--pos]);
        // allow also digits in non-western numbers
        if (IsNumeric(ch))
            continue;

        switch (ch)
        {
        case '+':
        case '*':
        case '#':
        case 'p':
        case 'w':
            break;
        default:
            return EFalse;
        }
    }
    return ETrue;
}

/**
 * Tests whether a typed string is a legal NUMERIC string. This test is more
 * loose than that in IsJavaInteger(), as "-" is a
 * legal text value during typing but not when set programmatically.
 */
TBool CMIDEdwinUtils::IsNumeric(const TDesC& aText)
{
    TInt length = aText.Length();
    TInt pos = length;
    for (; pos>0;)
    {
        TChar ch((aText)[--pos]);
        //
        if (IsNumeric(ch))
            continue;
        //
        switch (ch)
        {
        case '-':
            if (pos != 0)
                return EFalse;
            break;
        default:
            return EFalse;
        }
    }
    return ETrue;
}

/**
* Tests whether a character is legal number, correct values
* are 0-9 and all language variants (arabic etc.)
*/
TBool CMIDEdwinUtils::IsNumeric(const TChar& aChar)
{
    return ((aChar >= '0' && aChar <= '9')            // Western
            || (aChar >= 0x0660 && aChar <= 0x0669)  // Arabic-Indic
            || (aChar >= 0x06F0 && aChar <= 0x06F9)  // Extended Arabic-Indic
            || (aChar >= 0x0966 && aChar <= 0x096F)  // Devanagari
            || (aChar >= 0xFF10 && aChar <= 0xFF19)  // Fullwidth
           );
}

/**
 * Tests whether the given string is a legal NUMERIC string when set
 * programmatically.
 *
 * According to the Java API spec, a string cannot be parsed into an int if:
 * Any character of the string is not a digit of the specified radix,
 * except that the first character may be a minus sign '-' ('\u002D')
 * provided that the string is longer than length 1.
 */
TBool CMIDEdwinUtils::IsJavaInteger(const TDesC& aText)
{
    if (!IsNumeric(aText))
    {
        return EFalse;
    }
    // additional test #1: "-" is illegal
    if (aText.Length() == 1)
    {
        if ((aText.Locate(TChar('-')) == 0))
        {
            return EFalse;
        }
    }
    return ETrue;
}

/**
 * Tests whether a typed text is a valid one.
 */
TBool CMIDEdwinUtils::IsTextValid(const TDesC& aText)
{
    TInt pos = aText.Length();
    while (pos-- > 0)
    {
        TChar ch((aText)[ pos ]);

        // IsValidChar function will check chars over 255
        if (!IsValidChar(ch.GetNumericValue()) || ch == 0x7f)
        {
            return EFalse;
        }
    }

    return ETrue;
}

/**
 * Tests whether a char is a valid. Tests only characters over 255
 */
TBool CMIDEdwinUtils::IsValidChar(TInt aChar) const
{
    TBool ret(ETrue);

    // Tests only characters over 255
    if (aChar >= 0xFF)
    {
        // 0x20AC - Unicode for euro sign
        if (aChar == 0x20AC || aChar == CEditableText::EParagraphDelimiter ||
                aChar == CEditableText::ELineBreak)
        {
            ret = ETrue;
        }
        else
        {
            ret = EFalse;
        }
    }

    return ret;
}

/**
 * Tests whether a typed string is a legal DECIMAL string. This test is more
 * loose than that in IsJavaFloatingPointLiteral(), as ".", "-" and "-." are
 * legal text values during typing but not when set programmatically.
 */
TBool CMIDEdwinUtils::IsDecimal(const TDesC& aText, const TChar aDecimalSeparator)
{
    TInt length = aText.Length();
    TInt pos = length;
    for (; pos>0;)
    {
        TChar ch((aText)[--pos]);
        //
        if (ch.IsDigit())
        {
            continue;
        }
        else if (ch == '-')
        {
            if (pos != 0)
                return EFalse;
        }
        else if (ch == aDecimalSeparator)
        {
            if (aText.Locate(aDecimalSeparator) != aText.LocateReverse(aDecimalSeparator))
                return EFalse;
        }
        else
        {
            return EFalse;
        }
    }
    return ETrue;
}

/**
 * Tests whether the given string is a legal DECIMAL string when set
 * programmatically.
 */
TBool CMIDEdwinUtils::IsJavaFloatingPointLiteral(const TDesC& aText)
{
    if (!IsDecimal(aText, '.'))
    {
        return EFalse;
    }
    // additional test #1: "." and "-" are illegal
    if (aText.Length() == 1)
    {
        if ((aText.Locate(TChar('.')) == 0) || (aText.Locate(TChar('-')) == 0))
        {
            return EFalse;
        }
    }
    // additional test #2: "-." is illegal
    if (aText.Length() == 2)
    {
        if ((aText.Locate(TChar('-')) == 0) && (aText.Locate(TChar('.')) == 1))
        {
            return EFalse;
        }
    }
    return ETrue;
}

void CMIDEdwinUtils::RemoveNonPhoneNumberChars(HBufC* buf)
{
    TPtr text = buf->Des();
    for (TInt pos=0; pos < text.Length();)
    {
        TChar ch((text)[pos]);
        // allow also digits in non-western numbers
        if (IsNumeric(ch))
        {
            ++pos;
            continue;
        }

        switch (ch)
        {
        case '+':
        case '*':
        case '#':
        case 'p':
        case 'w':
            ++pos;
            continue;
        }
        text.Delete(pos, 1);
    }
}

/** Converts from java format text to localized (visible) format.
@see ConvertToLocalizedDecimalLC() */
HBufC* CMIDEdwinUtils::ConvertToLocalizedLC(const TDesC& aText, TUint aConstraints)
{
    HBufC* text = NULL;
    if ((aConstraints & MMIDTextField::EConstraintMask) == MMIDTextField::EDecimal)
    {
        text = ConvertToLocalizedDecimalLC(aText);
    }
    else
    {
        text = aText.AllocLC();
        TBool supportLineBreaks(ETrue);

        // Convert breaks if needed if constraints that do not allow line
        // breaks.
        if (aConstraints & MMIDTextField::EUrl ||
                aConstraints & MMIDTextField::EMailAddr)
        {
            // Line breaks are not supported
            supportLineBreaks = EFalse;
        }

        CMIDUtils::MapJavaToETextChars(text, supportLineBreaks);
    }
    return text;
}

/** Converts decimal point, if any, to a localized decimal separator. */
HBufC* CMIDEdwinUtils::ConvertToLocalizedDecimalLC(const TDesC& aText)
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

/** Converts localized decimal separator, if any, to a decimal point. */
void CMIDEdwinUtils::ConvertFromLocalizedDecimal(HBufC* buf)
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


TLanguage CMIDEdwinUtils::SelectLanguage(TInt aCharSubset,
        TLanguage aDefaultLanguage)
{
    TLanguage language = CAknEnv::Static()->SettingCache().InputLanguage();
    if (IsLanguageInCharSubset(language, aCharSubset))
    {
        return language;        // global input language
    }
    language = User::Language();
    if (IsLanguageInCharSubset(language, aCharSubset))
    {
        return language;        // display language
    }
    return aDefaultLanguage;    // given default
}

/**
 * Returns ETrue if the given language belongs to the given MIDP character subset.
 */
TBool CMIDEdwinUtils::IsLanguageInCharSubset(TLanguage aLanguage, TInt aCharSubset)
{
    switch (aLanguage)
    {
        // Cyrillic languages:
    case ELangBulgarian:
    case ELangRussian:
    case ELangSerbian:
    case ELangUkrainian:
        return (aCharSubset == EMidpUcbCyrillic);

        // Arabic languages:
    case ELangArabic:
    case ELangFarsi:
    case ELangUrdu:
        return (aCharSubset == EMidpUcbArabic);

        // Traditional Chinese languages, support latin, too:
    case ELangTaiwanChinese:
    case ELangHongKongChinese:
        return (aCharSubset == EMidpIsTraditionalHanzi ||
                aCharSubset == EMidpIsLatin);
        // Devanagari script languages
    case ELangHindi:
    case ELangMarathi:
        return (aCharSubset == EMidpUcbDevanagari);

        // Single languages that belong to other character subsets than latin
    case ELangHebrew:
    case ELangGreek:
    case ELangVietnamese:  // not yet in 3.0
    case ELangThai:
        return (aCharSubset == EMidpCharSubsetNone);

        // All the rest are latin languages; Japanese and Simplified Chinese
        // included here,  because they support latin, too
    default:
        return (aCharSubset == EMidpIsLatin);
    }
}
/**
 * Returns ETrue if number conversion is needed.
 */
TBool CMIDEdwinUtils::IsNumberConversionNeeded(TInt aConstraints)
{
    TUint constraint = aConstraints & MMIDTextField::EConstraintMask;
    if (constraint == 0)
        return AknTextUtils::DigitModeQuery(AknTextUtils::EDigitModeEditorDefault);
    else if ((constraint == MMIDTextField::ENumeric
              || constraint == MMIDTextField::EDecimal
              || constraint == MMIDTextField::EPhoneNumber
             ) && AknTextUtils::DigitModeQuery(AknTextUtils::EDigitModeNumberEditor)
            )
        return ETrue;
    else if ((constraint == MMIDTextField::EMailAddr
              || constraint == MMIDTextField::EUrl
             ) && AknTextUtils::DigitModeQuery(AknTextUtils::EDigitModeNumberEditor)
            )
        return ETrue;

    return EFalse;
}

void CMIDEdwinUtils::CreatePhoneCallL(TInt aTextLength, RWsSession aWsSession, CEikEdwin* aEdwin)
{
    if (aTextLength == 0 || aTextLength > AIWDialDataExt::KMaximumPhoneNumberLength)
    {
        return;  // silently ignore empty or too long number
    }

    /*
     * Get the current window group. Needed to activate the current app window
     * @note Temporary. This might not necessarily be the front-most window group,
     *  as window groups can disable keyboard focus.
     */
    RWsSession  wsSession = aWsSession;
    TInt currWindowGroup = wsSession.GetFocusWindowGroup();

    // ask for confirmation (as required by MIDP)
    CCallConfirmationDialog* dlg = CCallConfirmationDialog::NewL(aEdwin->GetTextInHBufL()->Des());

    if (!dlg->ExecuteLD(R_MIDP_CONFIRMATION_DIALOG))
        return;

    // Create service handler instance
    CAiwServiceHandler* aiwServiceHandler = CAiwServiceHandler::NewLC();  // cleanup stack 1

    // Create AIW interest
    RCriteriaArray interest;
    CleanupClosePushL(interest);        // cleanup stack 2
    CAiwCriteriaItem* criteria = CAiwCriteriaItem::NewLC(
                                     KAiwCmdCall, KAiwCmdCall,
                                     _L8("*"));     // cleanup stack 3
    TUid base;
    base.iUid = KAiwClassBase;
    criteria->SetServiceClass(base);
    User::LeaveIfError(interest.Append(criteria));

    // attach to AIW interest
    aiwServiceHandler->AttachL(interest);

    // create dial data
    CAiwDialData* dialData = CAiwDialData::NewLC();  // cleanup stack 4

    // define call parameters
    dialData->SetCallType(CAiwDialData::EAIWVoice);     // voice call

    // Use GetTextL to convert allow conversion of non-western numbers
    HBufC* phoneNumber = iTextBox->GetTextL();  // get the phone number
    CleanupStack::PushL(phoneNumber);       // cleanup stack 5 in case something leaves
    dialData->SetPhoneNumberL(*phoneNumber);

    // the window group where it switches to after the phone call
    dialData->SetWindowGroup(currWindowGroup);

    CAiwGenericParamList& paramList = aiwServiceHandler->InParamListL();
    dialData->FillInParamListL(paramList);

    // Execute AIW command with notification callback
    aiwServiceHandler->ExecuteServiceCmdL(KAiwCmdCall, paramList,
                                          aiwServiceHandler->OutParamListL(),
                                          0, NULL);

    // detach from AIW interest
    aiwServiceHandler->DetachL(interest);

    // aiwServiceHandler, interest, criteria, dialData, phoneNumber
    CleanupStack::PopAndDestroy(5);

    aiwServiceHandler = NULL;
    phoneNumber = NULL;
}

void CMIDEdwinUtils::SetInitialInputModeL(const TDesC& aCharacterSubset, TUint aConstraints, TInt& aInitialCurrentCase,
        TInt& aInitialCurrentInputMode, TLanguage& aInitialCurrentLanguage)
{
    TInt initialCurrentCase = 0;
    TInt initialCurrentInputMode = 0;
    TLanguage initialCurrentLanguage = ELangTest;  // 0

    TUint constraint = aConstraints & MMIDTextField::EConstraintMask;
    if (constraint == MMIDTextField::EAny ||
            constraint == MMIDTextField::EMailAddr ||
            constraint == MMIDTextField::EUrl)
    {
        if (aCharacterSubset.Length() == 0)
        {
            // null: default input mode
            initialCurrentLanguage = CAknEnv::Static()->SettingCache().InputLanguage();
            initialCurrentCase = EAknEditorTextCase;
        }
        else if (aCharacterSubset.Compare(KMidpUppercaseLatin) == 0)
        {
            // MIDP_UPPERCASE_LATIN
            // ignored if INITIAL_CAPS_SENTENCE or INITIAL_CAPS_WORD modifier in ANY
            if (!(aConstraints & MMIDTextField::EInitialCapsWordSentence ||
                    aConstraints & MMIDTextField::EInitialCapsWord) ||
                    constraint != MMIDTextField::EAny)
            {
                initialCurrentLanguage = CMIDEdwinUtils::SelectLanguage(EMidpIsLatin, ELangEnglish);
                initialCurrentInputMode = EAknEditorTextInputMode;
                initialCurrentCase = EAknEditorUpperCase;
            }
        }
        else if (aCharacterSubset.Compare(KMidpLowercaseLatin) == 0)
        {
            // MIDP_LOWERCASE_LATIN
            // ignored if INITIAL_CAPS_SENTENCE or INITIAL_CAPS_WORD modifier in ANY
            if (!(aConstraints & MMIDTextField::EInitialCapsWordSentence ||
                    aConstraints & MMIDTextField::EInitialCapsWord) ||
                    constraint != MMIDTextField::EAny)
            {
                initialCurrentLanguage = CMIDEdwinUtils::SelectLanguage(EMidpIsLatin, ELangEnglish);
                initialCurrentInputMode = EAknEditorTextInputMode;
                initialCurrentCase = EAknEditorLowerCase;
            }
        }
        else if (aCharacterSubset.Compare(KUcbBasicLatin) == 0 ||
                 aCharacterSubset.Compare(KIsLatin) == 0)
        {
            // UCB_BASIC_LATIN, IS_LATIN
            initialCurrentLanguage = CMIDEdwinUtils::SelectLanguage(EMidpIsLatin, ELangEnglish);
            initialCurrentInputMode = EAknEditorTextInputMode;
            initialCurrentCase = EAknEditorTextCase;
        }
        else if (aCharacterSubset.Compare(KIsLatinDigits) == 0)
        {
            // IS_LATIN_DIGITS
            initialCurrentInputMode = EAknEditorNumericInputMode;
        }
        else if (aCharacterSubset.Compare(KUcbArabic) == 0)
        {
            // UCB_ARABIC
            initialCurrentLanguage = ELangArabic;
        }
        else if (aCharacterSubset.Compare(KUcbHebrew) == 0)
        {
            // UCB_HEBREW
            initialCurrentLanguage = ELangHebrew;
        }
        else if (aCharacterSubset.Compare(KUcbGreek) == 0)
        {
            // UCB_GREEK
            initialCurrentLanguage = ELangGreek;
            initialCurrentCase = EAknEditorTextCase;
        }
        else if (aCharacterSubset.Compare(KUcbCyrillic) == 0)
        {
            // UCB_CYRILLIC
            initialCurrentLanguage = CMIDEdwinUtils::SelectLanguage(EMidpUcbCyrillic, ELangRussian);
            initialCurrentCase = EAknEditorTextCase;
        }
        else if (aCharacterSubset.Compare(KUcbDevanagari) == 0)
        {
            // UCB_DEVANAGARI
            initialCurrentLanguage = CMIDEdwinUtils::SelectLanguage(EMidpUcbDevanagari, ELangHindi);
        }
        else if (aCharacterSubset.Compare(KUcbThai) == 0)
        {
            // UCB_THAI
            if (FeatureManager::FeatureSupported(KFeatureIdThai))
            {
                initialCurrentLanguage = ELangThai;
            }
        }
        else if (aCharacterSubset.Compare(KIsSimplifiedHanzi) == 0)
        {
            // IS_SIMPLIIFED_HANZI
            if (FeatureManager::FeatureSupported(KFeatureIdChinese))
            {
                initialCurrentLanguage = ELangPrcChinese;
            }
        }
        else if (aCharacterSubset.Compare(KIsTraditionalHanzi) == 0)
        {
            // IS_TRADITIONAL_HANZI
            if (FeatureManager::FeatureSupported(KFeatureIdChinese))
            {
                initialCurrentLanguage = CMIDEdwinUtils::SelectLanguage(EMidpIsTraditionalHanzi,
                                         ELangTest);  // No suitable default for initial input language
            }
        }
        else if (aCharacterSubset.Compare(KUcbKatakana) == 0)
        {
            // UCB_KATAKANA
            if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
            {
                initialCurrentLanguage = ELangJapanese;
                initialCurrentInputMode = EAknEditorFullWidthKatakanaInputMode;
            }
        }
        else if (aCharacterSubset.Compare(KUcbHiragana) == 0 ||
                 aCharacterSubset.Compare(KIsKanji) == 0)
        {
            // UCB_HIRAGANA, IS_KANJI
            if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
            {
                initialCurrentLanguage = ELangJapanese;
                initialCurrentInputMode = EAknEditorHiraganaKanjiInputMode;
            }
        }
        else if (aCharacterSubset.Compare(KIsHalfwidthKatakana) == 0)
        {
            // IS_HALFWIDTH_KATAKANA
            if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
            {
                initialCurrentLanguage = ELangJapanese;
                initialCurrentInputMode = EAknEditorKatakanaInputMode;
            }
        }
        else if (aCharacterSubset.Compare(KIsFullwidthDigits) == 0)
        {
            // IS_FULLWIDTH_DIGITS
            if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
            {
                initialCurrentLanguage = ELangJapanese;
                initialCurrentInputMode = EAknEditorFullWidthNumericInputMode;
            }
        }
        else if (aCharacterSubset.Compare(KIsFullwidthLatin) == 0)
        {
            // IS_FULLWIDTH_LATIN
            if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
            {
                initialCurrentLanguage = ELangJapanese;
                initialCurrentInputMode = EAknEditorFullWidthTextInputMode;
            }
        }
        else if (aCharacterSubset.Compare(KXNokiaVietnamese) == 0)
        {
            // X_NOKIA_VIETNAMESE
            initialCurrentLanguage = ELangVietnamese;
            initialCurrentInputMode = EAknEditorTextInputMode;
            initialCurrentCase = EAknEditorTextCase;
        }
        else if (aCharacterSubset.Compare(KXNokiaUrdu) == 0)
        {
            // X_NOKIA_URDU
            initialCurrentLanguage = ELangUrdu;
            initialCurrentInputMode = EAknEditorTextInputMode;
        }
        else if (aCharacterSubset.Compare(KXNokiaFarsi) == 0)
        {
            // X_NOKIA_FARSI
            initialCurrentLanguage = ELangFarsi;
            initialCurrentInputMode = EAknEditorTextInputMode;
        }

        aInitialCurrentCase = initialCurrentCase;
        aInitialCurrentInputMode = initialCurrentInputMode;
        aInitialCurrentLanguage = initialCurrentLanguage;
    }
}

void CMIDEdwinUtils::SetFEPModeAndCharFormat(TUint aConstraints, CEikEdwin* aEdwin)
{
    // by default, allow all case modes
    aEdwin->SetAknEditorPermittedCaseModes(EAknEditorAllCaseModes);
    aEdwin->SetAknEditorCase(EAknEditorTextCase);
    if (aEdwin->TextView())
    {
        //This sets secret editors as one liners.
        //This is disabled because secret editors convert paragraph delimiters
        //in spaces and because the line wrapping works differently depending
        //on wheather the temporary char is displayed or the '*' is displayed.
        TRAP_IGNORE(aEdwin->SetWordWrapL(aConstraints & MMIDTextField::EPassword ? EFalse : ETrue));
    }

#ifdef RD_JAVA_S60_RELEASE_9_2
    aEdwin->SetAknEditorFlags(aEdwin->AknEdwinFlags() &
                              EAknEditorFlagEnablePartialScreen);
#else
    aEdwin->SetAknEditorFlags(EAknEditorFlagDefault);
#endif // RD_JAVA_S60_RELEASE_9_2
    aEdwin->SetAknEditorSpecialCharacterTable(-1);
    // handle all constraints and the PASSWORD modifier
    TUint constraint = aConstraints & MMIDTextField::EConstraintMask;
    if ((constraint == MMIDTextField::EAny) && (aConstraints & MMIDTextField::EPassword))
    {
        constraint = MMIDTextField::EPassword;
    }

    switch (constraint)
    {
    case MMIDTextField::EAny:
        aEdwin->SetAknEditorAllowedInputModes(EAknEditorTextInputMode | EAknEditorNumericInputMode);
        aEdwin->SetAknEditorInputMode(EAknEditorTextInputMode);
        // Calling AknEditorCurrentInputMode during construction caused problems in Chinese

        // handle other modifiers
        if ((aConstraints & MMIDTextField::ENonPredictive) || (aConstraints & MMIDTextField::ESensitive))
        {
            aEdwin->SetAknEditorFlags(aEdwin->AknEdwinFlags() |
                                      EAknEditorFlagNoT9 |
                                      EAknEditorFlagDeliverVirtualKeyEventsToApplication);
        }
        else
        {
            aEdwin->SetAknEditorFlags(aEdwin->AknEdwinFlags() |
                                      EAknEditorFlagDeliverVirtualKeyEventsToApplication);
        }
        if ((aConstraints & MMIDTextField::EInitialCapsWordSentence) || (aConstraints & MMIDTextField::EInitialCapsWord))
        {
            aEdwin->SetAknEditorCurrentCase(EAknEditorTextCase);
        }
        aEdwin->SetAknEditorSpecialCharacterTable(R_MIDP_TEXTBOX_SPECIAL_CHARACTER_TABLE_DIALOG);
        break;
    case MMIDTextField::EMailAddr:
    case MMIDTextField::EUrl:
    case MMIDTextField::EPassword:
        // set auto capitalization off, permit only UpperCase and LowerCase modes
        aEdwin->SetAknEditorPermittedCaseModes(EAknEditorUpperCase | EAknEditorLowerCase);

        aEdwin->SetAknEditorAllowedInputModes(EAknEditorTextInputMode | EAknEditorNumericInputMode);
        aEdwin->SetAknEditorInputMode(EAknEditorTextInputMode);
        aEdwin->SetAknEditorCurrentInputMode(EAknEditorTextInputMode);
        aEdwin->SetAknEditorCase(EAknEditorLowerCase);
        aEdwin->SetAknEditorCurrentCase(EAknEditorLowerCase);
        aEdwin->SetAknEditorFlags(aEdwin->AknEdwinFlags() |
                                  EAknEditorFlagNoT9 |
                                  EAknEditorFlagDeliverVirtualKeyEventsToApplication);
        aEdwin->SetAknEditorSpecialCharacterTable(R_MIDP_TEXTBOX_SPECIAL_CHARACTER_TABLE_DIALOG);
        break;
    case MMIDTextField::ENumeric:
    {
        aEdwin->SetAknEditorCase(EAknSCTNumeric);
        aEdwin->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorInputMode(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorCurrentInputMode(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorNumericKeymap(EAknEditorPlainNumberModeKeymap);
        //Disable SCT
        CAknExtendedInputCapabilities* extendedInputCapabilities = NULL;
        aEdwin->MopGetObject(extendedInputCapabilities);
        if (extendedInputCapabilities)
        {
            extendedInputCapabilities->SetCapabilities(
                extendedInputCapabilities->Capabilities() |
                CAknExtendedInputCapabilities::EDisableSCT);
        }
        aEdwin->SetAknEditorFlags(aEdwin->AknEdwinFlags() |
                                  EAknEditorFlagFixedCase | EAknEditorFlagNoT9 |
                                  EAknEditorFlagUseSCTNumericCharmap |
                                  EAknEditorFlagDeliverVirtualKeyEventsToApplication);
        aEdwin->SetAknEditorSpecialCharacterTable(R_MIDP_TEXTBOX_NUMERIC_MODE_DIALOG);
    }
    break;
    case MMIDTextField::EDecimal:
        aEdwin->SetAknEditorCase(EAknSCTNumeric);
        aEdwin->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorInputMode(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorCurrentInputMode(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorFlags(aEdwin->AknEdwinFlags() |
                                  EAknEditorFlagFixedCase | EAknEditorFlagNoT9 |
                                  EAknEditorFlagUseSCTNumericCharmap |
                                  EAknEditorFlagDeliverVirtualKeyEventsToApplication);
        aEdwin->SetAknEditorNumericKeymap(EAknEditorPlainNumberModeKeymap);
        if (iDecimalSeparator == ',')
        {
            aEdwin->SetAknEditorSpecialCharacterTable(R_MIDP_TEXTBOX_DECIMAL_MODE_WITH_COMMA_DIALOG);
        }
        else
        {
            aEdwin->SetAknEditorSpecialCharacterTable(R_MIDP_TEXTBOX_DECIMAL_MODE_DIALOG);
        }
        break;
    case MMIDTextField::EPhoneNumber:
        aEdwin->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorInputMode(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorCurrentInputMode(EAknEditorNumericInputMode);
        aEdwin->SetAknEditorFlags(aEdwin->AknEdwinFlags() |
                                  EAknEditorFlagFixedCase | EAknEditorFlagNoT9 |
                                  EAknEditorFlagDeliverVirtualKeyEventsToApplication);
        aEdwin->SetAknEditorNumericKeymap(EAknEditorStandardNumberModeKeymap);
        break;
    }

    // make changes take effect
    TRAP_IGNORE(aEdwin->NotifyEditorStateObserverOfStateChangeL());

}

// ---------------------------------------------------------------------------
// CMIDEdwinUtils::CropToSingleLine
// ---------------------------------------------------------------------------
//
void CMIDEdwinUtils::CropToSingleLine(TDes& aText)
{
    DEBUG_STR("CMIDEdwinUtils::CropToSingleLine +, aText=%S", aText);

    TInt index = aText.Length();
    while (index-- > 0)
    {
        TChar ch((aText)[ index ]);
        TChar::TCategory category = ch.GetCategory();

        // CR or LF character
        if (ch == 0x0D || ch == 0x0A ||
                category == TChar::EZlCategory || category == TChar::EZpCategory)
        {
            aText.Delete(index, 1);
        }
    }

    DEBUG_STR("CMIDEdwinUtils::CropToSingleLine -, aText=%S", aText);
}

// ---------------------------------------------------------------------------
// CMIDEdwinUtils::ConvertToUnicodeBreaks
// ---------------------------------------------------------------------------
//
void CMIDEdwinUtils::ConvertToUnicodeBreaks(TDes& aText)
{
    DEBUG_STR("CMIDEdwinUtils::ConvertToUnicodeBreaks +, aText=%S", aText);

    const TInt length = aText.Length();
    TText* ptr = const_cast < TText* >(aText.Ptr());
    TInt index  = 0;

    while (index < length)
    {
        if (ptr[ index ] == 0x0D || ptr[ index ] == 0x0A)
        {
            break;
        }

        index++;
    }

    TText prevChar    = 0;
    TInt  finalLength = index;

    while (index < length)
    {
        TText ch = ptr[ index ];

        if (ch != 0x0D && ch != 0x0A)
        {
            ptr[ finalLength++ ] = ch;
        }
        else
        {
            // Do not modify valid line breaks (CRLF)
            if (!(ch == 0x0A && prevChar == 0x0D))
            {
                ptr[ finalLength++ ] = CEditableText::EParagraphDelimiter;
            }
        }

        prevChar = ch;
        index++;
    }

    aText.SetLength(finalLength);

    DEBUG_STR("CMIDEdwinUtils::ConvertToUnicodeBreaks -, aText=%S", aText);
}

/***************************************************************************
*
* Class CPasswordBoxGlobalText
*
****************************************************************************/

CPasswordBoxGlobalText::CPasswordBoxGlobalText(const CParaFormatLayer* aGlobalParaLayer,
        const CCharFormatLayer* aGlobalCharLayer,
        CEikEdwin& aTextComponent)
        : CGlobalText(aGlobalParaLayer,aGlobalCharLayer), iTextComponent(aTextComponent), iClearCharPos(-1)
{}

void CPasswordBoxGlobalText::ConstructL()
{
    CGlobalText::ConstructL(CEditableText::ESegmentedStorage,CEditableText::EDefaultTextGranularity);

    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iClearText = HBufC::NewL(CEditableText::EDefaultTextGranularity);
}

CPasswordBoxGlobalText::~CPasswordBoxGlobalText()
{
    delete iClearText;
    delete iTimer;
}

/** */
void CPasswordBoxGlobalText::Reset()
{
    if (iTimer->IsActive())
    { //commit any previous character before deleting or else we
        // may crash in the timer callback when trying to replace an uncommited
        // but already deleted char
        TRAP_IGNORE(DoTimerCallbackL(EFalse));
    }
    // clear local (hidden) copy
    TPtr ptr = iClearText->Des();
    ptr.SetLength(0);

    CGlobalText::Reset();
}

/**
 * Support for pasting in the TextBox with PASSWORD constraint. The normal insertion
 * mechanism is not used in the paste operation, so it must be handled separately
 * here. This is an overriden method that stores the pasted chars to the local
 * store and puts masked chars (*) to the visible store (in super class).
 */
TInt CPasswordBoxGlobalText::PasteFromStoreL(const CStreamStore& aStore,const CStreamDictionary& aDictionary,TInt aPos)
{
    // 1. Paste clipboard text (to visible text)
    TInt numCharsPasted = CGlobalText::PasteFromStoreL(aStore, aDictionary, aPos);
    // 2. Insert the pasted text to the local clear text store
    TPtrC ptr = Read(aPos, numCharsPasted);
    SaveClearTextL(aPos, ptr);
    // 3. Mask the visible text
    for (int i = 0; i < numCharsPasted; i++)
    {
        TPtrC charToProcess = Read(aPos+i, 1);
        TChar replaceChar   = KPasswordEchoChar;
        if (charToProcess[0] == CEditableText::EParagraphDelimiter)
        {
            replaceChar = ' ';
        }
        CGlobalText::DeleteL(aPos+i,1);
        CGlobalText::InsertL(aPos+i, replaceChar);
    }
    return numCharsPasted;
}

/** Insert a single character. First of all save the character in the clear text by
calling SaveClearTextL(). Then if the character is a paragraph delimiter insert a space.
Otherwise if we are in alpha mode insert the clear character and start the timer. Otherwise
insert the asterisks. */
void CPasswordBoxGlobalText::InsertL(TInt aPos, const TChar& aChar)
{
    TBuf<1> buf;
    buf.Append(aChar);
    SaveClearTextL(aPos, buf);

    if (aChar == CEditableText::EParagraphDelimiter)
    {
        CGlobalText::InsertL(aPos, ' ');
    }
    else if (iTextComponent.AknEditorCurrentInputMode() & KInputModesRequiringFeedbackWithSingleChar)
    {
        if (iTimer->IsActive() && iClearCharPos != aPos)
        { //commit any previous character in a different position
            DoTimerCallbackL(ETrue);
        }
        else
        { //or restart the timer if the char is in the same position (multi-tap looping)
            iTimer->Cancel();
        }

        CGlobalText::InsertL(aPos, aChar);
        if (!iRejectNextKey) //For example the Enter key will be rejected
        {
            iClearCharPos = aPos;
            iTimer->Start(KPasswordCharTimeout, KPasswordCharTimeout, TCallBack(TimerCallback, this));
        }
        else
        {
            iRejectNextKey = EFalse;
        }
    }
    else
    {
        CGlobalText::InsertL(aPos, KPasswordEchoChar);
    }
}

/** Insert one or more characters. If inserting only one character rely
on InsertL() accepting only one char. Otherwise, save the clear text by
calling SaveClearTextL(). Then for every character either insert an asterisk
or a space if the character is a space delimiter. Note: this method is
called at start-up so it makes no sense to show a temporary clear character. */
void CPasswordBoxGlobalText::InsertL(TInt aPos,const TDesC& aBuf)
{
    if (aBuf.Length() == 1)
    {
        CPasswordBoxGlobalText::InsertL(aPos, aBuf[0]);
    }
    else if (aBuf.Length() > 1 && (iTextComponent.AknEditorCurrentInputMode() & KInputModesRequiringFeedbackWithString))
    {
        // When writing in kanji or hiragana input mode, the characters are entered in
        // sequences. Insert the sequence char by char so that the user will see the last
        // character temporarily for feedback.
        for (TInt i = 0; i < aBuf.Length(); i++)
        {
            InsertL(aPos + i, aBuf[i]);
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
                CGlobalText::InsertL(aPos + i, KPasswordEchoChar);
            }
        }
    }
}

/** Delete one or more chars, this makes sure our clear text stays
in sync. */
TBool CPasswordBoxGlobalText::DeleteL(TInt aPos,TInt aLength)
{
    if (iTimer->IsActive())
    { //commit any previous character before deleting or else we
        // may crash in the timer callback when trying to replace an uncommited
        // but already deleted char
        DoTimerCallbackL(EFalse);
    }

    TPtr ptr = iClearText->Des();
    ptr.Delete(aPos, aLength);

    return CGlobalText::DeleteL(aPos, aLength);
}

/** This method is called by edwin when returning text to the user via
a GetText() call. We make sure we return the clear text instead of the
asterisks used by edwin. */
void CPasswordBoxGlobalText::Extract(TDes& aBuf,TInt aPos) const
{
    aBuf.SetLength(0);
    aBuf.Append(iClearText->Mid(aPos));
}

/** This method is called by edwin when returning text to the user via
a GetText() call. We make sure we return the clear text instead of the
asterisks used by edwin. */
void CPasswordBoxGlobalText::Extract(TDes& aBuf,TInt aPos,TInt aLength) const
{
    aBuf.SetLength(0);
    aBuf.Append(iClearText->Mid(aPos, aLength));
}

/** We save our clear text. We reallocate if not enough memory. */
void CPasswordBoxGlobalText::SaveClearTextL(TInt aPos,const TDesC& aBuf)
{
    ASSERT(iClearText);
    TPtr ptr = iClearText->Des();

    TInt totalLength = ptr.Length() + aBuf.Length();
    if (totalLength > ptr.MaxLength())
    {    
		iClearText = iClearText->ReAllocL(totalLength);
		ptr.Set(iClearText->Des());
    }

    ptr.Insert(aPos,aBuf);
}

/** The timer callback, see DoTimerCallbackL() */
TInt CPasswordBoxGlobalText::TimerCallback(TAny* aThis)
{
    CPasswordBoxGlobalText* self = STATIC_CAST(CPasswordBoxGlobalText*, aThis);
    ASSERT(self);

    TRAPD(err, self->DoTimerCallbackL(ETrue));
    return err;
}

/** It is time to remove the temporary clear character. Do a delete at iClearCharPos and
then insert an asterisk at this position. Then we need to notify the edwin to update its
characters, NotifyNewFormatL(). Note: this is only needed when inserting a char from the
special char table or when holding a key down so that a number is inserted even if in alpha
input mode. Otherwise you do not notice the differece as edwin update its text anyway when
commiting a fep transaction after a timeout that is just about longer than our timeout.
*/
void CPasswordBoxGlobalText::DoTimerCallbackL(TBool aNotifyFormat)
{
    iTimer->Cancel();

    ASSERT(iClearCharPos != -1 && (DocumentLength() > iClearCharPos)); //udeb builds only
    if (iClearCharPos != -1 && (DocumentLength() > iClearCharPos)) //prevent crash in urel builds
    {
        CGlobalText::DeleteL(iClearCharPos,1);
        CGlobalText::InsertL(iClearCharPos, KPasswordEchoChar);

        DEBUG_INT("CMIDEdwin::DoTimerCallbackL, skip NotifiyNewFormatL = %D", aNotifyFormat);
        // NotifyNewFormat can not be called after a char deletion or when the
        // text editor does not have a view assosiated with it. The latter happens
        // when text is set to the field before it is shown.
        //
        if (aNotifyFormat && iTextComponent.TextView())
        {
            iTextComponent.NotifyNewFormatL();
        }
        iTextComponent.DrawDeferred();
    }
}


/***************************************************************************
*
* Class CCallConfirmationDialog
*
****************************************************************************/

CCallConfirmationDialog* CCallConfirmationDialog::NewL(const TDesC& aNumber, const TTone& aTone)
{
    CCallConfirmationDialog* self = new(ELeave) CCallConfirmationDialog(aTone);
    CleanupStack::PushL(self);
    self->ConstructL(aNumber);
    CleanupStack::Pop(self);
    return self;
}

CCallConfirmationDialog::CCallConfirmationDialog(const TTone aTone)
        : CAknQueryDialog(aTone), iResources(*CCoeEnv::Static())
{
}

CCallConfirmationDialog::~CCallConfirmationDialog()
{
    iResources.Close();
}

TKeyResponse CCallConfirmationDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (aType == EEventKey && aKeyEvent.iCode == EKeyYes)
    {
        TryExitL(EKeyYes);
        return EKeyWasConsumed;
    }
    else
    {
        return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
    }
}

TBool CCallConfirmationDialog::OkToExitL(TInt aButtonId)
{
    if (EKeyYes == aButtonId)
    {
        return ETrue;
    }
    else
    {
        TBool retValue(CAknDialog::OkToExitL(aButtonId));
        return retValue;
    }
}

TInt CCallConfirmationDialog::MappedCommandId(TInt aButtonId)
{
    switch (aButtonId)
    {
    case EKeyYes:
        return EKeyYes;
    default :
        return CEikDialog::MappedCommandId(aButtonId);
    }
}

void CCallConfirmationDialog::ConstructL(const TDesC& aNumber)
{
    HBufC* bufNumber = aNumber.AllocLC();
    TPtr bufNumberPtr = bufNumber->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(bufNumberPtr);
    HBufC* prompt = StringLoader::LoadLC(R_MIDP_QUEST_CALL_NUMBER, bufNumberPtr);
    SetPromptL(*prompt);
    CleanupStack::PopAndDestroy(2);   // bufNumber, prompt
}


/**
 * class CMIDAiwPbk2Client
 */
CMIDAiwPbk2Client* CMIDAiwPbk2Client::NewL(MMIDTextBox& aEdwin)
{
    CMIDAiwPbk2Client* self = new(ELeave) CMIDAiwPbk2Client(aEdwin);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CMIDAiwPbk2Client::ConstructL()
{
    iServiceHandler = CAiwServiceHandler::NewL();

    //attach interest
    iServiceHandler->AttachL(R_PBK2_CONTACT_SELECTION_INTEREST);

    //create contact manager with same configuration like in current phonebook
    CPbk2StoreConfiguration* configuration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(configuration);

    CVPbkContactStoreUriArray* uriArray = configuration->CurrentConfigurationL();
    CleanupStack::PushL(uriArray);

    iContactManager = CVPbkContactManager::NewL(*uriArray);

    CleanupStack::PopAndDestroy(uriArray);

    CleanupStack::PopAndDestroy(configuration);
}


CMIDAiwPbk2Client::CMIDAiwPbk2Client(MMIDTextBox& aEdwin): iEdwin(aEdwin)
{
}

CMIDAiwPbk2Client::~CMIDAiwPbk2Client()
{
    delete iServiceHandler;
    delete iContactManager;
    delete iFetchedLinks;
}

/**
 * Entry point
 */
void CMIDAiwPbk2Client::FetchFromPhoneBookL(TInt aCmdId)
{
    iCmdId = aCmdId;
    //open contact stores
    iContactManager->ContactStoresL().OpenAllL(*this);
}


/**
 * Open contact stores callback methods
 */
void CMIDAiwPbk2Client::StoreReady(MVPbkContactStore&)
{
}
void CMIDAiwPbk2Client::StoreUnavailable(MVPbkContactStore& , TInt)
{
}
void CMIDAiwPbk2Client::HandleStoreEventL(MVPbkContactStore& , TVPbkContactStoreEvent)
{
}

/**
 * Stores are sucesfully open
 */
void CMIDAiwPbk2Client::OpenComplete()
{
    TRAPD(err, OpenCompleteL());

    if (err != KErrNone)
    {
        DEBUG_INT("CMIDAiwPbk2Client::OpenComplete - Exception. Error = %d", err);
    }
}
/**
 * Stores are sucesfully open
 */
void CMIDAiwPbk2Client::OpenCompleteL()
{

    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
    //single item fetch
    TRAPD(err,
          paramList.AppendL(
              TAiwGenericParam(
                  EGenericParamContactSelectionData,
                  TAiwVariant(TAiwSingleItemSelectionDataV1Pckg(TAiwSingleItemSelectionDataV1().
                              SetAddressSelectType((TAiwAddressSelectType)iCmdId).
                              SetFlags(0))))));

    if (err != KErrNone)
    {
        DEBUG_INT("CMIDAiwPbk2Client::OpenComplete - Exception from paramList.AppendL. Error = %d", err);
    }

    //execute service with handlenotify observer
    TRAP(err,
         iServiceHandler->ExecuteServiceCmdL(KAiwCmdSelect, paramList,
                                             iServiceHandler->OutParamListL(),
                                             0, this));

    if (err != KErrNone)
    {
        DEBUG_INT("CMIDAiwPbk2Client::OpenComplete - Exception from ExecuteServiceCmdL. Error = %d", err);
    }

}

/**
 * Retrieving of single contact suceeded
 */
void CMIDAiwPbk2Client::VPbkSingleContactOperationComplete(MVPbkContactOperationBase&, MVPbkStoreContact* aContact)
{

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    MVPbkStoreContactField* field = aContact->Fields().RetrieveField(*iCurrentLink);

    TPtrC text = MVPbkContactFieldTextData::Cast(field->FieldData()).Text();

    TRAPD(err, iEdwin.SetTextL(text));
    if (err != KErrNone)
    {
        DEBUG_INT("CMIDAiwPbk2Client::VPbkSingleContactOperationComplete - Exception from Edwin.SetTextL. Error = %d", err);
    }

    delete aContact;

}

/**
 * Retrieving of single contact failed
 */
void CMIDAiwPbk2Client::VPbkSingleContactOperationFailed(MVPbkContactOperationBase&, TInt)
{
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
}

/**
 * AiwNotify callback
 */
TInt CMIDAiwPbk2Client::HandleNotifyL(TInt aCmdId, TInt aEventId,
                                      CAiwGenericParamList& aEventParamList, const CAiwGenericParamList&)
{
    if (aCmdId == KAiwCmdSelect && aEventId == KAiwEventCompleted)
    {
        TInt index = 0 ;
        const TAiwGenericParam* param = aEventParamList.FindFirst(index, EGenericParamContactLinkArray);
        if (param)
        {
            TPtrC8 contactLinks = param->Value().AsData();

            iFetchedLinks = iContactManager->CreateLinksLC(contactLinks);
            CleanupStack::Pop();

            iCurrentLink = &iFetchedLinks->At(index);

            iRetrieveOperation = iContactManager->RetrieveContactL(*iCurrentLink, *this);

        }
    }
    // other status do not interests us, just continue till eventcompleted
    return 1;
}


// End of File
