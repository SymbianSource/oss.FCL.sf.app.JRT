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
* Description:  Downscales images when needed
*
*/


// INCLUDE FILES
#include "cpimimagescaler.h"
#include "logger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMImageScaler::CPIMImageScaler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMImageScaler::CPIMImageScaler() :
        CActive(EPriorityStandard), iState(EReady), iError(KErrNone),
        iFbsSessionConnected(EFalse)
{
    JELOG2(EPim);
    // Add object to the active scheduler
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
// CPIMImageScaler::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CPIMImageScaler::ConstructL()
{
    JELOG2(EPim);
    User::LeaveIfError(iFileServer.Connect());
}

// -----------------------------------------------------------------------------
// CPIMImageScaler::Complete
// Performs various cleanups
// -----------------------------------------------------------------------------
//
void CPIMImageScaler::Complete(TInt aError)
{
    JELOG2(EPim);
    if (KErrNone == aError)
    {
        iState = ECompleted;
    }
    else
    {
        iError = aError;
        iState = EError;
    }
    iWait->AsyncStop();
}

// -----------------------------------------------------------------------------
// CPIMImageScaler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMImageScaler* CPIMImageScaler::NewL()
{
    JELOG2(EPim);
    CPIMImageScaler* self = new(ELeave) CPIMImageScaler;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CPIMImageScaler::~CPIMImageScaler()
{
    JELOG2(EPim);
    delete iImageReader;
    delete iImageWriter;
    delete iBitmapScaler;
    delete iBitmap;
    if (iFbsSessionConnected)
    {
        RFbsSession::Disconnect();
    }
    delete iImage;
    delete iWait;
    iFileServer.Close();
}

// -----------------------------------------------------------------------------
// CPIMImageScaler::ScaleL
// Opens a JPEG image, checks its size and scales to 80x96 if larger.
// -----------------------------------------------------------------------------
//
HBufC8* CPIMImageScaler::ScaleL(TDesC8& aSrcImage)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iState == EReady, User::Leave(KErrNotReady));
    // First we create the decoder to read the image
    CImageDecoder* imageReader = NULL;
    TRAPD(err, imageReader = CImageDecoder::DataNewL(iFileServer, aSrcImage));
    if (err == KErrNoMemory)
    {
        User::Leave(err);
    }
    else if (err != KErrNone)
    {
        User::Leave(KErrAbort);
    }

    delete iImageReader;
    iImageReader = imageReader;
    // we create bitmap for the raw image
    const TFrameInfo& frameInfo = iImageReader->FrameInfo();
    if (!CheckSize(frameInfo.iOverallSizeInPixels))
    {
        // we don't have to resize
        delete iImageReader;
        iImageReader = NULL;
        return aSrcImage.AllocL();
    }

    if (!iFbsSessionConnected)
    {
        User::LeaveIfError(RFbsSession::Connect());
        iFbsSessionConnected = ETrue;
    }
    if (!iBitmap)
    {
        iBitmap = new(ELeave) CFbsBitmap;
    }
    iBitmap->Create(frameInfo.iOverallSizeInPixels, frameInfo.iFrameDisplayMode);

    // We create the encoder as well
    CImageEncoder* imageWriter = CImageEncoder::DataNewL(iImage,
                                 CImageEncoder::EOptionNone, KImageTypeJPGUid);
    delete iImageWriter;
    iImageWriter = imageWriter;
    // and scaler
    if (!iBitmapScaler)
    {
        iBitmapScaler = CBitmapScaler::NewL();
    }
    if (!iWait)
    {
        iWait = new(ELeave) CActiveSchedulerWait;
    }

    iState = EOpening;
    iImageReader->Convert(&iStatus, *iBitmap);
    SetActive();
    iWait->Start();

    // These can't be reused
    delete iBitmap;
    iBitmap = NULL;
    delete iImageReader;
    iImageReader = NULL;
    delete iImageWriter;
    iImageWriter = NULL;

    if (ECompleted != iState)
    {
        if (iState == EError)
        {
            User::Leave(iError);
        }
        else
        {
            User::Leave(KErrUnknown);
        }
    }
    HBufC8* retVal = iImage;
    iImage = NULL;
    iState = EReady;
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMImageScaler::CheckSize
// Checks image size and resizes if necessary
// -----------------------------------------------------------------------------
//
TBool CPIMImageScaler::CheckSize(TSize aSize)
{
    JELOG2(EPim);
    if ((aSize.iHeight > KThumbHeight) || (aSize.iWidth > KThumbWidth))
    {
        // need to resize
        return ETrue;
    }
    return EFalse;
}

// -----------------------------------------------------------------------------
// CPIMImageScaler::Encode
// Encodes the image
// -----------------------------------------------------------------------------
//
void CPIMImageScaler::Encode()
{
    JELOG2(EPim);
    iState = EEncoding;
    iImageWriter->Convert(&iStatus, *iBitmap);
    SetActive();
}

// -----------------------------------------------------------------------------
// CPIMImageScaler::RunL
// Active object run method
// -----------------------------------------------------------------------------
//
void CPIMImageScaler::RunL()
{
    JELOG2(EPim);
    if (iStatus != KErrNone)
    {
        // Error has occured; inform java
        Complete(iStatus.Int());
    }
    else
    {
        switch (iState)
        {
        case EOpening:
        {
            TSize KThumbSize(KThumbWidth, KThumbHeight);
            iState = EScaling;
            iBitmapScaler->Scale(&iStatus, *iBitmap, KThumbSize, ETrue);
            SetActive();
            break;
        }
        case EScaling:
        {
            Encode();
            break;
        }
        case EEncoding:
        {
            Complete(KErrNone);
            break;
        }
        default:
        {
            // unknown state
            __ASSERT_DEBUG(EFalse, User::Invariant());
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMImageScaler::DoCancel
// Active object cancel method
// -----------------------------------------------------------------------------
void CPIMImageScaler::DoCancel()
{
    JELOG2(EPim);
    if (iBitmapScaler)
    {
        iBitmapScaler->Cancel();
    }
    if (iImageReader)
    {
        iImageReader->Cancel();
    }
    if (iImageWriter)
    {
        iImageWriter->Cancel();
    }
    iState = EReady;
}

//  End of File
