/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for playing sounds
*
*/

#ifndef CMMAMMFPLAYERBASE_H
#define CMMAMMFPLAYERBASE_H

//  INCLUDES
#include <mmf/common/mmfstandardcustomcommands.h>
#include "cmmaplayer.h"

class CMMAMMFResolver;
//  CLASS DECLARATION
/**
*   This is base class for players those use MMF
*
*
*/

class CMMAMMFPlayerBase : public CMMAPlayer,
        public MMMFControllerEventMonitorObserver
{
protected:
    //   C++ constructor
    CMMAMMFPlayerBase(CMMAMMFResolver* aResolver);
    void ConstructL();

    ~CMMAMMFPlayerBase();
public: // new methods
    /**
     * Getter for RMMFController
     */
    IMPORT_C RMMFController& Controller();

    /**
     * Check whether this player is playing from a file locator
     * @return ETrue if is a file player, EFalse otherwise
     */
    TBool IsFilePlayer();

public: // from CMMAPlayer
    void StartL();
    void StopL(TBool aPostEvent);
    void DeallocateL();
    IMPORT_C void GetDuration(TInt64* aDuration);
    void SetMediaTimeL(TInt64* aTime);
    void GetMediaTime(TInt64* aMediaTime);
    void CloseL();

protected: // New methods

    /**
     * Loops through iControllerInfos and tries to open those
     * @param aSourceUid Data source uid
     * @param aSourceData Data for source
     * @param aSinkUid Data sink uid
     * @param aSinkData Data for sink.
     * @param aPrioritySettings Controller's priority.
     * @return KErrNone if controller was opened
     */
    IMPORT_C virtual TInt DoOpen(TUid aSourceUid,
                                 const TDesC8& aSourceData,
                                 TUid aSinkUid,
                                 const TDesC8& aSinkData,
                                 TMMFPrioritySettings aPrioritySettings);

public: // from MMMFControllerEventMonitorObserver
    void HandleEvent(const class TMMFEvent& aEvent);

protected:

    /**
     * Controller infos, owned
     */
    RMMFControllerImplInfoArray* iControllerInfos;

    /**
     * Used to control mmf plugin.
     */
    RMMFController iController;

    /**
     * Owned member.
     * Event monitor is used to monitor iController for events.
     * If an event occurs, this class will be notified via the
     * MMMFControllerEventMonitorObserver interface
     */
    CMMFControllerEventMonitor* iEventMonitor;

    /**
     * Filename used for playing directly from file, owned
     */
    HBufC* iFileName;

    /**
    * Cached media time
    */
    TInt64 iMediaTime;

    /**
    * The time that will be sent with CMMAPlayerEvent::EStarted
    * (may be different from iMediaTime).
    */
    TInt64 iStartedEventTime;
};

#endif // CMMAMMFPLAYERBASE_H
