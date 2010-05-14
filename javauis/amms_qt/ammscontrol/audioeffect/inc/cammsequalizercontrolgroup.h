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
* Description:  Group for equalizer controls
*
*/


#ifndef CAMMSEQUALIZERCONTROLGROUP_H
#define CAMMSEQUALIZERCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammseffectcontrolgroup.h"
#include <AudioEqualizerData.h>

// CONSTANTS
_LIT(KAMMSEqualizerControlClassName,
     ".amms.control.audioeffect.EqualizerControl");


// FORWARD DECLARATIONS
class CAMMSBaseEqualizerControl;
class CAudioEqualizerUtility;


// CLASS DECLARATION
/**
 *  Group for equalizer controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSEqualizerControlGroup): public CAMMSEffectControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSEqualizerControlGroup* NewLC();

    /**
     * Destructor.
     */
    ~CAMMSEqualizerControlGroup();

public: // New functions
    /**
     * Gets the band that has the most effect on the given frequency
     *
     * @param aFrequency    The frequency in milliHertz which is
     *                      to be equalized via the returned band
     *
     * @return The frequency band that has most effect on the given
     * frequency or -1 if no band has effect on the given frequency
     */
    TInt Band(TInt aFrequency);

    /**
     * Gets the gain set for the given equalizer band.
     *
     * @param aBand         The frequency band whose gain is asked.
     * @param aBandLevel    The returned level for the given band in millibels.
     *
     */
    void GetBandLevelL(TInt aBand, TInt& aBandLevel);

    /**
     * Gets the bass level.
     *
     * @return The current level that is set to the bass band. If the
     * bass level cannot been defined EqualizerControl.UNDEFINED will
     * be returned
     */
    TInt Bass();

    /**
     * Gets the center frequency of the given band
     *
     * @param aBand         The frequency band whose center frequency
     *                      is asked.
     * @param aCenterFreq   The returned center frequency in milliHertz.
     */
    void GetCenterFreqL(TInt aBand, TInt& aCenterFreq);

    /**
     * Returns the maximum band level supported
     *
     * @return the maximum band level supported
     */
    TInt MaxBandLevel();

    /**
     * Returns the minimum band level supported.
     *
     * @return the minimum band level supported.
     */
    TInt MinBandLevel();

    /**
     * Gets the number of frequency bands that the equalizer supports.
     *
     * @return the number of frequency bands that the equalizer supports.
     */
    TInt NumberOfBands();

    /**
     * Gets the treble level.
     *
     * @return The current level that is set to the treble band. If the
     * treble level cannot been defined EqualizerControl.UNDEFINED will
     * be returned
     */
    TInt Treble();

    /**
     * Sets the given equalizer band to the given gain value.
     *
     * @param aLevel    The new gain in millibels that will be set to the
     *                  given band
     * @param aBand     The frequency band that will have the new gain
     */
    void SetBandLevelL(TInt aLevel, TInt aBand);

    /**
     * Sets the bass level using a linear point scale with values between
     * 0 and 100: a value of 0 applies the maximum available attenuation
     * to frequencies in the bass band; a value of 50 gives a flat
     * equalization of the bass band; and a value of 100 applies the
     * maximum available amplification to frequencies in the bass band.
     *
     * @param aLevel    The new level on a linear point scale that will
     *                  be set to the bass band
     * @param aSetLevel Returned level that was actually set
     */
    void SetBassL(TInt aLevel, TInt& aSetLevel);

    /**
     * Sets the treble level using a linear point scale with values between
     * 0 and 100: a value of 0 applies the maximum available attenuation to
     * frequencies in the treble band; a value of 50 gives a flat
     * equalization of the treble band; and a value of 100 applies the
     * maximum available amplification to frequencies in the treble band
     *
     * @param aLevel    The new level on a linear point scale that will
     *                  be set to the treble band
     *
     * @param aSetLevel Returned level that was actually set
     */
    void SetTrebleL(TInt aLevel, TInt& aSetLevel);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSBaseEqualizerControl* TypeSafeControl(TInt aIndex) const;

    /**
     * Checks whether the band index is between 0 and number of bands -1.
     * Function leaves with KErrArgument if the band is not between the
     * limits.
     */
    void CheckBandIndexL(TInt aBand) const;

    /**
     * Gets bands corresponding to the given preset index.
     * The data is obtained from controls (or from empty group utility, if the
     * group has no controls).
     * @param aPresetIndex Index of the preset whose bands are asked
     * @param aBands Returned bands
     */
    void GetPresetBandsL(TUint aPresetIndex,
                         RArray< TEfAudioEqualizerBand >& aBands);

public: // Functions from base classes
    /**
     * Returns class name that identifies this control group.
     *
     * @return Control group name.
     */
    const TDesC16& ClassName();

protected: // Functions from base classes
    /**
     * Called by when a new player is added
     *
     * @param aPlayer   The player being added
     * @param aControl  The player's control relevant to this group
     */
    void NotifyPlayerAddedL(CMMAPlayer *aPlayer, CMMAControl* aControl);

    /**
     * Called when the current preset changes
     */
    void PresetChangedL();

    /**
     * Finish initialization (after the 1st player is added)
     */
    void InitializeL();

    /**
     * Creates utilities that can be used to obtain preset names
     * and preset data (needed when the group has no controls).
     * Does nothing if the utility already exists.
     */
    void PrepareEmptyGroupUtilitiesL();

    /**
     * Deletes utilities that are used to obtain preset names
     * and preset data (needed when the group has no controls).
     * Does nothing if the utilities have already been deleted.
     */
    void DeleteEmptyGroupUtilities();

    /**
     * Gets list of preset names available.
     * @param aPresetNames Returned preset names
     */
    void GetPresetNamesL(CDesCArray& aPresetNames);

protected:
    /**
     * C++ default constructor.
     */
    CAMMSEqualizerControlGroup();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // data

    // Needed to get preset names when the group has no controls.
    CAudioEqualizerUtility* iEmptyAudioEqualizerUtility;  // Owned.

    RArray< TEfAudioEqualizerBand > iBands;
    TInt iBass; // bass level
    TInt iTreble; // treble level
    TInt iMaxBandLevel;
    TInt iMinBandLevel;
};

#endif // CAMMSEQUALIZERCONTROLGROUP_H


