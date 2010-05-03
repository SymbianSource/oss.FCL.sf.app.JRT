/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <imageconversion.h>
#include <f32file.h>
#include <gdi.h>
#include "swtimagedataloader.h"
#include "swtimage.h"


//lint -esym( 613, CSwtImageDataLoader::iDecoder )
//lint -esym( 613, CSwtImageDataLoader::iEncoder )


// ======== MEMBER FUNCTIONS ========


CSwtImageDataLoader::~CSwtImageDataLoader()
{
    Cancel();
    FreeBitmaps(); // delete iBitmap and iMask
    FreeBuffers(); // delete iBuffer
    delete iDecoder;

    delete iImageData;
    if (iImageDataArray)
    {
        iImageDataArray->ResetAndDestroy();
    }
    delete iImageDataArray;

    delete iEncoder;

    if (iActiveScheduler)
    {
        delete iActiveScheduler;
    }
    RFbsSession::Disconnect();
    iFs.Close();

#ifdef _lint
    iBuffer = NULL;
    iMask   = NULL;
    iBitmap = NULL;
#endif
}

CSwtImageDataLoader::CSwtImageDataLoader()
        : CActive(EPriorityStandard)
        , iImageCoordinates(TPoint::EUninitialized)
        , iDesc(NULL, 0, 0)
{
    CActiveScheduler::Add(this);
}

CSwtImageDataLoader* CSwtImageDataLoader::NewL()
{
    CActiveScheduler* activeScheduler = NULL;
    if (!CActiveScheduler::Current())
    {
        activeScheduler = new(ELeave) CActiveScheduler;
        CleanupStack::PushL(activeScheduler);
        CActiveScheduler::Install(activeScheduler);
    }
    CSwtImageDataLoader* self = new(ELeave) CSwtImageDataLoader();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(1);   // self

    if (activeScheduler)
    {
        self->iActiveScheduler = activeScheduler;
        CleanupStack::Pop(1);  //activescheduler
    }
    return self;
}

void CSwtImageDataLoader::ConstructL()
{
    iImageDataArray = new(ELeave) CSwtImageDataArray(10);
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(RFbsSession::Connect(iFs));
}

void CSwtImageDataLoader::DecodeImageL(const TDesC& aFileName)
{
    // In normal use of ImageDataLoader, it must be disposed by its creator
    // after a call to this method ( so iDecoder should not be not NULL
    // because of a previous call to this method )
    ASSERT(!iDecoder);

    // In case ImageDataLoader is not used normally and we are not in debug,
    // ASSERT is ignored, so deleting iDecoder is safer
    delete iDecoder;
    iDecoder = NULL;
    // Same applies to other data members of CSwtImageDataLoader

    /* Known issue:
    Sometimes after a few thousand RThread::Create(), it fails with KErrGeneral from RThread::Create().
    There is a problem with the underlying PC emulation of threads. When a thread is terminated under Symbian OS,
    the thread has exited and freed it's memory, but the underlying Windows thread cannot free it's resources until
    the emulator process exits. After a few thousand threads have been created and destroyed, the resources mount up
    so that there is no space left to create a new thread and the emulator becomes useless and needs restarting.
    Apparent Memory Leak. This behaviour can be seen when opening and closing an application. The memory footprint
    of the emulator keeps going up each time the app is terminated and closed.
    There is no current solution for 7.0 and precursors, it is a limitation of the emulator.
    The problem does not show up on hardware.
    */
    TRAPD(error, (iDecoder = CImageDecoder::FileNewL(iFs, aFileName,
                             CImageDecoder::EOptionAlwaysThread)));
    LeaveIfErrorFromICLDecoderL(error);

    // Check if the image is GIF
    TUid imgType;
    TUid imgSubType;
    iDecoder->ImageType(0, imgType, imgSubType);
    if (imgType == KImageTypeGIFUid)
    {
        iIsGif = ETrue;
    }

    TInt nbFrame = iDecoder->FrameCount();
    for (TInt index = 0; index < nbFrame; ++index)
    {
        FreeBitmaps();
        const TFrameInfo& currentFrame = iDecoder->FrameInfo(index);

        SetMaskL(currentFrame);
        SetLogicalScreenValues(currentFrame);
        SetFrameInfosL(currentFrame);

        // Calculate the optimal display mode for the new bitmap and create it
        TDisplayMode dispMode(EColor16MU);
        if (iIsGif && currentFrame.iFrameDisplayMode == EColor256)
        {
            // iFrameDisplayMode may always return EColor256.
            // Return maximum color mode to ensure best image quality.
            dispMode = EColor16M;
        }
        else if (currentFrame.iFrameDisplayMode < EColor16M
                 || currentFrame.iFrameDisplayMode == EColor4K)
        {
            dispMode = EColor64K;
        }
        iBitmap = new(ELeave) CFbsBitmap;
        User::LeaveIfError(
            iBitmap->Create(
                currentFrame.iOverallSizeInPixels,
                dispMode));

        TRequestStatus localStatus;
        // Mask is created each time because the frames size may vary in a single source
        if (iMask == NULL)
        {
            iDecoder->Convert(&localStatus, *iBitmap, index);
        }
        else
        {

            iDecoder->Convert(&localStatus,*iBitmap, *iMask, index);
        }

        // We are waiting on the TRequestStatus we passed to the asynchronous
        // function "convert( ... )" so that we transform the call to an
        // asyncrhonous method into a call to a synchronous one.
        User::WaitForRequest(localStatus);
        LeaveIfErrorFromICLDecoderL(localStatus.Int());

        ExtractImagedataL();
    }

    FreeBitmaps();
}

void CSwtImageDataLoader::DecodeWholeImageFromBufferL(const TDesC8& aBuffer)
{
    iWholeImageAtOnce = ETrue;

    // In normal use of ImageDataLoader, it must be disposed by its creator after a call to this method ( so iDecoder should not be not NULL because of a previous call to this method )
    ASSERT(iDecoder == NULL);
    // In case ImageDataLoader is not used normally and we are not in debug, ASSERT is ignored, so deleting iDecoder is safer
    delete iDecoder;
    iDecoder = NULL;
    // Same applies to other data members of CSwtImageDataLoader

    TRAPD(error,(iDecoder = CImageDecoder::DataNewL(iFs, aBuffer,CImageDecoder::EOptionAlwaysThread)));
    LeaveIfErrorFromICLDecoderL(error);

    TInt nbFrame = iDecoder->FrameCount();
    for (TInt index = 0; index < nbFrame; ++index)
    {
        DecodeFrameL(index);
    }

    FreeBitmaps();

    LeaveIfErrorFromICLDecoderL(iResult);
}

void CSwtImageDataLoader::DecodeImageFromBufferL(const TDesC8& aBuffer)
{
    iNextFrameToDecode = 0;

    iBuffer = aBuffer.AllocL();
    iDesc.Set(iBuffer->Des());

    // In normal use of ImageDataLoader, it must be disposed by its creator after decoding an image ( so iDecoder should not be not NULL because of a previous call to this method )
    ASSERT(iDecoder == NULL);
    // In case ImageDataLoader is not used normally and we are not in debug, ASSERT is ignored, so deleting iDecoder is safer
    delete iDecoder;
    iDecoder = NULL;
    // Same applies to other data members of CSwtImageDataLoader

    TRAPD(error, (iDecoder = CImageDecoder::DataNewL(iFs, iDesc, CImageDecoder::EOptionAlwaysThread)));
    LeaveIfErrorFromICLDecoderL(error);

    // We are starting with a new frame ( the first one )
    iStartDecodingAnother = ETrue;

    // If frame to decode has not been detected by decoder and header is not complete, then more data is needed
    if ((iDecoder->FrameCount() <= iNextFrameToDecode) && (!iDecoder->IsImageHeaderProcessingComplete()))
        return; // need data

    // Start decoding frames from the buffer
    while (iStartDecodingAnother)
    {
        DecodeNextFrameL();
    };
}

TBool CSwtImageDataLoader::DecodeNextFrameL()
{
    if (iNextFrameToDecode < iDecoder->FrameCount() && iStartDecodingAnother)
    {
        DecodeFrameL(iNextFrameToDecode);
        LeaveIfErrorFromICLDecoderL(iResult);

        return ETrue;
    }

    return EFalse;
}

void CSwtImageDataLoader::DecodeFrameL(TInt aIndexOfFrame)
{
    FreeBitmaps();
    const TFrameInfo& currentFrame = iDecoder->FrameInfo(aIndexOfFrame);

    SetMaskL(currentFrame);
    SetLogicalScreenValues(currentFrame);
    SetFrameInfosL(currentFrame);
    // Check if the image is GIF
    TUid imgType;
    TUid imgSubType;
    iDecoder->ImageType(0, imgType, imgSubType);
    if (imgType == KImageTypeGIFUid)
    {
        iIsGif = ETrue;
    }


    // Calculate the optimal display mode for the new bitmap and create it
    TDisplayMode dispMode(EColor16MU);
    if (iIsGif && currentFrame.iFrameDisplayMode == EColor256)
    {
        // iFrameDisplayMode may always return EColor256.
        // Return maximum color mode to ensure best image quality.
        dispMode = EColor16M;
    }
    else if (currentFrame.iFrameDisplayMode < EColor16M
             || currentFrame.iFrameDisplayMode == EColor4K)
    {
        dispMode = EColor64K;
    }

    // creating the main bitmap
    iBitmap = new(ELeave) CFbsBitmap();
    User::LeaveIfError(iBitmap->Create(currentFrame.iOverallSizeInPixels,dispMode));


    // Mask is created each time because the frames size may vary in a single source
    if (iMask == NULL)
    {
        iDecoder->Convert(&iStatus, *iBitmap, aIndexOfFrame);
    }
    else
    {
        iDecoder->Convert(&iStatus, *iBitmap, *iMask, aIndexOfFrame);
    }

    // The mechanism used in the function DecodeImage( ... ) does not work here for unclear reasons.
    // ( the convert function returns and the returned status is KRequestPending ). So we use this one.
    SetActive();
    CActiveScheduler::Start();

    // Extracting Imagedata is performed in Runl;
}

void CSwtImageDataLoader::AppendDataL(const TDesC8& aBuffer)
{
    // newBuffer is created and initialized with iBuffer content ( and iBuffer is deleted )
    HBufC8* newBuffer = iBuffer->ReAllocL(iBuffer->Length()+aBuffer.Length());
    // Set iBuffer to the new larger buffer ( for next time )
    iBuffer = newBuffer;
    // Refresh iDesc and append new data
    iDesc.Set(iBuffer->Des());
    iDesc.Append(aBuffer);

    // In normal use of ImageDataLoader, it must be disposed by its creator after decoding an image ( so iDecoder should not be not NULL because of a previous call to DecodeImageFromBufferL method )
    ASSERT(iDecoder);
    // In case ImageDataLoader is not used normally and we are not in debug, ASSERT is ignored, so deleting iDecoder is safer
    delete iDecoder;
    iDecoder = NULL;
    // Same applies to other data members of CSwtImageDataLoader

    TRAPD(error, (iDecoder = CImageDecoder::DataNewL(iFs, iDesc, CImageDecoder::EOptionAlwaysThread)));
    LeaveIfErrorFromICLDecoderL(error);

    // data have been appended. Now launching the correct action
    iStartDecodingAnother = ETrue;

    // If frame to decode has not been detected by decoder and header is not complete, then process newly read data
    if ((iDecoder->FrameCount() <= iNextFrameToDecode) && (!iDecoder->IsImageHeaderProcessingComplete()))
    {
        iDecoder->ContinueProcessingHeaderL();
        // If now processed data is enough to decode then do it
        if (!((iDecoder->FrameCount() <= iNextFrameToDecode) && (!iDecoder->IsImageHeaderProcessingComplete())))
        {
            while (DecodeNextFrameL()) {};
            return;
        }
        // If processed data is still not enough to decode then wait for next data
        else
        {
            return;
        }
    }
    // If frame to decode has been detected by decoder then decode frame
    else if (iDecoder->FrameCount() > iNextFrameToDecode)
    {
        while (DecodeNextFrameL()) {};
        return;
    }
    // If frame to decode has not been detected by decoder while header is complete, then don't process this unknown newly read data
    else
    {
        return;
    }
}

void CSwtImageDataLoader::EncodeImageToFileL(MSwtImageData& aImageData, TInt aFormat, const TDesC& aDestination)
{
    CheckDestinationL(aDestination, iFs);
    // -- First of all : getting an image object
    CSwtImage* image = NULL;
    image = CSwtImage::NewL(NULL, aImageData, NULL);
    CleanupStack::PushL(image);

    // -- Second extracting a CFbsBitmat
    CFbsBitmap* bitmap(NULL);
    TBool haveMask = image->MaskBitmap() != NULL;
    if (haveMask)
    {
        bitmap = image->BitmapWithAlphaLC();
    }
    else
    {
        bitmap = &image->Bitmap();
    }



    // -- third :  create an encoder based on the format

    const TDesC8& mime = GetMimeType(static_cast<TSwtImageType>(aFormat));

    // REMINDER : Symbian does not manage any kind of transparency for the time being.
    TRAPD(error, (iEncoder = CImageEncoder::FileNewL(
                                 iFs,
                                 aDestination,
                                 mime,
                                 CImageEncoder::EOptionAlwaysThread
                             )));

    LeaveIfErrorFromICLEncoderL(error);
    // Fourth : execute decoding
    // Note, the encoder does not support to save alpha channel yet.
    // so, if the bitmamp's display mode is EColor16MA, the alpha channel data will be ignored
    iEncoder->Convert(&iStatus,*bitmap);

    // -- Finally wait and quit

    // The mechanism used in the equivalent function DecodeImage( ... ) does not work here for unclear reasons.
    // ( the convert function returns and the returned status is KRequestPending ). So we use this one.
    SetActive();
    CActiveScheduler::Start();

    if (haveMask)
    {
        CleanupStack::PopAndDestroy(bitmap);
    }

    CleanupStack::PopAndDestroy(image);
    delete iEncoder;
    iEncoder = NULL;
    LeaveIfErrorFromICLEncoderL(iResult);
}

HBufC8* CSwtImageDataLoader::EncodeImageToStreamL(MSwtImageData& aImageData, TInt aFormat)
{

    // -- First of all : getting an image object
    CSwtImage* image = NULL;
    image = CSwtImage::NewL(NULL, aImageData, NULL);
    CleanupStack::PushL(image);

    // -- Second extracting a CFbsBitmat
    CFbsBitmap& bitmap = image->Bitmap();

    // -- third :  create an encoder based on the format

    const TDesC8& mime = GetMimeType(static_cast<TSwtImageType>(aFormat));
    // REMINDER : Symbian does not manage any kind of transparency for the time being.
    TRAPD(error, (iEncoder = CImageEncoder::DataNewL(
                                 iBuffer,
                                 mime,
                                 CImageEncoder::EOptionAlwaysThread
                             )));   // lint !e613

    LeaveIfErrorFromICLEncoderL(error);
    // Fourth : execute decoding
    iEncoder->Convert(&iStatus,bitmap);

    // -- Finally wait and quit

    // The mechanism used in the equivalent function DecodeImage( ... ) does not work here for unclear reasons.
    // ( the convert function returns and the returned status is KRequestPending ). So we use this one.
    SetActive();
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(image);
    delete iEncoder;
    iEncoder = NULL;
    LeaveIfErrorFromICLEncoderL(iResult);
    return iBuffer;
}

/*
 *  Free the bitmaps and their associated data
 */
void CSwtImageDataLoader::LeaveIfErrorFromICLDecoderL(TInt aLeaveCode) const
{
    switch (aLeaveCode)
    {
    case KErrNone:
        break;
    case KErrNotFound:
        // Error in loading related codec
        User::Leave(ESwtErrorIO);
    case KErrUnderflow:
    case KEComErrNoInterfaceIdentified:
    case KErrCorrupt:
        // Codec unknown for the image
        User::Leave(ESwtErrorInvalidImage);
    default:
        User::Leave(aLeaveCode);
    }
}

void CSwtImageDataLoader::LeaveIfErrorFromICLEncoderL(TInt aLeaveCode) const
{
    switch (aLeaveCode)
    {
    case KErrNone:
        break;
    case KErrNotFound:
    case KEComErrNoInterfaceIdentified:
        // Error in loading related codec : should not happen
        User::Leave(aLeaveCode);      // what else?
    case KErrUnderflow:
    case KErrCorrupt:
        User::Leave(ESwtErrorInvalidImage);
    default:
        User::Leave(aLeaveCode);
    }
}

void CSwtImageDataLoader::FreeBitmaps()
{
    delete iBitmap;
    iBitmap = NULL;
    delete iMask;
    iMask = NULL;
}

void CSwtImageDataLoader::FreeBuffers()
{
    delete iBuffer;
    iBuffer = NULL;
}

void CSwtImageDataLoader::ExtractImagedataL()
{
    CSwtImage* image = NULL;

    image = CSwtImage::NewL(*iBitmap, iMask, iImageCoordinates, iDelay, iDisposal);   //lint !e613
    CleanupStack::PushL(image);
    // After this line the ImageData object pointed by iImageDataArray will be owned by us
    SetImageDataL(*image->GetImageDataL());

    CleanupStack::PopAndDestroy(image);
}

CSwtImageDataArray* CSwtImageDataLoader::GetImageData()
{
    return iImageDataArray;
}

void CSwtImageDataLoader::SetImageDataL(MSwtImageData& aImageData)
{
    iImageDataArray->AppendL(&aImageData);
}

void CSwtImageDataLoader::Dispose()
{
    delete this;
}

void CSwtImageDataLoader::SetMaskL(const TFrameInfo& aInfo)
{
    if (!(aInfo.iFlags & TFrameInfo::ETransparencyPossible))
    {
        return;
    }

    TDisplayMode mode(EGray2);
    if (aInfo.iFlags & TFrameInfo::EAlphaChannel)
    {
        mode = EGray256;
    }

    iMask = new(ELeave) CFbsBitmap();
    User::LeaveIfError(iMask->Create(aInfo.iOverallSizeInPixels, mode));
}

void CSwtImageDataLoader::SetLogicalScreenValues(const TFrameInfo& aInfo)
{
    if (iLogicalScreenValues.iHeight < aInfo.iOverallSizeInPixels.iHeight)
        iLogicalScreenValues.iHeight = aInfo.iOverallSizeInPixels.iHeight;
    if (iLogicalScreenValues.iWidth  < aInfo.iOverallSizeInPixels.iWidth)
        iLogicalScreenValues.iWidth = aInfo.iOverallSizeInPixels.iWidth;
}

TInt CSwtImageDataLoader::GetLogicalScreenHeight()
{
    return iLogicalScreenValues.iHeight;
}

TInt CSwtImageDataLoader::GetLogicalScreenWidth()
{
    return iLogicalScreenValues.iWidth;
}

void CSwtImageDataLoader::SetFrameInfosL(const TFrameInfo& aInfo)
{
    iImageCoordinates = aInfo.iFrameCoordsInPixels.iTl;

    iDelay = I64INT(aInfo.iDelay.Int64()) / 10000;   // micro to 1/100 th of second

    if (aInfo.iFlags & TFrameInfo::ELeaveInPlace)
        iDisposal = KSwtDisposalFillNone;
    else if (aInfo.iFlags & TFrameInfo::ERestoreToBackground)
        iDisposal = KSwtDisposalFillBackground;
    else if (aInfo.iFlags & TFrameInfo::ERestoreToPrevious)
        iDisposal = KSwtDisposalFillPrevious;
    else
        iDisposal = KSwtDisposalUnspecified;
}

void CSwtImageDataLoader::RunL()
{
    CActiveScheduler::Stop();
    iResult = iStatus.Int();

    if (iDecoder != NULL)
    {
        ASSERT(iEncoder ==NULL);
        switch (iResult)
        {
        case KErrNone:
            ExtractImagedataL();
            // Update decoding state in case of bufferized decoding
            iStartDecodingAnother = ETrue;
            ++iNextFrameToDecode;
            break;

        case KErrUnderflow:
            if (iWholeImageAtOnce)
            {
                iResult = KErrCorrupt;
            }
            // In case of bufferized decoding this is not an error
            else
            {
                iResult = KErrNone;
                iStartDecodingAnother = EFalse;
            }
            break;

        default:
            break;
        }
    }
    else
    {
        ASSERT(iEncoder !=NULL);
        switch (iResult)
        {
        case KErrNone:
            break;
        case KErrUnderflow:
            iResult = KErrCorrupt;
            break;
        default:
            ASSERT(0);
            break;
        }
    }
}

void CSwtImageDataLoader::DoCancel()
{
    if (iDecoder)
        iDecoder->Cancel();
    if (iEncoder)
        iEncoder->Cancel();
}

TInt CSwtImageDataLoader::RunError(TInt aError)
{
    // Delete all processed bitmaps
    FreeBitmaps(); // delete iBitmap and iMask
    FreeBuffers(); // delete iBuffer
    if (iImageDataArray)
    {
        iImageDataArray->ResetAndDestroy();
        delete iImageDataArray;
        iImageDataArray = NULL;
    }


    // Complete with error
    iImageStatus = &iStatus;
    User::RequestComplete(iImageStatus, aError);
    return KErrNone;
}


void CSwtImageDataLoader::CheckDestinationL(const TDesC& aDestination, const RFs& aFs) const
{
    // As we expect the file being correctly set, if it not possible to create a file then we leave

    TEntry entry;
    TInt err = KErrNone;

    TParse parser;
    parser.Set(aDestination,NULL,NULL);

    if (!parser.IsRoot())
    {
        err = aFs.Entry(parser.DriveAndPath(), entry);
        if (err != KErrNone)
            User::Leave(ESwtErrorIO);
    }
    else if (!parser.NameOrExtPresent())
    {
        User::Leave(ESwtErrorIO);
    }

    err = aFs.Entry(parser.FullName(), entry);
    if (entry.IsReadOnly())
        User::Leave(ESwtErrorIO);

}

const TDesC8& CSwtImageDataLoader::GetMimeType(TSwtImageType aFormat) const
{
    _LIT8(KGif,"image/gif");
    _LIT8(KPng,"image/png");
    _LIT8(KJpg, "image/jpeg");

    switch (aFormat)
    {
    case ESwtImageGif:
        return KGif;
    case ESwtImagePng:
        return KPng;
    case ESwtImageJpeg:
        return KJpg;
    default:
        ASSERT(EFalse);
        return KNullDesC8;
    }
}

//lint +esym( 613, CSwtImageDataLoader::iDecoder )
//lint +esym( 613, CSwtImageDataLoader::iEncoder )
