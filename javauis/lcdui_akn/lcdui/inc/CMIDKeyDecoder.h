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


#ifndef CMIDKEYDECODER_H
#define CMIDKEYDECODER_H

#include <e32base.h>
#include <lcdui.h>

// CMIDQwertyWatch* iQwertyWatch
// monitoring status of qwerty input
#include "CMIDQwertyWatch.h"

// FORWARD DECLARATION
class CMIDDisplayable;
class CMIDMenuHandler;
class CMIDUIManager;
class CMIDRemConObserver;
class CRepository;

/** A private class containing key information,
such as name, key code and so forth. */
class TMIDKey
{
public:
    TMIDKey(TInt aScanCode, TInt aMIDPCode, TInt aNameResId)
            : iScanCode(aScanCode), iMIDPCode(aMIDPCode), iNameResId(aNameResId)
    { }

public:
    /** The symbian physical scan code*/
    TInt iScanCode;

    /** They key code to be passed MIDP side */
    TInt iMIDPCode;

    /** The resource Id to load the key name from resource file */
    TInt iNameResId;
};

/** A private class containing game action information,
can have one or more keys associated to it -
it must at least have one. */
class TMIDGameAction
{
public:
    TMIDGameAction(TInt aActionCode) : iActionCode(aActionCode)
    { }

    /** The action enumeration code as defined in MMIDUtils */
    TInt iActionCode;

    /** The MIDP keys codes associated with this game action */
    RArray<TInt> iCodes;
};

/** Decodes scan codes and symbian
key codes into MIDP key codes and game actions.
It also converts key codes into key names.

This class is created and deleted by the menu hander,
which stores an instance of TlsStruct in the TLS.

@see CMIDMenuHandler, TlsStruct
*/
NONSHARABLE_CLASS(CMIDKeyDecoder): public CBase, public MMIDQwertyObserver
{
public:
    //This is the preferred way to get hold of an instance of this object (singleton)
    static CMIDKeyDecoder* Instance();

public:
    //These are meant to be called only once by the menu handler at startup
    static CMIDKeyDecoder* NewL(MMIDEnv& aEnv);
    ~CMIDKeyDecoder();

public:
    //This is the public API
    TInt GetKeyCode(TInt aGameAction);
    void GetKeyName(TDes& aText,TInt aKeyCode);
    TInt GameAction(TInt aKeyCode);

    TBool IsJavaKey(TInt aScanCode);
    TInt MapNonUnicodeKey(TUint aScanCode);

    /**
     * Checks if media key support is enabled.
     *
     * @return  ETrue, if media key support is enabled.
     * @since S60 3.2
     */
    TBool MediaKeysEnabled() const;

    /**
     * Returns a pointer to the CMIDRemConObserver object.
     *
     * @return  Pointer to CMIDRemConObserver if it exists, or NULL.
     * @since S60 3.2
     */
    CMIDRemConObserver* GetRemConObserver();

    /**
     * Returns scan code for additional selection key mapping
     *
     * @return scan code for additional selection key mapping
     * @since S60 5.0
     */
    TInt AdditionalSelectionKeyMappingCode();

private:
    //Private two phase construction and destruction
    CMIDKeyDecoder(MMIDEnv& aEnv);
    void ConstructL();

    /** Initialisation of special key translation table, see iSpecialKeys */
    void InitSpecialKeysL();

    /** Initialisation of iQwertyWatch and iQwertyModeActive */
    void CreateQwertyWatchL();

    /**
     * Creates RemConObserver. Does nothing if RemConObserver already exists.
     *
     * @since S60 3.2
     */
    void InitRemConObserverL();

private:
    // MMIDQwertyObserver
    void NewQwertyValueL(TInt aNewValue);

private:
    //Private helper methods
    void CreateGameActionsL();
    void LoadGameActionCodesL();

    void LoadCentralRepositoryCodesL(CRepository* aRepository);
    TBool QwertyInputAvailable() const;
    void AddCodeToActionL(TMIDGameAction* aGameAction, TInt aScanCode);

    TInt ScanToMidpCode(TInt aScanCode);
    TBool IsUnicode(TInt aKeyCode) const;

private:
    /** For each MIDP Game action contains a TMIDGameAction*/
    RPointerArray<TMIDGameAction> iGameActions;

    /** This is needed by CKeyTranslator::TranslateKey() */
    CCaptureKeys*   iCaptureKeys;

    /** The window server key translator for converting SCAN codes to KEY codes */
    CKeyTranslator* iKeyTranslator;

    /** Special key translation table */
    RArray<TMIDKey> iSpecialKeys;

    /** Monitor status of qwerty input */
    CMIDQwertyWatch* iQwertyWatch;

    /** True when qwerty keyboard is available */
    TBool iQwertyModeActive;

    /** The action scan codes currently in use, either default ITU-T,
    or default QWERTY or from Central Repository */
    TInt* iActionScanCodes;

    /** True when we must use the scan codes from Central Repository */
    TBool iUseCRScanCodes;

    MMIDEnv&                iEnv;

    /** Receives media key events from RemCon */
    CMIDRemConObserver*     iRemConObserver;

    /** ETrue when media key support is enabled */
    TBool                   iMediaKeysEnabled;

    /** Scancode of additional Select key is stored here. Used when mapping MIDP special keys. */
    TUint iAdditionalSelectkeyMapping;

};

#endif // CMIDKEYDECODER_H
