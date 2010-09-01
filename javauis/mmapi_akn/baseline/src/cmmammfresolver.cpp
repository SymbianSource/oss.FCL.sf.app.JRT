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
* Description:  This class is used for playing sounds
*
*/


//  INCLUDE FILES
#include <jdebug.h>


#include "cmmammfresolver.h"
#include "apgcli.h"
#include "apmrec.h"


#include "hxmetadatautil.h"
_LIT8(KMimetypeRM, "audio/x-pn-realaudio");
_LIT(KRVMimeType1, "video/x-pn-realvideo");
_LIT(KRVMimeType2, "video/x-realvideo");
_LIT(KRVMimeType3, "video/vnd.rn-realvideo");


// CONSTANTS

CMMAMMFResolver* CMMAMMFResolver::NewLC()
{
    CMMAMMFResolver* self = new(ELeave)CMMAMMFResolver();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

void CMMAMMFResolver::ListImplementationsL()
{
    iControllerSelection->ListImplementationsL(*iImplementations);
    if (iImplementations->Count() > 0)
    {
        ResolveContentTypeL();
    }
}

RMMFControllerImplInfoArray* CMMAMMFResolver::Implementations()
{
    // ImplementationsOwnership method must not be called before this
    __ASSERT_DEBUG(iImplementations, User::Invariant());
    return iImplementations;
}

RMMFControllerImplInfoArray* CMMAMMFResolver::ImplementationsOwnership()
{
    // ImplementationsOwnership method must not be called many times
    __ASSERT_DEBUG(iImplementations, User::Invariant());

    RMMFControllerImplInfoArray* implementations = iImplementations;
    iImplementations = NULL; // ownership is transferred
    return implementations;
}

HBufC* CMMAMMFResolver::ContentTypeOwnership()
{
    HBufC* ct = iContentType;
    iContentType = NULL;
    return ct;
}

HBufC* CMMAMMFResolver::ContentType()
{
    return iContentType;
}

void CMMAMMFResolver::SetFileNameL(const TDesC* aFileName)
{
    HBufC* fn = NULL;
    if (aFileName)
    {
        fn = aFileName->AllocL();
    }
    delete iFileName;
    iFileName = fn;
}

HBufC* CMMAMMFResolver::FileNameOwnership()
{
    HBufC* fn = iFileName;
    iFileName = NULL;
    return fn;
}

void CMMAMMFResolver::ResolveContentTypeL()
{
    DEBUG("+ CMMAMMFResolver::ResolveContentTypeL()");
    CMMFFormatSelectionParameters* fSelect = iRequiredPlayFormatSupport;
    if (!fSelect)
    {
        fSelect = iRequiredRecordFormatSupport;
    }
    // if there is no play or record formats this object is not
    // initialized correctly.
    __ASSERT_DEBUG(fSelect, User::Invariant());

    CMMFFormatSelectionParameters::TMatchDataType type =
        fSelect->MatchDataType();

    // if there is no implementation this method must not be called.
    __ASSERT_DEBUG(iImplementations->Count() > 0, User::Invariant());
    const RMMFFormatImplInfoArray* formats =
        Formats((*iImplementations)[ 0 ]);

    HBufC8* contentType = NULL;
    TInt formatsCount = formats->Count();

    if (type == CMMFFormatSelectionParameters::EMatchMimeType)
    {
        DEBUG("CMMAMMFResolver::ResolveContentTypeL - EMatchMimeType");
        // The match was based on a mime type.
        contentType = fSelect->MatchData().AllocLC();
    }
    else if (type == CMMFFormatSelectionParameters::EMatchFileExtension)
    {
        DEBUG("CMMAMMFResolver::ResolveContentTypeL - EMatchFileExtension");
        // The match was based on a file extension.
        RApaLsSession ls;
        TInt error;
        error = ls.Connect();
        TDataRecognitionResult result;
        error = ls.RecognizeData(*iFileName, KNullDesC8, result);
        ls.Close();

        if (iFileName && (iFileName->Right(3).Compare(KRaFileExtension()) == 0))
        {
            contentType = KMimetypeRM().AllocLC();
        }
        else if (iFileName && (iFileName->Right(3).Compare(KRmFileExtension()) == 0) &&
                 !(IsRealVideoTypeL(*iFileName)))
        {
            contentType = KMimetypeRM().AllocLC();
        }
        else if (!error && (result.iConfidence >= CApaDataRecognizerType::EProbable))
        {
            DEBUG_INT("CMMAMMFResolver::ResolveContentTypeL - Confidence : %d", result.iConfidence);
            contentType = result.iDataType.Des8().AllocLC();
        }


        else
        {
            // The probability is less, hence matching is done based on legacy style
            for (TInt format = 0; (format < formatsCount) &&
                    !contentType; format++)
            {
                const CMMFFormatImplementationInformation* info = (*formats)[ format ];
                if (info->SupportsFileExtension(fSelect->MatchData()))
                {
                    if (info->SupportedMimeTypes().Count() > 0)
                    {
                        contentType = info->SupportedMimeTypes()[ 0 ].AllocLC();
                    }
                }
            }
        }
    }
    else if (type == CMMFFormatSelectionParameters::EMatchHeaderData)
    {
        DEBUG("CMMAMMFResolver::ResolveContentTypeL - EMatchHeaderData");
        // The match was on a header data.
        RApaLsSession ls;
        TInt error;
        error = ls.Connect();
        TDataRecognitionResult result;
        error = ls.RecognizeData(KNullDesC, fSelect->MatchData() , result);
        ls.Close();
        if (!error && (result.iConfidence >= CApaDataRecognizerType::EProbable))
        {
            contentType = result.iDataType.Des8().AllocLC();
        }
        else
        {
            // The probability is less, hence matching is done based on legacy style
            for (TInt format = 0; (format < formatsCount) &&
                    !contentType; format++)
            {
                CMMFFormatImplementationInformation* info = (*formats)[ format ];
                if (info->SupportsHeaderDataL(fSelect->MatchData()))
                {
                    if (info->SupportedMimeTypes().Count() > 0)
                    {
                        contentType = info->SupportedMimeTypes()[ 0 ].AllocLC();
                    }
                }
            }
        }
    }
    else
    {
        DEBUG("CMMAMMFResolver::ResolveContentTypeL - KErrNotSupported");
        // EMatchAny is not supported
        User::Leave(KErrNotSupported);
    }

    // iContentType remains NULL if content type was not found.
    if (contentType)
    {
        DEBUG_STR8("CMMAMMFResolver::ResolveContentTypeL - contentType : %s", (*contentType).Des());

        HBufC* ct = HBufC::NewL(contentType->Length());
        (ct->Des()).Copy(*contentType);
        CleanupStack::PopAndDestroy(contentType);
        delete iContentType;
        iContentType = ct;
    }
    else
    {
        //This situation should never occur
        DEBUG("CMMAMMFResolver::ResolveContentTypeL - contentType = NULL");
        User::Leave(KErrUnknown);
    }
    DEBUG("- CMMAMMFResolver::ResolveContentTypeL()");
}


TBool CMMAMMFResolver::IsRealVideoTypeL(const TDesC& aFileName)
{
    TBool result = EFalse;

    CHXMetaDataUtility *putil;
    putil = CHXMetaDataUtility::NewL();
    CleanupStack::PushL(putil);
    TRAPD(err, putil->OpenFileL(aFileName));
    DEBUG_INT("MMA:CMMAMMFResolver::IsRealVideoTypeL, err = %d", err);

    if (err != KErrNone)
    {
        CleanupStack::Pop(putil);
        putil->ResetL();
        delete putil;
        return EFalse;
    }

    TUint count = 0;
    putil->GetMetaDataCount(count);

    TUint i;
    HXMetaDataKeys::EHXMetaDataId  id;

    for (i = 0; i < count; i++)
    {
        HBufC* pDes = NULL;

        putil->GetMetaDataAt(i, id, pDes);

        if (id == HXMetaDataKeys::EHXMimeType)
        {
            if (IsRealMimeTypeSupported(*pDes))
            {
                result = ETrue;
            }
        }
    }

    CleanupStack::Pop(putil);

    putil->ResetL();
    delete putil;

    return result;
}


TBool CMMAMMFResolver::IsRealMimeTypeSupported(const TDesC& aMimeType)
{
    TBool match = EFalse;

    if (!aMimeType.Compare(KRVMimeType1()))
    {
        match = ETrue;
        return match;
    }
    if (!aMimeType.Compare(KRVMimeType2()))
    {
        match = ETrue;
        return match;
    }
    if (!aMimeType.Compare(KRVMimeType3()))
    {
        match = ETrue;
        return match;
    }

    return match;
}


void CMMAMMFResolver::GetSupportedContentTypesL(CDesC16Array& aMimeTypeArray)
{
    iControllerSelection->ListImplementationsL(*iImplementations);

    TInt impCount = iImplementations->Count();
    for (TInt i = 0; i < impCount; i++)
    {
        const RMMFFormatImplInfoArray* formats = Formats((*iImplementations)[ i ]);
        TInt formatsCount(formats->Count());
        for (TInt format = 0; format < formatsCount; format++)
        {
            const CMMFFormatImplementationInformation* info = (*formats)[ format ];
            const CDesC8Array& supportedMimes = info->SupportedMimeTypes();
            TInt mimeCount = supportedMimes.Count();
            for (TInt mime = 0; mime < mimeCount; mime++)
            {
                // must convert from 8 bits to 16 bits
                HBufC* tmpBuf = HBufC::NewLC(supportedMimes[ mime ].Length());
                tmpBuf->Des().Copy(supportedMimes[ mime ]);
                aMimeTypeArray.AppendL(*tmpBuf);
                CleanupStack::PopAndDestroy(tmpBuf);
            }
        }
    }
}


void CMMAMMFResolver::SetRequiredPlayFormatSupportL(
    CMMFFormatSelectionParameters& aRequiredSupport)
{
    iControllerSelection->SetRequiredPlayFormatSupportL(aRequiredSupport);
    iRequiredPlayFormatSupport = &aRequiredSupport;
}

void CMMAMMFResolver::SetRequiredRecordFormatSupportL(
    CMMFFormatSelectionParameters& aRequiredSupport)
{
    iControllerSelection->SetRequiredRecordFormatSupportL(aRequiredSupport);
    iRequiredRecordFormatSupport = &aRequiredSupport;
}

const RMMFFormatImplInfoArray* CMMAMMFResolver::Formats(CMMFControllerImplementationInformation* aImplementation)
{
    const RMMFFormatImplInfoArray* formats = NULL;
    if (iRequiredPlayFormatSupport)
    {
        formats = &(aImplementation->PlayFormats());
    }
    else
    {
        // if there is no play or record formats this object is not
        // initialized correctly.
        __ASSERT_DEBUG(iRequiredRecordFormatSupport, User::Invariant());

        formats = &(aImplementation->RecordFormats());
    }
    return formats;
}

CMMFControllerPluginSelectionParameters*
CMMAMMFResolver::SelectionParameters()
{
    return iControllerSelection;
}

CMMAMMFResolver::~CMMAMMFResolver()
{
    if (iImplementations)
    {
        iImplementations->ResetAndDestroy();
    }
    delete iImplementations;
    delete iContentType;
    delete iControllerSelection;
    delete iFileName;
}


CMMAMMFResolver::CMMAMMFResolver()
{
}


void CMMAMMFResolver::ConstructL()
{
    iImplementations = new(ELeave)RMMFControllerImplInfoArray();
    iControllerSelection = CMMFControllerPluginSelectionParameters::NewL();
}


//  END OF FILE
