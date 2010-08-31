/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides GlobalManager functionality.
*
*/


#ifndef CAMMSGLOBALMANAGER_H
#define CAMMSGLOBALMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <mmmaplayerinstanceobserver.h>

#include "cammsmodule.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CAMMSModuleContainer;
class CAMMSPlayerBuilderGroup;

// CLASS DECLARATION

/**
*
*  This class provides GlobalManager functionality.
*  The GlobalManager handles the creation of EffectModules, SoundSource3Ds
*  and MediaProcessors. Furthermore, a Spectator can be get from the
*  GlobalManager. GlobalManager extends the CAMMSModule and contains global
*  controls.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSGlobalManager): public CAMMSModule, public MMMAPlayerInstanceObserver
{
public:

    /**
    * Two-phased constructor.
    */
    static CAMMSGlobalManager* NewLC();

    /**
    * Destructor.
    */
    ~CAMMSGlobalManager();

public: // From MMMAPlayerInstanceObserver
    /**
    * MMA will call this method when new player is created.
    * @since 3.0
    * @param aPlayer Player to add.
    */
    void AddPlayerNotifyL(CMMAPlayer* aPlayer);

    /**
    * MMA will call this method when player does not exist anymore.
    * @since 3.0
    * @param aPlayer Player to remove.
    */
    void RemovePlayerNotify(CMMAPlayer* aPlayer);

public: // New functions
    /**
    * Initializes GlobalManager. This method is called when Java
    * GlobalManager is accessed for the first time.
    * This method creates mma controls to the added players and creates
    * global controls. After this method is called amms controls are added
    * to players immediately.
    * @param aPlayers Players created with mma api.
    * @since 3.0
    */
    void InitL(RPointerArray< CMMAPlayer >& aPlayers);

    /**
    * Creates a new sound source 3D module.
    * Module will be owned by global manager.
    * @since 3.0
    * @return New sound source 3D module.
    */
    CAMMSModule* CreateSoundSource3DL();

    /**
    * Creates a new effect module.
    * Module will be owned by global manager.
    * @since 3.0
    * @return New effect module.
    */
    CAMMSModule* CreateEffectModuleL();

    /**
    * Returns the spectator.
    * @since 3.0
    * @return Spectator.
    */
    CAMMSModule* Spectator();

    /**
    * Disposes module.
    * @param aModule Module to dispose.
    */
    void DisposeModule(CAMMSModule* aModule);

    /**
     * Checks whether the given player can be added to a module (EffectModule
     * or SoundSource3D).
     * @param aPlayer Player to be checked.
     * @return KErrNone if the player can be added or some error
     * code if the adding is not allowed.
     */
    TInt PlayerAddingAllowed(CMMAPlayer* aPlayer);

private:
    /**
    * C++ default constructor.
    */
    CAMMSGlobalManager();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:  // New functions

    /**
     * Checks whether the given player can be added to a module
     * according to the given module (is there any player in
     * the module that can prevent player adding to any module).
     * @param aPlayer Player to be checked.
     * @param aModule Module to be checked.
     * @return KErrNone if the player can be added or some error
     * code if the adding is not allowed.
     */
    TInt PlayerAddingAllowed(CMMAPlayer* aPlayer, CAMMSModule* aModule);


private:    // Data
    // Owned spectator
    CAMMSModule* iSpectator;

    // Owned effect modules
    CAMMSModuleContainer* iEffectModules;

    // Owned sound source 3Ds
    CAMMSModuleContainer* iSoundSource3Ds;

    // Owned player builder
    CAMMSPlayerBuilderGroup* iPlayerBuilder;
};

#endif // CAMMSGLOBALMANAGER_H


