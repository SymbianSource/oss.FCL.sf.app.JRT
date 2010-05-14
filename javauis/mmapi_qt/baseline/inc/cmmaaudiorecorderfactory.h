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
* Description:  This class is used for creating audio recorder
*
*/


#ifndef CMMAAUDIORECORDERFACTORY_H
#define CMMAAUDIORECORDERFACTORY_H

//  INCLUDES
#include "cmmammfplayerfactory.h"
#include "tmmaparametervalidator.h"

//  CLASS DECLARATION
/**
*   This class is used for creating audio recorder
*
*
*/

NONSHARABLE_CLASS(CMMAAudioRecorderFactory): public CMMAMMFPlayerFactory
{
public: // Constructors and destructor
    static CMMAAudioRecorderFactory* NewLC(TInt aMIDletSuiteID);
    ~CMMAAudioRecorderFactory();

private: // Constructor
    CMMAAudioRecorderFactory(TInt aMIDletSuiteID);

public: // From MMMAPlayerFactory
    CMMAPlayer* CreatePlayerL(const TDesC& aContentType);

    CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                              const TDesC& aMiddlePart,
                              const TDesC& aParameters);

    CMMAPlayer* CreatePlayerL(const TDesC8& aHeaderData);

    void GetSupportedProtocolsL(const TDesC& aContentType,
                                CDesC16Array& aProtocolArray);

    void GetSupportedContentTypesL(const TDesC& aProtocol,
                                   CDesC16Array& aMimeTypeArray);

public: // From CMMAMMFPlayerFactory

    CMMAPlayer* CreatePlayerL(
        CMMAMMFResolver* aResolver);


    void MediaIdsL(RArray<TUid>& aMediaIds);


    void PreparePluginSelectionParametersL(
        CMMAMMFResolver* aResolver,
        CMMFFormatSelectionParameters* aFormatSelection);

private: // Data
    CMMAAudioSettings* iSettings; // Owned settings
    TInt iMIDletSuiteID;

};

#endif // CMMAAUDIORECORDERFACTORY_H
