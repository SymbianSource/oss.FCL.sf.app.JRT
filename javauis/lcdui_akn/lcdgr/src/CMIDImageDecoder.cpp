/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "CMIDImageDecoder.h"
#include "LcdFbsImage.h"
#include <j2me/jdebug.h>

CMIDImageDecoder::CMIDImageDecoder(RFs& aFsSession, TDisplayMode aDisplayMode)
        : CActive(CActive::EPriorityStandard), iRFsSession(aFsSession), iDisplayMode(aDisplayMode)
{
    CActiveScheduler::Add(this);
    iRFsSession = aFsSession;
}

void CMIDImageDecoder::ConstructL()
{
    iDecoder = CBufferedImageDecoder::NewL(iRFsSession);
}

CMIDImageDecoder::~CMIDImageDecoder()
{
    Cancel();
    ASSERT(NULL == iCallback);
    delete iBitmapImage;
    delete iDecoder;
}

void CMIDImageDecoder::AppendL(const TDesC8& aBuffer)
{
    TInt err = KErrNone;

    if (iState == EStart)
    {
        TRAP(err, iDecoder->OpenL(aBuffer, CImageDecoder::EOptionPngMissingiENDFail));

        if (err == KEComErrNoInterfaceIdentified || err == KErrNotFound)
        {
            // If the format is unrecognised AND the first 2 bytes are 0 then we assume it
            // is a WBMP. This is a requirement of the below defect.
            // The first two bytes of other formats (e.g. ico) may also begin with 0, 0
            // but they should have already been recognised by the first call to OpenL
            if (aBuffer[0] == 0 && aBuffer[1] == 0)
            {
                delete iDecoder;
                iDecoder = NULL;
                iDecoder = CBufferedImageDecoder::NewL(iRFsSession);
                DEBUG("* CMIDImageDecoder::AppendL - Format can't be fully identified, assuming this is WBMP");
                _LIT8(KWBMPMimeType, "image/vnd.wap.wbmp");
                TRAP(err, iDecoder->OpenL(aBuffer, KWBMPMimeType, CImageDecoder::EOptionPngMissingiENDFail));
            }
        }

        if ((err != KErrUnderflow) && (err != KEComErrNoInterfaceIdentified) && (err != KErrNotFound))
        {
            //
            // something's gone wrong that we can't fix by appending more data
            //
            User::LeaveIfError(err);
        }
        iState = EOpenPending;
    }
    else
    {
        iDecoder->AppendDataL(aBuffer);

        if (!iDecoder->ValidDecoder())
        {
            TRAP(err, iDecoder->ContinueOpenL());
            if ((err == KErrUnderflow) || (err == KEComErrNoInterfaceIdentified) || (err == KErrNotFound))
            {
                // decoder or framework needs more data to determine codec.
                return;
            }

            //
            // If something else has gone wrong then we can't fix by appending more data
            //
            User::LeaveIfError(err);
        }
    }

    if (iDecoder->ValidDecoder())
    {
        if (!iDecoder->IsImageHeaderProcessingComplete())
        {
            iDecoder->ContinueProcessingHeaderL();
            if (iDecoder->IsImageHeaderProcessingComplete())
            {
                iState = EOpenComplete;
            }
        }
        else
        {
            iState = EOpenComplete;
        }
    }
}

void NullPointer(TAny* aPtr)
{
    * static_cast<TAny**>(aPtr) = NULL;
}

void CMIDImageDecoder::DecodeL(MMIDImageObserver* aCallback)
{
    //
    // If we are asked to decode, but have not yet managed to process the
    // image header we leave with KErrCorrrupt indicating that we think
    // the encoded image data is corrupt.
    //
    if (iState < EOpenComplete)
    {
        User::Leave(KErrCorrupt);
    }
    ASSERT(NULL == iCallback);
    CleanupStack::PushL(TCleanupItem(NullPointer, &iCallback));
    iCallback = aCallback;
    ConvertL();
    CleanupStack::Pop();
}

void CMIDImageDecoder::Complete(TInt aResult)
{
    if (iCallback)
    {
        iCallback->DecodeComplete(aResult);
        iCallback = NULL;
    }
}

void CMIDImageDecoder::ConvertL()
{
    iState = EConvertPending;

    if (NULL == iBitmapImage)
    {
        const TFrameInfo& frameInfo = iDecoder->FrameInfo();

        TSize size = frameInfo.iOverallSizeInPixels;

        // S60: Set the colormode of the bitmap to be exactly as the phone's display.
        // This will cause CLcdImage class just to duplicate the handle of this image,
        // and not converting the pixels from the image's original color format to phone's color format.
        // The image is needed to be in the same color format as the phone's display so BitBlt() is the most efficient.
        //
        // Decoding directly to 16MA does not work in 5.0 HW. It works only in emulator and not for JPEG.
        // The image decoders need to fix this.
        // We decode to 16MU still and then a conversion from 16MU->16MA will be done
        //
        // Insufficient information available to load image in its natural format.

        TDisplayMode colorMode = frameInfo.iFrameDisplayMode;
        TDisplayMode alphaMode = ENone;

        if (frameInfo.iFlags & TFrameInfo::ETransparencyPossible)
        {
            if (frameInfo.iFlags & TFrameInfo::EAlphaChannel)
            {
                alphaMode = EGray256;
            }
            else
            {
                alphaMode = EGray2;
            }
        }

        //
        // This step is the most likely to fail in low memory conditions.
        //
        CLcdFbsImage* fbsImage = CLcdFbsImage::NewL(size, colorMode, alphaMode);
        CleanupStack::PushL(fbsImage);
        iBitmapImage = new(ELeave) CMIDBitmapImage(*fbsImage);
        CleanupStack::Pop(fbsImage);
    }

    CFbsBitmap* colorBitmap = iBitmapImage->ColorBitmap();
    CFbsBitmap* alphaBitmap = iBitmapImage->AlphaBitmap();

    if (alphaBitmap)
    {
        iDecoder->Convert(&iStatus, *colorBitmap, *alphaBitmap, 0);
    }
    else
    {
        iDecoder->Convert(&iStatus, *colorBitmap, 0);
    }
    SetActive();
}

void CMIDImageDecoder::Dispose()
{
    delete this;
}

MMIDBitmapImage* CMIDImageDecoder::BitmapImage() const
{
    return iBitmapImage;
}

void CMIDImageDecoder::RunL()
{
    ASSERT(iState == EConvertPending);
    Complete(iStatus.Int());
}

TInt CMIDImageDecoder::RunError(TInt aError)
{
    ASSERT(KErrNone == aError);   // should never happen
    return aError;
}

void CMIDImageDecoder::DoCancel()
{
    ASSERT(iState == EConvertPending);
    iDecoder->Cancel();
}


