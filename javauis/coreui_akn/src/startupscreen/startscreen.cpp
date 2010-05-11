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
* Description: A user start screen impl.
*
*/

#include <iostream>
#include <fstream>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <layoutmetadata.cdl.h>
#include <imageconversion.h>

#include "startscreen.h"
#include "logger.h"
#include "javasymbianoslayer.h"
#include "coreuiavkonimpl.h"

_LIT(KUserFolder, "startupimages\\");
_LIT(KUserDefaultImage, "startupscreen.img");
_LIT(KUserLandscapeImage, "startupscreen_landscape.img");
_LIT(KUserPortraitImage, "startupscreen_portrait.img");

_LIT(KAutoFolder, "autostartupimages\\");
_LIT(KAutoLandscapeImage, "landscape.png"); // Gets prefixed by TUid
_LIT(KAutoPortraitImage, "portrait.png"); // Gets prefixed by TUid


CStartScreen* CStartScreen::NewL(CStartScreen::TStartScreenType aType,
                                 const java::ui::CoreUiParams& aParams)
{
    JELOG2(EJavaUI);
    CStartScreen* self = new(ELeave) CStartScreen(aType, aParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CStartScreen::~CStartScreen()
{
    JELOG2(EJavaUI);
    Cancel();
    delete mPortraitBmp;
    delete mPortraitMask;
    if (mPortraitBmp != mLandscapeBmp)
    {
        delete mLandscapeBmp;
        delete mLandscapeMask;
    }
    delete mAsyncSaveBmp;
    delete mDecoder;
    delete mEncoder;
    mPath.Close();
    mAsyncSavePath.Close();
}

void CStartScreen::Draw(CWindowGc& aGc,
                        const TRect& aRect) const
{
    JELOG2(EJavaUI);

    CFbsBitmap* bmp = 0;
    CFbsBitmap* mask = 0;
    if (Layout_Meta_Data::IsLandscapeOrientation())
    {
        bmp = mLandscapeBmp;
        mask = mLandscapeMask;
    }
    else
    {
        bmp = mPortraitBmp;
        mask = mPortraitMask;
    }

    if (!bmp)
    {
        return;
    }

    TPoint topLeft(aRect.iTl);
    if (mType == EStartScreenAutomatic)
    {
        aGc.BitBlt(topLeft, bmp);
    }
    else
    {
        const TSize bmpSizeOrg(bmp->SizeInPixels());
        const TSize rectSize(aRect.Size());
        TSize bmpSize(bmpSizeOrg);

        // Scale the image if needed
        TBool scaled(EFalse);
        if (bmpSize.iWidth > rectSize.iWidth || bmpSize.iHeight > rectSize.iHeight)
        {
            scaled = ETrue;
            bmpSize = GetAspectRatioScaledBitmapSize(bmpSize, rectSize);
        }

        // Center the image
        if (bmpSize.iWidth < rectSize.iWidth)
        {
            topLeft.iX = (rectSize.iWidth - bmpSize.iWidth) / 2;
        }
        if (bmpSize.iHeight < rectSize.iHeight)
        {
            topLeft.iY = (rectSize.iHeight - bmpSize.iHeight) / 2;
        }

        if (scaled)
        {
            TRect destRect(topLeft, bmpSize);
            if (!mask)
            {
                aGc.DrawBitmap(destRect, bmp, TRect(bmpSizeOrg));
            }
            else
            {
                aGc.DrawBitmapMasked(destRect, bmp, TRect(bmpSizeOrg), mask, EFalse);
            }
        }
        else
        {
            // Faster drawing
            if (!mask)
            {
                aGc.BitBlt(topLeft, bmp);
            }
            else
            {
                aGc.BitBltMasked(topLeft, bmp, TRect(bmpSize), mask, EFalse);
            }
        }
    }
}

void CStartScreen::HandleResolutionChange()
{
    if (mLandscapeBmp && mPortraitBmp)
    {
        return;
    }

    if (Layout_Meta_Data::IsLandscapeOrientation())
    {
        if (!mLandscapeBmp)
        {
            TRAP_IGNORE(SyncLoadImageL(EImageLandscape));
        }
    }
    else
    {
        if (!mPortraitBmp)
        {
            TRAP_IGNORE(SyncLoadImageL(EImagePortrait));
        }
    }
}

TBool CStartScreen::IsEmpty() const
{
    if (Layout_Meta_Data::IsLandscapeOrientation())
    {
        return (mLandscapeBmp == 0);
    }
    else
    {
        return (mPortraitBmp == 0);
    }
}

TSize CStartScreen::Size() const
{
    if (Layout_Meta_Data::IsLandscapeOrientation())
    {
        if (mLandscapeBmp)
            return mLandscapeBmp->SizeInPixels();
    }
    else
    {
        if (mPortraitBmp)
            return mPortraitBmp->SizeInPixels();
    }

    return TSize(0, 0);
}

void CStartScreen::AsyncSaveScreenL(TCallBack aCallBack)
{
    // Expecting this is done once and only once.
    ASSERT(!mAsyncSaveBmp);
    ASSERT(!mEncoder);

    mCallBack = aCallBack;
    mAsyncSaveBmp = TakeScreenShotL();
    if (mAsyncSaveBmp)
    {
        if (Layout_Meta_Data::IsLandscapeOrientation())
        {
            AsyncSaveImageL(EImageLandscape, *mAsyncSaveBmp);
        }
        else
        {
            AsyncSaveImageL(EImagePortrait, *mAsyncSaveBmp);
        }
    }
}

CStartScreen::TStartScreenType CStartScreen::Type() const
{
    return mType;
}

TInt64 CStartScreen::LoadStartupTime()
{
    TInt64 res = 0;
    std::ifstream file;
    file.open(StartupTimeFileName().c_str(), std::ifstream::in);
    file >> res;
    file.close();
    return res;
}

void CStartScreen::SaveStartupTime(TInt64 aPeriod)
{
    std::ofstream file;
    file.open(StartupTimeFileName().c_str());
    file << aPeriod;
    file.close();
}

CStartScreen::CStartScreen(TStartScreenType aType,
                           const java::ui::CoreUiParams& aParams)
        : CActive(EPriorityStandard), mType(aType), mParams(aParams)
{
    CActiveScheduler::Add(this);
}

void CStartScreen::ConstructL()
{
    JELOG2(EJavaUI);

    InitPathL();
    ScanFolderL();

    if (mImageCount > 0)
    {
        if (mType == EStartScreenAutomatic || mImageCount >= 2)
        {
            if (Layout_Meta_Data::IsLandscapeOrientation())
            {
                TRAP_IGNORE(SyncLoadImageL(EImageLandscape));
            }
            else
            {
                TRAP_IGNORE(SyncLoadImageL(EImagePortrait));
            }
        }
        else
        {
            TRAP_IGNORE(SyncLoadImageL(EImageDefault));
        }
    }
}

void CStartScreen::InitPathL()
{
    HBufC* root(wstringToBuf(mParams.getImagePath()));
    if (!root || root->Length() == 0)
    {
        delete root;
        User::Leave(KErrPathNotFound);
    }
    CleanupStack::PushL(root);
    const TDesC& folder = FolderName();
    mPath.CreateL(root->Length() + folder.Length());
    mPath.Append(root->Des());
    CleanupStack::PopAndDestroy(root);
    mPath.Append(folder);
}

void CStartScreen::ScanFolderL()
{
    ASSERT(mPath.Length());

    TUint fileAttrMask = KEntryAttNormal | KEntryAttSystem | KEntryAttDir;
    CDir* dir = 0;
    RFs& fs = CCoeEnv::Static()->FsSession();
    TInt err = fs.GetDir(mPath, fileAttrMask, ESortByName, dir);

    if (KErrNone == err)
    {
        if (!dir)
        {
            err = KErrNotFound;
            ELOG1(EJavaUI, "CStartScreen::InitStateL, "
                  "RFs::GetDir failed: %d", err);
        }
        else
        {
            mImageCount = dir->Count();
        }
    }
    else
    {
        ELOG1(EJavaUI, "CStartScreen::InitStateL, "
              "RFs::GetDir failed: %d", err);
    }

    if (mType == EStartScreenAutomatic && KErrNone != err)
    {
        err = fs.MkDir(mPath);
    }

    User::LeaveIfError(err);
}

void CStartScreen::SyncLoadImageL(TImageId aId)
{
    RBuf fullPath;
    const TDesC& img = ImageName(aId);
    // Do prefixing here just to avoid dealing with another RBuf :)
    TUidName uid;
    if (mType == EStartScreenAutomatic)
        uid = java::ui::CoreUiAvkonImpl::getInstanceImpl().getMidletTUid().Name();
    fullPath.CreateL(mPath.Length() + img.Length() + uid.Length());
    fullPath.CleanupClosePushL();
    fullPath.Append(mPath);
    if (mType == EStartScreenAutomatic)
        fullPath.Append(uid);
    fullPath.Append(img);

    switch (aId)
    {
    case EImageDefault:
        ASSERT(!mPortraitBmp);
        ASSERT(!mLandscapeBmp);
        DoSyncLoadImageL(fullPath, mPortraitBmp, mPortraitMask);
        mLandscapeBmp = mPortraitBmp;
        mLandscapeMask = mPortraitMask;
        break;
    case EImagePortrait:
        ASSERT(!mPortraitBmp);
        DoSyncLoadImageL(fullPath, mPortraitBmp, mPortraitMask);
        break;
    case EImageLandscape:
        ASSERT(!mLandscapeBmp);
        DoSyncLoadImageL(fullPath, mLandscapeBmp, mLandscapeMask);
        break;
    default:
        break;
    }

    CleanupStack::PopAndDestroy(&fullPath);
}


void CStartScreen::DoSyncLoadImageL(const TDesC& aPath, CFbsBitmap*& aResBmp,
                                    CFbsBitmap*& aResMask)
{
    ASSERT(!aResBmp);
    ASSERT(!aResMask);

    if (IsActive())
    {
        Cancel();
    }

    if (mDecoder != NULL)
    {
        delete mDecoder;
        mDecoder = 0;
    }

    mDecoder = CImageDecoder::FileNewL(CCoeEnv::Static()->FsSession(), aPath);

    CFbsBitmap* bmp = new(ELeave) CFbsBitmap();
    CleanupStack::PushL(bmp);

    const TFrameInfo& frameInfo = mDecoder->FrameInfo();

    TInt err = bmp->Create(frameInfo.iOverallSizeInPixels,
                           frameInfo.iFrameDisplayMode);
    if (KErrNone != err)
    {
        CleanupStack::PopAndDestroy(bmp);
        ELOG1(EJavaUI, "CStartScreen::DoSyncLoadImageL, "
              "Bitmap creation failed: %d", err);
        User::Leave(err);
    }

    CFbsBitmap* mask = 0;

    if (!(frameInfo.iFlags & TFrameInfo::ETransparencyPossible))
    {
        mState = ESyncRead;
        mDecoder->Convert(&iStatus, *bmp);
    }
    else
    {
        mask = new(ELeave) CFbsBitmap();
        CleanupStack::PushL(mask);

        TDisplayMode mode(EGray2);
        if (frameInfo.iFlags & TFrameInfo::EAlphaChannel)
        {
            mode = EGray256;
        }

        err = mask->Create(frameInfo.iOverallSizeInPixels, mode);
        if (KErrNone != err)
        {
            CleanupStack::PopAndDestroy(mask);
            CleanupStack::PopAndDestroy(bmp);
            ELOG1(EJavaUI, "CImageReader::DoSyncLoadImageL, "
                  "Mask creation failed: %d", err);
            User::Leave(err);
        }

        mState = ESyncRead;
        mDecoder->Convert(&iStatus, *bmp, *mask);
    }

    SetActive();
    mWait.Start();

    err = iStatus.Int();
    if (KErrNone != err)
    {
        if (mask)
        {
            CleanupStack::PopAndDestroy(mask);
        }
        CleanupStack::PopAndDestroy(bmp);
        ELOG1(EJavaUI, "CImageReader::DoSyncLoadImageL, "
              "CImageDecoder::Convert failed: %d", err);
        User::Leave(err);
    }

    if (mask)
    {
        CleanupStack::Pop(mask);
    }
    CleanupStack::Pop(bmp);
    aResBmp = bmp;
    aResMask = mask;
}

void CStartScreen::AsyncSaveImageL(TImageId aId, const CFbsBitmap& aBmp)
{
    const TDesC& img = ImageName(aId);
    // Do prefixing here just to avoid dealing with another RBuf :)
    TUidName uid;
    if (mType == EStartScreenAutomatic)
        uid = java::ui::CoreUiAvkonImpl::getInstanceImpl().getMidletTUid().Name();
    mAsyncSavePath.CreateL(mPath.Length() + img.Length() + uid.Length());
    mAsyncSavePath.Append(mPath);
    if (mType == EStartScreenAutomatic)
        mAsyncSavePath.Append(uid);
    mAsyncSavePath.Append(img);

    DoAsyncSaveImageL(mAsyncSavePath, aBmp);
}

void CStartScreen::DoAsyncSaveImageL(const TDesC& aPath, const CFbsBitmap& aBmp)
{
    if (IsActive())
    {
        Cancel();
    }

    mEncoder = CImageEncoder::FileNewL(CCoeEnv::Static()->FsSession(), aPath,
                                       CImageEncoder::EOptionNone, KImageTypePNGUid);

    mState = EAsyncWrite;
    mEncoder->Convert(&iStatus, aBmp);
    SetActive();
}

TSize CStartScreen::GetAspectRatioScaledBitmapSize(
    const TSize& aSourceSize, const TSize& aMaxDestSize) const
{
    JELOG2(EJavaUI);
    TSize imageSize = aSourceSize;
    TInt yDiff = 0;
    TInt xDiff = 0;
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
            imageSize.iHeight = (aSourceSize.iHeight * aMaxDestSize.iWidth)
                                / aSourceSize.iWidth;
        }
        else if (yDiff > xDiff)
        {
            imageSize.iHeight = aMaxDestSize.iHeight;
            imageSize.iWidth = (aSourceSize.iWidth * aMaxDestSize.iHeight)
                               / aSourceSize.iHeight;
        }
        else
        {
            // Aspect ratios are the same
            imageSize = aMaxDestSize;
        }
    }

    return imageSize;
}

CFbsBitmap* CStartScreen::TakeScreenShotL()
{
    CEikonEnv* env = CEikonEnv::Static();
    CFbsBitmap* bmp = new(ELeave) CFbsBitmap();
    CleanupStack::PushL(bmp);
    CWsScreenDevice* screenDevice = env->ScreenDevice();
    TInt err = bmp->Create(screenDevice->SizeInPixels(), screenDevice->DisplayMode());
    if (KErrNone != err)
    {
        ELOG1(EJavaUI, "CStartScreen::TakeScreenShotL, "
              "CFbsBitmap::Create failed: %d", err);
    }
    err = screenDevice->CopyScreenToBitmap(bmp);
    if (KErrNone != err)
    {
        ELOG1(EJavaUI, "CStartScreen::TakeScreenShotL, "
              "CWsScreenDevice::CopyScreenToBitmap failed: %d", err);
    }
    User::LeaveIfError(err);
    CleanupStack::Pop(bmp);
    return bmp;
}

const TDesC& CStartScreen::FolderName() const
{
    if (mParams.getScreenMode() == java::ui::USER_DEFINED_SCREEN)
    {
        return KNullDesC;
    }
    if (mType == EStartScreenAutomatic)
    {
        return KAutoFolder;
    }
    else
    {
        return KUserFolder;
    }
}

const TDesC& CStartScreen::ImageName(CStartScreen::TImageId aId) const
{
    if (mParams.getScreenMode() == java::ui::USER_DEFINED_SCREEN)
    {
        return KNullDesC;
    }
    if (mType == EStartScreenAutomatic)
    {
        switch (aId)
        {
        case EImagePortrait:
            return KAutoPortraitImage;
        case EImageLandscape:
            return KAutoLandscapeImage;
        default:
            return KNullDesC;
        }
    }
    else
    {
        switch (aId)
        {
        case EImagePortrait:
            return KUserPortraitImage;
        case EImageLandscape:
            return KUserLandscapeImage;
        case EImageDefault:
        default:
            return KUserDefaultImage;
        }
    }
}

std::string CStartScreen::StartupTimeFileName() const
{
    std::wstring temp(mParams.getImagePath());
    temp.append(L"autostartupimages\\");
    temp.append(java::ui::CoreUiAvkonImpl::getInstanceImpl().getMidletUid().toString());
    temp.append(L"time.txt");
    // This is ok as we know for sure we have only ascii in the path.
    std::string path(temp.begin(), temp.end());
    return path;
}

void CStartScreen::RunL()
{
    if (mState == ESyncRead)
    {
        if (mWait.IsStarted())
        {
            mWait.AsyncStop();
        }

        // Release the lock on the file.
        ASSERT(mDecoder);
        delete mDecoder;
        mDecoder = NULL;

        // Reset state
        mState = EIdle;
    }
    else if (mState == EAsyncWrite)
    {
        // Release the lock on the file.
        ASSERT(mEncoder);
        delete mEncoder;
        mEncoder = 0;

        // Reset state
        mState = EIdle;

        if (iStatus == KErrNone)
        {
            // The callback will delete the object in the current setup.
            mCallBack.CallBack();
        }
        else
        {
            TInt err = iStatus.Int();
            ELOG1(EJavaUI, "CStartScreen::RunL, "
                  "CImageEncoder::Convert failed: %d", err);
        }
    }
}

void CStartScreen::DoCancel()
{
    if (mState == ESyncRead)
    {
        if (mDecoder)
        {
            mDecoder->Cancel();
        }
        if (mWait.IsStarted())
        {
            mWait.AsyncStop();
        }
    }
    else if (mState == EAsyncWrite)
    {
        if (mEncoder)
        {
            mEncoder->Cancel();
        }
    }

    mState = EIdle;
}
