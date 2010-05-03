/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CMIDGaugeItem
*
*/


#include <coemain.h>
#include <e32std.h>
#include <eikcapc.h>
// CAknSlider - needed for iSlider member
#include <aknslider.h>
// macros for resources
#include <lcdui.rsg>
// using TMifAvkon enumeration
#include <avkon.mbg>
// Api for skin instance
// and creating icons
#include <AknsUtils.h>

// needed CEikProgressInfo API for iProgressInfo member
// control used to indicate the progress of an operation
#include <eikprogi.h>
#include <eikon.hrh>
#include <eikpanic.h>
// using TResourceReader class in ConstructSliderL (interactive gauge)
// interprets resource data read from a resource file
#include <barsread.h>
#include <gulbordr.h>
#include <eikenv.h>
// API use to retrieve info if pen is enabled (PenEnabled) in HandlePointerEventL function
#include <AknUtils.h> // for arabic texts support
#include <gulcolor.h>

// using AknsDrawUtils for drawing background
#include <AknsDrawUtils.h>
#include <applayout.cdl.h>
// LAF
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknDef.h> // For layout change event definitions

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

#include <j2me/jdebug.h>
#include "CMIDGaugeItem.h"
// API used for obtain info about command count for IsSelectable function
#include "CMIDCommandList.h"
// API for iLabelControl (inherited from CMIDControlItem)
#include "CMIDItemLabel.h"
// using TlsStruct for creating bitmap storage
#include "CMIDMenuHandler.h"
#include "CMIDUtils.h"
#include "CMIDUIManager.h"
// Using of CAknBitmapAnimation for iBitmapAnimation
#include <AknBitmapAnimation.h>

//
// Constants
//
const TInt KDelayInMicroSeconds = 50000;
const TInt KContRunningAnimFramesCount = 7;
const TInt KIncUpdatingAnimFramesCount = 19;
// Divider to correct pointerEvent y-coordinate close to graphics center
const TInt KSliderHeightDivider = 3;


// class CGaugeTimer

inline CMIDGaugeItem::CGaugeTimer::CGaugeTimer(CMIDGaugeItem& aGauge)
        :CTimer(CActive::EPriorityStandard),iGauge(aGauge)
{
    CActiveScheduler::Add(this);
}

CMIDGaugeItem::CGaugeTimer* CMIDGaugeItem::CGaugeTimer::NewL(CMIDGaugeItem& aGauge)
{
    CGaugeTimer* self = new(ELeave) CGaugeTimer(aGauge);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CMIDGaugeItem::CGaugeTimer::RunL()
{
    if (!iGauge.iGaugeFrameData)
    {//if the bitmaps haven't been created it means we are inside an alert,
        //which is using its own bitmaps for the gauge animation.
        //Return without restarting the timer
        return;
    }

    iGauge.iValue++;

    if (iGauge.iValue >= iGauge.iGaugeFrameData->iContRunBitmap.Count())
    {
        iGauge.iValue = 0;
    }

    CMIDNonInteractiveGauge* nig = static_cast<CMIDNonInteractiveGauge*>(&iGauge);
    nig->DoSafeDraw();

    After(TTimeIntervalMicroSeconds32(KDelayInMicroSeconds));
}


// class CMIDGaugeItem


TInt CMIDGaugeItem::GetValue()
{
    return iValue;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDGaugeItem::CMIDGaugeItem(CMIDUIManager* aUIManager)
        : CMIDControlItem(EDefault, aUIManager),
        iIndefiniteState(EIncrementalIdle),
        iGaugeFrameData(NULL),
        iTimer(NULL)
{
    iMMidItem = this;
    iIsInForeground = ETrue;
}

CMIDGaugeItem::~CMIDGaugeItem()
{
    if (iGaugeFrameData)
    {
        iGaugeFrameData->iNumInstances--;

        if (!(iGaugeFrameData->iNumInstances))
        {
            // delete bitmaps from thread-local bitmap storage
            DeleteBitmapsInTls();
            iGaugeFrameData->Close();
            delete iGaugeFrameData;
            iUIManager->SetGaugeItemData(NULL);
        }
    }
}


void CMIDGaugeItem::BaseConstructL(const TDesC& aLabel,TInt aMaxValue,TInt aInitialValue)
{
    DEBUG("CMIDGaugeItem::BaseConstructL <");

    CMIDControlItem::ConstructL();
    SetLabelL(aLabel);

    iLabelControl->SetWidthL(FormClientAreaWidth());

    if (aMaxValue == 0)
    {
        aMaxValue = 1;
    }

    iMaxValue = aMaxValue;
    iValue = aInitialValue;

    SetStateL(aMaxValue,aInitialValue);

    DEBUG("CMIDGaugeItem::BaseConstructL >");
}

void CMIDGaugeItem::CreateBitmapsIfNeededL()
{
    if (iGaugeFrameData)
    {
        return;
    }
    iGaugeFrameData = reinterpret_cast< TGaugeFrameData* >(iUIManager->GetGaugeItemData());
    if (iGaugeFrameData)
    {
        iGaugeFrameData->iNumInstances++;
    }
    else
    {
        // Create shared data
        iGaugeFrameData = new(ELeave) TGaugeFrameData;
        iGaugeFrameData->Clean();
        iUIManager->SetGaugeItemData(reinterpret_cast< void* >(iGaugeFrameData));
        // Initialize data
        CreateBitmapsInTlsL();
        // Resize data
        ResizeBitmapsInTlsL();
    }
}


void CMIDGaugeItem::CreateBitmapsInTlsL()
{
    // check if thread-local bitmap storage is created
    if (!iGaugeFrameData)
    {
        return;
    }

    // eventually delete existing bitmaps
    DeleteBitmapsInTls();

    // skin instance and default bitmaps file
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    TFileName avkonbmpFilename = AknIconUtils::AvkonIconFileName();

    // for bitmap loading
    const TAknsItemID* aknsItemId;
    TInt bitmapId;
    TInt bitmapMaskId;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    TInt i;

    // continuous running bitmaps
    for (i=0; i<KContRunningAnimFramesCount; i++)
    {
        switch (i)
        {
        case 0:
            aknsItemId = &KAknsIIDQgnGrafBarWait1;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_1;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_1_mask;
            break;
        case 1:
            aknsItemId = &KAknsIIDQgnGrafBarWait2;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_2;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_2_mask;
            break;
        case 2:
            aknsItemId = &KAknsIIDQgnGrafBarWait3;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_3;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_3_mask;
            break;
        case 3:
            aknsItemId = &KAknsIIDQgnGrafBarWait4;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_4;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_4_mask;
            break;
        case 4:
            aknsItemId = &KAknsIIDQgnGrafBarWait5;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_5;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_5_mask;
            break;
        case 5:
            aknsItemId = &KAknsIIDQgnGrafBarWait6;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_6;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_6_mask;
            break;
        default:
            aknsItemId = &KAknsIIDQgnGrafBarWait7;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_7;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        }

        AknsUtils::CreateIconL(skinInstance, *aknsItemId, bitmap,
                               bitmapMask, avkonbmpFilename, bitmapId, bitmapMaskId);

        TInt appendErr1 = iGaugeFrameData->iContRunBitmap.Append(bitmap);
        TInt appendErr2 = iGaugeFrameData->iContRunBitmapMask.Append(bitmapMask);

        if (appendErr1 != KErrNone)
        {
            delete bitmap;
        }
        if (appendErr2 != KErrNone)
        {
            delete bitmapMask;
        }
        if ((appendErr1 != KErrNone) || (appendErr2 != KErrNone))
        {
            User::LeaveIfError((appendErr1 != KErrNone) ? appendErr1 : appendErr2);
        }
    }

    // incremental updating bitmaps
    // (bitmaps masks ids "EMbmAvkonQgn_graf_bar_wait_increm_mask" -
    // "EMbmAvkonQgn_graf_bar_wait_increm_19_mask" are not defined and that's
    // why masks for continuous running animation are used instead)
    for (i=0; i<KIncUpdatingAnimFramesCount; i++)
    {
        switch (i)
        {
        case 0:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_1_mask;
            break;
        case 1:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem02;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_02;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_2_mask;
            break;
        case 2:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem03;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_03;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_3_mask;
            break;
        case 3:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem04;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_04;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_4_mask;
            break;
        case 4:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem05;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_05;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_5_mask;
            break;
        case 5:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem06;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_06;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_6_mask;
            break;
        case 6:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem07;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_07;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 7:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem08;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_08;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 8:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem09;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_09;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 9:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem10;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_10;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 10:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem11;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_11;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 11:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem12;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_12;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 12:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem13;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_13;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 13:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem14;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_14;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 14:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem15;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_15;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 15:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem16;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_16;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 16:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem17;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_17;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        case 17:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem18;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_18;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        default:
            aknsItemId = &KAknsIIDQgnGrafBarWaitIncrem19;
            bitmapId = EMbmAvkonQgn_graf_bar_wait_increm_19;
            bitmapMaskId = EMbmAvkonQgn_graf_bar_wait_7_mask;
            break;
        }

        AknsUtils::CreateIconL(skinInstance, *aknsItemId, bitmap,
                               bitmapMask, avkonbmpFilename, bitmapId, bitmapMaskId);

        TInt appendErr1 = iGaugeFrameData->iIncUpdBitmap.Append(bitmap);
        TInt appendErr2 = iGaugeFrameData->iIncUpdBitmapMask.Append(bitmapMask);

        if (appendErr1 != KErrNone)
        {
            delete bitmap;
        }
        if (appendErr2 != KErrNone)
        {
            delete bitmapMask;
        }
        if ((appendErr1 != KErrNone) || (appendErr2 != KErrNone))
        {
            User::LeaveIfError((appendErr1 != KErrNone) ? appendErr1 : appendErr2);
        }
    }

    // continuous idle bitmap
    // (bitmap mask id "EMbmAvkonQgn_graf_bar_wait_idle_mask" is not
    // defined and that's why mask for continuous running animation is used instead)
    AknsUtils::CreateIconL(skinInstance, KAknsIIDQgnGrafBarWaitIdle,
                           iGaugeFrameData->iContIdleBitmap,
                           iGaugeFrameData->iContIdleBitmapMask,
                           avkonbmpFilename,
                           EMbmAvkonQgn_graf_bar_wait_idle, EMbmAvkonQgn_graf_bar_wait_1_mask);

    // incremental idle bitmap
    // (bitmap mask id "EMbmAvkonQgn_graf_bar_wait_increm_idle_mask" is not
    // defined and that's why mask for continuous running animation is used instead)
    AknsUtils::CreateIconL(skinInstance, KAknsIIDQgnGrafBarWaitIncremIdle,
                           iGaugeFrameData->iIncIdleBitmap,
                           iGaugeFrameData->iIncIdleBitmapMask,
                           avkonbmpFilename,
                           EMbmAvkonQgn_graf_bar_wait_increm_idle,
                           EMbmAvkonQgn_graf_bar_wait_1_mask);

    // bar frame bitmaps (left, center and right part of frame)
    AknsUtils::CreateIconL(skinInstance, KAknsIIDQgnGrafBarFrameSideL,
                           iGaugeFrameData->iFrameL, iGaugeFrameData->iFrameLMask,
                           avkonbmpFilename, EMbmAvkonQgn_graf_bar_frame_side_l,
                           EMbmAvkonQgn_graf_bar_frame_side_l_mask);

    AknsUtils::CreateIconL(skinInstance, KAknsIIDQgnGrafBarFrameCenter,
                           iGaugeFrameData->iFrameCenter, iGaugeFrameData->iFrameCenterMask,
                           avkonbmpFilename, EMbmAvkonQgn_graf_bar_frame_center,
                           EMbmAvkonQgn_graf_bar_frame_center_mask);

    AknsUtils::CreateIconL(skinInstance, KAknsIIDQgnGrafBarFrameSideR,
                           iGaugeFrameData->iFrameR, iGaugeFrameData->iFrameRMask,
                           avkonbmpFilename, EMbmAvkonQgn_graf_bar_frame_side_r,
                           EMbmAvkonQgn_graf_bar_frame_side_r_mask);
}


void CMIDGaugeItem::DeleteBitmapsInTls()
{
    // check if thread-local bitmap storage is created
    if (!iGaugeFrameData)
    {
        return;
    }

    // continuous running bitmaps
    TInt i;

    for (i=0; i < iGaugeFrameData->iContRunBitmap.Count(); i++)
    {
        delete iGaugeFrameData->iContRunBitmap[i];
    }
    iGaugeFrameData->iContRunBitmap.Reset();

    for (i=0; i < iGaugeFrameData->iContRunBitmapMask.Count(); i++)
    {
        delete iGaugeFrameData->iContRunBitmapMask[i];
    }
    iGaugeFrameData->iContRunBitmapMask.Reset();

    // incremental updating bitmaps
    for (i=0; i < iGaugeFrameData->iIncUpdBitmap.Count(); i++)
    {
        delete iGaugeFrameData->iIncUpdBitmap[i];
    }
    iGaugeFrameData->iIncUpdBitmap.Reset();

    for (i=0; i < iGaugeFrameData->iIncUpdBitmapMask.Count(); i++)
    {
        delete iGaugeFrameData->iIncUpdBitmapMask[i];
    }
    iGaugeFrameData->iIncUpdBitmapMask.Reset();

    // continuous idle bitmap
    delete iGaugeFrameData->iContIdleBitmap;
    iGaugeFrameData->iContIdleBitmap = NULL;

    delete iGaugeFrameData->iContIdleBitmapMask;
    iGaugeFrameData->iContIdleBitmapMask = NULL;

    // incremental idle bitmap
    delete iGaugeFrameData->iIncIdleBitmap;
    iGaugeFrameData->iIncIdleBitmap = NULL;

    delete iGaugeFrameData->iIncIdleBitmapMask;
    iGaugeFrameData->iIncIdleBitmapMask = NULL;

    // frame bitmaps
    delete iGaugeFrameData->iFrameL;
    iGaugeFrameData->iFrameL = NULL;

    delete iGaugeFrameData->iFrameLMask;
    iGaugeFrameData->iFrameLMask = NULL;

    delete iGaugeFrameData->iFrameCenter;
    iGaugeFrameData->iFrameCenter = NULL;

    delete iGaugeFrameData->iFrameCenterMask;
    iGaugeFrameData->iFrameCenterMask = NULL;

    delete iGaugeFrameData->iFrameR;
    iGaugeFrameData->iFrameR = NULL;

    delete iGaugeFrameData->iFrameRMask;
    iGaugeFrameData->iFrameRMask = NULL;
}


void CMIDGaugeItem::ResizeBitmapsInTlsL()
{
    // check if thread-local bitmap storage is created
    if (!iGaugeFrameData)
    {
        return;
    }

    // get layout info
    TRect formRect;
    TRect waitGaugeRect;
    TRect waitGaugeBarRect;
    TRect waitGaugeBarFrameLRect;
    TRect waitGaugeBarFrameCRect;
    TRect waitGaugeBarFrameRRect;
    TAknLayoutRect layoutRect;
    TSize waitGaugeBarRectSize;

    formRect = FormClientAreaRect();

    layoutRect.LayoutRect(formRect,
                          AknLayoutScalable_Avkon::form2_midp_gauge_wait_pane().LayoutLine());
    waitGaugeRect = layoutRect.Rect();

    layoutRect.LayoutRect(waitGaugeRect,
                          AknLayoutScalable_Avkon::form2_midp_wait_pane(1).LayoutLine());
    waitGaugeBarRect = layoutRect.Rect();

    layoutRect.LayoutRect(waitGaugeBarRect,
                          AknLayoutScalable_Avkon::form2_midp_wait_pane_g1().LayoutLine());
    waitGaugeBarFrameLRect = layoutRect.Rect();

    layoutRect.LayoutRect(waitGaugeBarRect,
                          AknLayoutScalable_Avkon::form2_midp_wait_pane_g2().LayoutLine());
    waitGaugeBarFrameCRect = layoutRect.Rect();

    layoutRect.LayoutRect(waitGaugeBarRect,
                          AknLayoutScalable_Avkon::form2_midp_wait_pane_g3().LayoutLine());
    waitGaugeBarFrameRRect = layoutRect.Rect();

    waitGaugeBarRectSize = waitGaugeBarRect.Size();

    // resize bitmaps
    TInt i;

    // continuous running bitmaps
    for (i=0; i < iGaugeFrameData->iContRunBitmap.Count(); i++)
    {
        CFbsBitmap* bitmap = iGaugeFrameData->iContRunBitmap[i];

        if (bitmap->SizeInPixels() != waitGaugeBarRectSize)
        {
            User::LeaveIfError(AknIconUtils::SetSize(bitmap,
                               waitGaugeBarRectSize, EAspectRatioNotPreserved));
        }
    }

    for (i=0; i < iGaugeFrameData->iContRunBitmapMask.Count(); i++)
    {
        CFbsBitmap* bitmap = iGaugeFrameData->iContRunBitmapMask[i];

        if (bitmap->SizeInPixels() != waitGaugeBarRectSize)
        {
            User::LeaveIfError(AknIconUtils::SetSize(bitmap,
                               waitGaugeBarRectSize, EAspectRatioNotPreserved));
        }
    }

    // incremental updating bitmaps
    for (i=0; i < iGaugeFrameData->iIncUpdBitmap.Count(); i++)
    {
        CFbsBitmap* bitmap = iGaugeFrameData->iIncUpdBitmap[i];

        if (bitmap->SizeInPixels() != waitGaugeBarRectSize)
        {
            User::LeaveIfError(AknIconUtils::SetSize(bitmap,
                               waitGaugeBarRectSize, EAspectRatioNotPreserved));
        }
    }

    for (i=0; i < iGaugeFrameData->iIncUpdBitmapMask.Count(); i++)
    {
        CFbsBitmap* bitmap = iGaugeFrameData->iIncUpdBitmapMask[i];

        if (bitmap->SizeInPixels() != waitGaugeBarRectSize)
        {
            User::LeaveIfError(AknIconUtils::SetSize(bitmap,
                               waitGaugeBarRectSize, EAspectRatioNotPreserved));
        }
    }

    // continuous idle bitmap
    if (iGaugeFrameData->iContIdleBitmap && (iGaugeFrameData->
            iContIdleBitmap->SizeInPixels() != waitGaugeBarRectSize))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iContIdleBitmap, waitGaugeBarRectSize,
                               EAspectRatioNotPreserved));
    }

    if (iGaugeFrameData->iContIdleBitmapMask && (iGaugeFrameData->
            iContIdleBitmapMask->SizeInPixels() != waitGaugeBarRectSize))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iContIdleBitmapMask, waitGaugeBarRectSize,
                               EAspectRatioNotPreserved));
    }

    // incremental idle bitmap
    if (iGaugeFrameData->iIncIdleBitmap && (iGaugeFrameData->
                                            iIncIdleBitmap->SizeInPixels() != waitGaugeBarRectSize))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iIncIdleBitmap, waitGaugeBarRectSize,
                               EAspectRatioNotPreserved));
    }

    if (iGaugeFrameData->iIncIdleBitmapMask && (iGaugeFrameData->
            iIncIdleBitmapMask->SizeInPixels() != waitGaugeBarRectSize))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iIncIdleBitmapMask, waitGaugeBarRectSize,
                               EAspectRatioNotPreserved));
    }

    // frame bitmaps
    if (iGaugeFrameData->iFrameL && (iGaugeFrameData->
                                     iFrameL->SizeInPixels() != waitGaugeBarFrameLRect.Size()))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iFrameL, waitGaugeBarFrameLRect.Size(),
                               EAspectRatioNotPreserved));
    }

    if (iGaugeFrameData->iFrameLMask && (iGaugeFrameData->
                                         iFrameLMask->SizeInPixels() != waitGaugeBarFrameLRect.Size()))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iFrameLMask, waitGaugeBarFrameLRect.Size(),
                               EAspectRatioNotPreserved));
    }

    if (iGaugeFrameData->iFrameCenter && (iGaugeFrameData->
                                          iFrameCenter->SizeInPixels() != waitGaugeBarFrameCRect.Size()))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iFrameCenter, waitGaugeBarFrameCRect.Size(),
                               EAspectRatioNotPreserved));
    }

    if (iGaugeFrameData->iFrameCenterMask && (iGaugeFrameData->
            iFrameCenterMask->SizeInPixels() != waitGaugeBarFrameCRect.Size()))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iFrameCenterMask, waitGaugeBarFrameCRect.Size(),
                               EAspectRatioNotPreserved));
    }

    if (iGaugeFrameData->iFrameR && (iGaugeFrameData->
                                     iFrameR->SizeInPixels() != waitGaugeBarFrameRRect.Size()))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iFrameR, waitGaugeBarFrameRRect.Size(),
                               EAspectRatioNotPreserved));
    }

    if (iGaugeFrameData->iFrameRMask && (iGaugeFrameData->
                                         iFrameRMask->SizeInPixels() != waitGaugeBarFrameRRect.Size()))
    {
        User::LeaveIfError(AknIconUtils::SetSize(
                               iGaugeFrameData->iFrameRMask, waitGaugeBarFrameRRect.Size(),
                               EAspectRatioNotPreserved));
    }
}


void CMIDGaugeItem::ColorChange(TInt aType)
{
    // call default implementation
    CMIDControlItem::ColorChange(aType);

    // recreate bitmaps (current bitmaps are deleted in CreateBitmapsInTlsBitmapsL())
    TRAPD(createErr, CreateBitmapsInTlsL());

    if (createErr != KErrNone)
    {
        DEBUG_INT("CMIDGaugeItem::ColorChange() - Exception from CreateBitmapsInTlsL. Error = %d ", createErr);
    }

    TRAPD(resizeErr, ResizeBitmapsInTlsL());

    if (resizeErr != KErrNone)
    {
        DEBUG_INT("CMIDGaugeItem::ColorChange() - Exception from ResizeBitmapsInTlsL. Error = %d ", resizeErr);
    }
}

void CMIDGaugeItem::HandleForegroundL(TBool aForeground)
{
    // synchronize foreground flag
    iIsInForeground = aForeground;

    // if continuous-running ni-gauge is in foreground and it's timer
    // is not created then create it else if gauge is in background
    // and if it is inserted in form then dispose it's timer
    if (iIsInForeground)
    {
        if ((iIndefiniteState == EContinuousRunning) && (!iTimer))
        {
            iTimer = CGaugeTimer::NewL(*this);
            iTimer->After(TTimeIntervalMicroSeconds32(KDelayInMicroSeconds));
        }
    }
    else if (iForm)
    {
        DisposeTimer();
    }
}

void CMIDGaugeItem::SetStateL(TInt aMaxValue,TInt aValue)
{
    iIndefinite = (aMaxValue == EIndefinite) ? ETrue : EFalse;

    if (iIndefinite)
    {
        iIndefiniteState = aValue;
        iValue = 0;
        switch (aValue)
        {
        case EContinuousIdle:
        case EIncrementalIdle:
            DisposeTimer();
            break;
        case EContinuousRunning:
            if (!iTimer)
            {
                iTimer = CGaugeTimer::NewL(*this);
                iTimer->After(TTimeIntervalMicroSeconds32(KDelayInMicroSeconds));
            }
            break;
        case EIncrementalUpdating:
            DisposeTimer();
            break;
        default:
            ASSERT(EFalse);
        }
    }
    else
    {
    }
    //
    // We're now back in normal mode
    //
}

#ifdef RD_SCALABLE_UI_V2
void CMIDGaugeItem::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
        if (IsSelectable())
        {
            CMIDControlItem::HandlePointerEventL(aPointerEvent);
        }
    }
}
#endif // RD_SCALABLE_UI_V2


/**
 * An interactive Gauge will always be selectable. A non-interactive
 * Gauge will only be selectable if it has at least one Command.
 */
TBool CMIDGaugeItem::IsSelectable() const
{
    return (iCommandList->Count() > 0 || !IsNonFocusing());
}

void CMIDGaugeItem::DisposeTimer()
{
    if (iTimer)
        iTimer->Cancel();
    delete iTimer;
    iTimer = NULL;
}

void CMIDGaugeItem::FocusChanged(TDrawNow aDrawNow)
{
    CMIDControlItem::FocusChanged(aDrawNow);
}

void CMIDGaugeItem::SetLabelL(const TDesC& aLabel)
{
    CMIDControlItem::SetLabelL(aLabel);
}

void CMIDGaugeItem::SetLayoutL(TLayout aLayout)
{
    CMIDItem::SetLayoutL(aLayout);
}

void CMIDGaugeItem::Dispose()
{
    delete this;
}

void CMIDGaugeItem::AddCommandL(MMIDCommand* aCommand)
{
    CMIDItem::AddCommandL(aCommand);
}

void CMIDGaugeItem::RemoveCommand(MMIDCommand* aCommand)
{
    CMIDItem::RemoveCommand(aCommand);
}

void CMIDGaugeItem::SetDefaultCommand(MMIDCommand* aCommand)
{
    CMIDItem::SetDefaultCommand(aCommand);
}

void CMIDGaugeItem::SetPreferredSizeL(const TSize& aSize)
{
    CMIDItem::SetPreferredSize(aSize,MinimumSize());
}

TSize CMIDGaugeItem::PreferredSize() const
{
    return ResetPreferredSize();
}

TSize CMIDGaugeItem::MinimumSize() const
{
    CCoeControl* control = const_cast<CMIDGaugeItem*>(this);
    return control->MinimumSize();
}

TInt CMIDGaugeItem::GetIndefiniteState() const
{
    return iIndefiniteState;
}
//////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MMIDGauge* CMIDNonInteractiveGauge::NewL(
    const TDesC& aLabel,TInt aMaxValue,TInt aInitialValue, CMIDUIManager* aUIManager)
{
    CMIDNonInteractiveGauge* self = new(ELeave) CMIDNonInteractiveGauge(aUIManager);
    CleanupStack::PushL(self);
    self->BaseConstructL(aLabel, aMaxValue, aInitialValue);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CMIDNonInteractiveGauge::~CMIDNonInteractiveGauge()
{
    DisposeTimer();
    delete iProgressInfo;
    delete iBitmapAnimation;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDNonInteractiveGauge::CMIDNonInteractiveGauge(CMIDUIManager* aUIManager)
        : CMIDGaugeItem(aUIManager),
        iBitmapAnimation(NULL)
{
    SetFocusing(EFalse);
}

void CMIDNonInteractiveGauge::ConstructL()
{
    UpdateMemberVariables();
    ConstructProgressInfoL();
    ActivateL();
}

void CMIDNonInteractiveGauge::UpdateMemberVariables()
{
    TRect formRect = FormClientAreaRect();

    iWaitGaugeRect.LayoutRect(formRect,
                              AknLayoutScalable_Avkon::form2_midp_gauge_wait_pane().LayoutLine());

    iWaitBarRect.LayoutRect(iWaitGaugeRect.Rect(),
                            AknLayoutScalable_Avkon::form2_midp_wait_pane(1).LayoutLine());

    iWaitBarSize = iWaitBarRect.Rect().Size();

    // WaitBar position in relation to CCoeControl (CMIDControlItem Rect) area
    // Note that label height has to be added to the y coordinate
    iProgressInfoWithLabelHeightTl = iWaitBarRect.Rect().iTl - formRect.iTl;

    iItemheightWithoutLabel = iWaitGaugeRect.Rect().Height() +
                              iWaitGaugeRect.Rect().iTl.iY - formRect.iTl.iY + ItemContentBottomMargin();
}

void CMIDNonInteractiveGauge::ConstructProgressInfoL()
{
    ASSERT(!iProgressInfo);

    CEikProgressInfo::SInfo info;
    info.iFinalValue = iIndefinite ? 100 : iMaxValue;
    info.iSplitsInBlock = 0;
    info.iTextType = EEikProgressTextPercentage;

    info.iWidth = iWaitBarSize.iWidth;
    info.iHeight = iWaitBarSize.iHeight;

    iProgressInfo = new(ELeave) CEikProgressInfo(info);
    iProgressInfo->ConstructL();
    iProgressInfo->SetContainerWindowL(*this);
    iProgressInfo->SetAndDraw(iValue);

    if (IsActivated())
    {
        iProgressInfo->ActivateL();
    }
}

void CMIDNonInteractiveGauge::SetValueL(TInt aValue)
{
    if (iIndefinite)
    {
        // indefinite gauge will not be updated when it is inserted in form
        // and the form is sent to background
        TBool updateGauge = (iForm) ? iIsInForeground : ETrue;

        // update gauge state
        TInt oldIndefiniteState = iIndefiniteState;
        iIndefiniteState = aValue;

        switch (iIndefiniteState)
        {
        case EContinuousIdle:
            iValue = 0;
            DisposeTimer();
            break;
        case EIncrementalIdle:
            iValue = 0;
            DisposeTimer();
            break;
        case EContinuousRunning:
            if (!iTimer && updateGauge)
            {
                iTimer = CGaugeTimer::NewL(*this);
                iTimer->After(TTimeIntervalMicroSeconds32(KDelayInMicroSeconds));
            }
            break;
        case EIncrementalUpdating:
            DisposeTimer();

            // update gauge's state
            if (updateGauge)
            {
                if (iIndefiniteState != oldIndefiniteState)
                {
                    iValue = -1;
                }
                iValue++;
                if (iGaugeFrameData && iValue >= iGaugeFrameData->iIncUpdBitmap.Count())
                {
                    iValue = 0;
                }
            }
            break;
        default:
            ASSERT(EFalse);
        }

        // update gauge in alert
        if (iGaugeToAlertListner)
        {
            iGaugeToAlertListner->UpdateGaugeInAlertL(iValue);
        }

        // eventually notify alert dialog that indefinite state changed
        if (iGaugeToAlertListner && (iIndefiniteState != oldIndefiniteState))
        {
            iGaugeToAlertListner->GaugeTypeInAlertChangedL();
        }

        // redraw gauge if it should be updated or if it's type changed
        if (updateGauge || (iIndefiniteState != oldIndefiniteState))
        {
            DoSafeDraw();
        }
    }
    else
    {
        iValue = aValue;

        if (iGaugeToAlertListner)
        {
            iGaugeToAlertListner->UpdateGaugeInAlertL(iValue);
        }

        DoSafeDraw();
    }
}

void CMIDNonInteractiveGauge::SetGaugeListenerFromAlert(
    MMIDGaugeToAlertListner* aGaugeToAlertListner)
{
    iGaugeToAlertListner = aGaugeToAlertListner;
}

void CMIDNonInteractiveGauge::SetMaxValueL(TInt aMaxValue)
{
    // is new max value indefinite?
    if (aMaxValue == EIndefinite)
    {
        if (!iIndefinite)
        {
            // set gauge state to indefinite
            iIndefinite = ETrue;
            iMaxValue = EIndefinite;

            // set indefinite state to other type than with which
            //  SetValueL function will be called in order to detect change
            //  of gauge type in this function
            iIndefiniteState = EContinuousIdle+1;
            SetValueL(EContinuousIdle);
        }
    }
    else
    {
        // no timer needed for definite gauge
        DisposeTimer();

        // set gauge state to definite
        iIndefinite = EFalse;
        iMaxValue = aMaxValue;

        // check gauge's value
        if (iValue > iMaxValue)
        {
            iValue = iMaxValue;
        }
        else if (iValue < 0)
        {
            iValue = 0;
        }

        // eventually update gauge in alert
        if (iGaugeToAlertListner)
        {
            iGaugeToAlertListner->GaugeTypeInAlertChangedL();
        }

        // redraw
        DoSafeDraw();
    }
}

TSize CMIDNonInteractiveGauge::MinimumSize()
{
    if (!iLabelControl || (iLabelControl->Text()->Length() == 0))
    { // item doesn't have label
        return TSize(FormClientAreaWidth(), iItemheightWithoutLabel);
    }
    else // item has label
    {
        return TSize(FormClientAreaWidth(),
                     iItemheightWithoutLabel + OneLineLabelHeight());
    }
}

TInt CMIDNonInteractiveGauge::CountComponentControls() const
{
    if (iIndefinite)
    {
        return 1; // we will draw the gauge in our Draw() method
    }
    else
    {
        return 2;
    }
}

CCoeControl* CMIDNonInteractiveGauge::ComponentControl(TInt aIndex) const
{
    switch (aIndex)
    {
    case 0:
        return iLabelControl;
    case 1:
        return iProgressInfo;
    default:
        return NULL;
    }
}

/** Unlike other items, this one here draw the backgorund as well because its draw
    method is called without drawing the form first. Other items only draw the control.
*/
void CMIDNonInteractiveGauge::Draw(const TRect& /*aRect*/) const
{
    CWindowGc& gc=SystemGc();

    // get drawing position
    TRect rect(Rect());

    // draw background and control
    CMIDControlItem::Draw(rect);


    // draw non-interactive gauge graphics
    if (iIndefinite && iGaugeFrameData)
    {
        // If Gauge animated in current skin, is drawn by iBitmapAnimation.
        // Otherwise is drawn in this block of code.
        if ((!iBitmapAnimation) || (!(iBitmapAnimation->BitmapAnimData())) ||
                (!(iBitmapAnimation->BitmapAnimData()->FrameArray().Count()> 0)))
        {
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* bitmapMask = NULL;

            // Finding of correct bitmap and bitmapMask from TLS
            switch (iIndefiniteState)
            {
            case EContinuousIdle:
                bitmap = iGaugeFrameData->iContIdleBitmap;
                bitmapMask = iGaugeFrameData->iContIdleBitmapMask;
                break;
            case EIncrementalIdle:
                bitmap = iGaugeFrameData->iIncIdleBitmap;
                bitmapMask = iGaugeFrameData->iIncIdleBitmapMask;
                break;
            case EContinuousRunning:
                if ((iValue>=0) &&
                        (iValue < iGaugeFrameData->iContRunBitmap.Count()))
                {
                    bitmap = iGaugeFrameData->iContRunBitmap[iValue];
                }
                if ((iValue>=0) &&
                        (iValue < iGaugeFrameData->iContRunBitmapMask.Count()))
                {
                    bitmapMask = iGaugeFrameData->iContRunBitmapMask[iValue];
                }
                break;
            case EIncrementalUpdating:
                if ((iValue>=0) &&
                        (iValue < iGaugeFrameData->iIncUpdBitmap.Count()))
                {
                    bitmap = iGaugeFrameData->iIncUpdBitmap[iValue];
                }
                if ((iValue>=0) &&
                        (iValue < iGaugeFrameData->iIncUpdBitmapMask.Count()))
                {
                    bitmapMask = iGaugeFrameData->iIncUpdBitmapMask[iValue];
                }
                break;
            default:
                ASSERT(EFalse);
            }

            // get drawing position
            TPoint drawPosition = Position() + iProgressInfoWithLabelHeightTl +
                                  TPoint(0,LabelHeight());

            // draw gague bar
            if (bitmap && bitmapMask)
            {
                TSize bitmapSize = bitmap->SizeInPixels();
                TRect bitmapRect = TRect(0, 0, bitmapSize.iWidth, bitmapSize.iHeight);
                gc.BitBltMasked(drawPosition, bitmap, bitmapRect, bitmapMask, EFalse);
            }
            else if (bitmap)
            {
                gc.BitBlt(drawPosition, bitmap);
            }
        }
    }

    TPoint drawPositionFrame = Position() + iProgressInfoWithLabelHeightTl +
                               TPoint(0,LabelHeight());
    // draw gague bar frame
    if (iGaugeFrameData && iGaugeFrameData->iFrameL && iGaugeFrameData->iFrameLMask &&
            iGaugeFrameData->iFrameCenter && iGaugeFrameData->iFrameCenterMask &&
            iGaugeFrameData->iFrameR && iGaugeFrameData->iFrameRMask)
    {
        // draw of left frame of gauge bar
        gc.BitBltMasked(drawPositionFrame, iGaugeFrameData->iFrameL,
                        TRect(iGaugeFrameData->iFrameL->SizeInPixels()),
                        iGaugeFrameData->iFrameLMask, EFalse);

        // draw of central frame of gauge bar
        drawPositionFrame.iX += iGaugeFrameData->iFrameL->SizeInPixels().iWidth;

        gc.BitBltMasked(drawPositionFrame, iGaugeFrameData->iFrameCenter,
                        TRect(iGaugeFrameData->iFrameCenter->SizeInPixels()),
                        iGaugeFrameData->iFrameCenterMask, EFalse);

        // draw of right frame of gauge bar
        drawPositionFrame.iX += iGaugeFrameData->iFrameCenter->SizeInPixels().iWidth;

        gc.BitBltMasked(drawPositionFrame, iGaugeFrameData->iFrameR,
                        TRect(iGaugeFrameData->iFrameR->SizeInPixels()),
                        iGaugeFrameData->iFrameRMask, EFalse);
    }
}

void CMIDNonInteractiveGauge::SizeChanged()
{
    // Base implemation
    CMIDControlItem::SizeChanged();

    // If Gauge is animated, the animation will be resized.
    if (iBitmapAnimation && iBitmapAnimation->BitmapAnimData() &&
            iBitmapAnimation->BitmapAnimData()->FrameArray().Count()> 0)
    {
        // Stoping of animation.
        iBitmapAnimation->CancelAnimation();
        // Resizing of animation
        SetAnimationSize();

        // Start of animation
        TRAPD(errStart, iBitmapAnimation->StartAnimationL());
        if (errStart != KErrNone)
        {
            DEBUG_INT("CMIDNonInteractiveGauge::SizeChanged() - Exception from CAknBitmapAnimation::StartAnimationL(). Error = %d ", errStart);
        }

        // Setting of animation mode (cycle).
        TRAPD(errSet, iBitmapAnimation->Animation().SetPlayModeL(CBitmapAnimClientData::ECycle));
        if (errSet != KErrNone)
        {
            DEBUG_INT("CMIDNonInteractiveGauge::SizeChanged() - Exception from RBitmapAnim::SetPlayModeL(). Error = %d ", errSet);
        }

    }

    // Resizing of label.
    iLabelControl->SetExtent(Position(), TSize(FormClientAreaWidth(), LabelHeight()));

    // Positioning progress bar
    TPoint p = Position() + iProgressInfoWithLabelHeightTl + TPoint(0,LabelHeight());
    iProgressInfo->SetExtent(p, iProgressInfo->Size());
}

void CMIDNonInteractiveGauge::SetContainerWindowL(const CCoeControl& aContainer)
{
    CreateBitmapsIfNeededL();
    CMIDControlItem::SetContainerWindowL(aContainer);
    iProgressInfo->SetContainerWindowL(*this);
#ifdef RD_SCALABLE_UI_V2
    //Enable cancelation of long tap i.e. up event is grabbed from long tap animation
    DrawableWindow()->SetPointerGrab(ETrue);
#endif //RD_SCALABLE_UI_V2
}

void CMIDNonInteractiveGauge::DoSafeDraw()
{
    if (iForm && DrawableWindow())
    {
        if (!iIndefinite && iProgressInfo->DrawableWindow())
        {
            iProgressInfo->SetFinalValue(iMaxValue);
            iProgressInfo->SetAndDraw(iValue);
        }

        DrawNow();
    }
}

#ifdef RD_SCALABLE_UI_V2
void CMIDNonInteractiveGauge::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        if (!iForm->TryDetectLongTapL(aPointerEvent) &&
                !iForm->IsFocusChangingWithPen())
        {
            switch (aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
            case TPointerEvent::EButton1Up:
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if (feedback)
                {
                    feedback->InstantFeedback(ETouchFeedbackList);
                }
                break;
            }
        }
#else
        iForm->TryDetectLongTapL(aPointerEvent);
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK        

    }
}
#endif // RD_SCALABLE_UI_V2

TInt CMIDNonInteractiveGauge::ItemPreferredHeightWithoutLabel()
{
    return iItemheightWithoutLabel;
}

TInt CMIDNonInteractiveGauge::GetMaxValue()
{
    return iMaxValue;
}

/* ResolutionChange
 *
 * This method is called after dynamic resolution change
 */

void CMIDNonInteractiveGauge::ResolutionChange(TInt /*aType*/)
{
    // update member variables
    UpdateMemberVariables();

    // resize bitmaps in TLS
    TRAPD(resizeErr, ResizeBitmapsInTlsL());
    if (resizeErr != KErrNone)
    {
        DEBUG_INT("CMIDNonInteractiveGauge::ResolutionChange() - Exception from ResizeBitmapsInTlsL. Error = %d ", resizeErr);
    }

    // update progressinfo
    delete iProgressInfo;
    iProgressInfo = NULL;
    TRAPD(err, ConstructProgressInfoL());
    if (err != KErrNone)
    {
        DEBUG_INT("NonInteractiveGauge::ResolutionChange - Exception from ConstructProgressInfoL. Error = %d", err);
    }
}

void CMIDNonInteractiveGauge::ConstructAnimation(CAknBitmapAnimation* aBitmapAnimation)
{
    // Setting aBitmapAnimation context into this (first stage).
    aBitmapAnimation->CopyControlContextFrom(this);
    aBitmapAnimation->SetMopParent(this);

    // Setting of CCoeBrushAndPenContext for aBitmapAnimation
    CCoeBrushAndPenContext* brushAndPenContext = NULL;
    TRAPD(err, brushAndPenContext = CCoeBrushAndPenContext::NewL());
    if (err != KErrNone)
    {
        DEBUG_INT("CMIDNonInteractiveGauge::ResolutionChange() - Exception fromCCoeBrushAndPenContext::NewL. Error = %d ", err);
    }

    // Setting aBitmapAnimation context into this (second stage).
    aBitmapAnimation->SetControlContext(brushAndPenContext);

    // If iBitmapAnimation exists, it have  to been deleted.
    if (iBitmapAnimation)
    {
        delete iBitmapAnimation;
        iBitmapAnimation = NULL;
    }

    // Set aAnimation like iBitmapAnimation
    iBitmapAnimation = aBitmapAnimation;
}

void CMIDNonInteractiveGauge::ItemAddedToFormL()
{
    // call parent's implementation
    CMIDGaugeItem::ItemAddedToFormL();

    CAknBitmapAnimation* bitmapAnimation = CAknBitmapAnimation::NewL();
    CleanupStack::PushL(bitmapAnimation);

    TRect waitGaugeRect;

    // setting of window of bitmapAnimation to this
    bitmapAnimation->SetContainerWindowL(*this);

    // costruction of bitmapAnimation by ID of Gauge
    bitmapAnimation->ConstructFromSkinL(GetAknsItemID());

    CleanupStack::Pop(bitmapAnimation);

    // If bitmapAnimation exist, set bitmapAnimation to IBitmapAnimation.
    if (bitmapAnimation && bitmapAnimation->BitmapAnimData()->FrameArray().Count() > 0)
    {
        ConstructAnimation(bitmapAnimation);
    }
    // Else create Bitmap in TLS.
    else
    {
        CreateBitmapsIfNeededL();
    }

    // initially assume that gauge is in foreground so that indefinite ni-gauges
    // can be updated and redrawn (gauge can be inserted into form after form
    // is moved to foreground -> no notification to gauge)
    HandleForegroundL(ETrue);
}

void CMIDNonInteractiveGauge::ItemRemovedFromForm()
{
    // call parent's implementation
    CMIDGaugeItem::ItemRemovedFromForm();

    // if indefinite ni-gauge is removed from form then no notifications
    // about moving to foreground/background are sent and so in order to ensure
    // full functionality it is assumed that gauge is in foreground
    TRAPD(err, HandleForegroundL(ETrue));

    if (err != KErrNone)
    {
        DEBUG_INT("CMIDNonInteractiveGauge::ItemRemovedFromForm() - Exception from HandleForegroundL. Error = %d ", err);
    }

    // When NonInteractiveGauge was removed from form, then animation
    // must be deleted because uses form's window.
    if (iBitmapAnimation)
    {
        delete iBitmapAnimation;
        iBitmapAnimation = NULL;
    }
}

void CMIDNonInteractiveGauge::ColorChange(TInt aType)
{
    // The original CMIDNonInteractiveGauge::ColorChange is using
    // CleanupStack::PushL( bitmapAnimation ); in non-leavung method.
    // Codescanner notices this as an error.
    TRAPD(creatingErr, ColorChangeL(aType));
    if (creatingErr != KErrNone)
    {
        DEBUG_INT("CMIDNonInteractiveGauge::ColorChange() - Exception. Error = %d ", creatingErr);
    }
}

void CMIDNonInteractiveGauge::ColorChangeL(TInt aType)
{
    // stopping and deleting iBitmapAnimation
    if (iBitmapAnimation && iBitmapAnimation->BitmapAnimData()->FrameArray().Count()> 0)
    {
        iBitmapAnimation->CancelAnimation();
        delete iBitmapAnimation;
        iBitmapAnimation = NULL;
    }

    // creating new instance of CAknBitmapAnimation for Gauge animation in new skin
    CAknBitmapAnimation* bitmapAnimation = NULL;
    TRAPD(creatingErr, bitmapAnimation = CAknBitmapAnimation::NewL());
    if (creatingErr != KErrNone)
    {
        DEBUG_INT("CMIDNonInteractiveGauge::ColorChange() - Exception from CAknBitmapAnimation::NewL. Error = %d ", creatingErr);
    }

    // CleanupStack::PushL can not be in TRAPD macro.
    CleanupStack::PushL(bitmapAnimation);

    // setting of window of bitmapAnimation to this
    TRAPD(setErr, bitmapAnimation->SetContainerWindowL(*this));
    if (setErr != KErrNone)
    {
        DEBUG_INT("CMIDNonInteractiveGauge::ColorChange() - Exception from CAknBitmapAnimation::SetContainerWindowL. Error = %d ", setErr);
    }

    // costruction of bitmapAnimation by ID of Gauge
    TRAPD(constructErr, bitmapAnimation->ConstructFromSkinL(GetAknsItemID()));
    if (constructErr != KErrNone)
    {
        DEBUG_INT("CMIDNonInteractiveGauge::ColorChange() - Exception from CAknBitmapAnimation::ConstructFromSkinL. Error = %d ", constructErr);
    }

    CleanupStack::Pop(bitmapAnimation);

    // If bitmapAnimation exist, it sets bitmapAnimation to IBitmapAnimation and
    // sets correct size iBitmapAnimation.
    if (bitmapAnimation->BitmapAnimData()->FrameArray().Count()> 0)
    {
        // call of parent's implementation
        CMIDGaugeItem::ColorChange(aType);

        // setting bitmapAnimation to IBitmapAnimation
        ConstructAnimation(bitmapAnimation);

        // settting correct size iBitmapAnimation
        SetAnimationSize();

        // Start of playing iBitmapAnimation
        TRAPD(errStart, iBitmapAnimation->StartAnimationL());
        if (errStart != KErrNone)
        {
            DEBUG_INT("CMIDNonInteractiveGauge::ColorChange() - Exception from CAknBitmapAnimation::StartAnimationL(). Error = %d ", errStart);
        }

        // Setting of animation mode (cycle).
        TRAPD(errSet, iBitmapAnimation->Animation().SetPlayModeL(CBitmapAnimClientData::ECycle));
        if (errSet != KErrNone)
        {
            DEBUG_INT("CMIDNonInteractiveGauge::ColorChange() - Exception from RBitmapAnim::SetPlayModeL(). Error = %d ", errSet);
        }
    }
    else
    {
        // call parent's implementation
        CMIDGaugeItem::ColorChange(aType);
    }

    // send resource change event to progress info
    if (iProgressInfo)
    {
        iProgressInfo->HandleResourceChange(aType);
    }
}

void CMIDNonInteractiveGauge::UpdateProgressInfo(CEikProgressInfo* aProgressInfo) const
{
    if (!iProgressInfo || !aProgressInfo)
    {
        return;
    }
    aProgressInfo->SetFinalValue(iProgressInfo->Info().iFinalValue);
}

void CMIDNonInteractiveGauge::SetAnimationSize()
{
    // When Gauge is animated and initialized, then Get the right rect for the animation.
    if (iBitmapAnimation && iForm)
    {
        TRect parentRect = FormClientAreaRect();
        TRect waitGaugeBarRect;
        TRect waitGaugeRect;
        TAknLayoutRect layoutRect;
        TSize waitGaugeBarRectSize;
        TRect animationRect;

        // computing layout for whole Gauge
        layoutRect.LayoutRect(parentRect,
                              AknLayoutScalable_Avkon::form2_midp_gauge_wait_pane().LayoutLine());
        waitGaugeRect = layoutRect.Rect();

        // computing layout for animation rectagle
        layoutRect.LayoutRect(waitGaugeRect,
                              AknLayoutScalable_Avkon::form2_midp_wait_pane(1).LayoutLine());
        waitGaugeBarRect = layoutRect.Rect();

        // moving rectagle for animation to correct position
        waitGaugeBarRectSize = waitGaugeBarRect.Size();
        TPoint drawPosition = Position() + iProgressInfoWithLabelHeightTl +
                              TPoint(0,LabelHeight());
        animationRect = TRect(drawPosition.iX, drawPosition.iY,
                              drawPosition.iX + waitGaugeBarRectSize.iWidth,
                              drawPosition.iY + waitGaugeBarRectSize.iHeight);

        // setting EAspectRatioNotPreserved mode for SetRect;
        // This prevent of wrong scalling.
        iBitmapAnimation->SetScaleModeForAnimationFrames(EAspectRatioNotPreserved);

        //Scalling of iBitmapAnimation
        iBitmapAnimation->SetRect(animationRect);
    }
}

TAknsItemID CMIDNonInteractiveGauge::GetAknsItemID()
{
    if (GetMaxValue() <= 0)
    {
        switch (GetIndefiniteState())
        {
        case MMIDGauge::EContinuousIdle: // continuous idle
        {
            return KAknsIIDQgnGrafBarWaitIdle;
        }
        case MMIDGauge::EContinuousRunning: // continuous running
        {
            return KAknsIIDQgnGrafBarWaitAnim;
        }
        case MMIDGauge::EIncrementalUpdating: //incremental updating
        {
            return KAknsIIDQgnGrafBarWaitIncrem;
        }
        case MMIDGauge::EIncrementalIdle: //incremental idle
        {
            return KAknsIIDQgnGrafBarWaitIncremIdle;
        }
        default:
            return KAknsIIDNone;
        }
    }
    return KAknsIIDNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//

MMIDGauge* CMIDInteractiveGauge::NewL(
    const TDesC& aLabel,TInt aMaxValue,TInt aInitialValue, CMIDUIManager* aUIManager)
{
    CMIDInteractiveGauge* self = new(ELeave) CMIDInteractiveGauge(aUIManager);
    CleanupStack::PushL(self);
    self->BaseConstructL(aLabel, aMaxValue, aInitialValue);
    self->ConstructL();

    CleanupStack::Pop(self);

    return self;
}

CMIDInteractiveGauge::~CMIDInteractiveGauge()
{
}


CMIDInteractiveGauge::CMIDInteractiveGauge(CMIDUIManager* aUIManager)
        : CMIDGaugeItem(aUIManager)
{
    SetFocusing(ETrue);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//

void CMIDInteractiveGauge::ConstructL()
{
    iIndefinite = (iMaxValue == EIndefinite);

    UpdateMemberVariables();

    ConstructSliderL();

    ActivateL();
}

void CMIDInteractiveGauge::UpdateMemberVariables()
{
    TRect formRect = FormClientAreaRect();

    iSliderGaugeRect.LayoutRect(formRect,
                                AknLayoutScalable_Avkon::form2_midp_gauge_slider_pane().LayoutLine());

    // Slider position in relation to CCoeControl (CMIDControlItem Rect) area
    iSliderPositionTl = iSliderGaugeRect.Rect().iTl - formRect.iTl;

    iItemheightWithoutLabel = iSliderGaugeRect.Rect().Height() +
                              iSliderPositionTl.iY + ItemContentBottomMargin();
}

void CMIDInteractiveGauge::ConstructSliderL()
{
    ASSERT(!iSlider);
    iSlider = new(ELeave) CAknSlider();
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader, R_MIDP_SLIDER_DEFAULT);
    iSlider->HandleResourceChange(KEikMessageCaptionedControlEditableStateChange);
    iSlider->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy();    // reader

    iSlider->SetRange(0, iMaxValue);
    iSlider->SetStepSize(1);
    iSlider->SetValueL(iValue);
    TBuf<10> number;
    number.Num(0);
    iSlider->SetMinimumTextL(number);
    number.Num(iMaxValue);
    iSlider->SetMaximumTextL(number);

    iSlider->SetSize(TSize(iSliderGaugeRect.Rect().Width(), iSliderGaugeRect.Rect().Height()));
}

void CMIDInteractiveGauge::SetValueL(TInt aValue)
{
    iValue = aValue;
    iSlider->SetValueL(iValue);
    DoSafeDraw();
}

void CMIDInteractiveGauge::SetMaxValueL(TInt aValue)
{
    iMaxValue = aValue;
    if (iValue > iMaxValue)
    {
        iValue = iMaxValue;
        iSlider->SetValueL(iValue);
    }
    TBuf<10> number;
    number.Num(iMaxValue);
    iSlider->SetMaximumTextL(number);
    iSlider->SetRange(0, iMaxValue);
    DoSafeDraw();
}

TSize CMIDInteractiveGauge::MinimumSize()
{
    if (!iLabelControl || (iLabelControl->Text()->Length() == 0))
    { // item doesn't have label
        return TSize(FormClientAreaWidth(), iItemheightWithoutLabel);
    }
    else // item has label
    {
        return TSize(FormClientAreaWidth(), iItemheightWithoutLabel + OneLineLabelHeight());
    }
}

TInt CMIDInteractiveGauge::CountComponentControls() const
{
    return 2;
}

CCoeControl* CMIDInteractiveGauge::ComponentControl(TInt aIndex) const
{
    switch (aIndex)
    {
    case 0:
        return iLabelControl;
    case 1:
        return iSlider;
    default:
        return NULL;
    }
}

void CMIDInteractiveGauge::Draw(const TRect& aRect) const
{
    CMIDControlItem::Draw(aRect);
}

void CMIDInteractiveGauge::SizeChanged()
{
    CMIDControlItem::SizeChanged();

    iLabelControl->SetExtent(Position() , TSize(FormClientAreaWidth(), LabelHeight()));
    iSlider->SetExtent(Position() +
                       TSize(iSliderPositionTl.iX, LabelHeight()), iSlider->Size());
}

void CMIDInteractiveGauge::PositionChanged()
{
    iSlider->SetExtent(Position() +
                       TSize(iSliderPositionTl.iX, LabelHeight()), iSlider->Size());
}

void CMIDInteractiveGauge::SetContainerWindowL(const CCoeControl& aContainer)
{
    CMIDControlItem::SetContainerWindowL(aContainer);
    iSlider->SetContainerWindowL(*this);
#ifdef RD_SCALABLE_UI_V2
    if (AknLayoutUtils::PenEnabled())
    {
        iSlider->EnableDrag();
    }
#endif // RD_SCALABLE_UI_V2
    SetObserver(iForm);
}

void CMIDInteractiveGauge::DoSafeDraw()
{
    if (DrawableWindow())
    {
        DrawNow();
    }
}

TInt CMIDInteractiveGauge::ItemPreferredHeightWithoutLabel()
{
    return iItemheightWithoutLabel;
}

TKeyResponse CMIDInteractiveGauge::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TBool valueChanged = EFalse;
    if (aType != EEventKey ||
            (aKeyEvent.iCode != EKeyLeftArrow && aKeyEvent.iCode != EKeyRightArrow) ||
            IsNonFocusing())
    {
        return EKeyWasNotConsumed;
    }
    iValue = iSlider->Value();
    if (aKeyEvent.iCode == EKeyLeftArrow)
    {
        if (iValue > 0)
        {
            iValue -= 1;
            iSlider->SetValueL(iValue);
            valueChanged = ETrue;
        }
    }
    else if (aKeyEvent.iCode == EKeyRightArrow)
    {
        if (iValue < iMaxValue)
        {
            iValue += 1;
            iSlider->SetValueL(iValue);
            valueChanged = ETrue;
        }
    }

    if (valueChanged)
    {
        ReportEventL(MCoeControlObserver::EEventStateChanged);
    }
    DoSafeDraw();

    return EKeyWasConsumed;
}

#ifdef RD_SCALABLE_UI_V2
void CMIDInteractiveGauge::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
        if (aPointerEvent.iType == TPointerEvent::EButton1Down && !iForm->PhysicsScrolling())
        {
            iPhysicsScrollingTriggered = EFalse;
            iOriginalSliderValue = iSlider->Value();
        }
        // Enlarge the control area by changing the pointer y-position
        // close to center of graphics area.
        TPointerEvent pEvent = aPointerEvent;
        TBool sliderArea = iSlider->Rect().Contains(aPointerEvent.iPosition);
        if (sliderArea)
        {
            pEvent.iPosition.iY = iSlider->Rect().iTl.iY +
                                  ((iSlider->Rect().iBr.iY - iSlider->Rect().iTl.iY) /
                                   KSliderHeightDivider);
        }
        CMIDGaugeItem::HandlePointerEventL(pEvent);
        if (iValue != iSlider->Value() && !iForm->PhysicsScrolling())
        {
            iValue = iSlider->Value();
            ReportEventL(MCoeControlObserver::EEventStateChanged);
            return;
        }
        if (sliderArea && iForm->PhysicsScrolling() && !iPhysicsScrollingTriggered)
        {
            //Physics scrolling triggered, set new slider value.
            iPhysicsScrollingTriggered = ETrue;
            iSlider->SetValueL(iValue);
            return;
        }

        iForm->TryDetectLongTapL(aPointerEvent);
    }
}

#endif // RD_SCALABLE_UI_V2

/* ResolutionChange
 *
 * This method is called after dynamic resolution change
 */
void CMIDInteractiveGauge::ResolutionChange(TInt aType)
{
    UpdateMemberVariables();
    iSlider->HandleResourceChange(aType);
    iSlider->SetSize(TSize(iSliderGaugeRect.Rect().Width(), iSliderGaugeRect.Rect().Height()));
}

// End of File
