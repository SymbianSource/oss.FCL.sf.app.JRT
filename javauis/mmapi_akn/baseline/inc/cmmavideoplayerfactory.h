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


#ifndef CMMAVIDEOPLAYERFACTORY_H
#define CMMAVIDEOPLAYERFACTORY_H

//  INCLUDES
#include "cmmammfplayerfactory.h"
// CONSTANTS

//  CLASS DECLARATION
/**
*   This class is used for creating video player.
*
*
*/

NONSHARABLE_CLASS(CMMAVideoPlayerFactory): public CMMAMMFPlayerFactory
{
public: // Constructor and destructor
    static CMMAVideoPlayerFactory* NewLC();
    ~CMMAVideoPlayerFactory();

private: // Constructor
    CMMAVideoPlayerFactory();

public: // New methods

#ifdef RD_JAVA_OMA_DRM_V2
    CMMAPlayer* CreatePlayerWithFileL(const TDesC& aContentType,
                                      const TDesC* aFileName);
#endif // RD_JAVA_OMA_DRM_V2

public: // From CMMAMMFPlayerFactory

    CMMAPlayer* CreatePlayerL(
        CMMAMMFResolver* aResolver);


    void MediaIdsL(RArray<TUid>& aMediaIds);

private: //data
#ifdef RD_JAVA_OMA_DRM_V2
    const TDesC* iFileName;
#endif // RD_JAVA_OMA_DRM_V2
};

#endif // CMMAVIDEOPLAYERFACTORY_H
