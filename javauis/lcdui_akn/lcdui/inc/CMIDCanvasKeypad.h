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


#ifndef CMIDCANVASKEYPAD_H
#define CMIDCANVASKEYPAD_H

#include <coecntrl.h>
#include <gdi.h>
#include <lcdui.h>

class CAknButton;
class CMIDDisplayable;
class CMIDCanvas;

// OnScreenKeypad types
enum TCanvasKeypadType
{
    ENotDefined,
    EKeypadValueNo,
    EKeypadValueNavigationKeys,
    EKeypadValueGameActions,
    EKeypadValueLskRsk
};

// type of pressed button
enum TButtonPress
{
    ENoneKey,
    ERockerKey,
    EOtherKey
};

// All possible OnScreenKeypad keys
enum TKeypadKeys
{
    ENavigationKeysFire,
    ENavigationKeysRight,
    ENavigationKeysLeft,
    ENavigationKeysUp,
    ENavigationKeysDown,
    ENavigationKeysNumberOfButtons,//separator value for key types
    EGameA,
    EGameB,
    EGameC,
    EGameD,
    ELsk,
    ERsk
};

NONSHARABLE_CLASS(CMIDCanvasKeypad) : public CCoeControl
{
    struct TButtonData
    {
        CAknButton* iButton;
        TKeyEvent iKeyEvent;
        TKeyEvent iKeyEvent_2;
        TKeypadKeys keyType;
    };

public:
    friend class CMIDUIManager;

    /**
    * Ctor
    */
    ~CMIDCanvasKeypad();

    //From CCoeControl
    /**
    * @see CCoeControl::CountComponentControls()
    */
    TInt CountComponentControls() const;

    /**
    * @see CCoeControl::ComponentControl()
    */
    CCoeControl* ComponentControl(TInt) const;

    /**
    * @see CCoeControl::SizeChanged()
    */
    void SizeChanged();

    /**
    * @see CCoeControl::HandlePointerEventL()
    */
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);

    /**
    * Layouts and draws OnScreenKeypad to the screen.
    *
    *
    * @param aCanvas            Canvas that which listens to OnScreenKeypad events.
    * @param aKeypadType        OnScreenKeypad type
    * @param aDisplayable       MMIDisplayable that currently uses the CMIDCanvasKeypad.
    * @since S60 5.0
    */
    void UpdateVisualAppearanceL(CMIDCanvas& aCanvas, TInt aKeypadType, MMIDDisplayable& aDisplayable);

    /**
    * Notify a CMIDCanvasKeypad then application is switch to background.
    *
    * @since S60 5.0
    */
    void HandleApplicationBackground();
    /**
    * Send EButton1Up event and stoped repeat timer if OSK in background
    *
    */
    void OSKInBackground(TBool aOSKInBackground);

protected:
    /**
    * Draws skin and separator lines between Canvas and keypad.
    *
    * @since S60 5.0
    */
    void Draw(const TRect &aRect) const;

private:
    /**
    * Ctor
    * @since S60 5.0
    */
    static CMIDCanvasKeypad* NewL(MMIDDisplayable* aDisplayable);

    /**
    * Ctor
    * @since S60 5.0
    */
    void ConstructL(MMIDDisplayable* aDisplayable);

    /**
    * Ctor
    * @since S60 5.0
    */
    CMIDCanvasKeypad();

    /**
    * Deletes all the created buttons.
    *
    * @since S60 5.0
    */
    void DeleteButtons();

    /**
    * Creates all buttons
    *
    * @since S60 5.0
    */
    void CreateButtonsL();

    /**
    * Initializes buttons that are used.
    *
    * @since S60 5.0
    */
    void InitializeKeys();

    /**
    * Sets key event struct for each button.
    * Key events are sent to Canvas when button pressed.
    * @since S60 5.0
    */
    void InitializeKeyEvents(TKeypadKeys aKeypadKeys, TButtonData* aButtonData);

    /**
    * Sets Rect areas for buttons from LAF.
    *
    * @since S60 5.0
    */
    void SetRectForAllButtons();

    /**
    * Updates keypad area rect from LAF.
    *
    * @since S60 5.0
    */
    void UpdateKeypadPaneRect();

    /**
    * Calls OfferKeyEventL function in CMIDCanvas.
    *
    * @param aEvent             Event that is generated.
    * @param aType              Event code that is that is generated.
    * @param aFeedbckRpt    Feedback on key repeat on/off
    * @return TKeyResponse      Returns TKeyResponse indicating whether the key event was consumend.
    * @since S60 5.0
    */
    TKeyResponse RaiseOfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType, TBool aFeedbckRpt = ETrue);

    /**
    * Static callback function called by the repeat timer.
    *
    * @param aThis     Pointer to an instance of this class.
    * @return TInt     Returns zero always.
    * @since S60 5.0
    */
    static TInt RepeatTimerCallbackL(TAny* aThis);

    /**
    * Generates keyRepeated events when repeat timer expires.
    *
    * @since S60 5.0
    */
    void HandleRepeatTimerEventL();

    /**
    * Cancels key repeat timer.
    * Sends also up event for a key that is possible pressed down
    * and also resets button state.
    *
    * @since S60 5.0
    */
    void ResetButtons();

    /**
    * Reference counter.
    * Number of Displayables (Canvases) that are using CanvasKeypad.
    * If reference counter becomes 0, then we destroy ourself.
    */
    TUint16 iRefC;

    /**
    * The whole screen size.
    * Needed when we decide whether we must re-create the button graphics.
    *
    */
    TRect iScreenRect;

    /**
    * Pointer to acting Displayable.
    */
    CMIDDisplayable* iDisplayable;

    /**
    * Pointer to acting Canvas.
    */
    CMIDCanvas* iCanvas;

    /**
    * Rect for the whole keypad area.
    */
    TRect iKeypadPaneRect;

    /**
    * Rect for the rocker keys.
    */
    TRect iRockerRect;

    /**
    * Rect for the left softkey.
    */
    TRect iLskRect;

    /**
    * Rect for the right softkey.
    */
    TRect iRskRect;

    /**
    * Rect for the GAME A button.
    */
    TRect iGameARect;

    /**
    * Rect for the GAME B button.
    */
    TRect iGameBRect;

    /**
    * Rect for the GAME C button.
    */
    TRect iGameCRect;

    /**
    * Rect for the GAME D button.
    */
    TRect iGameDRect;

    /**
    * Information about Canvas display mode.
    */
    TBool iIsFullScreenMode;

    /**
    * Information about layout orientation.
    */
    TBool iIsPortrait;

    /**
    * Information about used keypad type.
    */
    TInt iKeypadType;

    /**
    * Key event type.
    */
    TKeyEvent iKeyEvent;

    /**
    * Counter for received drag events.
    */
    TInt iDrag;

    /**
    * Array for the button data of all buttons.
    */
    RArray<TButtonData> iButtonData;

    /**
    * Array for the pointers to the current buttons in iButtonData.
    */
    RArray<TButtonData*> iCurrentButtonData;

    /**
    * Timer for generating keyRepeated events, owned.
    */
    CPeriodic*     iRepeatTimer;

    /**
    * The Rocker key which is currently active.
    * E.g. keyRepeated events are being generated with iRepeatTimer.
    */
    TButtonData*   iCurrentRockerKey;

    /**
    * The Other key which is currently active.
    * E.g. keyRepeated events are being generated with iRepeatTimer.
    */
    TButtonData*   iCurrentOtherKey;

    /**
    * Indicates value of the application attribute
    * Nokia-MIDlet-S60-Selection-Key-Compatibility.
    */
    TBool iSelectionKeyCompatibility;
    /**
     * Variables for store counter of tap events
     */
    TInt iTapCnt;
    TInt iOldTapCnt;

    /**
     * Timer for counting tap events.
     */
    CPeriodic*    iTapTimer;

    /**
     * Static callback function called on tap timer.
     */
    static TInt TapTimerCallbackL(TAny* aThis);

    /**
     * Tap counter with timer interval KTapTimerExpiryInterval
     */
    void HandleTapTimerEventL();

    /**
     * Change the rocker key state, set skin
     * and refresh rocker keys area
     */
    void ChangeStatus(TInt aState, TButtonData* aButton);

    /**
    * Queue for last hitting keys.
    */
    RArray<TInt> iButtonStack;

    /**
    * Key was hit on the last is set to first in iButtonStack array.
    */
    void ButtonPriorityQueue(TInt aIdx);

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    /**
    * Flag indicates if user dragged from one button to another
    */
    TBool iDragBetweenArrowKeys;

    /**
    * Flag indicates if user dragged outside of button area
    */
    TBool iDragOutsideArrowKeys;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    /**
    * Flag iOSKInBackground sets on true if OSK in background
    */
    TBool iOSKInBackground;

    /**
    * Structure for store key down event
    */
    TPointerEvent iStoreKeyDownEvent;
};

#endif // CMIDCANVASKEYPAD_H

