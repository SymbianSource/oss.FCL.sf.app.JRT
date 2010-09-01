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
* Description:  This class is used for volume setting
*
*/


#ifndef CMMAVOLUMECONTROL_H
#define CMMAVOLUMECONTROL_H

//  INCLUDES
#include "cmmacontrol.h"
#include "mmmaplayerstatelistener.h"
#include <MProfileChangeObserver.h>

// CONSTANS
_LIT(KMMAVolumeControlName, "VolumeControl");
const TInt KMMAVolumeMaxLevel = 100;

class CMMAPlayer;
class CProfileChangeNotifyHandler;
//  CLASS DECLARATION
/**
*   This class is used for volume setting
*
*
*/

class CMMAVolumeControl : public CMMAControl,
        public MMMAPlayerStateListener,
        public MProfileChangeObserver
{
public:
    static void StaticSetLevelL(CMMAVolumeControl* aVolumeControl,
                                TInt aLevel);
    static void StaticGetLevelL(CMMAVolumeControl* aVolumeControl,
                                TInt* aLevel);

    IMPORT_C ~CMMAVolumeControl();

protected:
    IMPORT_C CMMAVolumeControl(CMMAPlayer* aPlayer);
    IMPORT_C void ConstructBaseL();

    /**
     * Sets the level to player.
     * @param aLevel Sound level to set.
     */
    virtual void DoSetLevelL(TInt aLevel) = 0;

    /**
     * Get volume level from player.
     * @return Current level.
     */
    virtual TInt DoGetLevelL() = 0;

public: // From CMMAControl
    IMPORT_C const TDesC& ClassName() const;

    /**
     * Refresh volume control.
     */
    IMPORT_C void RefreshControl();

public: // From MMMAPlayerStateListener
    IMPORT_C void StateChanged(TInt aState);

public: // from MProfileChangeObserver
    IMPORT_C void HandleActiveProfileEventL(TProfileEvent aProfileEvent, TInt aProfileId );

public: // New methods
    /**
     * Adds new volume level to volume control. Actual volume level will
     * be calculated using all levels added to the control.
     * @return Index for new level.
     */
    IMPORT_C TInt AddLevelL();

    /**
     * Sets new volume level for certain level index. After level is set
     * new actual level is calculated and commited to player in derived
     * class.
     * @param aLevelIndex Level's index
     * @param aVolumeLevel New volume level for level index. Level must be
     * between 0 and 100.
     */
    IMPORT_C void SetVolumeLevelL(TInt aLevelIndex,
                                  TInt aVolumeLevel);
    void GetVolumeLevelL(TInt aLevelIndex,
                         TInt* aVolumeLevel);
    void SetProfilesBasedSoundMutingL();
    /**
     * Sets the audio o/p preference. Based on this the profile based
     * volume setting is done.
     */
    IMPORT_C void SetAudioOutputPreferenceL( TInt aRoutingPreference);
    
    void InitializeGlobalVolumeLevel(TInt aGlobalVolumeLevel);

private: // New methods

    /**
     * Refresh volume level, basically set the volume level again.
     */
    void RefreshVolume();

    /**
     * Sets level if player is started and sends java event level
     * changed.
     * @param aLevel Sound level to set.
     */
    void SetLevelL(TInt aLevel);

    /**
     * Calculates new overall level using all values in iLevels array.
     * @return Current level.
     */
    TInt CalculateLevel();
    
    void SetJavaSoundVolumeLevelL();
    
    void SetProfileSoundVolumeLevelL();
protected: // data
    CMMAPlayer* iPlayer;

    // Previous level setted to player. Used to check if level is changed.
    TInt iLevel;

    // Array containing all levels.
    RArray< TInt > iLevels;
    // profile change notifier 
    // Owning
    CProfileChangeNotifyHandler* iProfChangeNotifier;
    // Current audio o/p preference.
    TInt iAudioOutputPreference;
    // Current profile Id.
    TInt iProfileId;
private:
    // The Global Volume level at the time when a player is created
    TInt iInitialGlobalVolumeLevel;
};

#endif // CMMAVOLUMECONTROL_H
