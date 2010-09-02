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
#include <logger.h>
#include "cammsaudiooutputcontrol.h"
#include <cmmaplayerevent.h>
#include <cmmaaudioplayer.h>
#include <cmmavideoplayer.h>
#include <mmf/common/mmfcontroller.h>
#include <midiclientutility.h>
#include <cammscustomcommandutility.h>
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
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::~");
    if (iAudioOutput)
    {
        iAudioOutput->UnregisterObserver(*this);
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
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::SetJavaAudioOutputObject ");
    iJavaAudioOutputObj = object;
    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
  //  iJni = JniEnvWrapper::GetValidJniRef();
}

void CAMMSAudioOutputControl::ResetJavaAudioOutputObject()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::ResetJavaAudioOutputObject ");
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
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::SetAudioOutputL %d", aPreference);

    TInt temppreference = 0;
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::SetAudioOutputL  After __ASSERT_DEBUG");
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

    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::SetAudioOutputL - = %d", temppreference);
    return temppreference;

}

// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::GetAudioOutput
// Gets the preference.
// -----------------------------------------------------------------------------
TInt CAMMSAudioOutputControl::GetAudioOutput()
{
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::GetAudioOutput %d", (TInt)iRoutingUserPreference);
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
        LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::GetCurrentPrefInt_if %d", (TInt)iDefaultDevicePreference);
        pref = GetDeviceDefaultPreference();
    }
    else
    {
        LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::GetCurrentPrefInt_else %d", (TInt)iRoutingUserPreference);
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
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::GetDeviceDefaultPreference %d", (TInt)iDefaultDevicePreference);
    array.Reset();
    TInt temp = 0;
    TRAPD(err,iAccMonitor->GetConnectedAccessoriesL(array));
    if (err)
    {
        temp = -1;
    }
    TInt count = array.Count();
    if (count == 0)
    {
        temp = (TInt)CAudioOutput::EPublic;
    }
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::GetDeviceDefaultPreference :RConnectedAccessories count = %d",count);
    for (TInt i = 0; i != count; i++)
    {
        TAccMonCapability deviceType = array[ i ]->AccDeviceType();
        if ((deviceType == KAccMonHeadset)||(deviceType == KAccMonBluetooth))
        {
            LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::CreateHeadsetStateObserverL info = Headset Connected  ");
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
    LOG1( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::StateChanged +, state = %d",  aState);
    playerState = (CMMAPlayer::TPlayerState)aState;
    if (aState == CMMAPlayer::EStarted)
    {
        NotifyJavaOnChange();
    }
}

const TDesC& CAMMSAudioOutputControl::ClassName() const
{
    LOG( EJavaAMMS, EInfo, "CAMMSAudioOutputControl::ClassName");
    return KAMMSAudioOutputControl;
}

// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::SetPriorityToMmf
// Scales the given AMMS priority to MMF priority and sets it to MMF.
// -----------------------------------------------------------------------------
TInt CAMMSAudioOutputControl::SetAudioOutputToMmf(CAudioOutput::TAudioOutputPreference aPref)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::SetAudioOutputToMmfL +");
    CAudioOutput::TAudioOutputPreference tempPreference = iRoutingUserPreference ;
    iRoutingUserPreference = aPref;
    TRAPD(err,CreateNativeAudioOutputControlL();
          iAudioOutput->SetAudioOutputL(aPref));
    if (KErrNone != err)
    {
        iRoutingUserPreference = tempPreference;
        TBuf<KEventMessageSize> errorMessage;
        errorMessage.Format(KErrAudioOutputControlError, err);
        iPlayer->PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
    }
    // if during play user set a preference event should be sent to java
    if (playerState == CMMAPlayer::EStarted)
    {
        NotifyJavaOnChange();
    }
    return (TInt)iRoutingUserPreference;
}
// -----------------------------------------------------------------------------
// CAMMSAudioOutputControl::DefaultAudioOutputChanged
// MAudioOutputObserver's function is implemented to notify about the change in routing preference
// -----------------------------------------------------------------------------

void CAMMSAudioOutputControl::DefaultAudioOutputChanged(CAudioOutput& /*aAudioOutput*/,
        CAudioOutput::TAudioOutputPreference /*aNewDefault*/)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::DefaultAudioOutputChanged ");

}

void CAMMSAudioOutputControl::NotifyJavaOnChange()
{
    TInt tempPref = GetCurrentPrefInt();
    if (iCurrentPreference == tempPref)
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::NotifyJavaOnChange - No Event ");
        return;
    }
    //reset the java side object with the current iCurrentActualPreference
    ResetJavaAudioOutputObject();
    iCurrentPreference = tempPref;
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::NotifyJavaOnChange - Sending Event ");
    iPlayer->PostObjectEvent(CMMAPlayerEvent::EAudioOutputPreferenceChangeEvent, iJavaAudioOutputObj);
}

void CAMMSAudioOutputControl::AccMonitorObserverError(TInt /*aError*/)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::AccMonitorObserverError");
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
{    // Reserve memory for the accessory information instance if necessary
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::ConnectedL +");
    if (!iAccessoryInfo)
    {
        iAccessoryInfo = CAccMonitorInfo::NewL();
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::ConnectedL iAccessoryInfo created");
    }
    // Otherwise just reset accessory information instance
    else
    {
        iAccessoryInfo->Reset();
    }
    iAccessoryInfo->CopyL(aAccessoryInfo);
    TAccMonCapability deviceType = iAccessoryInfo->AccDeviceType() ;
    if ((deviceType == KAccMonHeadset) || (deviceType == KAccMonBluetooth))
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::DisconnectedL: Headset connected");
        //send a callback
        if (iRoutingUserPreference == (TInt)(CAudioOutput::ENoPreference))
        {
            NotifyJavaOnChange();
        }
    }
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::ConnectedL -");
}


void CAMMSAudioOutputControl::DisconnectedL(CAccMonitorInfo*  aAccessoryInfo)
{   // Reserve memory for the accessory information instance if necessary

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::DisconnectedL +");
    if (!iAccessoryInfo)
    {
        iAccessoryInfo = CAccMonitorInfo::NewL();
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::DisconnectedL: iAccessoryInfo created");
    }
    else
    {
        iAccessoryInfo->Reset();
    }
    iAccessoryInfo->CopyL(aAccessoryInfo);
    TAccMonCapability deviceType = iAccessoryInfo->AccDeviceType();
    if ((deviceType == KAccMonHeadset)||(deviceType == KAccMonBluetooth))
    {
        LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::DisconnectedL: Headset Disconnected");
        //send a callback
        if (iRoutingUserPreference == (TInt)(CAudioOutput::ENoPreference))
        {
            NotifyJavaOnChange();
        }
    }
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::DisconnectedL -");
}

// start observing headset connection disconnection

void CAMMSAudioOutputControl::CreateHeadsetStateObserverL()
{
    // Headset connection and disconnection
    iAccessoryInfo = NULL;
    capabilityArray.Append(KAccMonHeadset);
    capabilityArray.Append(KAccMonBluetooth);

    iAccMonitor = CAccMonitor::NewL();
    iDefaultDevicePreference = (CAudioOutput::TAudioOutputPreference)GetDeviceDefaultPreference();
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
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSAudioOutputControl::ConstructL +");
    // create an observer to notify the state of headset
    //and initialize iDefaultDevicePreference with CAudioOutput::EPrivate if headset is connected.
    CreateHeadsetStateObserverL();
    iPlayer->AddStateListenerL(this);
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
    
    if(iAudioOutput)
    {
   		iAudioOutput->RegisterObserverL(*this);
  	}
    else
    {
    	User::Leave(KErrNotSupported);
    }	
}


//  End of File
