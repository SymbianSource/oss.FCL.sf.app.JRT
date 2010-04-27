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
* Description:  Manipulates the settings of an audio effect called reverb.
*
*/



#ifndef CAMMSEMCREVERBCONTROL_H
#define CAMMSEMCREVERBCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <EnvironmentalReverbBase.h>
#include <EnvironmentalReverbUtility.h>
#include "cammseffectcontrol.h"
#include "cammseffectcontrolgroup.h"

//EMC
#include <EffectControl.h>
#include <ReverbControl.h>
#include <cammsbasereverbcontrol.h>
#include <cmmaemcaudioplayer.h>
//Utility for getting EnvironmentalReverb
#include "cammsemcreverbenvironmentalpresetutility.h"


using multimedia :: KReverbEffectControl;
using multimedia :: MReverbControl;

// CONSTANTS
_LIT(KAMMSEMCReverbControl, "ReverbControl");
_LIT(KAMMSEMCDefaultReverbPreset, "smallroom");

// CLASS DECLARATION

/**
*
*  Controls for the Reverb effect.
*  This class delegates Reverb effect method calls to CReverb.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSEMCReverbControl): public CAMMSBaseReverbControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSEMCReverbControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSEMCReverbControl();

public:  // New functions

    /**
    * Sets the gain level of the reverberation. The value defines what is
    * the relative level of the first reflections compared to the sound
    * source without possible distance attenuations, directivities or
    * obstructions taken into account.
    *
    * @param aLevel The new level of the reverberation in millibels.
    *
    * @return The value that was actually set.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aLevel is greater than 0
    */
    TInt SetReverbLevelL(TInt aLevel);

    /**
    * Sets the reverberation time of the reverb. The reverberation time is
    * the time taken for the reverberant sound to attenuate by 60 dB from
    * its initial level. Typical values are in the range from 100 to 20000
    * milliseconds.
    * The implementation might not support long reverberation times.
    * Therefore, the actual time used might be shorter than the time
    * specified with this method.
    *
    * @param aTime The new reverberation time in milliseconds.
    *
    * @return The value that was actually set.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aTime is negative.
    * From Java API more leave codes are:
    * - java.lang.IllegalArgumentException - when the given time is
    * negative
    * - javax.microedition.media.MediaException - when the changing of the
    * reverb time is not supported
    */
    void SetReverbTimeL(TInt aTime);

    /**
    * Gets the gain level of the reverberation.
    *
    * @return The level of the reverberation in millibels.
    */
    TInt ReverbLevel();

    /**
    * Gets the reverberation time.
    *
    * @return The time of the reverberation in milliseconds.
    */
    TInt ReverbTime();

    /**
    * Gets the minimum level of the reverberation.
    *
    * @return The minimum level of the reverberation in millibels.
    */
    TInt MinReverbLevel();

    /**
    * Gets the maximum level of the reverberation.
    *
    * @return The maximum level of the reverberation in millibels.
    */
    TInt MaxReverbLevel();

public: // Functions from base classes
    /**
    * Sets the effect according to the given preset.
    *
    * @param aPreset The new preset that will be taken into use.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aPreset is not available or it is null.
    */
    void SetPresetL(const TDesC& aPreset);

    /**
    * Gets the available preset names.
    *
    * @return The names of all the available preset modes.
    */
    const CDesCArray& PresetNamesL();

    /**
    * Gets the current preset.
    *
    * @return The preset that is set at the moment. If none of the presets
    * is set, null will be returned.
    */
    const TDesC& PresetL();

    /**
    * Enables/disables the effect.
    *
    * @param aEnabled The boolean value, true=enabled, false=disabled.
    *
    * @par Leaving:
    * From Java API the leave codes are:
    * - java.lang.IllegalStateException - if the effect cannot be enabled
    * in this state of the player.
    * - javax.microedition.media.MediaException - if enabling is not
    * supported (with the scope set).
    */
    void SetEnabledL(TBool aEnable);

    const TDesC& ClassName() const;

    /**
    * Prepares the Control.
    */
    void PrepareControlL();

    /**
    * Deallocates the Control.
    */
    void DeallocateControl();

public:  // Functions needed by reverb source control
    /**
    * Returns the Reverb Control.
    * CAMMSEMCReverbSourceControl has to use the same instance.
    * The function creates the instance if they do not exist yet.
    */
    MReverbControl * GetReverbControlL();


    /**
     * Returns the Current Preset Index.
     * CAMMSEMCReverbSourceControl has to use it.
     */

    TInt CurrentPresetIndex();

public: // override the virtual functions from base class CAMMSEffectControl

    void SetEnforcedL(TBool aEnforced);
    TBool Enforced();
    void SetScopeL(TInt aScope);
    TInt Scope();
    void ApplySettingsL();

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSEMCReverbControl(CMMAPlayer* aPlayer);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:  // Data

    /* Array for querying the preset names, owned */
    CDesCArray* iPresetNames;

    TBuf<KAMMSMaxPresetNameLength> iPresetName;

    /* Index of the current preset */
    TInt iCurrentPreset;

    /* Reverb utility to get Environmental Reverb*/
    CAMMSEMCReverbEnvironmentalPresetUtility *iAMMSEMCReverbEnvironmentalPresetUtility;
private:

    CMMAPlayer *iMMAPlayer;
    /**
    * EMC Reverb Control
    */

    CMultimediaFactory* iFactory;
    MStreamControl* iStreamControl;
    MReverbControl *iMReverbControl;
    TInt iEnvReverbLevel;

};

#endif // CAMMSEMCREVERBCONTROL_H


