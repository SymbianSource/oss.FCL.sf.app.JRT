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
* Description:  ?Description
*
*/


#ifndef CMIDEDWINUTILS_H
#define CMIDEDWINUTILS_H

#include <coecntrl.h>
#include <lcdui.h>
#include <eikedwin.h>
#include <coemain.h>

#include <AknQueryDialog.h> // CAknQueryDialog
#include <txtglobl.h>       // CGlobalText
#include <ConeResLoader.h>  // RConeResourceLoader

//phonebook2
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkSingleContactOperationObserver.h>

//aiw api for phonebook2
#include <AiwServiceHandler.h>

#include "CMIDUtils.h"

// FORWARD DECLARATIONS
class CMIDAiwPbk2Client;
class CVPbkContactManager;


/** The character displayed in password editors */
#define KPasswordEchoChar '*'
#define KInputModesRequiringFeedbackWithSingleChar (EAknEditorTextInputMode | EAknEditorKatakanaInputMode | EAknEditorFullWidthTextInputMode | EAknEditorFullWidthKatakanaInputMode | EAknEditorHiraganaKanjiInputMode | EAknEditorHiraganaInputMode | EAknEditorHalfWidthTextInputMode)
#define KInputModesRequiringFeedbackWithString (EAknEditorHiraganaInputMode | EAknEditorHiraganaKanjiInputMode)

/** Special keys definition */
const TInt KKeyQwerty0 = 0x30;
const TInt KKeyQwerty9 = 0x39;
const TInt KMidpKeyNkpAsteriskHW = 0x2A;

/** In multi-tap mode, show character before hiding it with KPasswordEchoChar
for this amount of time */
const TInt KPasswordCharTimeout = 700000; // 700 msecs
// used in numeric mode
_LIT(KMinusChar,             "-");
// used in decimal mode
_LIT(KFullStopChar,             ".");

// New line character
_LIT(KLineSeparator,  "\x2029");

// Null title character, because setting null title to query dialog does nothing.
_LIT(KNullTitle,  " ");

// character subsets for SetInitialInputModeL
_LIT(KUcbBasicLatin,        "UCB_BASIC_LATIN");
_LIT(KIsLatin,              "IS_LATIN");
_LIT(KIsLatinDigits,        "IS_LATIN_DIGITS");
_LIT(KMidpUppercaseLatin,   "MIDP_UPPERCASE_LATIN");
_LIT(KMidpLowercaseLatin,   "MIDP_LOWERCASE_LATIN");

_LIT(KUcbArabic,            "UCB_ARABIC");
_LIT(KUcbHebrew,            "UCB_HEBREW");
_LIT(KUcbGreek,             "UCB_GREEK");
_LIT(KUcbCyrillic,          "UCB_CYRILLIC");         // Russian etc.
_LIT(KUcbThai,              "UCB_THAI");
_LIT(KUcbDevanagari,        "UCB_DEVANAGARI");        // Hindi and Marathi
_LIT(KIsSimplifiedHanzi,    "IS_SIMPLIFIED_HANZI");  // Simplified Chinese
_LIT(KIsTraditionalHanzi,   "IS_TRADITIONAL_HANZI"); // Traditional Chinese (Taiwan, Hongkong)

_LIT(KUcbHiragana,           "UCB_HIRAGANA");
_LIT(KUcbKatakana,          "UCB_KATAKANA");
_LIT(KIsHalfwidthKatakana,     "IS_HALFWIDTH_KATAKANA");
_LIT(KIsKanji,                 "IS_KANJI");
_LIT(KIsFullwidthDigits,    "IS_FULLWIDTH_DIGITS");
_LIT(KIsFullwidthLatin,     "IS_FULLWIDTH_LATIN");

_LIT(KXNokiaVietnamese,     "X_NOKIA_VIETNAMESE");
_LIT(KXNokiaUrdu,           "X_NOKIA_URDU");
_LIT(KXNokiaFarsi,          "X_NOKIA_FARSI");

// Character subsets that contain more than one S60 language
enum
{
    EMidpCharSubsetNone= 0,
    EMidpIsLatin,                // "IS_LATIN"
    EMidpUcbCyrillic,            // "UCB_CYRILLIC"
    EMidpUcbArabic,                // "UCB_ARABIC"
    EMidpUcbDevanagari,            // "UCB_DEVANAGARI"
    EMidpIsTraditionalHanzi        // "IS_TRADITIONAL_HANZI"
};

// TextBox and TextField utils
//

NONSHARABLE_CLASS(CMIDEdwinUtils) : public CBase
{
public:

    /** IDs for non-midlet commands of the TextBox/Field
        It is important that these are negative and unique.
        Therefore if another control needs to add non-midlet cmds
        the ids should be different than these.
    */
    enum
    {
        EMenuCommandFetchPhoneNumber = -1,
        EMenuCommandFetchEmailAddress = -2,
        EMenuCommandCreatePhoneCall = -3
    };

    static CMIDEdwinUtils* NewL(MMIDTextBox* aTextBox, const TChar aDecimalSeparator);

private:
    CMIDEdwinUtils(MMIDTextBox* aTextBox, const TChar aDecimalSeparator);
    void ConstructL();

public:
    TBool ConstraintsValidForText(const TDesC& aText, TUint aConstraints, TBool aStrict);
    TBool ConstraintsValidForInsertedTextL(const TDesC& aText, const TDesC& aTextToInsert,
                                           TInt aPos, TUint aConstraints, TBool aStrict);
    void RemoveNonPhoneNumberChars(HBufC* buf);

    TBool IsHotKeyL(const TKeyEvent& aKeyEvent, CCoeEnv* aCoeEnv);
    TBool IsNavigationKey(const TKeyEvent& aKeyEvent);
    TBool IsPhoneNumber(const TDesC& aText);
    TBool IsNumeric(const TDesC& aText);
    TBool IsNumeric(const TChar& aChar);
    TBool IsJavaInteger(const TDesC& aText);
    TBool IsTextValid(const TDesC& aText);
    TBool IsValidChar(TInt aChar) const;
    TBool IsDecimal(const TDesC& aText, const TChar aDecimalSeparator);
    TBool IsJavaFloatingPointLiteral(const TDesC& aText);
    HBufC* ConvertToLocalizedLC(const TDesC& aText, TUint aConstraints);
    HBufC* ConvertToLocalizedDecimalLC(const TDesC& aText);
    void ConvertFromLocalizedDecimal(HBufC* buf);
    TLanguage SelectLanguage(TInt aCharSubset, TLanguage aDefaultLanguage);
    TBool IsLanguageInCharSubset(TLanguage aLanguage, TInt aCharSubset);

    /**
     * Determines if languge conversion for numbers is needed based
     * on constraints, language settings, display language setting
     * and general setting.
     * @returns ETrue to indicate whether digit conversions need to be used.
     * @returns EFalse to indicate that no conversion is needed.
     */
    TBool IsNumberConversionNeeded(TInt aConstraints);

    /**
     * Opens phone call dialog
     */
    void CreatePhoneCallL(TInt aTextLength, RWsSession aWsSession, CEikEdwin* aEdwin);

    void SetInitialInputModeL(const TDesC& aCharacterSubset, TUint iConstraints, TInt& aInitialCurrentCase,
                              TInt& aInitialCurrentInputMode, TLanguage& aInitialCurrentLanguage);

    void SetFEPModeAndCharFormat(TUint aConstraints, CEikEdwin* aEdwin);

    /**
     * Removes line and paragraph breaks from the text.
     *
     * @param aText The text to be checked.
         * @since S60 5.0
     */
    void CropToSingleLine(TDes& aText);

    /**
     * Converts the line breaks in the given text to
     * unicode breaks.
     *
     * @param aText The text to be converted.
         * @since S60 5.0
     */
    void ConvertToUnicodeBreaks(TDes& aText);

private:
    MMIDTextBox*    iTextBox;
    TChar           iDecimalSeparator;
};

/** class CPasswordBoxGlobalText
    The purpose of this class is to implement a secret editor by
    replacing the text with '*'. The clear text is stored in this
    class instead. It would have been nicer to user CEikSecretEditor
    but the design of CMIDEdwin (which is a CEikEdwin) does not
    allow this option.

    We override CGlobalText methods dealing with text insertion
    and deletion to make sure our clear text, stored in iClearText,
    is always in sync with what the user types. We insert '*' in the
    edwin by using the CGlobalText methods appropriately. We use a timer
    to show a temporary clear character when in alpha input mode.

*/
NONSHARABLE_CLASS(CPasswordBoxGlobalText) : public CGlobalText
{
public:
    CPasswordBoxGlobalText(const CParaFormatLayer* aGlobalParaLayer,
    const CCharFormatLayer* aGlobalCharLayer,CEikEdwin& aTextComponent);
    void ConstructL();
    ~CPasswordBoxGlobalText();

public: // Methods overwritten from CGlobalText
    void Reset();
    void InsertL(TInt aPos,const TChar& aChar);
    void InsertL(TInt aPos,const TDesC& aBuf);
    TBool DeleteL(TInt aPos,TInt aLength);
    TInt PasteFromStoreL(const CStreamStore& aStore,const CStreamDictionary& aDictionary,TInt aPos);

    void Extract(TDes& aBuf,TInt aPos=0) const;
    void Extract(TDes& aBuf,TInt aPos,TInt aLength) const;

public: //Public API to text box
    TPtrC ClearText() const
    {
        return *iClearText;
    }
    void RejectNextKey(TBool aReject)
    {
        iRejectNextKey = aReject;
    }

private: // Implementation helpers
    void SaveClearTextL(TInt aPos,const TDesC& aBuf);

    static TInt TimerCallback(TAny* aThis);
    void DoTimerCallbackL(TBool aNotifyFormat);


private:
    /** A reference to the text editor that owns us*/
    CEikEdwin& iTextComponent;

    /** The position of the temporary clear character */
    TInt iClearCharPos;

    /** The clear text */
    HBufC* iClearText;

    /** The timer used to show the temporary clear character */
    CPeriodic* iTimer;

    /** This flag is set to true when the editor will reject the next key, e.g. Enter key */
    TBool iRejectNextKey;
};


// class CCallConfirmationDialog

NONSHARABLE_CLASS(CCallConfirmationDialog) : public CAknQueryDialog
{
public:
    static CCallConfirmationDialog* NewL(const TDesC& aNumber,
    const TTone& aTone = ENoTone);
    ~CCallConfirmationDialog();
protected: // from CAknQueryDialog
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TBool OkToExitL(TInt aButtonId);
    TInt MappedCommandId(TInt aButtonId);
private:
    CCallConfirmationDialog(const TTone aTone);
    void ConstructL(const TDesC& aNumber);
private:
    RConeResourceLoader iResources;
};

/**
 * class CMIDAiwPbk2Client
 *  This class allows fetching of email/phonenumber into Edwin.
 * It's needed for textbox/field when constraints are set to EMAILADDR/PHONENUMBER
 */
NONSHARABLE_CLASS(CMIDAiwPbk2Client) : public CBase, public MVPbkContactStoreListObserver,
        public MAiwNotifyCallback, public MVPbkSingleContactOperationObserver
{
public:
    static CMIDAiwPbk2Client* NewL(MMIDTextBox& aEdwin);
    ~CMIDAiwPbk2Client();
    void FetchFromPhoneBookL(TInt aCmdId);

private:
    void ConstructL();
    CMIDAiwPbk2Client(MMIDTextBox& aEdwin);

    void OpenCompleteL();

    //from ContactStoreListObserver
    void StoreReady(MVPbkContactStore&);
    void StoreUnavailable(MVPbkContactStore& , TInt);
    void HandleStoreEventL(MVPbkContactStore&, TVPbkContactStoreEvent);
    void OpenComplete();

    //from AiwNotifyCallback
    TInt HandleNotifyL(TInt aCmdId, TInt aEventId,
                       CAiwGenericParamList& aEventParamList,
                       const CAiwGenericParamList&);

    //from SingleContactOperationObserver
    void VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact);
    void VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError);


    // current contact manager
    CVPbkContactManager* iContactManager;

    // contact link to fetched data
    const MVPbkContactLink* iCurrentLink;

    // service handler for invoking phonebook dialog
    CAiwServiceHandler* iServiceHandler;

    // array of fetched links
    MVPbkContactLinkArray* iFetchedLinks;

    // data to fetch, ie. phonenumber or email
    TInt iCmdId;

    // a reference to text editor
    MMIDTextBox& iEdwin;

    // SingleContactOperation handle
    MVPbkContactOperationBase* iRetrieveOperation;
};

#endif // CMIDEDWINUTILS_H
