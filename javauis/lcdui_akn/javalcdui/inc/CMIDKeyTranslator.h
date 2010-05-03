/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef CMIDKEYTRANSLATOR_H
#define CMIDKEYTRANSLATOR_H

#include <coecntrl.h>
#include "lcdui.h"

class CKeyList;

struct TKeyRecord
{
    enum TState
    {
        EStateInitial = 0,
        EStateDown,
        EStateDownTranslated,
        EStateKeyTranslated,
        EStateKeyRepeated,
        EStateDownRepeated,
    };

    TInt iScanCode;
    TInt iKeyCode;
    TInt iState;
    TBool iIsSpecialKey;

    void Transition(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats, TBool aS60SelectionKeyCompatibility);
    void StateInitial(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats, TBool aS60SelectionKeyCompatibility);
    void StateDown(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats);
    void StateDownTranslated(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats);
    void StateKeyTranslated(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats);
    void StateKeyRepeated(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats);
    void StateDownRepeated(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats);
    void KeyPressed(TMIDKeyEvent& aMIDKeyEvent);
    void KeyRepeated(TMIDKeyEvent& aMIDKeyEvent,TInt aRepeats);
    void KeyReleased(TMIDKeyEvent& aMIDKeyEvent);
    void SetKeyCode(TMIDKeyEvent& aMIDPKeyEvent);

};

/**
 * Maps EPOC key events to MIDP key codes.
 *
 *  Translates EEventKeyDown and EEventKeyUp scancodes to
 *  key codes.
 *
 *  Maps non-unicode keys to negative codes.
 *
 *  Stores Game key mappings.
 *
 */
class CMIDKeyTranslator : public CBase
{
public:
    enum { KInvalidKeyCode    = 0  };
    enum { KInvalidGameAction = 0  };
    enum { KMaxGameAction     = 12 };
    enum { KMaxUnicodeKeyCode = 65535 };    // TUint16 max value

public:
    CMIDKeyTranslator(MMIDEnv& aEnv);
    void ConstructL();
    ~CMIDKeyTranslator();

    /**
     * Utils are not available at construction time, so
     * are set later.
     */
    void    SetUtils(MMIDUtils* aUtils);

    /**
     * Map EPOC Key event to MIDP key code.
     *
     *@returns ETrue if key event should be posted to Java.
     */
    TBool   TranslateL(TMIDKeyEvent& aEvent, const TKeyEvent& aWsEvent, TEventCode aType);
    TBool   PostKeyEvent(MMIDComponent& aSource, const TMIDKeyEvent& aEvent);

    /**
     * Forgets all currently tracked keys. Used to release some memory
     * if key events will not be sent up to java. Also resets the
     * game action key state.
     */
    void    Reset();


private:
    TInt FindKeyRecord(TInt aScanCode) const;
    TInt Map(TInt aScanCode, TInt aCode);
    /**
     * Map an invalid (non-unicode) SymbianOS keycode to a valid MIDP unicode or
     * negative keycode.
     *@param  aScanCode SymbianOS scan code.
     *@return MIDP key code
     */
    TInt    MapNonUnicodeKeyCode(TInt aScanCode);
    void    PostKeyEvent(MMIDComponent& aComponent, TEventType aType, TInt aKeyCode, TInt aRepeats);

private:
    MMIDEnv&    iEnv;
    MMIDUtils*  iUtils;
    RArray<TKeyRecord>  iKeyList;

    TBool iS60SelectionKeyCompatibility;

};

#endif // CMIDKEYTRANSLATOR_H
