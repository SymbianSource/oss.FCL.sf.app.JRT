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
* Description:  Static general reusable methods.
*
*/


#ifndef AMMSUTIL_H
#define AMMSUTIL_H

//  INCLUDES
#include <e32base.h>
#include "ammsconstants.h"

// FORWARD DECLARATIONS
class CMMAPlayer;
class CMMAControl;

// CLASS DECLARATION

/**
*  Static general reusable methods.
*
*  This class contains only static method which can be used in many classes.
*
*  @since 3.0
*/
NONSHARABLE_CLASS(AMMSUtil)
{
public: // New functions
    /**
     * Finds the control of a corresponding type that
     * belongs to the player given as a parameter
     *
     * @param aPlayer The Player where to find the Control
     * @param aControlName Name of the control to look for
     * @param aControlType Special AMMS type of the Control
     * @return Control of type aControlName or NULL if not found
     */
    static CMMAControl* FindControl(
        CMMAPlayer* aPlayer,
        const TDesC& aControlName,
        TAMMSControlTypes aControlType = EAMMSBaseControl);

    /**
      * Converts vector from spherical to cartesian.
      *
      * @param aSphericalVector Spherical vector to be converted
      * @param aCartesianVector Result cartesian vector
      */
    static void FromSphericalToCartesianL(
        TInt aSphericalVector[ KAMMSVectorComponents ],
        TInt aCartesianVector[ KAMMSVectorComponents ]);

    /*
     * Rotates a vector round the given axis. The starting point of each
     * vectors is in the origo, and thus, only coordinates of the ending
     * point should be given.
     *
     * @param aVector X, Y, and Z value of a vector to be rotated
     * @param aAxisVector X, Y, and Z value of an axis vector
     * @param aAngle Rotation angle in degrees
     * @param aRotatedVector X, Y, and Z value of the rotated vector
     */
    static void RotateVectorL(TReal aVector[ KAMMSVectorComponents ],
                              TReal aAxisVector[ KAMMSVectorComponents ],
                              TInt aAngle,
                              TReal aRotatedVector[ KAMMSVectorComponents ]);

    /**
     * Rounds each component in the given vector.
     *
     * @param aVector A vector to be rounded
     * @param aRoundedVector Rounded vector
     */
    static void RoundVectorL(TReal aVector[ KAMMSVectorComponents ],
                             TInt aRoundedVector[ KAMMSVectorComponents ]);

    /**
     * Multiplies the given vector by the given scalar value.
     *
     * @param aVector A vector to be rounded
     * @param aMultiplier A multiplier
     */
    static void MultiplyVector(TReal aVector[ KAMMSVectorComponents ],
                               TReal aMultiplier);

    /**
     * Calculates vector cross product.
     * @param aA A vector having KAMMSVectorComponents elements
     * @param aB A vector having KAMMSVectorComponents elements
     * @param aResultVector Result of the cross product
     */
    static void CrossProduct(TReal aA[ KAMMSVectorComponents ],
                             TReal aB[ KAMMSVectorComponents ],
                             TReal aResultVector[ KAMMSVectorComponents ]);

    /**
     * Calculates the length of the given vector.
     *
     * @param aVector A vector whose length is calculated
     */
    static TReal VectorLengthL(TReal aVector[ KAMMSVectorComponents ]);

    /**
     * Converts the given vector to the unit vector. The original vector
     * is replaced by the result vector.
     *
     * @param aVector A vector that is converted to the unit vector and is
     * replaced by the result vector
     */
    static void ConvertToUnitVectorL(TReal aVector[ KAMMSVectorComponents ]);

    /**
     * Checks whether two given vectors are similar according to the given
     * maximum error percentage. The function compares each component in
     * aA to the corresponding component in aB.
     *
     * @param aA A vector
     * @param aB A vector
     * @param aMaxComponentErrorPercentage Maximum error percentage between
     * a component in aA and the corresponding component in aB
     * @return ETrue if difference between each component pair is lower
     * than the given error, else ETrue is returned
     */
    static TBool AreVectorsSimilar(TReal aA[ KAMMSVectorComponents ],
                                   TInt aB[ KAMMSVectorComponents ],
                                   TInt aMaxComponentErrorPercentage);

    /**
     * Returns the maximum component value in the given vector.
     *
     * @param aVector A vector
     * @return Maximum component value in aVector
     */
    static TReal MaxVectorComponent(TReal aVector[ KAMMSVectorComponents ]);

    /**
     * Returns the minimum component value in the given vector.
     *
     * @param aVector A vector
     * @return Minimum component value in aVector
     */
    static TReal MinVectorComponent(TReal aVector[ KAMMSVectorComponents ]);

private:
    /**
    * C++ default constructor. Private to not allow construction.
    */
    AMMSUtil();
};

#endif // AMMSUTIL_H


