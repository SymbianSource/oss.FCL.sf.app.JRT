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
* Description:  This class is used for creating stream player.
*
*/


#ifndef CMMAAUDIOSTREAMPLAYERFACTORY_H
#define CMMAAUDIOSTREAMPLAYERFACTORY_H

//  INCLUDES
#include "CMMAMMFPlayerFactory.h"

//  CLASS DECLARATION
/**
*   This class is used for creating stream player.
*
*
*/
NONSHARABLE_CLASS(CMMAAudioStreamPlayerFactory): public CMMAMMFPlayerFactory
{
public: // Constructor and destructor
    static CMMAAudioStreamPlayerFactory* NewLC();
    ~CMMAAudioStreamPlayerFactory();

private: // Constructor
    CMMAAudioStreamPlayerFactory();

public: // From CMMAMMFPlayerFactory
    CMMAPlayer* CreatePlayerL(const TDesC& /*aContentType*/);
    CMMAPlayer* CreatePlayerL(const TDesC8& /*aHeaderData*/);
    CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                              const TDesC& aMiddlePart,
                              const TDesC& aParameters);
    CMMAPlayer* CreatePlayerL(
        CMMAMMFResolver* aResolver);
    void GetSupportedContentTypesL(const TDesC& aProtocol,
                                   CDesC16Array& aMimeTypeArray);
    void MediaIdsL(RArray<TUid>& aMediaIds);



};

#endif // CMMAAUDIOSTREAMPLAYERFACTORY_H
