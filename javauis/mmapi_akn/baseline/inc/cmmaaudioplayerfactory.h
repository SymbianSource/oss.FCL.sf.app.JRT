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
* Description:  This class is used for creating wav player.
*
*/


#ifndef CMMAAUDIOPLAYERFACTORY_H
#define CMMAAUDIOPLAYERFACTORY_H

//  INCLUDES
#include "cmmammfplayerfactory.h"

//  CLASS DECLARATION
/**
*   This class is used for creating wav player.
*
*
*/

NONSHARABLE_CLASS(CMMAAudioPlayerFactory): public CMMAMMFPlayerFactory
{
public: // Constructor and destructor
    static CMMAAudioPlayerFactory* NewLC();
    ~CMMAAudioPlayerFactory();

private: // Constructor
    CMMAAudioPlayerFactory();

public: // From CMMAMMFPlayerFactory
    CMMAPlayer* CreatePlayerL(const TDesC& aContentType);
    CMMAPlayer* CreatePlayerL(
        CMMAMMFResolver* aResolver);
    void GetSupportedContentTypesL(const TDesC& aProtocol,
                                   CDesC16Array& aMimeTypeArray);
    void MediaIdsL(RArray<TUid>& aMediaIds);

};

#endif // CMMAAUDIOPLAYERFACTORY_H
