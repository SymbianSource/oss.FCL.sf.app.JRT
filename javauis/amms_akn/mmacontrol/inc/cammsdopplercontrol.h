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
* Description:  Manipulates the settings of an effect called Doppler.
*
*/


#ifndef CAMMSDOPPLERCONTROL_H
#define CAMMSDOPPLERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <DopplerBase.h>
#include "cammscontrol.h"

// CLASS DECLARATION

/**
*
*  Controls for the Doppler effect.
*  This class delegates Doppler effect method calls to CDoppler.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSDopplerControl): public CAMMSControl
{
public:  // Destructor
    /**
    * Destructor.
    */
    ~CAMMSDopplerControl();

public: // New functions
    /**
    * Specifies if this Doppler effect is active or ignored. In case the
    * DopplerControl is fetched from the Spectator, this method does not
    * affect anything.
    *
    * @param aEnabled The boolean specifying if this Doppler effect is to
    * be applied.
    *
    * @par Leaving:
    * From Java API there are no leave codes, but the uset EffectControl's
    * method leaves with the sama way as in effect control, if:
    * - the effect cannot be enabled in this state of the player.
    * - enabling is not supported (with the scope set).
    */
    virtual void SetEnabledL(TBool aDopplerEnabled);

    /**
    * Returns whether this Doppler effect is currently active.
    *
    * @return The boolean, true if Doppler is being applied.
    */
    virtual TBool Enabled();

    /**
    * Sets the velocity, used in calculations for the Doppler effect.
    *
    * The velocity is specified using right-handed cartesian components in
    * units defined by GlobalManager.getUnitsPerMeter(). For example, if
    * the x parameter is specified to be 5000 and
    * GlobalManager.getUnitsPerMeter() returns 1000 the actual x component
    * will be 5 m/s. The same applies to y and z parameters.
    *
    * @param aX The x-coordinate of the new velocity.
    * @param aY The y-coordinate of the new velocity.
    * @param aZ The z-coordinate of the new velocity.
    *
    * @par Leaving:
    * @li \c KErrArgument - coordinates are not correct.
    */
    virtual void SetVelocityCartesianL(TInt aX, TInt aY, TInt aZ);

    /**
    * Returns the current velocity, used in calculations for the Doppler
    * effect. The velocity is specified using right-handed cartesian
    * components in units defined by GlobalManager.getUnitsPerMeter().
    * The referenced memory of arguments will contain the coordinate values.
    *
    * @param aX The x-coordinate of the velocity.
    * @param aY The y-coordinate of the velocity.
    * @param aZ The z-coordinate of the velocity.
    */
    virtual void VelocityCartesian(TInt& aX, TInt& aY, TInt& aZ);

    /**
    * Sets the velocity, used in calculations for the Doppler effect.
    *
    * The velocity is specified using spherical components. The radius
    * component is specified in units defined by
    * GlobalManager.getUnitsPerMeter(). For example, if the radius
    * parameter is specified to be 5000 and
    * GlobalManager.getUnitsPerMeter() returns 1000 the actual radius
    * component will be 5 m/s.
    *
    * @param aAzimuth The azimuth angle of the new velocity in degrees.
    * @param aElevation The elevation angle of the new velocity in degrees.
    * @param aRadius The magnitude of the new velocity (namely the speed).
    *
    * @par Leaving:
    * @li \c KErrArgument - coordinates are not correct.
    */
    virtual void SetVelocitySphericalL(TInt aAzimuth, TInt aElevation,
                                       TInt aRadius);

protected:
    /**
    * C++ default constructor, protected to allow access from derived class
    * @param aPlayer Player that has this control.
    */
    CAMMSDopplerControl(CMMAPlayer* aPlayer);


protected:  // Data

    /* Native Doppler effect, set from derived classes */
    CDoppler* iDopplerEffect;
};

#endif // CAMMSDOPPLERCONTROL_H


