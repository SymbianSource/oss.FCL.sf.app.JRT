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
* Description:  Group for reverb controls
*
*/


#ifndef CAMMSREVERBCONTROLGROUP_H
#define CAMMSREVERBCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammseffectcontrolgroup.h"
#include <EnvironmentalReverbData.h>

// CONSTANTS
_LIT(KAMMSReverbControlClassName, ".amms.control.audioeffect.ReverbControl");

// FORWARD DECLARATIONS
class CAMMSBaseReverbControl;
class CEnvironmentalReverbUtility;


// CLASS DECLARATION
/**
 *  Group for reverb controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSReverbControlGroup): public CAMMSEffectControlGroup
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAMMSReverbControlGroup* NewLC();

    /**
     * destructor
     */
    ~CAMMSReverbControlGroup();

public: // New functions

    /**
     * Gets the gain level of the reverberation
     *
     * @return the gain level of the reverberation
     */
    TInt ReverbLevel();

    /**
     * Gets the reverberation time, as set either explicitly via
     * setReverbTime or implicitly via setPreset (whichever was called last).
     *
     * @return the reverberation time
     */
    TInt ReverbTime();

    /**
     * Sets the gain level of the reverberation
     *
     * @param the gain level of the reverberation to be set
     */
    void SetReverbLevelL(TInt aLevel);

    /**
     * Sets the reverberation time of the reverb
     *
     * @param the reverberation time to be set
     */
    void SetReverbTimeL(TInt aTime);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSBaseReverbControl* TypeSafeControl(TInt aIndex) const;

    /**
     * Gets reverb data for the given preset index from the central
     * repository.
     * @param aPresetIndex Index of the preset whose reverb is asked.
     * @param aReverbData Returned data.
     */
    void GetReverbDataFromCenRepL(TUint aPresetIndex,
                                  TEfEnvReverbDataPckg& aReverbData);

public: // Functions from base classes
    /**
     * Returns class name that identifies this control group.
     *
     * @return Control group name.
     */
    const TDesC16& ClassName();

protected:
    /**
     * Called by when a new player is added
     *
     * @param aPlayer   The player being added
     * @param aControl  The player's control relevant to this group
     */
    void NotifyPlayerAddedL(CMMAPlayer *aPlayer, CMMAControl* aControl);

    /**
     * Called when the current preset changes
     */
    void PresetChangedL();

    /**
     * Finish initialization (after the 1st player is added)
     */
    void InitializeL();

    /**
     * Creates utilities that can be used to obtain preset names
     * and preset data (needed when the group has no controls).
     * Does nothing if the utility already exists.
     */
    void PrepareEmptyGroupUtilitiesL();

    /**
     * Deletes utilities that are used to obtain preset names
     * and preset data (needed when the group has no controls).
     * Does nothing if the utilities have already been deleted.
     */
    void DeleteEmptyGroupUtilities();

    /**
     * Gets list of preset names available.
     * @param aPresetNames Returned preset names
     */
    void GetPresetNamesL(CDesCArray& aPresetNames);

protected:
    /**
     * C++ default constructor.
     */
    CAMMSReverbControlGroup();

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

protected: // data

    // Needed to get preset names when the group has no controls.
    CEnvironmentalReverbUtility* iEmptyEnvironmentalReverbUtility; // Owned.

    TInt iReverbLevel;
    TInt iReverbTime;
    TInt iReverbMinLevel;
    TInt iReverbMaxLevel;

};

#endif // CAMMSREVERBCONTROLGROUP_H
