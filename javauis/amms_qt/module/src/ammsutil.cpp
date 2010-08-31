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
* Description:  Static general reusable methods.
*
*/


// INCLUDE FILES
#include <e32math.h>
#include "ammsutil.h"
#include <cmmaplayer.h>
#include <cmmacontrol.h>
#include "cammscontrol.h"

// CONSTANTS
namespace
{
const TInt KAMMSStraightAngle = 180;

// Set a value to 0 if its absolute value is less than this.
const TReal KAMMSDiscreteErrorConstant = 0.00000001;
}

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAMMSControlGroup::FindControl
// Finds the control of a corresponding name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CMMAControl* AMMSUtil::FindControl(CMMAPlayer* aPlayer,
                                   const TDesC& aControlName,
                                   TAMMSControlTypes aControlType)
{
    TInt controlCount = aPlayer->ControlCount();
    TInt index = 0;

    // if control is not found NULL will be returned
    CMMAControl* control = NULL;

    // go through all controls
    while (index < controlCount)
    {
        CMMAControl* tmp = aPlayer->Control(index);

        // if the name of the Control matches the name used in finding
        if (tmp->ClassName() == aControlName)
        {

            // a base Control can be accepted always, it can not be derived
            if (aControlType == EAMMSBaseControl)
            {
                // found correct control
                control = tmp;

                // set index to stop while loop
                index = controlCount;
            }

            // in case the Control is not a base Control, check AMMS subtype
            else
            {
                if (aControlType == ((CAMMSControl*)tmp)->iControlType)
                {
                    // found correct (derived) control
                    control = tmp;

                    // set index to stop while loop
                    index = controlCount;
                }
                else
                {
                    // move to next control
                    index++;
                }
            }
        }

        // Control name was not the one used in finding
        else
        {
            // move to next control
            index++;
        }
    }
    return control;
}

// -----------------------------------------------------------------------------
// AMMSUtil::FromSphericalToCartesianL
// Converts vector from spherical to cartesian.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void AMMSUtil::FromSphericalToCartesianL(
    TInt aSphericalVector[ KAMMSVectorComponents ],
    TInt aCartesianVector[ KAMMSVectorComponents ])
{
    // convert to radians
    TReal elevation = aSphericalVector[ EElevation ] *
                      (KPi / KAMMSStraightAngle);
    TReal azimuth = aSphericalVector[ EAzimuth ] *
                    (KPi / KAMMSStraightAngle);
    TReal radius = aSphericalVector[ ERadius ];

    TReal elevationSin;
    TReal elevationCos;
    User::LeaveIfError(Math::Sin(elevationSin, elevation));
    User::LeaveIfError(Math::Cos(elevationCos, elevation));

    TReal cartesian[ KAMMSVectorComponents ];

    cartesian[ EComponentY ] = elevationSin * radius;
    TReal distXZ = elevationCos * radius; // distance in x-z plane

    TReal azimuthSin;
    TReal azimuthCos;
    User::LeaveIfError(Math::Sin(azimuthSin, azimuth));
    User::LeaveIfError(Math::Cos(azimuthCos, azimuth));

    // azimuth of 0 degrees points to negative z axis
    cartesian[ EComponentZ ] = -azimuthCos * distXZ;
    cartesian[ EComponentX ] = azimuthSin * distXZ;

    // round real values and convert them to integers
    RoundVectorL(cartesian, aCartesianVector);
}

// -----------------------------------------------------------------------------
// AMMSUtil::RotateVectorL
// Rotates a vector round the given axis.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void AMMSUtil::RotateVectorL(
    TReal aVector[ KAMMSVectorComponents ],
    TReal aAxisVector[ KAMMSVectorComponents ],
    TInt aAngle,
    TReal aRotatedVector[ KAMMSVectorComponents ])
{
    // calculate the length of the axis vector
    TReal lengthSquare = aAxisVector[ EComponentX ] *
                         aAxisVector[ EComponentX ] + aAxisVector[ EComponentY ] *
                         aAxisVector[ EComponentY ] + aAxisVector[ EComponentZ ] *
                         aAxisVector[ EComponentZ ];

    TReal length;
    User::LeaveIfError(Math::Sqrt(length, lengthSquare));

    // check that the vector is long enough
    __ASSERT_DEBUG(length > 0, User::Invariant());

    // normalize the axis vector
    TReal x = aAxisVector[ EComponentX ] / length;
    TReal y = aAxisVector[ EComponentY ] / length;
    TReal z = aAxisVector[ EComponentZ ] / length;

    // calculate sine and cosine values
    TReal angleRad = aAngle / 180.0 * KPi;
    TReal c;
    User::LeaveIfError(Math::Cos(c, angleRad));
    TReal s;
    User::LeaveIfError(Math::Sin(s, angleRad));

    // calculate some help variables
    TReal t = 1 - c;
    TReal txy = t * x * y;
    TReal txz = t * x * z;
    TReal tyz = t * y * z;
    TReal sz = s * z;
    TReal sy = s * y;
    TReal sx = s * x;
    TReal x2 = aVector[ EComponentX ];
    TReal y2 = aVector[ EComponentY ];
    TReal z2 = aVector[ EComponentZ ];

    // calculate new x value
    aRotatedVector[ EComponentX ] = (t * x * x + c) * x2 +
                                    (txy - sz) * y2 + (txz + sy) * z2;

    // calculate new y value
    aRotatedVector[ EComponentY ] = (txy + sz) * x2 +
                                    (t * y * y + c) * y2 + (tyz - sx) * z2;

    // calculate new z value
    aRotatedVector[ EComponentZ ] = (txz - sy) * x2 +
                                    (tyz + sx) * y2 + (t * z * z + c) * z2;

    // Remove error of discrete values.
    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        if (Abs(aRotatedVector[ i ]) < KAMMSDiscreteErrorConstant)       // CSI: 2 Wrong index means implementation error #
        {
            aRotatedVector[ i ] = 0;  // CSI: 2 Wrong index means implementation error #
        }
    }
}

// -----------------------------------------------------------------------------
// AMMSUtil::RoundVectorL
// Rounds vector components.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void AMMSUtil::RoundVectorL(
    TReal aVector[ KAMMSVectorComponents ],
    TInt aRoundedVector[ KAMMSVectorComponents ])
{
    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        TReal roundedValue;
        User::LeaveIfError(Math::Round(roundedValue,
                                       (TReal)aVector[ i ], 0));      // CSI: 2 Wrong index means implementation error #

        aRoundedVector[ i ] = (TInt)roundedValue;    // CSI: 2 Wrong index means implementation error #
    }
}

// -----------------------------------------------------------------------------
// AMMSUtil::MultiplyVector
// Multiplies a vector by a scalar.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void AMMSUtil::MultiplyVector(
    TReal aVector[ KAMMSVectorComponents ],
    TReal aMultiplier)
{
    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        aVector[ i ] *= aMultiplier;  // CSI: 2 Wrong index means implementation error #
    }
}

// -----------------------------------------------------------------------------
// AMMSUtil::CrossProduct
// Calculates vector cross product.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void AMMSUtil::CrossProduct(
    TReal aA[ KAMMSVectorComponents ],
    TReal aB[ KAMMSVectorComponents ],
    TReal aResultVector[ KAMMSVectorComponents ])
{
    // This function can handle only vectors with 3 components

    // Calculate the cross product.
    aResultVector[ EComponentX ] = -aA[ EComponentZ ] * aB[ EComponentY ] +
                                   aA[ EComponentY ] * aB[ EComponentZ ];

    aResultVector[ EComponentY ] = aA[ EComponentZ ] * aB[ EComponentX ] -
                                   aA[ EComponentX ] * aB[ EComponentZ ];

    aResultVector[ EComponentZ ] = -aA[ EComponentY ] * aB[ EComponentX ] +
                                   aA[ EComponentX ] * aB[ EComponentY ];
}

// -----------------------------------------------------------------------------
// AMMSUtil::VectorLengthL
// Calculates the length of the given vector.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TReal AMMSUtil::VectorLengthL(
    TReal aVector[ KAMMSVectorComponents ])
{
    TReal squareLength = 0;

    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        squareLength += aVector[ i ] * aVector[ i ];
    }

    TReal length;
    User::LeaveIfError(Math::Sqrt(length, squareLength));

    return length;
}

// -----------------------------------------------------------------------------
// AMMSUtil::ConvertToUnitVectorL
// Converts the given vector to unit vector.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void AMMSUtil::ConvertToUnitVectorL(
    TReal aVector[ KAMMSVectorComponents ])
{
    TReal length = VectorLengthL(aVector);

    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        aVector[ i ] /= length;  // CSI: 2 Wrong index means implementation error #
    }
}

// -----------------------------------------------------------------------------
// AMMSUtil::AreVectorsSimilar
// Checks whether two vectors are similar enough.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool AMMSUtil::AreVectorsSimilar(
    TReal aA[ KAMMSVectorComponents ],
    TInt aB[ KAMMSVectorComponents ],
    TInt aMaxComponentErrorPercentage)
{
    TReal maxRelativeError = aMaxComponentErrorPercentage / 100.0;  // CSI: 47 Value 100 means 100% #

    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        TReal maxError =
            Max(Abs(aA[ i ]), Abs(aB[ i ])) * maxRelativeError;        // CSI: 2 Wrong index means implementation error #

        if (Abs(aA[ i ] - aB[ i ]) > maxError)     // CSI: 2 Wrong index means implementation error #
        {
            return EFalse;
        }
    }

    // Vectors were similar enough.
    return ETrue;
}

// -----------------------------------------------------------------------------
// AMMSUtil::MaxVectorComponent
// Returns the maximum component in the given vector.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TReal AMMSUtil::MaxVectorComponent(
    TReal aVector[ KAMMSVectorComponents ])
{
    TReal maxValue = aVector[ 0 ];

    for (TInt i = 1; i < KAMMSVectorComponents; i++)
    {
        maxValue = Max(maxValue, aVector[ i ]);    // CSI: 2 Wrong index means implementation error #
    }

    return maxValue;
}

// -----------------------------------------------------------------------------
// AMMSUtil::MinVectorComponent
// Returns the minimum component in the given vector.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TReal AMMSUtil::MinVectorComponent(
    TReal aVector[ KAMMSVectorComponents ])
{
    TReal minValue = aVector[ 0 ];

    for (TInt i = 1; i < KAMMSVectorComponents; i++)
    {
        minValue = Min(minValue, aVector[ i ]);    // CSI: 2 Wrong index means implementation error #
    }

    return minValue;
}

//  End of File
