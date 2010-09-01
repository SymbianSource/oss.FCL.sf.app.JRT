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
* Description:  AMMS player builder group for adding players.
*
*/


#ifndef CAMMSPLAYERBUILDERGROUP_H
#define CAMMSPLAYERBUILDERGROUP_H

//  INCLUDES
#include "cammsplayerbuilder.h"

// CLASS DECLARATION

/**
*
*  This class is used to add AMMS audio controls to the player.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSPlayerBuilderGroup): public CAMMSPlayerBuilder
{
public:  // Constructors and destructor
    /**
    * Two-phased constructor.
    */
    static CAMMSPlayerBuilderGroup* NewL();

    /**
    * Two-phased constructor.
    */
    static CAMMSPlayerBuilderGroup* NewLC();

    /**
    * Destructor.
    */
    ~CAMMSPlayerBuilderGroup();

public: // Funtions from base classes
    /**
    * Adds the AMMS audio controls to the player
    *
    * @param aPlayer The MMA Player where the AMMS controls are added
    */
    virtual void PreparePlayerL(CMMAPlayer* aPlayer);

public: // New methods
    /**
    * Adds the AMMS control builder
    *
    * @param aPlayerBuilder The AMMS player builder
    */
    void AddBuilderAndPopL(CAMMSPlayerBuilder* aPlayerBuilder);

private:
    /**
    * C++ default constructor.
    */
    CAMMSPlayerBuilderGroup();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private: // Data

    CArrayPtrSeg< CAMMSPlayerBuilder >* iPlayerBuilders; // Owned

};

#endif // CAMMSPLAYERBUILDERGROUP_H


