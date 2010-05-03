/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles LocationProvider operations
 *
*/


// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>
#include "clocationprovider.h"
#include "cpositioner.h"
#include "ctrackingpositioner.h"
#include "cdelaystatechangetimer.h"
#include "javax_microedition_location_LocationProvider.h"
#include "javax_microedition_location_Location.h"
#include "com_nokia_mj_impl_location_LocationProviderImpl.h"
#include "locationfunctionserver.h"
#include "fs_methodcall.h"
#include "logger.h"

using namespace java::location;

// CONSTANTS
const TInt KAvailable =
    com_nokia_mj_impl_location_LocationProviderImpl_AVAILABLE;
const TInt KTemporarilyUnavailable =
    com_nokia_mj_impl_location_LocationProviderImpl_TEMPORARILY_UNAVAILABLE;
const TInt KOutOfService =
    com_nokia_mj_impl_location_LocationProviderImpl_OUT_OF_SERVICE;

const TInt KAddressInfo =
    com_nokia_mj_impl_location_LocationProviderImpl_ADDRESS_REQUIRED;
const TInt KCostAllowed =
    com_nokia_mj_impl_location_LocationProviderImpl_COST_ALLOWED;
const TInt KAltitude =
    com_nokia_mj_impl_location_LocationProviderImpl_ALTITUDE_REQUIRED;
const TInt KSpeedAndCourse =
    com_nokia_mj_impl_location_LocationProviderImpl_SPEED_AND_COURSE_REQUIRED;

const TInt KMteSatellite = javax_microedition_location_Location_MTE_SATELLITE;
const TInt KMtyTerminalbased =
    javax_microedition_location_Location_MTY_TERMINALBASED;
const TInt KMtyNetworkbased =
    javax_microedition_location_Location_MTY_NETWORKBASED;
const TInt KMtaAssisted = javax_microedition_location_Location_MTA_ASSISTED;
const TInt KMtaUnassisted = javax_microedition_location_Location_MTA_UNASSISTED;

const TInt KAvailableToTempUnavailableDelay = 5000000; // 5 seconds
const TInt KTempUnavailableToAvailableDelay = 2000000; // 2 seconds

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLocationProvider::CLocationProvider
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLocationProvider::CLocationProvider(LocationFunctionServer* aFunctionSource) :
        CActive(EPriorityNormal), mFunctionServer(aFunctionSource), iStatusEvent(
            TPositionModuleStatusEvent::EEventAll), iState(KAvailable)
{
}

// -----------------------------------------------------------------------------
// CLocationProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLocationProvider::ConstructL()
{
    CActiveScheduler::Add(this);
    JELOG2(EJavaLocation);
    User::LeaveIfError(iPositionServer.Connect());
    iStateTimer = CDelayStateChangeTimer::NewL(this);
}

// -----------------------------------------------------------------------------
// CLocationProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CLocationProvider::NewL(LocationFunctionServer* aFunctionSource,
                             TInt* aHandle)
{
    JELOG2(EJavaLocation);

    CLocationProvider* self = new(ELeave) CLocationProvider(aFunctionSource);

    CleanupStack::PushL(self);
    CallMethodL(self, &CLocationProvider::ConstructL, self->mFunctionServer);
    CleanupStack::Pop(self);

    *aHandle = reinterpret_cast<TInt>(self);
}

// Destructor
CLocationProvider::~CLocationProvider()
{
    JELOG2(EJavaLocation);
    Cancel();
    delete iStateTimer;
    delete iTrackingPositioner;
    iPositionServer.Close();
}

// -----------------------------------------------------------------------------
// CLocationProvider::StaticCreatePositionerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLocationProvider::StaticCreatePositionerL(JNIEnv* aJni,
        jobject aJavaPositioner, CLocationProvider* aSelf, TInt* aHandle,
        LocationFunctionServer* aFunctionSource)
{
    JELOG2(EJavaLocation);

    CPositioner* positioner = NULL;
    TRAPD(error, CallMethodL(positioner, aSelf,
                             &CLocationProvider::CreatePositionerL, aFunctionSource));

    if ((error != KErrNone) || (positioner == NULL))
    {
        ELOG1(EJavaLocation, "StaticCreatePositionerL - error=%d", error);
        *aHandle = error;
        return;
    }

    // Positioner takes ownership of the event since it is reusable
    positioner->SetCallBackData(aJavaPositioner, aJni);

    *aHandle = reinterpret_cast<TInt>(positioner);
}

// -----------------------------------------------------------------------------
// CLocationProvider::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLocationProvider::RunL()
{
    JELOG2(EJavaLocation);
    if (iStatus == KErrNone)
    {
        iStateTimer->Cancel();

        TPositionModuleStatus moduleStatus;
        iStatusEvent.GetModuleStatus(moduleStatus);
        TInt newState = StatusToState(moduleStatus);

        if (newState == KTemporarilyUnavailable && iState == KAvailable)
        {
            iStateTimer->ChangeStateAfter(KAvailableToTempUnavailableDelay,
                                          newState);
        }
        else if (newState == KAvailable && iState == KTemporarilyUnavailable)
        {
            iStateTimer->ChangeStateAfter(KTempUnavailableToAvailableDelay,
                                          newState);
        }
        else
        {
            ChangeState(newState);
        }
    }

    RequestModuleStatus();
}

// -----------------------------------------------------------------------------
// CLocationProvider::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLocationProvider::DoCancel()
{
    JELOG2(EJavaLocation);
    iStateTimer->Cancel();
    iPositionServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
}

// -----------------------------------------------------------------------------
// CLocationProvider::CreatePositionerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPositioner* CLocationProvider::CreatePositionerL()
{
    JELOG2(EJavaLocation);
    CPositioner* positioner = CPositioner::NewL(mFunctionServer,
                              iPositionServer, iModuleId, iCapabilities);
    return positioner;
}

// -----------------------------------------------------------------------------
// CLocationProvider::SelectModuleL
//
// Selects a positioning module. Prefers modules in AVAILABLE state.
//   Leaves with KErrNotSupported if no module match the requirements.
//   Leaves with KErrNotFound if no modules are available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLocationProvider::SelectModuleL(TInt aHacc, TInt aVacc, TInt aRespTime,
                                      TInt aPower, TInt aRequiredFlags, TInt* aMethodTimeout)
{
    JELOG2(EJavaLocation);
    mJni = mFunctionServer->getValidJniEnv();
    mPeer = mFunctionServer->getPeer();

    jclass peerClass = (*mJni).GetObjectClass(mPeer);

    //Get Method ID of Azimuth Data Callback
    mStateChangMethod = mJni->GetMethodID(peerClass, "stateChange", "(I)V");

    if (AllLocationRequestsDeniedL())
    {
        User::Leave(KErrNotFound); // Location is off, no modules can be used
    }

    TBool allModulesOutOfService(ETrue);
    TPositionModuleInfo modInfo;

    // Check if requirements are met for available modules
    TUint numModules(0);
    User::LeaveIfError(iPositionServer.GetNumModules(numModules));

    for (TUint i = 0; i < numModules; ++i)
    {
        User::LeaveIfError(iPositionServer.GetModuleInfoByIndex(i, modInfo));

        if (modInfo.IsAvailable())
        {
            TPositionQuality moduleQuality;
            modInfo.GetPositionQuality(moduleQuality);
            if (CheckRequirements(moduleQuality, modInfo.Capabilities(), aHacc,
                                  aVacc, aRespTime, aPower, aRequiredFlags))
            {
                iModuleId = modInfo.ModuleId();
                break;
            }

            allModulesOutOfService = EFalse;
        }
    }

    if (iModuleId == KPositionNullModuleId) // No module found
    {
        User::Leave(allModulesOutOfService ? KErrNotFound : KErrNotSupported);
    }

    RequestModuleStatus();

    iCapabilities = modInfo.Capabilities();

    iTrackingPositioner = CTrackingPositioner::NewL(mFunctionServer,
                          iPositionServer, iModuleId, iCapabilities);

    TInt locationMethod = (iCapabilities
                           & TPositionModuleInfo::ECapabilitySatellite) ? KMteSatellite : 0;

    TInt tech = modInfo.TechnologyType();
    TBool networkBased = EFalse;

    if (tech & TPositionModuleInfo::ETechnologyTerminal)
    {
        locationMethod |= KMtyTerminalbased;
    }
    if (tech & TPositionModuleInfo::ETechnologyNetwork)
    {
        locationMethod |= KMtyNetworkbased;
        networkBased = ETrue;
    }

    TPositionQuality quality;
    modInfo.GetPositionQuality(quality);
    iTrackingPositioner->SetDefaultValues(quality.TimeToNextFix(), networkBased);

    locationMethod
    |= (tech & TPositionModuleInfo::ETechnologyAssisted) ? KMtaAssisted
       : KMtaUnassisted;

    aMethodTimeout[0] = locationMethod;
    // Default timeout is set to TTFF * 2
    aMethodTimeout[1] = (I64INT(quality.TimeToFirstFix().Int64()) * 2)
                        / 1000000;
}

// -----------------------------------------------------------------------------
// CLocationProvider::CheckRequirements
//
// Returns ETrue if requirements are met, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLocationProvider::CheckRequirements(const TPositionQuality& aQuality,
        TPositionModuleInfo::TCapabilities aCaps, TInt aHacc, TInt aVacc,
        TInt aRespTime, TInt aPower, TInt aRequiredFlags)
{
    JELOG2(EJavaLocation);
    if ((aRequiredFlags & KAddressInfo) && !(aCaps
            & (TPositionModuleInfo::ECapabilityAddress
               | TPositionModuleInfo::ECapabilityBuilding
               | TPositionModuleInfo::ECapabilityMedia)))
    {
        return EFalse;
    }
    if ((aRequiredFlags & KAltitude) && !(aCaps
                                          & TPositionModuleInfo::ECapabilityVertical))
    {
        return EFalse;
    }
    if ((aRequiredFlags & KSpeedAndCourse) && !((aCaps
            & TPositionModuleInfo::ECapabilitySpeed) && (aCaps
                    & TPositionModuleInfo::ECapabilityDirection)))
    {
        return EFalse;
    }
    if (!(aRequiredFlags & KCostAllowed) && aQuality.CostIndicator()
            != TPositionQuality::ECostZero)
    {
        return EFalse;
    }
    if (aHacc != 0 && aQuality.HorizontalAccuracy() > aHacc)
    {
        return EFalse;
    }
    if (aVacc != 0 && aQuality.VerticalAccuracy() > aVacc)
    {
        return EFalse;
    }
    if (aRespTime != 0 && (aQuality.TimeToNextFix().Int64() / 1000) > aRespTime)
    {
        return EFalse;
    }

    if (aPower > 0 && aPower < 3) // LOW or MEDIUM
    {
        switch (aQuality.PowerConsumption())
        {
        case TPositionQuality::EPowerHigh: // Fall through
        case TPositionQuality::EPowerUnknown:
            return EFalse;
        case TPositionQuality::EPowerMedium:
            if (aPower == 1)
            {
                return EFalse;
            }
        default:
            break;
        }
    }

    return ETrue;
}

// -----------------------------------------------------------------------------
// CLocationProvider::RequestModuleStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLocationProvider::RequestModuleStatus()
{
    JELOG2(EJavaLocation);
    iPositionServer.NotifyModuleStatusEvent(iStatusEvent, iStatus, iModuleId);
    SetActive();
}

// -----------------------------------------------------------------------------
// CLocationProvider::StatusToState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLocationProvider::StatusToState(TPositionModuleStatus& aStatus)
{
    JELOG2(EJavaLocation);
    switch (aStatus.DeviceStatus())
    {
    case TPositionModuleStatus::EDeviceUnknown: // Illegal state => Out of service
    case TPositionModuleStatus::EDeviceError:
    case TPositionModuleStatus::EDeviceDisabled:
        return KOutOfService;
    case TPositionModuleStatus::EDeviceInactive:
    case TPositionModuleStatus::EDeviceInitialising:
        return KAvailable;
    case TPositionModuleStatus::EDeviceStandBy:
    case TPositionModuleStatus::EDeviceReady:
    case TPositionModuleStatus::EDeviceActive:
        if (aStatus.DataQualityStatus()
                == TPositionModuleStatus::EDataQualityLoss)
        {
            return KTemporarilyUnavailable;
        }
        else
        {
            return KAvailable;
        }
    default:
        return KOutOfService;
    }
}

// -----------------------------------------------------------------------------
// CLocationProvider::ChangeState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLocationProvider::ChangeState(TInt aState)
{
    if (aState != iState)
    {
        mJni = mFunctionServer->getValidJniEnv();
        mPeer = mFunctionServer->getPeer();

        (*mJni).CallVoidMethod(mPeer, mStateChangMethod, aState);
        iState = aState;
    }
}

// -----------------------------------------------------------------------------
// CLocationProvider::AllLocationRequestsDeniedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLocationProvider::AllLocationRequestsDeniedL()
{
    // Global privacy not used anymore
    return EFalse;
}

