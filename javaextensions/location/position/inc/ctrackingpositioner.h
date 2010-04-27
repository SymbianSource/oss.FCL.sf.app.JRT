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
* Description:  Handles position request from LocationListener
 *
*/


#ifndef CTRACKINGPOSITIONER_H
#define CTRACKINGPOSITIONER_H

//  INCLUDES
#include "cpositionerbase.h"
#include "locationfunctionserver.h"
#include <jni.h>

namespace java
{
namespace location
{

//  FORWARD DECLARATIONS
class CTimeoutTimer;

// CLASS DECLARATION

/**
 *  Handles position requests with LocationListener.
 */
class CTrackingPositioner: public CPositionerBase
{
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CTrackingPositioner* NewL(LocationFunctionServer* aFunctionSource,
                                     RPositionServer& aServer, TPositionModuleId aModuleId,
                                     TPositionModuleInfo::TCapabilities aCapabilities);

    /**
     * Destructor.
     */
    ~CTrackingPositioner();

public:
    // New functions

    TInt StartTracking(TInt aInterval, TInt aTimeout, TInt aMaxAge);

    void StopTracking();

    void SetDefaultValues(const TTimeIntervalMicroSeconds& aTimeToFix,
                          TBool aNetworkbased);

protected:
    // Functions from base classes

    /**
     * From CActive
     */
    void RunL();

    /**
     * From CActive
     */
    void DoCancel();

    /**
     * From CActive
     */
    TInt RunError(TInt aError);

private:

    /**
     * C++ default constructor.
     */
    CTrackingPositioner(LocationFunctionServer* aFunctionSource);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL(RPositionServer& aServer, TPositionModuleId aModuleId,
                    TPositionModuleInfo::TCapabilities aCapabilities);

    // Prohibit copy constructor if not deriving from CBase.
    // ?classname( const ?classname& );
    // Prohibit assigment operator if not deriving from CBase.
    // ?classname& operator=( const ?classname& );

private:
    void CallBack();
    static TInt StaticCallBack(TAny* aTrackingPositioner);

public:
    // JNI Environment Data
    JNIEnv* mJni;
    jobject mPeer;

    // Location Updated Callback JNI Method ID
    jmethodID mLocationUpdatedMethod;

private:
    // Data

    CTimeoutTimer* iTimer;
    TBool iNetworkBased;
    TInt64 iTimeToFix;
    TInt64 iInterval;
    TInt64 iTimeout;
};

}
}

#endif // CTRACKINGPOSITIONER_H
// End of File
