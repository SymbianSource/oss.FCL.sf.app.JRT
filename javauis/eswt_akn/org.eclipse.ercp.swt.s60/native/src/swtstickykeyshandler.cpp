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
#include "swtstickykeyshandler.h"

/**
 * 1st & 2nd phase constructors wrapper
 */
CSwtStickyKeysHandler* CSwtStickyKeysHandler::NewL()
{
    CSwtStickyKeysHandler* self = new(ELeave) CSwtStickyKeysHandler;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/**
 * 2nd phase constructor
 */
void CSwtStickyKeysHandler::ConstructL()
{
    ResetState();
    iFnKeyState = EKeyNotActive;
    iShiftKeyState = EKeyNotActive;
}

/**
 * Destructor
 */
CSwtStickyKeysHandler::~CSwtStickyKeysHandler()
{
}

/**
 * Reset internal state.
 */
void CSwtStickyKeysHandler::ResetState()
{
    iLastStickyScanCode = 0;
    iStickyKeyState = EKeyNotPressed;
}

/**
 * Reset internal state and previously set sticky modifiers. Should be called
 * when non sticky key is pressed after sticky keys.
 */
void CSwtStickyKeysHandler::Reset()
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

/**
 * This method handle sticky keys and creating own modifiers value from all
 * handled sticky keys. It should behave exacly the same as is behaiour in
 * native CEikEdwin editor.
 * @param aKeyEvent     Symbian key event, which contain key scan code.
 * @param aType         TEventCode of current key event.
 * @return EFalse if aKeyEvent isn't sticky key or ETrue if aKeyEvent is sticky key.
 */
TBool CSwtStickyKeysHandler::HandleStickyKeyL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
{
    if (!IsStickyKey(aKeyEvent))
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

/**
 * @return modifier value of sticky keys.
 */
TInt CSwtStickyKeysHandler::StickyModifiers() const
{
    return iStickyModifier;
}

/**
 * @return ETrue if key is sticky key or EFalse if key isn't sticky key.
 */
TBool CSwtStickyKeysHandler::IsStickyKey(const TKeyEvent& aKeyEvent)
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
        break;;
    }

    return isStickyKey;
}

/**
 * @return modifier value from key scan code.
 */
TUint CSwtStickyKeysHandler::ModifierFromScanCode(TInt aScanCode)
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

/**
 * Handle lockable keys, which are currently only Fn and Shift key.
 */
void CSwtStickyKeysHandler::HandleLockableKeys(TUint aModifiers)
{
    if (IsFnModifierOn(aModifiers))
    {
        HandleFnKey();
    }

    if (IsShiftModifierOn(aModifiers))
    {
        HandleShiftKey();
    }
}

/**
 * Handle event with modifier of Fn key
 */
void CSwtStickyKeysHandler::HandleFnKey()
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
void CSwtStickyKeysHandler::HandleShiftKey()
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

void CSwtStickyKeysHandler::AddShiftToStickyModifiers()
{
    iStickyModifier |= EModifierShift | EModifierLeftShift | EModifierRightShift;
}

void CSwtStickyKeysHandler::AddFnToStickyModifiers()
{
    iStickyModifier |= EModifierFunc | EModifierRightFunc;
}

TBool CSwtStickyKeysHandler::IsFnModifierOn(const TUint aModifiers) const
{
    return aModifiers & EModifierFunc || aModifiers & EModifierRightFunc;
}

TBool CSwtStickyKeysHandler::IsShiftModifierOn(const TUint aModifiers) const
{
    return aModifiers & EModifierShift || aModifiers & EModifierLeftShift
           || aModifiers & EModifierRightShift;
}
#endif //RD_INTELLIGENT_TEXT_INPUT

