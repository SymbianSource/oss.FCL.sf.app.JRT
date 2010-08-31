/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manipulates the equalization settings of a Player.
*
*/


#ifndef CAMMSEQUALIZERCONTROL_H
#define CAMMSEQUALIZERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <AudioEqualizerBase.h>
#include <AudioEqualizerUtility.h>
#include <CustomCommandUtility.h>
#include "cammsbaseequalizercontrol.h"

// CONSTANTS
_LIT(KAMMSEqualizerControl, "EqualizerControl");


// CLASS DECLARATION

/**
*
*  Controls for the Equalizer effect.
*  This class delegates Equalizer effect method calls to CAudioEqualizer.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSEqualizerControl): public CAMMSBaseEqualizerControl
{
public:
    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSEqualizerControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSEqualizerControl();

public: // New functions

    /**
    * Gets the gain set for the given equalizer band.
    *
    * @param aBand The frequency band whose gain is asked. The numbering of
    * the bands starts from 0 and ends at (getNumberOfBands() - 1).
    *
    * @return The gain set for the given band in millibels.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aBand is out of range.
    */
    TInt BandLevelL(TInt aBand);

    /**
    * Returns the maximum band level supported.
    *
    * @return The maximum band level in millibels.
    */
    TInt MaxBandLevel();

    /**
    * Returns the minimum band level supported.
    *
    * @return The minimum band level in millibels.
    */
    TInt MinBandLevel();

    /**
    * Returns the band width in Hz for the specified band.
    *
    * @param aBand The frequency band whose band width is asked.
    * The numbering of the bands starts from 0 and ends at
    * (getNumberOfBands() - 1).
    *
    * @return The band width in Hz for the specified band.
    */
    TInt BandWidth(TInt aBand);

    /**
    * Returns the center frequency in Hz for a given band.
    *
    * @param aBand The frequency band whose center frequency
    * is asked. The numbering of
    * the bands starts from 0 and ends at (getNumberOfBands() - 1).
    *
    * @return The center frequency in Hz for a given band.
    */
    TInt CenterFrequency(TInt aBand);

    /**
    * Returns the cross-over frequency between the given frequency
    * band (aBand) and the next band.
    *
    * @param aBand The frequency band whose gain is asked. The numbering of
    * the bands starts from 0 and ends at (getNumberOfBands() - 1).
    *
    * @return Crossover frequency.
    */
    TInt CrossoverFrequency(TInt aBand);

    /**
    * Gets the number of frequency bands that the equalizer supports.
    *
    * @return The number of frequency bands that the equalizer supports.
    */
    TInt NumberOfBands();

    /**
    * Sets the given equalizer band to the given gain value.
    *
    * @param aLevel The new gain in millibels that will be set to the given
    * band. getMinBandLevel() and getMaxBandLevel() will define the maximum
    * and minimum values.
    * @param aBand The frequency band that will have the new gain. The
    * numbering of the bands starts from 0 and ends at
    * (getNumberOfBands() - 1).
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aBand or \a aLevel is out of range.
    */
    void SetBandLevelL(TInt aLevel, TInt aBand);

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

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSEqualizerControl(CMMAPlayer* aPlayer);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:  // Data

    /** Native audio equalizer */
    CAudioEqualizerUtility* iEqualizerUtility;

    /* Array for querying the preset names, owned */
    CDesCArray* iPresetNames;

    /* Index of the current preset */
    TInt iCurrentPreset;

};

#endif // CAMMSEQUALIZERCONTROL_H


