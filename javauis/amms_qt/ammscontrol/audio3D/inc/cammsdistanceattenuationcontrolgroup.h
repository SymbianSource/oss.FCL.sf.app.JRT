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
* Description:  Group for distance attenuation controls
*
*/


#ifndef CAMMSDISTANCEATTENUATIONCONTROLGROUP_H
#define CAMMSDISTANCEATTENUATIONCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammsaudio3dcontrolgroup.h"

// CONSTANTS
_LIT(KAMMSDistanceAttenuationClassName,
     ".amms.control.audio3d.DistanceAttenuationControl");

// FORWARD DECLARATIONS
class CAMMSBaseDistanceAttenuationControl;

// CLASS DECLARATION
/**
 *  Group for distance attenuation controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSDistanceAttenuationControlGroup): public CAMMSAudio3DControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSDistanceAttenuationControlGroup* NewLC();

    /**
     * Destructor.
     */
    ~CAMMSDistanceAttenuationControlGroup();

public: // New functions

    /**
     * Returns the maximum distance. At the maximum distance, the gain does
     * not decrease any more. The exact behavior of the gain at distances
     * beyond the maximum distance depends on the value of muteAfterMax
     *
     * @return the maximum distance
     */
    TInt32 MaxDistance() const;

    /**
     * Returns the distance where the source is loudest.
     *
     * @return the minimum distance
     */
    TInt32 MinDistance() const;

    /**
     * Returns how the distance gain behaves for distances beyond the
     * maximum distance.
     *
     * @return true if beyond the maximum distance the source is silent,
     * or false if beyond the maximum distance the source's gain is held
     * constant at the level at the maximum distance
     */
    TBool MuteAfterMax() const;

    /**
     * Returns the rolloff factor for the distance gain.
     *
     * @return the rolloff factor as an exponent specified in thousandths
     */
    TUint32 RolloffFactor() const;

    /**
     * Sets all the 3D audio distance attenuation parameters simultaneously
     *
     * @param aMinDistance      the minimum distance, below which the
     *                          distance gain is clipped to its maximum
     *                          value of 1.0
     * @param aMaxDistance      the maximum distance, beyond which the
     *                          distance gain does not decrease any more
     * @param aMuteAfterMax     a boolean determining how the distance
     *                          gain behaves at distances greater than
     *                          maxDistance
     * @param aRolloffFactor    the rolloff factor
     */
    void SetParametersL(TInt32 aMinDistance, TInt32 aMaxDistance,
                        TBool aMuteAfterMax, TUint32 aRolloffFactor);

private: // New functions
    /**
     * Gets and casts a control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSBaseDistanceAttenuationControl* TypeSafeControl(TInt aIndex) const;

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
    CAMMSDistanceAttenuationControlGroup();

private: // Data

    class TVariables
    {
    public:

        TInt32 iMinDistance;
        TInt32 iMaxDistance;
        TBool iMuteAfterMax;
        TUint32 iRolloffFactor;
    };

    TVariables iCommited; // holder for variables after commit
    TVariables iUncommited; // holder for variables before commit

    enum TCommit { EDistance = 1 };
};

#endif // CAMMSDISTANCEATTENUATIONCONTROLGROUP_H


