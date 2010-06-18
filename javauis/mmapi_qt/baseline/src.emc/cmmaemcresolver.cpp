/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <logger.h>

#include "cmmaemcresolver.h"
#include "hxmetadatautil.h"
#include "apgcli.h"
#include "apmrec.h"

using namespace multimedia;

_LIT(KRVMimeType1, "video/x-pn-realvideo");
_LIT(KRVMimeType2, "video/x-realvideo");
_LIT(KRVMimeType3, "video/vnd.rn-realvideo");

// CONSTANTS

CMMAEMCResolver* CMMAEMCResolver::NewLC()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: NewLC ++");
    CMMAEMCResolver* self = new(ELeave)CMMAEMCResolver();
    CleanupStack::PushL(self);
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: NewLC --");
    return self;
}

CMMAEMCResolver::~CMMAEMCResolver()
{
    delete iContentType;

    delete iFileName;

    delete iMimeType;
}

HBufC* CMMAEMCResolver::ContentTypeOwnership()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: ContentTypeOwnership ++");
    HBufC* ct = iContentType;
    iContentType = NULL;
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: ContentTypeOwnership --");
    return ct;
}

HBufC8* CMMAEMCResolver::MimeTypeOwnership()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: MimeTypeOwnership ++");
    HBufC8* mt = iMimeType;
    iMimeType = NULL;
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: MimeTypeOwnership --");
    return mt;
}

HBufC* CMMAEMCResolver::ContentType()
{
    return iContentType;
}

void CMMAEMCResolver::SetFileNameL(const TDesC* aFileName)
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: SetFileNameL ++");
    HBufC* fn = NULL;
    if (aFileName)
    {
        fn = aFileName->AllocL();
    }
    delete iFileName;
    iFileName = fn;
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: SetFileNameL --");
}

HBufC* CMMAEMCResolver::FileNameOwnership()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: FileNameOwnership ++");
    HBufC* fn = iFileName;
    iFileName = NULL;
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: FileNameOwnership --");
    return fn;
}

void CMMAEMCResolver::SetMimeTypeL(const TDesC* aFileName)
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: SetMimeTypeL +");
    if (iContentType)
    {
        delete iContentType;
        iContentType = NULL;
    }
    iContentType = HBufC::NewL(KContentTypeMaxLength);
    TBuf8<KContentTypeMaxLength> mimeType;
    ResolveContentTypeL(*aFileName,mimeType);
    iContentType->Des().Copy(mimeType);
    iMimeType = HBufC8::NewL(mimeType.Length());        //8 bit Descriptor of iContentType
    iMimeType->Des().Copy(mimeType);

    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: SetMimeTypeL -");
}

void CMMAEMCResolver::ResolveContentTypeL()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver::ResolveContentTypeL +");

    if (iContentType)
    {
        delete iContentType;
        iContentType = NULL;
    }
    if (iMimeType)
    {
        delete iMimeType;
        iMimeType = NULL;
    }

    iContentType = HBufC::NewL(KContentTypeMaxLength);
    iMimeType = HBufC8::NewL(KContentTypeMaxLength);

    if (iFileName->Right(4).Compare(KAacFileExtension()) == 0)
    {
        iContentType->Des().Copy(KMimetypeAAC);
        iMimeType->Des().Copy(KMimetypeAAC);
    }
    else if (iFileName->Right(4).Compare(KAmrFileExtension()) == 0)
    {
        iContentType->Des().Copy(KMimetypeAMR);
        iMimeType->Des().Copy(KMimetypeAMR);
    }
    else if (iFileName->Right(4).Compare(KAwbFileExtension()) == 0)
    {
        iContentType->Des().Copy(KMimetypeAMRWB);
        iMimeType->Des().Copy(KMimetypeAMRWB);
    }
    else if (iFileName->Right(4).Compare(KWmaFileExtension()) == 0)
    {
        iContentType->Des().Copy(KMimetypeXMSWMA);
        iMimeType->Des().Copy(KMimetypeXMSWMA);
    }
    else if (iFileName->Right(3).Compare(KRaFileExtension()) == 0)
    {
        iContentType->Des().Copy(KMimetypeRM);
        iMimeType->Des().Copy(KMimetypeRM);
    }
    else if (iFileName->Right(4).Compare(KMp3FileExtension()) == 0)
    {
        iContentType->Des().Copy(KMimetypeMPEG);
        iMimeType->Des().Copy(KMimetypeMPEG);
    }
    else if (iHeaderData)
    {
        RApaLsSession ls;
        TInt err;
        err = ls.Connect();

        if (iFileName->Right(3).Compare(KRmFileExtension()) == 0)
        {
            TBool res = IsRealVideoTypeL(*iHeaderData);

            if (!res)
            {
                iContentType->Des().Copy(KMimetypeRM);
                iMimeType->Des().Copy(KMimetypeRM);
            }
            else
            {
                iContentType->Des().Copy(KNullDesC);
                iMimeType->Des().Copy(KNullDesC8);
            }
        }
        else if ((iFileName->Right(4).Compare(K3gpFileExtension()) == 0) ||
                 (iFileName->Right(4).Compare(KMp4FileExtension()) == 0))
        {
            TDataRecognitionResult result;
            err = ls.RecognizeData(*iFileName, *iHeaderData, result);
            if (!err && (result.iConfidence >= CApaDataRecognizerType::EProbable))
            {
                iContentType->Des().Copy(result.iDataType.Des8());
                iMimeType->Des().Copy(result.iDataType.Des8());
            }
            else
            {
                iContentType->Des().Copy(KNullDesC);
                iMimeType->Des().Copy(KNullDesC8);
            }
        }
        ls.Close();
    }
    else
    {
        iContentType->Des().Copy(KNullDesC);
        iMimeType->Des().Copy(KNullDesC8);
    }

    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver::ResolveContentTypeL -");
}
//

void CMMAEMCResolver::ResolveContentTypeL(const TDesC& /*aFileName*/, TDes8& /*aMimeType*/)
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: ResolveContentTypeL");
}

void CMMAEMCResolver::GetSupportedContentTypesL(CDesC16Array& aMimeTypeArray)
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: GetSupportedContentTypesL +");
    // All supported mimetypes are taken from "MimeTypes.h"
    TBuf16<KContentTypeMaxLength> tempBuf;
    tempBuf.Copy(KMimetype3GPP);
    aMimeTypeArray.AppendL(tempBuf);
    tempBuf.Zero();
    tempBuf.Copy(KMimetype3GPP2);
    aMimeTypeArray.AppendL(tempBuf);
    tempBuf.Zero();
    tempBuf.Copy(KMimetypeAAC);
    aMimeTypeArray.AppendL(tempBuf);
    tempBuf.Zero();
    tempBuf.Copy(KMimetypeAMR);
    aMimeTypeArray.AppendL(tempBuf);
    tempBuf.Zero();
    tempBuf.Copy(KMimetypeAMRWB);
    aMimeTypeArray.AppendL(tempBuf);
    tempBuf.Zero();
    tempBuf.Copy(KMimetypeMPEG);
    aMimeTypeArray.AppendL(tempBuf);
    tempBuf.Zero();
    tempBuf.Copy(KMimetypeMP4);
    aMimeTypeArray.AppendL(tempBuf);
    tempBuf.Zero();
    tempBuf.Copy(KMimetypeXMSWMA);
    aMimeTypeArray.AppendL(tempBuf);
    tempBuf.Zero();
    tempBuf.Copy(KMimetypeRM);
    aMimeTypeArray.AppendL(tempBuf);
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAEMCResolver :: GetSupportedContentTypesL -");
}

// EMC - II
void CMMAEMCResolver::SetSourceInfoL(const HBufC8* aHeaderData)
{
    iHeaderData = aHeaderData;
    ResolveContentTypeL();
}

TBool CMMAEMCResolver::IsRealVideoTypeL(const TDesC8& aHeader)
{
    TBool result = EFalse;

    CHXMetaDataUtility *putil;
    putil = CHXMetaDataUtility::NewL();
    CleanupStack::PushL(putil);
    TRAPD(err, putil->OpenDesL((TDesC8 &)aHeader));
    ELOG1(EJavaMMAPI, "MMA:CMMAEMCResolver::IsRealVideoTypeL, err = %d", err);

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

TBool CMMAEMCResolver::IsRealMimeTypeSupported(const TDesC& aMimeType)
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
//
//  END OF FILE
