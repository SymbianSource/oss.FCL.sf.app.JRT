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


// EXTERNAL INCLUDES
#include <lbtsessiontrigger.h>
#include <lbtgeocircle.h>
#include <lbttriggerconditionarea.h>
#include <lbtstatuspskeys.h>
#include <lbs.h>
#include <unistd.h>

// INTERNAL INCLUDES
#include "cproximitylistener.h"
#include "logger.h"

using namespace java::location;

//trigger name.
_LIT(KMyTriggerName, "JavaProxmityTrigger");

//Construct requestor
_LIT(KMyRequestorName, "JavaRuntime");   //Requestor identifier

const TInt KListen = 0;
const TInt KCreate = 1;

// ----------------------------------------------------------------------------
// CProximityListener::CProximityListener
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CProximityListener::CProximityListener(
    LocationFunctionServer* aFunctionSource) :
        CActive(EPriorityNormal), mFunctionServer(aFunctionSource)
{
    JELOG2(EJavaLocation);
    CActiveScheduler::Add(this);
    iIsServerOpen = false;
    iIsStatusNotActive = true;
    iNotifier = NULL;
    iState = KListen;
    iCreateTriggerMonitor = NULL;
}

CProximityListener::~CProximityListener()
{
    Cancel();
    if (NULL !=  iNotifier)
    {
        // Close the notifier handle
        iNotifier->Cancel();
        delete iNotifier;
    }

    iProperty.Close();
    iLbt.Close();
    iLbtServer.Close();
    iIsServerOpen = false;
    iCreateTriggerMonitor = NULL;
}

void CProximityListener::ConstructL()
{
    JELOG2(EJavaLocation);

    iJni = mFunctionServer->getValidJniEnv();

    iLocationProviderImplClass =
        iJni->FindClass("com/nokia/mj/impl/location/LocationProviderImpl");

    iProximityEventCallBack =
        iJni->GetStaticMethodID(iLocationProviderImplClass,
                                "proximityEventCallback", "(IDDF)V");

    iProximityStateChangeCallBack =
        iJni->GetStaticMethodID(iLocationProviderImplClass,
                                "proximityStateChangeCallback",  "(Z)V");
}

// ----------------------------------------------------------------------------
// CProximityListener::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CProximityListener* CProximityListener::NewL(
    LocationFunctionServer* aFunctionSource)
{
    JELOG2(EJavaLocation);

    CProximityListener* self =
        new(ELeave) CProximityListener(aFunctionSource);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ----------------------------------------------------------------------------
// CProximityListener::IntializeTriggerL
// Initalize trigger for Proximity updates
// ----------------------------------------------------------------------------
//
void CProximityListener::IntializeTriggerL(TReal64 aLat, TReal64 aLon,
        TReal32 aAlt, TReal aProximityRadius, java::util::Monitor* aMonitor)
{
    JELOG2(EJavaLocation);
    iState = KCreate;
    iCreateTriggerMonitor = aMonitor;

    if (!iIsServerOpen)
    {
        // Open session and subsession to Location Triggering Server
        User::LeaveIfError(iLbtServer.Connect());
        User::LeaveIfError(iLbt.Open(iLbtServer));

        iIsServerOpen = true;
    }

    iCord.SetCoordinate(aLat, aLon, aAlt);

    //Define the triggering area
    CLbtGeoCircle* trigArea =
        CLbtGeoCircle::NewL(iCord,                //center coordinate
                            aProximityRadius    //radius in meters.
                           );
    CleanupStack::PushL(trigArea);

    // Trigger Condition
    CLbtTriggerConditionArea* cond =
        CLbtTriggerConditionArea::NewL(trigArea,
                                       CLbtTriggerConditionArea::EFireOnEnter
                                      );
    CleanupStack::Pop(trigArea);
    CleanupStack::PushL(cond);

    //Construct a session trigger
    CLbtSessionTrigger* trig = CLbtSessionTrigger::NewL(
                                   KMyTriggerName,
                                   CLbtTriggerEntry::EStateEnabled,
                                   CRequestorBase::ERequestorService,
                                   CRequestorBase::EFormatApplication,
                                   KMyRequestorName,
                                   TUid::Null(),
                                   cond
                               );

    // Delte iNotifier when creating a trigger when already
    // outstanding request is present
    if (iNotifier != NULL)
    {
        delete iNotifier;
        iNotifier = NULL;
    }

    if (IsActive())
    {
        Cancel();
    }

    iLbt.CreateTrigger(*trig, iTLbtTriggerId, ETrue, iStatus);
    CleanupStack::Pop(cond);

    SetActive();
}

// ----------------------------------------------------------------------------
// CProximityListener::StartTriggerL
// Start the trigger for Proximity updates
// ----------------------------------------------------------------------------
//
TInt CProximityListener::StartTriggerL()
{
    iCordinateMap[iTLbtTriggerId] = iCord;

    // Start the trigger
    iNotifier = CLbtTriggerFiringEventNotifier::NewL(iLbt,  *this);
    iNotifier->Start();
    iState = KListen;

    StartListeningStatusL();

    return iTLbtTriggerId;
}

// ----------------------------------------------------------------------------
// CProximityListener::StartListneingStatus
// Listen to the LBT status
// ----------------------------------------------------------------------------
//
void CProximityListener::StartListeningStatusL()
{
    JELOG2(EJavaLocation);
    //Get location triggering supervision status
    User::LeaveIfError(iProperty.Get(
                           KPSUidLbtStatusInformation,
                           KLbtLocationTriggeringSupervisionStatus,
                           iLTSupervisionStatus));

    //Listen for change event of location triggering
    //supervision status. //Attach to the key
    User::LeaveIfError(iProperty.Attach(
                           KPSUidLbtStatusInformation,
                           KLbtLocationTriggeringSupervisionStatus));

    switch (iLTSupervisionStatus)
    {
    case ELbtLocationTriggeringSupervisionOff :
    case ELbtLocationTriggeringSupervisionNotActive:
    case ELbtLocationTriggeringSupervisionFailed:
    case ELbtLocationTriggeringSupervisionNotReady:
    {
        iIsStatusNotActive = true;
    }
    break;

    case ELbtLocationTriggeringSupervisionSuccessful:
    {
        iIsStatusNotActive = false;
    }
    break;

    default:
        break;
    }

    iProperty.Subscribe(iStatus);
    SetActive();
}

// ----------------------------------------------------------------------------
// CProximityListener::DeleteTriggerL
// Delete trigger for Location Triggering Server
// ----------------------------------------------------------------------------
//
void CProximityListener::DeleteTriggerL(TInt aTriggerID)
{
    iLbt.DeleteTriggerL(aTriggerID);
}

// ----------------------------------------------------------------------------
// CProximityListener::TriggerFiredL
// CallBack Function is called when one or more triggers are fired.
// ----------------------------------------------------------------------------
//
void CProximityListener::TriggerFiredL(const TLbtTriggerFireInfo &  aFireInfo)
{
    JELOG2(EJavaLocation);

    iJni = mFunctionServer->getValidJniEnv();

    TCoordinate cord = iCordinateMap.find(aFireInfo.iTriggerId)->second;

    // This will cancel the registration for the cordinates which are fired
    iLbt.DeleteTriggerL(aFireInfo.iTriggerId);

    // Notify Java about the callback
    (*iJni).CallStaticVoidMethod(iLocationProviderImplClass,
                                 iProximityEventCallBack,
                                 aFireInfo.iTriggerId,
                                 cord.Latitude(),
                                 cord.Longitude(),
                                 cord.Altitude());
}

void CProximityListener::RunL()
{
    JELOG2(EJavaLocation);

    if (iState ==  KListen)
    {
        iJni = mFunctionServer->getValidJniEnv();

        //Get the new value
        User::LeaveIfError(iProperty.Get(
                               KPSUidLbtStatusInformation,
                               KLbtLocationTriggeringSupervisionStatus,
                               iLTSupervisionStatus));

        switch (iLTSupervisionStatus)
        {
        case ELbtLocationTriggeringSupervisionOff :
        case ELbtLocationTriggeringSupervisionNotActive:
        case ELbtLocationTriggeringSupervisionFailed:
        case ELbtLocationTriggeringSupervisionNotReady:
        {
            (*iJni).CallStaticVoidMethod(iLocationProviderImplClass,
                                         iProximityStateChangeCallBack,
                                         false);
            iIsStatusNotActive = true;
        }
        break;

        case ELbtLocationTriggeringSupervisionSuccessful:
        {
            if (iIsStatusNotActive)
            {
                (*iJni).CallStaticVoidMethod(iLocationProviderImplClass,
                                             iProximityStateChangeCallBack,
                                             true);
                iIsStatusNotActive = false;
            }
        }
        break;

        default:
            break;
        }
        iProperty.Subscribe(iStatus);
        SetActive();
    }
    else
    {
        iCreateTriggerMonitor->notify();
    }
}

void CProximityListener::DoCancel()
{
    JELOG2(EJavaLocation);
    iProperty.Cancel();
}
