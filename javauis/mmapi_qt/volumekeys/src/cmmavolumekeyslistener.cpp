/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles the volume keysupdate events
*
*/


// INCLUDE FILES
/*#include <remconcoreapitarget.h>
#include <remconcoreapitargetobserver.h>
#include <remconinterfaceselector.h>
*/
//#include <aknconsts.h>
#include <logger.h>

#include "cmmavolumekeyslistener.h"

// CONSTANTS

// Keyboard delays and repeat periods
//const TInt KFirstTimerExpiryInterval = KAknKeyboardRepeatInitialDelay;
// For second time onwards ,the duration of the time interval, is below
// Should not be greater than 1 Minute
// const TInt KTimerExpiryInterval = KAknStandardKeyboardRepeatRate;

CMMAVolumeKeysListener* CMMAVolumeKeysListener::NewL(MMMAVolumeKeysListener* aListener)
{
    CMMAVolumeKeysListener* self = new(ELeave) CMMAVolumeKeysListener();
    CleanupStack::PushL(self);
    self->ConstructL(aListener);
    CleanupStack::Pop();
    return self;
}

CMMAVolumeKeysListener::~CMMAVolumeKeysListener()
{
    LOG(EJavaMMAPI, EInfo, "CMMAVolumeKeysListener::~CMMAVolumeKeysListener() +");
    /* if ((NULL != iJavaRemConObserver) && (NULL != iJavaRemConManager))
     {
         iJavaRemConManager->RemoveObserver(*iJavaRemConObserver);
     }

     if (NULL != iJavaRemConObserver)
     {
         delete iJavaRemConObserver;
         iJavaRemConObserver = NULL;
     }

     if (NULL != iJavaRemConManager)
     {
         delete iJavaRemConManager;
         iJavaRemConManager = NULL;
     }
    */
    /*
    if (NULL != iTimer)
    {
        iTimer->Cancel();
        delete iTimer;
    }*/
    LOG(EJavaMMAPI, EInfo, "CMMAVolumeKeysListener::~CMMAVolumeKeysListener() -");
}

CMMAVolumeKeysListener::CMMAVolumeKeysListener()
{
}

void CMMAVolumeKeysListener::ConstructL(MMMAVolumeKeysListener* aListener)
{
    LOG(EJavaMMAPI, EInfo, "CMMAVolumeKeysListener::ConstructL +");
    /*iJavaRemConObserver = CJavaRemConObserver::NewL(*this);
    iJavaRemConManager = CJavaRemConManager::NewL();
    iJavaRemConManager->SetObserverL(*iJavaRemConObserver);
    */
    // Timer for implementing Pressed/Released/Clicked feature.
    //TODO For Mediakey(Hard VolumeKey)
    //iTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    iListener = aListener;
    LOG(EJavaMMAPI, EInfo, "CMMAVolumeKeysListener::ConstructL -");
}

/*
// ----------------------------------------------------------------------------
// void CMMAVolumeKeysListener::MrccatoPlay()
// Called when Play button is pressed/clicked/released
// ----------------------------------------------------------------------------
void CMMAVolumeKeysListener::MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed,
        TRemConCoreApiButtonAction aButtonAct)
{
}
// ----------------------------------------------------------------------------
// void CMMAVolumeKeysListener::MrccatoCommand()
// This function receives the events when following buttons are press/click/rel
// - Volume Up , Volume Down
// ----------------------------------------------------------------------------
void CMMAVolumeKeysListener::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
        TRemConCoreApiButtonAction aButtonAct)
{
    switch (aOperationId)
    {
    case ERemConCoreApiVolumeUp:
    {
        iUp = ETrue;
        switch (aButtonAct)
        {
        case ERemConCoreApiButtonPress:
        {
            // Start Timer
            if (!iTimer->IsActive())
            {
                // Start Timer
                iTimer->Start(KFirstTimerExpiryInterval,
                              KTimerExpiryInterval,
                              TCallBack(TimerCallback, this));
            }
            break;
        }
        case ERemConCoreApiButtonRelease:
        {
            if (iTimer->IsActive())
            {
                iTimer->Cancel();
            }
            break;
        }
        case ERemConCoreApiButtonClick:
        {
            iListener->VolumeUp();
            break;
        }
        default:
        {
            break;
        }
        }
        break;
    }
    case ERemConCoreApiVolumeDown:
    {
        iUp = EFalse;
        switch (aButtonAct)
        {
        case ERemConCoreApiButtonPress:
        {
            // Start Timer
            if (!iTimer->IsActive())
            {
                // Start Timer
                iTimer->Start(KFirstTimerExpiryInterval,
                              KTimerExpiryInterval,
                              TCallBack(TimerCallback, this));
            }
            break;
        }
        case ERemConCoreApiButtonRelease:
        {
            if (iTimer->IsActive())
            {
                iTimer->Cancel();
            }
            break;
        }
        case ERemConCoreApiButtonClick:
        {
            iListener->VolumeDown();
            break;
        }
        default:
        {
            break;
        }
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

// ----------------------------------------------------------------------------
// void CMMAVolumeKeysListener::MrccatoTuneFunction()
//
// ----------------------------------------------------------------------------

void CMMAVolumeKeysListener::MrccatoTuneFunction(TBool aTwoPart,
        TUint aMajorChannel,
        TUint aMinorChannel,
        TRemConCoreApiButtonAction aButtonAct)
{
}

// ----------------------------------------------------------------------------
// void CMMAVolumeKeysListener::MrccatoSelectDiskFunction()
//
// ----------------------------------------------------------------------------
void CMMAVolumeKeysListener::MrccatoSelectDiskFunction(TUint aDisk,
        TRemConCoreApiButtonAction aButtonAct)
{
}

// ----------------------------------------------------------------------------
// void CMMAVolumeKeysListener::MrccatoSelectAvInputFunction()
//
// ----------------------------------------------------------------------------
void CMMAVolumeKeysListener::MrccatoSelectAvInputFunction(TUint8 aAvInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct)
{
}

// ----------------------------------------------------------------------------
// void CMMAVolumeKeysListener::MrccatoSelectAudioInputFunction()
//
// ----------------------------------------------------------------------------
void CMMAVolumeKeysListener::MrccatoSelectAudioInputFunction(TUint8 aAudioInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct)
{
}

*/
// -----------------------------------------------------------------------
// CMMAVolumeKeysListener::HandleTimerEvent
// Description: When Timer is expired, this function is called from
//              callback function.
// return :
// -----------------------------------------------------------------------
//
void CMMAVolumeKeysListener::HandleTimerEvent()
{
    LOG(EJavaMMAPI, EInfo, "CMMAVolumeKeysListener::HandleTimerEvent +");
    if (iUp)
    {
        iListener->VolumeUp();
    }
    else
    {
        iListener->VolumeDown();
    }
    LOG(EJavaMMAPI, EInfo, "CMMAVolumeKeysListener::HandleTimerEvent -");
}

// -----------------------------------------------------------------------
// CMMAVolumeKeysListener::TimerCallback
// Description: When Timer is expired, this function is called
// return :
// -----------------------------------------------------------------------
//
TInt CMMAVolumeKeysListener::TimerCallback(TAny* aThis)
{
    CMMAVolumeKeysListener* listener =
        static_cast< CMMAVolumeKeysListener* >(aThis);
    listener->HandleTimerEvent();
    return 0;
}

//  End of File
