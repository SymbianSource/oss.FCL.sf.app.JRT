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
* Description:  Handles getLocation requests
 *
*/


// INCLUDE FILES
#include "cpositioner.h"
#include "logger.h"
#include "locationfunctionserver.h"

using namespace java::location;

// UNNAMED LOCAL NAMESPACE
namespace
{
// Delay subsequent getLocation() calls. This way the failing
// requests do not steal all processing time
const TInt KLocCallDelay = 500000;
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPositioner::CPositioner
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPositioner::CPositioner(LocationFunctionServer* aFunctionSource) :
        CPositionerBase(aFunctionSource)
{
}

// -----------------------------------------------------------------------------
// CPositioner::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPositioner::ConstructL(RPositionServer& aServer,
                             TPositionModuleId aModuleId,
                             TPositionModuleInfo::TCapabilities aCapabilities)
{
    BaseConstructL(aServer, aModuleId, aCapabilities);
    User::LeaveIfError(iTimer.CreateLocal());
}

// -----------------------------------------------------------------------------
// CPositioner::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPositioner* CPositioner::NewL(LocationFunctionServer* aFunctionSource,
                               RPositionServer& aServer, TPositionModuleId aModuleId,
                               TPositionModuleInfo::TCapabilities aCapabilities)
{
    CPositioner* self = new(ELeave) CPositioner(aFunctionSource);
    CleanupStack::PushL(self);
    self->ConstructL(aServer, aModuleId, aCapabilities);
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CPositioner::~CPositioner()
{
    iTimer.Close();
    Cancel();
}

// -----------------------------------------------------------------------------
// CPositioner::StaticCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPositioner::StaticCancel(CPositioner* aSelf)
{
    aSelf->Cancel();
}

// -----------------------------------------------------------------------------
// CPositioner::SetCallBackData
// -----------------------------------------------------------------------------
//
void CPositioner::SetCallBackData(jobject aJavaPositioner, JNIEnv* aJni)
{
    mJni = aJni;
    mPeer = aJavaPositioner;

    jclass positionerClass = (*mJni).GetObjectClass(mPeer);

    //Get Method ID of Get Location Data Callback
    mGetLocationCallBack = mJni->GetMethodID(positionerClass, "syncComplete",
                           "(I)V");
}

// -----------------------------------------------------------------------------
// CPositioner::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPositioner::RunL()
{
    TInt returnCode = iStatus.Int();

    switch (returnCode)
    {
    case KErrNone:
    {
        // If previously returned code was buffer overflow just inform
        // that the request was successful.
        TInt ret = (iDelayedCode == KErrPositionBufferOverflow)
                   || (iDelayedCode == KErrOverflow) ? KErrNone : iDelayedCode;
        // Obtained location information successfully, location fix is partial,
        // or the quality of the fix is poor. In the last two cases the
        // Java implementation tries to obtain location fix again. see API
        // specification about location fixes when temporary unavailable.
        // This is because the current implementation does not support partial
        // location fixes
        mJni = mFunctionServer->getValidJniEnv();
        (*mJni).CallVoidMethod(mPeer, mGetLocationCallBack, ret);
        break;
    }
    case KErrOverflow: // Fallthrough
    case KErrPositionBufferOverflow:
    {
        // CPositionerBase handles buffer size errors
        HandleBufferSizeErrorL(returnCode);
        // Buffer increased successfully. Make a new request
        UpdatePosition();
        break;
    }
    // Handle other return codes of RPositioner::NotifyPositionUpdate()
    // Partial update feature is not supported by this implementation
    case KPositionPartialUpdate: // Fallthrough
    case KPositionQualityLoss: // Fallthrough
        // KErrTimedOut is handled on Java-side
    case KErrTimedOut: // Fallthrough
        // KErrNotFound is returned if current module is invalid
    case KErrNotFound: // Fallthrough
        // HPositionGenericInfo should be supported by all PSYs
    case KErrArgument: // Fallthrough
        // KErrAccessDenied if no requestor specified
    case KErrAccessDenied: // Fallthrough
    case KErrUnknown: // Used in MLFW
        // Position request was canceled
    case KErrCancel: // Fallthrough 
        // Partial update feature is not supported. So Java tries to obtain
        // location fixes until time specified in Java-side expires
        // Delay response so that repeated calls to getLocation do not steal
        // all CPU time
    default:
    {
        iTimer.After(iStatus, KLocCallDelay);
        SetActive();
        break;
    }
    }
    iDelayedCode = returnCode;
}

// -----------------------------------------------------------------------------
// CPositioner::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPositioner::DoCancel()
{
    iTimer.Cancel();
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
    mJni = mFunctionServer->getValidJniEnv();
    (*mJni).CallVoidMethod(mPeer, mGetLocationCallBack, KErrCancel);
}

// -----------------------------------------------------------------------------
// CPositioner::RunError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPositioner::RunError(TInt aError)
{
    mJni = mFunctionServer->getValidJniEnv();
    (*mJni).CallVoidMethod(mPeer, mGetLocationCallBack, aError);
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CPositioner::GetLocation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPositioner::GetLocation(TInt aTimeout)
{
    TTimeIntervalMicroSeconds timeout = MAKE_TINT64(0, aTimeout) * 1000000;
    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateTimeOut(timeout);

    TInt err = iPositioner.SetUpdateOptions(updateOptions);

    if (err == KErrNone)
    {
        UpdatePosition();
    }
    return err;
}

//  End of File
