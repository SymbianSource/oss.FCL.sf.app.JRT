/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manipulates the audio output mode.
*
*/

// INCLUDE FILES
#include <AudioPreference.h>
#include <jdebug.h>
#include "cammsaudiooutputcontrol.h"
#include "cmmavolumecontrol.h"
#include <cmmaplayerevent.h>
#include <cmmaaudioplayer.h>
#include <cmmavideoplayer.h>
#include <mmfcontroller.h>
#include <midiclientutility.h>
#include <cammscustomcommandutility.h>
#include <JniEnvWrapper.h>
// CONSTANTS
_LIT(KErrAudioOutputControlError, "AMMS AudioOutputControl error: %d");
const TInt KEventMessageSize = 64;
const TInt KNoPriference = 0;
const TInt KAllSpeakers = 1;
const TInt KNoAudioOutput = 2;
const TInt KAudioEarpiece = 3;
const TInt KAudioLoudspeaker = 4;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSAudioOutputControl* CAMMSAudioOutputControl::NewLC(CMMAPlayer* aPlayer)
{
    CAMMSAudioOutputControl* self = new(ELeave) CAMMSAudioOutputControl(aPlayer);

    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// Destructor
CAMMSAudioOutputControl::~CAMMSAudioOutputControl()
{
    DEBUG("AMMS::CAMMSAudioOutputControl::~");
    if (iAudioOutput)
    {
        delete iAudioOutput;
    }
    if (iAccMonitor)
    {
        iAccMonitor ->StopObserving();
        delete iAccMonitor;
    }
}

//set java AudioOutput object
void CAMMSAudioOutputControl::SetJavaAudioOutputObject(jobject object)
{
    DEBUG("AMMS::CAMMSAudioOutputControl::SetJavaAudioOutputObject ");
    iJavaAudioOutputObj = object;
    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    iJni = JniEnvWrapper::GetValidJniRef();
}

void CAMMSAudioOutputControl::ResetJavaAudioOutputObject()
{
    DEBUG("AMMS::CAMMSAudioOutputControl::ResetJavaAudioOutputObject ");
    jmethodID setOutputModeID = iJni->GetMethodID(
                                    iJni->GetObjectClass(iJavaAudioOutputObj),
                                    "setOutputMode",
                                    "(I)V");

    jint jpref = (jint)GetCurrentPrefInt();
    iJni->CallVoidMethod(iJavaAudioOutputObj,setOutputModeID,jpref);
}

// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::SetAudioOutput
// Sets the preference using a linear point scale between 0 and 4.
// -----------------------------------------------------------------------------
//
TInt CAMMSAudioOutputControl::SetAudioOutput(TInt aPreference)
{
    DEBUG_INT("AMMS::CAMMSAudioOutputControl::SetAudioOutputL %d", aPreference);

    TInt temppreference = 0;
    DEBUG("AMMS::CAMMSAudioOutputControl::SetAudioOutputL  After __ASSERT_DEBUG");
    // Set a new preference only if it differs from the previous one.
    if (aPreference != iRoutingUserPreference)
    {
        switch (aPreference)
        {
        case KNoPriference:
        {
            temppreference = SetAudioOutputToMmf(CAudioOutput::ENoPreference);
        }
        break;
        case KAllSpeakers:
        {
            temppreference = SetAudioOutputToMmf(CAudioOutput::EAll);
        }
        break;
        case KNoAudioOutput:
        {
            temppreference = SetAudioOutputToMmf(CAudioOutput::ENoOutput);
        }
        break;
        case KAudioEarpiece:
        {
            temppreference = SetAudioOutputToMmf(CAudioOutput::EPrivate);
        }
        break;
        case KAudioLoudspeaker:
        {
            temppreference = SetAudioOutputToMmf(CAudioOutput::EPublic);
        }
        break;
        default:
            break;
        }
    }

    DEBUG_INT("AMMS::CAMMSAudioOutputControl::SetAudioOutputL - = %d", temppreference);
    return temppreference;

}

// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::GetAudioOutput
// Gets the preference.
// -----------------------------------------------------------------------------
TInt CAMMSAudioOutputControl::GetAudioOutput()
{
    DEBUG_INT("AMMS::CAMMSAudioOutputControl::GetAudioOutput %d", (TInt)iRoutingUserPreference);
    return (TInt)iRoutingUserPreference;
}

// ----------------------------------------------------------------------------------------
// CAMMSAudioOutputControl::GetCurrentPreference
// Gets the preference.if user preference is not set return the current device preference
// ----------------------------------------------------------------------------------------
void CAMMSAudioOutputControl::GetCurrentPreference()
{
    // reset the java AudioOutput object with current mode
    ResetJavaAudioOutputObject();
}

// ----------------------------------------------------------------------------------------
// CAMMSAudioOutputControl::GetCurrentPrefInt
// Gets the preference.if user preference is not set return the current device preference in TInt form
// ----------------------------------------------------------------------------------------

TInt CAMMSAudioOutputControl::GetCurrentPrefInt()
{
    TInt pref ;
    // get the value of current preference
    if (iRoutingUserPreference == CAudioOutput::ENoPreference)
    {
        DEBUG_INT("AMMS::CAMMSAudioOutputControl::GetCurrentPrefInt_if %d", (TInt)iDefaultDevicePreference);
        pref = GetDeviceDefaultPreference();
    }
    else
    {
        DEBUG_INT("AMMS::CAMMSAudioOutputControl::GetCurrentPrefInt_else %d", (TInt)iRoutingUserPreference);
        pref = (TInt)iRoutingUserPreference;
    }
    return pref;
}
// ----------------------------------------------------------------------------------------
// CAMMSAudioOutputControl::GetDeviceDefaultPreference
// Gets the current device preference used as default
// ----------------------------------------------------------------------------------------
TInt CAMMSAudioOutputControl::GetDeviceDefaultPreference()
{
    DEBUG_INT("AMMS::CAMMSAudioOutputControl::GetDeviceDefaultPreference %d", (TInt)iDefaultDevicePreference);
    iArray.Reset();
    TInt temp = 0;
    TRAPD(err,iAccMonitor->GetConnectedAccessoriesL(iArray));
    if (err)
    {
        temp = -1;
    }
    TInt count = iArray.Count();
    if (count == 0)
    {
        temp = (TInt)CAudioOutput::EPublic;
    }
    DEBUG_INT("AMMS::CAMMSAudioOutputControl::GetDeviceDefaultPreference :RConnectedAccessories count = %d",count);
    for (TInt i = 0; i != count; i++)
    {
        TAccMonCapability deviceType = iArray[ i ]->AccDeviceType();
        if (deviceType == KAccMonHeadset)
        {
            DEBUG("AMMS::CAMMSAudioOutputControl::CreateHeadsetStateObserverL info = Headset Connected  ");
            temp = (TInt)CAudioOutput::EPrivate;
            break;
        }

    }
    return temp;
}
// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::StateChanged
// Called when player state is changed.
// -----------------------------------------------------------------------------
void CAMMSAudioOutputControl::StateChanged(TInt aState)
{
    DEBUG_INT("AMMS::CAMMSAudioOutputControl::StateChanged +, state = %d",  aState);
    iPlayerState = (CMMAPlayer::TPlayerState)aState;
    if (aState == CMMAPlayer::EStarted)
    {
        NotifyJavaOnChange();
    }
}

const TDesC& CAMMSAudioOutputControl::ClassName() const
{
    DEBUG("CAMMSAudioOutputControl::ClassName");
    return KAMMSAudioOutputControl;
}

// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::SetPriorityToMmf
// Scales the given AMMS priority to MMF priority and sets it to MMF.
// -----------------------------------------------------------------------------
TInt CAMMSAudioOutputControl::SetAudioOutputToMmf(CAudioOutput::TAudioOutputPreference aPref)
{
    DEBUG("AMMS::CAMMSAudioOutputControl::SetAudioOutputToMmfL +");
    TRAPD(err,CreateNativeAudioOutputControlL();
          iAudioOutput->SetAudioOutputL(aPref));
    if (KErrNone != err)
    {
        TBuf<KEventMessageSize> errorMessage;
        errorMessage.Format(KErrAudioOutputControlError, err);
        iPlayer->PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
        return (TInt)iRoutingUserPreference;
    }
    iRoutingUserPreference = aPref;
    if (iVolumeControl)
    {
        TRAPD(err,
        {
           // If current audio o/p preference is default and Headset or 
           // Bluetooth device is connected or if preference is private then 
           // set the audio preference as private to volume control else set it
           // as public
           if ( (iRoutingUserPreference == (TInt)(CAudioOutput::ENoPreference) &&
               GetDeviceDefaultPreference() == (TInt)(CAudioOutput::EPrivate))||
                iRoutingUserPreference == (TInt)(CAudioOutput::EPrivate))
           {
               iVolumeControl->SetAudioOutputPreferenceL(
                   (TInt)(CAudioOutput::EPrivate));
           }
           else
           {
               iVolumeControl->SetAudioOutputPreferenceL(
                   (TInt)(CAudioOutput::EPublic));
           }
        });
        if (KErrNone > err)
        {
           TBuf<KEventMessageSize> errorMessage;
           errorMessage.Format(KErrAudioOutputControlError, err);
           iPlayer->PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
        }
    }
    // if during play user set a preference event should be sent to java
    if (iPlayerState == CMMAPlayer::EStarted)
    {
        NotifyJavaOnChange();
    }
    return (TInt)iRoutingUserPreference;
}

void CAMMSAudioOutputControl::NotifyJavaOnChange()
{
    TInt tempPref = GetCurrentPrefInt();
    if (iCurrentPreference == tempPref)
    {
        DEBUG("AMMS::CAMMSAudioOutputControl::NotifyJavaOnChange - No Event ");
        return;
    }
    //reset the java side object with the current iCurrentActualPreference
    ResetJavaAudioOutputObject();
    iCurrentPreference = tempPref;
    DEBUG("AMMS::CAMMSAudioOutputControl::NotifyJavaOnChange - Sending Event ");
    iPlayer->PostObjectEvent(CMMAPlayerEvent::EAudioOutputPreferenceChangeEvent, iJavaAudioOutputObj);
}

void CAMMSAudioOutputControl::AccMonitorObserverError(TInt aError)
{
    DEBUG("AMMS::CAMMSAudioOutputControl::AccMonitorObserverError");
    TBuf<KEventMessageSize> errorMessage;
    errorMessage.Format(KErrAudioOutputControlError, aError);
    iPlayer->PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
}
// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::CAMMSAudioOutputControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSAudioOutputControl::CAMMSAudioOutputControl(CMMAPlayer* aPlayer)
        : CAMMSControl(aPlayer),
        iRoutingUserPreference(CAudioOutput::ENoPreference)
{
}
// HEADSET CONNECTED OR NOT
void CAMMSAudioOutputControl::ConnectedL(CAccMonitorInfo* aAccessoryInfo)
{
    DEBUG("AMMS::CAMMSAudioOutputControl::ConnectedL +");
    TAccMonCapability deviceType = aAccessoryInfo->AccDeviceType() ;
    if (deviceType == KAccMonHeadset)
    {
        DEBUG("AMMS::CAMMSAudioOutputControl::DisconnectedL: Headset connected");
        // If audio o/p preference is default set the preference as private
        // to volume control
        if ( iRoutingUserPreference == (TInt)(CAudioOutput::ENoPreference))
        {
            if (iVolumeControl)
            {
                iVolumeControl->SetAudioOutputPreferenceL(
                    (TInt)(CAudioOutput::EPrivate));
            }
        }
        //send a callback
        if (iRoutingUserPreference == (TInt)(CAudioOutput::ENoPreference))
        {
            NotifyJavaOnChange();
        }
    }
    DEBUG("AMMS::CAMMSAudioOutputControl::ConnectedL -");
}


void CAMMSAudioOutputControl::DisconnectedL(CAccMonitorInfo*  aAccessoryInfo)
{
    DEBUG("AMMS::CAMMSAudioOutputControl::DisconnectedL +");
    TAccMonCapability deviceType = aAccessoryInfo->AccDeviceType();
    if (deviceType == KAccMonHeadset)
    {
        DEBUG("AMMS::CAMMSAudioOutputControl::DisconnectedL: Headset Disconnected");
        iArray.Reset();
        iAccMonitor->GetConnectedAccessoriesL(iArray);   
        TInt count = iArray.Count();
        if (count != 0)
        {   
           for (TInt i = 0; i != count; i++)
           {
              TAccMonCapability deviceType = iArray[ i ]->AccDeviceType();
              if (deviceType == KAccMonHeadset)
                 return;
           }
        }
        // If audio o/p preference is default and no accessories is connected or
        // o/p preference is not private then set the preference as public to
        // volume control
        if ( (iRoutingUserPreference == (TInt)(CAudioOutput::ENoPreference) &&
               GetDeviceDefaultPreference() == (TInt)(CAudioOutput::EPublic)) ||
                iRoutingUserPreference != (TInt)(CAudioOutput::EPrivate))
        {
            if (iVolumeControl)
            {
                iVolumeControl->SetAudioOutputPreferenceL(
                    (TInt)(CAudioOutput::EPublic));
            }
        }
        //send a callback
        if (iRoutingUserPreference == (TInt)(CAudioOutput::ENoPreference))
        {
            NotifyJavaOnChange();
        }
    }
    DEBUG("AMMS::CAMMSAudioOutputControl::DisconnectedL -");
}

// start observing headset connection disconnection

void CAMMSAudioOutputControl::CreateHeadsetStateObserverL()
{
    // Headset connection and disconnection
    RAccMonCapabilityArray capabilityArray;
    capabilityArray.Append(KAccMonHeadset);

    iAccMonitor = CAccMonitor::NewL();
    iDefaultDevicePreference = (CAudioOutput::TAudioOutputPreference)
                               GetDeviceDefaultPreference();
    iCurrentPreference = (TInt)iDefaultDevicePreference;
    TBool isObserving = iAccMonitor->IsObserving();
    if (!isObserving)
    {
        iAccMonitor->StartObservingL(this, capabilityArray);
    }
}

// end CreateHeadsetStateObserver

// -----------------------------------------------------------------------------
// CAMMSPlayerStateListener::ConstructL
// 2nd phase constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudioOutputControl::ConstructL()
{
    DEBUG("AMMS::CAMMSAudioOutputControl::ConstructL +");
    // create an observer to notify the state of headset
    // and initialize iDefaultDevicePreference with CAudioOutput::EPrivate
    // if headset is connected.
    CreateHeadsetStateObserverL();
    iPlayer->AddStateListenerL(this);
    // set the current preference to volume control
    iVolumeControl = GetVolumeControl();
    if (iVolumeControl)
    {
        iVolumeControl->SetAudioOutputPreferenceL(iCurrentPreference);
    }

}

CMMAVolumeControl* CAMMSAudioOutputControl::GetVolumeControl()
{
    // loop through all the available controls of player and return
    // volume control if found else return null.
    TInt count(iPlayer->ControlCount());
    for (TInt i(0); i < count; i++)
    {
        CMMAControl* control = iPlayer->Control(i);
        // Check that if this control supports volume control
        if (control->ClassName() == KMMAVolumeControlName)
        {
            return static_cast< CMMAVolumeControl* >(control);
        }
    }
    return 0;
}

void CAMMSAudioOutputControl::CreateNativeAudioOutputControlL()
{
    if (iAudioOutput)
    {
        return;
    }

    if (iPlayer->Type() == KMMAMIDIPlayer)
    {
        CMMAMIDIPlayer* mmaMIDIPlayer =
            reinterpret_cast< CMMAMIDIPlayer* >(iPlayer);
        iAudioOutput = CAudioOutput::NewL(*(mmaMIDIPlayer->MidiClient()));
    }
    else if (iPlayer->Type() == KMMAAudioPlayer)
    {
        MCustomCommand* customCommandUtility = (MCustomCommand *)CreateCustomCommandUtilityL();
        // Create the CAudioOutput Object to handle the audio routing
        iAudioOutput = CAudioOutput::NewL(*customCommandUtility);
    }
    else if (iPlayer->Type() == KMMAVideoPlayer)
    {
        CMMAVideoPlayer* mmaVideoPlayer =
            reinterpret_cast< CMMAVideoPlayer* >(iPlayer);
        RMMFController& mmfController = mmaVideoPlayer->Controller();
        MCustomCommand* customCommandUtility =
            CAMMSCustomCommandUtility::NewL(mmfController);
        iAudioOutput = CAudioOutput::NewL(*customCommandUtility);
    }
    else
    {
			  User::Leave(KErrNotSupported);
	  }
}


//  End of File
