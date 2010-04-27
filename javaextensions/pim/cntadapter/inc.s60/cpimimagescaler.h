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
* Description:  Downscales images when necessary
 *
*/


#ifndef CPIMIMAGESCALER_H
#define CPIMIMAGESCALER_H

//  INCLUDES
#include <e32base.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>

// CONSTANTS
const TInt KThumbHeight = 96;
const TInt KThumbWidth = 80;
const TInt KMaxJpegSize = 23296; // 80*96*3+256

// CLASS DECLARATION

/**
 *  Scales bitmaps when necessary.
 *  The purpose of this class is to resize given bitmaps to thumbnail size
 * (80x96). If the given image is already smaller than 80x96, it is not scaled.
 *
 */
NONSHARABLE_CLASS(CPIMImageScaler): public CActive
{
public: // type definitions
    enum TState
    {
        EReady,
        EOpening,
        EScaling,
        EEncoding,
        ECompleted,
        // Error state
        EError
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMImageScaler* NewL();

    /**
     * Destructor.
     */
    virtual ~CPIMImageScaler();

public: // New functions

    /**
     * Scales the bitmap to thumbnail size (80x96) if necessary.
     * The image is not scaled, if it already is smaller than necessary.
     *
     * @param aSrcImage Source image.
     * @return thumbnail sized full color JPEG as a descriptor. Caller takes
     * the ownership.
     */
    HBufC8* ScaleL(TDesC8& aSrcImage);

public: // CActive
    void RunL();
    void DoCancel();

private:

    /**
     * C++ default constructor.
     */
    CPIMImageScaler();

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Completes the operation
     * @param aError Error status or KErrNone if no error
     */
    void Complete(TInt aError);

    /**
     * Checks the given size. If the size is larger than required,
     * returns true
     */
    TBool CheckSize(TSize aSize);

    /**
     * Encodes the image
     */
    void Encode();

private: // Data

    // Opens JPEG images from a descriptor, owned
    CImageDecoder* iImageReader;

    // Converts bitmap to a JPEG image, owned
    CImageEncoder* iImageWriter;

    // Scales bitmaps,owned
    CBitmapScaler* iBitmapScaler;

    // Holds the encoded image. Owned while processing the image
    // when the image is returned, this pointer is changed to NULL.
    HBufC8* iImage;

    // Holds the bitmap. Owned.
    CFbsBitmap* iBitmap;

    // Waits for the operation to complete. Owned.
    CActiveSchedulerWait* iWait;

    // Current state
    TState iState;

    // Error code, propagated upwards on error
    TInt iError;

    // Do we have a connection to Font and Bitmap Server
    TBool iFbsSessionConnected;

    RFs iFileServer;

};

#endif // CPIMIMAGESCALER_H
// End of File
