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
* Description:  This class is used for creating video player.
*
*/


#ifndef CMMAVIDEOURLPLAYERFACTORY_H
#define CMMAVIDEOURLPLAYERFACTORY_H

//  INCLUDES
#include "cmmammfplayerfactory.h"
// CONSTANTS


//  CLASS DECLARATION
/**
*   This class is used for creating URL video players
*
*
*/

NONSHARABLE_CLASS(CMMAVideoUrlPlayerFactory): public CMMAMMFPlayerFactory
{
public: // Constructor and destructor
    static CMMAVideoUrlPlayerFactory* NewLC();
    ~CMMAVideoUrlPlayerFactory();

private: // Constructor
    CMMAVideoUrlPlayerFactory();

public: // From MMMAPlayerFactory
    CMMAPlayer* CreatePlayerL(const TDesC& aContentType);

    CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                              const TDesC& aMiddlePart,
                              const TDesC& aParameters);

    CMMAPlayer* CreatePlayerL(const TDesC8& aHeaderData);

    void GetSupportedContentTypesL(const TDesC& aProtocol,
                                   CDesC16Array& aMimeTypeArray);

    void GetSupportedProtocolsL(const TDesC& aContentType,
                                CDesC16Array& aProtocolArray);

public: // From CMMAMMFPlayerFactory
    CMMAPlayer* CreatePlayerL(
        CMMAMMFResolver* aResolver);


    void MediaIdsL(RArray<TUid>& aMediaIds);

    CMMFPluginSelectionParameters::TMediaIdMatchType
    MediaIdMatchType();

private: // Data
    HBufC* iUrl; // owned url
};

#endif // CMMAVIDEOURLPLAYERFACTORY_H
