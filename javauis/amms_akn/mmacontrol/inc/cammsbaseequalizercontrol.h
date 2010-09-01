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


#ifndef CAMMSBASEEQUALIZERCONTROL_H
#define CAMMSBASEEQUALIZERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include "cammseffectcontrol.h"

// CONSTANTS
_LIT(KAMMSBaseEqualizerControl, "EqualizerControl");
const TInt KAMMSHalfOfSamplingFrequency = 24000000; //in milliHertz
const TInt KAMMSBandOffset = 1; // Band 0 in JSR-234 equals Band 1 in Effect API

// CLASS DECLARATION

/**
*
*  Controls for the Equalizer effect.
*
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSBaseEqualizerControl): public CAMMSEffectControl
{
public:
    /**
     * Destructor.
     */
    ~CAMMSBaseEqualizerControl();

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
    virtual TInt BandLevelL(TInt aBand) = 0;

    /**
    * Returns the maximum band level supported.
    *
    * @return The maximum band level in millibels.
    */
    virtual TInt MaxBandLevel() = 0;

    /**
    * Returns the minimum band level supported.
    *
    * @return The minimum band level in millibels.
    */
    virtual TInt MinBandLevel() = 0;

    /**
    * Returns the band width in Hz for the specified band.
    *
    * @param aBand The frequency band whose band width is asked.
    * The numbering of the bands starts from 0 and ends at
    * (getNumberOfBands() - 1).
    *
    * @return The band width in Hz for the specified band.
    */
    virtual TInt BandWidth(TInt aBand) = 0;

    /**
    * Returns the center frequency in Hz for a given band.
    *
    * @param aBand The frequency band whose center frequency
    * is asked. The numbering of
    * the bands starts from 0 and ends at (getNumberOfBands() - 1).
    *
    * @return The center frequency in Hz for a given band.
    */
    virtual TInt CenterFrequency(TInt aBand) = 0;

    /**
    * Returns the cross-over frequency between the given frequency
    * band (aBand) and the next band.
    *
    * @param aBand The frequency band whose gain is asked. The numbering of
    * the bands starts from 0 and ends at (getNumberOfBands() - 1).
    *
    * @return Crossover frequency.
    */
    virtual TInt CrossoverFrequency(TInt aBand) = 0;

    /**
    * Gets the number of frequency bands that the equalizer supports.
    *
    * @return The number of frequency bands that the equalizer supports.
    */
    virtual TInt NumberOfBands() = 0;

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
    virtual void SetBandLevelL(TInt aLevel, TInt aBand) = 0;

public:
    virtual const TDesC& ClassName() const = 0;

protected:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSBaseEqualizerControl(CMMAPlayer* aPlayer);

};

#endif // CAMMSBASEEQUALIZERCONTROL_H


