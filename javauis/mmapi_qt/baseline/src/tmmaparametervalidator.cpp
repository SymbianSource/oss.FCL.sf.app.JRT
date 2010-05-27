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
* Description:  This class is used for storing and parsing properties
*
*/


//  INCLUDE FILES
#include <logger.h>
#include <mmfformatimplementationuids.hrh>
#include <mmfcontrollerimplementationuids.hrh>
#include "cmmaplayerproperties.h"
#include <mmfcontrollerpluginresolver.h>


#include "tmmaparametervalidator.h"

// CONSTANTS

// AUDIO PARAMETERS
_LIT(KPropertyEncoding,     "encoding");
_LIT(KPropertyEncodingPcm,  "pcm");     // Wav
_LIT(KPropertyEncodingAmr,  "amr");     // Amr
_LIT(KPropertyEncodingGsm,  "gsm");
_LIT(KPropertyEncodingUlaw, "ulaw");    // Au
_LIT(KPropertyEncodingAlaw, "alaw");
_LIT(KPropertyEncodingRaw,  "raw");     // Raw pcm
_LIT(KPropertyEncodingNone,  "");       // empty string

_LIT(KContentTypeWav, "audio/wav");
_LIT(KContentTypeAu,  "audio/au");
_LIT(KContentTypeAmr, "audio/amr");
_LIT(KContentTypeRaw, "audio/raw");

// Format used if not specified in locator
_LIT(KDefaultAudioRecordContentType, "audio/wav");

_LIT(KPropertyRate,         "rate");
_LIT(KPropertyBits,         "bits");
_LIT(KPropertyChannels,     "channels");

_LIT(KPropertyEndian,       "endian");
MMA_PARAMETER_STR(KPropertyEndianLittle, "little");
MMA_PARAMETER_STR(KPropertyEndianBig,    "big");
MMA_PARAMETER_ARRAY(KValidEndianValues)
{
    {
        &KPropertyEndianLittle
    }, {&KPropertyEndianBig}
};

_LIT(KPropertySigned,       "signed");
MMA_PARAMETER_STR(KPropertySignedSigned, "signed");
MMA_PARAMETER_STR(KPropertySignedUnsigned, "unsigned");
MMA_PARAMETER_ARRAY(KValidSignedValues)
{
    {
        &KPropertySignedSigned
    }, {&KPropertySignedUnsigned}
};

// VIDEO PARAMETERS
_LIT(KPropertyFps,         "fps");
_LIT(KPropertyWidth,       "width");
_LIT(KPropertyHeight,      "height");

const TInt KSizeNotSet = -1;

// Format used if not specified in locator
_LIT(KDefaultVideoRecordContentType, "video/3gpp");

//IMAGE PARAMETERS
_LIT(KPropertyType,             "type");
MMA_PARAMETER_STR(KPropertyJfif,  "jfif");
MMA_PARAMETER_STR(KPropertyExif,  "exif");
MMA_PARAMETER_ARRAY(KValidTypes)
{
    {
        &KPropertyJfif
    }, {&KPropertyExif}
};

_LIT(KFormatPng, "png");
_LIT(KFormatJpg, "jpeg");   // not "jpg"
_LIT(KFormatGif, "gif");
_LIT(KFormatBmp, "bmp");
_LIT(KContentTypePng, "image/png");
_LIT(KContentTypeJpg, "image/jpeg");
_LIT(KContentTypeJpg2,"image/jpg");
_LIT(KContentTypeGif, "image/gif");
_LIT(KContentTypeBmp, "image/bmp");

_LIT(KPropertyColor, "colors");
MMA_PARAMETER_STR(KPropertyRGB888,  "rgb888");
MMA_PARAMETER_STR(KPropertyRGB444,  "rgb444");
MMA_PARAMETER_STR(KPropertyGray16,  "gray16");
MMA_PARAMETER_STR(KPropertyGray8,   "gray8");

_LIT(KPropertyQuality, "quality");
const TInt KDefaultQuality = 75; // digged out from ICL source
const TInt KDefaultBpp = 24;
const TInt KDefaultBpp444 = 12;
const TInt KDefaultBppGray16 = 16;
const TInt KDefaultBppGray8 = 8;
const TInt KNoUserDefinedQuality = -1;
const TReal32 KNoUserDefinedFps = -1;

// CMMAImageSettings container destructor
CMMAImageSettings::~CMMAImageSettings()
{
    delete iMimeType;
    delete iType;
    delete iImageData;
}

CMMAAudioSettings::~CMMAAudioSettings()
{
    delete iContentType;
}

CMMAAudioSettings *TMMAParameterValidator::ValidateAudioPropertiesL(const TDesC& aProperties)
{
    CMMAAudioSettings* settings = new(ELeave) CMMAAudioSettings;
    CleanupStack::PushL(settings);

    CMMAParameterRuleSet* rules = CMMAParameterRuleSet::NewLC();

    // "bits" must be between 0 - 24
    TMMAParameterRuleInt bitsRule(KPropertyBits, 0, 24);
    rules->AppendRuleL(&bitsRule);

    // "rate" must be positive
    TMMAParameterRuleInt rateRule(KPropertyRate, 1);
    rules->AppendRuleL(&rateRule);

    // "channels" must be 1 or 2
    TMMAParameterRuleInt channelsRule(KPropertyChannels, 1, 2);
    rules->AppendRuleL(&channelsRule);

    // "signed" must be "signed" or "unsigned"
    TMMAParameterRuleDes signedRule(KPropertySigned, KValidSignedValues,
                                    MMA_PARAMETER_ARRAY_SIZE(KValidSignedValues));
    rules->AppendRuleL(&signedRule);

    // "endian" must be "little" or "big"
    TMMAParameterRuleDes endianRule(KPropertyEndian, KValidEndianValues,
                                    MMA_PARAMETER_ARRAY_SIZE(KValidEndianValues));
    rules->AppendRuleL(&endianRule);

    // "encoding" must be one of valid values
    TMMAParameterRuleDes encodingRule(KPropertyEncoding);
    rules->AppendRuleL(&encodingRule);

    CMMAPlayerProperties* properties = CMMAPlayerProperties::NewL(aProperties, *rules);
    CleanupStack::PushL(properties);

    // validating properties
    properties->ValidateL();

    TInt bits = KDefaultBits;
    properties->GetProperty(KPropertyBits, bits);

    settings->iRate = KDefaultRate;
    properties->GetProperty(KPropertyRate, settings->iRate);

    settings->iChannels = KDefaultChannels;
    properties->GetProperty(KPropertyChannels, settings->iChannels);


    // Normally encoders are not streamable
    settings->iStreamableFormat = EFalse;

    // signed=signed/unsigned (default is unsigned for 8bit, signed for 16bit)
    TBool dataSigned = EFalse;
    if (properties->Compare(KPropertyBits, 16))
    {
        dataSigned = ETrue;
    }

    if (properties->Compare(KPropertySigned, KPropertySignedSigned()))
    {
        dataSigned = ETrue;
    }
    else if (properties->Compare(KPropertySigned, KPropertySignedUnsigned()))
    {
        dataSigned = EFalse;
    }

    // little by default
    TBool endianLittle = ETrue;
    if (properties->Compare(KPropertyEndian, KPropertyEndianBig()))
    {
        endianLittle = EFalse;
    }

    settings->iDataType = KMMFFourCCCodeNULL;

    // encoding specific

    TPtrC encoding(NULL, 0);
    properties->GetProperty(KPropertyEncoding, encoding);

    if ((encoding == KPropertyEncodingAmr()) ||
            (encoding == KPropertyEncodingGsm()))
    {
        // AMR format
        encoding.Set(KContentTypeAmr());
    }

    else if (encoding == KPropertyEncodingUlaw())
    {
        // AU format, ulaw codec
        encoding.Set(KContentTypeAu());
        settings->iDataType = KMMFFourCCCodeMuLAW;
    }

    else if (encoding == KPropertyEncodingAlaw())
    {
        // AU format, alaw codec
        encoding.Set(KContentTypeAu());
        settings->iDataType = KMMFFourCCCodeALAW;
    }

    else if (encoding == KPropertyEncodingPcm())
    {
        // wav format
        encoding.Set(KContentTypeWav());
    }

    else if (encoding == KPropertyEncodingRaw())
    {
        // raw format, default codec
        encoding.Set(KContentTypeRaw());
    }

    else if ((encoding == KPropertyEncodingNone()))     // this is the default case
    {
        encoding.Set(KDefaultAudioRecordContentType());
    }

    settings->iContentType = encoding.AllocL();


    // wav specific
    if (*settings->iContentType == KContentTypeWav())
    {
        if (bits == 8)
        {
            settings->iDataType = KMMFFourCCCodePCMU8;
        }

        else if (bits == 16)
        {
            settings->iDataType = KMMFFourCCCodePCM16;
        }

        else if (bits != KDefaultBits)
        {
            User::Leave(KErrNotSupported);
        }
    }

    // raw specific
    if (*settings->iContentType == KContentTypeRaw())
    {
        if (bits == 8)
        {
            if (dataSigned)
            {
                settings->iDataType = KMMFFourCCCodePCM8;
            }
            else
            {
                settings->iDataType = KMMFFourCCCodePCMU8;
            }
        }

        else if (bits == 16)
        {
            if (endianLittle)
            {
                if (dataSigned)
                {
                    settings->iDataType = KMMFFourCCCodePCM16;
                }
                else
                {
                    settings->iDataType = KMMFFourCCCodePCMU16;
                }
            }
            else // Big endian
            {
                if (dataSigned)
                {
                    settings->iDataType = KMMFFourCCCodePCM16B;
                }
                else
                {
                    settings->iDataType = KMMFFourCCCodePCMU16B;
                }
            }
        }

        else if (bits != KDefaultBits) // bits not 8 or 16
        {
            User::Leave(KErrNotSupported);
        }

        settings->iEncoding = TUid::Uid(KMmfUidFormatRAWWrite);
        settings->iController = TUid::Uid(KMmfUidControllerAudio);
    }


    CleanupStack::PopAndDestroy(2);   // rules, properties
    CleanupStack::Pop(settings);
    return settings;
}

TMMAVideoSettings TMMAParameterValidator::ValidateVideoPropertiesL(const TDesC& aProperties)
{
    CheckEncodingL(aProperties);
    TMMAVideoSettings settings;
    CMMAParameterRuleSet* rules = CMMAParameterRuleSet::NewLC();

    // "fps" is real, taking it as string and validating later
    TMMAParameterRuleDes fpsRule(KPropertyFps);
    rules->AppendRuleL(&fpsRule);

    // "width" must be positive
    TMMAParameterRuleInt widthRule(KPropertyWidth, 1);
    rules->AppendRuleL(&widthRule);

    // "height" must be positive
    TMMAParameterRuleInt heightRule(KPropertyHeight, 1);
    rules->AppendRuleL(&heightRule);

    // "encoding" can be anything in this point (actual validation will be done by MMF)
    TMMAParameterRuleDes encodingRule(KPropertyEncoding);
    rules->AppendRuleL(&encodingRule);

    CMMAPlayerProperties* properties = CMMAPlayerProperties::NewL(aProperties, *rules);
    CleanupStack::PushL(properties);

    // validating properties
    properties->ValidateL();

    properties->GetProperty(KPropertyWidth, settings.iWidth = KSizeNotSet);
    properties->GetProperty(KPropertyHeight, settings.iHeight = KSizeNotSet);

    TPtrC fps;
    settings.iFps = KNoUserDefinedFps;
    if (properties->GetProperty(KPropertyFps, fps))
    {
        TLex lex(fps);
        TReal32 fpsReal;
        // try to get fps value from string
        TInt err = lex.Val(fpsReal);
        if (err == KErrNone)
        {
            // Check if there other characters after
            // number ( 34xxx )
            if (!lex.Eos() ||
                    (fpsReal <= 0))
            {
                User::Leave(KErrArgument);
            }
            else
            {
                settings.iFps = fpsReal;
            }
        }
        else if (err == KErrNoMemory)
        {
            User::Leave(KErrNoMemory);
        }
        else
        {
            // value is not available
            User::Leave(KErrArgument);
        }
    }

    settings.iEncoding.Set(KDefaultVideoRecordContentType());
    properties->GetProperty(KPropertyEncoding, settings.iEncoding);

    CleanupStack::PopAndDestroy(2);   // rules, properties
    return settings;
}

CMMAImageSettings* TMMAParameterValidator::ValidateImagePropertiesL(const TDesC& aProperties)
{
    CMMAImageSettings* settings = new(ELeave)CMMAImageSettings;
    CleanupStack::PushL(settings);

    CMMAParameterRuleSet* rules = CMMAParameterRuleSet::NewLC();

    // "width" must be positive
    TMMAParameterRuleInt widthRule(KPropertyWidth, 1);
    rules->AppendRuleL(&widthRule);

    // "height" must be positive
    TMMAParameterRuleInt heightRule(KPropertyHeight, 1);
    rules->AppendRuleL(&heightRule);

    // "encoding" final validation is done with image encoder
    TMMAParameterRuleDes encodingRule(KPropertyEncoding);
    rules->AppendRuleL(&encodingRule);

    // "color" can be anything in this point, it will be validated later
    TMMAParameterRuleDes colorRule(KPropertyColor);
    rules->AppendRuleL(&colorRule);

    // "type" must be "jfif" or "exif"
    TMMAParameterRuleDes typeRule(KPropertyType, KValidTypes,
                                  MMA_PARAMETER_ARRAY_SIZE(KValidTypes));
    rules->AppendRuleL(&typeRule);

    // "quality" must be between 0 and 100
    TMMAParameterRuleInt qualityRule(KPropertyQuality, 0, 100);
    rules->AppendRuleL(&qualityRule);

    CMMAPlayerProperties* properties = CMMAPlayerProperties::NewL(aProperties, *rules);
    CleanupStack::PushL(properties);

    // validating properties
    properties->ValidateL();

    properties->GetProperty(KPropertyWidth, settings->iWidth = KSizeNotSet);
    properties->GetProperty(KPropertyHeight, settings->iHeight = KSizeNotSet);

    // do not set quality if user has not defined it
    TInt quality = KNoUserDefinedQuality;
    properties->GetProperty(KPropertyQuality, quality);

    TPtrC encoding;
    if (properties->Compare(KPropertyEncoding, KFormatPng) ||
            properties->Compare(KPropertyEncoding, KContentTypePng))
    {
        //PNG
        settings->iMimeType = CreateHBufC8FromUnicodeL(KContentTypePng);
        TPngEncodeData* imageData = new(ELeave)TPngEncodeData();
        imageData->iBitsPerPixel = KDefaultBpp;
        imageData->iColor = ETrue;
        imageData->iPaletted = EFalse;
        imageData->iLevel = TPngEncodeData::EDefaultCompression;

        CleanupStack::PushL(imageData);
        TPtrC color;
        if (properties->Compare(KPropertyColor, KPropertyRGB888()))
        {
            // default
        }
        else if (properties->Compare(KPropertyColor, KPropertyRGB444()))
        {
            imageData->iBitsPerPixel = KDefaultBpp444;
        }
        else if (properties->Compare(KPropertyColor, KPropertyGray16()))
        {
            imageData->iBitsPerPixel = KDefaultBppGray16;
            imageData->iColor = EFalse;
        }
        else if (properties->Compare(KPropertyColor, KPropertyGray8()))
        {
            imageData->iBitsPerPixel = KDefaultBppGray8;
            imageData->iColor = EFalse;
        }
        else if (properties->GetProperty(KPropertyColor, color))
        {
            // not valid color parameter
            User::Leave(KErrArgument);
        }
        settings->iImageData = CFrameImageData::NewL();
        User::LeaveIfError(settings->iImageData->AppendFrameData(imageData));
        // CFrameImageData took ownership of imageData
        CleanupStack::Pop(imageData);

        // png quality setting not supported, "type" is only for jpeg.
        TPtrC tmp;
        if (quality != KNoUserDefinedQuality ||
                properties->GetProperty(KPropertyType, tmp)
           )
        {
            User::Leave(KErrArgument);
        }
    }
    else if (properties->Compare(KPropertyEncoding, KFormatJpg) ||
             properties->Compare(KPropertyEncoding, KContentTypeJpg) ||
             properties->Compare(KPropertyEncoding, KContentTypeJpg2))
    {
        // JPG
        settings->iMimeType = CreateHBufC8FromUnicodeL(KContentTypeJpg);
        if (properties->Compare(KPropertyType, KPropertyJfif()))
        {
            settings->iType = CreateHBufC8FromUnicodeL(KPropertyJfif());
        }
        else if (properties->Compare(KPropertyType, KPropertyExif()))
        {
            settings->iType = CreateHBufC8FromUnicodeL(KPropertyExif());
        }
        else
        {
            settings->iType = CreateHBufC8FromUnicodeL(KPropertyExif());
        }

        TJpegImageData* imageData = new(ELeave)TJpegImageData();
        CleanupStack::PushL(imageData);
        imageData->iQualityFactor = KDefaultQuality;
        imageData->iSampleScheme = TJpegImageData::EColor444;
        if (quality != KNoUserDefinedQuality)
        {
            imageData->iQualityFactor = quality;
        }
        settings->iImageData = CFrameImageData::NewL();
        User::LeaveIfError(settings->iImageData->AppendImageData(imageData));
        // CFrameImageData took ownership of imageData
        CleanupStack::Pop(imageData);

        TPtrC tmp;
        // jpg encoders colors cannot be configured,
        // notice that if there was "type" defined it must be "jfif"
        // and encoder will check that anyway.
        if (properties->GetProperty(KPropertyColor, tmp))
        {
            User::Leave(KErrArgument);
        }
    }
    else if (properties->Compare(KPropertyEncoding, KFormatGif) ||
             properties->Compare(KPropertyEncoding, KContentTypeGif))
    {
        //GIF
        settings->iMimeType = CreateHBufC8FromUnicodeL(KContentTypeGif);
        TPtrC tmp;
        // gif encoder is not configurable
        if (properties->GetProperty(KPropertyColor, tmp) ||
                properties->GetProperty(KPropertyType, tmp)  ||
                quality != KNoUserDefinedQuality)
        {
            User::Leave(KErrArgument);
        }
    }
    else if (properties->Compare(KPropertyEncoding, KFormatBmp) ||
             properties->Compare(KPropertyEncoding, KContentTypeBmp))
    {
        //BMP
        settings->iMimeType = CreateHBufC8FromUnicodeL(KContentTypeBmp);
        TBmpImageData* imageData = new(ELeave)TBmpImageData();
        CleanupStack::PushL(imageData);
        imageData->iBitsPerPixel = KDefaultBpp;
        TPtrC color;
        if (properties->Compare(KPropertyColor, KPropertyRGB888()))
        {
            // default
        }
        else if (properties->Compare(KPropertyColor, KPropertyRGB444()))
        {
            imageData->iBitsPerPixel = KDefaultBpp444;
        }
        else if (properties->GetProperty(KPropertyColor, color))
        {
            // not valid color parameter
            User::Leave(KErrArgument);
        }

        settings->iImageData = CFrameImageData::NewL();
        User::LeaveIfError(settings->iImageData->AppendImageData(imageData));
        // CFrameImageData took ownership of imageData
        CleanupStack::Pop(imageData);

        //  quality not supported, "type" is only for jpg
        TPtrC tmp;
        if (quality != KNoUserDefinedQuality ||
                properties->GetProperty(KPropertyType, tmp))
        {
            User::Leave(KErrArgument);
        }
    }
    else if (properties->GetProperty(KPropertyEncoding, encoding))
    {
        // OTHER POSSIBLE TYPE
        // trying to create encoder with only content/mime-type
        settings->iMimeType = CreateHBufC8FromUnicodeL(encoding);

        // No extra parameters, only content-type
        TPtrC tmp;
        if (properties->GetProperty(KPropertyColor, tmp) ||
                properties->GetProperty(KPropertyType, tmp) ||
                quality != KNoUserDefinedQuality)
        {
            User::Leave(KErrArgument);
        }
    }
    else // default: we had empty properties string
    {
        settings->iMimeType = CreateHBufC8FromUnicodeL(KContentTypePng);
    }
    CleanupStack::PopAndDestroy(properties);
    CleanupStack::PopAndDestroy(rules);
    CleanupStack::Pop(settings);
    return settings;
}

void TMMAParameterValidator::CheckEncodingL(const TDesC& aProperties)
{
    TInt encodingPos = aProperties.Find(KPropertyEncoding);
    // empty properties is valid and
    // encoding must be first parameter and must exist every time
    if ((aProperties.Length() > 0) &&
            ((encodingPos == KErrNotFound) ||
             (encodingPos != 0)))
    {
        User::Leave(KErrArgument);
    }
}

HBufC8* TMMAParameterValidator::CreateHBufC8FromUnicodeL(const TDesC& aDes)
{
    HBufC8* tmp = HBufC8::NewL(aDes.Length());
    tmp->Des().Copy(aDes);
    return tmp;
}

//  END OF FILE
