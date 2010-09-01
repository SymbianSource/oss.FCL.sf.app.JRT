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
* Description:  Virtualizes audio channels.
*
*/


#ifndef CAMMSAUDIOVIRTUALIZERCONTROL_H
#define CAMMSAUDIOVIRTUALIZERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <StereoWideningBase.h>
#include <StereoWideningUtility.h>
#include <CustomCommandUtility.h>
#include "cammseffectcontrol.h"

// CONSTANTS
_LIT(KAMMSAudioVirtualizerControl, "AudioVirtualizerControl");

// CLASS DECLARATION
/**
*
*  Controls for the Audio Virtualizer effect.
*  This class delegates Audio Virtualizer effect method calls to
*  CStereoWidening.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSAudioVirtualizerControl): public CAMMSEffectControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSAudioVirtualizerControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSAudioVirtualizerControl();

public:  // New functions

public: // Functions from base classes
    /**
    * Sets the effect according to the given preset.
    *
    * @param aPreset The new preset that will be taken into use.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aPreset is not available or it is null.
    */
    void SetPresetL(const TDesC& aPreset);

    /**
    * Gets the available preset names.
    *
    * @return The names of all the available preset modes.
    */
    const CDesCArray& PresetNamesL();

    /**
    * Gets the current preset.
    *
    * @return The preset that is set at the moment. If none of the presets
    * is set, null will be returned.
    */
    const TDesC& PresetL();

    /**
    * Enables/disables the effect.
    *
    * @param aEnabled The boolean value, true=enabled, false=disabled.
    *
    * @par Leaving:
    * From Java API the leave codes are:
    * - java.lang.IllegalStateException - if the effect cannot be enabled
    * in this state of the player.
    * - javax.microedition.media.MediaException - if enabling is not
    * supported (with the scope set).
    */
    void SetEnabledL(TBool aEnable);

    const TDesC& ClassName() const;

    /**
    * Prepares the Control.
    */
    void PrepareControlL();

    /**
    * Deallocates the Control.
    */
    void DeallocateControl();

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSAudioVirtualizerControl(CMMAPlayer* aPlayer);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:  // Data

    /** Native Stereo Widening Utility */
    CStereoWideningUtility* iStereoWideningUtility;

    /** Array for querying the preset names, owned */
    CDesCArray* iPresetNames;

    /** Index of the current preset */
    TInt iCurrentPreset;

};

#endif // CAMMSAUDIOVIRTUALIZERCONTROL_H


