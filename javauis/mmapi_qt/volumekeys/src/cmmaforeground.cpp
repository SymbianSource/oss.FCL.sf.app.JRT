/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class is used to find out from java is midlet foreground or not
*
*/



// INCLUDE FILES
#include <e32std.h>

#include "cmmaforeground.h"
#include "cmmaforegroundevent.h"
//  INCLUDE FILES
#include <logger.h>

// xm-radio fix
//#include "cmidenv.h"

CMMAForeground* CMMAForeground::NewL(MMMAEventPoster* aEventPoster,
                                     jobject aForegroundListener,
                                     jmethodID aJavaMethod/* ,
                                     CMIDToolkit* aToolkit*/)
{
    LOG(EJavaMMAPI, EInfo, "CMMAForeground::NewL + ");
    CMMAForeground* self = new(ELeave) CMMAForeground;

    CleanupStack::PushL(self);
    self->ConstructL(aEventPoster, aForegroundListener, aJavaMethod/*, aToolkit*/);
    CleanupStack::Pop();

    LOG(EJavaMMAPI, EInfo, "CMMAForeground::NewL - ");
    return self;
}

CMMAForeground::~CMMAForeground()
{
    LOG(EJavaMMAPI, EInfo, "CMMAForeground::~CMMAForeground() + ");
    /* if (iMidEnv)
     {
         // unregister for getting the foreground change event
         LOG( EJavaMMAPI, EInfo, "CMMAForeground::~CMMAForeground() : iMidEnv RemoveObserver +");
         iMidEnv->RemoveObserver(*this);
         LOG( EJavaMMAPI, EInfo, "CMMAForeground::~CMMAForeground() : iMidEnv RemoveObserver -");
     }

     CEikonEnv* eikEnv = ((CEikonEnv*)CEikonEnv::Static());

     if (eikEnv)
     {
         LOG( EJavaMMAPI, EInfo, "CMMAForeground::~CMMAForeground() : eikEnv RemoveForegroundObserver +");
         eikEnv->RemoveForegroundObserver(*this);
         LOG( EJavaMMAPI, EInfo, "CMMAForeground::~CMMAForeground() : eikEnv RemoveForegroundObserver -");
     }
    */
    if (iEventPoster && iDeleteRefEvent)
    {
        iEventPoster->PostEvent(iDeleteRefEvent);
    }
    else
    {
        delete iDeleteRefEvent;
    }
    delete iForegroundEvent;
    delete iActiveScheduler;
    LOG(EJavaMMAPI, EInfo, "CMMAForeground::~CMMAForeground() - ");
}

CMMAForeground::CMMAForeground()
        : iIsForeground(ETrue) // Initialize IsForeground to true, we might have already missed the event (HandleForegroundL), events before observer registration
{
}

void CMMAForeground::ConstructL(MMMAEventPoster* aEventPoster,
                                jobject aForegroundListener,
                                jmethodID aJavaMethod /* ,
                                CMIDToolkit* aToolkit*/) // xm-radio fix
{
    LOG1(EJavaMMAPI, EInfo, "CMMAForeground::ConstructL + ISFOREGROUND = %d",iIsForeground);

    iActiveScheduler = new(ELeave) CActiveSchedulerWait();
    iEventPoster = aEventPoster;
    iForegroundListener = aForegroundListener;
    iJavaMethod = aJavaMethod;

    iForegroundEvent = new(ELeave) CMMAForegroundEvent(
        aForegroundListener,
        aJavaMethod,
        CMMAEvent::EReusableEvent);
    iDeleteRefEvent = new(ELeave) CMMADeleteRefEvent(aForegroundListener);

    // xm-radio fix
    /*
    iToolkit = aToolkit;

    if (iToolkit)
    {
        DEBUG("CMMAForeground::ConstructL : iToolkit->Env(); +");
        iMidEnv = iToolkit->Env();
        DEBUG("CMMAForeground::ConstructL : iToolkit->Env(); -");
    }

    if (iMidEnv)
    {
        DEBUG("CMMAForeground::ConstructL : iMidEnv->AddObserverL + ");
        // register for getting the foreground change event
        iMidEnv->AddObserverL(*this);
        DEBUG("CMMAForeground::ConstructL : iMidEnv->AddObserverL - ");
    }

    CEikonEnv* eikEnv = ((CEikonEnv*)CEikonEnv::Static());

    if (eikEnv)
    {
        DEBUG("CMMAForeground::ConstructL - eikAppUi->AddForegroundObserverL() + ");
        eikEnv->AddForegroundObserverL(*this);
        DEBUG("CMMAForeground::ConstructL - eikAppUi->AddForegroundObserverL() - ");
    }
     */
    LOG(EJavaMMAPI, EInfo, "CMMAForeground::ConstructL - ");
}

TBool CMMAForeground::IsForeground()
{
    LOG2(EJavaMMAPI, EInfo, "THREADID = %d : CMMAForeground::IsForeground : ISFOREGROUND = %d",RThread().Id().Id(),iIsForeground);
    return iIsForeground;
}

void CMMAForeground::SetForeground(TBool aIsForeground)
{
    iIsForeground = aIsForeground;
    LOG1(EJavaMMAPI, EInfo, "CMMAForeground::SetForeground - ISFOREGROUND = %d",iIsForeground);
}

// Implement MMIDEnvObserver
/*
void CMMAForeground::HandleSwitchOnL(TBool aSwitchOn)
{
    // Dummy implementation, no intent to do anything
}
*/
/**
 * Handles the case when the MIDlet is brought to the foreground.
 */
/*
void CMMAForeground::HandleForegroundL(TBool aForeground)
{
   LOG1( EJavaMMAPI, EInfo, "CMMAForeground::HandleForegroundL %d", aForeground);
   iIsForeground = aForeground;
}
*/
/**
 * Handles a change to resources which are shared accross the environment.
 */
/*void CMMAForeground::HandleResourceChangeL(TInt aType)
{
    // Dummy implementation, no intent to do anything
}
*/
// Handles the application coming to the foreground.
void CMMAForeground::HandleGainingForeground()
{
    iIsForeground = ETrue;
}

// Handles the application going into the background.
void CMMAForeground::HandleLosingForeground()
{
    iIsForeground = EFalse;
}
//  End of File
