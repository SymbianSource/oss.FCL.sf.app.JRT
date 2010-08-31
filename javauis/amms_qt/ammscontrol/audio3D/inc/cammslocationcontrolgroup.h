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
* Description:  Group for location controls
*
*/


#ifndef CAMMSLOCATIONCONTROLGROUP_H
#define CAMMSLOCATIONCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammsaudio3dcontrolgroup.h"
#include "ammsconstants.h"

// CONSTANTS
_LIT(KAMMSLocationControl, "LocationControl");
_LIT(KAMMSLocationControlClassName, ".amms.control.audio3d.LocationControl");

// FORWARD DECLARATIONS
class CAMMSLocationControl;

// CLASS DECLARATION
/**
 *  Group for location controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSLocationControlGroup): public CAMMSAudio3DControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSLocationControlGroup* NewLC(
        TAMMSControlTypes aControlType);

    /**
     * Destructor.
     */
    ~CAMMSLocationControlGroup();

public: // New functions
    /**
     * Gets the coordinates of the current location
     *
     * @param aLocation     the current location
     */
    void CartesianL(TInt aLocation[ KAMMSVectorComponents ]);

    /**
     * Moves the object to the new location
     *
     * @param aX    the x component of the new location
     * @param aY    the y component of the new location
     * @param aZ    the z component of the new location
     */
    void SetCartesianL(TInt aX, TInt aY, TInt aZ);

    /**
     * Moves the object to the new location
     *
     * @param aAzimuth      the azimuth angle of the new location in degrees
     * @param aElevation    the elevation angle of the new location in degrees
     * @param aRadius       the magnitude of the new location
     */
    void SetSphericalL(TInt aAzimuth, TInt aElevation, TInt aRadius);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSLocationControl* TypeSafeControl(TInt aIndex) const;

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
    void NotifyPlayerAddedL(CMMAPlayer *aPlayer, CMMAControl* aControl);

private:
    /**
     * C++ default constructor.
     */
    CAMMSLocationControlGroup(TAMMSControlTypes aControlType);

private: // Data

    class TVariables
    {
    public:

        TInt iLocation[ KAMMSVectorComponents ];
    };

    TVariables iCommited; // holder for variables after commit
    TVariables iUncommited; // holder for variables before commit

    enum TCommit { ELocation = 1 };
};

#endif // CAMMSLOCATIONCONTROLGROUP_H


