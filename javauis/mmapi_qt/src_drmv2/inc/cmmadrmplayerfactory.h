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
* Description:  This class is used for creating DRM v2 players.
*
*/


#ifndef CMMADRMPLAYERFACTORY_H
#define CMMADRMPLAYERFACTORY_H

//  INCLUDES
#include "mmmaplayerfactory.h"
#include "cmmavideoplayerfactory.h"

class CMMAMMFResolver;

//  CLASS DECLARATION
/**
*   This class is used for creating DRM players.
*
*
*/
NONSHARABLE_CLASS(CMMADRMPlayerFactory): public CBase, public MMMAPlayerFactory
{
public: // Constructor and destructor
    static CMMADRMPlayerFactory* NewLC(CMMAVideoPlayerFactory* aVideoPlayerFactory);

    CMMADRMPlayerFactory(CMMAVideoPlayerFactory* aVideoPlayerFactory);

    ~CMMADRMPlayerFactory();

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

protected:
    void UpdateRightsL(TInt aError,
                       const TDesC& aFileName);

    CMMAPlayer* CreateAudioPlayerL(const TDesC& aContentType,
                                   const TDesC& aFileName);

    CMMAPlayer* CreateVideoPlayerL(const TDesC& aContentType,
                                   const TDesC& aFileName);


private: //data
    CMMAVideoPlayerFactory* iVideoPlayerFactory;

};

#endif // CMMADRMPLAYERFACTORY_H
