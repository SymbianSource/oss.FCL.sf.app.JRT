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
* Description:
 *
*/


#ifndef CLOCATIONPROVIDER_H
#define CLOCATIONPROVIDER_H

//  INCLUDES
#include <e32base.h>
#include <lbs.h>
#include <jni.h>
#include "locationfunctionserver.h"

namespace java
{
namespace location
{

//  FORWARD DECLARATIONS
class LocationFunctionServer;
class CPositioner;
class CTrackingPositioner;
class CPosGlobalPrivacyDb;
class CDelayStateChangeTimer;

// CLASS DECLARATION

/**
 *  Handles LocationProvider operations.
 */
class CLocationProvider: public CActive
{
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static void NewL(LocationFunctionServer* aFunctionSource, TInt* aHandle);

    /**
     * Destructor.
     */
    ~CLocationProvider();

public:
    // New functions

    static void StaticCreatePositionerL(JNIEnv* aJni, jobject aPositioner,
                                        CLocationProvider* aSelf, TInt* aHandle,
                                        LocationFunctionServer* aFunctionSource);

    void SelectModuleL(TInt aHacc, TInt aVacc, TInt aRespTime, TInt aPower,
                       TInt aRequiredFlags, TInt* aLocationMethod);

protected:
    // Functions from base classes

    /**
     * From CBase
     */
    void RunL();

    /**
     * From CBase
     */
    void DoCancel();

private:

    /**
     * C++ default constructor.
     */
    CLocationProvider(LocationFunctionServer* aEventSource);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    CPositioner* CreatePositionerL();

    static TBool CheckRequirements(const TPositionQuality& aQuality,
                                   TPositionModuleInfo::TCapabilities aCaps, TInt aHacc, TInt aVacc,
                                   TInt aRespTime, TInt aPower, TInt aRequiredFlags);

    void RequestModuleStatus();

    TInt StatusToState(TPositionModuleStatus& aStatus);

    void ChangeState(TInt aState);

    TBool AllLocationRequestsDeniedL();

public:
    CTrackingPositioner* iTrackingPositioner;

    // JNI Environment Data
    JNIEnv* mJni;
    jobject mPeer;

    // Location Updated Callback JNI Method ID
    jmethodID mStateChangMethod;

private:
    // Data
    LocationFunctionServer* mFunctionServer;
    RPositionServer iPositionServer;
    TPositionModuleId iModuleId;
    TPositionModuleInfo::TCapabilities iCapabilities;
    TPositionModuleStatusEvent iStatusEvent;
    TInt iState;
    TInt iNewState;
    CDelayStateChangeTimer* iStateTimer;
private:
    friend class CDelayStateChangeTimer;
};

}
}
#endif // CLOCATIONPROVIDER_H
// End of File
