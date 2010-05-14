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
* Description:  Group for effect controls
*
*/


#ifndef CAMMSEFFECTCONTROLGROUP_H
#define CAMMSEFFECTCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <mdaaudiosampleplayer.h>  // MMdaAudioPlayerCallback
#include "cammscontrolgroup.h"

// CONSTANTS
// Used to get preset names and data when the group is empty.
_LIT(KAMMSEmptyGroupSoundPath, "Z:\\System\\Sounds\\Digital\\CamcorderJavaStart.wav");
const TInt KAMMSMaxPresetNameLength = 32;
const TInt KAMMSPresetGranularity = 5;

// FORWARD DECLARATIONS
class CAMMSEffectControl;


// CLASS DECLARATION

/**
 *  Group for effect controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSEffectControlGroup)
        : public CAMMSControlGroup, public MMdaAudioPlayerCallback
{
public:  // Enumerations

    enum TEffectScope {
        EScopeLiveOnly = 1,
        EScopeRecordOnly = 2,
        EScopeLiveAndRecord = 3 };

public:  // Constructors and destructor
    /**
     * Destructor.
     */
    ~CAMMSEffectControlGroup();

public: // New functions

    /**
     * Gets the current preset.
     *
     * @return the current preset
     */
    void GetPresetL(TDes& aPreset);

    /**
     * Gets the available preset names
     *
     * @return all the available current presets
     */
    const CDesCArray& PresetNamesL();

    /**
     * Returns the scope in which the effect is present.
     *
     * @return SCOPE_LIVE_ONLY, SCOPE_RECORD_ONLY or SCOPE_LIVE_AND_RECORD
     */
    TEffectScope Scope();

    /**
     * Returns true if the effect is enabled and false otherwise.
     *
     * @return true=enable, false=disable
     */
    TBool Enabled();

    /**
     * Returns the current enforced setting of the effect.
     *
     * @return true if the effect is an enforced effect, false if not
     */
    TBool Enforced();

    /**
     * Enables/disables the effect.
     *
     * @param aEnabled      true=enabled, false=disabled
     */
    void SetEnabledL(TBool aEnabled);

    /**
     * Enforces the effect to be in use.
     *
     * @param aEnforced     true if the effect is essential and cannot
     *                      be dropped, false if the effect can be dropped
     *                      if the system runs out of resources.
     */
    void SetEnforcedL(TBool aEnforced);

    /**
     * Sets the effect according to the given preset.
     *
     * @param aPreset       The new preset that will be taken into use
     */
    void SetPresetL(const TDesC& aPreset);

    /**
     * Sets the scope of the effect.
     *
     * @param aScope        SCOPE_LIVE_ONLY, SCOPE_RECORD_ONLY or
     *                      SCOPE_LIVE_AND_RECORD.
     */
    void SetScopeL(TEffectScope aScope);

public:  // From MMdaAudioPlayerCallback

    /**
     * Called when file KAMMSEmptyGroupSoundPath has been opened.
     */
    void MapcInitComplete(TInt aError,
                          const TTimeIntervalMicroSeconds& aDuration);

    /**
     * Called when KAMMSEmptyGroupSoundPath has been played,
     * no implementation needed in this class.
     */
    void MapcPlayComplete(TInt aError);

private: // New functions
    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    CAMMSEffectControl* TypeSafeControl(TInt aIndex) const;

protected:  // New functions
    /**
     * Called by when a new player is added
     *
     * @param aPlayer   The player being added
     * @param aControl  The player's control relevant to this group
     */
    void NotifyPlayerAddedL(CMMAPlayer* aPlayer, CMMAControl* aControl);

    /**
     * Called when the current preset changes
     */
    virtual void PresetChangedL();

    /*
     * Finish initialization (after the 1st player is added)
     */
    virtual void InitializeL();

    /**
     * Creates utilities that can be used to obtain preset names
     * and preset data (needed when the group has no controls).
     * Does nothing if the utility already exists.
     */
    virtual void PrepareEmptyGroupUtilitiesL();

    /**
     * Deletes utilities that are used to obtain preset names
     * and preset data (needed when the group has no controls).
     * Does nothing if the utilities have already been deleted.
     */
    virtual void DeleteEmptyGroupUtilities();

    /**
     * Gets list of preset names available.
     * @param aPresetNames Returned preset names
     */
    virtual void GetPresetNamesL(CDesCArray& aPresetNames) = 0;

protected:
    /**
    * C++ default constructor.
    */
    CAMMSEffectControlGroup(const TDesC& aName);

protected:
    /**
     * 2nd phase constructor.
     */
    void BaseConstructL();

protected: // data

    // Available preset names, owned
    CDesCArrayFlat* iPresetNames;

    // Index of the current preset or -1 if no preset is set
    TInt iPresetIndex;

    TBool iInitialized; // Flag to store whether effect is initialized

    // Used to get preset names and data when the group is empty
    // (there are no controls in the group, and thus, no actual
    // control can be used for that purpose).
    CMdaAudioPlayerUtility* iEmptyPlayerUtility;  // Owned.

private: //data

    TEffectScope iScope; // Scope of the effect
    TBool iEnabled; // Flag to store whether effect is enabled
    TBool iEnforced; // Flag to store whether effect is enforced

    // Flag to store whether enforce was set before the group was initialized
    TBool iInitialEnforceSet;
    // Flag to store whether scope was set before the group was initialized
    TBool iInitialScopeSet;

    // Used to wait for opening KAMMSEmptyGroupSoundPath.
    CActiveSchedulerWait* iActiveSchedulerWait; // Owned.

    // Error code occured when creating a player for empty group.
    TInt iEmptyPlayerUtilityError;

};

#endif // CAMMSEFFECTCONTROLGROUP_H


