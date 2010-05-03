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
* Description:  Class is used to find out from java is midlet foreground or not
*
*/


#ifndef CMMAFOREGROUND_H
#define CMMAFOREGROUND_H

#include <e32base.h>

#include "mmmaeventposter.h"
#include "cmmadeleterefevent.h"
#include "cmmaforegroundevent.h"

// xm-radio fix
#include "CMIDToolkit.h"
#include "lcdui.h"

#include <coemain.h>
#include <eikenv.h>

//  CLASS DECLARATION
/**
*
* Class is used to find out from java is midlet foreground or not
*/
NONSHARABLE_CLASS(CMMAForeground) : public CBase, public MMIDEnvObserver, public MCoeForegroundObserver
{
public:

    /**
     * Creates and returns a new instance of this class.
     * @param aEventPoster for posting event to java side
     * @param aForegroundListener java side foreground object
     * @param aJavaMethod java side method id
     * @return New instance from this class.
     */
    static CMMAForeground* NewL(MMMAEventPoster* aEventPoster,
    jobject aForegroundListener,
    jmethodID aJavaMethod,
    CMIDToolkit* aToolkit); // xm-radio fix
    /**
     * Destructor.
     */
    ~CMMAForeground();

protected:

    /**
     * C++ constructor
     */
    CMMAForeground();

    /**
     * Second phase constructor
     * @param aEventPoster for posting event to java side
     * @param aForegroundListener java side foreground object
     * @param aJavaMethod java side method id
     */
    void ConstructL(MMMAEventPoster* aEventPoster,
                    jobject aForegroundListener,
                    jmethodID aJavaMethod,
                    CMIDToolkit* aToolkit); // xm-radio fix


public: // New methods


    /**
     * Tells is midlet foreground or not
     * @return boolean
     */
    TBool IsForeground();

    /**
     * Sets member boolean iIsForeground
     */
    void SetForeground(TBool aIsForeground);

public: // from MMIDEnvObserver

    void HandleSwitchOnL(TBool /*aSwitchOn*/);

    /**
     * Handles the case when the MIDlet is brought to the foreground.
     */
    void HandleForegroundL(TBool /*aForeground*/);

    /**
     * Handles a change to resources which are shared accross the environment.
     */
    void HandleResourceChangeL(TInt /*aType*/);

public:
    /** Handles the application coming to the foreground. */
    void HandleGainingForeground();

    /** Handles the application going into the background. */
    void HandleLosingForeground();

private: // Member data

    CActiveSchedulerWait* iActiveScheduler; // owned
    MMMAEventPoster* iEventPoster; // not owned
    CMMAForegroundEvent* iForegroundEvent; // owned

    //Event is used to destroy reference to associated java object,
    //when foreground is destroyed.
    CMMADeleteRefEvent* iDeleteRefEvent; // owned

    jobject iForegroundListener;
    jmethodID iJavaMethod;

    TBool iIsForeground;
    // xm-radio fix
    CMIDToolkit* iToolkit;
    MMIDEnv* iMidEnv;
};

#endif // CMMAFOREGROUND_H
