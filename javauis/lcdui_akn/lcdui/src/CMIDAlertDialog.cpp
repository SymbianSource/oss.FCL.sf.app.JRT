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
* Description:  Alert dialog implementation for Series 60
*
*/


#include "CMIDAlert.h"
#include "CMIDAlertDialog.h"
#include "CMIDDisplayable.h"
#include "CMIDCommandList.h"
#include "CMIDUtils.h"

// using TAlertDialogLineId enumeration
#include "lcdui.hrh"
// macros definitions for resources
#include <lcdui.rsg>

// using CAknBitmapAnimation and CBitmapAnimClientData
// - in CheckAndSetAnimationModeL and UpdateGaugeInAlertL functions
// for animation in gauge
#include <aknnotecontrol.h>
#include <eikprogi.h>
// using CEikCaptionedControl
// - in CheckAndSetAnimationModeL and UpdateGaugeInAlertL functions
#include <eikcapc.h>

#include <AknDef.h>
#include <aknappui.h>

// macros definitions for outputing logs
#include <j2me/jdebug.h>

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK


/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDDisplayable.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);


/** CMIDAlertDialog::CMIDAlertDialog */
CMIDAlertDialog* CMIDAlertDialog::NewL(CMIDAlert* aAlert,CMIDDisplayable* aDisplayable)
{
    CMIDAlertDialog* self = new(ELeave) CMIDAlertDialog(aAlert, aDisplayable);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}
//
CMIDAlertDialog::CMIDAlertDialog(CMIDAlert* aAlert, CMIDDisplayable* aDisplayable) :
        iParent(aAlert), iDisplayable(aDisplayable)
{
    iGaugeResourceType = EPopupFormUnknown;

}

/** CMIDAlertDialog::~CMIDAlertDialog */
CMIDAlertDialog::~CMIDAlertDialog()
{
    if (iGauge)
    {
        iGauge->SetGaugeListenerFromAlert(NULL);
        iGauge = NULL;
    }
}

/** CMIDAlertDialog::PreLayoutDynInitL */
void CMIDAlertDialog::PreLayoutDynInitL()
{
    // Check if there is a gauge.... and create it...
    // NOTE: the type and properties need to be set here also...
    CAknPopupForm::PreLayoutDynInitL();
}

/** When showing dialogs set their CBA into CMIDDisplayable and then use
the logic of CMIDDisplayable::InitializeCbasL() to set the correct CBA
commands. However modeless dialogs always have empty CBAs. When not
showing we must reset the parent CBAs.
*/
void CMIDAlertDialog::UpdateCbasL()
{
    if (iShowing)
    {
        iDisplayable->SetCba(&ButtonGroupContainer());

        if (iParent->IsModal())
        {
            iDisplayable->InitializeCbasL();
        }
        else
        {
            ButtonGroupContainer().SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY);
        }

        CEikCba* cba = static_cast<CEikCba*>(ButtonGroupContainer().ButtonGroup()->AsControl());
        if (cba)
        {
            cba->DrawableWindow()->SetOrdinalPosition(0);
#ifdef RD_JAVA_S60_RELEASE_9_2
            cba->EnableItemSpecificSoftkey(EFalse);
#endif // RD_JAVA_S60_RELEASE_9_2
            cba->MakeVisible(ETrue);
            cba->DrawNow();
        }
    }
    else
    {
        //restore parent CBAs
        iDisplayable->SetCba(iDisplayable->MenuHandler()->Cba());
    }
}

/** CMIDAlertDialog::PostLayoutDynInitL */
void CMIDAlertDialog::PostLayoutDynInitL()
{
    CAknPopupForm::PostLayoutDynInitL();
    // Above call is starting the animation. In case of updating gauge
    // we will stop it and midlet will update it later.
    CheckAndSetAnimationModeL();
}

/** CMIDAlertDialog::InsertGaugeIntoDialogL
 Insert/remove the gauge in Alert */
void CMIDAlertDialog::InsertGaugeIntoDialogL()
{
    DEBUG("CMIDAlertDialog::InsertGaugeIntoDialogL <");

    TBool skipVisibilityChange = EFalse;

    // skipping disabling visibility change in case of error
    // where incremental update type gauge wasn't show
    if (iGauge)
    {
        if (iGauge->GetIndefiniteState() == MMIDGauge::EIncrementalUpdating
                && iShowing == EFalse)
        {
            skipVisibilityChange = ETrue;
        }
    }

    if (!skipVisibilityChange)
    {
        DisableVisibilityChange(ETrue);
    }


    TInt gaugeVal = iGauge ? iGauge->GetValue() : -1;

    //remove any existing gauge
    if (iGaugeResourceType != EPopupFormUnknown)
    {
        DeleteControl(iGaugeResourceType);
        iGaugeResourceType = EPopupFormUnknown;
    }

    //add the new gauge
    if (iGauge)
    {
        iGauge->SetGaugeListenerFromAlert(this);
        DEBUG("CMIDAlertDialog::InsertGaugeIntoDialogL - InsertControlL()");

        if (iGauge->GetMaxValue() > 0)
        { // definite type Gauge
            // progressbar adding
            InsertControlL(0, R_MIDP_ALERT_PROGI_LINE, EFalse);
            iGaugeResourceType = EPopupFormProgbar;
            iGauge->UpdateProgressInfo(CAknPopupForm::ProgressInfo());
        }
        else
        {
            iGaugeResourceType = EPopupFormWaitAnim;
            switch (iGauge->GetIndefiniteState())
            {
            case MMIDGauge::EContinuousIdle: // continuous idle
            {   //r_midp_alert_wait_animation_continouse_idle_line
                InsertControlL(0, R_MIDP_ALERT_WAIT_ANIMATION_CONTINUOUS_IDLE_LINE, ETrue);
                break;
            }
            case MMIDGauge::EContinuousRunning: // continuous running
            {
                InsertControlL(0, R_MIDP_ALERT_WAIT_ANIMATION_CONTINUOUS_RUNNING_LINE, ETrue);
                break;
            }
            case MMIDGauge::EIncrementalUpdating: //incremental updating
            {
                InsertControlL(0, R_MIDP_ALERT_WAIT_BAR_INCREM_LINE, ETrue);
                break;
            }
            case MMIDGauge::EIncrementalIdle: //incremental idle
            {
                InsertControlL(0, R_MIDP_ALERT_WAIT_BAR_INCREM_IDLE_LINE, ETrue);
                break;
            }
            default:
                ASSERT(EFalse);
                break;
            }
        }
    }

    DEBUG("CMIDAlertDialog::InsertGaugeIntoDialogL - UpdateGaugeInAlertL()");

    UpdateGaugeInAlertL(gaugeVal);

    // skipping enabling visibility change in case of error
    // where incremental update type gauge wasn't show
    if (!skipVisibilityChange)
    {
        DisableVisibilityChange(EFalse);
    }

    DEBUG("CMIDAlertDialog::InsertGaugeIntoDialogL >");
}

/** CMIDAlertDialog::CheckAndSetAnimationMode */
void CMIDAlertDialog::CheckAndSetAnimationModeL()
{
    DEBUG("CMIDAlertDialog::CheckAndSetAnimationModeL <");

    TBitFlags flags = Flags();
    if (iGauge && (iGauge->GetIndefiniteState() == MMIDGauge::EIncrementalUpdating)
            && flags.IsSet(EPopupFormWaitAnim))
    {
        CCoeControl* coe = GetControlByControlTypeL(EPopupFormWaitAnim)->iControl;
        CAknBitmapAnimation* ani = REINTERPRET_CAST(CAknBitmapAnimation*, coe);


        DEBUG("CMIDAlertDialog::CheckAndSetAnimationModeL - CancelAnimation()");
        ani->CancelAnimation();

        CBitmapAnimClientData* animClientData = ani->BitmapAnimData();
        iGaugeFrameCount = animClientData->FrameArray().Count();

        // A non-interactive Gauge that has indefinite range will exist in one of four states:
        // continuous-idle, incremental-idle, continuous-running, or incremental-updating.
        // These states are intended to indicate to the user that some level of activity is
        // occurring. With incremental-updating, progress can be indicated to the user even
        // though there is no known endpoint to the activity.
        if (iGauge->GetMaxValue() < 0)
        { //CBitmapAnimClientData::TPlayMode::
            DEBUG("CMIDAlertDialog::CheckAndSetAnimationModeL - SetPlayModeL()");
            ani->Animation().SetPlayModeL(CBitmapAnimClientData::ECycle);
            iIncrementalFrameCounter = 0;
            DEBUG("CMIDAlertDialog::CheckAndSetAnimationModeL - SetFrameIndexL()");
            ani->SetFrameIndexL(iIncrementalFrameCounter++);
        }
    }
    DEBUG("CMIDAlertDialog::CheckAndSetAnimationModeL >");
}


/**
*/
void CMIDAlertDialog::UpdateGaugeInAlertL(TInt aValue)
{
    if (iGauge && aValue >= 0 &&
            (iGauge->GetIndefiniteState() == MMIDGauge::EIncrementalUpdating ||
             iGauge->GetMaxValue() > 0))
    {
        TBitFlags flags = Flags();
        if (flags.IsSet(EPopupFormWaitAnim))
        {
            CCoeControl* coe = GetControlByControlTypeL(EPopupFormWaitAnim)->iControl;
            CAknBitmapAnimation* ani = REINTERPRET_CAST(CAknBitmapAnimation*, coe);
            if (iGaugeFrameCount < 1)
                iGaugeFrameCount = 1;
            ani->SetFrameIndexL(iIncrementalFrameCounter++ % iGaugeFrameCount);
        }
        else
        {
            // draw and update the progress bar
            SetProgressBarAndDraw(aValue);
            //draw of Alert
            DrawDeferred();
        }
    }
}


void CMIDAlertDialog::GaugeTypeInAlertChangedL()
{
    // create new gauge type in wait dialog
    InsertGaugeIntoDialogL();
    CheckAndSetAnimationModeL();

    // redraw gauge
    DrawDeferred();
}

/** If the alert is timed (not modal) and if we receive a valid key event then
we dismiss the dialog. Otherwise we pass on the key event to CAknPopupForm,
which will take care of scrolling text for example. @see CMIDUtils::IgnoreKeyEvent()
for key events that are ignored (eg. flip open or flip close key events. */
TKeyResponse CMIDAlertDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    //DEBUG START
    if (aType == EEventKeyDown)
    {
        DEBUG_INT2("CMIDAlertDialog::OfferKeyEventL - key down, scan code %d key code %d",
                   aKeyEvent.iScanCode, aKeyEvent.iCode);
    }
    else if (aType == EEventKey)
    {
        DEBUG_INT2("CMIDAlertDialog::OfferKeyEventL - key event, scan code %d key code %d",
                   aKeyEvent.iScanCode, aKeyEvent.iCode);
    }
    else if (aType == EEventKeyUp)
    {
        DEBUG_INT2("CMIDAlertDialog::OfferKeyEventL - key up, scan code %d key code %d",
                   aKeyEvent.iScanCode, aKeyEvent.iCode);
    }
    //DEBUG END

    if (!iParent->IsModal() && (aType == EEventKey) && !CMIDUtils::IgnoreKeyEvent(aKeyEvent.iCode))
    { // if not modal then pressing any valid key should dismiss it
        iParent->TryToDismissDialog();
        return EKeyWasConsumed;
    }
    //Enter_Key have a same function as Selection key.
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iParent->IsModal() && (aType == EEventKey) && (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyDevice3))
#else
    if (iParent->IsModal() && (aType == EEventKey) &&   aKeyEvent.iCode == EKeyEnter)
#endif // RD_JAVA_S60_RELEASE_9_2
    {
        OkToExitL(iDisplayable->MainCommandList()->CommandOffset());
        return EKeyWasConsumed;
    }
    return CAknPopupForm::OfferKeyEventL(aKeyEvent, aType);
}

/** Modal dialogs must let the displayable handle commands whereas modeless
dialogs can just exit on any key (including a CBA key). However we cannot
exit directly, we must post a java side event and wait for the java request
to dismiss the dialog. This is why we return EFalse. */
TBool CMIDAlertDialog::OkToExitL(TInt aCommandId)
{
    if (iParent->IsModal())
    {
        TInt offset = iDisplayable->MainCommandList()->CommandOffset();

        if (aCommandId == EAknSoftkeyOptions)   // 3000
        {
            iDisplayable->MenuHandler()->ProcessCommandL(EAknSoftkeyOptions);
        }
        else if (aCommandId - offset < 0)   // aCommandId value is over 3000 - used by MSK
        {
            iDisplayable->ProcessCommandL(aCommandId);
        }
        else // aCommandId value is 7000 and over - used by LSK, RSK and EnterKey
        {
            CMIDCommand* tempCmd = NULL;
            CMIDCommand* command = NULL;
            TBool isMaped = EFalse;
            TInt mapedCnt = ENone;
            TInt mapedId = ENone;

            // number of all commands
            TInt numberOfCommands = iDisplayable->CommandCount();
            // number of Ok and Item commands
            TInt numberOfOkOptionsCommands = iDisplayable->NumCommandsForOkOptionsMenu();
            // command ID which is get from aCommandId
            TInt commandId = aCommandId - offset;

            if (numberOfCommands > 0)
            {
                // Go through every midlet command, and save its ID if maped
                // and also count maped commands.
                for (TInt i = 0; i < numberOfCommands; i++)
                {
                    tempCmd = iDisplayable->MainCommandList()->At(i).iCommand;
                    if (tempCmd->IsMappedToSoftKey())
                    {
                        mapedId = i;
                        isMaped = ETrue;
                        mapedCnt++;
                    }
                }

                // If there are more than one maped command (LSK & RSK maped)
                // or maped command is different than commandId (EnterKey used),
                // set CMIDCommand command with this command, which index
                // is the same as commandId value.
                if (mapedCnt > 1 || mapedId != commandId)
                {
                    command = iDisplayable->MainCommandList()->At(commandId).iCommand;
                }
                else // Else if there are one or zero command maped.
                {
                    if (isMaped)   // One command maped.
                    {
                        command = iDisplayable->MainCommandList()->At(mapedId).iCommand;
                    }
                    else // Zero commands maped.
                    {
                        command = iDisplayable->MainCommandList()->At(0).iCommand;
                    }
                }

                // type of the command
                TInt commandType = command->CommandType();

                // If there aren't any EExit, EBack, ECancel and EStop
                // command mapped and there are still some commands
                // show menu, else process with maped command.
                if (commandType != MMIDCommand::EExit &&
                        commandType != MMIDCommand::EBack &&
                        commandType != MMIDCommand::ECancel &&
                        commandType != MMIDCommand::EStop &&
                        numberOfOkOptionsCommands > 1)
                {
                    iDisplayable->MenuHandler()->ShowMenuL(CMIDMenuHandler::EOkMenu);
                }
                else
                {
                    iDisplayable->ProcessCommandL(aCommandId);
                }
            }
        }
    }
    else
    {
        iParent->TryToDismissDialog();
    }

    //The dialog can only be dismissed java side or else we'll end
    //up with the alert displayed and no dialog
    return EFalse;
}

/** */
void CMIDAlertDialog::SetGaugeL(CMIDGaugeItem* aGauge)
{
    DEBUG("CMIDAlertDialog::SetGaugeL <");

    if (iGauge)
    {
        iGauge->SetGaugeListenerFromAlert(NULL);
    }
    iGauge = (CMIDNonInteractiveGauge*)(aGauge);

    DEBUG("CMIDAlertDialog::SetGaugeL - InsertGaugeIntoDialogL");
    InsertGaugeIntoDialogL();

    DEBUG("CMIDAlertDialog::SetGaugeL - CheckAndSetAnimationModeL");
    CheckAndSetAnimationModeL();

    DrawDeferred();
    DEBUG("CMIDAlertDialog::SetGaugeL >");
}

/** Show a pre-created dialog. Dialogs are created as soon
as Alerts are created java side. This is not necessarily when
the dialog must be displayed. */
void CMIDAlertDialog::ShowL(TBool aShow)
{
    if (aShow != iShowing)
    {
        if (aShow)
        {
            iShowing = ETrue;
            RouseSleepingDialog();

#ifdef RD_SCALABLE_UI_V2
            SetPointerCapture(ETrue);
#endif
        }
        else
        {
            ExitSleepingDialog();

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            if (iButtonDownReceived)
            {
                //do feedback if alert was closed by tapping
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if (feedback)
                {
                    feedback->InstantFeedback(ETouchFeedbackPopUp);
                }
            }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

#ifdef RD_SCALABLE_UI_V2
            SetPointerCapture(EFalse);
            iButtonDownReceived = EFalse;
#endif
            iShowing = EFalse; //attention: iShowing must be true when dismissing
            // a dialog or else MakeVisible() won't execute and
            // we won't update the fader's stack, @see MakeVisible()
        }

        UpdateCbasL();
    }
}

/** Because CAknPopupForm calls MakeVisible(ETrue) in several
places, including LayoutAndDraw() and because we don't want to
become visible when we are not showing, we had to override the
MakeVisible() method and pass on the call only when we are
showing. Also, CMIDAlert can disable temporarily the MakeVisible
calls by setting iVisibilityChangeDisabled via DisableVisibilityChange().
This is because when we call SetTitle() or SetImage() we must insert
or remove controls into the dialog and popup form does MakeVisible(EFalse)
and then MakeVisible(ETrue) after the control has been added or removed.
This results in an unpleasant appearing and disappearing of the dialog
because we add or remove more than one control. */
void CMIDAlertDialog::MakeVisible(TBool aVisible)
{
    if (iShowing && !iVisibilityChangeDisabled)
    {
        if (aVisible)
        {
            //re-layouting done just before dialog is shown
            //to be sure layout is correct in CAknPopupForm
            CAknPopupForm::SizeChanged();
        }
        CAknPopupForm::MakeVisible(aVisible);
    }
}


#ifdef RD_SCALABLE_UI_V2
/**
 * Closes timed alert on EButton1Up event. Detecting long taps is done through
 * CMIDAlert::TryDetectLongTapL.
 */
void CMIDAlertDialog::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
        // Long tap may be canceled by dragging pointer out of alert area,
        // need to make sure that events are forwarded to long tap detector in this case.
        if ((AlertRegionContains(aPointerEvent.iPosition) ||
                aPointerEvent.iType != TPointerEvent::EButton1Down) &&
                iParent->TryDetectLongTapL(aPointerEvent))
        {
            return;
        }

        switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
            iButtonDownReceived = ETrue;
            break;
        case TPointerEvent::EButton1Up:
            if (!iParent->IsModal() && iButtonDownReceived)
            {
                iParent->TryToDismissDialog();
            }
            break;
        case TPointerEvent::EDrag:
            break;
        default:
            break;
        }

    }
}
#endif


/** Disable the MakeVisible() calls. See @MakeVisible() */
void CMIDAlertDialog::DisableVisibilityChange(TBool aDisable)
{
    iVisibilityChangeDisabled = aDisable;
}

/** We disable unnecessary layout operation in an attempt to optimise because of
    defect OULM-6K4C2X. This would not be needed if CAknPopupForm::InsertControlL()
    would not call LayoutAndDraw() every time and also CAknPopupForm::SetSizeAndPosition()
    results in the execution of SizeChanged() 3 times. So a lot of layout operations get
    in the way of performance. */
void CMIDAlertDialog::SizeChanged()
{
    if (!iVisibilityChangeDisabled || iShowing)
    {
        CAknPopupForm::SizeChanged();
    }
}

void CMIDAlertDialog::HandleResourceChange(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        LayoutAndDraw();

        TRAP_IGNORE(InsertGaugeIntoDialogL(); CheckAndSetAnimationModeL(););

        DrawDeferred();
    }
    else
    {
        CAknPopupForm::HandleResourceChange(aType);
    }
}

#ifdef RD_SCALABLE_UI_V2
TBool CMIDAlertDialog::AlertRegionContains(const TPoint& aPoint)
{
    return (aPoint.iX >= 0 && aPoint.iX <= iSize.iWidth &&
            aPoint.iY >= 0 && aPoint.iY <= iSize.iHeight);
}
#endif

// End of File
