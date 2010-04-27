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
* Description:  Manipulates the virtual orientation of an object.
*
*/


#ifndef CAMMSORIENTATIONCONTROL_H
#define CAMMSORIENTATIONCONTROL_H

//  INCLUDES
#include <OrientationBase.h>
#include "cammscontrol.h"

// CONSTANS
_LIT(KAMMSOrientationControl, "OrientationControl");

// CLASS DECLARATION

/**
*
*  Controls for the Orientation Control effect.
*  This class delegates Orientation Control effect method calls to
*  CListenerOrientation.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSOrientationControl): public CAMMSControl
{
public:  // Constructors and destructor
    /**
    * Destructor.
    */
    ~CAMMSOrientationControl();

public: // New functions
    /**
    * Turns the object to the new orientation.
    *
    * The new orientation is given using rotation angles. A zero vector
    * corresponds to the orientation pointing directly towards the negative
    * Z-axis. Orientation is applied in the following order: heading,
    * pitch, and roll. Therefore, notice that heading turns the X-axis and
    * therefore affects the pitch, and similarly heading and pitch turn the
    * Z-axis and therefore affect the roll.
    *
    * @param aHeading The rotation around the Y-axis in degrees.
    * @param aPitch The rotation around the X-axis in degrees.
    * @param aRoll The rotation around the Z-axis in degrees.
    */
    virtual void SetOrientationL(TInt aHeading, TInt aPitch, TInt aRoll);

    /**
    * Turns the object to the new orientation.
    *
    * The orientation is specified using two vectors, one specifying the
    * direction of the front vector of the object in world coordinates, and
    * another specifying the "above" vector of the object. The right and up
    * vectors of the object are calculated by first normalizing both source
    * vectors, then calculating the right vector as the cross product of the
    * "above" vector and the front vector, and the up vector as a cross
    * product of the front and right vectors.
    *
    * Because both vectors are normalized, they may be of any length.
    *
    * @param aFrontX X value of Front vector
    * @param aFrontY Y value of Front vector
    * @param aFrontZ Z value of Front vector
    * @param aAboveX X value of Above vector
    * @param aAboveY Y value of Above vector
    * @param aAboveZ Z value of Above vector
    *
    * @par Leaving:
    * @li \c KErrArgument - In case any of the parameters is a zero vector
    * or they are parallel to each other.
    * In that case, the orientation of the object remains unchanged.
    */
    virtual void SetOrientationL(TInt aFrontX,
                                 TInt aFrontY,
                                 TInt aFrontZ,
                                 TInt aAboveX,
                                 TInt aAboveY,
                                 TInt aAboveZ);

    /**
    * Gets the orientation of the object using two vectors.
    *
    * Sets the location using cartesian right-handed coordinates that are
    * relative to the origin. The measures are defined in units specified
    * by GlobalManager.getUnitsPerMeter().
    * Referenced memory of the arguments will contain the coordinate values.
    *
    * @param aFrontX X value of Front vector
    * @param aFrontY Y value of Front vector
    * @param aFrontZ Z value of Front vector
    * @param aAboveX X value of Above vector
    * @param aAboveY Y value of Above vector
    * @param aAboveZ Z value of Above vector
    */
    virtual void OrientationVectors(TInt& aFrontX,
                                    TInt& aFrontY,
                                    TInt& aFrontZ,
                                    TInt& aAboveX,
                                    TInt& aAboveY,
                                    TInt& aAboveZ);

protected:
    /**
    * C++ default constructor, protected to allow access from derived class
    * @param aPlayer Player that has this control.
    */
    CAMMSOrientationControl(CMMAPlayer* aPlayer);

protected:  // Data

    /* Native orientation effect, set from derived classes */
    COrientation* iOrientationEffect;
};

#endif // CAMMSORIENTATIONCONTROL_H


