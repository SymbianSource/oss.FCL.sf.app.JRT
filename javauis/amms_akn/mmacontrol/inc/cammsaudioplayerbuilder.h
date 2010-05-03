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
* Description:  AMMS player builder for adding AMMS audio player controls.
*
*/


#ifndef CAMMSAUDIOPLAYERBUILDER_H
#define CAMMSAUDIOPLAYERBUILDER_H

//  INCLUDES
#include "cammsplayerbuilder.h"


// CLASS DECLARATION

/**
*
*  AudioControl player builder.
*  This class adds AMMS audio player controls.
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSAudioPlayerBuilder): public CAMMSPlayerBuilder
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CAMMSAudioPlayerBuilder* NewLC();

    /**
    * Destructor.
    */
    ~CAMMSAudioPlayerBuilder();

public: // Funtions from base classes
    /**
    * Adds the AMMS audio controls to the player
    *
    * @param aPlayer The Player where the AMMS controls are added
    */
    void PreparePlayerL(CMMAPlayer* aPlayer);

private:
    /**
    * C++ default constructor.
    */
    CAMMSAudioPlayerBuilder();

};

#endif // CAMMSAUDIOPLAYERBUILDER_H


