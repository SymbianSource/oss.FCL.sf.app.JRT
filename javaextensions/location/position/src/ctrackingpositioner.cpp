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
* Description:  Handles position requests with LocationListener
 *
*/


// INCLUDE FILES
#include "ctrackingpositioner.h"
#include "ctimeouttimer.h"
#include "logger.h"

using namespace java::location;

// UNNAMED LOCAL NAMESPACE
namespace
{
// Terminal multiplier = 1.25 (divided by 1000)
const TInt KIntervalTerminal = 1250;
// Network multiplier = 4.0 (divided by 1000)
const TInt KIntervalNetwork = 4000;
// Timeout is never less than 1 sec
const TInt KMinTimeout = 1000000;
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTrackingPositioner::CTrackingPositioner
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTrackingPositioner::CTrackingPositioner(
    LocationFunctionServer* aFunctionSource) :
        CPositionerBase(aFunctionSource)
{
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTrackingPositioner::ConstructL(RPositionServer& aServer,
                                     TPositionModuleId aModuleId,
                                     TPositionModuleInfo::TCapabilities aCapabilities)
{
    JELOG2(EJavaLocation);
    BaseConstructL(aServer, aModuleId, aCapabilities);
    // Must use own timer for timeouts due to different behavior of timeouts in MLFW
    iTimer = CTimeoutTimer::NewL(TCallBack(&StaticCallBack, this));
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTrackingPositioner* CTrackingPositioner::NewL(
    LocationFunctionServer* aFunctionSource, RPositionServer& aServer,
    TPositionModuleId aModuleId,
    TPositionModuleInfo::TCapabilities aCapabilities)
{
    JELOG2(EJavaLocation);
    CTrackingPositioner* self =
        new(ELeave) CTrackingPositioner(aFunctionSource);

    CleanupStack::PushL(self);
    self->ConstructL(aServer, aModuleId, aCapabilities);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CTrackingPositioner::~CTrackingPositioner()
{
    JELOG2(EJavaLocation);
    Cancel();
    delete iTimer;
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::StartTracking
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTrackingPositioner::StartTracking(TInt aInterval, TInt aTimeout,
                                        TInt aMaxAge)
{
    JELOG2(EJavaLocation);
    mJni = mFunctionServer->getValidJniEnv();
    mPeer = mFunctionServer->getPeer();

    jclass peerClass = (*mJni).GetObjectClass(mPeer);

    //Get Method ID of Azimuth Data Callback
    mLocationUpdatedMethod = mJni->GetMethodID(peerClass, "complete", "(I)V");

    //Check if all the JNI inits have succeeded
    if (NULL == mLocationUpdatedMethod)
    {
        return KErrGeneral;
    }

    TInt64 maxAge = 0;
    TPositionUpdateOptions updateOptions;

    iPositioner.SetUpdateOptions(updateOptions);

    if (aInterval > 0)
    {
        iInterval = MAKE_TINT64(0, aInterval) * 1000000;
        iTimeout = (aTimeout > 0) ? MAKE_TINT64(0, aTimeout) * 1000000 : Min(
                       iTimeToFix * 2, iInterval);

        // Subtract 1 so that maxage is always smaller than interval
        maxAge = (aMaxAge > 0) ? MAKE_TINT64(0, aMaxAge) * 1000000 - 1
                 : iInterval / 2;
    }
    else
    {
        // Default interval = time to fix * factor
        // (different for network and terminal based modules)
        const TInt factor = iNetworkBased ? KIntervalNetwork
                            : KIntervalTerminal;
        iInterval = (iTimeToFix * factor) / 1000;

        iTimeout = Min(iTimeToFix * 2, iInterval);
        maxAge = iInterval / 2;
    }

    iTimeout = Max(iTimeout, KMinTimeout);
    updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(iInterval));
    updateOptions.SetMaxUpdateAge(TTimeIntervalMicroSeconds(maxAge));

    TInt err = iPositioner.SetUpdateOptions(updateOptions);
    if (err == KErrNone)
    {
        UpdatePosition();

        // Must use own timer due to different behavior of timeouts in MLFW
        iTimer->TimeoutAfter(TTimeIntervalMicroSeconds(iInterval));
    }

    return err;
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::StopTracking
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTrackingPositioner::StopTracking()
{
    JELOG2(EJavaLocation);
    iTimer->Cancel();
    Cancel();
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::SetDefaultValues
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTrackingPositioner::SetDefaultValues(
    const TTimeIntervalMicroSeconds& aTimeToFix, TBool aNetworkBased)
{
    JELOG2(EJavaLocation);
    iTimeToFix = aTimeToFix.Int64();
    iNetworkBased = aNetworkBased;
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTrackingPositioner::RunL()
{
    JELOG2(EJavaLocation);

    TInt err = iStatus.Int();

    switch (err)
    {
    case KErrNone:
    {
        iTimer->Cancel();

        mJni = mFunctionServer->getValidJniEnv();
        mPeer = mFunctionServer->getPeer();

        (*mJni).CallVoidMethod(mPeer, mLocationUpdatedMethod, err);

        UpdatePosition();
        iTimer->TimeoutAfter(iInterval + iTimeout);
        break;
    }
    case KErrOverflow: // Fallthrough
    case KErrPositionBufferOverflow:
    {
        iTimer->Cancel();
        // CPositionerBase handles buffer size errors
        HandleBufferSizeErrorL(err);
        // Buffer increased successfully. Make a new request
        UpdatePosition();
        // Set time out timer since tracking must not be stopped
        // in any case if the listener has been set from Java-side
        iTimer->TimeoutAfter(iInterval + iTimeout);
        break;
    }
    default:
    {
        // Do not handle other error situations. Time out will take
        // care if the location request didn't succeed
        break;
    }
    }
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTrackingPositioner::DoCancel()
{
    JELOG2(EJavaLocation);
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::RunError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTrackingPositioner::RunError(TInt aError)
{
    JELOG2(EJavaLocation);
    mJni = mFunctionServer->getValidJniEnv();
    mPeer = mFunctionServer->getPeer();

    (*mJni).CallVoidMethod(mPeer, mLocationUpdatedMethod, aError);

    iTimer->TimeoutAfter(iInterval);
    // RunL caused a leave, but we still have to acquire next location fix
    // even though it will propably fail. Just try to keep tracking
    if (!IsActive())
    {
        // Last position update failed, request a new position
        UpdatePosition();
    }
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::CallBack
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTrackingPositioner::CallBack()
{
    JELOG2(EJavaLocation);
    mJni = mFunctionServer->getValidJniEnv();
    mPeer = mFunctionServer->getPeer();

    (*mJni).CallVoidMethod(mPeer, mLocationUpdatedMethod, KErrTimedOut);

    iTimer->TimeoutAfter(iInterval);

    if (!IsActive())
    {
        // Last position update failed, request a new position
        UpdatePosition();
    }
}

// -----------------------------------------------------------------------------
// CTrackingPositioner::StaticCallBack
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTrackingPositioner::StaticCallBack(TAny* aTrackingPositioner)
{
    JELOG2(EJavaLocation);
    CTrackingPositioner* positioner =
        reinterpret_cast<CTrackingPositioner*>(aTrackingPositioner);
    positioner->CallBack();
    return KErrNone;
}

//  End of File
