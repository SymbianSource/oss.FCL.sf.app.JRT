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

#include <e32keys.h>
#include <e32property.h>
#include <AknUtils.h>
#include "swtkeymapper.h"
#include "swtmidmediakeyslistener.h"

// These are copies of constansts from Avkon. This is not pretty but necessary
// since they are defined in internal header files.
const TUid KCRUidAvkon = { 0x101F876E };
const TUint32 KAknQwertyInputModeActive = 0x00000004;

#ifdef RD_INTELLIGENT_TEXT_INPUT
const TUint32 KAknFepInputTxtLang = 0x00000005;
const TUid KCRUidAknFep = { 0x101F876D };
const TUint32 KAknKeyBoardLayout = 0x0000000B;

// constant for result size used by PTI Engine for mapping keys, this value is
// based e.g. on PtiXt9CandidateList implementation
const TInt KPTIEngineResultSize = 32;
#endif //RD_INTELLIGENT_TEXT_INPUT


/**
 * Mapping table between Symbian scan codes and SWT key codes
 *
 * The table is sorted by ascending Symbian scan codes.
 *
 * Note: two SWT key codes have no mapping, <code>SWT.PAUSE</code> and
 *       <code>SWT.KEYPAD_EQUAL</code>.
 */
const CSwtKeyMapper::TScanCodeMapping CSwtKeyMapper::KScanCodeTable[] =
{
    // Media keys first to preserver scan code table ascending scan code order
    {MSwtMediaKeysListener::ESwtMediaKeyStop,     0, MSwtMediaKeysListener::ESwtMediaKeyStop},     // Media key Stop
    {MSwtMediaKeysListener::ESwtMediaKeyNext,     0, MSwtMediaKeysListener::ESwtMediaKeyNext},     // Media key Next
    {MSwtMediaKeysListener::ESwtMediaKeyPrevious, 0, MSwtMediaKeysListener::ESwtMediaKeyPrevious}, // Media key Previous
    {MSwtMediaKeysListener::ESwtMediaKeyPlay,     0, MSwtMediaKeysListener::ESwtMediaKeyPlay},  // Media key Play

    //iScanCode                 iKeyCode       iSwtCode
    {EStdKeyPrintScreen,     EKeyPrintScreen, ESwtKeyPrintScreen},
    {EStdKeyPause,           EKeyPause,       ESwtKeyPause},
    {EStdKeyHome,            EKeyHome,        ESwtKeyHome},
    {EStdKeyEnd,             EKeyEnd,         ESwtKeyEnd},
    {EStdKeyPageUp,          EKeyPageUp,      ESwtKeyPageUp},
    {EStdKeyPageDown,        EKeyPageDown,    ESwtKeyPageDown},
    {EStdKeyInsert,          EKeyInsert,      ESwtKeyInsert},
    {EStdKeyLeftArrow,       EKeyLeftArrow,   ESwtKeyArrowLeft},
    {EStdKeyRightArrow,      EKeyRightArrow,  ESwtKeyArrowRight},
    {EStdKeyUpArrow,         EKeyUpArrow,     ESwtKeyArrowUp},
    {EStdKeyDownArrow,       EKeyDownArrow,   ESwtKeyArrowDown},

    // Special handling for Ctrl, Shift & Alt. Those keys don't have dedicated
    // SWT keys codes, so we use the modifier code instead.
    {EStdKeyLeftShift,       EKeyLeftShift,   KSwtModifierShift},
    // In S60 right shift scancode is mapped to edit key
    {EStdKeyRightShift,      EKeyRightShift,  -50}, // Edit key
    //{EStdKeyRightShift,      EKeyRightShift,  KSwtModifierShift},
    {EStdKeyLeftAlt,         EKeyLeftAlt,     KSwtModifierAlt},
    {EStdKeyRightAlt,        EKeyRightAlt,    KSwtModifierAlt},
    {EStdKeyLeftCtrl,        EKeyLeftCtrl,    KSwtModifierControl},
    {EStdKeyRightCtrl,       EKeyRightCtrl,   KSwtModifierControl},
    {EStdKeyLeftFunc,        EKeyLeftFunc,    KSwtModifierCommand},
    {EStdKeyRightFunc,       EKeyRightFunc,   KSwtModifierCommand},

    {EStdKeyCapsLock,        EKeyCapsLock,    ESwtKeyCapsLock},
    {EStdKeyNumLock,         EKeyNumLock,     ESwtKeyNumLock},
    {EStdKeyScrollLock,      EKeyScrollLock,  ESwtKeyScrollLock},
    //Function keys do not work properly, They are considered dead keys.
    //S60  do not have function keys anyway.
//  {EStdKeyF1,              EKeyF1,          ESwtKeyF1},
//  {EStdKeyF2,              EKeyF2,          ESwtKeyF2},
//  {EStdKeyF3,              EKeyF3,          ESwtKeyF3},
//  {EStdKeyF4,              EKeyF4,          ESwtKeyF4},
//  {EStdKeyF5,              EKeyF5,          ESwtKeyF5},
//  {EStdKeyF6,              EKeyF6,          ESwtKeyF6},
//  {EStdKeyF7,              EKeyF7,          ESwtKeyF7},
//  {EStdKeyF8,              EKeyF8,          ESwtKeyF8},
//  {EStdKeyF9,              EKeyF9,          ESwtKeyF9},
//  {EStdKeyF10,             EKeyF10,         ESwtKeyF10},
//  {EStdKeyF11,             EKeyF11,         ESwtKeyF11},
//  {EStdKeyF12,             EKeyF12,         ESwtKeyF12},
//  {EStdKeyF13,             EKeyF13,         ESwtKeyF13},
//  {EStdKeyF14,             EKeyF14,         ESwtKeyF14},
//  {EStdKeyF15,             EKeyF15,         ESwtKeyF15},

    {EStdKeyNkpForwardSlash, 0,               ESwtKeyKeypadDivide},
    {EStdKeyNkpAsterisk,     0,               ESwtKeyKeypadMultiply},
    {EStdKeyNkpMinus,        0,               ESwtKeyKeypadSubtract},
    {EStdKeyNkpPlus,         0,               ESwtKeyKeypadAdd},
    {EStdKeyNkpEnter,        0,               ESwtKeyKeypadCR},
    {EStdKeyNkp1,            0,               ESwtKeyKeypad1},
    {EStdKeyNkp2,            0,               ESwtKeyKeypad2},
    {EStdKeyNkp3,            0,               ESwtKeyKeypad3},
    {EStdKeyNkp4,            0,               ESwtKeyKeypad4},
    {EStdKeyNkp5,            0,               ESwtKeyKeypad5},
    {EStdKeyNkp6,            0,               ESwtKeyKeypad6},
    {EStdKeyNkp7,            0,               ESwtKeyKeypad7},
    {EStdKeyNkp8,            0,               ESwtKeyKeypad8},
    {EStdKeyNkp9,            0,               ESwtKeyKeypad9},
    {EStdKeyNkp0,            0,               ESwtKeyKeypad0},
    {EStdKeyNkpFullStop,     0,               ESwtKeyKeypadDecimal},
    {EStdKeyHelp,            EKeyHelp,        ESwtKeyHelp},

    {EStdKeyDevice0,         EKeyDevice0,     -6}, // SoftKey 1
    {EStdKeyDevice1,         EKeyDevice1,     -7}, // SoftKey 2


    {EStdKeyDevice3,         EKeyDevice3,     -5},  // Selection key for S60.
    {EStdKeyDevice6,         EKeyDevice6,     -13}, // Voice key
    {EStdKeyDevice7,         EKeyDevice7,     ESwtKeyArrowRight},
    {EStdKeyDevice8,         EKeyDevice8,     ESwtKeyArrowUp},
    {EStdKeyDevice9,         EKeyDevice9,     ESwtKeyArrowDown},
    {EStdKeyDeviceA,         EKeyDeviceA,     EStdKeyEnter},

    {EStdKeyApplication0,    EKeyApplication0,  -12}, // Applications key
    {EStdKeyApplication9,    EKeyApplication9,  -16}, // Clock-wise rotate.
    {EStdKeyApplicationA,    EKeyApplicationA,  -17}, // Counter-clock-wise rotate.
    {EStdKeyYes,             EKeyYes,           -10}, // Send key
    {EStdKeyNo,              EKeyNo,            -11}  // End key

};


// ======== MEMBER FUNCTIONS ========


inline TInt CSwtKeyMapper::MappingTableSize()
{
    return sizeof(KScanCodeTable)/sizeof(KScanCodeTable[0]);
}

/**
 * Constructor, empty.
 */
inline CSwtKeyMapper::CSwtKeyMapper()
{
#ifdef _DEBUG
    // Ensure the table is sorted in ascending scan code order
    const TInt count = MappingTableSize();
    for (TInt i=1; i<count; ++i)
        ASSERT(KScanCodeTable[i-1].iScanCode < KScanCodeTable[i].iScanCode);
#endif
}

/**
 * Destructor
 */
CSwtKeyMapper::~CSwtKeyMapper()
{
    if (iKeyMappings)
    {
        iKeyMappings->Reset();
        delete iKeyMappings;
    }

#ifdef RD_INTELLIGENT_TEXT_INPUT
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    iPtiSupportLib.Close();
#endif //RD_JAVA_S60_RELEASE_5_0_IAD

    if (iPtiEngine)
    {
        TInt err = KErrNone;
        TRAP(err, iPtiEngine->CloseCurrentLanguageL());
        delete iPtiEngine;
    }
    if (iRepository)
    {
        delete iRepository;
    }
    if (iSwtStickyKeysHandler)
    {
        delete iSwtStickyKeysHandler;
    }
#endif //RD_INTELLIGENT_TEXT_INPUT   
}

/**
 * 2nd phase constructor
 */
void CSwtKeyMapper::ConstructL()
{
    iKeyMappings = new(ELeave) CKeyMappings(4);

#ifdef RD_INTELLIGENT_TEXT_INPUT
    iPtiEngine = NULL;
    iRepository = NULL;
    iSwtStickyKeysHandler = CSwtStickyKeysHandler::NewL();
#endif //RD_INTELLIGENT_TEXT_INPUT
}

/**
 * 1st & 2nd phase constructors wrapper
 */
CSwtKeyMapper* CSwtKeyMapper::NewL()
{
    CSwtKeyMapper* self = new(ELeave) CSwtKeyMapper;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/**
 * Tests if a key sends repeat events (<code>EEventKey</code>)
 */
TBool CSwtKeyMapper::IsRepeatKey(TInt aScanCode)
{
    switch (aScanCode)
    {
    case EStdKeyPrintScreen:
    case EStdKeyLeftShift:
    case EStdKeyRightShift:
    case EStdKeyLeftAlt:
    case EStdKeyRightAlt:
    case EStdKeyLeftCtrl:
    case EStdKeyRightCtrl:
    case EStdKeyLeftFunc:
    case EStdKeyRightFunc:
    case EStdKeyCapsLock:
    case EStdKeyNumLock:
    case EStdKeyScrollLock:
    case EStdKeyDevice0:
    case EStdKeyDevice1:
        return EFalse;
    case EStdKeyDevice3:
#ifdef RD_JAVA_S60_RELEASE_9_2
        // Avoiding duplicate down events, Selection key does send EEventKey!
        return ETrue;
#else
        return !AknLayoutUtils::MSKEnabled();
#endif // RD_JAVA_S60_RELEASE_9_2
    default:
        return ETrue;
    }
}

/**
 *
 */
TSwtKeyEventData CSwtKeyMapper::MapKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    ASSERT(aType==EEventKeyDown || aType==EEventKey || aType==EEventKeyUp);

    TKeyEvent keyEvent(aKeyEvent);

#ifdef RD_INTELLIGENT_TEXT_INPUT
    TBool wasModifierHandledAsAKey;
    TBool wasKeyMappedByPtiEngine = RemapKeyCodeByPtiEngineL(
                                        keyEvent, aType, wasModifierHandledAsAKey);
#endif //RD_INTELLIGENT_TEXT_INPUT

    TSwtKeyEventData result;
    result.iType      = ESwtEventNone;
    result.iCharacter = 0;
    result.iKeyCode   = 0;
    result.iStateMask = 0;

    TKeyArrayFix key(0, ECmpTInt);

    // The character info is not provided at key down and key up events.
    // We delay sending of the key down event until we receive a key event.
    // When we receive a key up event we simply look up what was the value of
    // the character for the matching key event.

    // Key down event: create a slot for the character info to be stored later on
    if (aType != EEventKeyUp)
    {
        TKeyMapping mapping(keyEvent.iScanCode);


        // There is a really problem in InsertIsqL(). According to the specs of Symbian:
        // "The function leaves with a KErrAlreadyExists error code
        // if an element with the same key already exists within the array."
        // Problem is that when InsertIsqL() calls User::Leave(KErrAlreadyExists),
        // 1032 bytes are allocated during the Leave process.
        // We currently don't know why, but this allocation will never be freed.
        // So rather than using InsertIsqL(), we will use FindIsq() and InsertL().

        /*
                TRAPD(error, iKeyMappings->InsertIsqL(mapping, key));
                if (error == KErrAlreadyExists) // This should never happen, but still we overwrite the existing value
                    {
                    TInt index;
                    iKeyMappings->FindIsq(mapping, key, index);
                    (*iKeyMappings)[index] = mapping;
                    }
                else
                    User::LeaveIfError(error);
        */

        // Non-repeat keys don't generate EEventKey and don't have a character
        // anyway, so we can send key down now.
        // EEventKeyDown can be posted only if the key does not generate EEventKey!
        if (!IsRepeatKey(keyEvent.iScanCode))
        {
            result.iType      = ESwtEventKeyDown;
            result.iCharacter = 0;
            // set the iKeycode, it is used on KeyUp.
            mapping.iKeyCode   = GetSwtKeyCode(keyEvent);
            result.iKeyCode   = mapping.iKeyCode;
            result.iStateMask = GetSwtStateMask(keyEvent);

#ifdef RD_INTELLIGENT_TEXT_INPUT
            if (wasKeyMappedByPtiEngine && wasModifierHandledAsAKey)
            {
                // Handling half-qwerty characters mapped to modifier keys
                mapping.iCharacter = TChar(
                                         (mapping.iKeyCode < ENonCharacterKeyBase) ?
                                         mapping.iKeyCode : 0);
                ++mapping.iCount;
                result.iCharacter = mapping.iCharacter;
                result.iType = ESwtEventKeyDown;
                // When Fn is pressed
                result.iStateMask = keyEvent.iModifiers & EModifierRightFunc ?
                                    KSwtModifierCommand : 0;
            }
#endif
        }

        TInt index;
        TInt findIsqReturn = iKeyMappings->FindIsq(mapping, key, index);
        if (findIsqReturn == 0)
        {
            // The current mapping is already in the array
            // We will already overwrite the existing value
            (*iKeyMappings)[index] = mapping;
        }
        else
        {
            iKeyMappings->InsertL(index,mapping);
        }
    }

    // Key or key up
    if (aType == EEventKey || aType == EEventKeyUp)
    {
        // Find the slot created during key down.
        TInt  index;
        TBool error = iKeyMappings->FindIsq(
                          TKeyMapping(keyEvent.iScanCode), key, index);

        if (error)
        {
            // This should never happen, silently fail (i.e. skip the key)
            return result;
        }

        TKeyMapping& mapping = (*iKeyMappings)[index];

        // Key up: check for dead keys
        if (aType == EEventKeyUp)
        {
            // A dead key is a key that doesn't generate a character, but will
            // combine with the next key. For example ^ + e = ê where ^ is the
            // dead key. A dead key has a EEventKeyDown followed by EEventKeyUp
            // but no EEventKey in between. In SWT, dead keys don't trigger events,
            // so we simply skip them.
            if (mapping.iCount == 0 && IsRepeatKey(keyEvent.iScanCode))
            {
                return result;
            }
        }
        else if (aType == EEventKey)
        {
            // Key event: store the character value
            mapping.iCharacter = TChar(
                                     (keyEvent.iCode < ENonCharacterKeyBase) ?
                                     keyEvent.iCode : 0);
            mapping.iKeyCode   = GetSwtKeyCode(keyEvent);
            ++mapping.iCount;
        }

        // Set up result
        result.iType      = (aType == EEventKeyUp) ? ESwtEventKeyUp : ESwtEventKeyDown;
        result.iCharacter = mapping.iCharacter;
        result.iKeyCode   = mapping.iKeyCode;
        result.iStateMask = GetSwtStateMask(keyEvent);
    }

    return result;
}

/**
 * Converts a Symbian scan code to an SWT key code
 */
TInt CSwtKeyMapper::GetSwtKeyCode(const TKeyEvent& aKeyEvent)
{
    const TInt count = MappingTableSize();

    // Search (by dichotomy) the mapping table to find the matching SWT key code
    TInt i1 = 0;
    TInt i2 = count;

    while ((i2 - i1) > 1)
    {
        TInt i = (i1 + i2) / 2;

        if (KScanCodeTable[i].iScanCode <= aKeyEvent.iScanCode)
        {
            i1 = i;
        }
        else
        {
            i2 = i;
        }
    }

    if (KScanCodeTable[i1].iScanCode == aKeyEvent.iScanCode)
    {
        // Found
        TUint result = KScanCodeTable[i1].iSwtCode;
        TInt qwertyMode = 0;
        switch (KScanCodeTable[i1].iScanCode)
        {
        case EStdKeyBackspace:
        case EStdKeyRightShift:
            //handle qwerty
            RProperty::Get(KCRUidAvkon,
                           KAknQwertyInputModeActive, qwertyMode);
            if (qwertyMode)
            {
                result = KScanCodeTable[i1].iScanCode == EStdKeyBackspace ?
                         aKeyEvent.iCode : KSwtModifierShift;
            }
            break;

#ifdef RD_INTELLIGENT_TEXT_INPUT
        case EStdKeyLeftShift:
        case EStdKeyLeftFunc:
            if (0 != aKeyEvent.iCode)
            {
                if (IsHalfQwertyKeyboard())
                {
                    // For half-QWERTY devices left shift and Chr
                    // behaves as a key when FN key was pressed
                    // before it.
                    // In such case there is proper keycode already.
                    result = aKeyEvent.iCode;
                }
            }
            break;
#endif //RD_INTELLIGENT_TEXT_INPUT                

        default:
            break;
        }

        return result;
    }
    else
    {
        // Not found
        const TUint keyCode = aKeyEvent.iCode;
        if (keyCode == 0)
        {
            // return symbian scan code + scancodebase
            return KSwtScanCodeBase + aKeyEvent.iScanCode;
        }
        else
        {
            return keyCode;
        }
    }
}

/**
 * Converts Symbian modifiers to SWT state masks
 */
TInt CSwtKeyMapper::GetSwtStateMask(const TKeyEvent& aKeyEvent)
{
    TInt result = 0;
    if (aKeyEvent.iModifiers & EModifierCtrl)
        result |= KSwtModifierControl;
    if (aKeyEvent.iModifiers & EModifierShift)
        result |=  KSwtModifierShift;
    if (aKeyEvent.iModifiers & EModifierAlt)
        result |= KSwtModifierAlt;
    if (aKeyEvent.iModifiers & EModifierFunc)
        result |= KSwtModifierCommand;
    return result;
}


/**
 * brief Converts a SWT key code to Symbian key code and scan code
 *
 * @param aSwtKeyCode      The SWT key code
 * @param aSymbianScanCode The Symbian scan code (see TStdScanCode).
 * @param aSymbianKeyCode  The Symbian key code (see TKeyCode). Could be NULL.
 *
 * aSymbianKeyCode is not modified if aSwtKeyCode is a character code.
 */
void CSwtKeyMapper::GetSymbianCodes(TInt aSwtKeyCode, TInt& aSymbianScanCode, TUint* aSymbianKeyCode)
{
    const TInt count = MappingTableSize();

    for (TInt i=0; i<count; ++i)
    {
        const TScanCodeMapping& mapping = KScanCodeTable[i];
        if (mapping.iSwtCode == aSwtKeyCode)
        {
            if (aSymbianKeyCode!=NULL)
            {
                *aSymbianKeyCode  = mapping.iKeyCode;
            }
            aSymbianScanCode = mapping.iScanCode;
            return;
        }
    }

    aSymbianScanCode = aSwtKeyCode;
}

/**
 * Converts a SWT key code to a Symbian scan code
 */
TInt CSwtKeyMapper::GetSymbianScanCode(TInt aSwtKeyCode)
{
    TInt  scanCode;
    GetSymbianCodes(aSwtKeyCode, scanCode);
    return scanCode;
}

/**
 * Converts SWT state masks to Symbian modifier
 */
TUint CSwtKeyMapper::GetSymbianModifier(const TSwtKeyEventData& aData)
{
    TUint modifier = 0;
    if (aData.iStateMask & KSwtModifierControl)
        modifier |= EModifierCtrl;
    if (aData.iStateMask & KSwtModifierShift)
        modifier |=  EModifierShift;
    if (aData.iStateMask & KSwtModifierAlt)
        modifier |= EModifierAlt;
    if (aData.iStateMask & KSwtModifierCommand)
        modifier |= EModifierFunc;

    if ((ESwtKeyKeypadSubtract<=aData.iKeyCode) && (aData.iKeyCode<=ESwtKeyKeypad9)
            || (aData.iKeyCode==ESwtKeyKeypadEqual)
            || (aData.iKeyCode==ESwtKeyKeypadCR)
            || (aData.iKeyCode==ESwtKeyKeypadMultiply)
            || (aData.iKeyCode==ESwtKeyKeypadAdd))
    {
        modifier |= EModifierKeypad;
    }

    return modifier;
}


#ifdef RD_INTELLIGENT_TEXT_INPUT
/**
 * If RD_INTELLIGENT_TEXT_INPUT is defined, use PtiEngine to remap keyCode
 * of given TKeyEvent. This is needed on some QWERTY keyboard layouts to
 * allow some key combination with Function (Fn) key.
 */
TBool CSwtKeyMapper::RemapKeyCodeByPtiEngineL(
    TKeyEvent& aKeyEvent,
    TEventCode aType,
    TBool& aIsModifierHandledAsAKey)
{
    TInt qwertyMode = 0;
    aIsModifierHandledAsAKey = EFalse;
    RProperty::Get(KCRUidAvkon, KAknQwertyInputModeActive, qwertyMode);
    if (!qwertyMode)
    {
        return EFalse;
    }
    TInt err = KErrNone;
    TBool isStickyKey = iSwtStickyKeysHandler->HandleStickyKeyL(aKeyEvent, aType);
    TUint modifiers = iSwtStickyKeysHandler->StickyModifiers();
    if (!isStickyKey && modifiers == 0)
    {
        // not a ticky key, and no sticky key was active
        modifiers = aKeyEvent.iModifiers;
    }
    if (!isStickyKey && aType == EEventKeyUp)
    {
        // Since key is not sticky key, it is neccessary to reset internal state
        // of the sticky key handler
        iSwtStickyKeysHandler->Reset();
    }

    if (!iPtiEngine)
    {
        TRAP(err, iPtiEngine = CPtiEngine::NewL(ETrue));
        if (err != KErrNone)
        {
            return EFalse;
        }
    }

    if (!iRepository)
    {
        TRAP(err, iRepository = CRepository::NewL(KCRUidAknFep));
        if (err != KErrNone)
        {
            if (iPtiEngine)
            {
                delete iPtiEngine;
            }
            return EFalse;
        }
    }

    TInt inputLang = 0;
    // Read input language from central repository
    if (iRepository)
    {
        TInt ret = iRepository->Get(KAknFepInputTxtLang, inputLang);
    }

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

    TRAP(err, iPtiEngine->ActivateLanguageL(inputLang, mode));
    if (KErrNone != err)
    {
        return EFalse;
    }

    TPtiTextCase textCase = EPtiCaseLower;

    // It is important to keep these ifs in order, since
    // it defines priority of modifiers
    if (modifiers & EModifierShift)
    {
        textCase = EPtiCaseUpper;
    }
    if (modifiers & EModifierRightFunc)
    {
        textCase = EPtiCaseFnLower;
        // For halfqwerty devices we need special handling when
        // Fn is active and Chr is pressed or when shift is pressed
        if (IsHalfQwertyKeyboard() &&
                (EStdKeyLeftFunc == aKeyEvent.iScanCode ||
                 EStdKeyLeftShift == aKeyEvent.iScanCode ||
                 EStdKeyRightShift == aKeyEvent.iScanCode))
        {
            aIsModifierHandledAsAKey = ETrue;
        }
    }
    else if (modifiers & EModifierLeftFunc)
    {
        textCase = EPtiCaseChrLower;
    }

    //If Control key is holded, don't use PtiEngine to map aKeyEvent.iCode
    if (modifiers & EModifierLeftCtrl || modifiers & EModifierRightCtrl)
    {
        iSwtStickyKeysHandler->Reset();
        return EFalse;
    }

    TBuf<KPTIEngineResultSize> mapData;
    iPtiEngine->MappingDataForKey((TPtiKey)aKeyEvent.iScanCode,
                                  mapData, textCase);

    TBool wasMapped = EFalse;
    if (mapData.Length() > 0)
    {
        aKeyEvent.iCode = mapData[0];
        wasMapped = ETrue;
    }

    return wasMapped;
}

TBool CSwtKeyMapper::IsHalfQwertyKeyboard()
{
    TBool halfQwerty = EFalse;
    TInt qwertyMode = 0;
    // There is keycode to be returned, so handle qwerty
    RProperty::Get(KCRUidAvkon, KAknQwertyInputModeActive, qwertyMode);
    if (qwertyMode)
    {
        // Read keyboard layout
        TInt keyboardLayout = 0;
        RProperty::Get(KCRUidAvkon, KAknKeyBoardLayout, keyboardLayout);
        halfQwerty = EPtiKeyboardHalfQwerty == keyboardLayout;
    }
    return halfQwerty;
}

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
void CSwtKeyMapper::CallToJavaPtiVariationL(TInt aType)
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
