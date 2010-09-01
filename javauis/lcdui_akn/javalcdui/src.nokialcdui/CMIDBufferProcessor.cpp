/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for buffer processor.
*
*/



#include "CMIDBufferProcessor.h"
#include "CMIDDirectGraphics.h"
#include "CMIDToolkit.h"
#include "CMIDGraphics.h"
#include "CMIDConvertFactory.h"

// -----------------------------------------------------------------------------
// CMIDBufferProcessor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CMIDBufferProcessor::NewL(CMIDToolkit* aToolkit,
                               TInt* aHandle, CMIDGraphics* aGraphics,
                               MMIDImage* aImage)
{
    CMIDBufferProcessor* processor = new(ELeave) CMIDBufferProcessor();
    CleanupStack::PushL(processor);
    processor->ConstructL(aGraphics, aImage);
    *aHandle = aToolkit->RegisterComponentL(processor, NULL);
    CleanupStack::Pop(processor);
}

CMIDBufferProcessor::CMIDBufferProcessor()
{
}

// -----------------------------------------------------------------------------
// CMIDBufferProcessor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMIDBufferProcessor::ConstructL(CMIDGraphics* aGraphics,
                                     MMIDImage* aImage)
{
    iGraphics = aGraphics;
    iDirectGraphics = CMIDDirectGraphics::NewL(aGraphics, aImage);
}

CMIDBufferProcessor::~CMIDBufferProcessor()
{
    delete iDirectGraphics;
}

void CMIDBufferProcessor::Dispose()
{
    delete this;
}

MMIDComponent::TType CMIDBufferProcessor::Type() const
{
    return EUnknown;
}

MMIDBufferProcessor* CMIDBufferProcessor::Processor()
{
    return this;
}

LOCAL_C void EndBitmapUtil(TAny* aBitmapUtil)
{
    ((TBitmapUtil*)aBitmapUtil)->End();
}

TBool CMIDBufferProcessor::ProcessL(
    const TMIDBufferOp*& aRead,
    const TMIDBufferOp* aEnd,
    TInt& aCycles,
#ifdef RD_JAVA_NGA_ENABLED
    java::util::Monitor* /*aMonitor*/)
#else
    TRequestStatus* /*aStatus*/)
#endif
{
    TBitmapUtil bitmapUtil(iGraphics->Bitmap());
    CleanupStack::PushL(TCleanupItem(&EndBitmapUtil,
                                     &bitmapUtil));
    bitmapUtil.Begin(TPoint(0, 0));

    while ((aRead < aEnd) && (aCycles > 0))
    {
        TInt opCode = aRead->OpCode();
        switch (opCode)
        {
        case MMIDGraphics::EGrOpSetClipRect:
        {
            SetClipRect(RectData(aRead));
            break;
        }
        case EDrawTriangle:
        {
            DrawTriangle(TriangleData(aRead));
            break;
        }
        case EFillTriangle:
        {
            FillTriangle(TriangleData(aRead));
            break;
        }
        case EDrawImage:
        {
            DrawImageL(aRead, bitmapUtil);
            break;
        }
        default:
        {
            bitmapUtil.End();
            ASSERT(EFalse);
            break;
        }
        }
        aRead += aRead->Size();
    }
    CleanupStack::Pop();   // CSI: 12 TCleanupItem parameter cannot be used with Pop #
    bitmapUtil.End();
    return EFalse;
}

void CMIDBufferProcessor::AbortAsync()
{
}

void CMIDBufferProcessor::DrawImageL(
    const TMIDBufferOp* aRead,
    TBitmapUtil& aUtil)
{
    TMIDBitmapParameters params;

    params.iTransparency = EFalse;

    UnlockedBitmapParameters(params, aRead);

    params.iFormat =
        CMIDConvertFactory::GetPixelFormat(params.iBitmap->DisplayMode());
    if ((params.iFormat == KMIDTypeInt8888ARGB) ||
            (params.iFormat == KMIDTypeUshort4444ARGB))
    {
        params.iTransparency = ETrue;
    }

    // if target and source bitmaps are in the same heap, locking the source
    // would result in a deadlock if target is locked.
    aUtil.End();

    TBitmapUtil imageBitmapUtil(params.iBitmap);
    CleanupStack::PushL(TCleanupItem(&EndBitmapUtil, &imageBitmapUtil));
    imageBitmapUtil.Begin(TPoint(0, 0), aUtil);

    LockedDrawImageL(params, imageBitmapUtil, aUtil);

    CleanupStack::PopAndDestroy();   // CSI: 12 TCleanupItem parameter cannot be used with PopAndDestroy #
}

void CMIDBufferProcessor::LockedDrawImageL(
    TMIDBitmapParameters& aParams,
    TBitmapUtil& aSourceUtil,
    TBitmapUtil& aTargetUtil)
{
    LockedBitmapParameters(aParams);
    // when we go to directgraphics code, target bitmap must be locked.
    // source must be unlocked in order to be sure that no deadlock occurs
    aSourceUtil.End();
    aTargetUtil.Begin(TPoint(0, 0), aSourceUtil);
    iDirectGraphics->DrawBitmapL(&aParams);
}


void CMIDBufferProcessor::DrawTriangle(const TTriangleData& aTriangle)
{
    iDirectGraphics->DrawTriangle(aTriangle.x1, aTriangle.y1, aTriangle.x2, aTriangle.y2,
                                  aTriangle.x3, aTriangle.y3, aTriangle.color);
}

void CMIDBufferProcessor::FillTriangle(const TTriangleData& aTriangle)
{
    iDirectGraphics->FillTriangle(aTriangle.x1, aTriangle.y1, aTriangle.x2, aTriangle.y2,
                                  aTriangle.x3, aTriangle.y3, aTriangle.color);
}

void CMIDBufferProcessor::SetClipRect(const TRect& aRect)
{
    iDirectGraphics->SetClippingRect(aRect);
}

