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
#ifndef CAMMSAUDIOOUTPUTCONTROL_H
#define CAMMSAUDIOOUTPUTCONTROL_H

//  INCLUDES
#include "cammscontrol.h"
#include <cmmamidiplayer.h>
#include <mmmaplayerstatelistener.h>
// for audio routing control
#include <AudioOutput.h>

// for audio routing observers
#include <MAudioOutputObserver.h>
#include <AccMonitor.h>
#include <AccMonitorInfo.h>




// CONSTANTS
_LIT(KAMMSAudioOutputControl, "AudioOutputControl");
// CLASS DECLARATION

/**
*
*  Controls for the Audio Routing.
*  This class delegates Audio Routing setting calls to RMMFController.
*
*
*  @since X.X
*/
NONSHARABLE_CLASS(CAMMSAudioOutputControl)
        : public CAMMSControl, public MMMAPlayerStateListener ,public MAudioOutputObserver,public MAccMonitorObserver
{
public:
    // Constructors and destructor
    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSAudioOutputControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSAudioOutputControl();

public:
    // New functions
    /**
    * Sets the AudioOutput.
    *
    * @param aPreference The new AudioOutput to be set.
    */
    TInt SetAudioOutput(TInt aPreference);

    /**
    * Gets the Audio Routing preference.
    *
    * @return The previously set Audio Routing preference.
    */
    TInt GetAudioOutput();
    /**
    * Reset the AudioOutput java object with changed value of current
    */
    void GetCurrentPreference();
    /**
    * Gets the Audio Routing preference.
    *
    * @return The previously set Audio Routing preference. and if not set retruns the device default
    */

    TInt GetCurrentPrefInt();
    /**
    * Gets the Audio Routing Device Default.
    *
    * @returns the device default
    */
    TInt GetDeviceDefaultPreference();
    /**
    * Create an Observer to get the notification for the Headset state.
    * Also initializes iCurrentActualPreference with CAudioOutput::EPrivate if headset is connected.
    * @returns the device default
    */
    void CreateHeadsetStateObserverL();
    /**
    * Actual native control is created using this function
    */
    void CreateNativeAudioOutputControlL();
    /**
    * seting java object used to send as callback event
    */
    void SetJavaAudioOutputObject(jobject object);
    /**
    * Reset the value of java object with current values
    */
    void ResetJavaAudioOutputObject();
    /**
    * Check if there is any change in Current output Mode,It sends a event to java
    * If there is no change it returns silently
    */
    void NotifyJavaOnChange();


public:
    // From MMMAPlayerStateListener
    void StateChanged(TInt aState);


public:
    // From MAudioOutputObserver
    void DefaultAudioOutputChanged(CAudioOutput& aAudioOutput,  CAudioOutput::TAudioOutputPreference aNewDefault);
    void DisconnectedL(CAccMonitorInfo *aAccessoryInfo);
    void ConnectedL(CAccMonitorInfo* aAccessoryInfo);
    void AccMonitorObserverError(TInt aError);

public:
    // Functions from base classes
    const TDesC& ClassName() const;

protected:
    // New functions
    /**
    * @param aAmmsPreference AMMS preference to be set.
    */
    TInt SetAudioOutputToMmf(CAudioOutput::TAudioOutputPreference aAmmsPreference);

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSAudioOutputControl(CMMAPlayer* aPlayer);

    /**
    * Symbian 2nd phase constructor.
    */
    void ConstructL();

private:
    //Data
    // Preference visible in AMMS.
    CAudioOutput::TAudioOutputPreference iRoutingUserPreference;
    // Preference of the device changes by inserting/removing Jack.
    CAudioOutput::TAudioOutputPreference iDefaultDevicePreference;
    // Preference of the device changes by inserting/removing Jack.
    TInt iCurrentPreference;
    // owned
    CAudioOutput* iAudioOutput;
    CMMAPlayer::TPlayerState playerState;
    //To get the peripheral attached/detached notification
    CAccMonitorInfo* iAccessoryInfo;
    RAccMonCapabilityArray capabilityArray;
    CAccMonitor *iAccMonitor;
    RConnectedAccessories array;
    jobject iJavaAudioOutputObj;
    JNIEnv* iJni;

};

#endif // CAMMSAUDIOOUTPUTCONTROL_H


