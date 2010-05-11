/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for Tuner component.
*
*/


#ifndef TUNER_H
#define TUNER_H

#include <e32base.h>
#include <MCustomCommand.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfaudio.h>
#include <Mda/Common/audio.h>
class RRadioSession;

/**
@file
@publishedAll
@prototype
*/
/**
The Tuner Capabilities structure defines the capabilities of the tuner
on the device, as retrieved using the function GetCapabilities.
*/
class TTunerCapabilities
{
public:
    inline TTunerCapabilities();
    inline TTunerCapabilities(TUint32 aTunerBands, TUint32 aAdditionalFunctions);
public:
    /**
    Bitfield (as defined by CMMTunerUtility::TTunerBand) with the bits of the
    supported bands set
    */
    TUint32 iTunerBands;

    /** Tuner Function bit flags - may be extended in future */
    enum TTunerFunctions
    {
        /** Recording of Tuner Audio is possible */
        ETunerFunctionRecording                 = 0x01,
        /** Tuner can record and play back simultaneously */
        ETunerFunctionSimultaneousPlayAndRecord = 0x02,
        /** The tuner requires an external antenna (e.g. a headset) to function */
        ETunerFunctionRequiresAntenna           = 0x04,
        /** CMMRdsTunerUtility supported */
        ETunerFunctionRds                       = 0x08,
        /** The tuner can be used when the device is in flight mode */
        ETunerFunctionAvailableInFlightMode     = 0x10
    };

    /** Bitfield (as defined by ETunerFunctions) with the bits of the supported functions set */
    TUint32 iAdditionalFunctions;
};

/**
Class representing a frequency.
*/
class TFrequency
{
public:
    inline TFrequency();
    explicit inline TFrequency(TInt aFrequency);
    inline TInt operator==(const TFrequency& aFrequency) const;
    inline TInt operator!=(const TFrequency& aFrequency) const;
    inline TInt operator> (const TFrequency& aFrequency) const;
    inline TInt operator>=(const TFrequency& aFrequency) const;
    inline TInt operator< (const TFrequency& aFrequency) const;
    inline TInt operator<=(const TFrequency& aFrequency) const;
public:
    /**
    The Frequency, in Hertz. A TInt is at least 32 bits, giving a maximum frequency
    of at least 2.4GHz (i.e. 0x7fffffff Hz)
    */
    TInt iFrequency;
};


class MMMTunerObserver;
class MMMTunerChangeObserver;
class MMMTunerStereoObserver;
class MMMSignalStrengthObserver;
class MMMTunerAudioPlayerObserver;
class MMMTunerAudioRecorderObserver;
class CMMTunerAudioPlayerUtility;
class CMMTunerAudioRecorderUtility;
class CMMTunerScannerUtility;
class CMMRdsTunerUtility;

/**
The MMF Tuner API is present to allow clients to control
the tuner hardware present on a device.
*/
class CMMTunerUtility : public CBase
{
    friend class CMMTunerAudioPlayerUtility;
    friend class CMMTunerAudioRecorderUtility;
    friend class CMMTunerScannerUtility;
    friend class CMMRdsTunerUtility;
public:
    /** Tuner Band bit flags - may be extended in future */
    enum TTunerBand
    {
        ETunerNoBand            = 0x00,
        /** Long Wave - uses frequencies */
        ETunerBandLw            = 0x01,
        /** Amplitude Modulation or Medium Wave - uses frequencies */
        ETunerBandAm            = 0x02,
        /** Frequency Modulation, European and American band - uses frequencies */
        ETunerBandFm            = 0x04,
        /** Frequency Modulation, Japanese band - uses frequencies */
        ETunerBandJapaneseFm    = 0x08,
        /** Digital Audio Broadcasting - uses channels */
        ETunerBandDab           = 0x10,
        /** Digital Video Broadcasting */
        ETunerBandDvb           = 0x20
    };
    /**
    Search direction enumeration
    */
    enum TSearchDirection
    {
        /** Search for stations upwards - i.e. by increasing frequency */
        ESearchDirectionUp = 1,
        /** Search for stations downwards - i.e. by decreasing frequency */
        ESearchDirectionDown
    };
    /**
    The Tuner Access Priority enables clients to correctly identify their needs
    when it comes to accessing the tuner. A process must have the MultimediaDD
    capability to use priorities greater than ETunerAccessPriorityNormal.
    */
    enum TTunerAccessPriority
    {
        /** Radio accessible when device is idle */
        ETunerAccessPriorityBackground      = -100,
        /** Ordinary application priority */
        ETunerAccessPriorityNormal          = 0,
        /** Radio is to be used as an alarm sound */
        ETunerAccessPriorityAlarm           = 75,
        /** System use only */
        ETunerAccessPrioritySystem          = 100
    };
    /**
    Bitmasks to indicate what state the tuner is in.
    */
    enum TTunerState
    {
        /**
        Tuner is active, and can therefore report frequency etc. If this bit is
        not set, none of the others should be set.
        */
        ETunerStateActive       = 0x01,
        /** The tuner is playing sound. */
        ETunerStatePlaying      = 0x02,
        /** The tuner is currently recording. */
        ETunerStateRecording    = 0x04,
        /** The tuner is currently retuning or searching for a new station. */
        ETunerStateRetuning     = 0x08,
    };
public:

    /**
    Factory function to create a new instance of the Tuner. Tuner access priority
    setting is required to ensure that applications such as alarms using the radio
    as an alarm sound are not prevented from doing so by other clients. Priority
    setting is needed for audio output when accessing the sound device. Tuner is
    ready for use on return from this function.

    @param aObserver The observer object for receiving async completion callbacks
    @param aTunerIndex An index from 0 to TunersAvailable() - 1 specifying the tuner
            device to use.
    @param aAccessPriority Tuner access priority value
    @leave KErrNoMemory Out of memory
    @leave KErrNotFound The specified tuner or tuner controller is not present
    @return A pointer and ownership of the fully constructed CMMTunerUtility object
    */
    IMPORT_C static CMMTunerUtility* NewL(MMMTunerObserver& aObserver,
                                          TTunerBand aBand,
                                          TInt aTunerIndex,
                                          CMMTunerUtility::TTunerAccessPriority aAccessPriority = ETunerAccessPriorityNormal);

    IMPORT_C virtual ~CMMTunerUtility();



    /**
    Set the current tuner access priority of this client. This priority is used to
    arbitrate between multiple tuner clients, determining who get control of the
    tuner.

    The platform security capability is MultimediaDD and a client with this capability
    is allowed to set the priority in preference to a client with a lower capability.

    @param aAccessPriority The new priority to use.
    @capability MultimediaDD
    @return A standard system error code.
    */
    IMPORT_C TInt SetPriority(TTunerAccessPriority aAccessPriority);

    /**
    Get the current tuner access priority of this client.

    @param aAccessPriority A variable to which the current priority will be written.
    @return A standard system error code.
    */
    IMPORT_C TInt GetPriority(TTunerAccessPriority& aAccessPriority) const;


    /**
    Get the current state of the tuner.

    @param aState A variable to set with the current state. Bits set according to
           TTunerState.
    @return A standard system error code.
    */
    IMPORT_C TInt GetState(TUint32& aState) const;

    /**
    Indicates if the external antenna is currently attached or not. The tuner
    capabilties should be queried to determine if the external antenna is required
    to use the tuner or not; A value of false returned here does not necessarily
    imply that the tuner cannot be used.

    @param aAttached When this function returns, this will contain ETrue if and only
           if an external antenna is attached.
    @return A standard system error code.
    */
    IMPORT_C TInt IsAntennaAttached(TBool& aAttached);

    /**
    Indicates if the device is currently in 'flight mode' or not. The tuner
    capabilities should be queried to determine in the tuner can be used  in flight
    mode or not.

    @param aFlightMode On return, this will have been set to ETrue if and only if
            the device is in flight mode.
    @return A standard system error code.
    */
    IMPORT_C TInt GetFlightMode(TBool& aFlightMode) const;

    /**
    Tune the tuner to the required frequency specified in Hertz. This is an asynchronous
    command and will result in a callback to MToTuneComplete or MToTunerError.

    If the session does not currently have control of the tuner, a request for control
    will be made. If control of the tuner is granted, a callback to MToTunerControlGranted
    will occur. If not, there will be a callback to MtoTunerError with error value
    KErrAccessDenied.

    Once control of the tuner has been granted, it will be retained until either a
    call to ReleaseTunerControl, or the session is preempted in which case there
    will be a callback to MToTunerError with error value KErrAccessDenied.

    @param aFrequency The frequency to tune to
    @param aBand The band to which aFrequency belongs
    */
    IMPORT_C void Tune(TFrequency aFrequency);

    /**
    Find a radio station, starting at the start frequency and searching in the
    direction specified (i.e. Up or down) the search is limited to the specified
    band.  If the session does not currently have control of the tuner, a request
    for control will be made. If control of the tuner is granted, a callback to
    MToTunerControlGranted will occur. If not, there will be a callback to MToTunerError
    with error value KErrAccessDenied.

    Once control of the tuner has been granted, it will be retained until either a
    call to ReleaseTunerControl, or the session is preempted in which case there
    will be a callback to MToTunerError with error value KErrAccessDenied.

    A callback to MToTuneComplete will occur if the Seek is successful.

    @param aBand The band
    @param aSearchDirect The direction to search in
    @param aCircularSeek If set to ETrue the station seek will loop back to the other
           end of the band once the end of the band has been reached. (Defaults to ETrue)
           If not set reaching the end of the band without finding a station will result
           in a callback to MToTuneComplete with error KErrNotFound.
    */
    IMPORT_C void StationSeek(TSearchDirection aSearchDirection);

    /**
    Cancels an ongoing retune operation, as initiated by a call to Tune or StationSeek.
    The usual callback will not occur if this has been called.

    Has not affect if no tune or seek operation is ongoing.
    */
    IMPORT_C void CancelRetune();

    /**
    Makes a synchronous request for control of the tuner. If this method returns
    KErrNone, control of the tuner has been granted. Control of the tuner is kept
    until it is explically released using ReleaseTunerControl, or it is revoked
    in which case a callback to MToTunerError with an error of KErrAccessDenied
    will occur.

    If this method returns with KErrAccessDenied, a request to recieve a
    notifiaction when control could be granted can be made using
    NotifyTunerControl.

    Note that methods that require control of the tuner (such as Tune) will make
    a request for control themselves if control has not already been granted.

    @return A standard system error code. If control was granted, KErrNone, and if
            control was denied KErrAccessDenied.
    */
    IMPORT_C TInt RequestTunerControl();

    /**
    Makes an asyncronous request for control of the tuner. This method should be
    called after an control of the tuner has been denied to receive a notification
    when control of the tuner can be granted. A callback to MToTunerControlGranted
    will occur in this event.
    */
    IMPORT_C TInt NotifyTunerControl();

    /**
    Release control of the tuner, allowing other clients to tune it. Change
    notifications may still be received. A request for control of the tuner can be
    made again by calling RequestTunerControl, or any methods that require control
    of the tuner.
    */
    IMPORT_C void ReleaseTunerControl();

    /**
    Release the tuner. Any ongoing playing or recording activity will be stopped,
    control of the tuner will be released, and the hardware will be powered down if
    no other clients need it.
    */
    IMPORT_C void Close();

    /**
    Retrieve the current frequency that the tuner is tuned to

    @param aFrequency The variable to set to the current frequency,
    -1 if channels are in use
    @param aBand The variable used to set the current band.
    @return A standard system error code
    */
    IMPORT_C TInt GetFrequency(TFrequency& aFrequency) const;

    /**
    Retrieve the signal strenth of the currently tuned signal

    @param aSignalStrength Variable into which the signal strength will be written.
    @return A standard system error code
    */
    IMPORT_C TInt GetSignalStrength(TInt& aSignalStrength) const;

    /**
    Get the maximum possible signal strength of a tuned signal.

    @param aMaxSignalStrength A variable that will have the maximun signal strength
            written to.
    @return A standard system error code
    */
    IMPORT_C TInt GetMaxSignalStrength(TInt& aMaxSignalStrength) const;

    /**
    Request notifications when the signal strength changes. Due to the potentially
    short intervals at which the signal strength may change at, notifications will
    only be sent when a relatively large change occurrs. This should allow a visual
    display of signal strength to be maintained fairly accurately.

    The first signal strength notification will be sent immediately after this
    request.

    @param aObserver The object which will receive notifications of signal strength
            changes.
    @return A standard system error code
    */
    IMPORT_C TInt NotifySignalStrength(MMMSignalStrengthObserver& aObserver);

    /**
    Cancel an outstanding NotifySignalStrength request.
    */
    IMPORT_C void CancelNotifySignalStrength();

    /**
    Find out if the current signal is being received in stereo or not.

    @param aStereo On return, will be ETrue if and only if a stereo signal is
           currently being received.
    */
    IMPORT_C TInt IsStereoSignal(TBool& aStereo);

    /**
    Request notifications when stereo reception is lost/restored.

    @param aObserver The object requiring notification when a stereo signal is lost
           or restored. The first notification will occur immediately.
    @return A standard system error code
    */
    IMPORT_C TInt NotifyStereoChange(MMMTunerStereoObserver& aObserver);

    /**
    Cancels a stereo change notification request.
    */
    IMPORT_C void CancelNotifyStereoChange();

    /**
    Indicates whether the reception should be forced into monophonic mode.

    @param aMono If ETrue, all reception will be in mono mode even if a stereo
           signal is available. If EFalse, a stereo signal will be received when
           possible.
    @return A standard system error code.
    */
    IMPORT_C TInt ForceMonoReception(TBool aMono);

    /**
    Find out whether reception is forced into monophonic mode or not.

    @param aMono This will be set to ETrue if all reception is forced to be mono. If
           this is EFalse, this does not imply that stereo reception is currently
           available.
    @return A standard system error code.
    */
    IMPORT_C TInt GetForcedMonoReception(TBool& aMono) const;

    /**
    Sets the current squleching (muting in frequencies without reception) setting.

    @param aEnabled ETrue to enable squelching, EFalse to disable it.
    @return KErrNone if successful, else a system wide error code.
    */
    IMPORT_C TInt SetSquelch(TBool aEnabled);

    /**
    Gets the current squleching (muting in frequencies without reception) setting.

    @param aEnabled This will be set to ETrue if squelching is enabled, EFalse otherwise.
    @return KErrNone if successful, else a system wide error code.
    */
    IMPORT_C TInt GetSquelch(TBool& aEnabled) const;

    /**
    Get the capabilities of the tuner on the device

    @param aCaps The capabilities object to fill
    @return A standard system error code
    */
    IMPORT_C TInt GetCapabilities(TTunerCapabilities& aCaps) const;

    /**
    Get the frequency range (in Hertz) of the specified band.
    This function should be used to enquire the frequency range
    of the bands that GetCapabilities reports as supported.

    @param aBand The band to query
    @param aBottomFrequency The variable to set to the lowest frequency allowed
    @param aTopFrequency The variable to set to the highest frequency allowed
    @return A standard system error code
    */
    IMPORT_C TInt GetFrequencyBandRange(TFrequency& aBottomFrequency, TFrequency& aTopFrequency);

    /**
    Request to be notified when the tuned frequency or channel changes, or when the
    tuner changes state (e.g. starts playing or recording)

    @param aObserver The object wishing to receive tuning change events
    @return A standard system error code
    */
    IMPORT_C TInt NotifyChange(MMMTunerChangeObserver& aObserver);

    /**
    Cancel request to be notified when the tuned frequency or channel changes
    */
    IMPORT_C void CancelNotifyChange();

    /**
    Send a synchronous custom command to the tuner.

    @param aFunction The function number to indicate which function is to be called
        on the interface defined by the first IPC argument
    @param aArgs The IPC arguments to send to the tuner. The first of these
        arguments must be the UID of the interface within the tuner to which the
        command is destined, represented as an integer. Failure to set the first
        argument properly will result in the command completing with
        KErrNotSupported at best, but possibly  the client being panicked.
    @return A standard system error code
    */
    IMPORT_C TInt CustomCommandSync(TInt aFunction, const TIpcArgs& aArgs);

    /**
    Send an asynchronous custom command to the tuner.

    @param aFunction The function number to indicate which function is to be called
        on the interface defined by the first IPC argument
    @param aArgs The IPC arguments to send to the tuner. The first of these
        arguments must be the UID of the interface within the tuner to which the
        command is destined, represented as an integer. Failure to set the first
        argument properly will result in the command completing with
        KErrNotSupported at best, but possibly  the client being panicked.
    @param aStatus The TRequestStatus of an active object. This will contain the
        result of the request on completion.  The exact range of result values is
        dependent on the interface.
    */
    IMPORT_C void CustomCommandAsync(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus);

    /**
    Get the Tuner Player Utility

    @param aAccessPriority A variable to which the current priority will be written.
    @return A standard system error code.
    */
    IMPORT_C CMMTunerAudioPlayerUtility* TunerPlayerUtilityL(MMMTunerAudioPlayerObserver& aObserver) ;
    /**
    Get the Tuner Recorder Utility

    @param aAccessPriority A variable to which the current priority will be written.
    @return A standard system error code.
    */
    IMPORT_C CMMTunerAudioRecorderUtility* TunerRecorderUtilityL(MMMTunerAudioRecorderObserver& aObserver) ;

    /**
    Get the Tuner Scanner Utility

    @param aAccessPriority A variable to which the current priority will be written.
    @return A standard system error code.
    */
    IMPORT_C CMMTunerScannerUtility* TunerScannerUtilityL() ;

    /**
    Get the Tuner Rds Utility

    @param aAccessPriority A variable to which the current priority will be written.
    @return A standard system error code.
    */
    IMPORT_C CMMRdsTunerUtility* TunerRdsUtilityL() ;

private:
    CMMTunerUtility();
protected:
    class CBody;
private:
    CBody* iBody;
};

/**
The Tuner Observer mixin class defines asynchronous
event completion function callbacks
*/
class MMMTunerObserver
{
public:
    /**
    Tune complete event. This event is asynchronous
    and is received after a call to the Tune method.

    @param aError A standard system error
    */
    virtual void MToTuneComplete(TInt aError) = 0;

};

/**
The Tuner Change Observer mixin class defines the interface via which
notification for changes to the tuned frequency, channel and other tuner
state can be received. A client interested in these notifications
should call the function CMMTunerUtility::NotifyChange.
*/
class MMMTunerChangeObserver
{
public:
    /**
    Called when the tuned frequency changes

    @param aOldFrequency The frequency in use before the change
    @param aNewFrequency The new tuned frequency
    */
    virtual void MTcoFrequencyChanged(const TFrequency& aOldFrequency, const TFrequency& aNewFrequency) = 0;

    /**
    Called when the state of the tuner changes.

    @param aOldState The old state. Bits are set according to TTunerState.
    @param aNewState The new state. Bits are set according to TTunerState.
    */
    virtual void MTcoStateChanged(const TUint32& aOldState, const TUint32& aNewState) = 0;

    /**
    This function is called when an external antenna is detached from the device.
    This does not necessarily indicate that the tuner can no longer be used; the
    capabilities of the tuner indicate if the external antenna is required in order
    to use the tuner.
    */
    virtual void MTcoAntennaDetached() = 0;

    /**
    This function is called when an external antenna is attached to the device. If
    the antenna is required to use the tuner, this indicates that the tuner can be
    used again.
    */
    virtual void MTcoAntennaAttached() = 0;

    /**
    This function is called when the device enters or leaves flight mode. If the tuner
    cannot be used in flight mode when the device enters this mode, this indicates
    that the tuner can no longer be used; the capabilities of the tuner indicate if
    it can be used in flight mode or not.

    @param aFlightMode ETrue if the device has just entered flight mode, EFalse if
            flight mode has just been left.
    */
    virtual void FlightModeChanged(TBool aFlightMode) = 0;
};

/**
The stereo observer mixin class defines the interface by which clients can be
notified when a stereo signal is received/lost. An interested client should call
the function CMMTunerUtility::NotifyStereoChange.
*/
class MMMTunerStereoObserver
{
public:
    /**
    Called when stereo reception is lost/restored.

    @param aStereo If true, indicates that stereo reception has just been restored.
           If false, indicates that stereo reception has just been lost.
    */
    virtual void MTsoStereoReceptionChanged(TBool aStereo) = 0;

    /**
    Called when a client enables/disabled forced mono reception.

    @param aForcedMono ETrue if reception is forced to be mono, even when a stereo
           signal is available.
    */
    virtual void MTsoForcedMonoChanged(TBool aForcedMono) = 0;
};

/**
This mixin class should be derived from by clients wishing to receive
notifications when the signal strength changes. Such a client should call
function CMMTunerUtility::NotifySignalStrength.
*/
class MMMSignalStrengthObserver
{
public:
    /**
    Callback indicating that the signal strength has changed by an amount meriting
    a notification.

    @param aNewSignalStrength The new signal strength.
    */
    virtual void MssoSignalStrengthChanged(TInt aNewSignalStrength) = 0;
};


class MMMAudioResourceNotificationCallback;

/**
The Tuner Audio Player Utility is used to initiate and control playback of audio
from the tuner.
*/
class CMMTunerAudioPlayerUtility : public CBase, public MCustomCommand
{
    friend class CMMTunerUtility::CBody;
public:

    IMPORT_C ~CMMTunerAudioPlayerUtility();

    /**
    Set-up the API for playing the output from tuner to the speaker asynchronously.
    Calls MMMTunerAudioPlayerObserver::MTapoInitializeComplete on completion. This must be
    called before Play.

    @param aPriority Sound device priority value
    @param aPref Sound device priority preference value
    */
    IMPORT_C void InitializeL(TInt aAudioPriority = EMdaPriorityNormal, TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

    /**
    Start playback of the tuner output. To stop playback, call Mute, or Stop if
    play will not need to be restarted. InitializeL() must have already been
    called, and a callback to MTapoInitializeComplete with an error of KErrNone must
    have occurred; if this is not the case, this raises a TunerAudioPlay 1 panic.
    */
    IMPORT_C void Play();

    /**
    Mute or unmute playback.

    Raises a TunerAudioPlay 1 panic if the player is not properly initialized.

    @param aMute ETrue to mute the audio, EFalse to unmute it.
    @return A standard system error code
    */
    IMPORT_C TInt Mute(TBool aMute);

    /**
    Stop playback, and release the output device for use by other clients

    Raises a TunerAudioPlay 1 panic if the player is not properly initialized.

    Playback should already be under way.
    */
    IMPORT_C void Stop();

    /**
    Set the current audio priority. This priority is used to arbitrate
    between multiple audio sources trying to access the audio hardware.

    @param aPriority A priority between EMdaPriorityMin and EMdaPriorityMax
    @param aPref Time vs Quality priority preferences, enumerated in TMdaPriorityPreference
    @return A standard system error code
    */
    IMPORT_C TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref);

    /**
    Get the current audio priority. This is used to arbitrate between simultaneous
    accesses to the sound hardware.

    @param aPriority A priority between EMdaPriorityMin and EMdaPriorityMax to return
    @param aPref Time vs Quality priority preferences to return, enumerated in TMdaPriorityPreference
    @return A standard system error code
    */
    IMPORT_C TInt GetPriority(TInt& aPriority, TMdaPriorityPreference& aPref) const;

    /**
    Set the volume to the specified level

    Raises a TunerAudioPlay 1 panic if the player is not properly initialized.

    @param aVolume The volume level to set
    @return A standard system error code
    */
    IMPORT_C TInt SetVolume(TInt aVolume);

    /**
    Return the current volume

    @param aVolume The variable to set to the current volume
    @return A standard system error code
    */
    IMPORT_C TInt GetVolume(TInt& aVolume) const;

    /**
    Define a volume ramp, aRampInterval defining
    the interval between valid volume settings

    Raises a TunerAudioPlay 1 panic if the player is not properly initialized.

    @param aRampInterval The time interval over which the volume
    should be increased from zero to the current volume setting
    @return A standard system error code
    */
    IMPORT_C TInt SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampInterval);

    /**
    Return the maximum volume supported

    @return The maximum volume setting permitted
    */
    IMPORT_C TInt MaxVolume() const;

    /**
    Set the stereo balance between left and right channels

    Raises a TunerAudioPlay 1 panic if the player is not properly initialized.

    @param aBalance The balance value to set - must be between
    KMMFBalanceMaxLeft and KMMFBalanceMaxRight
    @return A standard system error code
    */
    IMPORT_C TInt SetBalance(TInt aBalance = KMMFBalanceCenter);

    /**
    Return the current stereo balance

    @param aBalance The variable to set to the current balance
    @return A standard system error code
    */
    IMPORT_C TInt GetBalance(TInt& aBalance) const;

    /**
    Register for audio resource notifications, in the event that the audio resource is lost due to pre-emption
    by a higher priority audio client.
    */
    IMPORT_C TInt RegisterAudioResourceNotification(MMMAudioResourceNotificationCallback& aCallback, TUid aNotificationEventUid, const TDesC8* aNotificationRegistrationData = NULL);

    /**
    Cancel an outstanding audio resource notification.
    */
    IMPORT_C void CancelRegisterAudioResourceNotification(TUid aNotificationEventId);

    /**
    Returns the controller implementation information associated with the current controller, if any.

    @return The controller implementation structure associated with the controller
    @leave KErrNotFound if no controller is in use.
    */
    IMPORT_C const CMMFControllerImplementationInformation& ControllerImplementationInformationL();

    /**
    Send a synchronous custom command to the playback controller, if ones exists.

    @param aDestination The destination of the message, consisting of the uid of
            the interface of this message
    @param aFunction The function number to indicate which function is to be called
            on the interface defined in the aDestination parameter
    @param aDataTo1 The first chunk of data to be copied to the controller
            framework. The exact contents of the data are dependent on the
            interface being called. Can be KNullDesC8.
    @param aDataTo2 The second chunk of data to be copied to the controller
            framework. The exact contents of the data are dependent on the
            interface being called. Can be KNullDesC8.
    @return A standard system error code
    */
    IMPORT_C TInt CustomCommandSync(
        const TMMFMessageDestinationPckg& aDestination,
        TInt aFunction,
        const TDesC8& aDataTo1,
        const TDesC8& aDataTo2);

    /**
    Send a synchronous custom command to the playback controller, if ones exists.

    @param aDestination The destination of the message, consisting of the uid of
            the interface of this message
    @param aFunction The function number to indicate which function is to be called
            on the interface defined in the aDestination parameter
    @param aDataTo1 The first chunk of data to be copied to the controller
            framework. The exact contents of the data are dependent on the
            interface being called. Can be KNullDesC8.
    @param aDataTo2 The second chunk of data to be copied to the controller
            framework. The exact contents of the data are dependent on the
            interface being called. Can be KNullDesC8.
    @param aDataFrom The area of memory to which the controller framework
            will write any data to be passed back to the client. Can't be KNullDesC8.
    @return A standard system error code
    */
    IMPORT_C TInt CustomCommandSync(
        const TMMFMessageDestinationPckg& aDestination,
        TInt aFunction,
        const TDesC8& aDataTo1,
        const TDesC8& aDataTo2,
        TDes8& aDataFrom);

    /**
    Send an asynchronous custom command to the playback controller, if ones exists.

    @param aDestination The destination of the message, consisting of the uid of
            the interface of this message
    @param aFunction The function number to indicate which function is to be called
            on the interface defined in the aDestination parameter
    @param aDataTo1 The first chunk of data to be copied to the controller
            framework. The exact contents of the data are dependent on the
            interface being called. Can be KNullDesC8.
    @param aDataTo2 The second chunk of data to be copied to the controller
            framework. The exact contents of the data are dependent on the
            interface being called. Can be KNullDesC8.
    @param aStatus The TRequestStatus of an active object. This will contain the
            result of the request on completion.  The exact range of result values is
            dependent on the interface.
    */
    IMPORT_C void CustomCommandAsync(
        const TMMFMessageDestinationPckg& aDestination,
        TInt aFunction,
        const TDesC8& aDataTo1,
        const TDesC8& aDataTo2,
        TRequestStatus& aStatus);

    /**
    Send an asynchronous custom command to the playback controller, if ones exists.

    @param aDestination The destination of the message, consisting of the uid of
            the interface of this message
    @param aFunction The function number to indicate which function is to be called
            on the interface defined in the aDestination parameter
    @param aDataTo1 The first chunk of data to be copied to the controller
            framework. The exact contents of the data are dependent on the
            interface being called. Can be KNullDesC8.
    @param aDataTo2 The second chunk of data to be copied to the controller
            framework. The exact contents of the data are dependent on the
            interface being called. Can be KNullDesC8.
    @param aDataFrom The area of memory to which the controller framework
            will write any data to be passed back to the client. Can't be KNullDesC8.
    @param aStatus The TRequestStatus of an active object. This will contain the
            result of the request on completion.  The exact range of result values is
            dependent on the interface.
    */
    IMPORT_C void CustomCommandAsync(
        const TMMFMessageDestinationPckg& aDestination,
        TInt aFunction,
        const TDesC8& aDataTo1,
        const TDesC8& aDataTo2,
        TDes8& aDataFrom,
        TRequestStatus& aStatus);

protected:
    /**
    Factory function to create a new Tuner Audio Player utility. Note that only one audio player
    utility may be created per instance of CMMTunerUtility. Multiple instances will result in an
    error of KErrAlreadyExists when InitializeL() is called.

    @param aTuner The tuner object which this utility will play the audio from.
    @param aObserver The observer of the player utility to receive asychronous completion and
            notifiction callbacks.
    @leave KErrNoMemory Out of memory
    @leave KErrNotSupported If the given tuner doesn't support audio playback.
    @return A new tuner audio player utility.
    */
    static CMMTunerAudioPlayerUtility* NewL(CMMTunerUtility& aTuner, RRadioSession& aRadioSession, MMMTunerAudioPlayerObserver& aObserver);

private:
    CMMTunerAudioPlayerUtility();
private:
    class CBody;
    CBody* iBody;
};

class MMMTunerAudioPlayerObserver
{
public:
    /**
    The TEvent enumeration is used to indicate which type of event is being sent to the client.
    Each event will be associated with an error code and potentially some addition information,
    and will be passed to the client via method MTapoPlayEvent().
    */
    enum TEventType
    {
        /** An event relating to the tuner itself. Any error other than KErrNone associated
        with this event type may indicate that the tuner cannot be used anymore.

        No additional information is associated with this type of event. */
        ETunerEvent,
        /**
        An event relating to audio playback.

        No additional information is associated with this type of event.
        */
        EAudioEvent
    };
public:
    /**
    Initialize complete event. This event is asynchronous and is received after
    a call to CMMTunerAudioPlayerUtility::InitializeL().

    @param aError A standard system error
    */
    virtual void MTapoInitializeComplete(TInt aError) = 0;

    /**
    Passes an asychronous event to the tuner client.

    @param aEvent The type of event. See enumeration MMMTunerAudioPlayerObserver::TEventType
            for more information about when the event types mean.
    @param aError An error code associated with the event.
    @param aAdditionalInfo Any additional information associated with the event, or NULL if
            no such additional information exists.
    */
    virtual void MTapoPlayEvent(TEventType aEvent, TInt aError, TAny* aAdditionalInfo) = 0;
};

/**
This class is used to perform recording of audio from the tuner. Many of the methods
in this class have identical functionality to similarly names functions in class
CMdaAudioRecorderUtility.
*/
class CMMTunerAudioRecorderUtility : public CBase
{
    friend class CMMTunerUtility::CBody;
public:

    IMPORT_C ~CMMTunerAudioRecorderUtility();

    /**
    Initialize for recording from the tuner to the specified file
    asynchronously. Calls MMMTunerAudioRecorderObserver::MTaroInitializeComplete on completion

    @param aRecordFilename The name of the file to create, if necessary, and record to
    @param "aDestinationDataType" Data type for recording
    @param "aControllerUid" The Uid of the controller to use for recording
    @param "aDestinationFormatUid" Uid of record format
    @param aPriority Sound device priority value
    @param aPref Sound device priority preference value
    */
    IMPORT_C void InitializeL(const TDesC& aRecordFilename,
                              TFourCC aDestinationDataType = KFourCCNULL,
                              TUid aControllerUid=KNullUid,
                              TUid aDestinationFormatUid=KNullUid,
                              TInt aAudioPriority = EMdaPriorityNormal,
                              TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

    /**
    Initialize for recording from the tuner to the specified descriptor
    asynchronously. Calls MMMTunerAudioRecorderObserver::MTaroInitializeComplete on completion

    @param aRecordDescriptor The descriptor to record to
    @param "aDestinationDataType" Data type for recording
    @param "aControllerUid" The Uid of the controller to use for recording
    @param "aDestinationFormatUid" Uid of record format
    @param aPriority Sound device priority value
    @param aPref Sound device priority preference value
    */
    IMPORT_C void InitializeL(TDes8& aRecordDescriptor,
                              TFourCC aDestinationDataType = KFourCCNULL,
                              TUid aControllerUid=KNullUid,
                              TUid aDestinationFormatUid=KNullUid,
                              TInt aAudioPriority = EMdaPriorityNormal,
                              TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

    /**
    Return a list of the supported data types for the record destination
    @param "aSupportedDataTypes" list of four character codes, representing supported data
        encodings for the record destination.
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C void GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDataTypes) const;

    /**
    Set the data type of the destination audio clip
    @param "aDataType" four character code, representing the encoding of the destination audio clip
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C void SetDestinationDataTypeL(TFourCC aDataType);

    /**
    Return the data type of the destination audio clip
    @returns four character code, representing the encoding of the destination audio clip
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C TFourCC DestinationDataTypeL() const;

    /**
    Set the bit rate for recording
    @param "aBitRate" destination bit rate in bits/second
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C void SetDestinationBitRateL(TUint aBitRate);

    /**
    Return the recording bit rate
    @returns destination bit rate in bits/second
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C TUint DestinationBitRateL() const;

    /**
    Return a list of the supported bit rates for recording
    @param "aSupportedBitRates" List of bit rates supported for the record
           destination
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C void GetSupportedBitRatesL(RArray<TUint>& aSupportedBitRates) const;

    /**
    Set the sample rate for the record destination
    @param "aSampleRate" The sample rate of the record destination
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C void SetDestinationSampleRateL(TUint aSampleRate);

    /**
    Return the sample rate of the record destination
    @returns The sample rate of the record destination
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C TUint DestinationSampleRateL() const;

    /**
    Get a list of supported recording sample rates.
    @param "aSupportedSampleRates" List of the sample rates that are supported for
           recording
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C void GetSupportedSampleRatesL(RArray<TUint>& aSupportedSampleRates) const;

    /**
    Set the format of the audio clip. This can only be done if the audio clip does not
    exist

    @param "aFormatUid" Uid of the audio clip format
    @leaves KErrAlreadyExists if the clip already exists and the format is different
     from the existing format, or can leave with one of the system-wide error codes
    */
    IMPORT_C void SetDestinationFormatL(TUid aFormatUid);

    /**
    Return the format of the audio clip
    @returns Uid of the audio clip format
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C TUid DestinationFormatL() const;

    /**
    Set the number of channels for the recorded audio clip
    @param "aNumberOfChannels" The number of channels to record
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C void SetDestinationNumberOfChannelsL(TUint aNumberOfChannels);

    /**
    Return the number of channels in audio clip
    @returns number of channels supported by audio clip
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C TUint DestinationNumberOfChannelsL() const;

    /**
    Return a list of the supported number of channels for recording
    @param "aSupportedNumChannels" List of the number of channels supported for
            recording
    @leaves Can leave with one of the system-wide error codes
    */
    IMPORT_C void GetSupportedNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels) const;

    /** Start recording of the tuner output */
    IMPORT_C void RecordL();

    /**
    Pause recording. Recording can be resumed with another call to Record.

    @return A standard system error code
    */
    IMPORT_C TInt PauseRecord();

    /**
    Stop recording, and release the output device for use by other clients

    Recording should already be under way.
    */
    IMPORT_C void Stop();

    /**
    Stop recording, and release the output device for use by other clients

    Recording should already be under way.
    */
    IMPORT_C void Close();

    /**
    Return the maximum value for the gain

    @return The maximum gain. For devices where automatic gain control is used, this value may be zero.
    */
    IMPORT_C TInt MaxGain() const;

    /**
    Sets the gain for the audio device to a specified value.

    @param aGain The gain setting. This can be any value from zero to the value returned by a call to
            MaxGain(). A value which is less than zero is set to zero. A value which is greater than
            MaxGain() is set to MaxGain().
    */
    IMPORT_C void SetGain(TInt aGain);

    /**
    Sets the current recording balance setting between the left and right stereo channels

    The balance can be changed before or during recording and is effective immediately.

    @param aBalance The balance value to set. Any value between KMMFBalanceMaxLeft and
            KMMFBalanceMaxRight, the default value being KMMFBalanceCenter.
    @return An error code indicating if the call was successful. KErrNone on success,
            otherwise another of the system-wide error codes.
    */
    IMPORT_C TInt SetRecordBalance(TInt aBalance = KMMFBalanceCenter);

    /**
    Returns the current recording balance setting between the left and right stereo channels.

    @param aBalance On return, contains the current recording balance between KMMFBalanceMaxLeft
            and KMMFBalanceMaxRight.
    @return An error code indicating if the call was successful. KErrNone on success, otherwise
            another of the system-wide error codes.
    */
    IMPORT_C TInt GetRecordBalance(TInt& aBalance) const;

    /**
    Set the current audio priority. This priority is used to arbitrate
    between multiple audio sources trying to access the audio hardware.

    @param aPriority A priority between EMdaPriorityMin and EMdaPriorityMax
    @param aPref Time vs Quality priority preferences, enumerated in TMdaPriorityPreference
    @return A standard system error code
    */
    IMPORT_C TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref);

    /**
    Get the current audio priority. This is used to arbitrate between simultaneous
    accesses to the sound hardware.

    @param aPriority A priority between EMdaPriorityMin and EMdaPriorityMax to return
    @param aPref Time vs Quality priority preferences to return, enumerated in TMdaPriorityPreference
    @return A standard system error code
    */
    IMPORT_C TInt GetPriority(TInt& aPriority, TMdaPriorityPreference& aPref) const;

    /**
    Register for audio resource notifications, in the event that the audio resource is lost due to pre-emption
    by a higher priority audio client.
    */
    IMPORT_C TInt RegisterAudioResourceNotification(MMMAudioResourceNotificationCallback& aCallback, TUid aNotificationEventUid, const TDesC8* aNotificationRegistrationData = NULL);

    /**
    Cancel an outstanding audio resource notification.
    */
    IMPORT_C void CancelRegisterAudioResourceNotification(TUid aNotificationEventId);


    /**
    Sets the maximum size for a file that is being recorded.

    When this limit is reached, MMF stops recording and notifies the client application. Notification is caused
    by MMdaObjectStateChangeObserver::MoscoStateChangeEvent() with the error code KErrEof.

    This function is provided so that applications such as recorders can limit the amount of file storage/memory
    that should be allocated.

    @param aMaxWriteLength
           The maximum file size in kilobytes. If the default value is used, there is no maximum file size.

    */
    IMPORT_C void SetMaxWriteLength(TInt aMaxWriteLength = KMdaClipLocationMaxWriteLengthNone);

    /**
    Returns the recording time available for the selected file or descriptor and encoding format.
    */
    IMPORT_C const TTimeIntervalMicroSeconds& RecordTimeAvailable();

    /**
    Returns the duration of the audio sample data.
    */
    IMPORT_C const TTimeIntervalMicroSeconds& Duration();

    /**
    Return the controller implementation information structure of the current controller

    @leave KErrNoMemory Out of memory
    @return A reference to the current controller information
    */
    IMPORT_C const CMMFControllerImplementationInformation& ControllerImplementationInformationL();

    /*
    Returns the number of meta data entries associated with this clip.
    @return Number of metadata entries
    */
    IMPORT_C TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);

    /*
    Returns the specified meta data entry from the current audio clip.
    @return Metadata entry
    */
    IMPORT_C CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);

    /*
    Adds a meta data entry to the audio clip.
    */
    IMPORT_C void AddMetaDataEntryL(CMMFMetaDataEntry& aMetaDataEntry);

    /*
    Removes a specified meta data entry from the audio clip
    @return An error code indicating if the call was successful
    */
    IMPORT_C TInt RemoveMetaDataEntry(TInt aMetaDataIndex);

    /*
    Replaces the specified meta data entry with a new entry
    */
    IMPORT_C void ReplaceMetaDataEntryL(TInt aMetaDataIndex, CMMFMetaDataEntry& aMetaDataEntry);


    /**
    Send a synchronous custom command to the recording controller

    @param aDestination The destination of the message, consisting of the uid of
    the interface of this message
    @param aFunction The function number to indicate which function is to be called
    on the interface defined in the aDestination parameter
    @param aDataTo1 The first chunk of data to be copied to the controller
    framework. The exact contents of the data are dependent on the
    interface being called. Can be KNullDesC8.
    @param aDataTo2 The second chunk of data to be copied to the controller
    framework. The exact contents of the data are dependent on the
    interface being called. Can be KNullDesC8.
    @return A standard system error code
    */
    IMPORT_C TInt CustomCommandSync(
        const TMMFMessageDestinationPckg& aDestination,
        TInt aFunction,
        const TDesC8& aDataTo1,
        const TDesC8& aDataTo2);

    /**
    Send a synchronous custom command to the recording controller

    @param aDestination The destination of the message, consisting of the uid of
    the interface of this message
    @param aFunction The function number to indicate which function is to be called
    on the interface defined in the aDestination parameter
    @param aDataTo1 The first chunk of data to be copied to the controller
    framework. The exact contents of the data are dependent on the
    interface being called. Can be KNullDesC8.
    @param aDataTo2 The second chunk of data to be copied to the controller
    framework. The exact contents of the data are dependent on the
    interface being called. Can be KNullDesC8.
    @param aDataFrom The area of memory to which the controller framework
    will write any data to be passed back to the client. Can't be KNullDesC8.
    @return A standard system error code
    */
    IMPORT_C TInt CustomCommandSync(
        const TMMFMessageDestinationPckg& aDestination,
        TInt aFunction,
        const TDesC8& aDataTo1,
        const TDesC8& aDataTo2,
        TDes8& aDataFrom);

    /**
    Send an asynchronous custom command to the recording controller

    @param aDestination The destination of the message, consisting of the uid of
    the interface of this message
    @param aFunction The function number to indicate which function is to be called
    on the interface defined in the aDestination parameter
    @param aDataTo1 The first chunk of data to be copied to the controller
    framework. The exact contents of the data are dependent on the
    interface being called. Can be KNullDesC8.
    @param aDataTo2 The second chunk of data to be copied to the controller
    framework. The exact contents of the data are dependent on the
    interface being called. Can be KNullDesC8.
    @param aStatus The TRequestStatus of an active object. This will contain the
    result of the request on completion.  The exact range of result values is
    dependent on the interface.
    @return A standard system error code
    */
    IMPORT_C void CustomCommandAsync(
        const TMMFMessageDestinationPckg& aDestination,
        TInt aFunction,
        const TDesC8& aDataTo1,
        const TDesC8& aDataTo2,
        TRequestStatus& aStatus);

    /**
    Send an asynchronous custom command to the recording controller

    @param aDestination The destination of the message, consisting of the uid of
    the interface of this message
    @param aFunction The function number to indicate which function is to be called
    on the interface defined in the aDestination parameter
    @param aDataTo1 The first chunk of data to be copied to the controller
    framework. The exact contents of the data are dependent on the
    interface being called. Can be KNullDesC8.
    @param aDataTo2 The second chunk of data to be copied to the controller
    framework. The exact contents of the data are dependent on the
    interface being called. Can be KNullDesC8.
    @param aDataFrom The area of memory to which the controller framework
    will write any data to be passed back to the client. Can't be KNullDesC8.
    @param aStatus The TRequestStatus of an active object. This will contain the
    result of the request on completion.  The exact range of result values is
    dependent on the interface.
    @return A standard system error code
    */
    IMPORT_C void CustomCommandAsync(
        const TMMFMessageDestinationPckg& aDestination,
        TInt aFunction,
        const TDesC8& aDataTo1,
        const TDesC8& aDataTo2,
        TDes8& aDataFrom,
        TRequestStatus& aStatus);

protected:
    /**
    Create a new audio recorder utility. Note that only one audio recorder utility may be created
    per instance of CMMTunerUtility. Multiple instances will result in an error of KErrAlreadyExists
    when InitializeL() is called.
    */
    static CMMTunerAudioRecorderUtility* NewL(CMMTunerUtility& aTuner, MMMTunerAudioRecorderObserver& aObserver);

private:
    CMMTunerAudioRecorderUtility();
private:
    class CBody;
    CBody* iBody;

};

class MMMTunerAudioRecorderObserver
{
public:
    /**
    The TEvent enumeration is used to indicate which type of event is being sent to the client.
    Each event will be associated with an error code and potentially some addition information,
    and will be passed to the client via method MTaroRecordEvent().
    */
    enum TEventType
    {
        /** An event relating to the tuner itself. Any error other than KErrNone associated
        with this event type may indicate that the tuner cannot be used anymore.

        No additional information is associated with this type of event. */
        ETunerEvent,
        /**
        An event relating to audio recording.

        No additional information is associated with this type of event.
        */
        EAudioEvent
    };
public:
    /**
    Initialize complete event. This event is asynchronous and is received after
    a call to CMMTunerAudioRecorderUtility::InitializeL.

    @param aError A standard system error
    */
    virtual void MTaroInitializeComplete(TInt aError) = 0;

    /**
    Passes an asychronous event to the tuner client.

    @param aEvent The type of event. See enumeration MMMTunerAudioRecorderObserver::TEventType
            for more information about when the event types mean.
    @param aError An error code associated with the event.
    @param aAdditionalInfo Any additional information associated with the event, or NULL if
            no such additional information exists.
    */
    virtual void MTaroRecordEvent(TEventType aEvent, TInt aError, TAny* aAdditionalInfo) = 0;
};

/**
This class augments CMMTunerUtility to provide station scanning functionality,
whereby the frequency spectrum is searched, pausing for a specified amount of
time when a station is found.
*/
class CMMTunerScannerUtility : public CBase
{
    friend class CMMTunerUtility::CBody;
public:

    IMPORT_C ~CMMTunerScannerUtility();

    /**
    Continuously scan for a radio station, pausing for the time specified before
    continuing on to the next station. Call StopScan to select the currently tuned
    station. The search is limited to the specified band.

    @note The control of a CMMTunerUtility object must have been passed to this
          class (using TransferTunerControl) before this function is called.

    @param aBand The band to which aStartFrequency belongs
    @param aSearchDirect The direction to search in
    @param aPauseDuration Time to pause at each station
    @param aCircularScan If set to ETrue the station scan will loop back to the other
           end of the band once the end of the band has been reached. (Defaults to ETrue)
    */
    IMPORT_C void StationScan(CMMTunerUtility::TSearchDirection aSearchDirection,
                              TTimeIntervalMicroSeconds32 aPauseDuration
                             );

    /**
    Stop scanning and use the currently tuned station

    @return ETrue if a station is tuned, EFalse otherwise
    */
    IMPORT_C TBool StopScan();

protected:
    /**
    Factory function to create a new CMMTunerScannerUtility.

    @param aTuner The tuner with which to perform the scanning.
    @leave KErrNoMemory Out of memory.
    @return A newly contructed tuner scanner utility.
    */
    static CMMTunerScannerUtility* NewL(CMMTunerUtility& aTuner,CMMTunerUtility::TTunerBand aBand);

private:
    CMMTunerScannerUtility();
private:
    class CBody;
    CBody* iBody;

};


/** Programme Station name, 8 characters */
typedef TBuf<8> TRdsStationName;
/** Programme Type Name, 8 characters */
typedef TBuf<8> TRdsProgrammeTypeName;
/** RDS Programme Identifier, a unique ID for each radio station */
typedef TInt TRdsProgrammeIdentifier;

/**
Programme Item Number - identifies the start time of the current programme,
as published by the broadcaster.
*/
class TRdsProgrammeItemNumber
{
public:
    /** The day of the month of the broadcast, in the range 1-31 */
    TInt iDayOfMonth;
    /** The hour of the day, in the range 0-23 */
    TInt iHour;
    /** The minute past the hour, in the range 0-59 */
    TInt iMinute;
public:
    inline TInt operator==(const TRdsProgrammeItemNumber& aPin) const;
    inline TInt operator!=(const TRdsProgrammeItemNumber& aPin) const;
};

/**
An RDS extended country code. The meaning of a value of this type is defined
by the RDS specification, IEC62106.
*/
typedef TInt TRdsCountryCode;

/**
An RDS language identification code. The meaning of a value of this type is
defined by the RDS specification, IEC62106.
*/
typedef TInt TRdsLanguageIdentifier;

/**
RDS Programme Type.

The interpretation of values of this type depends on the origin of the RDS
broadcast: in North America, a slightly different standard, RBDS, is used.
These PTY codes are defined by static values KRbdsPtyXxxxx, for example
KRbdsPtySoftRock.

Elsewhere, including Europe, the RDS standard is used. In these areas, the
PTY codes are defined by static values KRdsPtyXxxxx, for example
KRdsPtyChildrensProgrammes.

In all other important aspects, the European RDS and North American RBDS
standards are identical.
*/
typedef TInt TRdsProgrammeType;

/** No programme type or undefined */
const static TRdsProgrammeType KRdsPtyNone = 0;
/** News */
const static TRdsProgrammeType KRdsPtyNews = 1;
/** CurrentAffairs */
const static TRdsProgrammeType KRdsPtyCurrentAffairs = 2;
/** Information */
const static TRdsProgrammeType KRdsPtyInformation = 3;
/** Sport */
const static TRdsProgrammeType KRdsPtySport = 4;
/** Education */
const static TRdsProgrammeType KRdsPtyEducation = 5;
/** Drama */
const static TRdsProgrammeType KRdsPtyDrama = 6;
/** Culture */
const static TRdsProgrammeType KRdsPtyCulture = 7;
/** Science */
const static TRdsProgrammeType KRdsPtyScience = 8;
/** VariedSpeech */
const static TRdsProgrammeType KRdsPtyVariedSpeech = 9;
/** PopMusic */
const static TRdsProgrammeType KRdsPtyPopMusic = 10;
/** RockMusic */
const static TRdsProgrammeType KRdsPtyRockMusic = 11;
/** EasyListening */
const static TRdsProgrammeType KRdsPtyEasyListening = 12;
/** LightClassical */
const static TRdsProgrammeType KRdsPtyLightClassical = 13;
/** SeriousClassics */
const static TRdsProgrammeType KRdsPtySeriousClassical = 14;
/** OtherMusic */
const static TRdsProgrammeType KRdsPtyOtherMusic = 15;
/** Weather */
const static TRdsProgrammeType KRdsPtyWeather = 16;
/** Finance */
const static TRdsProgrammeType KRdsPtyFinance = 17;
/** ChildrensProgrammes */
const static TRdsProgrammeType KRdsPtyChildrensProgrammes = 18;
/** SocialAffairs */
const static TRdsProgrammeType KRdsPtySocialAffairs = 19;
/** Religion */
const static TRdsProgrammeType KRdsPtyReligion = 20;
/** PhoneIn */
const static TRdsProgrammeType KRdsPtyPhoneIn = 21;
/** Travel */
const static TRdsProgrammeType KRdsPtyTravel = 22;
/** Leisure */
const static TRdsProgrammeType KRdsPtyLeisure = 23;
/** JazzMusic */
const static TRdsProgrammeType KRdsPtyJazzMusic = 24;
/** CountryMusic */
const static TRdsProgrammeType KRdsPtyCountryMusic = 25;
/** NationalMusic */
const static TRdsProgrammeType KRdsPtyNationalMusic = 26;
/** OldiesMusic */
const static TRdsProgrammeType KRdsPtyOldiesMusic = 27;
/** FolkMusic */
const static TRdsProgrammeType KRdsPtyFolkMusic = 28;
/** Documentary */
const static TRdsProgrammeType KRdsPtyDocumentary = 29;
/** AlarmTest */
const static TRdsProgrammeType KRdsPtyAlarmTest = 30;
/** Alarm */
const static TRdsProgrammeType KRdsPtyAlarm = 31;


/** No programme type or undefined */
const static TRdsProgrammeType KRbdsPtyNone = 0;
/** News */
const static TRdsProgrammeType KRbdsPtyNews = 1;
/** Informaion */
const static TRdsProgrammeType KRbdsPtyInformation = 2;
/** Sports */
const static TRdsProgrammeType KRbdsPtySports = 3;
/** Talk */
const static TRdsProgrammeType KRbdsPtyTalk = 4;
/** Rock */
const static TRdsProgrammeType KRbdsPtyRock = 5;
/** Classic Rock */
const static TRdsProgrammeType KRbdsPtyClassicRock = 6;
/** Adult Hits */
const static TRdsProgrammeType KRbdsPtyAdultHits = 7;
/** Soft Rock */
const static TRdsProgrammeType KRbdsPtySoftRock = 8;
/** Top 40 */
const static TRdsProgrammeType KRbdsPtyTop40 = 9;
/** Country */
const static TRdsProgrammeType KRbdsPtyCountry = 10;
/** Oldies */
const static TRdsProgrammeType KRbdsPtyOldies = 11;
/** Soft */
const static TRdsProgrammeType KRbdsPtySoft = 12;
/** Nostalgia */
const static TRdsProgrammeType KRbdsPtyNostalgia = 13;
/** Jazz */
const static TRdsProgrammeType KRbdsPtyJazz = 14;
/** Classical */
const static TRdsProgrammeType KRbdsPtyClassical = 15;
/** Rhythm and Blues */
const static TRdsProgrammeType KRbdsPtyRhythmAndBlues = 16;
/** Soft Rhythm and Blues */
const static TRdsProgrammeType KRbdsPtySoftRhythmAndBlues = 17;
/** Language */
const static TRdsProgrammeType KRbdsPtyLanguage = 18;
/** Religious Music */
const static TRdsProgrammeType KRbdsPtyReligiousMusic = 19;
/** Religious Talk */
const static TRdsProgrammeType KRbdsPtyReligiousTalk = 20;
/** Personality */
const static TRdsProgrammeType KRbdsPtyPersonality = 21;
/** Public */
const static TRdsProgrammeType KRbdsPtyPublic = 22;
/** College */
const static TRdsProgrammeType KRbdsPtyCollege = 23;
/** Unassigned */
const static TRdsProgrammeType KRbdsPtyUnassigned1 = 24;
/** Unassigned */
const static TRdsProgrammeType KRbdsPtyUnassigned2 = 25;
/** Unassigned */
const static TRdsProgrammeType KRbdsPtyUnassigned3 = 26;
/** Unassigned */
const static TRdsProgrammeType KRbdsPtyUnassigned4 = 27;
/** Unassigned */
const static TRdsProgrammeType KRbdsPtyUnassigned5 = 28;
/** Weather */
const static TRdsProgrammeType KRbdsPtyWeather = 29;
/** Emergency Test */
const static TRdsProgrammeType KRbdsPtyEmergencyTest = 30;
/** Emergency */
const static TRdsProgrammeType KRbdsPtyEmergency = 31;

/**
The RDS Capabilities class defines the capabilities of the RDS tuner on the
device, as retrieved using the function GetRdsCapabilities.
*/
class TRdsCapabilities
{
public:
    /** RDS Function Bit Flags */
    enum TRdsFunctions
    {
        /** Traffic Announcement */
        ERdsFunctionTa      = 0x01,
        /** Regional Links */
        ERdsFunctionRegLink = 0x02,
        /** News Announcement */
        ERdsFunctionNa      = 0x04,
        /** Programme Type */
        ERdsFunctionPty     = 0x08,
        /** Clock Time */
        ERdsFunctionCt      = 0x10,
        /** Enhanced Other Networks */
        ERdsFunctionEon     = 0x20,
        /** Alternative Frequency */
        ERdsFunctionAf      = 0x40
    };
public:
    /** Bitfield as defined by ERdsFunctions with the bits of the supported functions set */
    TUint32 iRdsFunctions;
};

class MMMRdsDataObserver;
class MMMRdsEonObserver;
class MMMRdsStateChangeObserver;
class MMMRdsAnnouncementObserver;

/**
This class represents the basic RDS data associated with an RDS station.
*/
class TRdsData
{
public:
    inline TRdsData();
public:
    /**
    Enumeration to indicate a subset of the members of class TRdsData. These values should
    be bitwise or'ed together to indicate which members belong in a set.
    */
    enum TField
    {
        /** Indicates the the member iPi is in a set */
        EProgrammeIdentifier    = 0x001,
        /** Indicates the the member iPs is in a set */
        EStationName            = 0x002,
        /** Indicates the the member iTp is in a set */
        ETrafficProgramme       = 0x004,
        /** Indicates the the member iTa is in a set */
        ETrafficAnnouncement    = 0x008,
        /** Indicates the the member iPty is in a set */
        EProgrammeType          = 0x010,
        /** Indicates the the member iPtyn is in a set */
        EProgrammeTypeName      = 0x020,
        /** Indicates the the member iPin is in a set */
        EProgrammeItemNumber    = 0x040,
        /** Indicates the the member iMs is in a set */
        EMusicSpeech            = 0x080,
        /** Indicates the the member iBroadcastLanguage is in a set */
        EBroadcastLanguage      = 0x100,
        /** Indicates the the member iEcc is in a set */
        EExtendedCountryCode    = 0x200
    };
    /**
    A value indicating a set containig all RDS data encapsulated by class TRdsData.
    */
    const static TUint32 KAllRdsData = (EProgrammeIdentifier | EStationName | ETrafficProgramme | ETrafficAnnouncement
                                        | EProgrammeType | EProgrammeTypeName | EProgrammeItemNumber | EMusicSpeech
                                        | EBroadcastLanguage | EExtendedCountryCode);
public:
    /** Programme Identifier of the station */
    TRdsProgrammeIdentifier iPi;
    /** Programme Station name of the station */
    TRdsStationName iPs;
    /** Value of the Traffic Programme flag of the station */
    TBool iTp;
    /** Value of the Traffic Announcement flag of the station */
    TBool iTa;
    /** Programme Type of the station */
    TRdsProgrammeType iPty;
    /** Programme Type Name of the station */
    TRdsProgrammeTypeName iPtyn;
    /** Programme Item Number of the station */
    TRdsProgrammeItemNumber iPin;
    /** Value of the Music Speech flag of the station. EFalse indicates the speech is being
    broadcast at present. ETrue indicates that music is being broadcast, or that the flag is
    not in use by the broadcaster. */
    TBool iMs;
    /** The current language of the broadcast */
    TRdsLanguageIdentifier iBroadcastLanguage;
    /** The Extended Country Code of the station */
    TRdsCountryCode iEcc;
};

/**
Class representing a station broadcast as an Enhanced Other Networks station.
*/
class TEonStation
{
public:
    /**
    Enumeration to indicate a subset of the members of class TEonStation. These values should
    be bitwise or'ed together to indicate which members belong in a set.
    */
    enum TField
    {
        /** Indicates that member iProgrammeIdentifier is valid. */
        EProgrammeIdentifier= 0x01,
        /** Indicates that member iStationName is valid. */
        EStationName        = 0x02,
        /** Indicates that member iProgrammeType is valid. */
        EProgrammeType      = 0x04,
        /** Indicates that member iTrafficProgramme is valid. */
        ETrafficProgramme   = 0x08,
        /** Indicates that member iTrafficAnnouncement is valid. */
        ETrafficAnnouncement= 0x10,
        /** Indicates that member iProgrammeItemNumber is valid. */
        EProgrammeItemNumber= 0x20,
        /** Indicates that member iNoFrequencies is valid. */
        ENoFrequencies      = 0x40,
        /** Indicates that member iNoMappedFrequencies is valid. */
        ENoMappedFrequencies= 0x80
    };
public:
    /** The unique identifier of this station */
    TRdsProgrammeIdentifier iProgrammeIdentifier;
    /** The textual name of this station */
    TRdsStationName iStationName;
    /** The current programme type (PTY) of this station */
    TRdsProgrammeType iProgrammeType;
    /** Indicates if this station broadcasts traffic programmes */
    TBool iTrafficProgramme;
    /** Indicates if this station is currently broadcasting a traffic announcement */
    TBool iTrafficAnnouncement;
    /** The programme item number (PIN) of the current broadcast on this station. */
    TRdsProgrammeItemNumber iProgrammeItemNumber;
    /**
    The number of frequencies associated with this station. If this number is zero,
    it may be that the EON station uses the mapped frequencies method instead. See
    iNoMappedFrequencies.
    */
    TInt iNoFrequencies;
    /**
    The number of mapped frequencies associated with this station. If this number is
    zero, it may be that the EON station uses a flat frequency list instead. See
    iNoFrequencies.
    */
    TInt iNoMappedFrequencies;
    /**
    Indicates the subset of fields of the class that are valid. Bits are set according to enumeration
    TEonStation::TField
    */
    TUint32 iValid;
};

/**
Mapped frequencies can be broadcast as a part of the EON information. They relate the
current tuning frequency with the frequency which the referred EON station will be
broadcast on.
*/
class TEonMappedFrequency
{
public:
    inline TEonMappedFrequency(TFrequency aTuningFrequency, TFrequency aMappedFrequency);
public:
    /** The current tuning frequency, relating to the station broadcasting the EON informarion. */
    TFrequency iTuningFrequency;
    /**
    The mapped frequency. If iTunedFrequency matches the currently tuned frequency, the
    EON station will be broadcast on this frequency.
    */
    TFrequency iMappedFrequency;
};

/**
The RDS class augments the tuner API to give access to the RDS capabilities
of the device. As such additional tuner technologies can be supported without
changing the Tuner API.

Note that despite this class being names 'RDS', it is capable of supporting both
the RDS standard, and the North American equivilant RBDS. The only important difference
from the APIs perspective is the interpretation of the Programme Type (PTY) codes. See
TRdsProgrammeType for more information.
*/
class CMMRdsTunerUtility : public CBase
{
    friend class CMMTunerUtility::CBody;
public:
    /** RDS Announcement Type */
    enum TAnnouncementType
    {
        /** Traffic announcement */
        ERdsTrafficAnnouncement,
        /** News announcement */
        ERdsNewsAnnouncement
    };
public:
    /**
    Factory function to create a new instance of the RDS Tuner API

    @param aTuner A RDS capable tuner object (check using CMMTunerUtility::GetCapabilities())
    @param aObserver The observer of the tuner to receive asynchronous completion messages.
    @leave KErrNoMemory Out of memory
    @leave KErrNotFound CMMRdsTunerUtility object is not present
    @leave KErrNotSupported RDS is not supported by the tuner
    @return A pointer and ownership of a fully constructed CMMRdsTunerUtility object
    */
    IMPORT_C static CMMRdsTunerUtility* NewL(CMMTunerUtility& aTuner, MMMTunerObserver& aObserver, CMMTunerUtility::TTunerBand aBand);

    IMPORT_C ~CMMRdsTunerUtility();

    /**
    Get the RDS capabilities of the device

    @param aCaps The capabilities object to fill
    @return A standard system error code
    */
    IMPORT_C TInt GetRdsCapabilities(TRdsCapabilities& aCaps) const;

    /**
    Find a radio station which contains RDS data starting at the start frequency
    and searching in the direction specified (i.e. Up or down).

    @note this function is subject to the same access control scheme as the Tune
          methods of CMMTunerUtility

    @param aStartFrequency The frequency to start searching from, or 0 to start at the
            beginning of the stated band.
    @param aBand The frequency band to search. This must be a FM band.
    @param aSearchDirection The direction to search in
    @param aCircularSeek If set to ETrue the station seek will loop back to the
           other end of the band once the end of the band has been reached.
           (Defaults to ETrue) If not set reaching the end of the band without
           finding a station will result in a callback to MToTuneComplete with error
           KErrNotFound.
    */
    IMPORT_C void StationSearchByRds(TFrequency     aStartFrequency,
                                     CMMTunerUtility::TSearchDirection aSearchDirection
                                    );

    /**
    Find a radio station, of the specified programme type starting at the start
    frequency and searching in the direction specified (i.e. Up or down).

    @note this function is subject to the same access control scheme as the Tune
          methods of CMMTunerUtility

    @param aProgType The type of programme to search for
    @param aStartFrequency The frequency to start searching from, or 0 to start at the
            beginning of the stated band.
    @param aBand The frequency band to search. This must be a FM band.
    @param aSearchDirection The direction to search in
    @param aCircularSeek If set to ETrue the station seek will loop back to the
           other end of the band once the end of the band has been reached.
           (Defaults to ETrue) If not set reaching the end of the band without
           finding a station will result in a callback to MToTuneComplete with error
           KErrNotFound.
    */
    IMPORT_C void StationSearchByProgrammeType(
        TRdsProgrammeType aProgType,
        TFrequency aStartFrequency,
        CMMTunerUtility::TSearchDirection aSearchDirection
    );

    /**
    Find a radio station, with the specified programme identifier starting at the
    start frequency and searching in the direction specified (i.e. Up or down).

    @note this function is subject to the same access control scheme as the Tune
          methods of CMMTunerUtility

    @param aPi The programme identifier of the station to search for
    @param aStartFrequency The frequency to start searching from, or 0 to start at the
            beginning of the stated band.
    @param aBand The frequency band to search. This must be a FM band.
    @param aSearchDirection The direction to search in
    @param aCircularSeek If set to ETrue the station seek will loop back to the other
           end of the band once the end of the band has been reached. (Defaults to ETrue)
           If not set reaching the end of the band without finding a station will result
           in a callback to MToTuneComplete with error KErrNotFound.
    */
    IMPORT_C void StationSearchByProgrammeIdentifier(
        TRdsProgrammeIdentifier             aPi,
        TFrequency                          aStartFrequency,
        CMMTunerUtility::TSearchDirection   aSearchDirection
    );

    /**
    Find a radio station, with the specified traffic programme flag value starting at
    the start frequency and searching in the direction specified (i.e. Up or down).

    @note this function is subject to the same access control scheme as the Tune
          methods of CMMTunerUtility

    @param aTp The TP flag value of a station to search for
    @param aStartFrequency The frequency to start searching from, or 0 to start at the
            beginning of the stated band.
    @param aBand The frequency band to search. This must be a FM band.
    @param aSearchDirection The direction to search in
    @param aCircularSeek If set to ETrue the station seek will loop back to the other
           end of the band once the end of the band has been reached. (Defaults to ETrue)
           If not set reaching the end of the band without finding a station will result
           in a callback to MToTuneComplete with error KErrNotFound.
    */
    IMPORT_C void StationSearchByTrafficProgramme(
        TBool                              aTp,
        TFrequency                         aStartFrequency,
        CMMTunerUtility::TSearchDirection  aSearchDirection
    );

    /**
    Cancels an ongoing RDS search as initiated by one of the functions
    StationSearchByRds, StationSearchByProgrammeType,
    StationSearchByProgrammeIdentifier or StationSearchByTrafficProgramme. The
    asynchronous callback will not occur if this is called.

    Has not affect if no RDS search operation is ongoing.
    */
    IMPORT_C void CancelRdsSearch();

    /**
    Requests notifications when all RDS data become invalid due to the tuner being
    retuned.

    @param aObserver The client to be notified.
    @param aWhichData The subset of data for which change notifications are required.
    @return A standard system wide error code.
    */
    IMPORT_C TInt NotifyRdsDataChange(MMMRdsDataObserver& aObserver, TUint32 aWhichData = TRdsData::KAllRdsData);

    /**
    Cancel a NotifyRdsDataChange request.
    */
    IMPORT_C void CancelNotifyRdsDataChange();

    /**
    Request some RDS data. This will complete immediately with whatever RDS data have already been
    received.

    When this function returns, and data that was requested but is not indicated to be
    valid can be assumed not to have been received.

    @param aData The RDS data will be written to this variable.
    @param aValid On return, indicates a subset of RDS data that are valid.
    @param aWhichData The subset of RDS data that are being requested.
    @return A standard system wide error code.
    */
    IMPORT_C TInt GetRdsData(TRdsData& aData, TUint32& aValid, TUint32 aWhichData = TRdsData::KAllRdsData) const;

    /**
    Converts an RDS language identifier into a Symbian TLanguage type. Note that not all
    languages defined by the RDS specification IEC62106 are present in the TLanguage
    enumeration; in these cases, a value of ELangOther will be returned.

    @param aRdsLangId An RDS language identification code
    @return The corresponding TLanguage member, or ELangOther if none exists.
    */
    IMPORT_C static TLanguage ConvertRdsLanguageId(TRdsLanguageIdentifier aRdsLangId);

    /**
    Get the length of the available radio text. If no radio text is available this
    function will return KErrNotFound. The maximum possible length for radio text is 64 characters.

    @param aLength The variable to set to the length of the avaiable radio text
    @return A standard system error code.
    */
    IMPORT_C TInt GetRadioTextLength(TUint& aLength) const;

    /**
    Get the radio text. If no radio text is available, this will return KErrNotFound. In this
    case, a client can call NotifyRadioText to receive a notification when it is received.

    The radio text will have been converted to unicode, eliminating any control characters
    within it.

    @param aRadioText The descriptor to fill with the radio text
    @return A standard system error code
    */
    IMPORT_C TInt GetRadioText(TDes& aRadioText) const;

    /**
    Request notification when the radio text is received or changes.

    @param aObserver The client to be notified when the radio text is received or changes.
    @return A standard system wide error code.
    */
    IMPORT_C TInt NotifyRadioText(MMMRdsDataObserver& aObserver);

    /**
    Cancel a NotifyRadioText request.
    */
    IMPORT_C void CancelNotifyRadioText();

    /**
    Turns regional link function on or off depending on the value of the parameter.
    A value of ETrue should be passed if you wish to stay tuned to the currently
    tuned local station regardless of signal quality and signal strength.
    i.e. don't switch to another local station in the region.

    @param aRegOn ETrue to turn regional link on, EFalse to turn it off
    @return A standard system error code
    */
    IMPORT_C TInt SetRegionalLink(TBool aRegOn);

    /**
    Finds out if the regional link function is currently on or off.

    @param aRegOn This will be set to ETrue on return if and only if the regional
           link function is currently enabled.
    @return A standard system error code.
    */
    IMPORT_C TInt GetRegionalLink(TBool& aRegOn) const;

    /**
    Turn the travel announcement function on or off depending on the value of the
    parameter. A value of ETrue turns on Traffic Announcements, EFalse turns them off.

    If Traffic announcements are disabled while the tuner is retuned to a traffic
    announcement, the tuner will not revert to the original frequency. To revert to
    the original frequency, StopAnnouncement() must be called before the traffic
    announcement feature is disabled.

    @param aTaOn ETrue to turn TA on, EFalse to turn it off
    @return A standard system error code
    */
    IMPORT_C TInt SetTrafficAnnouncement(TBool aTaOn);

    /**
    Finds out if the traffic announcement function is currently enabled or not.

    @param aTaOn This is set to ETrue on return if and only if the traffic
           announcement function is currenly enabled.
    */
    IMPORT_C TInt GetTrafficAnnouncement(TBool& aTaOn) const;

    /**
    Set the absolute volume to apply during a traffic or news announcement.

    @param aVolume The volume to use. Must be between 0 and MaxVolume.
    @return A standard system error code.
    */
    IMPORT_C TInt SetAnnouncementVolume(TInt aVolume);

    /**
    Find the current absolute volume level used for news of traffic annoucements.

    @param aVolume This will be set to the current volume used for annoucements.
    @return A standard system error code, KErrNotFound if a annoucement volume offset has been set.
            use.
    */
    IMPORT_C TInt GetAnnouncementVolume(TInt& aVolume) const;

    /**
    Set the offset to the system volume level to apply during a traffic or news announcement

    @param aVolumeOffset The offset to the volume level to set for announcements. Must be between -MaxVolume and MaxVolume inclusive.
                         the actual volume with the offset applied will be clipped between 0 and MaxVolume if the offset would
                         otherwise result in a volume outside this range.
    @return A standard system error code
    */
    IMPORT_C TInt SetAnnouncementVolumeOffset(TInt aVolumeOffset);

    /**
    Find the current offset of the system volume that applies during traffic and news announcements.

    @param aVolumeOffset This will be set to the current traffic and news announcement offset on return.
    @return A standard system error code, KErrNotFound if an absolute annoucement volume has been set.
    */
    IMPORT_C TInt GetAnnouncementVolumeOffset(TInt& aVolumeOffset) const;

    /**
    Turn the news announcement function on or off depending on the value of the
    parameter. The news announcement function when enabled causes the radio to
    retune to a station when that station is broadcasting a news report. When the
    news announcement is finished the radio will tune back to the original station.
    A value of ETrue turns on News Announcements, EFalse turns them off.

    If News announcements are disabled while the tuner is retuned to a news
    announcement, the tuner will not revert to the original frequency. To revert to
    the original frequency, StopAnnouncement() must be called before the news
    announcement feature is disabled.

    @param aNaOn ETrue to turn NA on, EFalse to turn it off
    @return A standard system error code
    */
    IMPORT_C TInt SetNewsAnnouncement(TBool aNaOn);

    /**
    Finds out whether the news announcement function is on or off.

    @param aNaOn This will be set to ETrue if and only if the new announcement
           function is currently on.
    @return A standard system error code.
    */
    IMPORT_C TInt GetNewsAnnouncement(TBool& aNaOn) const;

    /**
    Cancels any current announcement, reverting to the original frequency. The announcement
    feature will remain enabled. If no announcement is currently happening, this function
    has no affect. This can be used for both News and Traffic announcements.

    @return A standard system error code. KErrNone if an announcement was successfully
            stopped, or KErrNotReady if no announcement is currently happening.
    */
    IMPORT_C TInt StopAnnouncement();

    /**
    Turns alternative frequency function on or off depending on the value of the parameter.
    A value of ETrue should be passed if you wish to enable automatic retuning to the current
    station on an alternative frequency.

    @param aAfOn ETrue to turn alternative frequency on, EFalse to turn it off
    @return A standard system error code
    */
    IMPORT_C TInt SetAlternativeFrequency(TBool aAfOn);

    /**
    Finds out whether the alternative frequency function is on or off.

    @param aAfOn This will be set to ETrue if and only if the alternative frequency
           function is currently on.
    @return A standard system error code.
    */
    IMPORT_C TInt GetAlternativeFrequency(TBool& aAfOn) const;

    /**
    Requests a notification when RDS functionality is enabled or disabled, or when
    the traffic announcement volume offset is changed.

    @param aObserver The class which is to be notified of the changes.
    @return A standard system error code.
    */
    IMPORT_C TInt NotifyRdsStateChange(MMMRdsStateChangeObserver& aObserver);

    /**
    Cancels an outstanding RDS state change notification request.
    */
    IMPORT_C void CancelNotifyRdsStateChange();

    /**
    Get the current RDS time. This is an asynchronous function due to the latency of
    the RDS information becoming available. This information is broadcast at the start of
    every minute, and is not cached for obvious reasons. Thus, whenever a request is made,
    it will complete next time the Data and Time are broadcast. The RDS standard states
    that this is broadcast within 0.1s of the start of a minute, and is accurate to one
    minute. The latency of the notification reaching the application cannot be guarteed.

    @param aTimeAndDate The variable to set to the current RDS time and date
    @param aStatus A TRequestStatus. This will be signalled when the request completes
    and will contain the result of the request, this will be one of the system error codes.
    */
    IMPORT_C void GetRdsTime(TPckg<TDateTime>& aTimeAndDate, TRequestStatus& aStatus) const;

    /**
    Cancel the GetRdsTime request
    */
    IMPORT_C void CancelGetRdsTime();

    /**
    Request notification when a retune caused by an announcement occurs. This will be
    a traffic or news announcement. A notification will be provided both at
    the start of the announcement and at the end.

    @param aObserver The object wishing to receive announcement events
    @return A standard system error code
    */
    IMPORT_C TInt NotifyAnnouncement(MMMRdsAnnouncementObserver& aObserver);

    /**
    Cancel the NotifyAnnouncement request
    */
    IMPORT_C void CancelNotifyAnnouncement();

    /**
    Returns a list containing information about other networks broadcast with the currently tuned
    programmme. This call will return immediately with whatever EON information is currently available.
    Note that is is possible for this function to return no EON stations when several are being broadcast
    simply because not enough RDS frames have been received yet. An interested application should make a
    call to NotifyEonInfo to receive notifications when EON information is received or changes.

    @param aEonInfo An array to which the EON information will be appended.
    */
    IMPORT_C void GetEonInfoL(RArray<TEonStation>& aEonInfo) const;

    /**
    Gets the frequencies assoicated with an EON station. This will complete immediately with whatever
    frequencies are currently cached. An interested application should make a call to NotifyEonChange
    to receive notifications when more frequencies are received.

    @param aEonStation the EON station to get the frequencies for
    @param aFrequencies an array to which the frequencies associated with the given station will be
            appended.
    */
    IMPORT_C void GetEonFrequenciesL(const TEonStation& aEonStation, RArray<TFrequency>& aFrequencies) const;

    /**
    Gets the mapped frequencies associated with an EON station. This will complete immediately with whatever
    mapped frequencies are currently cached. An interested application should make a call to NotifyEonChange
    to receive notifications when more frequencies are received.

    @param aEonStation the EON station to get the mapped frequencies for
    @param aMappedFrequencies an array to which the mapped frequencies associated with the given station
            will be appended.
    */
    IMPORT_C void GetEonMappedFrequenciesL(const TEonStation& aEonStation, RArray<TEonMappedFrequency>& aMappedFrequencies) const;

    /**
    Request notification when the Enhanced Other Networks (EON) information changes.

    @param aObserver The client to be notifier when EON information changes or an error occurs.
    */
    IMPORT_C TInt NotifyEonInfo(MMMRdsEonObserver& aObserver);

    /**
    Cancels a NotifyEonInfo request.
    */
    IMPORT_C void CancelNotifyEonInfo();

    /**
    Tunes to a station represented by a TEonStation. This will result to a callback to MtoTuneComplete.

    @param aEonStation The EON station that is to be tuned to.
    */
    IMPORT_C void TuneToEonStation(const TEonStation& aEonStation);

    /**
    Send a synchronous custom command to the RDS tuner.

    @param aFunction The function number to indicate which function is to be called
        on the interface defined by the first IPC argument
    @param aArgs The IPC arguments to send to the RDS tuner. The first of these
        arguments must be the UID of the interface within the tuner to which the
        command is destined, represented as an integer. Failure to set the first
        argument properly will result in the command completing with
        KErrNotSupported at best, but possibly the client being panicked.
    @return A standard system error code
    */
    IMPORT_C TInt CustomCommandSync(TInt aFunction, const TIpcArgs& aArgs);

    /**
    Send an asynchronous custom command to the RDS tuner.

    @param aFunction The function number to indicate which function is to be called
        on the interface defined by the first IPC argument
    @param aArgs The IPC arguments to send to the RDS tuner. The first of these
        arguments must be the UID of the interface within the tuner to which the
        command is destined, represented as an integer. Failure to set the first
        argument properly will result in the command completing with
        KErrNotSupported at best, but possibly  the client being panicked.
    @param aStatus The TRequestStatus of an active object. This will contain the
        result of the request on completion.  The exact range of result values is
        dependent on the interface.
    */
    IMPORT_C void CustomCommandAsync(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus);

private:
    CMMRdsTunerUtility();
private:
    class CBody;
    CBody* iBody;

};

/**
This mixin class should be implemented by applications wishing to receive notifications
when RDS data is received, changes or becomes invalid. Each method corresponds to a
particular request in CMMRdsTunerUtility. Only methods corresponding to requests in
CMMRdsTunerUtility that are used by a client need be implemented - empty default
implementations are provided.
*/
class MMMRdsDataObserver
{
public:
    /**
    Called when some error occurs which makes RDS data unavailable.
    */
    virtual void MrdoError(TInt aError)  = 0;

    /**
    Called when some RDS data is received or has changed.

    Two subsets of the RDS data supplied are indicted: that which has changed and that which is
    valid. This information can be interpreted as follows:
    For an item of RDS data x:
        valid(x)  & !changed(x) => x was received before and has not changed
        valid(x)  & changed(x)  => x has either just been received for the first time, or has just changed
        !valid(x) & changed(x)  => x is no longer available
        !valid(x) & !changed(x) => x was not available before, and is still not available.

    When the tuner is retuned to a new station, all RDS data will be flushed. This will result in
    a call to this function indicating that all RDS data has changed and is longer valid.

    @param aData The RDS data.
    @param aValid Indicates a subset of aData that is valid (i.e. has been received)
    @param aChanged Indicates a subset of aData that has changed since the last call to this function.
    */
    virtual void MrdoDataReceived(const TRdsData& aData, TUint32 aValid, TUint32 aChanged) = 0;

    /**
    Called when the RDS Radio Text (RT) is received, changes, or is no longer available.

    @param aRt The Radio Text message. This will be empty if aValid==EFalse.
    @param aValid Indicates if the radio text is valid.
    @param aChanges Indicates if the radio test has changed.
    */
    virtual void MrdoRadioTextReceived(const TDesC& aRt, TBool aValid, TBool aChanged) = 0;
};

/**
The state change observer mixin class defines the interface via which changes to
the state of the RDS radio can be observed. These state changes will be a result
of a client enabling or disabling RDS functionality.
*/
class MMMRdsStateChangeObserver
{
public:
    /**
    Called when the regional link functionality is enabled/disabled.

    @param aNewRegLinkOn The new setting: ETrue if the regional link function has
           just been enabled.
    */
    virtual void MrscoRegionalLinkChanged(TBool aNewRegLinkOn) = 0;

    /**
    Called when the traffic announcement functionality has just been enabled or
    disabled.

    @param aNewTaOn ETrue if the TA function is now on, EFalse otherwise.
    */
    virtual void MrscoTrafficAnnouncementChanged(TBool aNewTaOn) = 0;

    /**
    Called when the traffic and news announcement volume offset is changed. A callback to
    this method indicates the a volume offset is being used instead of an absolute volume.

    @param aOldOffset The announcement volume offset before the change
    @param aNewOffset The announcement volume offset after the change
    */
    virtual void MrscoAnnouncementVolumeOffsetChanged(TInt aOldOffset, TInt aNewOffset) = 0;

    /**
    Called when the traffic an news announcement volume is changed. A callback to
    this method indicates that an absolute volume is being used instead of volume offsets.

    @param aOldVolume The announcement volume before the change.
    @param aNewVolume The announcement volume after the change.
    */
    virtual void MrscoAnnouncementVolumeChanged(TInt aOldVolume, TInt aNewVolume) = 0;

    /**
    Called when the news announcement functionality has just been enabled or
    disabled.

    @param aNewNAOn ETrue if the NA function is now enabled, else EFalse.
    */
    virtual void MrscoNewsAnnouncementChanged(TBool aNewNAOn) = 0;

    /**
    Called when the alternative frequencies function is turned on or off.

    @param aNewAFOn ETrue if the AF function has just been turned on, else EFalse.
    */
    virtual void MrscoAlternativeFrequenciesChanged(TBool aNewAFOn) = 0;
};


/**
The Announcement Observer mixin class defines the interface via which
announcement events can be received. A client interested in such
information calls the function NotifyAnnouncement.
*/
class MMMRdsAnnouncementObserver
{
public:
    /**
    Called when an announcement starts

    @param aType The type of announcement (travel or news)
    */
    virtual void MraoAnnouncementStart(CMMRdsTunerUtility::TAnnouncementType aType) = 0;

    /**
    Called when an announcement ends

    @param aType The type of announcement (travel or news)
    */
    virtual void MraoAnnouncementEnd(CMMRdsTunerUtility::TAnnouncementType aType) = 0;

    /**
    Called when an error occurs which results in announcement notifications
    becoming unavailable.
    */
    virtual void MraoError(TInt aError) = 0;
};

/**
The enhanced other networks observer mixin class defines the interface via
which changes to the enhanced other networks information can be notified. A
client interested in such information calls the function NotifyEonChange.
*/
class MMMRdsEonObserver
{
public:
    /**
    Called when some details of an EON station change. The station can be referenced
    to an existing one using it's Programme Identifier, which cannot change.

    @param aStation A TEonStation containing the new information about the station.
    @param aChanged The subset of the members of aStation that have changed. Bits are
            set according to TEonStation::TField.
    */
    virtual void MreoEonStationChanged(const TEonStation& aStation, TUint32 aChanged) = 0;

    /**
    Called when details of a new EON station are received.

    @param aStation A TEonStation containing the new information about the station.
    */
    virtual void MreoNewEonStation(const TEonStation& aStation) = 0;

    /**
    Called when details of <b>all</b> EON stations cease to be broadcast. This will
    typically happen when the tuner is retuned. More EON stations may continue to be
    recieved: this call does not indicate that EON information is no longer available.
    */
    virtual void MreoAllEonStationsRemoved() = 0;

    /**
    Called when details of an EON station cease to be broadcast.

    @param aPi The programme identifier of the EON station which has been removed.
    */
    virtual void MreoEonStationRemoved(const TRdsProgrammeIdentifier& aPi) = 0;

    /**
    Called when an error occurs resulting in EON notifications not being available.
    */
    virtual void MreoError(TInt aError) = 0;
};

#include <tuner.inl>

#endif // TUNER_H

// End of file

