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
* Description:  Controls how the sound is attenuated with its distance.
*
*/


#ifndef CAMMSEMCDISTANCEATTENUATIONCONTROL_H
#define CAMMSEMCDISTANCEATTENUATIONCONTROL_H

//  INCLUDES
#include <e32base.h>
#include<DistanceAttenuationControl.h>
#include <DistanceAttenuationData.h>
#include "cammsbasedistanceattenuationcontrol.h"

#include <cmmaemcaudioplayer.h>

using multimedia :: MDistanceAttenuationControl;
using multimedia :: KDistanceAttenuationEffectControl;


// CONSTANTS
_LIT(KAMMSEMCDistanceAttenuationControl, "DistanceAttenuationControl");

// CLASS DECLARATION

/**
*
*  Controls for the Distance Attenuation effect.
*  This class delegates Distance Attenuation effect method calls to
*  CDistanceAttenuation.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSEMCDistanceAttenuationControl): public CAMMSBaseDistanceAttenuationControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSEMCDistanceAttenuationControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSEMCDistanceAttenuationControl();

public: // Overriden the base class function

    /**
    * Sets all the 3D audio distance attenuation parameters simultaneously.
    * Distances are specified in units defined by
    * "GlobalManager.getUnitsPerMeter()"
    *
    * @param aMinDistance The minimum distance, below which the distance
    * gain is clipped to its maximum value of 1.0.
    * @param aMaxDistance The maximum distance, beyond which the distance
    * gain does not decrease any more. The exact behaviour of the gain at
    * distances beyond the maximum distance depends on the value of the
    * muteAfterMax.
    * @param aMuteAfterMax A boolean determining how the distance gain
    * behaves at distances greater than maxDistance: true if beyond the
    * maximum distance the source is silent; false if beyond the maximum
    * distance the source's gain is held constant at the level at the
    * maximum distance.
    * @param aRolloffFactor The rolloff factor, specified in thousandths
    * (1000 representing a rolloff factor of 1.0, 2000 representing 2.0 and
    * 500 representing 0.5). Higher values cause the distance gain to
    * attenuate more quickly.
    * @return The value that was actually set.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aMaxDistance <= \a aMinDistance,
    * \a aMinDistance <= 0, \a aMaxDistance <= 0 or \a aRolloffFactor < 0
    */
    void SetParametersL(
        TInt aMinDistance,
        TInt aMaxDistance,
        TBool aMuteAfterMax,
        TInt aRolloffFactor);

public: // Functions from base classes
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
    CAMMSEMCDistanceAttenuationControl(CMMAPlayer* aPlayer);

private:  // Data

    CMMAPlayer *iMMAPlayer;
    CMultimediaFactory* iFactory;
    MStreamControl* iStreamControl;
    MDistanceAttenuationControl *iMDistanceAttenuationControl;

};

#endif // CAMMSEMCDISTANCEATTENUATIONCONTROL_H


