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


#include "CMIDKeyTranslator.h"
#include <jdebug.h>

// Uncomment for loads of debug output
//#define DEBUG_KEY_TRANSLATOR

#ifndef DEBUG_KEY_TRANSLATOR

#ifdef DEBUG_STR
#undef DEBUG_STR
#define DEBUG_STR(msg, str)
#endif // def DEBUG_STR

#ifdef DEBUG_INT
#undef DEBUG_INT
#define DEBUG_INT(msg,val)
#endif // def DEBUG_INT

#ifdef DEBUG_INT2
#undef DEBUG_INT2
#define DEBUG_INT2(msg,val1,val2)
#endif // def DEBUG_INT2

#endif // def DEBUG_KEY_TRANSLATOR

#if defined(__WINS__) // from w32cmd.h
// Under WINS character code is passed in as HIWORD of the scan code,
// and will need to be removed in some situations
#define SCANCODE(x) ((x) & 0xFFFF)
#else
#define SCANCODE(x) (x)
#endif

const TInt KMIDKeyEdit = -50;
const TInt KMIDKeySelect = -5;

inline TBool NonUnicodeKeyCode(TInt aKeyCode)
{
    return (aKeyCode > CMIDKeyTranslator::KMaxUnicodeKeyCode) || (aKeyCode <= 0);
}

//
// Record of translated keys.
//

CMIDKeyTranslator::CMIDKeyTranslator(MMIDEnv& aEnv)
        : iEnv(aEnv)
        , iKeyList(4) // granularity
        , iS60SelectionKeyCompatibility(EFalse)
{
}

void CMIDKeyTranslator::ConstructL()
{

    iS60SelectionKeyCompatibility = iEnv.MidletAttributeIsSetToVal(
                                        LcduiMidletAttributes::KAttribS60SelectionKeyCompatibility,
                                        LcduiMidletAttributeValues::KTrueValue
                                    );

}

CMIDKeyTranslator::~CMIDKeyTranslator()
{
    iKeyList.Reset();
}

void CMIDKeyTranslator::SetUtils(MMIDUtils* aUtils)
{
    iUtils = aUtils;
}

void CMIDKeyTranslator::Reset()
{
    iKeyList.Reset();
}

/**
DEBUG
*/
TText EventCode(TEventCode aType)
{
    switch (aType)
    {
    case EEventKeyDown:
        return L'D';
    case EEventKey:
        return L'C';
    case EEventKeyUp:
        return L'U';
    default:
        return L'?';
    }
}

TBool CMIDKeyTranslator::TranslateL(TMIDKeyEvent& aMIDPKeyEvent, const TKeyEvent& aWsEvent, TEventCode aEventCode)
{
#ifdef DEBUG_KEY_TRANSLATOR
    TBuf<64> buf;
    buf.Format(_L("[%C,%d,%d,%d]"), EventCode(aEventCode), SCANCODE(aWsEvent.iScanCode), aWsEvent.iCode, aWsEvent.iRepeats);
    DEBUG_STR("TKeyEvent: %S", buf);
#endif
    ASSERT(iUtils);

    // check if this key should be sent to java
    if (!iUtils->IsJavaKey(SCANCODE(aWsEvent.iScanCode)))
    {
        DEBUG_INT("Non-Java scancode rejected: %d", SCANCODE(aWsEvent.iScanCode));
        return EFalse;
    }

    aMIDPKeyEvent.iEvents =0;
    aMIDPKeyEvent.iKeyCode=0;
    aMIDPKeyEvent.iRepeats=0;

    // check if key is already pressed
    TInt index = FindKeyRecord(SCANCODE(aWsEvent.iScanCode));
    if (index == KErrNotFound)
    {
        // new key press
        TKeyRecord record;
        record.iScanCode = SCANCODE(aWsEvent.iScanCode);
        record.iIsSpecialKey = EFalse;
        //store the value for some special key (non-unicode)
        TInt mappedCode = Map(record.iScanCode, aWsEvent.iCode);
        if (aWsEvent.iCode &&
                (aWsEvent.iScanCode == EStdKeyLeftShift ||
                 aWsEvent.iScanCode == EStdKeyLeftFunc ||
                 aWsEvent.iScanCode == EStdKeySpace))
        {
            //If iCode has some non-zero value, the key was already mapped
            //via PTIEngine. Key code value is simply copied to record.
            //This step is because in Half-QWERTY keyboard layout, some keys
            //can have additionl characters.
            //E.g. Chr key on half-QWERTY keyboard has '*'.
            record.iKeyCode = aWsEvent.iCode;
            if (mappedCode == KMIDKeyEdit)
            {
                //Some special keys (in different keyborad layouts) have
                //additional key value. So we need to store information that
                //special key was pressed.
                record.iIsSpecialKey = ETrue;
            }
        }
        else
        {
            //If iCode is 0, this means, that special key was pressed
            //(e.g Fn or Shift) and it doesn't have any additional character
            //(so PTIEngine didn't do any mapping)
            record.iKeyCode = mappedCode;
        }
        record.iState = TKeyRecord::EStateInitial;
        User::LeaveIfError(iKeyList.Append(record));
        index = iKeyList.Count()-1;
    }
    TKeyRecord& record = iKeyList[index];

    if (record.iKeyCode == 0 && aEventCode == EEventKey)
    {
        record.iKeyCode = Map(record.iScanCode, aWsEvent.iCode);
    }

    // transition to next state...
    record.Transition(aEventCode, aMIDPKeyEvent, aWsEvent.iRepeats, iS60SelectionKeyCompatibility);

    if (record.iState == TKeyRecord::EStateInitial)
    {
        iKeyList.Remove(index);
    }

    return aMIDPKeyEvent.iEvents;   // non-zero == true post something

}

TInt CMIDKeyTranslator::FindKeyRecord(TInt aScanCode) const
{
    for (TInt ii=iKeyList.Count(); ii--;)
    {
        if (iKeyList[ii].iScanCode==aScanCode)
            return ii;
    }
    return KErrNotFound;
}


TInt CMIDKeyTranslator::Map(TInt aScanCode, TInt aCode)
{
    //
    // Fitler out some non-unicode keys - only rejects obviously
    // out of bounds unicode values. Any keys that should be
    // mapped to Java will be mapped in MapNonUnicodeKeyCode
    // below.
    //
    if (NonUnicodeKeyCode(aCode))
    {
        aCode = KInvalidKeyCode;
    }

    //
    // At this point keyCode can be any valid EPOC key code, some of which
    // will not be UNICODE values. To comply with the MIDP spec, all non
    // UNICODE keys must be mapped to negative values.
    //
    // Any keys for which we do not have a valid MIDP keyCode, will be
    // mapped to KInvalidKeyCode.
    //
    // We do the mapping here regardless of the return value of
    // NonUnicodeKeyCode() as that function may not remove all non-unicode
    // keycodes.
    //
    // The mapping is done from the scancode.
    //
    TInt specialKey = MapNonUnicodeKeyCode(aScanCode);
    if (KInvalidKeyCode != specialKey)
    {
        DEBUG_INT2("mapping non-unicode keycode: %d to %d", aCode, specialKey);
        aCode = specialKey;
    }

    DEBUG_INT("keycode = %d\n", aCode);

    return aCode;
}

TInt CMIDKeyTranslator::MapNonUnicodeKeyCode(TInt aScanCode)
{
    ASSERT(iUtils);
    return iUtils->MapNonUnicodeKey(aScanCode);
}

TBool CMIDKeyTranslator::PostKeyEvent(MMIDComponent& aComponent, const TMIDKeyEvent& aEvent)
{
    ASSERT(aEvent.iKeyCode);
    TBool posted(EFalse);

    if (aEvent.iEvents & TMIDKeyEvent::EPressed)
    {
        PostKeyEvent(aComponent, EKeyPressed, aEvent.iKeyCode, 0);
        posted=ETrue;
    }

    if (aEvent.iEvents & TMIDKeyEvent::ERepeated)
    {
        PostKeyEvent(aComponent, EKeyRepeated, aEvent.iKeyCode, aEvent.iRepeats);
        posted=ETrue;
    }

    if (aEvent.iEvents & TMIDKeyEvent::EReleased)
    {
        PostKeyEvent(aComponent, EKeyReleased, aEvent.iKeyCode, 0);
        posted=ETrue;
    }

    return posted;
}

void CMIDKeyTranslator::PostKeyEvent(MMIDComponent& aComponent, TEventType aType, TInt aKeyCode, TInt aRepeats)
{
    ASSERT(aKeyCode != KInvalidKeyCode);
    DEBUG_INT2("PostKeyEvent[%d] (%d)", aType, aKeyCode);
    (void) iEnv.PostJavaEvent(
        aComponent,
        aComponent.Type() == MMIDComponent::ECustomItem ? EItem : EDisplayable,
        aType,
        aKeyCode,
        aRepeats,
        0
    );
}

void TKeyRecord::Transition(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats, TBool aS60SelectionKeyCompatibility)
{
    // transition to next state...
    switch (iState)
    {
    case EStateInitial:
        StateInitial(aEventCode, aMIDPKeyEvent, aRepeats, aS60SelectionKeyCompatibility);
        break;

    case EStateDown:
        StateDown(aEventCode, aMIDPKeyEvent,aRepeats);
        break;

    case EStateDownTranslated:
        StateDownTranslated(aEventCode, aMIDPKeyEvent,aRepeats);
        break;

    case EStateKeyTranslated:
        StateKeyTranslated(aEventCode, aMIDPKeyEvent,aRepeats);
        break;

    case EStateKeyRepeated:
        StateKeyRepeated(aEventCode, aMIDPKeyEvent,aRepeats);
        break;

    case EStateDownRepeated:
        StateDownRepeated(aEventCode, aMIDPKeyEvent,aRepeats);
        break;
    }
}

void TKeyRecord::StateInitial(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats, TBool aS60SelectionKeyCompatibility)
{
    switch (aEventCode)
    {
    case EEventKeyDown:
        // When canvas is not in full-screen mode then EEventKey event for Edit and
        // Select key-presses is catched in CEikCba::OfferKeyEventL. This function
        // returns EKeyWasConsumed and CCoeControlStack doesn't send EEvent to CMIDCanvas.
        // Therefore EEvent key never reach to CMIDCanvas::OfferKeyEventL.
        // This is why we use here EEventKeyDown for sending the key-press
        // events for Edit and Select to MIDlet.
        //
        // Special keys, like Edit key, can have also additional characters.
        // If special key carries the additional character (mapped by PTIEngine)
        // and it should be sent to Java, iIsSpecialKey is true and record
        // switches to EStateDownTranslated. This will guarantee, that event
        // will be sent to MIDlet.
        if (iKeyCode == KMIDKeyEdit ||
                (iKeyCode == KMIDKeySelect && aS60SelectionKeyCompatibility) ||
                (iIsSpecialKey)
           )
        {
            // have a -ve code, can post KeyPressed now
            KeyPressed(aMIDPKeyEvent);
            iState = EStateDownTranslated;
        }
        else
        {
            // must delay KeyPressed until have keyCode
            iState = EStateDown;
        }
        break;

    case EEventKey:
        // FEP
        KeyPressed(aMIDPKeyEvent);
        if (aRepeats > 0)
        {
            KeyRepeated(aMIDPKeyEvent,aRepeats);
        }
        KeyReleased(aMIDPKeyEvent);
        iState = EStateInitial;
        break;

    case EEventKeyUp:
        iState = EStateInitial;
        break;

    default:
        ASSERT(EFalse);
    }
}

void TKeyRecord::StateDown(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats)
{
    // key code was previously NOT translated.

    switch (aEventCode)
    {
    case EEventKeyDown:
        // no-op
        DEBUG_INT("THIS SCANCODE REQUIRES MAPPING TO NEGATIVE KEYCODE: %d", iScanCode);
        break;

    case EEventKey:
        // now we have key code, can send key pressed
        KeyPressed(aMIDPKeyEvent);
        if (aRepeats > 0)
        {
            KeyRepeated(aMIDPKeyEvent,aRepeats);
        }
        iState = EStateDownTranslated;
        break;

    case EEventKeyUp:
        iState = EStateInitial;
        DEBUG_INT("THIS SCANCODE REQUIRES MAPPING TO NEGATIVE KEYCODE: %d", iScanCode);
        break;

    default:
        ASSERT(EFalse);
    }
}

void TKeyRecord::StateDownTranslated(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats)
{

    switch (aEventCode)
    {
    case EEventKeyDown:
        KeyRepeated(aMIDPKeyEvent,1);
        iState = EStateDownRepeated;
        break;

    case EEventKey:
        if (aRepeats > 0)
        {
            KeyRepeated(aMIDPKeyEvent,aRepeats);
            iState= EStateKeyRepeated;
        }
        else
        {
            iState = EStateKeyTranslated;
        }
        break;

    case EEventKeyUp:
        KeyReleased(aMIDPKeyEvent);
        iState = EStateInitial;
        break;

    default:
        ASSERT(EFalse);
    }
}

void TKeyRecord::StateKeyTranslated(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats)
{
    switch (aEventCode)
    {
    case EEventKeyDown:
        // no-op
        break;
    case EEventKey:
        KeyRepeated(aMIDPKeyEvent, aRepeats ? aRepeats : 1);
        iState=EStateKeyRepeated;
        break;
    case EEventKeyUp:
        KeyReleased(aMIDPKeyEvent);
        iState = EStateInitial;
        break;
    default:
        ASSERT(EFalse);
    }
}

void TKeyRecord::StateKeyRepeated(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats)
{
    switch (aEventCode)
    {
    case EEventKeyDown:
        // no-op
        break;
    case EEventKey:
        KeyRepeated(aMIDPKeyEvent, aRepeats ? aRepeats : 1);
        iState=EStateKeyRepeated;
        break;
    case EEventKeyUp:
        KeyReleased(aMIDPKeyEvent);
        iState = EStateInitial;
        break;
    default:
        ASSERT(EFalse);
    }
}

void TKeyRecord::StateDownRepeated(TEventCode aEventCode, TMIDKeyEvent& aMIDPKeyEvent, TInt /*aRepeats*/)
{
    switch (aEventCode)
    {
    case EEventKeyDown:
        KeyRepeated(aMIDPKeyEvent,1);
        break;
    case EEventKey:
        iState = EStateDownTranslated;
        break;
    case EEventKeyUp:
        KeyReleased(aMIDPKeyEvent);
        iState = EStateInitial;
        break;
    default:
        ASSERT(EFalse);
    }
}


void TKeyRecord::SetKeyCode(TMIDKeyEvent& aMIDPKeyEvent)
{
    ASSERT(iKeyCode);
    if (0 == aMIDPKeyEvent.iKeyCode)
    {
        aMIDPKeyEvent.iKeyCode = iKeyCode;
    }
    ASSERT(aMIDPKeyEvent.iKeyCode == iKeyCode);
}

void TKeyRecord::KeyPressed(TMIDKeyEvent& aMIDPKeyEvent)
{
    SetKeyCode(aMIDPKeyEvent);
    aMIDPKeyEvent.iEvents |= TMIDKeyEvent::EPressed;
    aMIDPKeyEvent.iRepeats = 0;
}

void TKeyRecord::KeyRepeated(TMIDKeyEvent& aMIDPKeyEvent, TInt aRepeats)
{
    SetKeyCode(aMIDPKeyEvent);
    aMIDPKeyEvent.iEvents |=  TMIDKeyEvent::ERepeated;
    aMIDPKeyEvent.iRepeats = aRepeats;
}

void TKeyRecord::KeyReleased(TMIDKeyEvent& aMIDPKeyEvent)
{
    SetKeyCode(aMIDPKeyEvent);
    aMIDPKeyEvent.iEvents |=  TMIDKeyEvent::EReleased;
    aMIDPKeyEvent.iRepeats = 0;
}

