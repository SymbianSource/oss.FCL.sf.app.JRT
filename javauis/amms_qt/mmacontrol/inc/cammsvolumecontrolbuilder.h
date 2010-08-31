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
* Description:  VolumeControl player builder.
*
*/


#ifndef CAMMSVOLUMECONTROLBUILDER_H
#define CAMMSVOLUMECONTROLBUILDER_H

//  INCLUDES
#include "cammsplayerbuilder.h"

// CLASS DECLARATION

/**
*
*  VolumeControl player builder.
*  This class adds CAMMSVolumeControl to all players that have
*  CMMAVolumeControl.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSVolumeControlBuilder): public CAMMSPlayerBuilder
{
public:  // Constructors and destructor
    /**
    * Two-phased constructor.
    */
    static CAMMSVolumeControlBuilder* NewLC();

    /**
    * Destructor.
    */
    ~CAMMSVolumeControlBuilder();

public: // Funtions from CAMMSPlayerBuilder
    void PreparePlayerL(CMMAPlayer* aPlayer);

private:
    /**
    * C++ default constructor.
    */
    CAMMSVolumeControlBuilder();
};

#endif // CAMMSVOLUMECONTROLBUILDER_H


