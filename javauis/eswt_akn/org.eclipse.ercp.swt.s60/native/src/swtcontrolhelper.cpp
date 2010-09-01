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


#include <baclipb.h>
#include <s32ucmp.h>
#include "swtcontrolhelper.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <txtclipboard.h>
#endif

// ======== MEMBER FUNCTIONS ========


/**
 * Creates a copy of the bitmap passed as a parameter and returns a pointer to the copy.
 * The caller is responsible for destroying the returned pointer, which is on the cleanup stack.
 */
CFbsBitmap* SwtControlHelper::GetCopyOfBitmapLC(const CFbsBitmap* aBitmap,
        const TSize& aTargetSize)
{
    if (!aBitmap)
    {
        return NULL;
    }

    TSize bmpSize(aBitmap->SizeInPixels());
    TBool doScaledCopy(ETrue);
    if ((aTargetSize == TSize(-1, -1)) || (aTargetSize == bmpSize))
    {
        doScaledCopy = EFalse;
    }

    CFbsBitmap* result = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(result);

    TSize size;
    if (doScaledCopy)
    {
        size = aTargetSize;
    }
    else
    {
        size = bmpSize;
    }

    User::LeaveIfError(result->Create(size, aBitmap->DisplayMode()));

    if (doScaledCopy)
    {
        // Create a gc for drawing to the bitmap
        CFbsBitGc* fbsBitGc = CFbsBitGc::NewL();
        CleanupStack::PushL(fbsBitGc);
        CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(result);
        CleanupStack::Pop(fbsBitGc);
        fbsBitGc->Activate(bmpDevice);

        // Draw the scaled copy
        fbsBitGc->DrawBitmap(TRect(TPoint(0, 0), size), aBitmap);

        delete bmpDevice;
        delete fbsBitGc;
    }
    else // Do a same-sized copy
    {
        TInt len = size.iHeight * CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode());

        ASSERT(len > 0);

        result->LockHeap();
        Mem::Copy(reinterpret_cast<TUint8*>(result->DataAddress()),
                  reinterpret_cast<const TUint8*>(aBitmap->DataAddress()), len);
        result->UnlockHeap();
    }

    return result;
}


/**
 * Creates a copy of the bitmap passed as a parameter and returns a pointer to the copy.
 * The caller is responsible for destroying the returned pointer.
 */
CFbsBitmap* SwtControlHelper::GetCopyOfBitmapL(const CFbsBitmap* aBitmap,
        const TSize& aTargetSize)
{
    CFbsBitmap* result = GetCopyOfBitmapLC(aBitmap, aTargetSize);
    if (result)
    {
        CleanupStack::Pop(result);
    }
    return result;
}


/**
 * Creates a copy of the bitmap passed as a parameter and returns a pointer to the copy.
 * The caller is responsible for destroying the returned pointer, which is on the cleanup stack.
 * If the parameter bitmap is a monochrome bitmap then the copy will be inverted.
 * This function is intended to be used in cases where the component doing the drawing
 * is expecting an inverted mask bitmap.
 */
CFbsBitmap* SwtControlHelper::GetInvertedCopyOfMonoBitmapLC(const CFbsBitmap* aBitmap,
        const TSize& aTargetSize)
{
    if (!aBitmap)
    {
        return NULL;
    }

    if (!aBitmap->IsMonochrome())
    {
        return GetCopyOfBitmapLC(aBitmap, aTargetSize);
    }

    TBool doScaledCopy(aTargetSize == TSize(-1, -1) ? EFalse : ETrue);
    TSize size;
    if (doScaledCopy)
    {
        size = aTargetSize;
    }
    else
    {
        size = aBitmap->SizeInPixels();
    }

    CFbsBitmap* target = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(target);
    User::LeaveIfError(target->Create(size, EGray2));
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(target);
    CleanupStack::PushL(device);
    CFbsBitGc* gc = NULL;
    User::LeaveIfError(device->CreateContext(gc));
    TInt drawMode = CGraphicsContext::EDrawModeWriteAlpha | CGraphicsContext::EInvertPen;
    gc->SetDrawMode(CGraphicsContext::TDrawMode(drawMode));
    if (doScaledCopy)
    {
        gc->DrawBitmap(TRect(TPoint(0, 0), size), aBitmap);
    }
    else
    {
        gc->BitBlt(TPoint(), aBitmap);
    }
    delete gc;
    CleanupStack::PopAndDestroy(device);

    return target;
}


/**
 * Creates a copy of the bitmap passed as a parameter and returns a pointer to the copy.
 * The caller is responsible for destroying the returned pointer, which is on the cleanup stack.
 * If the parameter bitmap is a monochrome bitmap then the copy will be inverted.
 * This function is intended to be used in cases where the component doing the drawing
 * is expecting an inverted mask bitmap.
 */
CFbsBitmap* SwtControlHelper::GetInvertedCopyOfMonoBitmapL(const CFbsBitmap* aBitmap,
        const TSize& aTargetSize)
{
    CFbsBitmap* result = GetInvertedCopyOfMonoBitmapLC(aBitmap, aTargetSize);
    if (result)
    {
        CleanupStack::Pop(result);
    }
    return result;
}


/**
 * Retrieves the clipboard's text content
 *
 * It is up to the caller to free the descriptor by calling
 * <code>CleanupStack::PopAndDestroy()</code> if the descriptor's
 * pointer is not <code>NULL</code>
 *
 * @return A descriptor containing the clipboard's content.
 */
TPtr SwtControlHelper::GetClipboardTextContentLC()
{
    // Get the clipboard
    CClipboard* cb = NULL;
    TRAPD(error, (cb = CClipboard::NewForReadingL(CEikonEnv::Static()->FsSession()))); // codescanner::eikonenvstatic
    if (error == KErrPathNotFound || error==KErrNotFound)  // Nothing in the clipboard, not an error
        return TPtr(NULL, 0, 0);
    User::LeaveIfError(error);

    CleanupStack::PushL(cb);
    TStreamId id(cb->StreamDictionary().At(KClipboardUidTypePlainText));     //lint !e613
    if (id == KNullStreamId)
    {
        CleanupStack::PopAndDestroy(cb);
        return TPtr(NULL, 0, 0);
    }

    RStoreReadStream stream;
    stream.OpenLC(cb->Store(), id);   //lint !e613
    TInt length = stream.ReadInt32L();
    CBufFlat* buffer = CBufFlat::NewL(length);
    CleanupStack::PushL(buffer);

    RBufWriteStream bufStream(*buffer);
    CleanupClosePushL(bufStream);
    TMemoryStreamUnicodeSink sink(bufStream);
    TUnicodeExpander e;
    e.ExpandL(sink, stream, length);
    bufStream.CommitL();

    CleanupStack::PopAndDestroy();      // bufStream
    CleanupStack::Pop(buffer);
    CleanupStack::PopAndDestroy(2, cb);   // stream, cb

    CleanupStack::PushL(buffer);

    TPtrC8 ptr8(buffer->Ptr(0));
    return TPtr16(reinterpret_cast<TText16*>(const_cast<TText8*>(ptr8.Ptr())),
                  length,
                  length);  //lint !e826
}

/*
 * Returns size for a bitmap that is scaled to fit inside aMaxDestSize
 * if aSourceSize > aMaxDestSize. The scaling is done so that aspect ratio of the
 * result is the same as in source.
 *
 * If aSourceSize < aMaxDestSize, no scaling is done.
 */
TSize SwtControlHelper::GetAspectRatioScaledBitmapSize(const TSize& aSourceSize,
        const TSize& aMaxDestSize)
{
    TSize imageSize = aSourceSize;
    TInt yDiff = 0, xDiff = 0;
    TBool scalingNeeded = EFalse;

    if (aSourceSize.iWidth == 0 || aSourceSize.iHeight == 0)
    {
        return imageSize;
    }

    // Figure out if any scaling is needed
    if (aSourceSize.iHeight > aMaxDestSize.iHeight)
    {
        yDiff = aSourceSize.iHeight - aMaxDestSize.iHeight;
        scalingNeeded = ETrue;
    }
    if (aSourceSize.iWidth > aMaxDestSize.iWidth)
    {
        xDiff = aSourceSize.iWidth - aMaxDestSize.iWidth;
        scalingNeeded = ETrue;
    }

    if (scalingNeeded)
    {
        if (xDiff > yDiff)
        {
            imageSize.iWidth = aMaxDestSize.iWidth;
            imageSize.iHeight = (aSourceSize.iHeight * aMaxDestSize.iWidth) / aSourceSize.iWidth;
        }
        else if (yDiff > xDiff)
        {
            imageSize.iHeight = aMaxDestSize.iHeight;
            imageSize.iWidth = (aSourceSize.iWidth * aMaxDestSize.iHeight) / aSourceSize.iHeight;
        }
        else
        {
            // aspect ratios are the same
            imageSize = aMaxDestSize;
        }
    }

    return imageSize;
}
