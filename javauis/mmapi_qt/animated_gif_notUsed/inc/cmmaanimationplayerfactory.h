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
* Description:  This class is used for creating animation player
*
*/


#ifndef CMMAANIMATIONPLAYERFACTORY_H
#define CMMAANIMATIONPLAYERFACTORY_H

//  INCLUDES
#include "mmmaplayerfactory.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CMMAAnimationPlayer;

//  CLASS DECLARATION
/**
*   This class is used for creating animation player.
*
*/
NONSHARABLE_CLASS(CMMAAnimationPlayerFactory): public MMMAPlayerFactory
{
public: // Constructor and destructor
    static CMMAAnimationPlayerFactory* NewLC();
    ~CMMAAnimationPlayerFactory();

private: // Constructor
    CMMAAnimationPlayerFactory();

public: // From MMMAPlayerFactory

    // Creates new player according to a content type.
    virtual CMMAPlayer* CreatePlayerL(const TDesC& aContentType);

    // Creates new player according to a locator
    virtual CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                                      const TDesC& aMiddlePart,
                                      const TDesC& aParameters);

    // Creates new player according to a header data
    virtual CMMAPlayer* CreatePlayerL(const TDesC8& aHeaderData);

    // Gets player's supported content types.
    virtual void GetSupportedContentTypesL(const TDesC& aProtocol,
                                           CDesC16Array& aMimeTypeArray);

    // Gets player's supported protocols for the content type.
    virtual void GetSupportedProtocolsL(const TDesC& aContentType,
                                        CDesC16Array& aProtocolArray);

private:
    // Create instance of animation player with sufficient controls
    CMMAAnimationPlayer* CreateAnimationPlayerL();

    // Create instance of animation player with sufficient controls, with specified file
    CMMAAnimationPlayer* CreateAnimationPlayerL(const TDesC& aFileName);

    // Add animation controls to player
    void AddControlsL(CMMAAnimationPlayer* aPlayer);

};

#endif // CMMAANIMATIONPLAYERFACTORY_H
