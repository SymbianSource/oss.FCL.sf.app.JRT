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


//  INCLUDE FILES
#include <logger.h>
#include <ecam.h>

#include "cmmamanager.h"
#include "cmmaaudioplayerfactory.h"
#include "cmmavideoplayerfactory.h"
#include "cmmaaudiorecorderfactory.h"
#include "cmmacameraplayerfactory.h"
#include "cmmamidiplayerfactory.h"
#include "cmmavideourlplayerfactory.h"
#include "cmmaaudiostreamplayerfactory.h"
//#include "cmmaanimationplayerfactory.h"

#ifdef RD_JAVA_OMA_DRM_V2
#include "cmmadrmplayerfactory.h"
#endif // RD_JAVA_OMA_DRM_V2

// index of CMMAAudioStreamPlayerFactory in iPlayerFactories
// if CMMAAudioStreamPlayerFactory is moved in iPlayerFactories
// this constant should be updated accordingly
const TInt KAudioStreamPlayerFactoryIndex = 2;

CMMAManager::~CMMAManager()
{
    iPlayerFactories.ResetAndDestroy();
    iPlayerFactories.Close();
}

CMMAManager::CMMAManager()
{
}

void CMMAManager::ConstructL(TInt aMIDletSuiteID)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAManager::ConstructL +");
    //
    // Create and insert known player factories
    //

    CMMAVideoUrlPlayerFactory* videoUrlPlayerFactory = CMMAVideoUrlPlayerFactory::NewLC();
    AddPlayerFactoryL(videoUrlPlayerFactory);
    CleanupStack::Pop(); // videoUrlPlayerFactory

    CMMAMIDIPlayerFactory* midiPlayerFactory = CMMAMIDIPlayerFactory::NewLC();
    AddPlayerFactoryL(midiPlayerFactory);
    CleanupStack::Pop(); // midiPlayerFactory

    CMMAAudioStreamPlayerFactory* audioStreamPlayerFactory =
        CMMAAudioStreamPlayerFactory::NewLC();
    AddPlayerFactoryL(audioStreamPlayerFactory);
    CleanupStack::Pop(); // audioStreamPlayerFactory

    CMMAVideoPlayerFactory* videoPlayerFactory = CMMAVideoPlayerFactory::NewLC();
    AddPlayerFactoryL(videoPlayerFactory);
    CleanupStack::Pop(); // videoPlayerFactory

    CMMAAudioPlayerFactory* audioPlayerFactory = CMMAAudioPlayerFactory::NewLC();
    AddPlayerFactoryL(audioPlayerFactory);
    CleanupStack::Pop(); // audioPlayerFactory

    CMMAAudioRecorderFactory* audioRecorderFactory =
        CMMAAudioRecorderFactory::NewLC(aMIDletSuiteID);
    AddPlayerFactoryL(audioRecorderFactory);
    CleanupStack::Pop(); // audioRecorderFactory

    /*
        CMMAAnimationPlayerFactory* animationPlayerFactory =
            CMMAAnimationPlayerFactory::NewLC();
        AddPlayerFactoryL(animationPlayerFactory);
        CleanupStack::Pop(); // animationPlayerFactory
    */

    // Add camera playerfactory only if there is a camera
    if (CCamera::CamerasAvailable() > 0)
    {
        CMMACameraPlayerFactory* cameraPlayerFactory =
            CMMACameraPlayerFactory::NewLC();
        AddPlayerFactoryL(cameraPlayerFactory);
        CleanupStack::Pop(); // cameraPlayerFactory
    }

#ifdef RD_JAVA_OMA_DRM_V2
    CMMADRMPlayerFactory* drmPlayerFactory =
        CMMADRMPlayerFactory::NewLC(videoPlayerFactory);
    AddPlayerFactoryL(drmPlayerFactory);
    CleanupStack::Pop(); // drmPlayerFactory
#endif // RD_JAVA_OMA_DRM_V2

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAManager::ConstructL -");
}

void CMMAManager::StaticCreateManagerL(CMMAManager** aManager,
                                       TInt aMIDletSuiteID)
{
    CMMAManager* self = new(ELeave) CMMAManager();

    CleanupStack::PushL(self);
    self->ConstructL(aMIDletSuiteID);
    CleanupStack::Pop();

    *aManager = self;
}

/**
 * Use factories to create a player from content type
 *
 */
CMMAPlayer* CMMAManager::CreatePlayerL(const TDesC& aContentType)
{
    // Try all factories, in order
    TInt factoryCount = iPlayerFactories.Count();
    for (TInt i = 0; i < factoryCount; i++)
    {
        CMMAPlayer* player = iPlayerFactories[ i ]->CreatePlayerL(aContentType);
        if (player)
        {
            return player;
        }
    }
    // No PlayerFactory accepted the content type
    return NULL;
}

/**
 * Use factories to create a player from locator
 *
 */
CMMAPlayer* CMMAManager::CreatePlayerL(const TDesC& aProtocol,
                                       const TDesC& aMiddlePart,
                                       const TDesC& aParameters)
{
    // Try all factories, in order
    TInt factoryCount = iPlayerFactories.Count();
    for (TInt i = 0; i < factoryCount; i++)
    {
        CMMAPlayer* player = iPlayerFactories[ i ]->CreatePlayerL(aProtocol,
                             aMiddlePart,
                             aParameters);
        if (player)
        {
            return player;
        }
    }
    // No PlayerFactory accepted the content type
    return NULL;
}

/**
 * Use factories to create a player from header data
 *
 */
CMMAPlayer* CMMAManager::CreatePlayerL(const TDesC8& aHeaderData)
{
    // Try all factories, in order
    TInt factoryCount = iPlayerFactories.Count();
    for (TInt i = 0; i < factoryCount; i++)
    {
        CMMAPlayer* player = iPlayerFactories[ i ]->CreatePlayerL(aHeaderData);
        if (player)
        {
            return player;
        }
    }
    // No PlayerFactory accepted the content type
    return NULL;
}

/**
 * From MMMAPlayerFactory. Get all supported protocols
 */
void CMMAManager::GetSupportedProtocolsL(
    const TDesC& aContentType,
    CDesC16Array& aSupportedProtocols)
{
    // Query supported protocols from all player factories
    for (TInt i = 0; i < iPlayerFactories.Count(); i++)
    {
        iPlayerFactories[ i ]->GetSupportedProtocolsL(aContentType,
                aSupportedProtocols);
    }
}

/**
 * From MMMAPlayerFactory. Get all supported content types
 */
void CMMAManager::GetSupportedContentTypesL(
    const TDesC& aProtocol,
    CDesC16Array& aContentTypes)
{
    // Get all supported content types from PlayerFactories
    for (TInt i = 0; i < iPlayerFactories.Count(); i++)
    {
        iPlayerFactories[ i ]->GetSupportedContentTypesL(aProtocol,
                aContentTypes);
    }
}

EXPORT_C void CMMAManager::AddPlayerFactoryL(MMMAPlayerFactory* aPlayerFactory)
{
    User::LeaveIfError(iPlayerFactories.Append(aPlayerFactory));
}

void CMMAManager::SetSourceInfoL(const TUint8* aHeader, TInt aLength)
{

    if (iPlayerFactories.Count() > KAudioStreamPlayerFactoryIndex)
    {
        // assumption - iPlayerFactories[KAudioStreamPlayerFactoryIndex] is CMMAAudioStreamPlayerFactory
        CMMAAudioStreamPlayerFactory* audiostreamplayerfactory =
            static_cast<CMMAAudioStreamPlayerFactory*>(iPlayerFactories[ KAudioStreamPlayerFactoryIndex ]);
        audiostreamplayerfactory->SetSourceInfoL(aHeader, aLength);
    }
}

//  END OF FILE
