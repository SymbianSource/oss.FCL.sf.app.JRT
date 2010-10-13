/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Control for tuner
*
*/


#ifndef CAMMSTUNERCONTROL_H
#define CAMMSTUNERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <tuner.h>

#include <CMMAControl.h>


// FORWARD DECLARATIONS
class CAMMSTunerPlayer;
class CAMMSTunerPresetsHandler;

// CLASS DECLARATION
/**
*  Control for tuner
*/

NONSHARABLE_CLASS(CAMMSTunerControl) : public CMMAControl,
        public MMMTunerObserver,
        public MMMTunerChangeObserver,
        public MMMTunerStereoObserver
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CAMMSTunerControl* NewL();

    /**
    * Destructor.
    */
    virtual ~CAMMSTunerControl();

public: // New functions

    /**
    * Getter for CMMTunerUtility
    * @return CMMTunerUtility
    */
    CMMTunerUtility* TunerUtility();

    /**
    * Gets the minimum frequency supported by this tuner
    * with the given modulation.
    * @return minimum frequency supported by this tuner
    */
    TInt MinFreqL();

    /**
    * Gets the maximum frequency supported by this tuner
    * with the given modulation.
    * @return maximum frequency supported by this tuner
    */
    TInt MaxFreqL();

    /**
    * Tunes to the given frequency or to the closest supported frequency.
    * @param aFreq - the frequency in Hertzs that will be taken into use
    * @return the frequency in 100 Hertzs that was taken into use

    */
    void SetFrequencyL(TInt aFreq);

    /**
    * Gets the frequency which the tuner has been tuned to.
    * @return the frequency to which the device has been tuned,
    *         in 100 Hertzs
    */
    TInt FrequencyL();

    /**
    * Seeks for the next broadcast signal.
    * @param aStartFreq - the frequency in Hertzs wherefrom the scan starts
    * @param aUpwards - if true, the scan proceeds towards higher
    *                   frequencies, otherwise towards lower frequencies
    * @return the found frequency
    */
    TInt SeekL(TInt aStartFreq, TBool aUpwards);

    /**
    * Gets the current squelching (muting in frequencies without broadcast)
    * setting.
    * @return true if squelch is on or false if squelch is off
    */
    TBool SquelchL();

    /**
    * Sets squelching on or off.
    * @param aSquelch - true to turn the squelch on or false
    *                   to turn the squelch off
    */
    void SetSquelchL(TBool aSquelch);

    /**
    * Gets the strength of the recepted signal.
    * @return a value between 0 and 100 where 0 means the faintest
    *         and 100 the strongest possible signal strength
    */
    TInt SignalStrengthL();

    /**
    * Gets the stereo mode in use.
    * @return stereo mode in use
    */
    TInt StereoModeL();

    /**
    * Sets the stereo mode.
    * @param aMode - the stereo mode to be used
    */
    void SetStereoModeL(TInt aMode);

    /**
    * Tunes the tuner by using settings specified in the preset.
    * @param aPreset - the preset to be used.
    */
    void UsePresetL(TInt aPreset);

    /**
    * Configures the preset using current frequency and modulation
    * and stereo mode.
    * @param aPreset - the preset to be set.
    */
    void SetPresetL(TInt aPreset);

    /**
    * Configures the preset using given settings.
    * @param aPreset - the preset to be set.
    * @param aFrequency - the frequency to be set.
    * @param aStereoMode - the stereo mode to be set.
    */
    void SetPresetL(TInt aPreset, TInt aFrequency, TInt aStereoMode);

    /**
    * Gets preset frequency
    * @param aPreset - the preset whose frequency is to be returned
    .       * @return the frequency of the preset in 100 Hertzs
    */
    TInt PresetFrequencyL(TInt aPreset);

    /**
    * Gets preset stereo mode
    * @param aPreset - the preset whose stereo mode is to be returned
    .       * @return the stereo mode of the preset.
    */
    TInt PresetStereoModeL(TInt aPreset);

    /**
    * Set preset name
    * @param aPreset - the preset whose name is to be set
    * @param aPresetName - the name of the preset
    */
    void SetPresetNameL(TInt aPreset, const TDesC* aPresetName);

    /**
    * Get preset name
    * @param aPreset - the preset whose name is asked
    * @param aPresetName - the name of the preset
    */
    void GetPresetNameL(TInt aPreset, TPtrC* aPresetName);

public: // functions from base classes

    // from CMMAControl

    const TDesC& ClassName() const;

    const TDesC& PublicClassName() const;

    // from MMMTunerObserver

    void MToTuneComplete(TInt aError);

    // from class MMMTunerChangeObserver

    /**
    Called when the tuned frequency changes

    @param aOldFrequency The frequency in use before the change
    @param aNewFrequency The new tuned frequency
    */
    void MTcoFrequencyChanged(const TFrequency& aOldFrequency,
                              const TFrequency& aNewFrequency);

    /**
    Called when the state of the tuner changes.

    @param aOldState The old state. Bits are set according to TTunerState.
    @param aNewState The new state. Bits are set according to TTunerState.
    */
    void MTcoStateChanged(const TUint32& aOldState,
                          const TUint32& aNewState);

    /**
    This function is called when an external antenna is detached from the device.
    This does not necessarily indicate that the tuner can no longer be used; the
    capabilities of the tuner indicate if the external antenna is required in order
    to use the tuner.
    */
    void MTcoAntennaDetached();

    /**
    This function is called when an external antenna is attached to the device. If
    the antenna is required to use the tuner, this indicates that the tuner can be
    used again.
    */
    void MTcoAntennaAttached();

    /**
    This function is called when the device enters or leaves flight mode. If the tuner
    cannot be used in flight mode when the device enters this mode, this indicates
    that the tuner can no longer be used; the capabilities of the tuner indicate if
    it can be used in flight mode or not.

    @param aFlightMode ETrue if the device has just entered flight mode, EFalse if
            flight mode has just been left.
    */
    void FlightModeChanged(TBool aFlightMode);

    // from class MMMTunerStereoObserver

    /**
    Called when stereo reception is lost/restored.

    @param aStereo If true, indicates that stereo reception has just been restored.
           If false, indicates that stereo reception has just been lost.
    */
    void MTsoStereoReceptionChanged(TBool aStereo);

    /**
    Called when a client enables/disabled forced mono reception.

    @param aForcedMono ETrue if reception is forced to be mono, even when a stereo
           signal is available.
    */
    void MTsoForcedMonoChanged(TBool aForcedMono);


private:

    /**
    * C++ default constructor.
    */
    CAMMSTunerControl();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    // Prohibit copy constructor if not deriving from CBase.
    // CAMMSTunerControl( const CAMMSTunerControl& );
    // Prohibit assigment operator if not deriving from CBase.
    // CAMMSTunerControl& operator=( const CAMMSTunerControl& );


private:    // Data

    TInt iMinFreq;
    TInt iMaxFreq;

    CMMTunerUtility*  iTunerUtility;
    CActiveSchedulerWait* iActiveSchedulerWait;
    CAMMSTunerPresetsHandler* iPresetsHandler;
};

#endif // CAMMSTUNERCONTROL_H

// End of File
