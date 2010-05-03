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
* Description:  Handles proximity updates for the registered cordinates
 *
*/


#ifndef CPROXIMITYLISTENER_H
#define CPROXIMITYLISTENER_H

//  INTERNAL INCLUDES
#include "locationfunctionserver.h"
#include "monitor.h"

// EXTERNAL INCLUDES
#include <lbttriggerfiringeventobserver.h>
#include <lbttriggerfiringeventnotifier.h>
#include <lbtserver.h>
#include <lbt.h>
#include <map>
#include <e32property.h>

namespace java
{
namespace location
{

class CProximityListener: public CActive,
        public MLbtTriggerFiringEventObserver
{
public:

    /**
     * Two-phased constructor.
     */
    static CProximityListener* NewL(LocationFunctionServer* aFunctionSource);

    /**
    * Destructor.
    */
    ~CProximityListener();

    // Create and Start the notifer for coordinates monitoring
    void IntializeTriggerL(TReal64 aLat, TReal64 aLon, TReal32 aAlt,
                           TReal aProximityRadius,
                           java::util::Monitor* aMonitor);

    // Start the Trigger
    TInt StartTriggerL();

    // Delete the Trigger from LBT
    void DeleteTriggerL(TInt aTriggerID);

protected:

    /**
     * From CBase
     */
    void RunL();

    /**
     * From CBase
     */
    void DoCancel();

private:

    // This function is called when one or more triggers are fired.
    void TriggerFiredL(const TLbtTriggerFireInfo &  aFireInfo);

    // Listen to LBT status
    void StartListeningStatusL();

    void CProximityListener::ConstructL();

    // Default Constructor
    CProximityListener();

    // Constructor
    CProximityListener(LocationFunctionServer* aFunctionSource);

public:
    bool iIsStatusNotActive;
    TLbtTriggerId iTLbtTriggerId;

private:

    JNIEnv* iJni;
    jclass iLocationProviderImplClass;

    // Proximity Event and State Change Callback JNI Method ID
    jmethodID iProximityEventCallBack;
    jmethodID iProximityStateChangeCallBack;

    // Location function server
    LocationFunctionServer* mFunctionServer;

    RLbt                             iLbt;
    RLbtServer                       iLbtServer;
    bool                             iIsServerOpen;
    CLbtTriggerFiringEventNotifier * iNotifier;
    std::map<int, TCoordinate>       iCordinateMap;
    RProperty                        iProperty;
    TInt                             iLTSupervisionStatus;
    TInt                             iState;
    java::util::Monitor*             iCreateTriggerMonitor;
    TCoordinate                      iCord;
};

}
}

#endif // CPROXIMITYLISTENER_H
// End of File
