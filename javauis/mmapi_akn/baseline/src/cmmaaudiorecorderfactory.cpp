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
* Description:  This class is used for creating audio recorder
*
*/


//  INCLUDE FILES
#include "cmmaaudiorecorderfactory.h"
#include "cmmaaudiorecordcontrol.h"
#include "cmmammfresolver.h"

_LIT(KCaptureProtocol, "capture");
_LIT(KAudioRecord, "audio");
_LIT(KContentTypeAudioMp4NotIncluded, "audio/mp4");

CMMAAudioRecorderFactory* CMMAAudioRecorderFactory::NewLC(TInt aMIDletSuiteID)
{
    CMMAAudioRecorderFactory* pFactory =
        new(ELeave) CMMAAudioRecorderFactory(aMIDletSuiteID);
    CleanupStack::PushL(pFactory);
    return pFactory;
}


CMMAAudioRecorderFactory::CMMAAudioRecorderFactory(TInt aMIDletSuiteID)
{
    iMIDletSuiteID = aMIDletSuiteID;
}


CMMAAudioRecorderFactory::~CMMAAudioRecorderFactory()
{
    delete iSettings;
}

CMMAPlayer* CMMAAudioRecorderFactory::CreatePlayerL(const TDesC&)
{
    // record player cannot be created from content-type, since this is used only
    // when we have also data
    return NULL;
}


CMMAPlayer* CMMAAudioRecorderFactory::CreatePlayerL(const TDesC8&)
{
    // record player cannot be created with header data
    return NULL;
}

CMMAPlayer* CMMAAudioRecorderFactory::CreatePlayerL(
    CMMAMMFResolver* aResolver)
{
    // settings ownership transferred to audiorecorder
    CMMAAudioSettings* tmpSettings = iSettings;
    iSettings = NULL;
    CMMAAudioRecorder* recorder = CMMAAudioRecorder::NewLC(aResolver,
                                  tmpSettings,
                                  iMIDletSuiteID);



    CMMAAudioRecordControl* audioRecordControl = CMMAAudioRecordControl::NewL(recorder);
    CleanupStack::PushL(audioRecordControl);
    recorder->AddControlL(audioRecordControl);
    CleanupStack::Pop(audioRecordControl);

    CleanupStack::Pop(recorder);

    return recorder;
}


void CMMAAudioRecorderFactory::MediaIdsL(RArray<TUid>& aMediaIds)
{
    CleanupClosePushL(aMediaIds);
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeAudio));
    CleanupStack::Pop();
}

void CMMAAudioRecorderFactory::PreparePluginSelectionParametersL(
    CMMAMMFResolver* aResolver,
    CMMFFormatSelectionParameters* aFormatSelection)
{
    aResolver->SetRequiredRecordFormatSupportL(*aFormatSelection);
}

CMMAPlayer* CMMAAudioRecorderFactory::CreatePlayerL(
    const TDesC& aProtocol,
    const TDesC& aMiddlePart,
    const TDesC& aParameters)
{
    // check that locator is capture:://audio
    if ((aProtocol != KCaptureProtocol) ||
            (aMiddlePart != KAudioRecord))
    {
        return NULL;
    }

    // validate properties
    CMMAAudioSettings* settings =
        TMMAParameterValidator::ValidateAudioPropertiesL(aParameters);
    delete iSettings;
    iSettings = settings;

    // Find controller with content-type
    return CMMAMMFPlayerFactory::CreatePlayerL(*iSettings->iContentType);
}

void CMMAAudioRecorderFactory::GetSupportedProtocolsL(
    const TDesC& aContentType,
    CDesC16Array& aProtocolArray)
{
    // Support for MP4 recording disabled: All features cannot
    // be supported for MP4 capture at the moment (e.g. pause)
    if (IsSupportedContentTypeL(aContentType) &&
            (aContentType != KContentTypeAudioMp4NotIncluded))
    {
        aProtocolArray.AppendL(KMMACaptureProtocol);
    }
}

void CMMAAudioRecorderFactory::GetSupportedContentTypesL(
    const TDesC& aProtocol,
    CDesC16Array& aMimeTypeArray)
{
    // protocol is empty string all types must be returned.
    if (IsSupportedProtocolL(aProtocol) ||
            (aProtocol == KNullDesC))
    {
        // get supported types from MMF
        CMMAMMFPlayerFactory::GetSupportedContentTypesL(aProtocol,
                aMimeTypeArray);

        // "audio/mp4" content type must not be supported
        // for capture protocol
        if (aProtocol == KCaptureProtocol)
        {
            TInt position(0);
            TInt err = aMimeTypeArray.Find(KContentTypeAudioMp4NotIncluded, position);
            if (err == KErrNone)
            {
                aMimeTypeArray.Delete(position);
                aMimeTypeArray.Compress();
            }
        }
    }
}

//  END OF FILE
