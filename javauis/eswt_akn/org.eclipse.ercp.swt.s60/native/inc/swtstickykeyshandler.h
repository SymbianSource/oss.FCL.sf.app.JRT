/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifdef RD_INTELLIGENT_TEXT_INPUT
#ifndef SWTSTICKYKEYSHANDLER_H
#define SWTSTICKYKEYSHANDLER_H

#include <e32def.h>
#include <w32std.h>

enum TStickyKeyStatus
{
    EKeyNotPressed,
    EKeyWasPressed,
    EKeyWasReleased,
};

enum TLockableKeyStatus
{
    EKeyNotActive,
    EKeyActive,
    EKeyNotActiveNext,
    EKeyLocked,
};

NONSHARABLE_CLASS(CSwtStickyKeysHandler)
        : public CBase
{
public:
    static CSwtStickyKeysHandler* NewL();
    TBool HandleStickyKeyL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TInt StickyModifiers() const;
    void Reset();
    ~CSwtStickyKeysHandler();

private:
    void ConstructL();
    TBool IsStickyKey(const TKeyEvent& aKeyEvent);
    TUint ModifierFromScanCode(TInt aScanCode);
    void HandleLockableKeys(TUint aModifiers);
    void HandleFnKey();
    void HandleShiftKey();
    void AddShiftToStickyModifiers();
    void AddFnToStickyModifiers();
    void ResetState();
    TBool IsFnModifierOn(const TUint aModifiers) const;
    TBool IsShiftModifierOn(const TUint aModifiers) const;

private:
    TStickyKeyStatus iStickyKeyState;
    TLockableKeyStatus iFnKeyState;
    TLockableKeyStatus iShiftKeyState;
    TInt iLastStickyScanCode;
    TUint iStickyModifier;
    TUint iLastKeyModifier;
};
#endif //C_CSWTSTICKYKEYSHANDLER_H
#endif // SWTSTICKYKEYSHANDLER_H

