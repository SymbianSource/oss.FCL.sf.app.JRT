/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for creating EMC-based players.
*
*/


//  INCLUDE FILES
#include <badesca.h>
#include <logger.h>

#ifndef RD_JAVA_OMA_DRM_V2
#include <DRMCommon.h>
#endif // RD_JAVA_OMA_DRM_V2

#include "cmmaemcplayerfactory.h"
#include "cmmaemcresolver.h"
#include "MimeTypes.h"

// CONSTANTS
// Granularity used to create initial arrays.
const TInt KGranularity = 8;
_LIT(KContentTypePacketSrcNotIncluded, "application/x-ext-packetsrc");

CMMAEMCPlayerFactory::CMMAEMCPlayerFactory()
{
}

CMMAEMCPlayerFactory::~CMMAEMCPlayerFactory()
{
}

CMMAPlayer* CMMAEMCPlayerFactory::CreatePlayerL(const TDesC& aContentType)
{
    return CreatePlayerL(aContentType, NULL);
}

CMMAPlayer* CMMAEMCPlayerFactory::CreatePlayerL(const TDesC& /*aContentType*/,
        const TDesC* /*aFileName*/)
{
    return (CMMAPlayer*)NULL;
}

CMMAPlayer* CMMAEMCPlayerFactory::CreatePlayerL(const TDesC& aProtocol,
        const TDesC& aMiddlePart,
        const TDesC&)
{
    LOG1( EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerFactory::CreatePlayerL aMiddlePart = %S",
              aMiddlePart.Ptr());
    CMMAEMCResolver* emcresolver =
        CMMAEMCResolver::NewLC();

    emcresolver ->SetFileNameL(&aMiddlePart);

    CMMAPlayer* player = NULL;
    if (aProtocol == KMMAFileProtocol)
    {

        //player = CreatePlayerL( emcresolver, &aMiddlePart );

#ifndef RD_JAVA_OMA_DRM_V2
        // if opening is failed, it might be DRM file, trying it
        if (!player)
        {
            player = TryOpenDRMFileL(aMiddlePart);
        }
#endif // RD_JAVA_OMA_DRM_V2

    }
    else
    {
        player = CreatePlayerL(emcresolver);
    }

    CleanupStack::PopAndDestroy(emcresolver);
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerFactory::CreatePlayerL aMiddlePart ok");
    return player;
}

#ifndef RD_JAVA_OMA_DRM_V2
CMMAPlayer* CMMAEMCPlayerFactory::TryOpenDRMFileL(const TDesC& aFileName)
{
    // we are most likely going to play this file
    ContentAccess::TIntent intent = ContentAccess::EPlay;

    CContent* contentObj = CContent::NewL(aFileName);
    CleanupStack::PushL(contentObj);
    CData* dataObj = contentObj->OpenContentL(intent);
    CleanupStack::PushL(dataObj);
    User::LeaveIfError(dataObj->EvaluateIntent(intent));
    TBuf8<KMaxName> mimeType;
    CMMAPlayer* player = NULL;
    if (dataObj->GetMimeTypeL(mimeType))
    {
        // we use 16bit mimeType
        HBufC* mimeTypeBuf = HBufC::NewLC(mimeType.Length());
        mimeTypeBuf->Des().Copy(mimeType);
        player = CreatePlayerL(*mimeTypeBuf, &aFileName);
        CleanupStack::PopAndDestroy(mimeTypeBuf);
    }
    CleanupStack::PopAndDestroy(2); //dataObj, contentObj
    return player;
}
#endif // RD_JAVA_OMA_DRM_V2

CMMAPlayer* CMMAEMCPlayerFactory::CreatePlayerL(const TDesC8& /*aHeaderData*/)
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerFactory::CreatePlayerL header data +");

    return (CMMAPlayer*)NULL;
}

void CMMAEMCPlayerFactory::GetSupportedContentTypesL(const TDesC& aProtocol,
        CDesC16Array& aMimeTypeArray)
{
    //CMMAEMCResolver class should contain the constant array of all supported mimetype defined in EMC api MimeTypes.h
    if (!IsSupportedProtocolL(aProtocol))
    {
        return;
    }
    CMMAEMCResolver* emcresolver = CMMAEMCResolver::NewLC();
    emcresolver->GetSupportedContentTypesL(aMimeTypeArray);

    CleanupStack::PopAndDestroy(emcresolver);
}

void CMMAEMCPlayerFactory::GetSupportedProtocolsL(const TDesC& aContentType,
        CDesC16Array& aProtocolArray)
{
    // Check that this is supported content type
    if (!IsSupportedContentTypeL(aContentType))
    {
        return;
    }
    aProtocolArray.AppendL(KMMAHttpProtocol);
    aProtocolArray.AppendL(KMMAHttpsProtocol);
}

TBool CMMAEMCPlayerFactory::IsSupportedProtocolL(const TDesC& aProtocol)
{
    // With null desc we are getting all
    if (aProtocol == KNullDesC)
    {
        return ETrue;
    }
    CDesC16ArraySeg* protocols = new(ELeave) CDesC16ArraySeg(KGranularity);
    CleanupStack::PushL(protocols);
    GetSupportedProtocolsL(KNullDesC, *protocols);
    TInt pos = KErrNotFound;
    // Find returns 0 if there contentType is found
    TBool retValue = (protocols->Find(aProtocol, pos) == 0);
    CleanupStack::PopAndDestroy(protocols);
    return retValue;
}

TBool CMMAEMCPlayerFactory::IsSupportedContentTypeL(const TDesC& aContentType)
{
    // With null desc we are getting all
    if (aContentType == KNullDesC)
    {
        return ETrue;
    }

    // Content type application/x-ext-packetsrc
    // must not be supported at the moment.
    if (aContentType == KContentTypePacketSrcNotIncluded)
    {
        return EFalse;
    }

    CDesC16ArraySeg* contentTypes = new(ELeave) CDesC16ArraySeg(KGranularity);
    CleanupStack::PushL(contentTypes);
    GetSupportedContentTypesL(KNullDesC, *contentTypes);
    TInt pos = KErrNotFound;
    // Find returns 0 if there contentType is found
    TBool retValue = (contentTypes->Find(aContentType, pos) == 0);
    CleanupStack::PopAndDestroy(contentTypes);
    return retValue;
}

//  END OF FILE
