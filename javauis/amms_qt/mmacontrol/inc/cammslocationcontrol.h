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
* Description:  Manipulates the virtual location of an object.
*
*/


#ifndef CAMMSLOCATIONCONTROL_H
#define CAMMSLOCATIONCONTROL_H

//  INCLUDES
#include <LocationBase.h>
#include "cammscontrol.h"

// CLASS DECLARATION

/**
*
*  Controls for the Location Control effect.
*  This class delegates Location Control effect method calls to
*  CSourceLocation or CListenerLocation.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSLocationControl): public CAMMSControl
{
public:  // Constructors and destructor
    /**
    * Destructor.
    */
    ~CAMMSLocationControl();

public: // New functions
    /**
    * Moves the object to the new location.
    *
    * Sets the location using cartesian right-handed coordinates that are
    * relative to the origin. The measures are defined in units specified
    * by GlobalManager.getUnitsPerMeter().
    *
    * @param aX The x-coordinate of the new location.
    * @param aY The y-coordinate of the new location.
    * @param aZ The z-coordinate of the new location.
    *
    * @par Leaving:
    * @li \c KErrArgument - coordinates are not correct.
    */
    virtual void SetLocationCartesianL(TInt& aX, TInt& aY, TInt& aZ);

    /**
    * Gets the coordinates of the current location. The measures are
    * defined in units specified by GlobalManager.getUnitsPerMeter(). .
    *
    * @param aX The x-coordinate of the location.
    * @param aY The y-coordinate of the location.
    * @param aZ The z-coordinate of the location.
    */
    virtual void LocationCartesian(TInt& aX, TInt& aY, TInt& aZ);

    /**
    * Moves the object to the new location.
    *
    * Sets the new location using spherical coordinates. The negative
    * z-axis is the reference. That is, a location where both azimuth and
    * elevation are zero, is on the negative z-axis. Radius is defined in
    * units specified by GlobalManager.getUnitsPerMeter().
    *
    * @param aAzimuth The azimuth angle of the new location in degrees. The
    * azimuth is measured from the negative z-axis in the direction of the
    * x-axis.
    * @param aElevation The elevation angle of the new location in degrees.
    * The elevation is measured from the x-z-plane in the direction of the
    * y-axis.
    * @param aRadius The radius of the new location from the origin.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aRadius < 0.
    */
    virtual void SetLocationSphericalL(TInt& aAzimuth, TInt& aElevation,
                                       TInt& aRadius);

protected:
    /**
    * C++ default constructor, protected to allow access from derived class
    * @param aPlayer Player that has this control.
    */
    CAMMSLocationControl(CMMAPlayer* aPlayer);

protected:  // Data

    /* Native location effect, set from derived classes */
    CLocation* iLocationEffect;
};

#endif // CAMMSLOCATIONCONTROL_H


