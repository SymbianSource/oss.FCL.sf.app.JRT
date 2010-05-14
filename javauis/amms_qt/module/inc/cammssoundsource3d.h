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
* Description:  Module for holding control groups belonging to SoundSource3D.
*
*/


#ifndef CAMMSSOUNDSOURCE3D_H
#define CAMMSSOUNDSOURCE3D_H

//  INCLUDES
#include "cammsmodule.h"

// FORWARD DECLARATIONS
class CMMAPlayer;
class CAMMSGlobalManager;


// CLASS DECLARATION

/**
*  Module for holding control groups belonging to SoundSource3D.
*
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSSoundSource3D): public CAMMSModule
{
public:
    /**
    * Two-phased constructor.
    *
    * @param aSpectator Spectator.
    * @param aGlobalManager Global manager.
    */
    static CAMMSSoundSource3D* NewLC(CAMMSModule* aSpectator,
    CAMMSGlobalManager* aGlobalManager);

    /**
    * Destructor.
    */
    ~CAMMSSoundSource3D();

public: // New functions

    /**
     * From CAMMSModule.
     */
    void AddPlayerNoStateCheckL(CMMAPlayer* aPlayer);

    /**
     * From CAMMSModule.
     */
    TInt RemovePlayerNoStateCheck(CMMAPlayer* aPlayer);

protected:
    /**
    * C++ default constructor.
    *
    * @param aSpectator Spectator.
    * @param aGlobalManager Global manager.
    */
    CAMMSSoundSource3D(CAMMSModule* aSpectator,
                       CAMMSGlobalManager* aGlobalManager);

protected:
    /**
    * Protected because derived classes must call ConstructL.
    */
    void ConstructL();

private: // Data

    CAMMSModule* iSpectator;  // Not owned.
    CAMMSGlobalManager* iGlobalManager;  // Not owned.
};

#endif // CAMMSSOUNDSOURCE3D_H


