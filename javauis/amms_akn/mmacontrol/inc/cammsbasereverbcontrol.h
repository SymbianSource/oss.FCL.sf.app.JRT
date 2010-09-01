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


#ifndef CAMMSBASEREVERBCONTROL_H
#define CAMMSBASEREVERBCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <EnvironmentalReverbBase.h>
#include <EnvironmentalReverbUtility.h>
#include <CustomCommandUtility.h>
#include "cammseffectcontrol.h"
// only require if platform support EMC
#ifdef RD_JAVA_HTTP_EMC_ENABLED
#include <ReverbControl.h>
#endif
// CONSTANTS
_LIT(KAMMSBaseReverbControl, "ReverbControl");
_LIT(KAMMSBaseDefaultReverbPreset, "smallroom");

// only require if platform support EMC
#ifdef RD_JAVA_HTTP_EMC_ENABLED
using multimedia :: MReverbControl;
#endif
// CLASS DECLARATION

/**
*
*  Controls for the Reverb effect.
*  This class delegates Reverb effect method calls to CReverb.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSBaseReverbControl): public CAMMSEffectControl
{
public:  // destructor

    /**
    * Destructor.
    */
    ~CAMMSBaseReverbControl();

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
    virtual TInt SetReverbLevelL(TInt aLevel) = 0;

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
    virtual void SetReverbTimeL(TInt aTime) = 0;

    /**
    * Gets the gain level of the reverberation.
    *
    * @return The level of the reverberation in millibels.
    */
    virtual TInt ReverbLevel() = 0;

    /**
    * Gets the reverberation time.
    *
    * @return The time of the reverberation in milliseconds.
    */
    virtual TInt ReverbTime() = 0;

    /**
    * Gets the minimum level of the reverberation.
    *
    * @return The minimum level of the reverberation in millibels.
    */
    virtual TInt MinReverbLevel() = 0;

    /**
    * Gets the maximum level of the reverberation.
    *
    * @return The maximum level of the reverberation in millibels.
    */
    virtual TInt MaxReverbLevel() = 0;

    /**
    * Gets the current preset index.
    *
    */
    virtual TInt CurrentPresetIndex() = 0;

public:

    virtual const TDesC& ClassName() const = 0;


public:  // Functions needed by reverb source control

    /**
     * Returns the environmental reverb utility.
     * ReverbSourceControl has to use the same instance.
     * The function creates the instance if they do not exist yet.
     *
     * @param aEnvironmentalReverbUtility Returned utility instance.
     */
    virtual void GetEnvironmentalReverbUtilityL(
        CEnvironmentalReverbUtility** aEnvironmentalReverbUtility);
    // only require if platform support EMC
#ifdef RD_JAVA_HTTP_EMC_ENABLED
    virtual MReverbControl* GetReverbControlL();
#endif
protected:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSBaseReverbControl(CMMAPlayer* aPlayer);


};

#endif // CAMMSBASEREVERBCONTROL_H


