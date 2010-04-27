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
* Description:  This class uses player factories to generate different players
*
*/


#ifndef CMMAMANAGER_H
#define CMMAMANAGER_H

//  EXTERNAL INCLUDES
#include <badesca.h>

//  INTERNAL INCLUDES
#include "CMMAEventSource.h"
#include "MMMAPlayerFactory.h"


//  FORWARD DECLARATIONS
class CMMAPlayer;

//  CLASS DECLARATION
/**
*   This class uses player factories to generate different types of players.
*/

NONSHARABLE_CLASS(CMMAManager): public CBase, public MMMAPlayerFactory
{
public: // Constructors and destructors
    /**
     * Deletes all owned members.
     */
    virtual ~CMMAManager();

    /**
     * Static constructor
     */
    static void StaticCreateManagerL(CMMAManager** aManager,
                                     TInt aMIDletSuiteID);

protected:
    /**
     * Initializes member variables to defaults.
     */
    CMMAManager();

    /**
     * Second phase construct.
     */
    void ConstructL(TInt aMIDletSuiteID);

public: // From MMMAPlayerFactory

    /**
     * @see MMAPlayerFactory
     */

    CMMAPlayer* CreatePlayerL(const TDesC& aContentType);

    CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                              const TDesC& aMiddlePart,
                              const TDesC& aParameters);

    CMMAPlayer* CreatePlayerL(const TDesC8& aHeaderData);

    void GetSupportedContentTypesL(const TDesC& aProtocol,
                                   CDesC16Array& aMimeTypeArray);

    void GetSupportedProtocolsL(const TDesC& aContentType,
                                CDesC16Array& aProtocolArray);
public: // new methods

    /**
     * Adds player factory to manager
     *
     * @param aPlayerFactory handle to player factory
     */
    IMPORT_C void AddPlayerFactoryL(MMMAPlayerFactory* aPlayerFactory);

private:
    /**
     * Array of player factories. Owned.
     */
    RPointerArray< MMMAPlayerFactory > iPlayerFactories;
};

#endif // CMMAMANAGER_H
