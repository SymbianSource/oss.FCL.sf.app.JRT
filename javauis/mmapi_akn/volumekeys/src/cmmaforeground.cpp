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
#include <jdebug.h>

// xm-radio fix
#include "CMIDEnv.h"

CMMAForeground* CMMAForeground::NewL(MMMAEventPoster* aEventPoster,
                                     jobject aForegroundListener,
                                     jmethodID aJavaMethod ,
                                     CMIDToolkit* aToolkit)
{
    DEBUG("CMMAForeground::NewL + ");
    CMMAForeground* self = new(ELeave) CMMAForeground;

    CleanupStack::PushL(self);
    self->ConstructL(aEventPoster, aForegroundListener, aJavaMethod, aToolkit);
    CleanupStack::Pop();

    DEBUG("CMMAForeground::NewL - ");
    return self;
}

CMMAForeground::~CMMAForeground()
{
    DEBUG("CMMAForeground::~CMMAForeground() + ");
    if (iMidEnv)
    {
        // unregister for getting the foreground change event
        DEBUG("CMMAForeground::~CMMAForeground() : iMidEnv RemoveObserver +");
        iMidEnv->RemoveObserver(*this);
        DEBUG("CMMAForeground::~CMMAForeground() : iMidEnv RemoveObserver -");
    }

    CEikonEnv* eikEnv = ((CEikonEnv*)CEikonEnv::Static());

    if (eikEnv)
    {
        DEBUG("CMMAForeground::~CMMAForeground() : eikEnv RemoveForegroundObserver +");
        eikEnv->RemoveForegroundObserver(*this);
        DEBUG("CMMAForeground::~CMMAForeground() : eikEnv RemoveForegroundObserver -");
    }

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
    DEBUG("CMMAForeground::~CMMAForeground() - ");
}

CMMAForeground::CMMAForeground()
        : iIsForeground(ETrue) // Initialize IsForeground to true, we might have already missed the event (HandleForegroundL), events before observer registration
{
}

void CMMAForeground::ConstructL(MMMAEventPoster* aEventPoster,
                                jobject aForegroundListener,
                                jmethodID aJavaMethod ,
                                CMIDToolkit* aToolkit) // xm-radio fix
{
    DEBUG_INT("CMMAForeground::ConstructL + ISFOREGROUND = %d",iIsForeground);

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

    DEBUG("CMMAForeground::ConstructL - ");
}

TBool CMMAForeground::IsForeground()
{
    DEBUG_INT2("THREADID = %d : CMMAForeground::IsForeground : ISFOREGROUND = %d",RThread().Id(),iIsForeground);
    return iIsForeground;
}

void CMMAForeground::SetForeground(TBool aIsForeground)
{
    iIsForeground = aIsForeground;
    DEBUG_INT("CMMAForeground::SetForeground - ISFOREGROUND = %d",iIsForeground);
}

// Implement MMIDEnvObserver

void CMMAForeground::HandleSwitchOnL(TBool /*aSwitchOn*/)
{
    // Dummy implementation, no intent to do anything
}

/**
 * Handles the case when the MIDlet is brought to the foreground.
 */
void CMMAForeground::HandleForegroundL(TBool aForeground)
{
    DEBUG_INT("CMMAForeground::HandleForegroundL %d", aForeground);
    iIsForeground = aForeground;
}

/**
 * Handles a change to resources which are shared accross the environment.
 */
void CMMAForeground::HandleResourceChangeL(TInt /*aType*/)
{
    // Dummy implementation, no intent to do anything
}

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
