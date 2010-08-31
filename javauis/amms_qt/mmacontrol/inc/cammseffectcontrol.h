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
* Description:  Controls an abstract filter with various preset settings.
*
*/


#ifndef CAMMSEFFECTCONTROL_H
#define CAMMSEFFECTCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <AudioEffectBase.h>
#include "cammscontrol.h"

// CLASS DECLARATION

/**
*
*  Controls for the audio effect control.
*  This class delegates Audio Effect method calls to native
*  audio effect (created either in CAMMSEqualizerControl or
*  in CAMMSReverbControl).
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSEffectControl): public CAMMSControl
{
public:  // Constructors and destructor
    /**
    * Destructor.
    */
    ~CAMMSEffectControl();

public: // New functions
    /**
    * Sets the effect according to the given preset.
    *
    * @param aPreset The new preset that will be taken into use.
    *
    * @par Leaving:
    * @li \c KErrArgument - \a aPreset is not available or it is null.
    */
    virtual void SetPresetL(const TDesC& aPreset) = 0;

    /**
    * Gets the available preset names.
    *
    * @return The names of all the available preset modes.
    */
    virtual const CDesCArray& PresetNamesL() = 0;

    /**
    * Gets the current preset.
    *
    * @return The preset that is set at the moment. If none of the presets
    * is set, null will be returned.
    */
    virtual const TDesC& PresetL() = 0;

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
    virtual void SetEnabledL(TBool aEnable) = 0;

//Making the following funcitons virtual because of EMC added functionalities
    /**
    * Enforces the effect to be in use. If this is an EffectControl of a
    * MediaProcessor, the enforced setting does not affect in any way.
    *
    * @param aPreset The boolean value - true if the effect is essential
    * and cannot be dropped, false if the effect can be dropped if the
    * system runs out of resources.
    *
    * @par Leaving:
    * @li \c KErrArgument - some error happened.
    */
    virtual void SetEnforcedL(TBool aEnforced);

    /**
    * Returns the current enforced setting of the effect.
    *
    * @return The boolean, true if the effect is an enforced effect, false
    * if not.
    */
    virtual TBool Enforced();

    /**
    * Sets the scope of the effect. If this is an EffectControl of the
    * MediaProcessor, the scope setting does not affect in anything.
    *
    * @param aScope SCOPE_LIVE_ONLY, SCOPE_RECORD_ONLY or
    * SCOPE_LIVE_AND_RECORD.
    *
    * @par Leaving:
    * From Java API the leave codes are:
    * - javax.microedition.media.MediaException - if the given scope is
    * not supported
    */
    virtual void SetScopeL(TInt aScope);

    /**
    * Returns the scope in which the effect is present.
    *
    * @return SCOPE_LIVE_ONLY, SCOPE_RECORD_ONLY or SCOPE_LIVE_AND_RECORD.
    */
    virtual TInt Scope();

protected:
    /**
    * C++ default constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSEffectControl(CMMAPlayer* aPlayer);

protected:
    /**
    * Apply changed settings if Effect is in enabled state.
    */
    virtual void ApplySettingsL();

protected:
    /**
    * By default Symbian 2nd phase constructor is private,
    * but allow now the base class to access ConstructL.
    */
    void ConstructL();

protected: // Data

    /* Native audio effect */
    CAudioEffect* iAudioEffect;

};

#endif // CAMMSEFFECTCONTROL_H


