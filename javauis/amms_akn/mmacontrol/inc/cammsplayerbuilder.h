/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for AMMS player builders.
*
*/


#ifndef CAMMSPLAYERBUILDER_H
#define CAMMSPLAYERBUILDER_H

//  INCLUDES
#include <cmmaplayer.h>

// CLASS DECLARATION

/**
*
*  Base class for AMMS player and control builders.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSPlayerBuilder): public CBase
{
public:  // Constructors and destructor
    /**
    * Destructor.
    */
    ~CAMMSPlayerBuilder();

public: // New methods
    /**
    * Adds the AMMS audio controls to the player
    *
    * @param aPlayer The MMA Player where the AMMS controls are added
    */
    virtual void PreparePlayerL(CMMAPlayer* aPlayer) = 0;

protected: //Protected so derived classes can access
    /**
    * C++ default constructor.
    */
    CAMMSPlayerBuilder();
};

#endif // CAMMSPLAYERBUILDER_H


