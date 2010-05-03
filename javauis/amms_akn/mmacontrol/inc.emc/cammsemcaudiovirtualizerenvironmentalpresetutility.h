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
* Description:  Utility to get the preset and data
*
*/


#ifndef CAMMSEMCAUDIOVIRTUALIZERENVIRONMENTALPRESETUTILITY_H
#define CAMMSEMCAUDIOVIRTUALIZERENVIRONMENTALPRESETUTILITY_H

//  INCLUDES
#include <e32base.h>
#include "cammsemcbasemmfdevsound.h"
#include <StereoWideningUtility.h>
#include <StereoWideningUtilityData.h>


// CLASS DECLARATION

/**
 *  @since 5.1
 */
NONSHARABLE_CLASS(CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility)
        : public CAMMSEMCBaseMMFDevSound
{
public:  // destructor

    /**
     * Destructor.
     */
    ~CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility();

public: // New functions

    /**
     * Gets the current preset.
     *
     * @return the current preset
     */
    void GetPresetAtIndexL(TDes& aPreset ,TInt iPresetIndex);

    /**
    * Sets the effect according to the given preset.
    *
    * @param aPreset       The new preset that will be taken into use
    */
    void SetPresetL(const TDesC& aPreset);

    /**
    * Creates utilities that can be used to obtain CSterioWidening Utility
    * and preset data .
    * Does nothing if the utility already exists.
    */
    virtual void PrepareEmptyStereoWideningUtilitiesL();

    /**
     * Deletes Environmental utilities
     * Does nothing if the utilities have already been deleted.
     */
    virtual void DeleteEmptyStereoWideningUtilities();
    /**
     * Gets list of preset names available.
     * @param aPresetNames Returned preset names
     */
    virtual void GetPresetNamesL(CDesCArray& aPresetNames);

    CStereoWidening * GetStereoWideningAtPresetIndexL(TInt iPresetIndex);

public:
    /**
     * c++ constructor.
     */
    CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility();
    /**
     * 2nd phase constructor.
     */

    void ConstructL();

protected: // data

    // Available preset names, owned
    CDesCArrayFlat* iPresetNames;
    // Used to get preset names and data when the group is empty
    // (there are no controls in the group, and thus, no actual
    // control can be used for that purpose).
    CStereoWideningUtility* iEmptyStereoWideningUtility;

};

#endif // CAMMSEMCAUDIOVIRTUALIZERENVIRONMENTALPRESETUTILITY_H


