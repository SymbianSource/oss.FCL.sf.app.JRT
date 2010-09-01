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


#ifndef CAMMSAUDIOVIRTUALIZERCONTROLGROUP_H
#define CAMMSAUDIOVIRTUALIZERCONTROLGROUP_H

//  INCLUDES
#include <e32base.h>
#include "cammseffectcontrolgroup.h"

// CONSTANTS
_LIT(KAMMSAudioVirtualizerControlClassName,
     ".amms.control.audioeffect.AudioVirtualizerControl");

// FORWARD DECLARATIONS
class CAMMSAudioVirtualizerControl;
class CStereoWideningUtility;


// CLASS DECLARATION
/**
 *  Group for audio virtualizer controls
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CAMMSAudioVirtualizerControlGroup):
        public CAMMSEffectControlGroup
{
public:  // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CAMMSAudioVirtualizerControlGroup* NewLC();

    /**
     * destructor
     */
    ~CAMMSAudioVirtualizerControlGroup();

public: // Functions from base classes
    /**
     * Returns class name that identifies this control group.
     *
     * @return Control group name.
     */
    const TDesC16& ClassName();

protected:  // Functions from base classes
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

private:
    /**
     * C++ default constructor.
     */
    CAMMSAudioVirtualizerControlGroup();

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private: // data

    // Needed to get preset names when the group has no controls.
    CStereoWideningUtility* iEmptyStereoWideningUtility;  // Owned.

};

#endif // CAMMSAUDIOVIRTUALIZERCONTROLGROUP_H
