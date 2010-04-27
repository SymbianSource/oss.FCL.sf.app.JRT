/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Java startup screen model.
 *
 */

#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <e32base.h>
#include <coecntrl.h>
#include "javacoreuiparams.h"


class CImageDecoder;
class CImageEncoder;


NONSHARABLE_CLASS(CStartScreen) : public CActive
{
// Types
public:
    enum TStartScreenType
    {
        EStartScreenAutomatic = 0,
        EStartScreenUserDefined = 1
    };

private:
    enum TImageId
    {
        EImageDefault = 0,
        EImagePortrait,
        EImageLandscape
    };

    enum TState
    {
        EIdle = 0,
        ESyncRead,
        EAsyncWrite
    };

// Own methods
public:
    /**
     * Creates a new instance of a start screen.
     * @param aType One of TStartScreenType.
     * @param aParams Core Ui startup parameters.
     */
    static CStartScreen* NewL(TStartScreenType aType,
                              const java::ui::CoreUiParams& aParams);

    /**
     * Destructor.
     */
    virtual ~CStartScreen();

    /**
     * Draws the start screen image.
     * @param aGc The graphics context.
     * @param aRect The bounds where the image should be drawn.
     */
    void Draw(CWindowGc& aGc,
              const TRect& aRect) const;

    /**
     * The UI has rotated the orientation.
     * The start screen should load the coresponding start image.
     */
    void HandleResolutionChange();

    /**
     * Check if there is an image to show in the active orientation.
     */
    TBool IsEmpty() const;

    /**
     * Get start image size, if any.
     */
    TSize Size() const;

    /**
     * Take a screenshot of the ui and save it to disk.
     */
    void AsyncSaveScreenL(TCallBack aCallback);

    /**
     * Get the type of start screen.
     */
    TStartScreenType Type() const;

    /**
     * Load the time it took for the midlet to start on the previous launch.
     */
    TInt64 LoadStartupTime();

    /**
     * Store the time it took for the midlet to launch.
     */
    void SaveStartupTime(TInt64 aTime);

// From CActive
protected:
    void RunL();
    void DoCancel();

// Own methods
private:
    /**
     * Constructor.
     */
    CStartScreen(TStartScreenType aType,
                 const java::ui::CoreUiParams& aParams);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Constructs mPath based on CoreUiParams
     */
    void InitPathL();

    /**
     * Check if folder exists and count the images.
     */
    void ScanFolderL();

    /**
     * Attempts to load from disk the start image.
     * @param aImage one of TImageId
     */
    void SyncLoadImageL(TImageId aImage);

    /**
     * Load an image from disk.
     * @param aResBmp The resulting bitmap.
     *      Ownership transfered to caller.
     * @param aResMask The resulting mask.
     *      Ownership transfered to caller.
     */
    void DoSyncLoadImageL(const TDesC& aPath,
                          CFbsBitmap*& aResBmp,
                          CFbsBitmap*& aResMask);

    /**
     * Replace the start image on the disk.
     * @param aImage one of TImageId.
     * @param aBmp The new bitmap to be saved as start image.
     */
    void AsyncSaveImageL(TImageId aImage, const CFbsBitmap& aBmp);

    /**
     * Save an image to disk.
     * @param aPath Path on disk.
     * @param aBmp The bitmap to save.
     */
    void DoAsyncSaveImageL(const TDesC& aPath, const CFbsBitmap& aBmp);

    /**
     * Returns size for a bitmap that is scaled to fit inside aMaxDestSize
     * if aSourceSize > aMaxDestSize. The scaling is done so that aspect
     * ratio of the result is the same as in source.
     * If aSourceSize < aMaxDestSize, no scaling is done.
     */
    TSize GetAspectRatioScaledBitmapSize(const TSize& aSourceSize,
                                         const TSize& aMaxDestSize) const;

    /**
     * Copies the screen into a bitmap regardless of which app is focused.
     */
    CFbsBitmap* TakeScreenShotL();

    /**
     * Get the folder of the start image based on the type of the start screen.
     */
    const TDesC& FolderName() const;

    /**
     * Get the start image name based on the type of the start screen.
     */
    const TDesC& ImageName(TImageId aId) const;

    /**
     * Get the full path to the startup time file.
     */
    std::string StartupTimeFileName() const;

// Data
private:
    TStartScreenType mType;
    const java::ui::CoreUiParams& mParams;
    RBuf mPath;
    TInt mImageCount;

    // Portrait image data.
    CFbsBitmap* mPortraitBmp;
    CFbsBitmap* mPortraitMask;

    // Landscape image data.
    CFbsBitmap* mLandscapeBmp;
    CFbsBitmap* mLandscapeMask;

    // This is to identify image reading / writing states.
    TState mState;

    // Sync image reading data.
    CImageEncoder* mEncoder;
    CActiveSchedulerWait mWait;

    // Async image writing data.
    CFbsBitmap* mAsyncSaveBmp;
    RBuf mAsyncSavePath;
    CImageDecoder* mDecoder;
    TCallBack mCallBack;
};

#endif // STARTSCREEN_H

