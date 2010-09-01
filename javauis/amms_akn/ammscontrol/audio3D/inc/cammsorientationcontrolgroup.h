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


#ifndef CAMMSORIENTATIONCONTROLGROUP_H
#define CAMMSORIENTATIONCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammsaudio3dcontrolgroup.h"
#include "ammsconstants.h"

// CONSTANTS
_LIT(KAMMSOrientationControlClassName,
     ".amms.control.audio3d.OrientationControl");

// FORWARD DECLARATIONS
class CAMMSOrientationControl;


// CLASS DECLARATION
/**
 *  Group for location controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSOrientationControlGroup): public CAMMSAudio3DControlGroup
{
private:

    class TVariables
    {
    public:

        TInt iOrientation[ KAMMSTwoVectorComponents ];
        TBool iOrientationVector;
    };

public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSOrientationControlGroup* NewLC();

    /**
     * Destructor.
     */
    ~CAMMSOrientationControlGroup();

public: // New functions
    /**
    * Gets the orientation of the object using two vectors.
    *
    * Sets the location using cartesian right-handed coordinates that are
    * relative to the origin. The measures are defined in units specified
    * by GlobalManager.getUnitsPerMeter().
    * Referenced memory of the arguments will contain the coordinate values.
    *
    * @param aOrientation     the current orientation
    */
    void OrientationVectorsL(TInt aOrientation[ KAMMSTwoVectorComponents ]);

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
    void SetOrientationL(TInt aHeading, TInt aPitch, TInt aRoll);

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
    * @param aFrontVector X, Y and Z value of Front vector
    * @param aAboveVector X, Y and Z value of Above vector
    *
    * @par Leaving:
    * @li \c KErrArgument - In case any of the parameters is a zero vector
    * or they are parallel to each other or any of the parameters' lenghts
    * is not three. In that case, the orientation of the object remains
    * unchanged.
    */
    void SetOrientationVectorsL(
        TInt aFrontVector[ KAMMSVectorComponents ],
        TInt aAboveVector[ KAMMSVectorComponents ]);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSOrientationControl* TypeSafeControl(TInt aIndex) const;

    /**
     * Creates "up" vector by applying cross product operation to
     * "front" and "above" vectors.
     *
     * @param aVariables Used orientation parameters.
     * @param aUpVector Returned "up" vector.
     */
    static void GetUpVectorL(TVariables& aVariables,
                             TInt aUpVector[ KAMMSVectorComponents ]);

    /**
     * Converts orientation from vectors to angles.
     *
     * NOTE: THIS FUNCTION SHOULD BE REMOVED WHEN EFFECT API FIX IS AVAILABLE!
     * (it is possible to set an orientation as vectors).
     *
     * @param aVariables Used orientation parameters.
     * @param aSphericalOrientation Result vector.
     */
    static void ConvertOrientationToAnglesL(
        TVariables& aVariables,
        TInt aSphericalOrientation[ KAMMSVectorComponents ]);

    /**
     * Calculates how much the given vector should be rotated around the
     * given rotation vector so that the value of the specified vector
     * component is maximized.
     *
     * NOTE: THIS FUNCTION SHOULD BE REMOVED WHEN EFFECT API FIX IS AVAILABLE!
     *
     * @param aVector Vector thats components are investigated.
     * @param aRotationAxelVector Vector for the rotation axel.
     * @param aMaximizeComponent Index of the vector component to be
     * maximized.
     * @return An angle that the vector should be rotated.
     */
    static TReal CalculatePartialRotationL(
        TReal aVector[ KAMMSVectorComponents ],
        TReal aRotationAxelVector[ KAMMSVectorComponents ],
        TInt aMaximizeComponent);

    /**
     * Checks whether two given vectors are similar according to the given
     * maximum error percentage. The function compares each component in
     * aA to the corresponding component in aB.
     *
     * NOTE: THIS FUNCTION SHOULD BE REMOVED WHEN EFFECT API FIX IS AVAILABLE!
     *
     * @param aA A vector
     * @param aB A vector
     * @param aMaxComponentErrorPercentage Maximum error percentage between
     * a component in aA and the corresponding component in aB
     * @return ETrue if difference between each component pair is lower
     * than the given error, else ETrue is returned
     */
    static TBool AreVectorsSimilar(TReal aA[ KAMMSVectorComponents ],
                                   TReal aB[ KAMMSVectorComponents ],
                                   TInt aMaxComponentErrorPercentage);

public: // Functions from base classes
    /**
     * Returns class name that identifies this control group.
     *
     * @return Control group name.
     */
    const TDesC16& ClassName();

protected:
    /**
     * Transfers all the pending parameters to the audio processing system.
     *
     * @param aCommit   variable id that need to be commited
     */
    virtual void CommitL(TInt aCommit);

    /**
     * Called by when a new player is added
     *
     * @param aPlayer   The player being added
     * @param aControl  The player's control relevant to this group
     */
    void NotifyPlayerAddedL(CMMAPlayer* aPlayer, CMMAControl* aControl);

private:
    /**
     * C++ default constructor.
     */
    CAMMSOrientationControlGroup();

private: // Data

    enum TOrientationIndex { EFrontX = 0,
                             EFrontY = 1,
                             EFrontZ = 2,
                             EAboveX = 3,
                             EAboveY = 4,
                             EAboveZ = 5,
                             EHeading = 0,
                             EPitch = 1,
                             ERoll = 2 };

    TVariables iCommited; // holder for variables after commit
    TVariables iUncommited; // holder for variables before commit

    enum TCommit { EOrientation = 1 };
};

#endif // CAMMSORIENTATIONCONTROLGROUP_H


