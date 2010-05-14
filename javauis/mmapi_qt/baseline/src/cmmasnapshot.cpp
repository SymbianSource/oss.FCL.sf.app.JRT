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
* Description:  This class takes snapshot and resizes bitmap if needed.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmasnapshot.h"
#include "mmmaguiplayer.h"
#include "mmmasnapshotreadycallback.h"
#include "mmmasnapshot.h"

// CONSTANTS
const TInt KIgnoreSize = -1;


// CONSTRUCTION
CMMASnapshot* CMMASnapshot::NewL(MMMAGuiPlayer* aGuiPlayer,
                                 MMMASnapshotReadyCallback& aCallBack)
{
    CMMASnapshot* self = new(ELeave) CMMASnapshot(aGuiPlayer,
            aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
}

CMMASnapshot::~CMMASnapshot()
{
    if (iScaler)
    {
        iScaler->Cancel();
        delete iScaler;
    }
    if (iEncoder)
    {
        iEncoder->Cancel();
        delete iEncoder;
    }
    delete iBitmap;
    delete iBuffer;
    delete iSettings;
}

void CMMASnapshot::ConstructL()
{
    CActiveScheduler::Add(this);
    iScaler = CBitmapScaler::NewL();
}

CMMASnapshot::CMMASnapshot(MMMAGuiPlayer* aGuiPlayer,
                           MMMASnapshotReadyCallback& aCallBack):
        CActive(EPriorityStandard),
        iEncoding(MMMASnapshot::EBitmap),
        iCallBack(aCallBack),
        iState(EIdle)
{
    iGUIPlayer = aGuiPlayer;
}

void CMMASnapshot::TakeSnapShotL(const TDesC& aProperties)
{
    // snapshot sequence is not finished
    __ASSERT_DEBUG(iState == EIdle, User::Invariant());
    // image buffer must be taken with ImageBuffer before taking new snapshot
    __ASSERT_DEBUG(iBuffer == NULL, User::Invariant());

    CMMAImageSettings* settings
    = TMMAParameterValidator::ValidateImagePropertiesL(aProperties);
    CleanupStack::PushL(settings);

    delete iSettings;
    CleanupStack::Pop(settings);
    iSettings = settings;

    // take snapshot from player. RunL is called when image is ready
    // or error occures
    iState = ETakingSnapshot;
    TSize snapshotSize(iSettings->iWidth, iSettings->iHeight);
    iEncoding = iGUIPlayer->SnapshoterL()->TakeSnapshotL(&iStatus,
                snapshotSize,
                *iSettings);
    SetActive();
}

HBufC8* CMMASnapshot::ImageBuffer()
{
    // this must not be called when snapshot sequence is running
    __ASSERT_DEBUG(iState == EIdle, User::Invariant());
    HBufC8* buffer = iBuffer;
    // caller takes ownership of the buffer
    iBuffer = NULL;
    return buffer;
}

void CMMASnapshot::ResizeL()
{
    iState = EResizing;
    TSize imageSize(iBitmap->SizeInPixels());
    if (iSettings->iWidth != KIgnoreSize)
    {
        imageSize.iWidth = iSettings->iWidth;
    }
    if (iSettings->iHeight != KIgnoreSize)
    {
        imageSize.iHeight = iSettings->iHeight;
    }
    if (imageSize == iBitmap->SizeInPixels())
    {
        // no user resizing needed, continue sequence
        EncodeL();
    }
    else
    {
        iScaler->Scale(&iStatus, *iBitmap, imageSize, EFalse);
        SetActive();
    }
}

void CMMASnapshot::EncodeL()
{
    // CImageEncoder cannot be reused, so have to make it every time
    CImageEncoder* encoder = CImageEncoder::DataNewL(iBuffer, *iSettings->iMimeType);
    delete iEncoder;
    iEncoder = encoder;

    iState = EEncoding;
    iEncoder->Convert(&iStatus, *iBitmap, iSettings->iImageData);
    SetActive();
}

void CMMASnapshot::Completed(TInt aError)
{
    iStatus = aError;
    iCallBack.SnapshotReady();
}

void CMMASnapshot::RunL()
{
    if (iStatus != KErrNone)
    {
        // Error has occured, inform java side and change state
        iState = EIdle;
        Completed(iStatus.Int());
        return;
    }

    switch (iState)
    {
    case ETakingSnapshot:
    {
        LOG1( EJavaMMAPI, EInfo, "MMA::CMMASnapshot::RunL: iEncoding = %d", iEncoding);
        if (iEncoding == MMMASnapshot::EEncoded)
        {
            // take encoded image from player.
            // Ownership transfers to this class.
            iBuffer = iGUIPlayer->SnapshoterL()->SnapshotEncoded();
            if (!iBuffer)
            {
                // error has occured with taking image
                Completed(KErrNotFound);
            }
            // Image is ready, update internal state and inform listener
            // Encoded images are not resized
            iState = EIdle;
            Completed(KErrNone);
        }
        else
        {
            // take bitmap from player.
            // Ownership transfers to this class.
            iBitmap = iGUIPlayer->SnapshoterL()->SnapshotBitmap();
            if (!iBitmap)
            {
                // error has occured with taking image
                Completed(KErrNotFound);
            }
            // Continue to next state
            ResizeL();
        }
        break;
    }
    case EResizing:
    {
        // Continue to next state
        EncodeL();
        break;
    }
    case EEncoding:
    {
        delete iEncoder;
        iEncoder = NULL;

        delete iBitmap;
        iBitmap = NULL;
        iState = EIdle;
        // encoding is ready, inform listener
        Completed(KErrNone);
        break;
    }
    default:
    {
        // unknown state
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    }
}

TInt CMMASnapshot::RunError(TInt aError)
{
    // Reset state
    iState = EIdle;
    // Pass error code to observer
    Completed(aError);

    return KErrNone;
}

void CMMASnapshot::DoCancel()
{
    // snapshot taking cannot be cancelled
    if (iScaler)
    {
        iScaler->Cancel();
    }
    if (iEncoder)
    {
        iEncoder->Cancel();
    }
    iState = EIdle;
}

//  END OF FILE
