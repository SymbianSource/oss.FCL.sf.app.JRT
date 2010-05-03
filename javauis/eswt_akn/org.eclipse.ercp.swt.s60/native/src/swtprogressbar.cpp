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


#include <avkon.mbg>
#include <AknsUtils.h>
#include <barsread.h>
#include <aknconsts.h>
#include <gulicon.h>
#include <eikprogi.h>
#include <swtlaffacade.h>
#include <avkon.rsg>
#include <AknBitmapAnimation.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include "swtrotateimage.h"
#include "swtfont.h"
#include "swtprogressbar.h"

// ======== MEMBER FUNCTIONS ========

TInt CSwtProgressBar::AnimationCallBack(TAny* aPtr)
{
    CSwtProgressBar* progressBar = static_cast<CSwtProgressBar*>(aPtr);

    if (progressBar)
    {
        progressBar->iImageIndex++;
        if (progressBar->iImageIndex>=progressBar->iBarImages.Count())
        {
            progressBar->iImageIndex=0;
        }
        // Intentionally not drawing while urgent painting
        progressBar->Paint(TRect(progressBar->CoeControl().PositionRelativeToScreen(),
                                 progressBar->GetWidgetSize()));
    }

    return KErrNone;
}

/**
 * 1st & 2nd phase constructors wrapper
 */
CSwtProgressBar* CSwtProgressBar::NewL(MSwtDisplay& aDisplay,
                                       TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle)
{
    CSwtProgressBar* self = new(ELeave) CSwtProgressBar(aDisplay, aPeer, aParent, aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    CleanupStack::Pop(self);
    return self;
}

CSwtProgressBar::CSwtProgressBar(MSwtDisplay& aDisplay,
                                 TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle)
        : ASwtControlBase(aDisplay, aPeer, &aParent, aStyle)
        , iMinimum(KDefaultProgressBarMinValue)
        , iMaximum(KDefaultProgressBarMaxValue)
        , iSelection(KDefaultProgressBarSelectionValue)
{
    SetComponentsToInheritVisibility(ETrue);

    SetFocusing(EFalse);
}


CSwtProgressBar::~CSwtProgressBar()
{
    DeleteImages();
    iImagesToDelete.Close();

    iBarImages.Close();
    iBarMasks.Close();

    delete iImagesRotator;
    delete iAnimationPeriodic;

#ifdef _lint
    iBarFrameLeftImage  = NULL;
    iBarFrameLeftMask   = NULL;
    iBarFrameRightImage = NULL;
    iBarFrameRightMask  = NULL;
    iBarFrameCenterImage= NULL;
    iBarFrameCenterMask = NULL;
    iImagesRotator      = NULL;
    iAnimationPeriodic  = NULL;
#endif
}

/**
 * 2nd phase constructor
 */
void CSwtProgressBar::ConstructL()
{
    // Get parent
    CCoeControl& coeParent = iParent->Control()->CoeControl();

    // Synchronize with parent's state
    SetContainerWindowL(coeParent);

    if (iStyle & KSwtStyleVertical)
    {
        iImagesRotator = CAORotateImage::NewL();
    }

    LoadImagesL();

    // Synchronize with parent's state
    CAknControl::MakeVisible(coeParent.IsVisible());
    CAknControl::SetDimmed(coeParent.IsDimmed());

    SetBackground(this);   // Back will be drawn by ASwtControlBase::Draw

    ActivateL();
}

void CSwtProgressBar::LoadImagesL()
{
    TFileName avkonbmpFilename(AknIconUtils::AvkonIconFileName());
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CFbsBitmap* bitmap  = NULL;
    CFbsBitmap* mask    = NULL;

    if (iImagesRotator)
    {
        iImagesRotator->Cancel();
        iImagesRotator->RemoveAllImages();
    }

    DeleteImages();

    if (iStyle & KSwtStyleSmooth)
    {
        AknsUtils::CreateIconL(skin, KAknsIIDQgnGrafBarProgress, bitmap, mask,
                               KAvkonBitmapFile, EMbmAvkonQgn_graf_bar_progress, EMbmAvkonQgn_graf_bar_progress_mask);

        iImagesToDelete.Append(bitmap);
        iImagesToDelete.Append(mask);
        User::LeaveIfError(iBarImages.Append(bitmap));
        User::LeaveIfError(iBarMasks.Append(mask));
    }
    else
    {
        // If no specific style for the ProgressBar is speficied so use the Indeterminate style.
        ASSERT(iStyle & KSwtStyleIndeterminate);

        TInt aResourceId = R_QGN_GRAF_WAIT_BAR_ANIM;

        CAknBitmapAnimation* aknAnimation = CAknBitmapAnimation::NewL();
        aknAnimation ->SetScaleModeForAnimationFrames(EAspectRatioNotPreserved);

        TAknsItemID iid;
        iid.Set(EAknsMajorAvkon, aResourceId);
        if (!aknAnimation ->ConstructFromSkinL(iid))
        {
            TResourceReader readerForAnimation;
            iDisplay.CoeEnv()->CreateResourceReaderLC(readerForAnimation,aResourceId);
            aknAnimation->ConstructFromResourceL(readerForAnimation);
            CleanupStack::PopAndDestroy();
        }

        TInt endFrame = aknAnimation ->BitmapAnimData()->FrameArray().Count()-1;
        iFrameIntervalInMilliSeconds = aknAnimation ->BitmapAnimData()->FrameIntervalInMilliSeconds();

        for (TInt i=0; i<=endFrame; i++)
        {
            aknAnimation ->BitmapAnimData()->FrameArray().At(i)->SetBitmapsOwnedExternally(ETrue);
            bitmap  = aknAnimation ->BitmapAnimData()->FrameArray().At(i)->Bitmap();
            mask = aknAnimation ->BitmapAnimData()->FrameArray().At(i)->Mask();

            iImagesToDelete.Append(bitmap);
            iImagesToDelete.Append(mask);

            User::LeaveIfError(iBarImages.Append(bitmap));
            if (mask)
            {
                User::LeaveIfError(iBarMasks.Append(mask));
            }
        }

        delete aknAnimation;
    }

    // The long frame bitmap.
    AknsUtils::CreateIconL(skin, KAknsIIDQgnGrafBarFrameSideL,
                           iBarFrameLeftImage, iBarFrameLeftMask, KAvkonBitmapFile,
                           EMbmAvkonQgn_graf_bar_frame_side_l,
                           EMbmAvkonQgn_graf_bar_frame_side_l_mask);
    iImagesToDelete.Append(iBarFrameLeftImage);
    iImagesToDelete.Append(iBarFrameLeftMask);

    AknsUtils::CreateIconL(skin, KAknsIIDQgnGrafBarFrameCenter,
                           iBarFrameCenterImage, iBarFrameCenterMask, KAvkonBitmapFile,
                           EMbmAvkonQgn_graf_bar_frame_center,
                           EMbmAvkonQgn_graf_bar_frame_center_mask);
    iImagesToDelete.Append(iBarFrameCenterImage);
    iImagesToDelete.Append(iBarFrameCenterMask);


    AknsUtils::CreateIconL(skin, KAknsIIDQgnGrafBarFrameSideR,
                           iBarFrameRightImage, iBarFrameRightMask, KAvkonBitmapFile,
                           EMbmAvkonQgn_graf_bar_frame_side_r,
                           EMbmAvkonQgn_graf_bar_frame_side_r_mask);
    iImagesToDelete.Append(iBarFrameRightImage);
    iImagesToDelete.Append(iBarFrameRightMask);

    if (iStyle & KSwtStyleIndeterminate && IsVisible())
    {
        StartAnimation();
    }
}

void CSwtProgressBar::DeleteImages()
{
    StopAnimation();

    for (TInt i=0; i < iImagesToDelete.Count(); i++)
    {
        delete iImagesToDelete[i];
        iImagesToDelete[i] = NULL;
    }
    iImagesToDelete.Reset();
    iBarImages.Reset();
    iBarMasks.Reset();
}


void CSwtProgressBar::UpdateSelection()
{
    if (iSelection > iMaximum)
    {
        iSelection = iMaximum;
    }
    else if (iSelection < iMinimum)
    {
        iSelection = iMinimum;
    }

    if ((iStyle & KSwtStyleIndeterminate) == 0)
    {
        Redraw();
    }
}

void CSwtProgressBar::AddBitmapToRotator(CFbsBitmap*& aBitmap)
{
    if (iImagesRotator && aBitmap)
    {
        TInt error = KErrNone;
        if (IsBitmapConversionNeeded(aBitmap))
        {
            if (aBitmap->IsCompressedInRAM())
            {
                AknIconUtils::DisableCompression(aBitmap);
            }

            if (IsBitmapConversionNeeded(aBitmap))
            {
                TRAP(error, (aBitmap = CreatePlainBitmapL(aBitmap)));

                if (error == KErrNone)
                    iImagesToDelete.Append(aBitmap);
            }
        }

        if (error == KErrNone)
            iImagesRotator->AddImage(aBitmap);
    }
}

TBool CSwtProgressBar::IsBitmapConversionNeeded(CFbsBitmap* aBitmap) const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    return aBitmap->IsCompressedInRAM()
           || aBitmap->ExtendedBitmapType() != TUid::Null();
#else
    return aBitmap->IsCompressedInRAM();
#endif
}

// The CBitmapRotator which do the rotation of the Bitmap doesn't
// work with
// a) compressed bitmaps
//   A assert stop in CFbsBitmap::GetVerticalScanLine during the rotation,
//   but the documentation say this should works with
//   compressed and uncompressed bitmap 0-:
// b) extended bitmaps
//   When extended bitmap is passed to rotator, it fails with error
CFbsBitmap* CSwtProgressBar::CreatePlainBitmapL(CFbsBitmap* aSrcBitmap) const
{
    CFbsBitmap* trgBitmap = aSrcBitmap;
    if (IsBitmapConversionNeeded(aSrcBitmap))
    {
        // Create target uncompressed bitmap
        trgBitmap = new(ELeave) CFbsBitmap();
        CleanupStack::PushL(trgBitmap);
        trgBitmap->Create(aSrcBitmap->SizeInPixels(), aSrcBitmap->DisplayMode());

        // create bitmap device
        CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(trgBitmap);
        CleanupStack::PushL(bitmapDevice);

        // create graphic context
        CFbsBitGc* bitmapGc = NULL;
        bitmapDevice->CreateContext(bitmapGc);
        CleanupStack::PushL(bitmapGc);

        // blit old bitmap into new
        bitmapGc->BitBlt(TPoint(0,0),aSrcBitmap,TRect(aSrcBitmap->SizeInPixels()));

        // clear stack
        CleanupStack::PopAndDestroy(bitmapGc);
        CleanupStack::PopAndDestroy(bitmapDevice);
        CleanupStack::Pop(trgBitmap);
    }

    ASSERT(!trgBitmap->IsCompressedInRAM());
#ifdef RD_JAVA_S60_RELEASE_9_2
    ASSERT(trgBitmap->ExtendedBitmapType() == TUid::Null());
#endif
    return trgBitmap;
}


TSize CSwtProgressBar::LayoutPreferedSizeSize() const
{
    TInt w(CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EESwtCtrlWaitPane,
                                        TRect(0, 0, 0, 0), 0).Rect().Width());
    TInt h(CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EESwtCtrlWaitPaneH,
                                        TRect(0, 0, 0, 0), 0).Rect().Height());
    return TSize(w, h);
}


//
// Virtual methods from CCoeControl
//

TSize CSwtProgressBar::MinimumSize()
{
    TSize preferedSize(LayoutPreferedSizeSize());

    if (iStyle & KSwtStyleVertical)
    {
        preferedSize = TSize(preferedSize.iHeight, preferedSize.iWidth);
    }

    return BorderOuterRect(preferedSize).Size();
}

void CSwtProgressBar::SetDimmed(TBool aDimmed)
{
    CAknControl::SetDimmed(aDimmed);

    FocusabilityChanged();
}


void CSwtProgressBar::MakeVisible(TBool aVisible)
{
    CAknControl::MakeVisible(aVisible);

    if (iStyle & KSwtStyleIndeterminate)
    {
        if (aVisible)
            StartAnimation();
        else
            StopAnimation();
    }

    FocusabilityChanged();
}

void CSwtProgressBar::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(SwtHandleResourceChangeL(aType));
}


void CSwtProgressBar::SwtHandleResourceChangeL(TInt aType)
{
    CAknControl::HandleResourceChange(aType);

    // Update the Control if the Skin Change
    if (aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch)
    {
        if (aType == KAknsMessageSkinChange)
        {
            LoadImagesL();
            SizeChanged();
        }
    }
}


void CSwtProgressBar::FocusChanged(TDrawNow aDrawNow)
{
    CAknControl::FocusChanged(ENoDrawNow);
    // Spread focus information
    HandleFocusChanged(aDrawNow);
}


void CSwtProgressBar::SizeChanged()
{
    TRect innerRect(BorderInnerRect());
    TAknLayoutRect frameLayout;

    innerRect.SetSize(TSize(Max(innerRect.Width(), 1), Max(innerRect.Height(), 1)));

    if (iImagesRotator)
    {
        innerRect.SetSize(TSize(innerRect.Height(), innerRect.Width()));

        iImagesRotator->RemoveAllImages();
        DeleteImages();

        TRAP_IGNORE(LoadImagesL());
    }

    SetImagesSize(innerRect.Size());

    if (iImagesRotator)
    {
        AddAllImagesToRotate();
        //Active object already cancelled at this point
        iImagesRotator->Start(this);
    }

    HandleSizeChanged();
}

void CSwtProgressBar::PositionChanged()
{
    HandlePositionChanged();
}

TTypeUid::Ptr CSwtProgressBar::MopSupplyObject(TTypeUid aId)
{
    TTypeUid::Ptr id = ASwtControlBase::SwtMopSupplyObject(aId);

    if (id.Pointer() == NULL)
    {
        return CAknControl::MopSupplyObject(aId);
    }
    else
    {
        return id;
    }
}

void  CSwtProgressBar::Draw(const TRect& /*aRect*/) const
{
    if (iImagesRotator)
    {
        if (!iImagesRotator->IsFinish())
        {
            iImagesRotator->SetRedrawAfterRotation(ETrue);
            return;
        }
    }

    CWindowGc& gc = SystemGc();

    // Draw the image of the ProgressBar.
    TRect innerRect(BorderInnerRect());

    DrawProgressBar(gc, innerRect);

    DrawFrame(gc, innerRect);
}

void CSwtProgressBar::DrawProgressBar(CWindowGc& gc, TRect targetRect) const
{
    CFbsBitmap* bitmap      = iBarImages[iImageIndex];
    CFbsBitmap* bitmapMask  = iBarMasks[iImageIndex];


    // If this is the Smooth style
    if (iStyle & KSwtStyleSmooth)
    {
        // Fill area with background to apears empty.
        // Calculate the numbers of pixels which must be fill.
        TInt filledPixels = (iStyle & KSwtStyleVertical)?(targetRect.Height()):(targetRect.Width());

        // Indeterminate style progressbar always fills the whole space,
        // regardless of selection.
        if ((iStyle & KSwtStyleIndeterminate) == 0)
        {
            filledPixels = filledPixels * (iSelection-iMinimum) / (iMaximum - iMinimum);
        }

        if (iStyle & KSwtStyleVertical)
        {
            targetRect.Move(0, targetRect.Height() - filledPixels);
            targetRect.SetHeight(filledPixels);
        }
        else
        {
            TInt origWidth = targetRect.Width();
            targetRect.SetWidth(filledPixels);
            if (AknLayoutUtils::LayoutMirrored())
            {
                targetRect.Move(origWidth-filledPixels, 0);
            }
        }
    }

    // Left of the frame
    gc.BitBltMasked(targetRect.iTl, bitmap, targetRect.Size(), bitmapMask, EFalse);
}

void CSwtProgressBar::DrawFrame(CWindowGc& gc,const TRect& targetRect) const
{
    // Draw the Long Bar Frame.
    if (iBarFrameLeftImage && iBarFrameCenterImage && iBarFrameRightImage)
    {
        TPoint leftPos(TPoint::EUninitialized);
        TPoint centerPos(TPoint::EUninitialized);
        TPoint rightPos(TPoint::EUninitialized);

        TSize lSize(iBarFrameLeftImage->SizeInPixels());
        TSize rSize(iBarFrameRightImage->SizeInPixels());
        TSize cSize(iBarFrameCenterImage->SizeInPixels());

        if (iStyle & KSwtStyleVertical)
        {
            rightPos = targetRect.iTl;
            centerPos = rightPos + TPoint(0, iBarFrameRightImage->SizeInPixels().iHeight);
            leftPos = centerPos + TPoint(0, iBarFrameCenterImage->SizeInPixels().iHeight);
        }
        else
        {
            leftPos = targetRect.iTl;
            centerPos = leftPos + TPoint(iBarFrameLeftImage->SizeInPixels().iWidth, 0);
            rightPos = centerPos + TPoint(iBarFrameCenterImage->SizeInPixels().iWidth, 0);
        }

        // Left of the frame
        gc.BitBltMasked(leftPos, iBarFrameLeftImage, iBarFrameLeftImage->SizeInPixels(), iBarFrameLeftMask, EFalse);

        // Center of the frame
        gc.BitBltMasked(centerPos, iBarFrameCenterImage, iBarFrameCenterImage->SizeInPixels(), iBarFrameCenterMask, EFalse);

        // Right of the frame
        gc.BitBltMasked(rightPos, iBarFrameRightImage, iBarFrameRightImage->SizeInPixels(), iBarFrameRightMask, EFalse);
    }
}
//
// Virtual methods from MSwtControl
//

CCoeControl& CSwtProgressBar::CoeControl()
{
    return *this;
}

const CCoeControl& CSwtProgressBar::CoeControl() const
{
    return *this;
}

TBool CSwtProgressBar::IsFocusable(TInt aReason /*=KSwtFocusByApi*/) const
{
    const CCoeControl& coeCtrl = CoeControl();

    // Invisible or disabled controls can never get the focus (not even forced)
    if (!coeCtrl.IsVisible() || coeCtrl.IsDimmed())
        return EFalse;
    else
        return aReason == KSwtFocusByForce;
}

void CSwtProgressBar::ProcessKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
{
    // Do nothing
}

TKeyResponse CSwtProgressBar::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    return HandleKeyL(aKeyEvent, aType, ETrue);
}

TSize CSwtProgressBar::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    TSize size(MinimumSize());

    if (aWHint != KSwtDefault)
    {
        size.iWidth = aWHint;
    }
    if (aHHint != KSwtDefault)
    {
        size.iHeight = aHHint;
    }

    return size;
}

//
// Virtual methods from MSwtProgressBar
//

MSwtControl* CSwtProgressBar::Control()
{
    return this;
}

TInt CSwtProgressBar::GetMaximum() const
{
    return iMaximum;
}

TInt CSwtProgressBar::GetMinimum() const
{
    return iMinimum;
}

TInt CSwtProgressBar::GetSelection() const
{
    return iSelection;
}

void CSwtProgressBar::SetMaximum(TInt aValue)
{
    if (aValue != iMaximum && aValue > iMinimum)
    {
        iMaximum = aValue;
        UpdateSelection();
    }
}

void CSwtProgressBar::SetMinimum(TInt aValue)
{
    if (aValue != iMinimum && aValue >= 0 && aValue < iMaximum)
    {
        iMinimum = aValue;
        UpdateSelection();
    }
}

void CSwtProgressBar::SetSelection(TInt aValue)
{
    if (aValue == iSelection)
    {
        return;
    }

    iSelection = aValue;

    UpdateSelection();
}

void CSwtProgressBar::StartAnimation()
{
    if ((iStyle & KSwtStyleIndeterminate) == 0)
        return;

    if (iAnimationPeriodic == NULL)
    {
        iAnimationPeriodic = CPeriodic::New(CActive::EPriorityStandard);
    }
    else
    {
        iAnimationPeriodic->Cancel();
    }

    TCallBack callback(AnimationCallBack, this);
    iAnimationPeriodic->Start(TTimeIntervalMicroSeconds32(iFrameIntervalInMilliSeconds*1000),
                              TTimeIntervalMicroSeconds32(iFrameIntervalInMilliSeconds*1000), callback);
}

void CSwtProgressBar::StopAnimation()
{
    if ((iStyle & KSwtStyleIndeterminate) == 0)
        return;

    if (iAnimationPeriodic)
    {
        iAnimationPeriodic->Cancel();
    }
}

void CSwtProgressBar::AddAllImagesToRotate()
{
    ASSERT(iBarImages.Count() == iBarMasks.Count());
    if (iStyle & KSwtStyleVertical)
    {
        for (TInt indexInImages = 0; indexInImages < iBarImages.Count(); indexInImages ++)
        {
            AddBitmapToRotator(iBarImages[indexInImages]);
            AddBitmapToRotator(iBarMasks[indexInImages]);
        }

        // If control size is (0,0), the frame images returned by the skin don't get a
        // bitmap associated with them. Adding a NULL bitmap to the rotator would cause
        // a crash, so check here that each image that we add has a bitmap.
        if (iBarFrameLeftImage->Handle() && iBarFrameLeftMask->Handle())
        {
            AddBitmapToRotator(iBarFrameLeftImage);
            AddBitmapToRotator(iBarFrameLeftMask);
        }

        if (iBarFrameCenterImage->Handle() && iBarFrameCenterMask->Handle())
        {
            AddBitmapToRotator(iBarFrameCenterImage);
            AddBitmapToRotator(iBarFrameCenterMask);
        }

        if (iBarFrameRightImage->Handle() && iBarFrameRightMask->Handle())
        {
            AddBitmapToRotator(iBarFrameRightImage);
            AddBitmapToRotator(iBarFrameRightMask);
        }
    }
}

void CSwtProgressBar::SetImagesSize(const TSize& aImageSize)
{
    TAknLayoutRect frameLayout;
    ASSERT(iBarImages.Count() == iBarMasks.Count());

    for (TInt i=0; i < iBarImages.Count(); i++)
    {
        AknIconUtils::SetSize(iBarImages[i], aImageSize, EAspectRatioNotPreserved);

        if (iBarMasks[i])
        {
            AknIconUtils::SetSize(iBarMasks[i], aImageSize, EAspectRatioNotPreserved);
        }
    }

    TSize leftSize(CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EWaitBorderPaneG1, aImageSize, 0).Rect().Size());
    TSize centerSize(CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EWaitBorderPaneG2, aImageSize, 0).Rect().Size());

    TSize rightSize(CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EWaitBorderPaneG3, aImageSize, 0).Rect().Size());

    AknIconUtils::SetSize(iBarFrameLeftImage, leftSize, EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iBarFrameLeftMask, leftSize, EAspectRatioNotPreserved);

    AknIconUtils::SetSize(iBarFrameCenterImage, centerSize, EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iBarFrameCenterMask, centerSize, EAspectRatioNotPreserved);

    AknIconUtils::SetSize(iBarFrameRightImage, rightSize, EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iBarFrameRightMask, rightSize, EAspectRatioNotPreserved);
}
