/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for stoptime controlling
*
*/


#ifndef CMMASTOPTIMECONTROL_H
#define CMMASTOPTIMECONTROL_H

//  INCLUDES
#include "cmmacontrol.h"
#include "mmmaplayerstatelistener.h"

class CMMAPlayer;

//  CLASS DECLARATION
/**
*   This class is used for stoptime controlling
*
*
*/

NONSHARABLE_CLASS(CMMAStopTimeControl): public CMMAControl, MMMAPlayerStateListener
{
protected:
    /**
     * Protected class for timing.
     */
    class CStopTimer : public CTimer
    {
    public:
        static CStopTimer* NewL(CMMAStopTimeControl* aControl);

    private:
        CStopTimer(CMMAStopTimeControl* aControl);
        void ConstructL();

    public: // From CTimer
        void RunL();

    protected:
        CMMAStopTimeControl* iControl;
    };

public:
    static CMMAStopTimeControl* NewL(CMMAPlayer* aPlayer);
    ~CMMAStopTimeControl();

protected:
    CMMAStopTimeControl(CMMAPlayer* aPlayer);
    void ConstructL();

public: // new methods
    static void StaticGetStopTime(CMMAStopTimeControl* aControl,
                                  TInt64* aTime);
    static void StaticSetStopTimeL(CMMAStopTimeControl* aControl,
                                   TInt64* aTime);
    virtual void StopAtTimeL();

protected:
    virtual void GetStopTime(TInt64& aTime);
    virtual void SetStopTimeL(const TInt64& aTime);
    virtual void StartTimer(const TInt64& aTime);

public: // From CMMAControl
    const TDesC& ClassName() const;

public: // From MMMAPlayerStateListener
    void StateChanged(TInt aState);

protected:
    CMMAPlayer* iPlayer;
    TInt64 iStopTime;   // Stop time in microseconds
    const TInt64 iNoTimer;

private:
    CStopTimer* iTimer;

};

#endif // CMMASTOPTIMECONTROL_H
