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
#include <CPbkContactEngine.h>
#include <CPbkSingleEntryFetchDlg.h>
#include <CPbkContactItem.h>
#include <RPbkViewResourceFile.h>
#include <avkon.rsg>
#include <barsread.h>
#include <favouritesdb.h>
#include <favouritesitemlist.h>
#include <favouritessession.h>
#include <aknselectionlist.h>
#include <eikspane.h>
#include <akntitle.h>
#include <akncontext.h>
#include <AknsUtils.h>
#include <aknappui.h>
#include <eswtcore.rsg>
#include <featdiscovery.h>
#include <e32const.h>
#include <swtlaffacade.h>
#include "swtcontrolmenu.h"
#include "swtmenuitem.h"
#include "eswtmobile.hrh"
#include "swtuiutils.h"
#include "swttextextension.h"


//Pin Board UID for Favourites Icon
#define PINB_UID 0x1000590B

_LIT(KTAB,"\t");

const TInt KSWTTEXTEXTENSION_EMAIL_LENGTH = 150;
const TInt KSWTTEXTEXTENSION_URL_LENGTH = 1000;

// Character subsets for SetInitialInputModeL
_LIT(KUcbBasicLatin,        "UCB_BASIC_LATIN");
_LIT(KIsLatin,              "IS_LATIN");
_LIT(KIsLatinDigits,        "IS_LATIN_DIGITS");
_LIT(KUcbArabic,            "UCB_ARABIC");
_LIT(KUcbHebrew,            "UCB_HEBREW");
_LIT(KUcbGreek,             "UCB_GREEK");
_LIT(KUcbThai,              "UCB_THAI");
_LIT(KIsSimplifiedHanzi,    "IS_SIMPLIFIED_HANZI");
_LIT(KUcbHiragana,          "UCB_HIRAGANA");
_LIT(KUcbKatakana,          "UCB_KATAKANA");
_LIT(KIsHalfwidthKatakana,  "IS_HALFWIDTH_KATAKANA");
_LIT(KIsKanji,              "IS_KANJI");
_LIT(KIsFullwidthDigits,    "IS_FULLWIDTH_DIGITS");
_LIT(KIsFullwidthLatin,     "IS_FULLWIDTH_LATIN");
_LIT(KUcbCyrillic,          "UCB_CYRILLIC");
_LIT(KUcbArmenian,          "UCB_ARMENIAN");
_LIT(KUcbDevanagari,        "UCB_DEVANAGARI");
_LIT(KUcbBengali,           "UCB_BENGALI");
_LIT(KUcbHangulSyllables,   "UCB_HANGUL_SYLLABLES");
_LIT(KIsHanja,              "IS_HANJA");
_LIT(KIsTraditionalHanzi,   "IS_TRADITIONAL_HANZI");


// Character subsets that contain more than one S60 language
enum
{
    ECharSubsetNone= 0,
    EIsLatin,               // "IS_LATIN"
    EUcbCyrillic,           // "UCB_CYRILLIC"
    EUcbArabic,             // "UCB_ARABIC"
    EIsTraditionalHanzi     // "IS_TRADITIONAL_HANZI"
};


// ======== LOCAL FUNCTIONS ========


// ---------------------------------------------------------------------------
// Local function for sorting the Favourite items by Name
// ---------------------------------------------------------------------------
//
LOCAL_C TInt CompareItemsByNameL(const CFavouritesItem& aLeft, const CFavouritesItem& aRight)
{
    return aLeft.Name().CompareF(aRight.Name());
}


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Destructor for TextExtension
// ---------------------------------------------------------------------------
//
CSwtTextExtension::~CSwtTextExtension()
{
    delete iNaviDecorator;

    if (iMenuItem)
        iMenuItem->Dispose();

    if (iMenu)
        iMenu->Dispose();
}


// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::ConstructL()
{
    CSwtText::ConstructL();
}


// ---------------------------------------------------------------------------
// Constructs and returns an object.
// ---------------------------------------------------------------------------
//
CSwtTextExtension* CSwtTextExtension::NewL(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
{
    CSwtTextExtension* self = new(ELeave) CSwtTextExtension(aDisplay,
            aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}



// ---------------------------------------------------------------------------
// Sets content styles(Email address, URL), input modifier styles and
//  casing modifiers. Note that Non Midlet commands are added here
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::SetTypeStyleL(TInt aTypeStyle)
{
    iTypeStyle=aTypeStyle;

    TInt sct = R_AVKON_URL_SPECIAL_CHARACTER_TABLE_DIALOG;
    TUint flags = 0;
    if (iTypeStyle & EEmailAddress)
    {
        iMenu = CSwtControlMenu::NewL(iDisplay, NULL, 0);
        iMenu->SetObserver(this);
        iMenuItem = CSwtMenuItem::NewL(iDisplay, NULL, *iMenu, 0, 0, ESwtFetchEmail);
        // Email style implementation
        Editor().SetMaxLength(KSWTTEXTEXTENSION_EMAIL_LENGTH);
        Editor().SetAknEditorAllowedInputModes(EAknEditorAllInputModes);
        Editor().SetAknEditorCurrentInputMode(EAknEditorAlphaInputMode);
        Editor().SetAknEditorPermittedCaseModes(EAknEditorAllCaseModes);
        Editor().SetAknEditorCase(EAknEditorLowerCase);
        Editor().SetAknEditorCurrentCase(EAknEditorLowerCase);
        sct =  R_AVKON_EMAIL_ADDR_SPECIAL_CHARACTER_TABLE_DIALOG;
        flags |= EEikEdwinNoLineOrParaBreaks|EAknEditorFlagNoT9;
        //Adding Non Midlet Commands
        HBufC* buffer = iCoeEnv->AllocReadResourceLC(R_EMAIL_COMMAND_TEXT);
        TPtrC ptr = buffer->Des();
        iMenuItem->SetTextL(ptr);
        CleanupStack::PopAndDestroy(buffer);
    }
    else if (iTypeStyle & EUrl)
    {
        iMenu = CSwtControlMenu::NewL(iDisplay, NULL, 0);
        iMenu->SetObserver(this);
        iMenuItem = CSwtMenuItem::NewL(iDisplay, NULL, *iMenu, 0, 0, ESwtFetchUrl);
        // Url type implementation
        Editor().SetMaxLength(KSWTTEXTEXTENSION_URL_LENGTH);
        Editor().SetAknEditorAllowedInputModes(EAknEditorAllInputModes);
        Editor().SetAknEditorCurrentInputMode(EAknEditorAlphaInputMode);
        Editor().SetAknEditorPermittedCaseModes(EAknEditorAllCaseModes);
        Editor().SetAknEditorCase(EAknEditorLowerCase);
        Editor().SetAknEditorCurrentCase(EAknEditorLowerCase);
        sct = R_AVKON_URL_SPECIAL_CHARACTER_TABLE_DIALOG;
        flags |= EEikEdwinNoLineOrParaBreaks|EAknEditorFlagNoT9;
        //Adding Non Midlet Commands
        HBufC* buffer = iCoeEnv->AllocReadResourceLC(R_URL_COMMAND_TEXT);
        TPtrC ptr = buffer->Des();
        iMenuItem->SetTextL(ptr);
        CleanupStack::PopAndDestroy(buffer);
    }

    if (iTypeStyle & ELatinInputOnly)
    {
        //LatinInputOnly type implementation
        flags |=  EAknEditorFlagLatinInputModesOnly;
    }

    if (iTypeStyle & ENonPredictive)
    {
        //NonPredictive type implementation
        flags |= EAknEditorFlagNoT9;
    }
#ifdef RD_SCALABLE_UI_V2
    flags |= EAknEditorFlagDeliverVirtualKeyEventsToApplication;
#endif // RD_SCALABLE_UI_V2 
    Editor().SetAknEditorFlags(flags);
    Editor().SetAknEditorSpecialCharacterTable(sct);
    // make changes take effect
    TRAP_IGNORE(Editor().NotifyEditorStateObserverOfStateChangeL());
}


// ---------------------------------------------------------------------------
// Handling non midlet menu commands
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::ProcessCommandL(TInt aCmdId)
{

    switch (aCmdId)
    {

    case ESwtFetchEmail:
        FetchEmailL();// Fetching the Email address from the phone book
        break;

    case ESwtFetchUrl:
        FetchUrlL();// Fetching URL from favourites
        break;

    default:
        ASSERT(EFalse);
        break;
    }
}


// ---------------------------------------------------------------------------
// Return the control menu to the menu arranger
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtTextExtension::GetControlMenu() const
{
    if (iMenu)
    {
        return iMenu;
    }
    return ASwtControlBase::GetControlMenu();
}


#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// Return the control menu for the stylus popup menu
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtTextExtension::GetStylusPopupControlMenu() const
{
    return GetControlMenu();
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// Provides GUI that allows the user to select Email address from Phone Book
// and returns selected email id to the editor
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::FetchEmailL()
{
    // The dialog used to select the address doesn't work properly if there is no
    // status pane. Therefore, the status pane is temporarily made visible for
    // the duration of showing the dialog.
    MSwtShell* shell = iDisplay.UiUtils().TopMostTopShell();
    TBool fullScreenMode(EFalse);
    MSwtShell::TSwtStatusPaneStyle statusPaneStyle;
    MakeStatusPaneLargeL(shell, fullScreenMode, statusPaneStyle);

    CPbkContactEngine* pbkContactEngine = CPbkContactEngine::NewL();
    CleanupStack::PushL(pbkContactEngine);
    RPbkViewResourceFile pbkResource(*iCoeEnv);
    // Opening the default phone book
    TBool isOpen = EFalse;
    if (!pbkResource.IsOpen())
    {
        pbkResource.OpenL();
        isOpen = ETrue;
    }
    // Filtering phone book items which have email address
    CContactDatabase::TContactViewFilter filter(CContactDatabase::EMailable);
    CPbkSingleEntryFetchDlg::TParams params;
    params.iContactView = &pbkContactEngine->FilteredContactsViewL(filter);
    //Construct the dialog with filtered parameters
    CPbkSingleEntryFetchDlg* fetchDialog = CPbkSingleEntryFetchDlg::NewL(params);
    CleanupStack::PushL(fetchDialog);
    fetchDialog->SetMopParent(this);
    // Executes the dialog and waits for user selection
    TInt selection =  fetchDialog->ExecuteLD();
    CleanupStack::Pop(fetchDialog);
    if (selection)
    {
        //Retrieving the user selected email address
        const TContactItemId contactItemId = params.iSelectedEntry;
        CPbkContactItem* pbkItem = pbkContactEngine->ReadContactLC(contactItemId);
        TPbkContactItemField* emailField = pbkItem->FindField(EPbkFieldIdEmailAddress);
        TPtrC ptr = emailField->Text();
        Editor().SetTextL(&ptr);  // assigning email address to editor
        Editor().HandleTextChangedL();
        CleanupStack::PopAndDestroy(pbkItem);
    }
    if (isOpen)
    {
        pbkResource.Close();
    }
    CleanupStack::PopAndDestroy(pbkContactEngine);

    // Restore the original status pane style and fullscreenmode state
    RestoreStatusPaneL(shell, fullScreenMode, statusPaneStyle);
}


// ---------------------------------------------------------------------------
// Provides GUI that allows the user to select URL from Favourites Database
// and returns selected Bookmarked URL to the editor
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::FetchUrlL()
{
    // The dialog used to select the url doesn't work properly if there is no
    // status pane. Therefore, the status pane is temporarily made visible for
    // the duration of showing the dialog.
    MSwtShell* shell = iDisplay.UiUtils().TopMostTopShell();
    TBool fullScreenMode(EFalse);
    MSwtShell::TSwtStatusPaneStyle statusPaneStyle;
    MakeStatusPaneLargeL(shell, fullScreenMode, statusPaneStyle);

    RFavouritesSession favouritesSession;
    User::LeaveIfError(favouritesSession.Connect());
    CleanupClosePushL<RFavouritesSession>(favouritesSession);

    // Opening the Favourites Database
    RFavouritesDb favouritesDb;
    TInt error(favouritesDb.Open(favouritesSession, KBrowserBookmarks));
    if (!error)
    {
        CleanupClosePushL<RFavouritesDb>(favouritesDb);
        CFavouritesItemList* favouriteItemsList = new(ELeave) CFavouritesItemList();
        CleanupStack::PushL(favouriteItemsList);

        // Retrieving the items which have URL address into the items list
        User::LeaveIfError(favouritesDb.GetAll(*favouriteItemsList,
                                               KFavouritesNullUid,
                                               CFavouritesItem::EItem,
                                               NULL,
                                               KFavouritesNullContextId));
        TInt itemsCount = favouriteItemsList->Count();
        favouriteItemsList->SortL(&CompareItemsByNameL);  // Sorting the items by name

        // Defining the array with the size equals to the number of items retrieved
        CDesCArrayFlat* arrayList = new(ELeave) CDesCArrayFlat(
            itemsCount ? itemsCount : 1);

        CleanupStack::PushL(arrayList);
        for (TInt i = 0; i < itemsCount; i++)
        {
            // Adding items to the array
            const TInt size(((favouriteItemsList->At(i))->Name()).Length());
            HBufC* name = HBufC::NewLC(size + 1);
            name->Des().Append(KTAB);
            name->Des().Append((favouriteItemsList->At(i))->Name());
            arrayList->AppendL(*name);
            CleanupStack::PopAndDestroy(name);
        }
        TInt selectedItem(0);   // Default selected item
        CAknSelectionListDialog* selectionListDialog =
            CAknSelectionListDialog::NewL(selectedItem, arrayList, 0);

        // Constructing the dialog from the resource
        selectionListDialog->PrepareLC(R_SELECTION_LIST_DIALOG);
        selectionListDialog->SetMopParent(this);

        // Getting pointer to the AppUi's status pane
        CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();

        // Setting the dialog title to the title pane
        CAknNavigationControlContainer* naviContainer =
            static_cast<CAknNavigationControlContainer*>(statusPane->ControlL(
                        TUid::Uid(EEikStatusPaneUidNavi)));
        HBufC* buffer = iCoeEnv->AllocReadResourceLC(R_TITLEPANE_TITLE);
        TPtrC ptr = buffer->Des();
        if (iNaviDecorator)
        {
            delete iNaviDecorator;
            iNaviDecorator = NULL;
        }
        iNaviDecorator = naviContainer->CreateMessageLabelL(ptr);
        CleanupStack::PopAndDestroy(buffer);

        // Newly created navigation decorator onto the navigation panes object stack
        naviContainer->PushL(*iNaviDecorator);

        TInt selection(selectionListDialog->RunLD());   // pops the dialog from cleanupstack
        if (selection)
        {
            // Setting selected url to editor
            if (selectedItem >= 0)
            {
                TPtrC itemPtr((favouriteItemsList->At(selectedItem))->Url());
                Editor().SetTextL(&itemPtr);
                Editor().HandleTextChangedL();
            }
        }

        CleanupStack::PopAndDestroy(arrayList);
        CleanupStack::PopAndDestroy(favouriteItemsList);
        CleanupStack::PopAndDestroy(&favouritesDb);
    }
    CleanupStack::PopAndDestroy(&favouritesSession);

    // Restore the original status pane style and fullscreenmode state
    RestoreStatusPaneL(shell, fullScreenMode, statusPaneStyle);
}


// ---------------------------------------------------------------------------
// Sets input language and character subset
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::SetInitialInputModeL(TCasingModifier aCasingModifier, const TDesC* aMode)
{
    iInitialCurrentCase = 0;
    switch (aCasingModifier)
    {
    case ELowerCase:
        iInitialCurrentCase =  EAknEditorLowerCase;
        break;
    case EUpperCase:
        iInitialCurrentCase = EAknEditorUpperCase ;
        break;
    case ETitleCase://Fall-through
    case ETextCase:
        iInitialCurrentCase = EAknEditorTextCase ;
        break;
    default:
        iInitialCurrentCase = EAknEditorAllCaseModes;
        break;
    }

    iInitialCurrentInputMode = 0;
    iInitialCurrentLanguage = ELangTest;  // 0
    if (aMode)
    {
        HBufC* mode = aMode->AllocLC();
        if (mode->Compare(KUcbBasicLatin) == 0 || mode->Compare(KIsLatin) == 0)
        {
            // UCB_BASIC_LATIN, IS_LATIN
            iInitialCurrentLanguage = SelectLanguage(EIsLatin,ELangEnglish);
            iInitialCurrentInputMode = EAknEditorTextInputMode;

        }
        else if (mode->Compare(KIsLatinDigits) == 0)
        {
            // IS_LATIN_DIGITS
            iInitialCurrentInputMode = EAknEditorNumericInputMode;
        }
        else if (mode->Compare(KUcbArabic) == 0)
        {
            // UCB_ARABIC
            iInitialCurrentLanguage = ELangArabic;
        }
        else if (mode->Compare(KUcbHebrew) == 0)
        {
            // UCB_HEBREW
            iInitialCurrentLanguage = ELangHebrew;
        }
        else if (mode->Compare(KUcbGreek) == 0)
        {
            // UCB_GREEK
            iInitialCurrentLanguage = ELangGreek;
            iInitialCurrentCase = EAknEditorTextCase;
        }
        else if (mode->Compare(KUcbThai) == 0)
        {
            // UCB_THAI
#ifdef KFeatureIdThai
            if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdThai))
            {
                iInitialCurrentLanguage = ELangThai;
            }
#endif // KFeatureIdThai
        }
        else if (mode->Compare(KIsSimplifiedHanzi) == 0)
        {
            // IS_SIMPLIIFED_HANZI
#ifdef KFeatureIdChinese
            if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdChinese))
            {
                iInitialCurrentLanguage = ELangPrcChinese;
            }
#endif // KFeatureIdChinese
        }
        else if (mode->Compare(KUcbKatakana) == 0)
        {
            // UCB_KATAKANA
#ifdef KFeatureIdJapanese
            if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdJapanese))
            {
                iInitialCurrentLanguage = ELangJapanese;
                iInitialCurrentInputMode = EAknEditorFullWidthKatakanaInputMode;
            }
#endif // KFeatureIdJapanese
        }
        else if (mode->Compare(KUcbHiragana) == 0 || mode->Compare(KIsKanji) == 0)
        {
            // UCB_HIRAGANA, IS_KANJI
#ifdef KFeatureIdJapanese
            if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdJapanese))
            {
                iInitialCurrentLanguage = ELangJapanese;
                iInitialCurrentInputMode = EAknEditorHiraganaKanjiInputMode;
            }
#endif // KFeatureIdJapanese
        }
        else if (mode->Compare(KUcbHangulSyllables) == 0 || mode->Compare(KIsHanja) == 0)
        {
            // UCB_HANGUL_SYLLABLES, IS_HANJA
            iInitialCurrentLanguage = ELangKorean;

        }
        else if (mode->Compare(KIsTraditionalHanzi) == 0)
        {
            //IS_TRADITIONAL_HANZI
#ifdef KFeatureIdChinese
            if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdChinese))
            {
                iInitialCurrentLanguage = SelectLanguage(
                                              EIsTraditionalHanzi, ELangTest);
            }
#endif // KFeatureIdChinese
        }
        else if (mode->Compare(KUcbDevanagari) == 0)
        {
            //UCB_DEVANAGARI
            iInitialCurrentLanguage = ELangHindi;
        }
        else if (mode->Compare(KUcbBengali) == 0)
        {
            //UCB_BENGALI
            iInitialCurrentLanguage = ELangBengali;
        }
        else if (mode->Compare(KUcbArmenian) == 0)
        {
            //UCB_ARMENIAN
            iInitialCurrentLanguage = ELangArmenian;
        }
        else if (mode->Compare(KUcbCyrillic) == 0)
        {
            //UCB_CYRILLIC
            iInitialCurrentLanguage = SelectLanguage(EUcbCyrillic, ELangRussian);
            iInitialCurrentCase = EAknEditorTextCase;
        }

        else if (mode->Compare(KIsHalfwidthKatakana) == 0)
        {
            // IS_HALFWIDTH_KATAKANA
#ifdef KFeatureIdJapanese
            if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdJapanese))
            {
                iInitialCurrentLanguage = ELangJapanese;
                iInitialCurrentInputMode = EAknEditorKatakanaInputMode;
            }
#endif // KFeatureIdJapanese
        }
        else if (mode->Compare(KIsFullwidthDigits) == 0)
        {
            // IS_FULLWIDTH_DIGITS
#ifdef KFeatureIdJapanese
            if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdJapanese))
            {
                iInitialCurrentLanguage = ELangJapanese;
                iInitialCurrentInputMode = EAknEditorFullWidthNumericInputMode;
            }
#endif // KFeatureIdJapanese
        }
        else if (mode->Compare(KIsFullwidthLatin) == 0)
        {
            // IS_FULLWIDTH_DIGITS
#ifdef KFeatureIdJapanese
            if (CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdJapanese))
            {
                iInitialCurrentLanguage = ELangJapanese;
                iInitialCurrentInputMode = EAknEditorFullWidthTextInputMode;
            }
#endif // KFeatureIdJapanese
        }
        CleanupStack::PopAndDestroy(mode);   //mode
    }
    else
    {
        iInitialCurrentLanguage = CSwtLafFacade::GetLanguage();
    }
}


// ---------------------------------------------------------------------------
// Returns an intial input language for a given character subset.
// If the global input language or the display language belong to the desired
// character set, that language is returned as the initial input language.
// Otherwise the given default language is returned.  Note that this method
// is only used for character sets that contain more than one language.
// ---------------------------------------------------------------------------
//
TLanguage CSwtTextExtension::SelectLanguage(
    TInt aCharSubset,
    TLanguage aDefaultLanguage)
{
    TLanguage language = CSwtLafFacade::GetLanguage();
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


// ---------------------------------------------------------------------------
// Returns ETrue if the given language belongs to the given character subset
// ---------------------------------------------------------------------------
//
TBool CSwtTextExtension::IsLanguageInCharSubset(TLanguage aLanguage, TInt aCharSubset)
{
    switch (aLanguage)
    {
        // Cyrillic languages:
    case ELangBulgarian:
    case ELangRussian:
    case ELangSerbian:
    case ELangUkrainian:
        return (aCharSubset == EUcbCyrillic);
        // Arabic languages:
    case ELangArabic:
    case ELangFarsi:
    case ELangUrdu:
        return(aCharSubset == EUcbArabic);
        // Traditi onal Chinese languages, support latin, too:
    case ELangTaiwanChinese:
    case ELangHongKongChinese:
        return (aCharSubset == EIsTraditionalHanzi ||
                aCharSubset == EIsLatin);
        // Single languages that belong to other character subsets than latin
    case ELangHebrew:
    case ELangGreek:
    case ELangHindi:
    case ELangVietnamese:
    case ELangThai:
    case ELangBengali:
        return (aCharSubset == ECharSubsetNone);
        // All the rest are latin languages; Japanese and Simplified Chinese
        // included here,  because they support latin, too
    default:
        return (aCharSubset == EIsLatin);
    }
}


// =========================== VIRTUAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Virtual methods from ASwtControlBase
// Sets the Focus
// This is where the inputMode will be set (if needed).
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::FocusChanged(TDrawNow aDrawNow)
{
    if (iInitialCurrentLanguage)
    {
        Editor().SetAknEditorLocalLanguage(iInitialCurrentLanguage);
        if (iInitialCurrentInputMode == 0)
        {
            // clears any number mode used previously
            Editor().SetAknEditorCurrentInputMode(EAknEditorNullInputMode);
        }
    }

    if (iInitialCurrentInputMode)
    {
        Editor().SetAknEditorCurrentInputMode(iInitialCurrentInputMode);
    }

    if (iInitialCurrentCase)
    {
        TInt initialCurrentCase = iInitialCurrentCase;
        if ((iStyle & KSwtStylePassword ||
                iTypeStyle & EEmailAddress ||
                iTypeStyle & EUrl) &&
                iInitialCurrentCase == EAknEditorTextCase)
        {
            // Text case is not used in passwords, emailaddrs and urls
            initialCurrentCase = EAknEditorLowerCase;
        }
        Editor().SetAknEditorCurrentCase(initialCurrentCase);
        Editor().SetAknEditorCase(initialCurrentCase);
    }
    CSwtTextBase::FocusChanged(aDrawNow);
}


// ---------------------------------------------------------------------------
// Virtual methods from MSwtTextExtension
// ---------------------------------------------------------------------------
//
MSwtText* CSwtTextExtension::Text()
{
    return this;
}


// ---------------------------------------------------------------------------
// CSwtTextExtension::MakeStatusPaneLargeL
// This is only needed because the dialogs don't work unless status pane
// is certain size.
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::MakeStatusPaneLargeL(MSwtShell* aShell,
        TBool& aFullScreenMode,
        MSwtShell::TSwtStatusPaneStyle& aStyle)
{
    if (aShell)
    {
        aStyle = aShell->StatusPaneStyle();
        if (aStyle != MSwtShell::EStyleLargeStatusPane)
        {
            MSwtMobileShell* mobileShell = aShell->MobileShellInterface();
            ASSERT(mobileShell);
            // Also FullScreenMode has to be temporarily disabled because setting status
            // pane style doesn't have any effect otherwise.
            aFullScreenMode = aShell->FullScreenMode();
            if (aFullScreenMode)
            {
                aShell->SetFullScreen(EFalse);
            }
            mobileShell->ChangeTrimL(aShell->Control()->Style(),
                                     MSwtShell::EStyleLargeStatusPane);
        }
    }
}


// ---------------------------------------------------------------------------
// CSwtTextExtension::RestoreStatusPaneL
// ---------------------------------------------------------------------------
//
void CSwtTextExtension::RestoreStatusPaneL(MSwtShell* aShell,
        const TBool& aFullScreenMode,
        const MSwtShell::TSwtStatusPaneStyle& aStyle)
{
    if (aShell)
    {
        if (aFullScreenMode)
        {
            aShell->SetFullScreen(ETrue);
        }
        if (aStyle != MSwtShell::EStyleLargeStatusPane)
        {
            MSwtMobileShell* mobileShell = aShell->MobileShellInterface();
            if (mobileShell)
            {
                mobileShell->ChangeTrimL(aShell->Control()->Style(), aStyle);
            }
        }
    }
}
