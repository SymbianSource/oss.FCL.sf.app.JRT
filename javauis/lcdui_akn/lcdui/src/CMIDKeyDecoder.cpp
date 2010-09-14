/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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

// CEikonEnv API used for retrieving resource (resId) in GetKeyName function
#include <eikenv.h>
// macros for resources
#include <lcdui.rsg>
// FeatureManager
#include <featmgr.h>
#include <bldvariant.hrh>

#include <j2me/jdebug.h>
/**
@page Key Mapping.
*/

// used for retrieving CR repository codes
#include <centralrepository.h>
// S60 LCDUI Plugin internal keys
#include "S60LCDUIInternalCRKeys.h"

#include "CMIDKeyDecoder.h"
// used for getting instance of ourselves from TlsStruct
#include "CMIDMenuHandler.h"
// using API for iRemConObserver initialization
#include "CMIDRemConObserver.h"
// using constants from enumeration TMediaKey (in InitSpecialKeysL)
#include "MMIDMediaKeysListener.h"
#include "CMIDDisplayable.h"
#include "CMIDUIManager.h"

/**
 * MIDP key codes for keys that have no corresponding
 * Unicode character, as from MIDP specs negative values
 * must be used.
 *
 * ALL characters with code >= ENonCharacterKeyBase
 * should be mapped to -ve numbers (see e32keys.h)
 */
const TInt KMIDKeyUpArrow                       = -1;
const TInt KMIDKeyDownArrow                     = -2;
const TInt KMIDKeyLeftArrow                     = -3;
const TInt KMIDKeyRightArrow                    = -4;
const TInt KMIDKeyClear                         = -8;  // Clear key
const TInt KMIDKeySelect                        = -5;  // Selection key
const TInt KMIDKeyEdit                          = -50; // Edit Key
const TInt KMIDKeySend                          = -10; // Send Key
const TInt KMIDKeyCBA1                          = -6;  // Left CBA
const TInt KMIDKeyCBA2                          = -7;  // Right CBA
const TInt KMIDKeyEnd                           = -11; // End key
const TInt KMIDKeyApplications                  = -12; // Applications key
const TInt KMIDKeyVoiceKey                      = -13; // Voice key
//const TInt KMIDKeyClockWiseRotate               = -16; // not used at the moment
//const TInt KMIDKeyCounterClockWiseRotate        = -17; // not used at the moment
//const TInt KMIDKeyClockWiseFastRotate           = -18; // not used at the moment
//const TInt KMIDKeyCounterClockWiseFastRotate    = -19; // not used at the moment
const TInt KMIDKeyZoomIn                        = -EKeyZoomIn;  // Negative value (-63582)
const TInt KMIDKeyZoomOut                       = -EKeyZoomOut; // Negative value (-63583)

/** The number of game actions */
const TInt KNumGameActions = 9;

/** The max number of scan codes per game action */
const TInt KMaxScanCodesPerAction = 16;

/** The MIDP game action codes, this array must have the same index as
    KDefaultActionScanCodes and KActionKeys.*/
const TInt KActionCodes[KNumGameActions] =
{
    MMIDUtils::EActionUp,
    MMIDUtils::EActionDown,
    MMIDUtils::EActionLeft,
    MMIDUtils::EActionRight,
    MMIDUtils::EActionFire,

    MMIDUtils::EActionGameA,
    MMIDUtils::EActionGameB,
    MMIDUtils::EActionGameC,
    MMIDUtils::EActionGameD
};


/** The MIDP game action keys in the central repository.
These are used to override the default scan codes.
This array must have the same index as KActionCodes.
*/
const TUint32 KActionKeys[KNumGameActions] =
{
    KScanCodeUp,
    KScanCodeDown,
    KScanCodeLeft,
    KScanCodeRight,
    KScanCodeFire,
    KScanCodeGameA,
    KScanCodeGameB,
    KScanCodeGameC,
    KScanCodeGameD
};


/** Default scan codes for game actions. These are only used when
    the scan codes in the central repository have not been set.

    Because for each action there can be one or more code, use a zero
    to indicate that there are no more codes.

    This array must have the same index as KActionCodes.

    @see CR keys, ConstructL()
 */

// ITU-T keypad with standard grid layout.
const TInt KDefaultActionScanCodes[] =
{
    EStdKeyUpArrow, '2', 0,              // EActionUp
    EStdKeyDownArrow, '8', 0,            // EActionDown
    EStdKeyLeftArrow, '4', 0,            // EActionLeft
    EStdKeyRightArrow, '6', 0,           // EActionRight
    EStdKeyDevice3, EStdKeyYes, '5', 0,  // EActionFire

    '7', 0,                             // EActionGameA
    '9', 0,                             // EActionGameB
    EStdKeyNkpAsterisk, '*', 0,         // EActionGameC
    EStdKeyHash, 0                      // EActionGameD
};

// Full QWERTY keyboard with screen in the middle (Table 14 in internal specs)
const TInt KDefaultQwertyActionScanCodes[] =
{
    EStdKeyUpArrow, 'R', 'P', 0,                   // EActionUp
    EStdKeyDownArrow, 'V', EStdKeyForwardSlash, 0, // EActionDown
    EStdKeyLeftArrow, 'D', 'L', 0,                 // EActionLeft
    EStdKeyRightArrow, 'G', EStdKeySingleQuote, 0, // EActionRight
    'A', 'J', EStdKeyDevice3, 0,                   // EActionFire

    EStdKeyHash, 'H', 0,                     // EActionGameA
    'S', 'K', 0,                             // EActionGameB
    'Q', 'U', 0,                             // EActionGameC
    'Z', 'M', 0                              // EActionGameD
};



/** Usual symbian two phase construction. @see ConstructL() */
CMIDKeyDecoder* CMIDKeyDecoder::NewL(MMIDEnv& aEnv)
{
    CMIDKeyDecoder* self = new(ELeave) CMIDKeyDecoder(aEnv);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/** */
CMIDKeyDecoder::~CMIDKeyDecoder()
{
    DEBUG("< CMIDKeyDecoder::~CMIDKeyDecoder");
    iSpecialKeys.Close();
    iGameActions.ResetAndDestroy();

    delete iCaptureKeys;
    delete iKeyTranslator;

    delete iQwertyWatch;

    if (iUseCRScanCodes)
    {
        delete iActionScanCodes;
    }

    FeatureManager::UnInitializeLib();

    delete iRemConObserver;

    DEBUG("> CMIDKeyDecoder::~CMIDKeyDecoder");
}

/** */
CMIDKeyDecoder::CMIDKeyDecoder(MMIDEnv& aEnv)
    : iEnv(aEnv)
    ,iMediaKeysEnabled(EFalse)
{
    DEBUG("< CMIDKeyDecoder::CMIDKeyDecoder");
    DEBUG("> CMIDKeyDecoder::CMIDKeyDecoder");
}

/**
Creates the key translator and another accessory, the capture keys
(needed by the key translator). The key translator can convert scan codes
into key codes. Calls CreateGameActionsL(), which will fill iGameActions:
for each action a set of key codes will be stored. These are MIDP key codes.
*/
void CMIDKeyDecoder::ConstructL()
{
    DEBUG("< CMIDKeyDecoder::ConstructL");

    iCaptureKeys = new(ELeave) CCaptureKeys;
    iCaptureKeys->Construct();

    iKeyTranslator = CKeyTranslator::New();

    // If Select key mapping is defined in central repository an extra MIDP
    // keycode will be used for Select key.
    // Not all the devices have rocker in their keyboard,
    // so other key can be defined instead.
    CRepository* repository = NULL;
    TInt err = KErrNone;
    TRAP(err, repository = CRepository::NewL(KCRUidMidpLcdui));
    // 2 bytes in scan code
    TBuf8<2> scanCodeBuffer;

    if (err == KErrNone)
    {
        CleanupStack::PushL(repository);
        err = repository->Get(KAdditionalSelectKeyMapping,scanCodeBuffer);
        CleanupStack::PopAndDestroy(repository);
    }
    if (err == KErrNone)
    {
        TUint8 scanCodeLeft = scanCodeBuffer[0];
        TUint8 scanCodeRight = scanCodeBuffer[1];
        TUint scanCode = (scanCodeLeft << 8) + scanCodeRight;
        iAdditionalSelectkeyMapping = scanCode;
    }
    else
    {
        // Default value is set in error situations.
        iAdditionalSelectkeyMapping = 0;
    }

    CreateQwertyWatchL();
    CreateGameActionsL();

    FeatureManager::InitializeLibL();

    if (iEnv.MidletAttributeContainsVal(
                LcduiMidletAttributes::KAttribUIEnhancement,
                LcduiMidletAttributeValues::KUIEnhMediaKeys))
    {
        InitRemConObserverL();
        iMediaKeysEnabled = ETrue;
    }

    DEBUG("> CMIDKeyDecoder::ConstructL");
}

/**
Special keys that require separate handling: these are
either non unicode keys or non standard unicode keys.
For standard unicode keys, the MIDP key code is the
unicode charactler. */

void CMIDKeyDecoder::InitSpecialKeysL()
{
    iSpecialKeys.Reset();

    // Keys shared by ITU-T and QWERTY modes
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyLeftArrow,   KMIDKeyLeftArrow,  R_MIDP_KEY_LEFT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyRightArrow,  KMIDKeyRightArrow, R_MIDP_KEY_RIGHT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyUpArrow,     KMIDKeyUpArrow,    R_MIDP_KEY_UP)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDownArrow,   KMIDKeyDownArrow,  R_MIDP_KEY_DOWN)));
    // If Select key mapping is defined in central repository an extra MIDP keycode will be used for Select key.
    if (iAdditionalSelectkeyMapping != 0)
    {
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(iAdditionalSelectkeyMapping, KMIDKeySelect, R_MIDP_KEY_SELECT)));
    }
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDevice3,     KMIDKeySelect,       R_MIDP_KEY_SELECT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyRightShift,  KMIDKeyEdit,         R_MIDP_KEY_EDIT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyLeftShift,   KMIDKeyEdit,         R_MIDP_KEY_EDIT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyF21,         KMIDKeyEdit,         R_MIDP_KEY_EDIT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyLeftFunc,    KMIDKeyEdit,         R_MIDP_KEY_EDIT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyRightFunc,   KMIDKeyEdit,         R_MIDP_KEY_EDIT)));

    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyYes,         KMIDKeySend,         R_MIDP_KEY_SEND)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDevice0,     KMIDKeyCBA1,         R_MIDP_KEY_LSK)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDevice1,     KMIDKeyCBA2,         R_MIDP_KEY_RSK)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNo,          KMIDKeyEnd,          R_MIDP_KEY_END)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyApplication0,KMIDKeyApplications, R_MIDP_KEY_APPS)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDevice6,     KMIDKeyVoiceKey,     R_MIDP_KEY_VOICE)));
    // QWERTY keys for qwerty  and ITU-T mode with qwerty wireless keyboard.
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeySpace,EKeySpace, R_MIDP_KEY_SPACE)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyEscape,EKeyEscape, R_MIDP_KEY_ESC)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyTab,EKeyTab, R_MIDP_KEY_TAB)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyEnter, EKeyLineFeed, R_MIDP_KEY_ENTER)));
    // Diagonal keys for 9-way Navigation Support
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDevice10, EKeyLeftUpArrow, R_MIDP_KEY_LEFT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDevice11,EKeyRightUpArrow, R_MIDP_KEY_RIGHT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDevice12,EKeyRightDownArrow, R_MIDP_KEY_RIGHT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDevice13,EKeyLeftDownArrow, R_MIDP_KEY_LEFT)));
    User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyDelete,EKeyDelete, R_MIDP_KEY_DELETE)));
    if (!iQwertyModeActive)
    {
        // ITU-T keys with different behaviour
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyBackspace, KMIDKeyClear, R_MIDP_KEY_CLEAR)));
    }
    else
    {
        // QWERTY  keys with different behaviour
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyBackspace, EKeyBackspace, R_MIDP_KEY_BACKSPACE)));
    }

    /*
        // Keys defined in specs but not yet present in Series 60
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNull,EKeyFormFeed, R_MIDP_KEY_FORM_FEED)));
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNull,EKeyLineFeed, R_MIDP_KEY_LINE_FEED)));
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNull,EKeyBell, R_MIDP_KEY_BELL)));
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNull,EKeyVerticalTab, R_MIDP_KEY_VERTICAL_TAB)));

        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNull,KMIDKeyClockWiseRotate, R_MIDP_KEY_CLOCKWISE_ROTATE)));
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNull,KMIDKeyCounterClockWiseRotate, R_MIDP_KEY_COUNTER_CLOCKWISE_ROTATE)));
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNull,KMIDKeyClockWiseFastRotate, R_MIDP_KEY_CLOCKWISE_FAST_ROTATE)));
        User::LeaveIfError(iSpecialKeys.Append(TMIDKey(EStdKeyNull,KMIDKeyCounterClockWiseFastRotate, R_MIDP_KEY_COUNTER_CLOCKWISE_FAST_ROTATE)));
    */

    // Add media keys. These are used only in GetKeyName method.
    // EStdKeyNull used as a scancode as media keys don't have a real scancode.
    User::LeaveIfError(iSpecialKeys.Append(
                           TMIDKey(EStdKeyNull, MMIDMediaKeysListener::EMIDMediaKeyPlay,     R_MIDP_KEY_PLAY)));
    User::LeaveIfError(iSpecialKeys.Append(
                           TMIDKey(EStdKeyNull, MMIDMediaKeysListener::EMIDMediaKeyPrevious, R_MIDP_KEY_PREVIOUS)));
    User::LeaveIfError(iSpecialKeys.Append(
                           TMIDKey(EStdKeyNull, MMIDMediaKeysListener::EMIDMediaKeyNext,     R_MIDP_KEY_NEXT)));
    User::LeaveIfError(iSpecialKeys.Append(
                           TMIDKey(EStdKeyNull, MMIDMediaKeysListener::EMIDMediaKeyStop,     R_MIDP_KEY_STOP)));

    // Zoom in and zoom out keys events support
    User::LeaveIfError(iSpecialKeys.Append(
                           TMIDKey(EStdKeyApplicationC, KMIDKeyZoomIn, R_MIDP_KEY_ZOOM_IN)));
    User::LeaveIfError(iSpecialKeys.Append(
                           TMIDKey(EStdKeyApplicationD, KMIDKeyZoomOut, R_MIDP_KEY_ZOOM_OUT)));
};



/**  */
void CMIDKeyDecoder::CreateQwertyWatchL()
{
    iQwertyWatch = CMIDQwertyWatch::NewL(this);
}


void CMIDKeyDecoder::InitRemConObserverL()
{
    if (!iRemConObserver)
    {
        iRemConObserver = CMIDRemConObserver::NewL();
    }
}

/** */
void CMIDKeyDecoder::NewQwertyValueL(TInt aNewValue)
{
    TBool oldQwertyMode = iQwertyModeActive;

    if (aNewValue == 1)
    {
        iQwertyModeActive = ETrue;
    }
    else
    {
        iQwertyModeActive = EFalse;
    }

    if (iQwertyModeActive != oldQwertyMode)
    {
        CreateGameActionsL();
    }
}

/**
 * Gets a key code that corresponds to the specified game action on the device.
 * The implementation is required to provide a mapping for every game action,
 * so this method will always return a valid key code for every game action.
 *
 * Note that a key code is associated with at most one game action, whereas a
 * game action may be associated with several key codes.
 *
 * Returns zero if aGameAction is not a valid game action
 */
TInt CMIDKeyDecoder::GetKeyCode(TInt aGameAction)
{
    DEBUG_INT("< CMIDKeyDecoder::GetKeyCode - game action is %D", aGameAction);
    for (TInt i = 0; i < KNumGameActions; ++i)
    {
        if (iGameActions[i]->iActionCode == aGameAction)
        {
            DEBUG_INT("> CMIDKeyDecoder::GetKeyCode - return code %D", iGameActions[i]->iCodes[0]);
            return iGameActions[i]->iCodes[0]; //just return the first code
        }
    }

    DEBUG_INT("> CMIDKeyDecoder::GetKeyCode - return code %D", 0);
    return 0;
}

/**
 * Maps EPOC scan code to negative key code required by MIDP spec.
 * For keys that have no corresponding Unicode character,
 * the implementation must use negative values.
 */
TInt CMIDKeyDecoder::MapNonUnicodeKey(TUint aScanCode)
{
    DEBUG_INT("< CMIDKeyDecoder::MapNonUnicodeKey - scan code is %D", aScanCode);
    TInt numSpecialKeys = iSpecialKeys.Count();
    for (TInt i = 0; i < numSpecialKeys; i++)
    {
        if (iSpecialKeys[i].iScanCode == aScanCode)
        {
            DEBUG_INT("> CMIDKeyDecoder::MapNonUnicodeKey - return code is %D", iSpecialKeys[i].iMIDPCode);
            return iSpecialKeys[i].iMIDPCode;
        }
    }

    DEBUG_INT("> CMIDKeyDecoder::MapNonUnicodeKey - return code is %D", 0);
    return 0;
}

/**
 * Gets an informative key string for a key. The string returned will resemble the text
 * physically printed on the key. This string is suitable for displaying to the user.
 * For example, on a device with function keys F1 through F4, calling this method on
 * the keyCode for the F1 key will return the string "F1". A typical use for this string
 * will be to compose help text such as "Press F1 to proceed."
 *
 * This method will return a non-empty string for every valid key code.
 *
 */
void CMIDKeyDecoder::GetKeyName(TDes& aText,TInt aKeyCode)
{
    DEBUG_INT("< CMIDKeyDecoder::GetKeyName - key code is %D", aKeyCode);
    aText.Zero();

    TInt resId = R_MIDP_KEY_NON_CHARACTER;
    TInt numSpecialKeys = iSpecialKeys.Count();
    for (TInt i = 0; i < numSpecialKeys; i++)
    {
        if (iSpecialKeys[i].iMIDPCode == aKeyCode)
        {
            resId = iSpecialKeys[i].iNameResId;
            break;
        }
    }

    if ((resId == R_MIDP_KEY_NON_CHARACTER) && (IsUnicode(aKeyCode)))
    {
        aText.Append(TChar(aKeyCode));
    }
    else
    {
        CEikonEnv::Static()->ReadResource(aText,resId);
    }

    DEBUG_STR("> CMIDKeyDecoder::GetKeyName - key name is %S", aText);
}


TInt CMIDKeyDecoder::GameAction(TInt aKeyCode)
{
    DEBUG_INT("< CMIDKeyDecoder::GameAction - key code is %D", aKeyCode);

    for (TInt i = 0; i < KNumGameActions; ++i)
    {
        TInt numCodes = iGameActions[i]->iCodes.Count();
        for (TInt j = 0; j < numCodes; ++j)
        {
            if (iGameActions[i]->iCodes[j] == aKeyCode)
            {
                DEBUG_INT("> CMIDKeyDecoder::GameAction -game action is %D", iGameActions[i]->iActionCode);
                return iGameActions[i]->iActionCode;
            }
        }
    }

    DEBUG_INT("> CMIDKeyDecoder::GameAction -game action is %D", 0);
    return 0; //invalid action
}

/**
 * Determines if a key code should be sent to MIDP applications.
 */
TBool CMIDKeyDecoder::IsJavaKey(TInt aScanCode)
{
    TBool ret = (ScanToMidpCode(aScanCode) != 0);

    DEBUG_INT2("<> CMIDKeyDecoder::IsJavaKey - return %D for scan code %D", ret, aScanCode);
    return ret;
}

TBool CMIDKeyDecoder::MediaKeysEnabled() const
{
    return iMediaKeysEnabled;
}

CMIDRemConObserver* CMIDKeyDecoder::GetRemConObserver()
{
    return iRemConObserver;
}

/** Create the game actions. This method is called at startup and
each time the qwerty availability changes. Because special keys
also depend on qwerty availability, call InitSpecialKeysL() -
which will reset special keys. The reset the actions and call
LoadGameActionCodesL() - which loads the correct codes. Then
for each action appens the codes stored in iActionScanCodes.
Action codes in this array are separated by a null byte. */
void CMIDKeyDecoder::CreateGameActionsL()
{
    DEBUG("< CMIDKeyDecoder::CreateGameActionsL");

    InitSpecialKeysL();
    iGameActions.ResetAndDestroy();

    LoadGameActionCodesL();
    ASSERT(iActionScanCodes != NULL);

    TInt index = 0;
    for (TInt i = 0; i < KNumGameActions; ++i)
    {
        TMIDGameAction* action = new(ELeave) TMIDGameAction(KActionCodes[i]);
        CleanupStack::PushL(action);

        while (iActionScanCodes[index] != 0)
        {
            AddCodeToActionL(action, iActionScanCodes[index]);
            index++;
        }
        index++;    // skip null byte separating actions

        User::LeaveIfError(iGameActions.Append(action));
        CleanupStack::Pop(action);
    }

    DEBUG("> CMIDKeyDecoder::CreateGameActionsL");
}

/** Load the game action codes: either the default ITU-T codes
    if no qwerty input is available or load the codes
    from Central Repository (if available and if
    flag in CR indicates to do so) or load some default QWERTY codes.
*/
void CMIDKeyDecoder::LoadGameActionCodesL()
{
    DEBUG("< CMIDKeyDecoder::LoadGameActionCodesL");

    if (iUseCRScanCodes)
    {
        delete iActionScanCodes;
        iUseCRScanCodes = EFalse;
    }

    iActionScanCodes = NULL;

    if (!QwertyInputAvailable())
    {
        // if no qwerty always use default ITU-T codes
        DEBUG("  CMIDKeyDecoder::LoadGameActionCodesL - use default ITU-T scan codes");
        iActionScanCodes = (TInt*)&KDefaultActionScanCodes[0];
    }
    else
    {
        // if there is qwerty either use CR repository codes or default qwerty codes
        CRepository* repository = NULL;
        TRAPD(crExists, repository = CRepository::NewL(KCRUidMidpLcdui));

        if (crExists == KErrNone)
        {
            CleanupStack::PushL(repository);
            repository->Get(KUseCustomGameActionScanCodes, iUseCRScanCodes);
        }

        if (iUseCRScanCodes)
        {
            // use CR QWERTY codes
            DEBUG("  CMIDKeyDecoder::LoadGameActionCodesL - use central repository scan codes");
            LoadCentralRepositoryCodesL(repository);
        }
        else
        {
            //  use default QWERTY codes
            DEBUG("  CMIDKeyDecoder::LoadGameActionCodesL - add default qwerty scan codes");
            iActionScanCodes = (TInt*)&KDefaultQwertyActionScanCodes[0];
        }

        if (crExists == KErrNone)
        {
            CleanupStack::PopAndDestroy(repository);
        }
    }

    DEBUG("> CMIDKeyDecoder::LoadGameActionCodesL");
}

/** Load the game action scan codes stored in the central repository
    database. Two bytes are available per scan code, LSB is left. Max
    number of codes per action is KMaxScanCodesPerAction. Allocate
    some memory for iActionScanCodes.
    */
void CMIDKeyDecoder::LoadCentralRepositoryCodesL(CRepository* aRepository)
{
    DEBUG("< CMIDKeyDecoder::LoadCentralRepositoryCodesL");

    ASSERT(aRepository != NULL);
    ASSERT(iActionScanCodes == NULL);

    iActionScanCodes = new(ELeave) TInt[KNumGameActions*KMaxScanCodesPerAction];

    TInt index = 0;
    for (TInt i = 0; i < KNumGameActions; ++i)
    {
        //2 bytes per scan code
        TBuf8<KMaxScanCodesPerAction*2> scanCodeBuffer;
        aRepository->Get(KActionKeys[i],scanCodeBuffer);

        for (TInt j = 0; j < (scanCodeBuffer.Length() - 1); j++)
        {
            TUint8 scanCodeLeft = scanCodeBuffer[j++];
            TUint8 scanCodeRight = scanCodeBuffer[j];

            TUint scanCode = (scanCodeLeft << 8) + scanCodeRight;

            if (scanCode != 0)
            {
                iActionScanCodes[index++] = scanCode;
            }
        }

        iActionScanCodes[index++] = 0;   //indicates end of scan codes
    }

    DEBUG("> CMIDKeyDecoder::LoadCentralRepositoryCodesL");
}

/** Return true if this device supports qwerty and the qwerty keyboard
    is available */
TBool CMIDKeyDecoder::QwertyInputAvailable() const
{
    return iQwertyModeActive;
}

/**
    Convert the scan code into a MIDP key code taking into account possible
    modifiers. Add the resulting MIDP key codes to the action specified as long
    as they are all different.
  */
void CMIDKeyDecoder::AddCodeToActionL(TMIDGameAction* aGameAction, TInt aScanCode)
{
    DEBUG_INT("< CMIDKeyDecoder::AddCodeToActionL - scan code is %D", aScanCode);
    TInt midpCode = ScanToMidpCode(aScanCode);

    if (aGameAction->iCodes.Find(midpCode) == KErrNotFound && midpCode != 0)
    {
        DEBUG_INT("  CMIDKeyDecoder::AddCodeToActionL - adding midp code %D", midpCode);
        User::LeaveIfError(aGameAction->iCodes.Append(midpCode));
    }

    if (IsUnicode(midpCode))
    {
        midpCode = ScanToMidpCode(aScanCode | EModifierShift);

        if (aGameAction->iCodes.Find(midpCode) == KErrNotFound)
        {
            DEBUG_INT("  CMIDKeyDecoder::AddCodeToActionL - adding midp code %D", midpCode);
            User::LeaveIfError(aGameAction->iCodes.Append(midpCode));
        }
    }

    DEBUG("> CMIDKeyDecoder::AddCodeToActionL");
}

/** Take a scan code and convert it into a MIDP code. First see if it
is one of the special keys, otherwise ask the window server to convert it. */
TInt CMIDKeyDecoder::ScanToMidpCode(TInt aScanCode)
{
    DEBUG_INT("< CMIDKeyDecoder::ScanToMidpCode - scan Code is %D", aScanCode);
    TInt keyCode = MapNonUnicodeKey(aScanCode);

    if (keyCode == 0)
    {
        // fake a key down to convert scan code into key code
        // Note: on WINS (and WINSCW) this conversion works only for QWERTY layout
        // unless you specify the char code in the HI part of the scan code - which
        // we cannot do - hence on the emulator only QWERTY layout works.
        TKeyData keyData;
        TBool translated = iKeyTranslator->TranslateKey(aScanCode,EFalse,*iCaptureKeys,keyData);

        if (translated)
        {
            DEBUG_INT("  CMIDKeyDecoder::ScanToMidpCode - ws translated code is %D", keyData.iKeyCode);
            keyCode = IsUnicode(keyData.iKeyCode) ? keyData.iKeyCode : 0;
        }

        // fake a key up to restore modifiers
        iKeyTranslator->TranslateKey(aScanCode,ETrue,*iCaptureKeys,keyData);
    }

    DEBUG_INT("> CMIDKeyDecoder::ScanToMidpCode - return key code %D", keyCode);
    return keyCode;
}

/** Return true if the key code corresponds to a unicode character */
TBool CMIDKeyDecoder::IsUnicode(TInt aKeyCode) const
{
    TBool ret = (aKeyCode > 0) && (aKeyCode < ENonCharacterKeyBase ||
                                   aKeyCode >= (ENonCharacterKeyBase + ENonCharacterKeyCount));

    DEBUG_INT2("<> CMIDKeyDecoder::IsUnicode - return %D for key code %D", ret, aKeyCode);
    return ret;
}

TInt CMIDKeyDecoder::AdditionalSelectionKeyMappingCode()
{
    return iAdditionalSelectkeyMapping;
}
