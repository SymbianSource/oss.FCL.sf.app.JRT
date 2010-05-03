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
* Description:  This class is used to control tuner
*
*/



// INCLUDE FILES
#include    "CAMMSTunerControl.h"
#include    "CAMMSTunerPlayer.h"
#include    "CAMMSTunerPresetsHandler.h"
#include    "TAMMSTunerPreset.h"
#include    "AMMSTunerFactory.h"

#include    <tuner.h>
#include    <jdebug.h>


// CONSTANTS
_LIT(KControlImplName, "com.nokia.amms.control.tuner.TunerControlImpl");
_LIT(KControlPublicName, "javax.microedition.amms.control.tuner.TunerControl");

const TInt KStereoModeMono   = 1;
const TInt KStereoModeStereo = 2;
const TInt KStereoModeAuto   = 3;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSTunerControl::CAMMSTunerControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAMMSTunerControl::CAMMSTunerControl()
{
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAMMSTunerControl* CAMMSTunerControl::NewL()
{
    CAMMSTunerControl* self = new(ELeave) CAMMSTunerControl;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAMMSTunerControl::ConstructL()
{
    DEBUG("CAMMSTunerControl::ConstructL +");

    iTunerUtility = CMMTunerUtility::NewL(*this,
                                          CMMTunerUtility::ETunerBandFm,
                                          1,
                                          CMMTunerUtility::ETunerAccessPriorityNormal);

    iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait();

    iPresetsHandler = CAMMSTunerPresetsHandler::NewL();

    iMinFreq = MinFreqL();
    iMaxFreq = MaxFreqL();

    DEBUG("CAMMSTunerControl::ConstructL -");
}


// Destructor
CAMMSTunerControl::~CAMMSTunerControl()
{
    DEBUG("CAMMSTunerControl::~CAMMSTunerControl +");
    delete iTunerUtility;
    delete iActiveSchedulerWait;
    delete iPresetsHandler;
    DEBUG("CAMMSTunerControl::~CAMMSTunerControl -");
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::TunerUtility()
// -----------------------------------------------------------------------------
CMMTunerUtility* CAMMSTunerControl::TunerUtility()
{
    DEBUG("CAMMSTunerControl::TunerUtility");
    return iTunerUtility;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MinFreq()
// -----------------------------------------------------------------------------
TInt CAMMSTunerControl::MinFreqL()
{
    DEBUG("CAMMSTunerControl::MinFreqL +");
    TFrequency minFreq;
    TFrequency maxFreq;
    User::LeaveIfError(iTunerUtility->GetFrequencyBandRange(minFreq,
                       maxFreq));
    DEBUG_INT("CAMMSTunerControl::MinFreqL = %d", minFreq.iFrequency);
    return minFreq.iFrequency;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MaxFreq()
// -----------------------------------------------------------------------------
TInt CAMMSTunerControl::MaxFreqL()
{
    DEBUG("CAMMSTunerControl::MaxFreqL +");
    TFrequency minFreq;
    TFrequency maxFreq;
    User::LeaveIfError(iTunerUtility->GetFrequencyBandRange(minFreq,
                       maxFreq));
    DEBUG_INT("CAMMSTunerControl::MaxFreqL = %d", maxFreq.iFrequency);
    return maxFreq.iFrequency;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SetFrequency()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::SetFrequencyL(TInt aFreq)
{
    DEBUG("CAMMSTunerControl::SetFrequency +");
    if (aFreq < iMinFreq || aFreq > iMaxFreq)
    {
        User::Leave(KErrArgument);
    }

    TFrequency freq = TFrequency(aFreq);

    TUint32 state = 0;
    iTunerUtility->GetState(state);
    DEBUG_INT("CAMMSTunerControl::SetFrequency, state = %d", state);

    DEBUG_INT("CAMMSTunerControl::SetFrequency = %d", freq.iFrequency);

    iTunerUtility->Tune(freq);
    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();  // CSI: 10 Active object state already checked. #
    }
    DEBUG("CAMMSTunerControl::SetFrequency -");
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::Frequency()
// -----------------------------------------------------------------------------
TInt CAMMSTunerControl::FrequencyL()
{
    DEBUG("CAMMSTunerControl::Frequency +");
    TFrequency freq;

    User::LeaveIfError(iTunerUtility->GetFrequency(freq));
    TInt frequency = freq.iFrequency;
    DEBUG_INT("CAMMSTunerControl::Frequency frequency = %d", frequency);
    return frequency;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SeekL()
// -----------------------------------------------------------------------------
TInt CAMMSTunerControl::SeekL(TInt aStartFreq, TBool aUpwards)
{
    DEBUG("CAMMSTunerControl::SeekL +");
    if (aStartFreq < iMinFreq || aStartFreq > iMaxFreq)
    {
        User::Leave(KErrArgument);
    }

    if (aUpwards)
    {
        iTunerUtility->StationSeek(CMMTunerUtility::ESearchDirectionUp);
        if (!iActiveSchedulerWait->IsStarted())
        {
            iActiveSchedulerWait->Start();  // CSI: 10 Active object state already checked. #
        }
    }
    else
    {
        iTunerUtility->StationSeek(CMMTunerUtility::ESearchDirectionDown);
        if (!iActiveSchedulerWait->IsStarted())
        {
            iActiveSchedulerWait->Start();  // CSI: 10 Active object state already checked. #
        }
    }

    return FrequencyL();
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SquelchL()
// -----------------------------------------------------------------------------
TBool CAMMSTunerControl::SquelchL()
{
    DEBUG("CAMMSTunerControl::SquelchL +");
    TBool enabled = EFalse;
    User::LeaveIfError(iTunerUtility->GetSquelch(enabled));
    return enabled;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SetSquelchL()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::SetSquelchL(TBool aSquelch)
{
    DEBUG("CAMMSTunerControl::SetSquelchL +");
    User::LeaveIfError(iTunerUtility->SetSquelch(aSquelch));
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SignalStrengthL()
// -----------------------------------------------------------------------------
TInt CAMMSTunerControl::SignalStrengthL()
{
    DEBUG("CAMMSTunerControl::SignalStrengthL +");
    TInt maxSignalStrength = 0;
    TInt signalStrength = 0;

    User::LeaveIfError(iTunerUtility->GetMaxSignalStrength(maxSignalStrength));
    DEBUG_INT("CAMMSTunerControl::SignalStrengthL, maxSignalStrength = %d", maxSignalStrength);
    User::LeaveIfError(iTunerUtility->GetSignalStrength(signalStrength));
    DEBUG_INT("CAMMSTunerControl::SignalStrengthL, signalStrength = %d", signalStrength);
    if (maxSignalStrength > 0)
    {
        return signalStrength / maxSignalStrength * 100; // CSI: 47 signal strength must be between 0 and 100 #
    }
    return KErrNotSupported;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::StereoModeL()
// -----------------------------------------------------------------------------
TInt CAMMSTunerControl::StereoModeL()
{
    DEBUG("CAMMSTunerControl::StereoModeL +");
    TBool mono = EFalse;
    TBool stereo = EFalse;

    User::LeaveIfError(iTunerUtility->GetForcedMonoReception(mono));
    if (mono)
    {
        return KStereoModeMono;
    }

    //User::LeaveIfError( iTunerUtility->IsStereoSignal( stereo ) );
    TInt err = iTunerUtility->IsStereoSignal(stereo);
    //this is for emulator testing, FIX IT
    if (err == KErrNotReady)
    {
        return KStereoModeAuto;
    }
    else if (err != KErrNone)
    {
        User::LeaveIfError(err);
    }

    if (stereo)
    {
        return KStereoModeStereo;
    }

    return KStereoModeAuto;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SetStereoMode()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::SetStereoModeL(TInt aMode)
{
    DEBUG("CAMMSTunerControl::SetStereoModeL +");
    if (aMode == KStereoModeMono)
    {
        User::LeaveIfError(iTunerUtility->ForceMonoReception(ETrue));
    }

    if (aMode == KStereoModeStereo)
    {
        User::LeaveIfError(iTunerUtility->ForceMonoReception(EFalse));
        TBool stereo = EFalse;
        User::LeaveIfError(iTunerUtility->IsStereoSignal(stereo));
        if (!stereo)
        {
            User::Leave(KErrNotSupported);
        }
    }

    if (aMode == KStereoModeAuto)
    {
        User::LeaveIfError(iTunerUtility->ForceMonoReception(EFalse));
    }
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::UsePresetL()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::UsePresetL(TInt aPreset)
{
    DEBUG("CAMMSTunerControl::UsePresetL +");
    TAMMSTunerPreset tunerPreset;

    //get presets
    iPresetsHandler->GetPresetL(aPreset, tunerPreset);

    //set presets to tunerutility
    SetFrequencyL(tunerPreset.PresetFrequency());
    SetStereoModeL(tunerPreset.PresetStereoMode());
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SetPresetL()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::SetPresetL(TInt aPreset)
{
    DEBUG("CAMMSTunerControl::SetPresetL1 +");
    TAMMSTunerPreset tunerPreset;

    //get current frequency
    tunerPreset.SetPresetFrequency(FrequencyL());
    //get current stereo mode
    tunerPreset.SetPresetStereoMode(StereoModeL());

    iPresetsHandler->SavePresetL(aPreset, tunerPreset);
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SetPresetL()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::SetPresetL(TInt aPreset,
                                   TInt aFrequency,
                                   TInt aStereoMode)
{
    DEBUG("CAMMSTunerControl::SetPresetL2 +");
    TAMMSTunerPreset tunerPreset;

    tunerPreset.SetPresetFrequency(aFrequency);
    tunerPreset.SetPresetStereoMode(aStereoMode);

    iPresetsHandler->SavePresetL(aPreset, tunerPreset);
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::PresetFrequencyL()
// -----------------------------------------------------------------------------
TInt CAMMSTunerControl::PresetFrequencyL(TInt aPreset)
{
    DEBUG("CAMMSTunerControl::PresetFrequencyL +");
    TAMMSTunerPreset tunerPreset;

    iPresetsHandler->GetPresetL(aPreset, tunerPreset);

    return tunerPreset.PresetFrequency();
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::PresetStereoModeL()
// -----------------------------------------------------------------------------
TInt CAMMSTunerControl::PresetStereoModeL(TInt aPreset)
{
    DEBUG("CAMMSTunerControl::PresetStereoModeL +");
    TAMMSTunerPreset tunerPreset;

    iPresetsHandler->GetPresetL(aPreset, tunerPreset);

    return tunerPreset.PresetStereoMode();
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::SetPresetNameL()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::SetPresetNameL(TInt aPreset, const TDesC* aPresetName)
{
    DEBUG("CAMMSTunerControl::SetPresetNameL +");
    TAMMSTunerPreset tunerPreset;

    iPresetsHandler->GetPresetL(aPreset, tunerPreset);

    tunerPreset.SetPresetName(aPresetName);

    iPresetsHandler->SavePresetL(aPreset, tunerPreset);
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::GetPresetNameL()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::GetPresetNameL(TInt aPreset, TPtrC* aPresetName)
{
    DEBUG("CAMMSTunerControl::GetPresetNameL +");
    TAMMSTunerPreset tunerPreset;

    iPresetsHandler->GetPresetL(aPreset, tunerPreset);

    aPresetName->Set(tunerPreset.PresetName());
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::ClassName()
// -----------------------------------------------------------------------------
const TDesC& CAMMSTunerControl::ClassName() const
{
    return KControlImplName;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::PublicClassName()
// -----------------------------------------------------------------------------
const TDesC& CAMMSTunerControl::PublicClassName() const
{
    return KControlPublicName;
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MToTuneComplete()
// -----------------------------------------------------------------------------
void CAMMSTunerControl::MToTuneComplete(TInt aError)
{
    if (iActiveSchedulerWait->IsStarted())
    {
        DEBUG("CAMMSTunerControl::MToTuneComplete AsyncStop");
        iActiveSchedulerWait->AsyncStop();
    }
    DEBUG_INT("CAMMSTunerControl::MToTuneComplete ERROR = %d", aError);
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MTcoFrequencyChanged
// -----------------------------------------------------------------------------
void CAMMSTunerControl::MTcoFrequencyChanged(const TFrequency& /*aOldFrequency*/,
        const TFrequency& /*aNewFrequency*/)
{
    DEBUG("CAMMSTunerControl::MTcoFrequencyChanged");
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MTcoStateChanged
// -----------------------------------------------------------------------------
void CAMMSTunerControl::MTcoStateChanged(const TUint32& /*aOldState*/,
        const TUint32& /*aNewState*/)
{
    DEBUG("CAMMSTunerControl::MTcoStateChanged");
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MTcoAntennaDetached
// -----------------------------------------------------------------------------
void CAMMSTunerControl::MTcoAntennaDetached()
{
    DEBUG("CAMMSTunerControl::MTcoAntennaDetached");
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MTcoAntennaAttached
// -----------------------------------------------------------------------------
void CAMMSTunerControl::MTcoAntennaAttached()
{
    DEBUG("CAMMSTunerControl::MTcoAntennaAttached");
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::FlightModeChanged
// -----------------------------------------------------------------------------
void CAMMSTunerControl::FlightModeChanged(TBool /*aFlightMode*/)
{
    DEBUG("CAMMSTunerControl::FlightModeChanged");
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MTsoStereoReceptionChanged
// -----------------------------------------------------------------------------
void CAMMSTunerControl::MTsoStereoReceptionChanged(TBool /*aStereo*/)
{
    DEBUG("CAMMSTunerControl::MTsoStereoReceptionChanged");
}

// -----------------------------------------------------------------------------
// CAMMSTunerControl::MTsoForcedMonoChanged
// -----------------------------------------------------------------------------
void CAMMSTunerControl::MTsoForcedMonoChanged(TBool /*aForcedMono*/)
{
    DEBUG("CAMMSTunerControl::MTsoForcedMonoChanged");
}

//  End of File
