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
* Description:  This class is used for creating MMF-based players.
*
*/


//  INCLUDE FILES
#include <badesca.h>
#include <jdebug.h>

#ifndef RD_JAVA_OMA_DRM_V2
#include <DRMCommon.h>
#endif // RD_JAVA_OMA_DRM_V2

#include "cmmammfplayerfactory.h"
#include "cmmammfresolver.h"

// CONSTANTS
// Granularity used to create initial arrays.
const TInt KGranularity = 8;
_LIT(KContentTypePacketSrcNotIncluded, "application/x-ext-packetsrc");

EXPORT_C CMMAMMFPlayerFactory::CMMAMMFPlayerFactory()
{
}

EXPORT_C CMMAMMFPlayerFactory::~CMMAMMFPlayerFactory()
{
}

CMMAPlayer* CMMAMMFPlayerFactory::CreatePlayerL(const TDesC& aContentType)
{
    return CreatePlayerL(aContentType, NULL);
}

CMMAPlayer* CMMAMMFPlayerFactory::CreatePlayerL(const TDesC& aContentType,
        const TDesC* aFileName)
{
    DEBUG_STR("MMA::CMMAMMFPlayerFactory::CreatePlayerL content type = %S",
              aContentType);
    CMMFFormatSelectionParameters* fSelect =
        CMMFFormatSelectionParameters::NewLC();

    // MMF needs 8bit data
    HBufC8* contentType = HBufC8::NewLC(aContentType.Length());
    contentType->Des().Copy(aContentType);

    // Match to mime/content type
    fSelect->SetMatchToMimeTypeL(*contentType);

    CMMAPlayer* player = CreatePlayerL(fSelect, aFileName);

    CleanupStack::PopAndDestroy(contentType);
    CleanupStack::PopAndDestroy(fSelect);
    DEBUG("MMA::CMMAMMFPlayerFactory::CreatePlayerL content type ok");
    return player;
}

CMMAPlayer* CMMAMMFPlayerFactory::CreatePlayerL(const TDesC& aProtocol,
        const TDesC& aMiddlePart,
        const TDesC&)
{
    DEBUG_STR("MMA::CMMAMMFPlayerFactory::CreatePlayerL aMiddlePart = %S",
              aMiddlePart);
    CMMFFormatSelectionParameters* fSelect =
        CMMFFormatSelectionParameters::NewLC();

    // Match to file name, using only middle part of the locator
    fSelect->SetMatchToFileNameL(aMiddlePart);

    CMMAPlayer* player = NULL;
    if (aProtocol == KMMAFileProtocol)
    {
        DEBUG("MMA:CMMAMMFPlayerFactory::CreatePlayerL creating file player");
        player = CreatePlayerL(fSelect, &aMiddlePart);

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
        player = CreatePlayerL(fSelect, NULL);
    }

    CleanupStack::PopAndDestroy(fSelect);
    DEBUG("MMA::CMMAMMFPlayerFactory::CreatePlayerL aMiddlePart ok");
    return player;
}

#ifndef RD_JAVA_OMA_DRM_V2
CMMAPlayer* CMMAMMFPlayerFactory::TryOpenDRMFileL(const TDesC& aFileName)
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

CMMAPlayer* CMMAMMFPlayerFactory::CreatePlayerL(const TDesC8& aHeaderData)
{
    DEBUG("MMA::CMMAMMFPlayerFactory::CreatePlayerL header data +");
    CMMFFormatSelectionParameters* fSelect =
        CMMFFormatSelectionParameters::NewLC();

    // Match to header data
    fSelect->SetMatchToHeaderDataL(aHeaderData);

    CMMAPlayer* player = CreatePlayerL(fSelect);

    CleanupStack::PopAndDestroy(fSelect);
    DEBUG("MMA::CMMAMMFPlayerFactory::CreatePlayerL header data -");
    return player;
}

void CMMAMMFPlayerFactory::GetSupportedContentTypesL(const TDesC& aProtocol,
        CDesC16Array& aMimeTypeArray)
{
    // check that this is supported protocol
    if (!IsSupportedProtocolL(aProtocol))
    {
        return;
    }

    CMMFFormatSelectionParameters* fSelect =
        CMMFFormatSelectionParameters::NewLC();

    CMMAMMFResolver* cSelect =
        CMMAMMFResolver::NewLC();

    PreparePluginSelectionParametersL(cSelect, fSelect);

    // Set the media ids
    RArray<TUid> mediaIds;
    CleanupClosePushL(mediaIds);
    MediaIdsL(mediaIds);
    cSelect->SelectionParameters()->SetMediaIdsL(mediaIds, MediaIdMatchType());

    cSelect->GetSupportedContentTypesL(aMimeTypeArray);

    // Content type application/x-ext-packetsrc must not be supported,
    // thus it is removed from the list of supported content types
    TInt position(0);
    TInt err = aMimeTypeArray.Find(KContentTypePacketSrcNotIncluded, position);
    if (err == KErrNone)
    {
        aMimeTypeArray.Delete(position);
        aMimeTypeArray.Compress();
    }

    CleanupStack::PopAndDestroy(); // mediaIds
    CleanupStack::PopAndDestroy(cSelect);
    CleanupStack::PopAndDestroy(fSelect);
}

EXPORT_C CMMAPlayer* CMMAMMFPlayerFactory::CreatePlayerL(CMMFFormatSelectionParameters* aFormatSelect,
        const TDesC* aFileName)
{
    CMMAMMFResolver* cSelect =
        CMMAMMFResolver::NewLC();
    cSelect->SetFileNameL(aFileName);

    PreparePluginSelectionParametersL(cSelect,
                                      aFormatSelect);

    // Set the media ids
    RArray<TUid> mediaIds;
    CleanupClosePushL(mediaIds);
    MediaIdsL(mediaIds);
    cSelect->SelectionParameters()->SetMediaIdsL(
        mediaIds, MediaIdMatchType());

    cSelect->ListImplementationsL();
    CMMAPlayer* player = NULL;

    // check that did we get any hits
    if (cSelect->Implementations()->Count() > 0)
    {
        // Call actual factory to create player
        player = CreatePlayerL(cSelect);
    }

    CleanupStack::PopAndDestroy(); // mediaIds
    CleanupStack::PopAndDestroy(cSelect);
    return player;
}

void CMMAMMFPlayerFactory::GetSupportedProtocolsL(const TDesC& aContentType,
        CDesC16Array& aProtocolArray)
{
    // Check that this is supported content type
    if (!IsSupportedContentTypeL(aContentType))
    {
        return;
    }
    aProtocolArray.AppendL(KMMAHttpProtocol);
    aProtocolArray.AppendL(KMMAHttpsProtocol);
    aProtocolArray.AppendL(KMMAFileProtocol);
}

TBool CMMAMMFPlayerFactory::IsSupportedProtocolL(const TDesC& aProtocol)
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

TBool CMMAMMFPlayerFactory::IsSupportedContentTypeL(const TDesC& aContentType)
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

EXPORT_C void CMMAMMFPlayerFactory::PreparePluginSelectionParametersL(
    CMMAMMFResolver* aResolver,
    CMMFFormatSelectionParameters* aFormatSelection)
{
    // Play type is default
    aResolver->SetRequiredPlayFormatSupportL(*aFormatSelection);
}


CMMFPluginSelectionParameters::TMediaIdMatchType
CMMAMMFPlayerFactory::MediaIdMatchType()
{
    // We are now getting only Audio Controllers
    return CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds;
}
//  END OF FILE
