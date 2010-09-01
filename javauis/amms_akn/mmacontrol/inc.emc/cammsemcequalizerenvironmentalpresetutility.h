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


#ifndef CAMMSEMCEQUALIZERENVIRONMENTALPRESETUTILITY_H
#define CAMMSEMCEQUALIZERENVIRONMENTALPRESETUTILITY_H

//  INCLUDES
#include <e32base.h>
#include "cammsemcbasemmfdevsound.h"
#include <AudioEqualizerUtility.h>
#include <AudioEqualizerUtilityData.h>

// CLASS DECLARATION

/**
 *  Group for effect controls
 *
 *  @since 5.1
 */
NONSHARABLE_CLASS(CAMMSEMCEqualizerEnvironmentalPresetUtility)
        : public CAMMSEMCBaseMMFDevSound
{
public:  // destructor

    /**
     * Destructor.
     */
    ~CAMMSEMCEqualizerEnvironmentalPresetUtility();

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
    * Creates utilities that can be used to obtain CAudioEqualizer Utility
    * and preset data .
    * Does nothing if the utility already exists.
    */
    virtual void PrepareEmptyEqualizerUtilitiesL();

    /**
     * Deletes Environmental utilities
     * Does nothing if the utilities have already been deleted.
     */
    virtual void DeleteEmptyEqualizerUtilities();
    /**
     * Gets list of preset names available.
     * @param aPresetNames Returned preset names
     */
    virtual void GetPresetNamesL(CDesCArray& aPresetNames);

    CAudioEqualizer * GetEqualizerAtPresetIndexL(TInt iPresetIndex);

public:
    /**
     * c++ constructor.
     */
    CAMMSEMCEqualizerEnvironmentalPresetUtility();
    /**
     * 2nd phase constructor.
     */

    void ConstructL();

protected: // data

    // Available preset names, owned
    CDesCArrayFlat* iPresetNames;
    // Pointer to the EqualizerUtility
    CAudioEqualizerUtility* iEmptyEqualizerUtility;

};

#endif // CAMMSEMCEQUALIZERENVIRONMENTALPRESETUTILITY_H


