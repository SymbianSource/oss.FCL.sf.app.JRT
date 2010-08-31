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


#ifndef CAMMSEMCAUDIOVIRTUALIZERCONTROL_H
#define CAMMSEMCAUDIOVIRTUALIZERCONTROL_H

//  INCLUDES
#include <e32base.h>
#include "cammseffectcontrol.h"
#include "cammseffectcontrolgroup.h"
#include <cmmaemcaudioplayer.h>
#include<StereoWideningControl.h>
//Utility for getting EnvironmentalPreset
#include "cammsemcaudiovirtualizerenvironmentalpresetutility.h"


using multimedia :: KStereoWideningEffectControl;
using multimedia :: MStereoWideningControl;


// CONSTANTS
_LIT(KAMMSEMCAudioVirtualizerControl, "AudioVirtualizerControl");

// CLASS DECLARATION
/**
*
*  Controls for the Audio Virtualizer effect.
*  This class delegates Audio Virtualizer effect method calls to
*  CStereoWidening.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSEMCAudioVirtualizerControl): public CAMMSEffectControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSEMCAudioVirtualizerControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSEMCAudioVirtualizerControl();

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

public: // overriden virtual functions from CAMMSEffectControl


    virtual void SetEnforcedL(TBool aEnforced);

    virtual TBool Enforced();

    virtual void SetScopeL(TInt aScope);

    virtual TInt Scope();

protected:

    virtual void ApplySettingsL();

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSEMCAudioVirtualizerControl(CMMAPlayer* aPlayer);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:  // Data

    /** Array for querying the preset names, owned */
    CDesCArray* iPresetNames;
    TBuf<KAMMSMaxPresetNameLength> iPresetName;

    /** Index of the current preset */
    TInt iCurrentPreset;
    /* AudioVirtualizer utility to get preset and data related to that preset*/
    CAMMSEMCAudioVirtualizerEnvironmentalPresetUtility * iAMMSEMCAudioVirtualizerEnvironmentalPresetUtility;

private:

    CMMAPlayer *iMMAPlayer;
    /**
    * EMC AudioVirtualizer Control
    */
    CMultimediaFactory* iFactory;
    MStreamControl* iStreamControl;
    MStereoWideningControl *iMStereoWideningControl;


};

#endif // CAMMSEMCAUDIOVIRTUALIZERCONTROL_H


