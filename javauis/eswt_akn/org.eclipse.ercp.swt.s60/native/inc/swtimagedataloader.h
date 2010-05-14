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


#ifndef SWTIMAGEDATALOADER_H
#define SWTIMAGEDATALOADER_H


#include "eswtgraphics.h"


class CImageDecoder;
class CImageEncoder;
class TFrameInfo;


/**
 * CSwtImageDataLoader
 * Class used to encode and decode images (PNG, GIF, JPG).
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtImageDataLoader)
        : public CActive
        , public MSwtImageDataLoader
{
// Own methods
public:

    /**
     * Two phase constructor.
     * Create the native object in charge of load and save image from/to a stream or a file.
     */
    static CSwtImageDataLoader* NewL();

// From MSwtImageDataLoader
public:
    void EncodeImageToFileL(MSwtImageData& aImageData,
                            TInt aFormat, const TDesC& aDestination);
    HBufC8* EncodeImageToStreamL(MSwtImageData& aImageData, TInt aFormat);
    void DecodeImageL(const TDesC& aFileName);
    void DecodeWholeImageFromBufferL(const TDesC8& aBuffer);
    void DecodeImageFromBufferL(const TDesC8& aBuffer);
    void AppendDataL(const TDesC8& aBuffer); //loading
    CSwtImageDataArray* GetImageData();
    void Dispose();
    TInt GetLogicalScreenHeight();
    TInt GetLogicalScreenWidth();

// Own methods
private:
    TBool DecodeNextFrameL();                 //starting
    void  DecodeFrameL(TInt aIndexOfFrame);   //starting

protected:
    /**
     * Destructor.
     */
    ~CSwtImageDataLoader();

private:
    CSwtImageDataLoader();
    void ConstructL();
    void SetImageDataL(MSwtImageData& aImageData);

    /**
     * Extract the ImageData from the members bitmap.
     */
    void ExtractImagedataL();

    /**
     * Build the mask bitmap if needed.
     */
    void SetMaskL(const TFrameInfo& aInfo);

    /**
     * Ensure that the bitmap resources are freed before
     * reusing the decoder (for multi frame image).
     */
    void FreeBitmaps();

    /**
     * Free buffers.
     */
    void FreeBuffers();

    /**
     * Set the logical values for height or width equal to
     * the values of the greatest frame (for height or width).
     */
    void SetLogicalScreenValues(const TFrameInfo& aInfo);

    /**
     * Set the x and y coordinates of the image in the current frame.
     */
    void SetFrameInfosL(const TFrameInfo& aInfo);

    /**
     * Leaves if a leave happens. Transforms the leave's code into one expected by eSWT.
     * A leave code that is not handled in the function causes
     * the leave to be performed with the unchanged leave code.
     * @param aLeaveCode : the leave code to convert(if equal to KerrNone : do not leave)
     */
    void LeaveIfErrorFromICLDecoderL(TInt aLeaveCode) const;


    /**
     * Leaves if a leave happens. Transforms the leave's code into one expected by eSWT.
     * A leave code that is not handled in the function causes
     * the leave to be performed with the unchanged leave code.
     * @param aLeaveCode : the leave code to convert(if equal to KerrNone : do not leave)
     */
    void LeaveIfErrorFromICLEncoderL(TInt aLeaveCode) const;

    /**
     * Verify the file pointed by destination can be created and does not already exists.
     * @param aDestination : The file that is checked
     * @param aFs : a RFs object.
     */
    void CheckDestinationL(const TDesC& aDestination, const RFs& aFs) const;

    const TDesC8& GetMimeType(TSwtImageType aFormat) const;

// From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    CFbsBitmap*       iMask;                // The possible mask of the image
    CFbsBitmap*       iBitmap;              // The image bitmap (without its mask)
    MSwtImageData::TInfo iInfo;
    TSize             iLogicalScreenValues; // LogicalScreenHeight and Width
    TPoint            iImageCoordinates;    // The coordinates of the frame within the screen in pixels.
    TInt              iDelay;               // The delay time of a gif
    TSwtGifDisposal   iDisposal;            // The disposal method of a gif image
    CImageDecoder*    iDecoder;             // Current decoder. Is a member to be persistent in order to be used asynchronously
    MSwtImageData*    iImageData;           // The imageData built when the conversion is finished
    CSwtImageDataArray* iImageDataArray;    // The imageData array built step by step depending on how many images are stored in the image file/stream
    TInt              iNextFrameToDecode;   // The next frame we will decode
    HBufC8*           iBuffer;              // The data to decode when loading or the data encoded when saving
    TBool             iStartDecodingAnother;// ETrue if a frame has just been decoded and ready to decode another frame.
    TPtr8             iDesc;                // The descriptor furnished to the Convert function
    TInt              iResult;              // The code returned to the caller.
    TBool             iWholeImageAtOnce;    // ETrue when decoding whole image in one shot
    CImageEncoder*    iEncoder;             // Current encoder. Is a member to be persistent in order to be used asynchronously
    CActiveScheduler* iActiveScheduler;     // Installed active scheduler.
    RFs               iFs;                  // File server session.
    TBool             iIsGif;
    TRequestStatus*   iImageStatus;
};


#endif // SWTIMAGEDATALOADER_H
