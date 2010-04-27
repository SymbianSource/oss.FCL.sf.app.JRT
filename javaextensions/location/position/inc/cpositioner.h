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


#ifndef CPOSITIONER_H
#define CPOSITIONER_H

//  INCLUDES
#include "cpositionerbase.h"
#include "locationfunctionserver.h"

namespace java
{
namespace location
{

class LocationFunctionServer;

// CLASS DECLARATION

/**
 *  Handles getLocation position requests.
 */
class CPositioner: public CPositionerBase
{
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPositioner* NewL(LocationFunctionServer* aFunctionSource,
                             RPositionServer& aServer, TPositionModuleId aModuleId,
                             TPositionModuleInfo::TCapabilities aCapabilities);

    /**
     * Destructor.
     */
    ~CPositioner();

public:
    // New functions

    TInt GetLocation(TInt aTimeout);

    void StaticCancel(CPositioner* aSelf);

    void CPositioner::SetCallBackData(jobject aJavaPositioner, JNIEnv* aJni);

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
    CPositioner(LocationFunctionServer* aFunctionSource);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL(RPositionServer& aServer, TPositionModuleId aModuleId,
                    TPositionModuleInfo::TCapabilities aCapabilities);

    // Prohibit copy constructor if not deriving from CBase.
    // ?classname( const ?classname& );
    // Prohibit assigment operator if not deriving from CBase.
    // ?classname& operator=( const ?classname& );

public:
    // JNI Environment Data
    JNIEnv* mJni;
    jobject mPeer;

    // Location Updated Callback JNI Method ID
    jmethodID mGetLocationCallBack;

private:
    void RequestLocation();

private:
    // Data
    //CLocationEvent* iEvent;
    RTimer iTimer;
    TInt iDelayedCode;
};

}
}

#endif // CPOSITIONER_H
// End of File
