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
* Description:  This class is used to create tuner player and control
*
*/



// INCLUDE FILES
#include    "AMMSTunerFactory.h"
#include    "CAMMSTunerPlayer.h"
#include    "CAMMSTunerControl.h"
//#include    "CAMMSTunerVolumeControl.h"
#include    <CMMAPlayerProperties.h>

#include    <e32std.h>
#include    <e32math.h>
#include    <jdebug.h>

// CONSTANTS
_LIT(KFreqParam, "f");

_LIT(KModulationParam, "mod");

_LIT(KStereoModeParam, "st");
MMA_PARAMETER_STR(KStereoModeParamMono, "mono");
MMA_PARAMETER_STR(KStereoModeParamStereo, "stereo");
MMA_PARAMETER_STR(KStereoModeParamAuto, "auto");
MMA_PARAMETER_ARRAY(KValidStereoModeValues)
{
    {
        &KStereoModeParamMono
    }, {&KStereoModeParamStereo}, {&KStereoModeParamAuto}
};

_LIT(KProgramIdParam, "id");
_LIT(KPresetParam, "preset");

_LIT(KModulationFm, "fm");

_LIT(KStereoModeMono, "mono");
_LIT(KStereoModeStereo, "stereo");
_LIT(KStereoModeAuto, "auto");

const TInt KMegaHertzChar = 'M';
const TInt KKiloHertzChar = 'k';
const TInt KFreqDotChar   = '.';

const TInt KMegaHzMultiplier = 1000000;
const TInt KKiloHzMultiplier = 1000;

const TInt KStereoModeMonoInt   = 1;
const TInt KStereoModeStereoInt = 2;
const TInt KStereoModeAutoInt   = 3;

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
AMMSTunerFactory::~AMMSTunerFactory()
{

}


// -----------------------------------------------------------------------------
// AMMSTunerFactory::CreatePlayerL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void AMMSTunerFactory::CreatePlayerL(CAMMSTunerPlayer** aTunerPlayer,
                                     const TDesC* aLocatorParams)
{
    DEBUG("AMMSTunerFactory::CreatePlayerL +");

    TInt frequency = 0;
    TInt stereoMode = 0;
    TInt preset = 0;

    if (aLocatorParams->Length() != 0)
    {
        DEBUG("AMMSTunerFactory::CreatePlayerL 1");
        ParseParamsL(aLocatorParams, frequency, stereoMode, preset);
    }

    //create tunercontrol
    CAMMSTunerControl* tunerControl = CAMMSTunerControl::NewL();
    CleanupStack::PushL(tunerControl);

    //set values to tunercontrol
    if (frequency > 0)
    {
        tunerControl->SetFrequencyL(frequency);
    }

    if (frequency == 0)
    {
        //by default frequency is fm modulation's min freq
        tunerControl->SetFrequencyL(tunerControl->MinFreqL());
    }

    if (stereoMode > 0)
    {
        tunerControl->SetStereoModeL(stereoMode);
    }

    if (preset > 0)
    {
        tunerControl->UsePresetL(preset);
    }

    //create tunerplayer
    *aTunerPlayer = CAMMSTunerPlayer::NewL(tunerControl);

    //create tunervolumeconrol
    //CAMMSTunerVolumeControl* tunerVolumeControl = CAMMSTunerVolumeControl::NewL( *aTunerPlayer );

    //add controls to player
    (*aTunerPlayer)->AddControlL(tunerControl);
    //(*aTunerPlayer)->AddControlL( tunerVolumeControl );
    CleanupStack::Pop(tunerControl);
    DEBUG("AMMSTunerFactory::CreatePlayerL -");
}

// -----------------------------------------------------------------------------
// AMMSTunerFactory::ParseParamsL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void AMMSTunerFactory::ParseParamsL(const TDesC* aLocatorParams,
                                    TInt& aFrequency,
                                    TInt& aStereoMode,
                                    TInt& aPreset)
{
    DEBUG("AMMSTunerFactory::ParseParamsL +");
    CMMAParameterRuleSet* rules = CMMAParameterRuleSet::NewLC();

    //freq rule
    TMMAParameterRuleDes freqRule(KFreqParam);
    rules->AppendRuleL(&freqRule);

    //modulation rule
    TMMAParameterRuleDes modulationRule(KModulationParam);
    rules->AppendRuleL(&modulationRule);

    //stereo mode rule
    TMMAParameterRuleDes stereoModeRule(KStereoModeParam, KValidStereoModeValues,
                                        MMA_PARAMETER_ARRAY_SIZE(KValidStereoModeValues));
    rules->AppendRuleL(&stereoModeRule);

    //preset rule KMinTInt-KMaxTInt
    TMMAParameterRuleInt presetRule(KPresetParam);
    rules->AppendRuleL(&presetRule);

    //id rule
    TMMAParameterRuleDes idRule(KProgramIdParam);
    rules->AppendRuleL(&idRule);

    CMMAPlayerProperties* properties = CMMAPlayerProperties::NewL(*aLocatorParams, *rules);
    CleanupStack::PushL(properties);

    // validating properties
    properties->ValidateL();

    //get freq
    TPtrC freq(NULL, 0);
    properties->GetProperty(KFreqParam, freq);
    //parse frequency
    if (freq.Length() != 0)
    {
        aFrequency = ParseFreqL(freq);
    }

    //get modulation
    TPtrC modulation(NULL, 0);
    properties->GetProperty(KModulationParam, modulation);
    if (modulation.Length() != 0)
    {
        // only fm modulation is supported
        if (modulation.Compare(KModulationFm) != 0)
        {
            User::Leave(KErrNotSupported);
        }
    }

    //get stereo mode
    TPtrC stereoMode(NULL, 0);
    properties->GetProperty(KStereoModeParam, stereoMode);
    if (stereoMode.Compare(KStereoModeMono) == 0)
    {
        aStereoMode = KStereoModeMonoInt;
    }
    else if (stereoMode.Compare(KStereoModeStereo) == 0)
    {
        aStereoMode = KStereoModeStereoInt;
    }
    else if (stereoMode.Compare(KStereoModeAuto) == 0)
    {
        aStereoMode = KStereoModeAutoInt;
    }

    //get preset
    properties->GetProperty(KPresetParam, aPreset);

    //get id
    TPtrC id(NULL, 0);
    properties->GetProperty(KProgramIdParam, id);
    if (id.Length() != 0)
    {
        // id is not supported
        User::Leave(KErrNotSupported);
    }

    CleanupStack::PopAndDestroy(properties);
    CleanupStack::PopAndDestroy(rules);
    DEBUG("AMMSTunerFactory::ParseParamsL -");
}

// -----------------------------------------------------------------------------
// AMMSTunerFactory::ParseFreqL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt AMMSTunerFactory::ParseFreqL(const TPtrC aFrequency)
{
    DEBUG("AMMSTunerFactory::ParseFreqL +");
    TReal freqReal = 0;

    //is value MHz or kHz or just hertz
    TInt freqPrefixMPos = aFrequency.Locate(KMegaHertzChar);
    TInt freqPrefixKPos = aFrequency.Locate(KKiloHertzChar);

    if (freqPrefixMPos != KErrNotFound)
    {
        //there is a M
        TPtrC hertz = aFrequency.Left(freqPrefixMPos);
        //TPtrC -> TReal
        TReal value = TDesCToTRealL(hertz);

        freqReal = value * KMegaHzMultiplier;
    }
    else if (freqPrefixKPos != KErrNotFound)
    {
        //there is a k
        TPtrC hertz = aFrequency.Left(freqPrefixKPos);
        //TPtrC -> TReal
        TReal value = TDesCToTRealL(hertz);

        freqReal = value * KKiloHzMultiplier;
    }
    else
    {
        //parameter value is simply hertz, there is no M or k
        //TPtrC -> TReal
        freqReal = TDesCToTRealL(aFrequency);
    }
    TInt32 freqInt = 0;
    //TReal -> TInt
    TInt err = Math::Int(freqInt, freqReal);
    if (err != KErrNone)
    {
        User::Leave(KErrArgument);
    }

    return freqInt;
}

// -----------------------------------------------------------------------------
// AMMSTunerFactory::TDesCToTRealL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TReal AMMSTunerFactory::TDesCToTRealL(const TPtrC aHertz)
{
    DEBUG("AMMSTunerFactory::TDesCToTRealL +");
    TReal valueReal = 0;
    TLex lex(aHertz);
    if ((lex.Val(valueReal, KFreqDotChar) != KErrNone) ||
            !lex.Eos())
    {
        User::Leave(KErrArgument);
    }
    return valueReal;
}

//  End of File
