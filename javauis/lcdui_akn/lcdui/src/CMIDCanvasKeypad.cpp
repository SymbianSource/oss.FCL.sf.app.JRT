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


#include <barsread.h> //resourcereader
#include <lcdui.rsg>
#include <lcdui.mbg>

#include <aknbutton.h>
#include <aknconsts.h>
#include <gulicon.h>

#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <data_caging_path_literals.hrh>
#include <s60commonutils.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif // RD_TACTILE_FEEDBACK

#include "CMIDCanvasKeypad.h"
#include "CMIDDisplayable.h"
#include "CMIDCanvas.h"
#include "CMIDMenuHandler.h"

#include <j2me/jdebug.h>

_LIT(KLcduiBitmapResourceFileName, "lcdui.mif");



//Pointer Drag event dismiss rate (for performance reasons)
const TInt KDragEventHandlingRate = 3;//every third drag event is handled
//Alpha value for keypad/canvas separator lines
const TInt KAlphaValue = 50;
// Keyboard delays and repeat periods
const TInt KFirstTimerExpiryInterval = KAknKeyboardRepeatInitialDelay;
// For second time onwards ,the duration of the time interval.
const TInt KTimerExpiryInterval = KAknStandardKeyboardRepeatRate;

// Constants for tap timers, interval timer
const TInt KFirstTapTimerExpiryInterval = 10000; // 10 ms
const TInt KTapTimerExpiryInterval = 10000; // 10 ms
// Max tap counter KTapMaxCnt*KTapTimerExpiryInterval
const TInt KTapMaxCnt = 8;

CMIDCanvasKeypad::~CMIDCanvasKeypad()
{
    DeleteButtons();
    iButtonData.Close();
    iCurrentButtonData.Close();
    iButtonStack.Close();

    if (iRepeatTimer)
    {
        iRepeatTimer->Cancel();
        delete iRepeatTimer;
        iRepeatTimer = NULL;
    }
    // stop and destroy event timer
    if (iTapTimer)
    {
        iTapTimer->Cancel();
        delete iTapTimer;
        iTapTimer = NULL;
    }

    iOSKInBackground = EFalse;
    if (iBackgroundCC)
    {
        delete iBackgroundCC;
        iBackgroundCC = NULL;
    }
}
TInt CMIDCanvasKeypad::CountComponentControls() const
{
    return iCurrentButtonData.Count();
}

CCoeControl* CMIDCanvasKeypad::ComponentControl(TInt aIndex) const
{
    if (aIndex < iCurrentButtonData.Count())
    {
        return iCurrentButtonData[aIndex]->iButton;
    }
    return NULL;
}

void CMIDCanvasKeypad::SizeChanged()
{
    SetRectForAllButtons();
    if (iBackgroundCC)
    {
        delete iBackgroundCC;
        iBackgroundCC = NULL;
    }
    TRAP_IGNORE(iBackgroundCC = CAknsBasicBackgroundControlContext::NewL(
                                    KAknsIIDQsnBgScreen,
                                    Rect(),
                                    EFalse));
}

void CMIDCanvasKeypad::ButtonPriorityQueue(TInt aIdx)
{
    // Argument aIdx stored index of last pressed key on OSK.
    // Key was hit on the last is set to first in iButtonStack array.
    // Loop started from index aIdx.
    if (aIdx < iButtonStack.Count())
    {
        for (TInt i = aIdx; i > 0 ; i--)
        {
            TInt tmp = iButtonStack[i];
            iButtonStack[i] = iButtonStack[i-1];
            iButtonStack[i-1] = tmp;
        }
    }
}

void CMIDCanvasKeypad::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
    // Because of performance reasons we do not handle all the drag events
    if (aPointerEvent.iType == TPointerEvent::EDrag)
    {
        if (iDrag < KDragEventHandlingRate)
        {
            iDrag++;
            return;
        }
        else
        {
            iDrag = 0;
        }
    }
    else
    {
        iDrag = 0;
    }
    if (iOSKInBackground)
    {
        return;
    }
    TButtonData* oldRockerKey = iCurrentRockerKey;
    TButtonData* newRockerKey = NULL;
    // Indicates which area of the keypad (ERockerKey, EOtherKey or ENoneKey)
    // should handle the pointer event.
    TButtonPress keypadArea = ENoneKey;

    // Rocker keys are: up, down, left right and fire.
    // Other keys are: Game_A, Game_B, Game_C, Game_D, LSK and RSK.
    // 'Rocker keys' are handled differently than 'other keys'
    // When 'Other key' is pressed all the events are delivered to it
    // until the 'Other key' is released. When rocker key is pressed dragging
    // the pointer between rocker keys will also cause changes in button states.

    // If any of the 'Other keys' is not pressed already,
    // then we find out which On Screen Keypad key (if any) was hit by pointer event.
    if (!iCurrentOtherKey)
    {
        TInt buttonCount = iCurrentButtonData.Count();
        TInt i;

        // Array iButtonStack stored indexes of all OSK buttons.
        // Because of performance reasons we used iButtonStack array
        // for priority level sets. Key was hit on the last is set to first.
        for (i = 0; i < buttonCount ; i++)
        {
            if (iCurrentButtonData[ iButtonStack[i] ]->keyType < ENavigationKeysNumberOfButtons)
            {
                const MCoeControlHitTest* buttonHitArea = iCurrentButtonData[ iButtonStack[i] ]->iButton->HitTest();

                // Select rocker key. Method HitRegionContains recognized which button pressed.
                if (iRockerRect.Contains(aPointerEvent.iPosition) &&
                        buttonHitArea->HitRegionContains(aPointerEvent.iPosition, *this))
                {
                    keypadArea = ERockerKey;
                    newRockerKey = iCurrentButtonData[ iButtonStack[i] ];
                    break;
                }
            }
            else if (iCurrentButtonData[ iButtonStack[i] ]->keyType > ENavigationKeysNumberOfButtons)
            {
                // Select other key. Method Contains from Rect() recognized which button pressed.
                if (iCurrentButtonData[ iButtonStack[i] ]->iButton->Rect().Contains(aPointerEvent.iPosition))
                {
                    if (iCurrentRockerKey)
                    {
                        // Pointer event hits 'Other key', but because currently some rocker key is active
                        // the pointer event is handled in the rocker keys.
                        keypadArea = ERockerKey;
                    }
                    else if (!iCurrentRockerKey &&
                             aPointerEvent.iType == TPointerEvent::EButton1Down)
                    {
                        keypadArea = EOtherKey;
                        iCurrentOtherKey = iCurrentButtonData[ iButtonStack[i] ];
                    }
                    break;
                }
            }

        }

        ButtonPriorityQueue(i);
    }
    else // This means that 'Other key' is already pressed so this event is handled there.
    {
        keypadArea = EOtherKey;
    }

    TKeyResponse response = EKeyWasNotConsumed;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    iDragBetweenArrowKeys = EFalse;
    iDragOutsideArrowKeys = EFalse;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    switch (keypadArea)
    {
    case ENoneKey:
    {
        // Rocker key is released when pointer is dragged outside rocker keys area
        if (iCurrentRockerKey)
        {
            iRepeatTimer->Cancel();
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            iDragOutsideArrowKeys = ETrue;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            response = RaiseOfferKeyEventL(iCurrentRockerKey->iKeyEvent, EEventKeyUp);
            iCurrentRockerKey->iKeyEvent_2.iRepeats = 0;
            iCurrentRockerKey->iButton->SetCurrentState(0, ETrue);
            iCurrentRockerKey = NULL;
            return;
        }
        break;
    }
    case ERockerKey:
    {
        //Fire button:Ignore drag from rocker arrows to Fire button i.e. drag allowed only among rocker arrow keys
        if (newRockerKey && newRockerKey->keyType == ENavigationKeysFire &&
                aPointerEvent.iType == TPointerEvent::EDrag)
        {
            return;
        }
        // Rocker key changes from one to another, so we cancel the timer and post up event for the old rocker key etc.
        if (iCurrentRockerKey && newRockerKey != iCurrentRockerKey)
        {
            iRepeatTimer->Cancel();
            oldRockerKey = iCurrentRockerKey;
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            iDragBetweenArrowKeys = ETrue;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            response = RaiseOfferKeyEventL(oldRockerKey->iKeyEvent, EEventKeyUp);
            oldRockerKey->iKeyEvent_2.iRepeats = 0;
            //change button state, refresh rocker keys area
            ChangeStatus(EFalse, oldRockerKey);
        }
        //Fire button:iCurrentRockerKey is changed only for arrow rocker keys
        if (newRockerKey &&
                !(newRockerKey->keyType == ENavigationKeysFire &&
                  aPointerEvent.iType == TPointerEvent::EButton1Up))
        {
            iCurrentRockerKey = newRockerKey;
        }
        // New rocker key pressed. Posts EEventKeyDown and EEventKey events and then starts repeating.
        if ((aPointerEvent.iType == TPointerEvent::EButton1Down ||
                aPointerEvent.iType == TPointerEvent::EDrag) &&
                iCurrentRockerKey && (oldRockerKey != iCurrentRockerKey))
        {
            iRepeatTimer->Cancel();
            // If fire button pressed in normal mode Canvas, then we try to execute a Command
            if (iCurrentRockerKey->keyType == ENavigationKeysFire && !iIsFullScreenMode && !iSelectionKeyCompatibility)
            {
                response = RaiseOfferKeyEventL(iCurrentRockerKey->iKeyEvent, EEventKeyDown);

                //change button state, refresh rocker keys area
                ChangeStatus(ETrue, iCurrentRockerKey);
            }
            else
            {
                iStoreKeyDownEvent = aPointerEvent;
                RaiseOfferKeyEventL(iCurrentRockerKey->iKeyEvent, EEventKeyDown);
                response = RaiseOfferKeyEventL(iCurrentRockerKey->iKeyEvent_2, EEventKey, EFalse);

                if (response == EKeyWasNotConsumed)
                {
                    iCurrentRockerKey->iKeyEvent_2.iRepeats = 1;
                    iRepeatTimer->Start(KFirstTimerExpiryInterval, KTimerExpiryInterval,
                                        TCallBack(RepeatTimerCallbackL, this));
                }
                //change button state, refresh rocker keys area
                ChangeStatus(ETrue, iCurrentRockerKey);
            }
        }

        // Rocker key released. Cancels (possible) repeating and then posts corresponding key up event.
        else if (aPointerEvent.iType == TPointerEvent::EButton1Up && iCurrentRockerKey)
        {
            iRepeatTimer->Cancel();
            response = RaiseOfferKeyEventL(iCurrentRockerKey->iKeyEvent, EEventKeyUp);
            iCurrentRockerKey->iKeyEvent_2.iRepeats = 0;
            //change button state, refresh rocker keys area
            ChangeStatus(EFalse, iCurrentRockerKey);
            iCurrentRockerKey = NULL;
        }
        break;
    }
    case EOtherKey:
    {
        // Drag events are not handled at all.
        if (aPointerEvent.iType == TPointerEvent::EDrag)
        {
            return;
        }

        // Other key is pressed. Posts EEventKeyDown and EEventKey events and then starts repeating.
        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            iStoreKeyDownEvent = aPointerEvent;
            iRepeatTimer->Cancel();
            RaiseOfferKeyEventL(iCurrentOtherKey->iKeyEvent, EEventKeyDown);
            response = RaiseOfferKeyEventL(iCurrentOtherKey->iKeyEvent_2, EEventKey, EFalse);

            if (response == EKeyWasNotConsumed)
            {
                iCurrentOtherKey->iKeyEvent_2.iRepeats = 1;
                iRepeatTimer->Start(KFirstTimerExpiryInterval, KTimerExpiryInterval,
                                    TCallBack(RepeatTimerCallbackL, this));
            }
        }
        // Other key is released. Cancels (possible) repeating and then posts corresponding key up event etc.
        else if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            iRepeatTimer->Cancel();
            response = RaiseOfferKeyEventL(iCurrentOtherKey->iKeyEvent, EEventKeyUp);
            iCurrentOtherKey->iKeyEvent_2.iRepeats = 0;
            iCurrentOtherKey = NULL;
        }

        // Forwards pointer event to CCoeControl if we did not consume the event.
        if (response == EKeyWasNotConsumed)
        {
            CCoeControl::HandlePointerEventL(aPointerEvent);
        }
        break;
    }
    default:
    {
        return;
    }
    }

}

void CMIDCanvasKeypad::UpdateVisualAppearanceL(CMIDCanvas& aCanvas, TInt aKeypadType, MMIDDisplayable& aDisplayable)
{
    if (aKeypadType <= EKeypadValueNo)
    {
        return;
    }

    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);

    iCanvas = static_cast<CMIDCanvas*>(&aCanvas);
    iSelectionKeyCompatibility = iCanvas->SelectionKeyCompatibility();
    iKeypadType = aKeypadType;
    iDisplayable = static_cast<CMIDDisplayable*>(&aDisplayable);
    SetMopParent(iDisplayable);
    iIsFullScreenMode = iDisplayable->IsFullScreenMode();
    ResetButtons();
    TBool oldIsPortrait = iIsPortrait;
    UpdateKeypadPaneRect();

    if ((iScreenRect != screenRect) || (oldIsPortrait != iIsPortrait))
    {
        iScreenRect = screenRect;
    }
    SetRectForAllButtons();//Needed for updating OSK-buttons when closing/opening hw-keyboard
    CreateButtonsL();
    InitializeKeys();
    ActivateL(); // Window owning control draws all children (buttons)
    MakeVisible(ETrue);
    DrawDeferred();
    iOSKInBackground = EFalse;
    if (iBackgroundCC)
        {
            delete iBackgroundCC;
            iBackgroundCC = NULL;
        }
        TRAP_IGNORE(iBackgroundCC = CAknsBasicBackgroundControlContext::NewL(
                                        KAknsIIDQsnBgScreen,
                                        Rect(),
                                        EFalse));
}

void CMIDCanvasKeypad::Draw(const TRect& /*aRect*/) const
{
    CWindowGc& gc = SystemGc();
    //Drawing skin
    gc.SetBrushColor(TRgb(0x0));   //black
    if (!iIsFullScreenMode)  //normal mode
    {
        AknsDrawUtils::DrawBackground(AknsUtils::SkinInstance(),
                                      iBackgroundCC,
                                      NULL,
                                      gc,
                                      TPoint(0,0),
                                      Rect(),
                                      KAknsDrawParamDefault);
        gc.SetPenColor(TRgb(0x0, KAlphaValue));
    }
    else //full screen mode
    {
        AknsDrawUtils::DrawBackground(NULL,
                                      iBackgroundCC,
                                      NULL,
                                      gc,
                                      TPoint(0,0),
                                      Rect(),
                                      KAknsDrawParamDefault);
        gc.SetPenColor(TRgb(0xFF, KAlphaValue));
    }

    //Drawing separator lines
    if (iIsPortrait)
    {
        gc.DrawLine(TPoint(0,0), TPoint(Size().iWidth,0));
    }
    else //landscape
    {
        //left separator
        if (iCanvas->PositionRelativeToScreen().iX > 0)
        {
            gc.DrawLine(TPoint(iCanvas->PositionRelativeToScreen().iX - 1, 0),
                        TPoint(iCanvas->PositionRelativeToScreen().iX - 1,
                               iCanvas->Size().iHeight));
        }
        //right separator
        gc.DrawLine(TPoint(iCanvas->PositionRelativeToScreen().iX + iCanvas->Size().iWidth, 0),
                    TPoint(iCanvas->PositionRelativeToScreen().iX + iCanvas->Size().iWidth,
                           iCanvas->Size().iHeight));
    }
}

CMIDCanvasKeypad* CMIDCanvasKeypad::NewL(MMIDDisplayable* aDisplayable)
{
    if (!aDisplayable)
    {
        User::Leave(KErrArgument);
    }
    CMIDCanvasKeypad* self = new(ELeave) CMIDCanvasKeypad();
    CleanupStack::PushL(self);
    self->ConstructL(aDisplayable);
    CleanupStack::Pop(self);
    return self;
}

void CMIDCanvasKeypad::ConstructL(MMIDDisplayable* aDisplayable)
{
    iDisplayable = static_cast< CMIDDisplayable* >(aDisplayable);
    // Timer for implementing key repeat feature.
    iRepeatTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    // Timer for implementing tap counter feature.
    iTapTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iTapCnt = -1;
    CreateWindowL();
    SetComponentsToInheritVisibility(ETrue);
    Window().SetPointerGrab(ETrue);

    TRAP_IGNORE(iBackgroundCC = CAknsBasicBackgroundControlContext::NewL(
                                    KAknsIIDQsnBgScreen,
                                    Rect(),
                                    EFalse));
}

CMIDCanvasKeypad::CMIDCanvasKeypad()
{
}

void CMIDCanvasKeypad::DeleteButtons()
{
    TInt buttonCount = iButtonData.Count();

    for (TInt i = 0; i < buttonCount; i++)
    {
        delete iButtonData[i].iButton;
        iButtonData[i].iButton = NULL;
    }
}

void CMIDCanvasKeypad::CreateButtonsL()
{
    DeleteButtons();
    iButtonData.Reset();
    iCurrentButtonData.Reset();

    if (iKeypadType != EKeypadValueLskRsk)
    {
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC(reader, R_ONSCREEN_KEYBOARD_ROCKER_BUTTONS);
        TInt numButtons(reader.ReadInt16());

        for (TInt i = 0; i < numButtons; i++)
        {
            TButtonData buttonData;
            buttonData.iButton = CAknButton::NewL();
            CleanupStack::PushL(buttonData.iButton);
            buttonData.iButton->ConstructFromResourceL(reader);
            buttonData.iButton->SetContainerWindowL(*this);
            buttonData.iButton->SetExtent(iRockerRect.iTl, iRockerRect.Size());
            buttonData.iButton->ActivateL();
            InitializeKeyEvents(TKeypadKeys(i), &buttonData);
            iButtonData.Append(buttonData);
            CleanupStack::Pop(buttonData.iButton);
        }
        CleanupStack::PopAndDestroy(); // reader;
    }

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CFbsBitmap* bitmap(NULL);
    CFbsBitmap* mask(NULL);
    // Get icon ids.
    TAknsItemID skinItemId(KAknsIIDNone);

    TFileName fileName;
    fileName.Append(KDC_APP_BITMAP_DIR);
    fileName.Append(KLcduiBitmapResourceFileName);
    fileName = java::util::S60CommonUtils::VerifiedFileNameL(fileName);

    if (iKeypadType == EKeypadValueGameActions)
    {
        //GameA
        AknsUtils::CreateColorIconLC(
            skin,
            skinItemId,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG30,
            bitmap,
            mask,
            fileName,
            EMbmLcduiQgn_indi_game_a,
            EMbmLcduiQgn_indi_game_a_mask,
            KRgbBlack
        );

        CGulIcon* iconA = CGulIcon::NewL(bitmap, mask);
        //Ownership of bitmap and mask transferred
        CleanupStack::Pop(2);  //bitmap, mask
        CleanupStack::PushL(iconA);
        TButtonData buttonDataGameA;
        buttonDataGameA.iButton = CAknButton::NewL(
                                      iconA , // ownership taken
                                      NULL, NULL, NULL, _L(""), _L(""), 0, 0);
        buttonDataGameA.iButton->SetIconScaleMode(EAspectRatioPreserved);
        buttonDataGameA.iButton->SetMargins(TMargins8(0,0,0,0));
        CleanupStack::Pop(iconA);


        buttonDataGameA.iButton->SetContainerWindowL(*this);
        buttonDataGameA.iButton->SetExtent(iGameARect.iTl, iGameARect.Size());
        buttonDataGameA.iButton->ActivateL();
        InitializeKeyEvents(EGameA, &buttonDataGameA);
        iButtonData.Append(buttonDataGameA);

        //GameB
        bitmap = NULL;
        mask =NULL;
        AknsUtils::CreateColorIconLC(
            skin,
            skinItemId,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG30,
            bitmap,
            mask,
            fileName,
            EMbmLcduiQgn_indi_game_b,
            EMbmLcduiQgn_indi_game_b_mask,
            KRgbBlack
        );

        CGulIcon* iconB = CGulIcon::NewL(bitmap, mask);
        //Ownership of bitmap and mask transferred
        CleanupStack::Pop(2);  //bitmap, mask
        CleanupStack::PushL(iconB);
        TButtonData buttonDataGameB;
        buttonDataGameB.iButton = CAknButton::NewL(
                                      iconB , // ownership taken
                                      NULL, NULL, NULL, _L(""), _L(""), 0, 0);
        buttonDataGameB.iButton->SetIconScaleMode(EAspectRatioPreserved);
        buttonDataGameB.iButton->SetMargins(TMargins8(0,0,0,0));
        CleanupStack::Pop(iconB);


        buttonDataGameB.iButton->SetContainerWindowL(*this);
        buttonDataGameB.iButton->SetExtent(iGameBRect.iTl, iGameBRect.Size());
        buttonDataGameB.iButton->ActivateL();
        InitializeKeyEvents(EGameB, &buttonDataGameB);
        iButtonData.Append(buttonDataGameB);

        //GameC
        bitmap = NULL;
        mask =NULL;
        AknsUtils::CreateColorIconLC(
            skin,
            skinItemId,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG30,
            bitmap,
            mask,
            fileName,
            EMbmLcduiQgn_indi_game_c,
            EMbmLcduiQgn_indi_game_c_mask,
            KRgbBlack
        );

        CGulIcon* iconC = CGulIcon::NewL(bitmap, mask);
        //Ownership of bitmap and mask transferred
        CleanupStack::Pop(2);  //bitmap, mask
        CleanupStack::PushL(iconC);
        TButtonData buttonDataGameC;
        buttonDataGameC.iButton = CAknButton::NewL(
                                      iconC , // ownership taken
                                      NULL, NULL, NULL, _L(""), _L(""), 0, 0);
        buttonDataGameC.iButton->SetIconScaleMode(EAspectRatioPreserved);
        buttonDataGameC.iButton->SetMargins(TMargins8(0,0,0,0));
        CleanupStack::Pop(iconC);


        buttonDataGameC.iButton->SetContainerWindowL(*this);
        buttonDataGameC.iButton->SetExtent(iGameCRect.iTl, iGameCRect.Size());
        buttonDataGameC.iButton->ActivateL();
        InitializeKeyEvents(EGameC, &buttonDataGameC);
        iButtonData.Append(buttonDataGameC);

        //GameD
        bitmap = NULL;
        mask =NULL;
        AknsUtils::CreateColorIconLC(
            skin,
            skinItemId,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG30,
            bitmap,
            mask,
            fileName,
            EMbmLcduiQgn_indi_game_d,
            EMbmLcduiQgn_indi_game_d_mask,
            KRgbBlack
        );

        CGulIcon* iconD = CGulIcon::NewL(bitmap, mask);
        //Ownership of bitmap and mask transferred
        CleanupStack::Pop(2);  //bitmap, mask
        CleanupStack::PushL(iconD);
        TButtonData buttonDataGameD;
        buttonDataGameD.iButton = CAknButton::NewL(
                                      iconD , // ownership taken
                                      NULL, NULL, NULL, _L(""), _L(""), 0, 0);
        buttonDataGameD.iButton->SetIconScaleMode(EAspectRatioPreserved);
        buttonDataGameD.iButton->SetMargins(TMargins8(0,0,0,0));
        CleanupStack::Pop(iconD);

        buttonDataGameD.iButton->SetContainerWindowL(*this);
        buttonDataGameD.iButton->SetExtent(iGameDRect.iTl, iGameDRect.Size());
        buttonDataGameD.iButton->ActivateL();
        InitializeKeyEvents(EGameD, &buttonDataGameD);
        iButtonData.Append(buttonDataGameD);
    }

    //LSK & RSK buttons
    TButtonData buttonDataLSK;
    TButtonData buttonDataRSK;

    if (iIsPortrait)
    {
        //LSK
        bitmap = NULL;
        mask =NULL;
        AknsUtils::CreateColorIconLC(
            skin,
            skinItemId,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG30,
            bitmap,
            mask,
            fileName,
            EMbmLcduiQgn_indi_lsk_horizontal,
            EMbmLcduiQgn_indi_lsk_horizontal_mask,
            KRgbBlack
        );

        CGulIcon* iconLsk = CGulIcon::NewL(bitmap, mask);
        //Ownership of bitmap and mask transferred
        CleanupStack::Pop(2);  //bitmap, mask
        CleanupStack::PushL(iconLsk);
        buttonDataLSK.iButton = CAknButton::NewL(
                                    iconLsk , // ownership taken
                                    NULL, NULL, NULL, _L(""), _L(""), 0, 0);
        buttonDataLSK.iButton->SetIconScaleMode(EAspectRatioPreserved);
        buttonDataLSK.iButton->SetMargins(TMargins8(0,0,0,0));
        CleanupStack::Pop(iconLsk);

        //RSK
        bitmap = NULL;
        mask =NULL;
        AknsUtils::CreateColorIconLC(
            skin,
            skinItemId,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG30,
            bitmap,
            mask,
            fileName,
            EMbmLcduiQgn_indi_rsk_horizontal,
            EMbmLcduiQgn_indi_rsk_horizontal_mask,
            KRgbBlack
        );

        CGulIcon* iconRsk = CGulIcon::NewL(bitmap, mask);
        //Ownership of bitmap and mask transferred
        CleanupStack::Pop(2);  //bitmap, mask
        CleanupStack::PushL(iconRsk);
        buttonDataRSK.iButton = CAknButton::NewL(
                                    iconRsk , // ownership taken
                                    NULL, NULL, NULL, _L(""), _L(""), 0, 0);
        buttonDataRSK.iButton->SetIconScaleMode(EAspectRatioPreserved);
        buttonDataRSK.iButton->SetMargins(TMargins8(0,0,0,0));
        CleanupStack::Pop(iconRsk);
    }
    else //landscape
    {
        if (iKeypadType == EKeypadValueLskRsk)
        {
            //LSK
            bitmap = NULL;
            mask =NULL;
            AknsUtils::CreateColorIconLC(
                skin,
                skinItemId,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG30,
                bitmap,
                mask,
                fileName,
                (iDisplayable->GetSKPositionForOSK() == CMIDDisplayable::ESoftkeysBottom ?
                 EMbmLcduiQgn_indi_lsk_horizontal : EMbmLcduiQgn_indi_lsk_vertical),
                (iDisplayable->GetSKPositionForOSK() == CMIDDisplayable::ESoftkeysBottom ?
                 EMbmLcduiQgn_indi_lsk_horizontal_mask : EMbmLcduiQgn_indi_lsk_vertical_mask),
                KRgbBlack
            );
        }
        else
        {
            //LSK
            bitmap = NULL;
            mask =NULL;
            AknsUtils::CreateColorIconLC(
                skin,
                skinItemId,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG30,
                bitmap,
                mask,
                fileName,
                EMbmLcduiQgn_indi_lsk_vertical,
                EMbmLcduiQgn_indi_lsk_vertical_mask,
                KRgbBlack
            );
        }

        CGulIcon* iconLsk = CGulIcon::NewL(bitmap, mask);
        //Ownership of bitmap and mask transferred
        CleanupStack::Pop(2);  //bitmap, mask
        CleanupStack::PushL(iconLsk);
        //TButtonData buttonDataLSK;
        buttonDataLSK.iButton = CAknButton::NewL(
                                    iconLsk , // ownership taken
                                    NULL, NULL, NULL, _L(""), _L(""), 0, 0);
        buttonDataLSK.iButton->SetIconScaleMode(EAspectRatioPreserved);
        buttonDataLSK.iButton->SetMargins(TMargins8(0,0,0,0));
        CleanupStack::Pop(iconLsk);

        if (iKeypadType == EKeypadValueLskRsk)
        {
            //RSK
            bitmap = NULL;
            mask =NULL;
            AknsUtils::CreateColorIconLC(
                skin,
                skinItemId,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG30,
                bitmap,
                mask,
                fileName,
                (iDisplayable->GetSKPositionForOSK() == CMIDDisplayable::ESoftkeysBottom ?
                 EMbmLcduiQgn_indi_rsk_horizontal : EMbmLcduiQgn_indi_rsk_vertical),
                (iDisplayable->GetSKPositionForOSK() == CMIDDisplayable::ESoftkeysBottom ?
                 EMbmLcduiQgn_indi_rsk_horizontal_mask : EMbmLcduiQgn_indi_rsk_vertical_mask),
                KRgbBlack
            );
        }
        else
        {
            //RSK
            bitmap = NULL;
            mask =NULL;
            AknsUtils::CreateColorIconLC(
                skin,
                skinItemId,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG30,
                bitmap,
                mask,
                fileName,
                EMbmLcduiQgn_indi_rsk_vertical,
                EMbmLcduiQgn_indi_rsk_vertical_mask,
                KRgbBlack
            );
        }

        CGulIcon* iconRsk = CGulIcon::NewL(bitmap, mask);
        //Ownership of bitmap and mask transferred
        CleanupStack::Pop(2);  //bitmap, mask
        CleanupStack::PushL(iconRsk);
        //TButtonData buttonDataRSK;
        buttonDataRSK.iButton = CAknButton::NewL(
                                    iconRsk , // ownership taken
                                    NULL, NULL, NULL, _L(""), _L(""), 0, 0);
        buttonDataRSK.iButton->SetIconScaleMode(EAspectRatioPreserved);
        buttonDataRSK.iButton->SetMargins(TMargins8(0,0,0,0));
        CleanupStack::Pop(iconRsk);
    }

    buttonDataLSK.iButton->SetContainerWindowL(*this);
    buttonDataLSK.iButton->SetExtent(iLskRect.iTl, iLskRect.Size());
    buttonDataLSK.iButton->ActivateL();
    InitializeKeyEvents(ELsk, &buttonDataLSK);
    iButtonData.Append(buttonDataLSK);

    buttonDataRSK.iButton->SetContainerWindowL(*this);
    buttonDataRSK.iButton->SetExtent(iRskRect.iTl, iRskRect.Size());
    buttonDataRSK.iButton->ActivateL();
    InitializeKeyEvents(ERsk, &buttonDataRSK);
    iButtonData.Append(buttonDataRSK);

}

void CMIDCanvasKeypad::InitializeKeys()
{
    TInt buttonCount = iButtonData.Count();
    iCurrentButtonData.Reset();
    iButtonStack.Reset();

    for (TInt i=0; i < buttonCount; i++)
    {
        iButtonData[i].iButton->ResetState();
        // Rocker keys
        if (iButtonData[i].keyType < ENavigationKeysNumberOfButtons)
        {
            if (iButtonData[i].iButton)
            {
                iButtonData[i].iButton->SetPosition(iRockerRect.iTl);
                iButtonData[i].iButton->SetSize(iRockerRect.Size());
                iCurrentButtonData.Append(&iButtonData[i]);
                iButtonStack.Append(i);
            }
        }
        // Other keys
        else
        {
            // Game A, B, C, D keys
            if (iKeypadType == EKeypadValueGameActions)
            {
                if (iButtonData[i].keyType == EGameA)
                {
                    iButtonData[i].iButton->SetPosition(iGameARect.iTl);
                    iButtonData[i].iButton->SetSize(iGameARect.Size());
                    iCurrentButtonData.Append(&iButtonData[i]);
                    iButtonStack.Append(i);
                }
                else if (iButtonData[i].keyType == EGameB)
                {
                    iButtonData[i].iButton->SetPosition(iGameBRect.iTl);
                    iButtonData[i].iButton->SetSize(iGameBRect.Size());
                    iCurrentButtonData.Append(&iButtonData[i]);
                    iButtonStack.Append(i);
                }
                else if (iButtonData[i].keyType == EGameC)
                {
                    iButtonData[i].iButton->SetPosition(iGameCRect.iTl);
                    iButtonData[i].iButton->SetSize(iGameCRect.Size());
                    iCurrentButtonData.Append(&iButtonData[i]);
                    iButtonStack.Append(i);
                }
                else if (iButtonData[i].keyType == EGameD)
                {
                    iButtonData[i].iButton->SetPosition(iGameDRect.iTl);
                    iButtonData[i].iButton->SetSize(iGameDRect.Size());
                    iCurrentButtonData.Append(&iButtonData[i]);
                    iButtonStack.Append(i);
                }
            }

            // LSK and RSK
            if (iButtonData[i].keyType == ELsk && iIsFullScreenMode)
            {
                iButtonData[i].iButton->SetExtent(iLskRect.iTl, iLskRect.Size());
                iCurrentButtonData.Append(&iButtonData[i]);
                iButtonStack.Append(i);
            }
            else if (iButtonData[i].keyType == ERsk && iIsFullScreenMode)
            {
                iButtonData[i].iButton->SetExtent(iRskRect.iTl, iRskRect.Size());
                iCurrentButtonData.Append(&iButtonData[i]);
                iButtonStack.Append(i);
            }
        }
    }
}

void CMIDCanvasKeypad::InitializeKeyEvents(TKeypadKeys aKeypadKeys, TButtonData* aButtonData)
{
    aButtonData->iKeyEvent.iCode = 0;
    aButtonData->iKeyEvent.iModifiers = 0;
    aButtonData->iKeyEvent.iScanCode = 0;
    aButtonData->iKeyEvent.iRepeats = 0;
    aButtonData->iKeyEvent_2.iRepeats = 0;

    switch (aKeypadKeys)
    {
    case ENavigationKeysFire:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyDevice3;
        aButtonData->iKeyEvent_2.iCode = EKeyDevice3;
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyDevice3;
        aButtonData->keyType = ENavigationKeysFire;
        break;
    }
    case ENavigationKeysRight:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyRightArrow;
        aButtonData->iKeyEvent_2.iCode = EKeyRightArrow;
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyRightArrow;
        aButtonData->keyType = ENavigationKeysRight;
        break;
    }
    case ENavigationKeysLeft:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyLeftArrow;
        aButtonData->iKeyEvent_2.iCode = EKeyLeftArrow;
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyLeftArrow;
        aButtonData->keyType = ENavigationKeysLeft;
        break;
    }
    case ENavigationKeysUp:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyUpArrow;
        aButtonData->iKeyEvent_2.iCode = EKeyUpArrow;
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyUpArrow;
        aButtonData->keyType = ENavigationKeysUp;
        break;
    }
    case ENavigationKeysDown:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyDownArrow;
        aButtonData->iKeyEvent_2.iCode = EKeyDownArrow;
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyDownArrow;
        aButtonData->keyType = ENavigationKeysDown;
        break;
    }
    case ELsk:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyDevice0;
        aButtonData->iKeyEvent_2.iCode = EKeyDevice0;
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyDevice0;
        aButtonData->keyType = ELsk;
        break;
    }
    case ERsk:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyDevice1;
        aButtonData->iKeyEvent_2.iCode = EKeyDevice1;
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyDevice1;
        aButtonData->keyType = ERsk;
        break;
    }
    case EGameA:
    {
        aButtonData->iKeyEvent.iScanCode = '7';
        aButtonData->iKeyEvent_2.iCode = '7';
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = '7';
        aButtonData->keyType = EGameA;
        break;
    }
    case EGameB:
    {
        aButtonData->iKeyEvent.iScanCode = '9';
        aButtonData->iKeyEvent_2.iCode = '9';
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = '9';
        aButtonData->keyType = EGameB;
        break;
    }
    case EGameC:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyNkpAsterisk;
        aButtonData->iKeyEvent_2.iCode = '*';
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyNkpAsterisk;
        aButtonData->keyType = EGameC;
        break;
    }
    case EGameD:
    {
        aButtonData->iKeyEvent.iScanCode = EStdKeyHash;
        aButtonData->iKeyEvent_2.iCode = '#';
        aButtonData->iKeyEvent_2.iModifiers = EModifierAutorepeatable;
        aButtonData->iKeyEvent_2.iScanCode = EStdKeyHash;
        aButtonData->keyType = EGameD;
        break;
    }
    default:
        break;
    }
}

void CMIDCanvasKeypad::SetRectForAllButtons()
{
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);
    TInt layoutVariantForRockerKeys=0;
    TInt layoutVariantForGameGrid=0;
    TInt layoutVariantForSKLeft=0;
    TInt layoutVariantForSKRight=0;
    TInt layoutVariantForGameCell = 0;

    switch (iKeypadType)
    {
    case EKeypadValueNo:
        break;
    case EKeypadValueNavigationKeys:
    {
        if (iIsFullScreenMode)
        {
            if (iIsPortrait)
            {
                layoutVariantForRockerKeys = 0;
                layoutVariantForSKLeft = 0;
                layoutVariantForSKRight = 0;
            }
            else //landscape
            {
                layoutVariantForRockerKeys = 2;
                layoutVariantForSKLeft = 9;
                layoutVariantForSKRight = 8;
                layoutVariantForGameCell = 1;
            }
        }
        else //normal mode
        {
            if (iIsPortrait)
            {
                layoutVariantForRockerKeys = 4;
            }
            else //landscape
            {
                layoutVariantForRockerKeys = 6;
                layoutVariantForGameCell = 2;
            }
        }
        break;
    }
    case EKeypadValueGameActions:
    {
        if (iIsFullScreenMode)
        {
            if (iIsPortrait)
            {
                layoutVariantForRockerKeys = 1;
                layoutVariantForGameGrid = 0;
                layoutVariantForSKLeft = 1;
                layoutVariantForSKRight = 1;
            }
            else //landscape
            {
                layoutVariantForRockerKeys = 2;
                layoutVariantForGameGrid = 1;
                layoutVariantForSKLeft = 9;
                layoutVariantForSKRight = 8;
                layoutVariantForGameCell = 1;
            }
        }
        else //normal mode
        {
            if (iIsPortrait)
            {
                layoutVariantForRockerKeys = 5;
                layoutVariantForGameGrid = 0;
            }
            else//landscape
            {
                layoutVariantForRockerKeys = 6;
                layoutVariantForGameGrid = 3;
                layoutVariantForGameCell = 2;
            }
        }
        break;
    }
    case EKeypadValueLskRsk:
    {
        if (iIsFullScreenMode)
        {
            if (iIsPortrait)
            {
                layoutVariantForSKLeft = 12;
                layoutVariantForSKRight = 11;
            }
            else //landscape
            {
                if (iDisplayable->GetSKPositionForOSK() == CMIDDisplayable::ESoftkeysRight)
                {
                    layoutVariantForSKLeft = 11;
                    layoutVariantForSKRight = 10;
                }
                else  //default mode:Softkeys bottom
                {
                    layoutVariantForSKLeft = 10;
                    layoutVariantForSKRight = 9;
                }
            }
        }
        break;
    }
    default:
        break;
    }

    if (iKeypadType != EKeypadValueLskRsk)
    {
        //Rocker keys rect
        TAknLayoutRect layRectRocker;
        layRectRocker.LayoutRect(Rect(), AknLayoutScalable_Avkon::
                                 midp_keyp_rocker_pane(layoutVariantForRockerKeys).LayoutLine());
        iRockerRect = layRectRocker.Rect();

        //set game a,b,c,d
        TAknLayoutRect gameRect;
        gameRect.LayoutRect(Rect(), AknLayoutScalable_Avkon::
                            midp_keyp_game_grid_pane(layoutVariantForGameGrid).LayoutLine());

        TAknLayoutRect gamePaneButtonA;
        if (iIsPortrait)
        {                                            //variety, column, row
            gamePaneButtonA.LayoutRect(gameRect.Rect(), AknLayoutScalable_Avkon::
                                       keyp_game_cell_pane(0,0,0).LayoutLine());
        }
        else
        {
            gamePaneButtonA.LayoutRect(gameRect.Rect(), AknLayoutScalable_Avkon::
                                       keyp_game_cell_pane(layoutVariantForGameCell,0,0).LayoutLine());
        }
        iGameARect = gamePaneButtonA.Rect();

        TAknLayoutRect gamePaneButtonB;
        if (iIsPortrait)
        {
            gamePaneButtonB.LayoutRect(gameRect.Rect(), AknLayoutScalable_Avkon::
                                       keyp_game_cell_pane(0,1,0).LayoutLine());
        }
        else
        {
            gamePaneButtonB.LayoutRect(gameRect.Rect(), AknLayoutScalable_Avkon::
                                       keyp_game_cell_pane(layoutVariantForGameCell,0,1).LayoutLine());
        }
        iGameBRect = gamePaneButtonB.Rect();

        TAknLayoutRect gamePaneButtonC;
        if (iIsPortrait)
        {
            gamePaneButtonC.LayoutRect(gameRect.Rect(), AknLayoutScalable_Avkon::
                                       keyp_game_cell_pane(0,2,0).LayoutLine());
        }
        else
        {
            gamePaneButtonC.LayoutRect(gameRect.Rect(), AknLayoutScalable_Avkon::
                                       keyp_game_cell_pane(layoutVariantForGameCell,0,2).LayoutLine());
        }
        iGameCRect = gamePaneButtonC.Rect();

        TAknLayoutRect gamePaneButtonD;
        if (iIsPortrait)
        {
            gamePaneButtonD.LayoutRect(gameRect.Rect(), AknLayoutScalable_Avkon::
                                       keyp_game_cell_pane(0,3,0).LayoutLine());
        }
        else
        {
            gamePaneButtonD.LayoutRect(gameRect.Rect(), AknLayoutScalable_Avkon::
                                       keyp_game_cell_pane(layoutVariantForGameCell,0,3).LayoutLine());
        }

        iGameDRect = gamePaneButtonD.Rect();
    }

    //set midp_keyp_sk_left
    TAknLayoutRect layRectSKLeft;
    layRectSKLeft.LayoutRect(Rect(), AknLayoutScalable_Avkon::
                             midp_keyp_sk_left_pane(layoutVariantForSKLeft).LayoutLine());
    TAknLayoutRect layRectLsk;
    layRectLsk.LayoutRect(layRectSKLeft.Rect(), AknLayoutScalable_Avkon::
                          bg_button_pane_cp03(0).LayoutLine());
    iLskRect = layRectLsk.Rect();

    //set midp_keyp_sk_right
    TAknLayoutRect layRectSKRight;
    layRectSKRight.LayoutRect(Rect(), AknLayoutScalable_Avkon::
                              midp_keyp_sk_right_pane(layoutVariantForSKRight).LayoutLine());
    TAknLayoutRect layRectRsk;
    layRectRsk.LayoutRect(layRectSKRight.Rect(), AknLayoutScalable_Avkon::
                          bg_button_pane_cp04(0).LayoutLine());
    iRskRect = layRectRsk.Rect();
}

void CMIDCanvasKeypad::UpdateKeypadPaneRect()
{
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screen);
    TAknLayoutRect layRect;
    TInt layoutVariantForKeypadRect = 0;

    TAknLayoutRect mainPane;
    TAknLayoutRect mainMidpPane;
    mainPane.LayoutRect(screen, AknLayoutScalable_Avkon::main_pane(3).LayoutLine());
    mainMidpPane.LayoutRect(mainPane.Rect(),
                            AknLayoutScalable_Avkon::main_midp_pane(0).LayoutLine());

    if (Layout_Meta_Data::IsLandscapeOrientation())
    {
        iIsPortrait = EFalse;
    }
    else //portrait
    {
        iIsPortrait = ETrue;
    }

    switch (iKeypadType)
    {
    case EKeypadValueNo:
        break;
    case EKeypadValueNavigationKeys:
    {
        if (iIsFullScreenMode)
        {
            if (iIsPortrait)
            {
                layoutVariantForKeypadRect = 0;
            }
            else //landscape
            {
                layoutVariantForKeypadRect = 7;
            }
        }
        else //normal mode
        {
            if (iIsPortrait)
            {
                screen = mainMidpPane.Rect();
                layoutVariantForKeypadRect = 3;
            }
            else //landscape
            {
                mainPane.LayoutRect(screen,
                                    AknLayoutScalable_Avkon::main_pane(4).LayoutLine());
                mainMidpPane.LayoutRect(mainPane.Rect(), AknLayoutScalable_Avkon::
                                        main_midp_pane(0).LayoutLine());
                screen = mainMidpPane.Rect();
                layoutVariantForKeypadRect = 5;
            }
        }
        break;
    }
    case EKeypadValueGameActions:
    {
        if (iIsFullScreenMode)
        {
            if (iIsPortrait)
            {
                layoutVariantForKeypadRect = 1;
            }
            else //landscape
            {
                layoutVariantForKeypadRect = 2;
            }
        }
        else //normal mode
        {
            if (iIsPortrait)
            {
                screen = mainMidpPane.Rect();
                layoutVariantForKeypadRect = 4;
            }
            else//landscape
            {
                mainPane.LayoutRect(screen,
                                    AknLayoutScalable_Avkon::main_pane(4).LayoutLine());
                mainMidpPane.LayoutRect(mainPane.Rect(), AknLayoutScalable_Avkon::
                                        main_midp_pane(0).LayoutLine());
                screen = mainMidpPane.Rect();
                layoutVariantForKeypadRect = 5;
            }
        }
        break;
    }
    case EKeypadValueLskRsk:
    {
        if (iIsFullScreenMode)
        {
            if (iIsPortrait)
            {
                layoutVariantForKeypadRect = 10;
            }
            else //landscape
            {
                if (iDisplayable->GetSKPositionForOSK() == CMIDDisplayable::ESoftkeysRight)
                {
                    layoutVariantForKeypadRect = 9;
                }
                else  //default mode:Softkeys bottom
                {
                    layoutVariantForKeypadRect = 8;
                }
            }
        }
        break;
    }
    default:
        break;
    }

    layRect.LayoutRect(screen, AknLayoutScalable_Avkon::
                       midp_keypad_pane(layoutVariantForKeypadRect).LayoutLine());

    if (iKeypadPaneRect != layRect.Rect())
    {
        iKeypadPaneRect = layRect.Rect();
        SetRect(iKeypadPaneRect);
    }
    else
    {
        return;
    }
}

TKeyResponse CMIDCanvasKeypad::RaiseOfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType, TBool aFeedbckRpt)
{
#ifdef RD_TACTILE_FEEDBACK
    MTouchFeedback* feedback = MTouchFeedback::Instance();

    if (feedback)
    {
        switch (aType)
        {
        case EEventKeyDown:
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            if (iDragBetweenArrowKeys)
            {
                feedback->InstantFeedback(ETouchFeedbackSensitiveButton);
            }
            else
            {
                feedback->InstantFeedback(ETouchFeedbackBasicButton);
            }
#else
            feedback->InstantFeedback(ETouchFeedbackBasic);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            break;
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        case EEventKeyUp:
            if (!(iDragBetweenArrowKeys || iDragOutsideArrowKeys))
            {
                //Feedback on release shouldn't be in two cases:
                //- if pointer was dragged between arrow keys
                //- if pointer was dragged outside the keypad
                feedback->InstantFeedback(ETouchFeedbackBasicButton);
            }
            break;
        case EEventKey:
            if (aFeedbckRpt)
            {
                feedback->InstantFeedback(ETouchFeedbackSensitiveButton);
            }
            break;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK                            
        }
    }
#endif // RD_TACTILE_FEEDBACK

    (void)aFeedbckRpt;  // Just to remove a compiler warning

    // Posts event to Canvas.
    return iCanvas->OfferKeyEventL(aEvent, aType);
}

TInt CMIDCanvasKeypad::RepeatTimerCallbackL(TAny* aThis)
{
    CMIDCanvasKeypad* observer = static_cast<CMIDCanvasKeypad*>(aThis);
    observer->HandleRepeatTimerEventL();
    return 0;
}

void CMIDCanvasKeypad::HandleRepeatTimerEventL()
{
    DEBUG("CMIDCanvasKeypad::HandleTimerEvent +");
    if (iCurrentRockerKey && iCurrentRockerKey->iKeyEvent_2.iRepeats == 1)
    {
        // Posts repeat event
        RaiseOfferKeyEventL(iCurrentRockerKey->iKeyEvent_2, EEventKey);
    }
    else if (iCurrentOtherKey && iCurrentOtherKey->iKeyEvent_2.iRepeats == 1)
    {
        // Posts repeat event
        RaiseOfferKeyEventL(iCurrentOtherKey->iKeyEvent_2, EEventKey);
    }
    // Change rocker key status and refresh, while button is pressed
    // down for KTimerExpiryInterval time interval
    if (iCurrentRockerKey && iCurrentRockerKey->iButton && iCurrentRockerKey->iButton->StateIndex() <= 0)
    {
        iOldTapCnt = -1;
        ChangeStatus(ETrue, iCurrentRockerKey);
    }
    DEBUG("CMIDCanvasKeypad::HandleTimerEvent -");
}

void CMIDCanvasKeypad::ResetButtons()
{
    // If a timer exists, we need stop it.
    if (iRepeatTimer)
    {
        iRepeatTimer->Cancel();
    }

    // If there is a key pressed currently, we just update visual
    // state and set the current key to null.
    // Notice that, we don't try to send key up event to CMIDCanvas.
    if (iCurrentRockerKey)
    {
        iCurrentRockerKey->iKeyEvent_2.iRepeats = 0;
        iCurrentRockerKey->iButton->SetCurrentState(0, ETrue);
        iCurrentRockerKey->iButton->ResetState();
        iCurrentRockerKey = NULL;
    }
    else if (iCurrentOtherKey)
    {
        iCurrentOtherKey->iKeyEvent_2.iRepeats = 0;
        iCurrentOtherKey->iButton->SetCurrentState(0, ETrue);
        iCurrentOtherKey->iButton->ResetState();
        iCurrentOtherKey = NULL;
    }

}

void CMIDCanvasKeypad::HandleApplicationBackground()
{
    // This code stops On-Screen Keypad timer repeating keyevents
    // if current application goes to background.
    ResetButtons();
}

void CMIDCanvasKeypad::ChangeStatus(TInt aState, TButtonData* aDataButton)
{
    // Rocker key down, change skin and refresh for aState > 0.
    if (aState)
    {
        if (iTapTimer)
        {
            if (iTapTimer->IsActive())
            {
                // Stop tap timer
                iTapTimer->Cancel();
            }

            iOldTapCnt = iTapCnt;

            // Set button down and refresh recker keys area when tap cunter is greater
            // then KTapMaxCnt. Tap counter increases every KTapTimerExpiryInterval.
            if (iTapCnt >  KTapMaxCnt || iTapCnt == -1)
            {
                aDataButton->iButton->SetCurrentState(1, ETrue);
            }

            iTapCnt = 0;

            // Start timer for evry rocker key down with KTapTimerExpiryInterval time interval.
            iTapTimer->Start(KFirstTapTimerExpiryInterval, KTapTimerExpiryInterval,
                             TCallBack(TapTimerCallbackL, this));
        }
    }
    // Rocker key up, change skin and refresh for aState == 0
    else
    {
        // Set button up and refresh recker keys area when tap cunter
        // is greater then KTapMaxCnt
        if (iOldTapCnt >  KTapMaxCnt || iOldTapCnt == -1)
        {
            aDataButton->iButton->SetCurrentState(0, ETrue);
        }
    }
}

// Tap timer callback method
TInt CMIDCanvasKeypad::TapTimerCallbackL(TAny* aThis)
{
    if (aThis)
    {
        CMIDCanvasKeypad* observer = static_cast<CMIDCanvasKeypad*>(aThis);
        observer->HandleTapTimerEventL();
    }

    return 0;
}

// Counter of mouse tap events EEventKeyDown, increased  every KTapTimerExpiryInterval
// time interval. When iTapCnt > KTapMaxCnt timer was stopped.
void CMIDCanvasKeypad::HandleTapTimerEventL()
{
    ++iTapCnt;

    if (iTapTimer && iTapCnt > KTapMaxCnt)
    {
        iTapTimer->Cancel();
    }
}
// Send EButton1Up event and stoped repeat timer if OSK in background
void CMIDCanvasKeypad::OSKInBackground(TBool aOSKInBackground)
{
    if (aOSKInBackground)
    {
        iStoreKeyDownEvent.iType = TPointerEvent::EButton1Up;

        TRAPD(error, HandlePointerEventL(iStoreKeyDownEvent));
        if (error != KErrNone)
        {
            ResetButtons();
        }
    }

    iOSKInBackground = aOSKInBackground;
}
// End of File
