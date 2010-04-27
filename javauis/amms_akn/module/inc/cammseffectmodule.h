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
* Description:  Module for holding control groups belonging to EffectModule.
*
*/


#ifndef CAMMSEFFECTMODULE_H
#define CAMMSEFFECTMODULE_H

//  INCLUDES
#include "cammsmodule.h"

// FORWARD DECLARATIONS
class CMMAPlayer;
class CAMMSGlobalManager;


// CLASS DECLARATION

/**
*  Module for holding control groups belonging to EffectModule.
*
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSEffectModule): public CAMMSModule
{
public:
    /**
    * Two-phased constructor.
    *
    * @param aGlobalManager Global manager.
    */
    static CAMMSEffectModule* NewLC(CAMMSGlobalManager* aGlobalManager);

    /**
    * Destructor.
    */
    ~CAMMSEffectModule();

public: // From CAMMSModule

    void AddPlayerNoStateCheckL(CMMAPlayer* aPlayer);

protected:
    /**
    * C++ default constructor.
    *
    * @param aGlobalManager Global manager.
    */
    CAMMSEffectModule(CAMMSGlobalManager* aGlobalManager);

    void ConstructL();

private: // Data

    CAMMSGlobalManager* iGlobalManager;  // Not owned.
};

#endif // CAMMSEFFECTMODULE_H


