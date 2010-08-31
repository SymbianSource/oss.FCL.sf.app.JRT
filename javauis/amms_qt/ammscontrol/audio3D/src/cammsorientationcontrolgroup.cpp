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
* Description:  Group for orientation controls
*
*/


// INCLUDE FILES
#include <logger.h>
#include <e32math.h>
#include "cammsorientationcontrolgroup.h"
#include "cammsorientationcontrol.h"
#include "ammsutil.h"


// CONSTANTS
namespace
{
// since orientation vectors consist of integers rather than real values,
// normalized vectors have to be multiplied by a large constant to keep
// adequate precision
const TInt KAMMSAxisMultiplier = 1000;

// Some calculations are done with real values and then rounded to integers.
// Rounding may cause loss of precision. KAMMSMaxOrientationErrorPercentage
// tells how much error (in percentages) is acceptable when rounding values.
const TInt KAMMSMaxOrientationErrorPercentage = 5;
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSOrientationControlGroup* CAMMSOrientationControlGroup::NewLC()
{
    CAMMSOrientationControlGroup* self =
        new(ELeave) CAMMSOrientationControlGroup;

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;

}

// Destructor
CAMMSOrientationControlGroup::~CAMMSOrientationControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::OrientationVectorsL
// Gets the orientation of the object using two vectors
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSOrientationControlGroup::OrientationVectorsL(
    TInt aOrientation[ KAMMSTwoVectorComponents ])
{
    // If the orientation was set by using "front" and "above" vectors,
    // return "front" and "up" vectors.
    if (iCommited.iOrientationVector)
    {
        // Get "up" vector.
        TInt upVector[ KAMMSVectorComponents ];
        GetUpVectorL(iCommited, upVector);

        // Return the "front" and "up" vectors.
        for (TInt i = 0; i < KAMMSVectorComponents; i ++)
        {
            aOrientation[ i ] = iCommited.iOrientation[ i ];
            aOrientation[ i + KAMMSVectorComponents ] = upVector[ i ];
        }
    }
    // If the orientation was set by using angles, convert set angles
    // to "front" and "up" vectors.
    else
    {
        // Start rotation from these axis vectors.
        TReal xAxisVector[ KAMMSVectorComponents ] = { 1, 0, 0 };
        TReal yAxisVector[ KAMMSVectorComponents ] = { 0, 1, 0 };
        TReal zAxisVector[ KAMMSVectorComponents ] = { 0, 0, 1 };

        // Result axis vectors after rotation.
        TReal xAxisVector2[ KAMMSVectorComponents ];
        TReal yAxisVector2[ KAMMSVectorComponents ];
        TReal zAxisVector2[ KAMMSVectorComponents ];

        // rotate round y axis
        AMMSUtil::RotateVectorL(zAxisVector, yAxisVector,
                                iCommited.iOrientation[ EHeading ], zAxisVector2);
        AMMSUtil::RotateVectorL(xAxisVector, yAxisVector,
                                iCommited.iOrientation[ EHeading ], xAxisVector2);

        // rotate round x axis
        AMMSUtil::RotateVectorL(yAxisVector, xAxisVector2,
                                iCommited.iOrientation[ EPitch ], yAxisVector2);
        AMMSUtil::RotateVectorL(zAxisVector2, xAxisVector2,
                                iCommited.iOrientation[ EPitch ], zAxisVector);

        // rotate round z axis
        AMMSUtil::RotateVectorL(yAxisVector2, zAxisVector,
                                iCommited.iOrientation[ ERoll ], yAxisVector);

        // round and save "front" vector to the given parameter
        TReal roundedValue;
        for (TInt i = 0; i < KAMMSVectorComponents; i++)
        {
            User::LeaveIfError(Math::Round(roundedValue, zAxisVector[ i ] *    // CSI: 2 Wrong index means implementation error #
                                           KAMMSAxisMultiplier, 0));
            // "front" vector is opposite to z-axis
            aOrientation[ i ] = -(TInt)roundedValue;    // CSI: 2 Wrong index means implementation error #
        }

        // round and save "up" vector to the given parameter
        for (TInt e = 0; e < KAMMSVectorComponents; e++)
        {
            User::LeaveIfError(Math::Round(roundedValue, yAxisVector[ e ] *    // CSI: 2 Wrong index means implementation error #
                                           KAMMSAxisMultiplier, 0));
            aOrientation[ e + KAMMSVectorComponents ] = (TInt)roundedValue;    // CSI: 2 Wrong index means implementation error #
        }
    }
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::SetOrientationL
// Turns the object to the new orientation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSOrientationControlGroup::SetOrientationL(
    TInt aHeading,
    TInt aPitch,
    TInt aRoll)
{
    iUncommited.iOrientation[ EHeading ]    = aHeading;
    iUncommited.iOrientation[ EPitch ]      = aPitch;
    iUncommited.iOrientation[ ERoll ]       = aRoll;
    iUncommited.iOrientationVector = EFalse;

    UpdateL(EOrientation);
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::SetOrientationVectorsL
// Turns the object to the new orientation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSOrientationControlGroup::SetOrientationVectorsL(
    TInt aFrontVector[ KAMMSVectorComponents ],
    TInt aAboveVector[ KAMMSVectorComponents ])
{
    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        iUncommited.iOrientation[ i ] = aFrontVector[ i ];
        iUncommited.iOrientation[ i + KAMMSVectorComponents ] =
            aAboveVector[ i ];
    }
    iUncommited.iOrientationVector = ETrue;

    UpdateL(EOrientation);
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSOrientationControl*
CAMMSOrientationControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast<CAMMSOrientationControl*>(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::GetUpVectorL
// Calculates and returns "up" vector by using "front" and "above" vectors.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSOrientationControlGroup::GetUpVectorL(
    TVariables& aVariables,
    TInt aUpVector[ KAMMSVectorComponents ])
{
    __ASSERT_DEBUG(aVariables.iOrientationVector, User::Invariant());

    // Calculate first "right" vector and then "up" vector.
    // The "right" vector is calculated as cross product of the "front"
    // and "above" vectors.
    // The "up" vector is calculated as cross product of the "right"
    // and "front" vectors.

    TReal frontVector[ KAMMSVectorComponents ] =
    {
        aVariables.iOrientation[ EFrontX ],
        aVariables.iOrientation[ EFrontY ],
        aVariables.iOrientation[ EFrontZ ]
    };

    TReal aboveVector[ KAMMSVectorComponents ] =
    {
        aVariables.iOrientation[ EAboveX ],
        aVariables.iOrientation[ EAboveY ],
        aVariables.iOrientation[ EAboveZ ]
    };

    TReal rightVector[ KAMMSVectorComponents ];

    TReal vectorLength = Min(AMMSUtil::VectorLengthL(frontVector),
                             (TReal)KMaxTInt);

    // Perform cross product with unit vectors.
    AMMSUtil::ConvertToUnitVectorL(frontVector);
    AMMSUtil::ConvertToUnitVectorL(aboveVector);

    TReal upVector[ KAMMSVectorComponents ];

    AMMSUtil::CrossProduct(frontVector, aboveVector, rightVector);
    AMMSUtil::CrossProduct(rightVector, frontVector, upVector);

    AMMSUtil::ConvertToUnitVectorL(upVector);

    // Multiply the unit vector so that the result "up" vector has the same
    // length as the "front" vector.

    AMMSUtil::MultiplyVector(upVector, vectorLength);

    // Round the vector.
    AMMSUtil::RoundVectorL(upVector, aUpVector);

    // Check the rounding error. If it is too high, multiply the original
    // vector before rounding.
    if (!AMMSUtil::AreVectorsSimilar(upVector,
                                     aUpVector,
                                     KAMMSMaxOrientationErrorPercentage))
    {
        AMMSUtil::MultiplyVector(upVector, (TReal)KAMMSAxisMultiplier);

        // Accept this multiplied vector only if it can be fit into TInt.
        if ((AMMSUtil::MinVectorComponent(upVector) >= KMinTInt) &&
                (AMMSUtil::MaxVectorComponent(upVector) <= KMaxTInt))
        {
            AMMSUtil::RoundVectorL(upVector, aUpVector);
        }
    }
}


// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::ConvertOrientationToAnglesL
// Converts orientation from vectors to angles.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSOrientationControlGroup::ConvertOrientationToAnglesL(
    TVariables& aVariables,
    TInt aSphericalOrientation[ KAMMSVectorComponents ])
{
    // Get vector for y-axis.
    TInt temp[ KAMMSVectorComponents ];
    GetUpVectorL(aVariables, temp);
    TReal yAxisVector[] = { temp[ EComponentX ], temp[ EComponentY ],
                            temp[ EComponentZ ]
                          };

    AMMSUtil::ConvertToUnitVectorL(yAxisVector);


    // Get vector for z-axis.
    TReal frontVector[] =
    {
        aVariables.iOrientation[ EFrontX ],
        aVariables.iOrientation[ EFrontY ],
        aVariables.iOrientation[ EFrontZ ]
    };

    // Z-axis is opposite to the front vector.
    AMMSUtil::ConvertToUnitVectorL(frontVector);
    TReal zAxisVector[ KAMMSVectorComponents ] =
    {
        -frontVector[ EFrontX ],
        -frontVector[ EFrontY ],
        -frontVector[ EFrontZ ]
    };


    // Calculate orientation vector for the x-axis.
    // The vector is calculated as cross product of the y-
    // and z-axis vectors.
    TReal xAxisVector[ KAMMSVectorComponents ];

    AMMSUtil::CrossProduct(yAxisVector, zAxisVector, xAxisVector);
    AMMSUtil::ConvertToUnitVectorL(xAxisVector);


    // First rotate the object coordinate axels so that the object y-axel is
    // parallel to the global y-axis (this gives us roll and pitch angles).
    // Then rotate the object coordinate axels so that x- and z-axels are
    // parallel to the global x- and z-axels, respectively (this gives the
    // heading).


    // Calculate how much the axels should be rotated round the z-axis.
    TReal rotateRollDeg = CalculatePartialRotationL(yAxisVector, zAxisVector, EComponentY);

    AMMSUtil::RotateVectorL(xAxisVector, zAxisVector, rotateRollDeg, xAxisVector);
    AMMSUtil::RotateVectorL(yAxisVector, zAxisVector, rotateRollDeg, yAxisVector);


    // Calculate how much the axels should be rotated round the x-axis.
    TReal rotatePitchDeg = CalculatePartialRotationL(yAxisVector, xAxisVector, EComponentY);

    AMMSUtil::RotateVectorL(yAxisVector, xAxisVector, rotatePitchDeg, yAxisVector);
    AMMSUtil::RotateVectorL(zAxisVector, xAxisVector, rotatePitchDeg, zAxisVector);

    // Calculate how much the axels should be rotated round the y-axis.
    TReal rotateHeadingDeg = CalculatePartialRotationL(xAxisVector, yAxisVector, EComponentX);

    TReal resultAngles[] = { -rotateHeadingDeg,
                             -rotatePitchDeg,
                             -rotateRollDeg
                           };

    AMMSUtil::RoundVectorL(resultAngles, aSphericalOrientation);
}


// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::CalculatePartialRotationL
// Calculates how much the given vector should be rotated around the given
// rotation vector so that the value of the specified vector component is
// maximized.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TReal CAMMSOrientationControlGroup::CalculatePartialRotationL(
    TReal aVector[ KAMMSVectorComponents ],
    TReal aRotationAxelVector[ KAMMSVectorComponents ],
    TInt aMaximizeComponent)
{
    __ASSERT_DEBUG((aMaximizeComponent >= EComponentX) &&
                   (aMaximizeComponent <= EComponentZ),
                   User::Invariant());

    TReal x = aRotationAxelVector[ EComponentX ];
    TReal y = aRotationAxelVector[ EComponentY ];
    TReal z = aRotationAxelVector[ EComponentZ ];

    // calculate the length of the axis vector
    TReal lengthSquare = x * x + y * y + z * z;
    TReal length;
    User::LeaveIfError(Math::Sqrt(length, lengthSquare));

    // check that the vector is long enough
    __ASSERT_DEBUG(length > 0, User::Invariant());


    // Find out the components that are tried to set 0.
    TInt zeroComponents[ KAMMSVectorComponents - 1 ];
    TInt counter = 0;

    for (TInt i = 0; i < KAMMSVectorComponents; i++)
    {
        if (i != aMaximizeComponent)
        {
            zeroComponents[ counter ] = i;
            counter++;
        }
    }

    // normalize the axis vector
    x = x / length;
    y = y / length;
    z = z / length;

    // calculate some help variables
    TReal xx = x * x;
    TReal yy = y * y;
    TReal zz = z * z;

    TReal x2 = aVector[ EComponentX ];
    TReal y2 = aVector[ EComponentY ];
    TReal z2 = aVector[ EComponentZ ];

    TReal xx2 = x * x2;
    TReal yy2 = y * y2;
    TReal zz2 = z * z2;

    TReal xxx2 = xx * x2;
    TReal yyy2 = yy * y2;
    TReal zzz2 = zz * z2;

    TReal constants[] =
    {
        xxx2 + x *(yy2 + zz2),
        yyy2 + y *(xx2 + zz2),
        zzz2 + z *(xx2 + yy2)
    };

    TReal cosMultipliers[] =
    {
        constants[ EComponentX ] - x2,
        constants[ EComponentY ] - y2,
        constants[ EComponentZ ] - z2
    };

    TReal sinMultipliers[] =
    {
        z * y2 - y * z2,
        x * z2 - z * x2,
        y * x2 - x * y2
    };

    // The angles where some component of the vector is zero.
    TReal zeroAngles[ 4 ];  // CSI: 47 Two components may have zero points in 4 angles. #
    TInt angleCounter = 0;

    // Find rotation angles that makes (at least) one of the vector components
    // to be 0 (however, dismiss the component to be maximized).
    for (int i = 0; i < KAMMSVectorComponents - 1; i++)
    {
        TInt componentIndex = zeroComponents[ i ];

        TReal cosMultiplier = cosMultipliers[ componentIndex ];
        TReal sinMultiplier = sinMultipliers[ componentIndex ];

        TReal angleDeg = 0;

        // If both multipliers are zero, this vector is parallel or opposite
        // to the target vector. Thus, no need to calculate the angle.
        if ((cosMultiplier != 0) ||
                (sinMultiplier != 0))
        {
            // Calculate the angle that makes this vector element to be 0.
            TReal temp = cosMultiplier * cosMultiplier +
                         sinMultiplier * sinMultiplier;

            TReal divider = 0;
            User::LeaveIfError(Math::Sqrt(divider, temp));

            TReal arcTan = 0;
            User::LeaveIfError(
                Math::ATan(arcTan, sinMultiplier, cosMultiplier));

            TReal arcCos = 0;
            User::LeaveIfError(Math::ACos(
                                   arcCos, constants[ i ] / divider));    // CSI: 2 Wrong index means implementation error #

            TReal angleRad = arcCos + arcTan;

            // Save the angle in degrees.
            angleDeg = angleRad * 180 / KPi;  // CSI: 47 Value 180 means 180 degrees #
        }

        zeroAngles[ angleCounter ] = angleDeg;

        zeroAngles[ angleCounter + 1 ] = angleDeg + 180; // CSI: 47 Save also the opposite direction (+ 180 degrees) #

        angleCounter += 2;  // CSI: 47 Two angles have been found #
    }


    // Find the angle that gives the maximum value for the component
    // to be maximized.
    TReal maxValue = -0xFFFF;
    TInt maxIndex = KErrNotFound;

    for (TInt i = 0; i < angleCounter; i++)
    {
        // Rotate the original vector.
        TReal rotatedVector[ KAMMSVectorComponents ];

        AMMSUtil::RotateVectorL(aVector, aRotationAxelVector,
                                zeroAngles[ i ], rotatedVector);   // CSI: 2 Wrong index means implementation error #

        // Check the value of the component that should be maximized.
        TReal value = rotatedVector[ aMaximizeComponent ];

        if (value > maxValue)
        {
            maxValue = value;
            maxIndex = i;
        }
    }

    return zeroAngles[ maxIndex ];
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSOrientationControlGroup::ClassName()
{
    return KAMMSOrientationControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::CommitL
// Transfers all the pending parameters to the audio processing system.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSOrientationControlGroup::CommitL(TInt aCommit)
{
    if (aCommit & EOrientation)
    {
        TInt controls = ControlCount();

        if (controls > 0)
        {
            TInt angleVector[] =
            {
                iUncommited.iOrientation[ EHeading ],
                iUncommited.iOrientation[ EPitch ],
                iUncommited.iOrientation[ ERoll ]
            };

            // If the orientation was given in vectors, convert
            // the vectors to angles because at the moment the Effect API
            // can handle only angles.
            if (iUncommited.iOrientationVector)
            {
                ConvertOrientationToAnglesL(iUncommited, angleVector);
            }

            for (TInt i = 0; i < controls; i++)
            {
                CAMMSOrientationControl* control = TypeSafeControl(i);

                control->SetOrientationL(
                    angleVector[ EHeading ],
                    angleVector[ EPitch ],
                    angleVector[ ERoll ]);
            }
        }

        iCommited = iUncommited;
    }
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSOrientationControlGroup::NotifyPlayerAddedL(
    CMMAPlayer* aPlayer,
    CMMAControl* aControl)
{
    CAMMSAudio3DControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSOrientationControl* control =
        static_cast<CAMMSOrientationControl*>(aControl);

    // set the current parameters
    if (iCommited.iOrientationVector)
    {
        TInt angleVector[ KAMMSVectorComponents ];

        ConvertOrientationToAnglesL(iCommited, angleVector);

        control->SetOrientationL(
            angleVector[ EHeading ],
            angleVector[ EPitch ],
            angleVector[ ERoll ]);
    }
    else
    {
        control->SetOrientationL(
            iCommited.iOrientation[ EHeading ],
            iCommited.iOrientation[ EPitch ],
            iCommited.iOrientation[ ERoll ]);
    }
}

// -----------------------------------------------------------------------------
// CAMMSOrientationControlGroup::CAMMSOrientationControlGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSOrientationControlGroup::CAMMSOrientationControlGroup()
        : CAMMSAudio3DControlGroup(KAMMSOrientationControl)
{
    // Default orientation is:
    // heading = 0
    // pitch = 0
    // roll = 0
    iCommited.iOrientationVector = EFalse;
}

//  End of File
