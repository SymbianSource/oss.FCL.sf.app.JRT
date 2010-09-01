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
* Description:  Group for Doppler controls
*
*/


#ifndef CAMMSDOPPLERCONTROLGROUP_H
#define CAMMSDOPPLERCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammsaudio3dcontrolgroup.h"
#include "ammsconstants.h"

// FORWARD DECLARATIONS
class CAMMSDopplerControl;

// CONSTANTS
_LIT(KAMMSDopplerControl, "DopplerControl");
_LIT(KAMMSDopplerControlClassName, ".amms.control.audio3d.DopplerControl");

// CLASS DECLARATION
/**
 *  Group for Doppler controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSDopplerControlGroup): public CAMMSAudio3DControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSDopplerControlGroup* NewLC(
        TAMMSControlTypes aControlType);

    /**
     * Destructor.
     */
    ~CAMMSDopplerControlGroup();

public: // New functions
    /**
     * Returns the current velocity, used in calculations for the Doppler
     * effect
     *
     * @param aVelocity     the current velocity
     */
    void VelocityCartesianL(TInt aVelocity[ KAMMSVectorComponents ]);

    /**
     * Returns whether this Doppler effect is currently active.
     *
     * @return the enabled state of the control
     */
    TBool Enabled();

    /**
     * Specifies if this Doppler effect is active or ignored.
     *
     * @param aEnabled      new enabled state
     */
    void SetEnabledL(TBool aEnabled);

    /**
     * Sets the velocity, used in calculations for the Doppler effect.
     *
     * @param aX    the x component of the new velocity
     * @param aY    the y component of the new velocity
     * @param aZ    the z component of the new velocity
     */
    void SetVelocityCartesianL(TInt aX, TInt aY, TInt aZ);

    /**
     * Sets the velocity, used in calculations for the Doppler effect.
     *
     * @param aAzimuth   the azimuth angle of the new velocity in degrees
     * @param aElevation the elevation angle of the new velocity in degrees
     * @param aRadius    the magnitude of the new velocity
     */
    void SetVelocitySphericalL(TInt aAzimuth, TInt aElevation, TInt aRadius);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSDopplerControl* TypeSafeControl(TInt aIndex) const;

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
    CAMMSDopplerControlGroup(TAMMSControlTypes aControlType);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // Data

    class TVariables
    {
    public:

        TInt iVelocity[ KAMMSVectorComponents ];
        TBool iEnabled;
    };

    TVariables iCommited; // holder for variables after commit
    TVariables iUncommited; // holder for variables before commit

    enum TCommit { EEnabled = 1, EVelocity = 2 };
};

#endif // CAMMSDOPPLERCONTROLGROUP_H


