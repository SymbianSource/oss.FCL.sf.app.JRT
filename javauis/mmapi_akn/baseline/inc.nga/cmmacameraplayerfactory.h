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
* Description:  This class is used for creating camera players.
*
*/


#ifndef CMMACAMERAPLAYERFACTORY_H
#define CMMACAMERAPLAYERFACTORY_H

//  INCLUDES
#include <e32base.h>
#include "cmmammfplayerfactory.h"
#include "tmmaparametervalidator.h"
#include "cmmacameraplayer.h"

//  CLASS DECLARATION
/**
*   This class is used for creating camera players.
*
*
*/

NONSHARABLE_CLASS(CMMACameraPlayerFactory): public CMMAMMFPlayerFactory
{
public: // Construction
    static CMMACameraPlayerFactory* NewLC();
    ~CMMACameraPlayerFactory();

private:
    CMMACameraPlayerFactory();

public: // From MMMAPlayerFactory
    CMMAPlayer* CreatePlayerL(const TDesC& aContentType);

    CMMAPlayer* CreatePlayerL(const TDesC8& aHeaderData);

    CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                              const TDesC& aMiddlePart,
                              const TDesC& aParameters);

    void GetSupportedProtocolsL(const TDesC& aContentType,
                                CDesC16Array& aProtocolArray);

public: // From CMMAMMFPlayerFactory

    CMMAPlayer* CreatePlayerL(
        CMMAMMFResolver* aResolver);


    void MediaIdsL(RArray<TUid>& aMediaIds);

    void PreparePluginSelectionParametersL(
        CMMAMMFResolver* aResolver,
        CMMFFormatSelectionParameters* aFormatSelection);

private: // Data
    CMMAAudioSettings* iAudioSettings; // Owned audio settings
    TMMAVideoSettings iVideoSettings; // Video settings
    TCamActiveCamera iCameraIndex;
};

#endif // CMMACAMERAPLAYERFACTORY_H
