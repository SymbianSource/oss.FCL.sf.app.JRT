/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eikenv.h>
#include <eikappui.h>
#include <coecntrl.h>
#include <coeccntx.h>
#include <coecobs.h>
#include <bassnd.h>
#include <coesndpy.h>
#include <hal.h>
#include <lcdui.rsg>
#include <barsread.h>

#include <aknappui.h>
#include <aknsoundsystem.h>
#include <avkon.hrh> // for Alert Sounds
#include <hwrmlight.h> // backlight
#include <hwrmvibra.h> // vibra
// for layout data in BestImageSize function
#include <aknlayoutscalable_avkon.cdl.h> // for layout data
// API used Color function - mapping Java color type to Avkon-skinned color
#include <AknsUtils.h>

// CMIDKeyDecoder API in funcions related to keys
#include "CMIDKeyDecoder.h"
#include "CMIDUtils.h"
#include "CMIDUIManager.h"

#ifdef RD_SCALABLE_UI_V2
#include <AknUtils.h>
#endif // RD_SCALABLE_UI_V2

#include <e32property.h>
#include <AvkonInternalCRKeys.h>
#include <centralrepository.h>
#include <AknFepInternalCRKeys.h>

#ifdef RD_INTELLIGENT_TEXT_INPUT
#include <PtiEngine.h>
#endif //RD_INTELLIGENT_TEXT_INPUT


// macros definitions for outputing logs
#include <j2me/jdebug.h>

// LOCAL CONSTANTS AND MACROS

/**
 *Info messages for Wins Flashbacklight and Vibrate
 *For development purpose only, not localized
 */
#if defined(__WINS__)
_LIT(KMsgFlashBacklight,"Flash count:");
_LIT(KMsgVibrate,"Vibrating for ");
#endif

// line feed, carriage return, line separator, paragraph separator
_LIT(KSeparators, "\x000a\x000d\x2028\x2029");

// keyboard types
_LIT(KKeyboardNone,            "None");
_LIT(KKeyboardPhoneKeypad,     "PhoneKeypad");
_LIT(KKeyboardFullKeyboard,    "FullKeyboard");
_LIT(KKeyboardLimited4x10,     "LimitedKeyboard4x10");
_LIT(KKeyboardLimited3x11,     "LimitedKeyboard3x11");
_LIT(KKeyboardHalfKeyboard,    "HalfKeyboard");
_LIT(KKeyboardCustom,          "Custom");
_LIT(KKeyboardUnknown,         "Unknown");


// constants for color bits shifting (for creation of 0x00RRGGBB color value)
const TInt KRedBitsShift = 16;
const TInt KGreenBitsShift = 8;
const TInt KBlueBitsShift = 0;

// constant for result size used by PTI Engine for mapping keys, this value is
// based e.g. on PtiXt9CandidateList implementation

#ifdef RD_INTELLIGENT_TEXT_INPUT
const TInt KPTIEngineResultSize = 32;
#endif // RD_INTELLIGENT_TEXT_INPUT


// ============================ MEMBER FUNCTIONS ===============================

//
//
// class MMIDUtils

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MMIDUtils* CMIDUtils::NewL(MMIDEnv& aEnv, CMIDUIManager* aUIManager)
{
    CMIDUtils* utils = new(ELeave) CMIDUtils(aEnv, aUIManager);
    CleanupStack::PushL(utils);
    utils->ConstructL();
    CleanupStack::Pop(utils);
    return utils;
}

/**
 * CAknKeySoundSystem is use to play sounds,
 * emulator implementation, which does not support
 * this feature.
 */
#if defined(__WINSCW__)
TBool CMIDUtils::PlaySound(TInt /*aSoundType*/)
{
    return EFalse;
}
#else // __WINSCW__

/**
 * CAknKeySoundSystem is use to play sounds
 */
TBool CMIDUtils::PlaySound(TInt aSoundType)
{
    TInt toneType = -1;
    switch (aSoundType)
    {
    case MMIDAlert::EAlarm:
        toneType = (TInt) EAvkonSIDInformationTone;
        break;
    case MMIDAlert::EConfirmation:
        toneType = (TInt) EAvkonSIDConfirmationTone;
        break;
    case MMIDAlert::EError:
        toneType = (TInt) EAvkonSIDErrorTone;
        break;
    case MMIDAlert::EWarning:
        toneType = (TInt) EAvkonSIDWarningTone;
        break;
    case MMIDAlert::EInfo:
        toneType = (TInt) EAvkonSIDInformationTone;
        break;
    case MMIDAlert::ENone: // No Alerttype set
    default:
        toneType = (TInt) EAvkonSIDNoSound;
        break;
    }

    CAknKeySoundSystem* soundSystem = iAvkonAppUi->KeySounds();
    if (toneType  != -1)
    {
        soundSystem->PlaySound(toneType);
    }
    return ETrue;
}
#endif // __WINSCW__

/**
 * Determines if a key code should be sent to Canvas.keyPressed(int keyCode)
 */
TBool CMIDUtils::IsJavaKey(TInt aScanCode)
{
    ASSERT(iKeyDecoder);
    return iKeyDecoder->IsJavaKey(aScanCode);
}

/**
 * Maps EPOC scan code to negative key code required by MIDP spec.
 * For keys that have no corresponding Unicode character,
 * the implementation must use negative values.
 */
TInt CMIDUtils::MapNonUnicodeKey(TUint aScanCode)
{
    ASSERT(iKeyDecoder);
    return iKeyDecoder->MapNonUnicodeKey(aScanCode);
}

/**
 * Gets a key code that corresponds to the specified game action on the device.
 *
 * Returns zero if aGameAction is not a valid game action.
 *
 * @see CMIDKeyDecoder::GetKeyCode()
 */
TInt CMIDUtils::GetKeyCode(TInt aGameAction)
{
    ASSERT(iKeyDecoder);
    return iKeyDecoder->GetKeyCode(aGameAction);
}

/**
 * Gets an informative key string for a key. The string returned will resemble the text
 * physically printed on the key. This string is suitable for displaying to the user.
 * For example, on a device with function keys F1 through F4, calling this method on
 * the keyCode for the F1 key will return the string "F1". A typical use for this string
 * will be to compose help text such as "Press F1 to proceed."
 *
 * This method will return a non-empty string for every valid key code.
 * @see CMIDKeyDecoder::GetKeyName()
 */
void CMIDUtils::GetKeyName(TDes& aText,TInt aKeyCode)
{
    ASSERT(iKeyDecoder);
    return iKeyDecoder->GetKeyName(aText, aKeyCode);
}

/**
* Gets the game action associated with the given key code of the device.
*
* Return zero if no game action is associated with this key code.
*
* @see CMIDKeyDecoder::GameAction()
*/
TInt CMIDUtils::GetGameAction(TInt aKeyCode)
{
    ASSERT(iKeyDecoder);
    return iKeyDecoder->GameAction(aKeyCode);
}

void CMIDUtils::MapJavaToETextChars(HBufC* aText,TBool aSupportLineBreaks)
{
    TPtr text = aText->Des();
    TInt pos = text.Length();
    for (; pos>0;)
    {
        TText& ch=(text)[--pos];
        switch (ch)
        {
        case '\n':
            //From 3.0m onwards: If line breaks are not supported, returns ESpace instead of ETabCharacter
            ch = (TText)(aSupportLineBreaks ? CEditableText::EParagraphDelimiter : CEditableText::ESpace);
            break;
        case '\f':
            //From 3.0m onwards: If line breaks are not supported, returns ESpace instead of ETabCharacter
            ch = (TText)(aSupportLineBreaks ? CEditableText::EPageBreak : CEditableText::ESpace);
            break;
        case '\t':
            ch = CEditableText::ETabCharacter;
            break;
        }
    }
}

void CMIDUtils::MapETextToJavaChars(HBufC* aText)
{
    if (!aText)
        return;
    TPtr text = aText->Des();
    TInt pos = text.Length();
    for (; pos>0;)
    {
        TText& ch=(text)[--pos];
        switch (ch)
        {
        case CEditableText::EParagraphDelimiter:
            ch = '\n';
            break;
        case CEditableText::EPageBreak:
            ch = '\f';
            break;
        case CEditableText::ETabCharacter:
            ch = '\t';
            break;
        }
    }
}

/**
 * Looks for first character which indicates text direction such as Arabic character meaning right-to-left.
 * (similar code can be found in lcdgr)
 */
TBool CMIDUtils::IsStronglyRightToLeft(HBufC* aText)
{
    TPtr text = aText->Des();
    TInt pos;
    TInt len = text.Length();
    for (pos = 0; pos < len; pos++)
    {
        TChar ch((text)[pos]);
        TChar::TCharInfo info;
        ch.GetInfo(info);
        switch (info.iBdCategory)
        {
        case TChar::ERightToLeft:
        case TChar::ERightToLeftArabic:
        case TChar::ERightToLeftEmbedding:
        case TChar::ERightToLeftOverride:
        case TChar::EArabicNumber:
            return true;
        case TChar::ELeftToRight:
            return false;
        default:
            ;
        }
    }
    return false;
}

/**
 * Reverses order of contents of array.
 */
void CMIDUtils::Reverse(HBufC* aText)
{
    TPtr text = aText->Des();
    TInt s = 0;
    TInt e = text.Length() - 1;
    while (s < e)
    {
        TText t = text[s];
        text[s] = text[e];
        text[e] = t;
        ++s;
        --e;
    }
}

/** Some key events should be ignored, for example when opening or closing a device. */
TBool CMIDUtils::IgnoreKeyEvent(TUint aKeyEvent)
{
    return (aKeyEvent == EKeyGripOpen)   ||
           (aKeyEvent == EKeyGripClose)  ||
           (aKeyEvent == EKeyTwistOpen)  ||
           (aKeyEvent == EKeyTwistClose) ||
           (aKeyEvent == EKeyFlipOpen)   ||
           (aKeyEvent == EKeyFlipClose)  ||
           (aKeyEvent == EKeyQwertyOn)   ||
           (aKeyEvent == EKeyQwertyOff);
}


/** Creates an identical copy of the given bitmap. This utility function is used instead
of MMIDBitmapImage::CreateColorBitmapL() because this latter looses transparency information
when the display mode is EColor16MA (it blits on a white bitmap...).
*/
CFbsBitmap* CMIDUtils::CopyBitmapL(CFbsBitmap* aSource)
{
    CFbsBitmap* target = NULL;

    if (aSource)
    {
        target = new(ELeave) CFbsBitmap();
        target->Create(aSource->SizeInPixels(), aSource->DisplayMode());

        //if there is no memory for new bitmap, target has null adress pointer
        //and calling Mem::Copy causes a panic.
        //So if target->DataAdress() returns null pointer,
        //function leave with KErrNoMemory -> java.lang.OutOfMemoryError is thrown
        if (!target->DataAddress())
        {
            User::Leave(KErrNoMemory);
        }

        TInt nBytes = aSource->SizeInPixels().iHeight *
                      aSource->ScanLineLength(aSource->SizeInPixels().iWidth, aSource->DisplayMode());

        target->LockHeap();
        Mem::Copy(target->DataAddress(), aSource->DataAddress(), nBytes);
        target->UnlockHeap();
    }

    return target;
}

TBool CMIDUtils::IsLineSeparator(const TText aChar)
{
    return (KSeparators().Locate(aChar) != KErrNotFound);
}

TBool CMIDUtils::HasPointerEvents()
{
    return (HasPen() || HasMouse());
}

TBool CMIDUtils::HasPointerMotionEvents()
{
    return (HasPen() || HasMouse());
}

TBool CMIDUtils::HasRepeatEvents()
{
    return ETrue;
}

TBool CMIDUtils::HasPen()
{
#ifdef RD_SCALABLE_UI_V2
    return AknLayoutUtils::PenEnabled();
#else
    TInt hasPen = EFalse;
    return hasPen;
#endif // RD_SCALABLE_UI_V2
}

TBool CMIDUtils::HasMouse()
{
    TInt hasMouse = EFalse;
    return hasMouse;
}

TSize CMIDUtils::BestImageSize(TImageType aImageType) const
{
    TAknWindowLineLayout lineLayout;

    switch (aImageType)
    {
    case EListImage:
        lineLayout = AknLayoutScalable_Avkon::list_single_large_graphic_pane_g1(0).LayoutLine();
        break;
    case EChoiceImage:
        lineLayout = AknLayoutScalable_Avkon::list_single_2graphic_pane_g1_cp4().LayoutLine();
        break;
    case EAlertImage:
        lineLayout = AknLayoutScalable_Avkon::popup_midp_note_alarm_window_g1(0).LayoutLine();
        break;
    default:
        return TSize();
    }
    return TSize(lineLayout.iW, lineLayout.iH);
}


/**
 * Requests a flashing effect for the device's backlight
 * Returns false as the emulator does not support this feature.
 */
#if defined(__WINSCW__)

TBool CMIDUtils::FlashBacklightL(const TTimeIntervalMicroSeconds32& /*aDuration*/)
{
    return EFalse;
}

#else // __WINSCW__
/**
 * Requests a flashing effect for the device's backlight
 * Must return true if the backlight can be controlled by the application
 */
TBool CMIDUtils::FlashBacklightL(const TTimeIntervalMicroSeconds32& aDuration)
{
    if (!iLight)
    {
        iLight = CHWRMLight::NewL();
    }
    // SupportedTargets() returns the supported targets as a bitmap.
    if ((iLight->SupportedTargets() &
            CHWRMLight::EPrimaryDisplayAndKeyboard) !=
            CHWRMLight::EPrimaryDisplayAndKeyboard)
    {
        return EFalse;
    }

    TInt duration = aDuration.Int() / 1000;
    if (!duration)
    {
        // 0 would be infinite
        duration = 1;
    }
    iLight->LightBlinkL(CHWRMLight::EPrimaryDisplayAndKeyboard,
                        duration);
    return ETrue;
}
#endif // __WINSCW__


/**
 * There is no API for vibrate. Left to be customised according to each device
 * Printing InfoMsg for the user for the duration given to vibrate
 * Returns false in emulator builds as it does not support this feature.
 */
#if defined(__WINSCW__)
TBool CMIDUtils::Vibrate(const TTimeIntervalMicroSeconds32& /*aDuration*/)
{
    return EFalse;
}
#else // __WINSCW__

/**
 * There is no API for vibrate. Left to be customised according to each device
 * Printing InfoMsg for the user for the duration given to vibrate
 * Must return true if the vibrator can be controlled by the application
 */
TBool CMIDUtils::Vibrate(const TTimeIntervalMicroSeconds32& aDuration)
{
    TInt duration = aDuration.Int() / 1000; // convert micro to milli

    // as this method can't leave, and only returns a boolean, we must
    // return false if anything goes wrong.

    if (!iVibra)
    {
        TRAPD(err1, iVibra = CHWRMVibra::NewL());
        if (err1 != KErrNone)
        {
            return EFalse;
        }
    }

    TInt err2 = KErrNone;

    if (duration)
    {
        TRAP(err2, iVibra->StartVibraL(duration));
    }
    else
    {
        TRAP(err2, iVibra->StopVibraL());
    }

    if (err2 == KErrNone)
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}
#endif // __WINSCW__

/**
 * Mapping Java color type to Avkon-skinned color.
 */
TInt CMIDUtils::Color(TColorType aColorSpecifier)
{
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb rgbColor = 0;
    TInt result = KErrNone;

    // get skinned color
    switch (aColorSpecifier)
    {
    case EColorBackground:
        result = AknsUtils::GetCachedColor(skin, rgbColor,
                                           KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG10);
        break;
    case EColorForeground:
        result = AknsUtils::GetCachedColor(skin, rgbColor,
                                           KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
        break;
    case EColorHighlightedBackground:
        result = AknsUtils::GetCachedColor(skin, rgbColor,
                                           KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG14);
        break;
    case EColorHighlightedForeground:
        result = AknsUtils::GetCachedColor(skin, rgbColor,
                                           KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10);
        break;
    case EColorBorder:
        result = AknsUtils::GetCachedColor(skin, rgbColor,
                                           KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG12);
        break;
    case EColorHighlightedBorder:
        result = AknsUtils::GetCachedColor(skin, rgbColor,
                                           KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG15);
        break;
    default:
        // wrong argument
        return KErrArgument;
    }

    // check result
    if (result == KErrNone)
    {
        return rgbColor.Red() << KRedBitsShift |
               rgbColor.Green() << KGreenBitsShift |
               rgbColor.Blue() << KBlueBitsShift;
    }
    else
    {
        // return black in case of some skinning related problem
        return 0;
    }
}

MMIDUtils::TGraphicsType CMIDUtils::BorderStyle(TBool aHighlighted)
{
    return (aHighlighted) ? ESolid : EDotted;
}

/**
 * Return FontSpec for given Font Specifier, Font Spec is chosen for each specifier from exisiting.
 */
SFontSpec CMIDUtils::FontSpecifierSpecs(MMIDFont::TFontSpecifier aSpecifier)
{
    SFontSpec fontSpec;

    if (aSpecifier == MMIDFont::EInputText)
    {
        fontSpec.iFace = MMIDFont::EProportional;
        fontSpec.iStyle = MMIDFont::EBold;
        fontSpec.iSize = MMIDFont::EMedium;
    }
    else
    {
        fontSpec.iFace = MMIDFont::EProportional;
        fontSpec.iStyle = MMIDFont::EPlain;
        fontSpec.iSize = MMIDFont::ESmall;
    }
    return fontSpec;
}

/**
 * Function fills aText descriptor with name of actual keyboard layout.
 * Keyboard layout is stored in central repository. Obtained TInt value
 * is mapped to descriptor:
 * 0 No Keyboard            -> "None"
 * 1 Keyboard with 12 key   -> "PhoneKeypad"
 * 2 QWERTY 4x12 layout     -> "FullKeyboard"
 * 3 QWERTY 4x10 layout     -> "LimitedKeyboard4x10"
 * 4 QWERTY 3 x 11 layout   -> "LimitedKeyboard3x11"
 * 5 Half QWERTY            -> "HalfKeyboard"
 * 6 Custom QWERTY          -> "Custom"
 * In case of undefined value, aText is empty.
 */
void CMIDUtils::GetKeyboardTypeName(TDes* aText)
{
    TInt keyboardType = 0;
    RProperty::Get(KCRUidAvkon, KAknKeyBoardLayout, keyboardType);

    aText->Zero(); //empty the descriptor
    switch (keyboardType)
    {
    case ENoKeyboard: //No Keyboard
        aText->Append(KKeyboardNone);
        break;
    case EKeyboardWith12Key: //Keyboard with 12 key
        aText->Append(KKeyboardPhoneKeypad);
        break;
    case EQWERTY4x12Layout: //QWERTY 4x12 layout
        aText->Append(KKeyboardFullKeyboard);
        break;
    case EQWERTY4x10Layout: //QWERTY 4x10 layout
        aText->Append(KKeyboardLimited4x10);
        break;
    case EQWERTY3x11Layout: //QWERTY 3 x 11 layout
        aText->Append(KKeyboardLimited3x11);
        break;
    case EHalfQWERTY: //Half QWERTY
        aText->Append(KKeyboardHalfKeyboard);
        break;
    case ECustomQWERTY: //Custom QWERTY
        aText->Append(KKeyboardCustom);
        break;
    default:
        aText->Append(KKeyboardUnknown);
    }
}

/**
 * Function sets the scan code of the latest key event.
 */
void CMIDUtils::SetLastKeyEvent(const TKeyEvent& aEvent)
{
    iLastScanCode = aEvent.iScanCode;
    iModifier = aEvent.iModifiers;
}

/**
 * Function returns the scan code of the latest key event.
 */
TInt CMIDUtils::GetKeyScanCode()
{
    return iLastScanCode;
}

/**
 * Function returns modifier value of the latest key event.
 */
TInt CMIDUtils::GetKeyModifier()
{
    return iModifier;
}

void CMIDUtils::MappingDataForKey(TKeyEvent& aEvent, TEventCode aType)
{
    if (!iQwertyMode)
    {
        return;
    }

    TBool issticky = iStickyHandler.HandleKey(aEvent, aType);

    TUint modifiers = iStickyHandler.Modifiers();

    if (!issticky && modifiers == 0)
    {
        modifiers = aEvent.iModifiers;
    }
    if (!issticky && aType == EEventKeyUp)
    {
        iStickyHandler.Reset();
    }


#ifdef RD_INTELLIGENT_TEXT_INPUT
    TPtiTextCase textCase = EPtiCaseLower;

    if (modifiers & EModifierShift)
    {
        textCase = EPtiCaseUpper;
    }
    if (modifiers & EModifierRightFunc)
    {
        //EModifierRightFunc in Fn key
        textCase = EPtiCaseFnLower;
    }
    else if (modifiers & EModifierLeftFunc)
    {
        textCase = EPtiCaseChrLower;
    }

    //If Control key is held, don't use PtiEngine to map keyEvent.iCode
    if (modifiers & EModifierLeftCtrl ||
            modifiers & EModifierRightCtrl)
    {
        iStickyHandler.Reset();
        return;
    }

    TBuf<KPTIEngineResultSize> mapData;

    iPtiEngine->MappingDataForKey(
        (TPtiKey)aEvent.iScanCode, mapData, textCase);

    if (mapData.Length() > 0)
    {
        // set event code to the first mapped
        aEvent.iCode = mapData[0];
    }
#endif // RD_INTELLIGENT_TEXT_INPUT

}

void CMIDUtils::UpdatePTIEngineStatusL()
{

#ifdef RD_INTELLIGENT_TEXT_INPUT

    RProperty::Get(KCRUidAvkon, KAknQwertyInputModeActive, iQwertyMode);

    // keyboard layout default value: 0 - No Keyboard
    TInt keyboardLayout = 0;
    // Read keyboard layout from central repository
    RProperty::Get(KCRUidAvkon, KAknKeyBoardLayout, keyboardLayout);

    TPtiEngineInputMode mode = EPtiEngineInputModeNone;
    switch (keyboardLayout)
    {
    case EPtiKeyboard12Key:
        mode = EPtiEngineMultitapping;
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
        CallToJavaPtiVariationL(keyboardLayout);
#else
        iPtiEngine->SetKeyboardType((TPtiKeyboardType)keyboardLayout);
#endif
        break;
    case EPtiKeyboardQwerty4x12:
    case EPtiKeyboardQwerty4x10:
    case EPtiKeyboardQwerty3x11:
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
        CallToJavaPtiVariationL(keyboardLayout);
#else
        iPtiEngine->SetKeyboardType((TPtiKeyboardType)keyboardLayout);
#endif
        mode = EPtiEngineQwerty;
        break;
    case EPtiKeyboardHalfQwerty:
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
        CallToJavaPtiVariationL(keyboardLayout);
#else
        iPtiEngine->SetKeyboardType((TPtiKeyboardType)keyboardLayout);
#endif
        mode = EPtiEngineHalfQwerty;
        break;
    default:
        break;
    }
    // input language default value: 0 (automatic)
    TInt inputLang = 0;
    if (iRepository)
    {
        TInt ret = iRepository->Get(KAknFepInputTxtLang, inputLang);
    }

    // Change input language to western, if input language is Chinese
    // ELangPrcChinese      - 31
    // ELangTaiwanChinese   - 29
    // ELangHongKongChinese - 30
    if (iQwertyMode)
    {
        if (inputLang == ELangPrcChinese ||
                inputLang == ELangTaiwanChinese ||
                inputLang == ELangHongKongChinese)
        {
            inputLang = ELangEnglish;
        }
    }
    TRAPD(err, iPtiEngine->ActivateLanguageL(inputLang, mode));
    if (KErrNone != err)
    {
        DEBUG_INT("CMIDUtils::UpdatePTIEngineStatusL - ActivateLanguageL leaved with error = %d", err);
        return;
    }


#endif // RD_INTELLIGENT_TEXT_INPUT

}


void CMIDUtils::HandleResourceChangedL()
{
    UpdatePTIEngineStatusL();
}

void CMIDUtils::HandleForegroundL(TBool aForeground)
{
    if (aForeground)
    {
        UpdatePTIEngineStatusL();
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDUtils::CMIDUtils(MMIDEnv& aEnv, CMIDUIManager* aUIManager)
    : iEnv(&aEnv)
    , iUIManager(aUIManager)
    , iScalingData()
    , iQwertyMode(EFalse)
    , iStickyKey(0)
    , iLastScanCode(0)
    , iModifier(0)
    , iScalingDataInitialized(EFalse)
{}

CMIDUtils::~CMIDUtils()
{
    delete iLight;
    delete iVibra;

#ifdef RD_INTELLIGENT_TEXT_INPUT

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    iPtiSupportLib.Close();
#endif //RD_JAVA_S60_RELEASE_5_0_IAD

    delete iPtiEngine;
    iPtiEngine = NULL;

    delete iRepository;
    iRepository = NULL;
#endif //RD_INTELLIGENT_TEXT_INPUT

}

#ifdef RD_INTELLIGENT_TEXT_INPUT
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
void CMIDUtils::CallToJavaPtiVariationL(TInt aType)
{
    if (!iPtiSupportLib.Handle())
    {
        _LIT(KLibName, "javaptivariation.dll");
        iPtiSupportLib.Load(KLibName);
    }

    if (iPtiSupportLib.Handle())
    {
        TJavaPtiVariationLibEntry libEntryL = (TJavaPtiVariationLibEntry)iPtiSupportLib.Lookup(1);
        libEntryL(*iPtiEngine, aType);
    }
}
#endif //   RD_JAVA_S60_RELEASE_5_0_IAD
#endif // RD_INTELLIGENT_TEXT_INPUT 

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDUtils::ConstructL()
{
    ASSERT(iUIManager);
    iKeyDecoder = iUIManager->OpenKeyDecoderL();
    iMenuHandler = iUIManager->GetMenuHandler();

#ifdef RD_INTELLIGENT_TEXT_INPUT
    iPtiEngine = CPtiEngine::NewL(ETrue);
    iRepository = CRepository::NewL(KCRUidAknFep);
    UpdatePTIEngineStatusL();
#endif // RD_INTELLIGENT_TEXT_INPUT


}

TInt CMIDUtils::DoScaling(TInt aNonScaled, TInt aDirection)
{
    DEBUG("TInt CMIDUtils::DoScaling(TInt aNonScaled, TInt aDirection)");

    // initial value of return structure
    TInt scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // scaling in entered direction
    // no scaling if direction is not valid
    if (aDirection == EHorizontal)
    {
        scaled *= data.iRatioX;
    }
    else if (aDirection == EVertical)
    {
        scaled *= data.iRatioY;
    }

    DEBUG_INT("DoScaling processed %d", scaled);

    // return of scaled value
    return scaled;
}

TPoint CMIDUtils::DoScaling(TPoint aNonScaled)
{
    DEBUG("TPoint CMIDUtils::DoScaling(TPoint aNonScaled)");

    // initial value of return structure
    TPoint scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // scaling of point
    scaled.iX *= data.iRatioX;
    scaled.iY *= data.iRatioY;

    DEBUG_INT2("DoScaling processed %d %d", scaled.iX, scaled.iY);

    // return of scaled value
    return scaled;
}

TPoint CMIDUtils::DoScalingAndPositioning(TPoint aNonScaled)
{
    DEBUG("TPoint CMIDUtils::DoScalingAndPositioning(TPoint aNonScaled)");

    // initial value of return structure
    TPoint scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // scaling of point
    scaled.iX *= data.iRatioX;
    scaled.iY *= data.iRatioY;

    // get of canvas origin
    TSize subtract = data.iScreenSize - data.iCanvasSize;
    TPoint canvasOrigin = TPoint(subtract.iWidth / 2, subtract.iHeight / 2);

    // move according to canvas origin
    scaled += canvasOrigin;

    DEBUG_INT2("DoScalingAndPositioning processed %d %d",
               scaled.iX, scaled.iY);

    // return of scaled value
    return scaled;
}

TRect CMIDUtils::DoScaling(TRect aNonScaled)
{
    DEBUG("TRect CMIDUtils::DoScaling(TRect aNonScaled)");

    // initial value of return structure
    TRect scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data= GetScalingData();

    // scaling of rect
    scaled.iBr.iX *= data.iRatioX;
    scaled.iBr.iY *= data.iRatioY;
    scaled.iTl.iX *= data.iRatioX;
    scaled.iTl.iY *= data.iRatioY;

    DEBUG_INT4("DoScaling processed %d %d",
               scaled.iBr.iX, scaled.iBr.iY, scaled.iTl.iX, scaled.iTl.iY);

    // return of scaled value
    return scaled;
}

TRect CMIDUtils::DoScalingAndPositioning(TRect aNonScaled)
{
    DEBUG("TRect CMIDUtils::DoScalingAndPositioning(TRect aNonScaled)");

    // initial value of return structure
    TRect scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // scaling of rect
    scaled.iBr.iX *= data.iRatioX;
    scaled.iBr.iY *= data.iRatioY;
    scaled.iTl.iX *= data.iRatioX;
    scaled.iTl.iY *= data.iRatioY;

    // get of canvas origin
    TSize subtract = data.iScreenSize - data.iCanvasSize;
    TPoint canvasOrigin = TPoint(subtract.iWidth / 2, subtract.iHeight / 2);

    // move according to canvas origin
    scaled.Move(canvasOrigin);

    DEBUG_INT4("DoScalingAndPositioning processed %d %d",
               scaled.iBr.iX, scaled.iBr.iY, scaled.iTl.iX, scaled.iTl.iY);

    // return of scaled value
    return scaled;
}

TSize CMIDUtils::DoScaling(TSize aNonScaled)
{
    DEBUG("TSize CMIDUtils::DoScaling(TSize aNonScaled)");

    // initial value of return structure
    TSize scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // scaling of size
    scaled.iWidth *= data.iRatioX;
    scaled.iHeight *= data.iRatioY;

    DEBUG_INT2("DoScaling processed %d %d", scaled.iWidth, scaled.iHeight);

    // return of scaled value
    return scaled;
}

TInt CMIDUtils::DoDescaling(TInt aNonScaled, TInt aDirection)
{
    DEBUG("TInt CMIDUtils::DoDescaling(TInt aNonScaled, TInt aDirection)");

    // initial value of return structure
    TInt scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // descaling in entered direction
    // no scaling if direction is not valid
    if (aDirection == EHorizontal)
    {
        scaled /= data.iRatioX;
    }
    else if (aDirection == EVertical)
    {
        scaled /= data.iRatioY;
    }

    DEBUG_INT("DoDescaling processed %d", scaled);

    // return of descaled value
    return scaled;
}

TPoint CMIDUtils::DoDescaling(TPoint aNonScaled)
{
    DEBUG("TPoint CMIDUtils::DoDescaling(TPoint aNonScaled)");

    // initial value of return structure
    TPoint scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // descaling of point
    scaled.iX /= data.iRatioX;
    scaled.iY /= data.iRatioY;

    DEBUG_INT2("DoDescaling processed %d %d", scaled.iX, scaled.iY);

    // return of descaled value
    return scaled;
}

TPoint CMIDUtils::DoDescalingAndPositioning(TPoint aNonScaled)
{
    DEBUG("TPoint CMIDUtils::DoDescalingAndPositioning(TPoint aNonScaled)");

    // initial value of return structure
    TPoint scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // get of canvas origin
    TSize subtract = data.iScreenSize - data.iCanvasSize;
    TPoint canvasOrigin = TPoint(subtract.iWidth / 2, subtract.iHeight / 2);

    // move according to canvas origin
    scaled -= canvasOrigin;

    // descaling of point
    scaled.iX /= data.iRatioX;
    scaled.iY /= data.iRatioY;

    DEBUG_INT2("DoDescalingAndPositioning processed %d %d",
               scaled.iX, scaled.iY);

    // return of descaled value
    return scaled;
}

TRect CMIDUtils::DoDescaling(TRect aNonScaled)
{
    DEBUG("TRect CMIDUtils::DoDescaling(TRect aNonScaled)");

    // initial value of return structure
    TRect scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // descaling of rect
    scaled.iBr.iX /= data.iRatioX;
    scaled.iBr.iY /= data.iRatioY;
    scaled.iTl.iX /= data.iRatioX;
    scaled.iTl.iY /= data.iRatioY;

    DEBUG_INT4("DoDescaling processed %d %d",
               scaled.iBr.iX, scaled.iBr.iY, scaled.iTl.iX, scaled.iTl.iY);

    // return of descaled value
    return scaled;
}

TRect CMIDUtils::DoDescalingAndPositioning(TRect aNonScaled)
{
    DEBUG("TRect CMIDUtils::DoDescalingAndPositioning(TRect aNonScaled)");

    // initial value of return structure
    TRect scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // get of canvas origin
    TSize subtract = data.iScreenSize - data.iCanvasSize;
    TPoint canvasOrigin = TPoint(subtract.iWidth / 2, subtract.iHeight / 2);

    // move according to canvas origin
    scaled.Move(-canvasOrigin);

    // descaling of rect
    scaled.iBr.iX /= data.iRatioX;
    scaled.iBr.iY /= data.iRatioY;
    scaled.iTl.iX /= data.iRatioX;
    scaled.iTl.iY /= data.iRatioY;

    DEBUG_INT4("DoDescalingAndPositioning processed %d %d",
               scaled.iBr.iX, scaled.iBr.iY, scaled.iTl.iX, scaled.iTl.iY);

    // return of descaled value
    return scaled;
}

TSize CMIDUtils::DoDescaling(TSize aNonScaled)
{
    DEBUG("TSize CMIDUtils::DoDescaling(TSize aNonScaled)");
    TSize scaled = aNonScaled;

    // get of all needed values for scaling
    TScalingData data = GetScalingData();

    // descaling of size
    scaled.iWidth /= data.iRatioX;
    scaled.iHeight /= data.iRatioY;

    DEBUG_INT2("DoDescaling processed %d %d", scaled.iWidth, scaled.iHeight);

    // return of descaled value
    return scaled;
}

CMIDUtils::TScalingData CMIDUtils::GetScalingData()
{
	UpdateScalingData();
    return iScalingData;
}

void CMIDUtils::UpdateScalingData()
{
    // Create local instance of TScalingData
    TScalingData data = TScalingData();

    //
    iScalingDataInitialized = ETrue;

    // Get actual rect of screen without with eventual OSK.
    TRect screenRect =iEnv->Current()->GetCanvasRectFromLaf();

    // Traslate of rect of screen into size
    data.iScreenSize = screenRect.Size();

    // Check if scaling is is on now.
    if (iMenuHandler->IsScalingEffectiveInCurrentScreen())
    {

        // Get original and target size from JAD attributes
        data.iOriginalSize = iMenuHandler->GetScalingParameterOrgMIDletScrSize();
        data.iTargetSize = iMenuHandler->GetScalingParameterTargetMIDletScrSize();

        // Check if JAD attribute allows orientation change during scalling
        // is present. If it is present, this switches orientation of original
        // size, if it is needed.
        if (iMenuHandler->GetScalingParameterScaleMIDletOnOrientSwitch())
        {
            // portait
            if (data.iScreenSize.iWidth < data.iScreenSize.iHeight)
            {
                data.iOriginalSize = TSize(Min(data.iOriginalSize.iHeight,
                                               data.iOriginalSize.iWidth),
                                           Max(data.iOriginalSize.iHeight,
                                               data.iOriginalSize.iWidth));
            }
            // landscape
            else
            {
                data.iOriginalSize = TSize(Max(data.iOriginalSize.iHeight,
                                               data.iOriginalSize.iWidth),
                                           Min(data.iOriginalSize.iHeight,
                                               data.iOriginalSize.iWidth));
            }
        }

    }

    if (data.iTargetSize != TSize())
    {
        // When target size is set, then possible horizontal and vertical
        // scaling factor are various. And in this case canvas size is
        //  equal to target size.
        data.iRatioX = (TReal) data.iTargetSize.iWidth / data.iOriginalSize.iWidth;
        data.iRatioY = (TReal) data.iTargetSize.iHeight / data.iOriginalSize.iHeight;
        data.iCanvasSize = data.iTargetSize;
    }
    else if (data.iOriginalSize != TSize())
    {
        // When target size is not set, then only one scaling factor is present.
        // It is smaller of horizontal and vertical scaling factors.
        // And in this case canvas have size calculated according to
        // the scaling factor.
        data.iRatioX = (TReal) data.iScreenSize.iWidth / data.iOriginalSize.iWidth;
        data.iRatioY = (TReal) data.iScreenSize.iHeight / data.iOriginalSize.iHeight;
        TReal ratio = Min(data.iRatioX, data.iRatioY);
        data.iRatioX = ratio;
        data.iRatioY = ratio;
        data.iCanvasSize = TSize(data.iOriginalSize.iWidth * ratio,
                                 data.iOriginalSize.iHeight * ratio);
    }
    else
    {
        // When scaling is off, then canvas ocupied whole screen.
        data.iCanvasSize = data.iScreenSize;
    }

    // Set local variable to iScalingData
    iScalingData = data;
}

// ============================ TStickyKeysHandler ===========================

CMIDUtils::TStickyKeysHandler::TStickyKeysHandler()
{
    ResetState();
    iFnKeyState = EKeyNotActive;
    iShiftKeyState = EKeyNotActive;
}

void  CMIDUtils::TStickyKeysHandler::Reset()
{
    ResetState();
    iStickyModifier = 0;
    // When reseting state, we must take locked sticky keys into account
    // If Fn is locked, then after reset Fn still must be added to midifiers
    if (iFnKeyState == EKeyLocked)
    {
        AddFnToStickyModifiers();
    }
    else
    {
        iFnKeyState = EKeyNotActive;
    }

    // Fn has higher priority than shift, and only one sticky key might
    // be locked at the time
    // For half QWERTY we need to process also shift as a regular key when Fn is active
    if (iFnKeyState == EKeyNotActive)
    {
        // Pre-setting shift
        switch (iShiftKeyState)
        {
        case EKeyLocked:
            // Shift is locked, so add modifier
            AddShiftToStickyModifiers();
            break;
        case EKeyActive:
            // Shift wasn't locked
            iShiftKeyState = EKeyNotActive;
            break;
        case EKeyNotActiveNext:
            // Shift was locked, but should be deactivated for next keypress,
            // so add it to modifiers...
            AddShiftToStickyModifiers();
            // .. and move back to locked state
            iShiftKeyState = EKeyLocked;
            break;
        default:
            iShiftKeyState = EKeyNotActive;
            break;
        }
    }
}

TBool CMIDUtils::TStickyKeysHandler::HandleKey(
    const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (!IsSticky(aKeyEvent))
    {
        // Check Fn key
        if (aKeyEvent.iModifiers & EModifierRightFunc)
        {
            iStickyModifier = aKeyEvent.iModifiers;
        }
        ResetState();
        return EFalse;
    }

    // Depending on sticky key state...
    switch (iStickyKeyState)
    {
    case EKeyNotPressed:
        // No sticky key was pressed
        if (aType == EEventKeyDown)
        {
            // If key down event arrived, remember it's keycode and
            // move to next state (sticky key pressed)
            iLastStickyScanCode = aKeyEvent.iScanCode;
            iStickyKeyState = EKeyWasPressed;
        }
        else
        {
            // Reset handler's state
            Reset();
        }
        break;
    case EKeyWasPressed:
        // Sticky key was pressed before
        if (aType == EEventKeyUp)
        {
            // If same sticky key was released as pressed in previous state,
            // move to the next state (sticky key released)
            if (iLastStickyScanCode == aKeyEvent.iScanCode)
            {
                iStickyKeyState = EKeyWasReleased;
            }
        }
        else
        {
            // Reset handler's state
            Reset();
        }
        break;
    default:
        Reset();
        break;
    }

    TBool wasStickyKey = EFalse;
    if (EKeyWasReleased == iStickyKeyState)
    {
        // Sticky key was pressed and released, so we need to get
        // modifiers from scan code.
        TUint modifier = ModifierFromScanCode(aKeyEvent.iScanCode);

        // Add/remove modifier to/from stored modifiers
        if (iStickyModifier & modifier)
        {
            iStickyModifier &= ~modifier;
        }
        else
        {
            iStickyModifier |= modifier;
        }

        ResetState();
        HandleLockableKeys(modifier);
        wasStickyKey = ETrue;
    }

    return wasStickyKey;
}

TInt  CMIDUtils::TStickyKeysHandler::Modifiers() const
{
    return iStickyModifier;
}

void  CMIDUtils::TStickyKeysHandler::ResetState()
{
    iLastStickyScanCode = 0;
    iStickyKeyState = EKeyNotPressed;
}

TUint CMIDUtils::TStickyKeysHandler::ModifierFromScanCode(TInt aScanCode)
{
    switch (aScanCode)
    {
    case EStdKeyLeftShift:
        return EModifierShift | EModifierLeftShift;
    case EStdKeyRightShift:
        return EModifierShift | EModifierRightShift;
    case EStdKeyLeftAlt:
        return EModifierAlt | EModifierLeftAlt;
    case EStdKeyRightAlt:
        return EModifierAlt | EModifierRightAlt;
    case EStdKeyLeftCtrl:
        return EModifierCtrl | EModifierLeftCtrl;
    case EStdKeyRightCtrl:
        return EModifierCtrl | EModifierRightCtrl;
    case EStdKeyLeftFunc:
        return EModifierLeftFunc;
    case EStdKeyRightFunc:
        return EModifierFunc | EModifierRightFunc;
    default:
        return 0;
    }
}


TBool CMIDUtils::TStickyKeysHandler::IsSticky(const TKeyEvent& aKeyEvent)
{
    TBool isStickyKey = EFalse;
    switch (aKeyEvent.iScanCode)
    {
    case EStdKeyLeftFunc:
    case EStdKeyLeftShift:
    case EStdKeyRightShift:
        isStickyKey = !IsFnModifierOn(iStickyModifier) &&
                      !(aKeyEvent.iModifiers & EModifierRightFunc);
        break;
    case EStdKeyLeftAlt:
    case EStdKeyRightAlt:
    case EStdKeyLeftCtrl:
    case EStdKeyRightCtrl:
    case EStdKeyRightFunc:
        isStickyKey =  ETrue;
        break;
    default:
        break;
    }

    return isStickyKey;
}

void CMIDUtils::TStickyKeysHandler::HandleLockableKeys(TUint aModifiers)
{
    if (IsFnModifierOn(aModifiers))
    {
        iActualFnModifierValue = 0;
        if (aModifiers & EModifierLeftFunc)
        {
            iActualFnModifierValue = EModifierLeftFunc;
        }
        if (aModifiers & EModifierRightFunc)
        {
            iActualFnModifierValue = EModifierRightFunc;
        }
        HandleFnKey();
    }

    if (IsShiftModifierOn(aModifiers))
    {
        iActualShiftModifierValue = 0;
        if (aModifiers & EModifierLeftShift)
        {
            iActualShiftModifierValue = EModifierLeftShift;
        }
        if (aModifiers & EModifierRightShift)
        {
            iActualShiftModifierValue = EModifierRightShift;
        }
        HandleShiftKey();
    }
}

/**
 * Handle event with modifier of Fn key
 */
void CMIDUtils::TStickyKeysHandler::HandleFnKey()
{
    switch (iFnKeyState)
    {
    case EKeyNotActive:
        iFnKeyState = EKeyActive;
        break;
    case EKeyActive:
        iFnKeyState = EKeyLocked;
        AddFnToStickyModifiers();
        break;
    case EKeyLocked:
        iFnKeyState = EKeyNotActive;
        // If Fn lock is disabled, also disable Shift lock.
        iShiftKeyState = EKeyNotActive;
        Reset();
        break;
    }
}

/**
 * Handle event with modifier of Shift key
 */
void CMIDUtils::TStickyKeysHandler::HandleShiftKey()
{
    switch (iShiftKeyState)
    {
    case EKeyNotActive:
        iShiftKeyState = EKeyActive;
        break;
    case EKeyActive:
        iShiftKeyState = EKeyLocked;
        AddShiftToStickyModifiers();
        break;
    case EKeyLocked:
        iStickyModifier = 0;
        if (iFnKeyState == EKeyLocked)
        {
            AddFnToStickyModifiers();
        }
        iShiftKeyState = EKeyNotActiveNext;
        break;
    case EKeyNotActiveNext:
        iShiftKeyState = EKeyNotActive;
        Reset();
        break;
    }
}

void CMIDUtils::TStickyKeysHandler::AddShiftToStickyModifiers()
{
    iStickyModifier |= EModifierShift | iActualShiftModifierValue;
}

void CMIDUtils::TStickyKeysHandler::AddFnToStickyModifiers()
{
    //Chr key (EModifierLeftFunc) is not handled as sticky key because
    //it should not function in any special way
    if (iActualFnModifierValue != EModifierLeftFunc)
    {
        iStickyModifier |= EModifierFunc | iActualFnModifierValue;
    }
}

inline TBool CMIDUtils::TStickyKeysHandler::IsFnModifierOn(
    const TUint aModifiers) const
{
    return aModifiers & EModifierFunc ||
           aModifiers & EModifierRightFunc;
}


inline TBool CMIDUtils::TStickyKeysHandler::IsShiftModifierOn(
    const TUint aModifiers) const
{
    return aModifiers & EModifierShift ||
           aModifiers & EModifierLeftShift ||
           aModifiers & EModifierRightShift;
}

TBool CMIDUtils::IsScalingEnabled()
{
    // If iScalingData is not initialized, we do it.
    if (!iScalingDataInitialized)
    {
        UpdateScalingData();
    }

    //If iOrgMIDletScrSize has been initialized then scaling is on.
    //It's enough to check either height or width only.
    return (iScalingData.iOriginalSize.iHeight != 0);
}

TRect CMIDUtils::GetOnScreenCanvasRect()
{
    if (!iScalingDataInitialized)
    {
        UpdateScalingData();
    }

    TSize subtract = iScalingData.iScreenSize - iScalingData.iCanvasSize;
    TPoint canvasOrigin = TPoint(subtract.iWidth / 2, subtract.iHeight / 2);
    return TRect(canvasOrigin, iScalingData.iCanvasSize);
}

// End of File
