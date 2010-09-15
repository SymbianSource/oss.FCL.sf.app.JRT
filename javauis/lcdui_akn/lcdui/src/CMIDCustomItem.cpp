/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#include <coemain.h>
#include <eiklabel.h>
#include <hal.h>
#include <AknsDrawUtils.h>// skin
#include <AknsBasicBackgroundControlContext.h> //skin

#ifdef RD_INTELLIGENT_TEXT_INPUT
#include <PtiEngine.h>
#endif //RD_INTELLIGENT_TEXT_INPUT

#include "CMIDForm.h"
#include "CMIDFormPhysics.h"
#include "CMIDCustomItem.h"
// API for iLabelControl
#include "CMIDItemLabel.h"
// API needed for retrieving current displayable from iForm when background drawing
#include "CMIDDisplayable.h"

// retrieving CMIDRemConObserver from CMIDKeyDecoder
#include "CMIDKeyDecoder.h"
// needed for adding (removing) media keys observer to remConObserver
#include "CMIDRemConObserver.h"
#include "CMIDUIManager.h"
#include <eikenv.h>
#include <j2me/jdebug.h>

// LAF
// AknLayoutScalable_Avkon::form2_midp_content_pane
#include <aknlayoutscalable_avkon.cdl.h>

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

/** The custom item uses a different value for limiting the max sizes because
it needs to draw bitmaps with these sizes. @see KMaxScreenSizeFactor */
const TInt KCIMaxScreenSizeFactor = 2;

// ---------------------------------------------------------------------------
// TLcduiEvent
// ---------------------------------------------------------------------------
//
enum TLcduiEvent
{
    EFixUIOrientation,
    EUnFixUIOrientation
};


CMIDCustomItem* CMIDCustomItem::NewL(
    MMIDEnv& aEnv, const TDesC& aLabel, CMIDUIManager* aUIManager)
{
    CMIDCustomItem* item = new(ELeave) CMIDCustomItem(aEnv, aUIManager);
    CleanupStack::PushL(item);
    item->ConstructL(aLabel);
    CleanupStack::Pop(item);
    return item;
}

CMIDCustomItem::CMIDCustomItem(MMIDEnv& aEnv, CMIDUIManager* aUIManager)
    : CMIDControlItem(EDefault, aUIManager)
    , iEntered(EFalse)
    , iSupportsInternalTraversal(ETrue)
    , iEnv(aEnv)
    , iResetFrameBuffer(ETrue)
    , iDirectAreaAddedToDisplayable(EFalse)
    , iS60SelectionKeyCompatibility(EFalse)
#ifndef RD_JAVA_NGA_ENABLED
    , iDirectPaused(ETrue)
#endif
    , iRestoreDirectContentWhenUnfaded(EFalse)
    , iPreviousVisibility(EFalse)
    , iConsumerWaitingForDSAResourcesCallback(NULL)
    , iUiFixed(EFalse)
    , iUiToBeFixedLater(EFalse)
{
    iMMidItem = this;

    //Default values for PointerEventSuppressor
    iPESPointerMovementInTwips = CMIDUIManager::EPESPointerMovementInTwips;
    iPESTimeInMilliseconds = CMIDUIManager:: EPESTimeInMilliseconds;
}

void CMIDCustomItem::ConstructL(const TDesC& aLabel)
{
    DEBUG("< CMIDCustomItem::ConstructL");

    CMIDControlItem::ConstructL();
    SetLabelL(aLabel);
    UpdateMargins();
    ActivateL();

    // CustomItem listens media key events if CMIDRemConObserver is initialized and media keys keys are enabled
    ASSERT(iMenuHandler);
    ASSERT(iUIManager);
    iKeyDecoder = iUIManager->OpenKeyDecoderL();
    ASSERT(iKeyDecoder);
    iRemConObserver = iKeyDecoder->GetRemConObserver();
    if (iRemConObserver && iKeyDecoder->MediaKeysEnabled())
    {
        iRemConObserver->AddMediaKeysListenerL(static_cast<MMIDMediaKeysListener*>(this));
    }

#ifdef RD_TACTILE_FEEDBACK
    iTactileFeedback = new(ELeave) CMIDTactileFeedbackExtension(this, 2);
#endif
    //Create PointerEventSuppressor with default values or JAD parameter defined values
    iPointerEventSuppressor = CAknPointerEventSuppressor::NewL();
    TInt pointerMovementInPixels =
        ControlEnv()->ScreenDevice()->HorizontalTwipsToPixels(iPESPointerMovementInTwips);
    TSize suppressorValues =
        iUIManager->ReadPointerEventSuppressorValues();
    if (!(suppressorValues.iWidth == KPESErrorValue &&
            suppressorValues.iHeight == KPESErrorValue))
    {
        iPESTimeInMilliseconds = suppressorValues.iHeight;
        pointerMovementInPixels =
            ControlEnv()->ScreenDevice()->HorizontalTwipsToPixels(suppressorValues.iWidth);
    }
    iPointerEventSuppressor->SetMaxTapDuration(iPESTimeInMilliseconds * 1000);
    iPointerEventSuppressor->SetMaxTapMove(TSize(pointerMovementInPixels,
                                           pointerMovementInPixels));

    DEBUG("> CMIDCustomItem::ConstructL");
}

void  CMIDCustomItem::UpdateMargins()
{
    TAknWindowLineLayout layout =
        AknLayoutScalable_Avkon::form2_midp_content_pane(1).LayoutLine();

    iContentMargins.iTop    = layout.it;
    iContentMargins.iLeft   = layout.il;
    iContentMargins.iRight  = layout.ir;
    iContentMargins.iBottom = layout.ib;

}

/** */
void CMIDCustomItem::ResetBuffers()
{
    delete iFrameBuffer;
    delete iFrameContext;
    delete iFrameDevice;

    iFrameBuffer  = NULL;
    iFrameContext = NULL;
    iFrameDevice  = NULL;

    delete iOffScreenBuffer;
    delete iOffScreenContext;
    delete iOffScreenDevice;

    iOffScreenBuffer  = NULL;
    iOffScreenContext = NULL;
    iOffScreenDevice  = NULL;
}

/** Creates the frame buffer with its associated device and context and the
offscreen buffer with its device and context. They are both of the same size,
the size of the content rect. The skin background is then printed on the
frame buffer via the frame context.

Note: This method is called each time the content size changes. If it leaves
it is trapped in SizeChanged() and then ResetBuffers() is called so that we
are not left in an unconsistent situation in Out Of Memory. So care must be
taken in the code to always check that the buffers or their devices or contextes
do exist - because they may not exist in OOM.
*/
void CMIDCustomItem::CreateBuffersL()
{
    DEBUG("< CMIDCustomItem::CreateBuffersL");
    ResetBuffers();

    TRect rect = TRect(0,0,iContentRect.Size().iWidth, iContentRect.Size().iHeight);

    iFrameBuffer = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iFrameBuffer->Create(rect.Size(), iEnv.DisplayMode()));

    iFrameDevice = CFbsBitmapDevice::NewL(iFrameBuffer);
    User::LeaveIfError(iFrameDevice->CreateContext(iFrameContext));

    iOffScreenBuffer = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iOffScreenBuffer->Create(rect.Size(), iEnv.DisplayMode()));

    iOffScreenDevice = CFbsBitmapDevice::NewL(iOffScreenBuffer);
    User::LeaveIfError(iOffScreenDevice->CreateContext(iOffScreenContext));

    if (iForm)
    {
        AknsDrawUtils::DrawBackground(AknsUtils::SkinInstance(),
                                      iForm->CurrentDisplayable().BackGroundControlContext(), this,
                                      *iFrameContext, TPoint(0,0), iContentRect, KAknsDrawParamDefault);
    }
    DEBUG("> CMIDCustomItem::CreateBuffersL");
}

// Start Direct screen access in order to switch off Transition effects,
// which could damage the DSA (video)
void CMIDCustomItem::SetContainerWindowL(const CCoeControl& aWindow)
{
    DEBUG("< CMIDCustomItem::SetContainerWindowL");
#ifndef RD_JAVA_NGA_ENABLED
    if (iDirectAccess)
    {
        iDirectAccess->Cancel();
        delete iDirectAccess;
        iDirectAccess = NULL;
    }
#endif

    CMIDControlItem::SetContainerWindowL(aWindow);

#ifndef RD_JAVA_NGA_ENABLED
    RWsSession&      session = ControlEnv()->WsSession();
    CWsScreenDevice* device  = ControlEnv()->ScreenDevice();

    iDirectAccess = CDirectScreenAccess::NewL(
                        session,
                        *device,
                        *DrawableWindow(),
                        *this
                    );
#endif

    if (iConsumerWaitingForDSAResourcesCallback)
    {
        // Invoke the pending callback from LCDUI ES thread
        iEnv.ToLcduiObserver().InvokeDSAResourcesCallback(
            *this, *iConsumerWaitingForDSAResourcesCallback);
        iConsumerWaitingForDSAResourcesCallback = NULL;
    }

    DEBUG("> CMIDCustomItem::SetContainerWindowL");
}

CMIDCustomItem::~CMIDCustomItem()
{
    DEBUG("< CMIDCustomItem::~CMIDCustomItem");
    if (iDirectContent)
    {
        iDirectContent->MdcContainerDestroyed();
    }

    if (iUiFixed && iForm)
    {
        iForm->CurrentDisplayable().ReleaseOrientation();
        iUiFixed = EFalse;
    }

#ifndef RD_JAVA_NGA_ENABLED
    if (iDirectAccess)
    {
        iDirectAccess->Cancel();
        delete iDirectAccess;
        iDirectAccess = NULL;
    }
#endif

    ResetBuffers();

    // CustomItem removes itself from the CMIDRemConObserver.
    ASSERT(iKeyDecoder);
    if (iRemConObserver && iKeyDecoder->MediaKeysEnabled())
    {
        iRemConObserver->RemoveMediaKeysListener(static_cast<MMIDMediaKeysListener*>(this));
    }
#ifdef RD_TACTILE_FEEDBACK
    delete iTactileFeedback;
#endif

    if (iPointerEventSuppressor)
    {
        delete iPointerEventSuppressor;
        iPointerEventSuppressor = NULL;
    }

    DEBUG("> CMIDCustomItem::~CMIDCustomItem");
}

/**
 *
 */
TInt CMIDCustomItem::ItemPreferredHeightWithoutLabel()
{
    return PreferredContentSize().iHeight + iContentMargins.iTop + iContentMargins.iBottom;
}

/**
 * Gets the available interaction modes. This method is intended to be called by
 * CustomItem subclass code in order for it to determine what kinds of input are
 * available from this device
 */
TInt CMIDCustomItem::InteractionModes() const
{
    DEBUG("< CMIDCustomItem::InteractionModes");
    TInt modes = 0;

    if (AknLayoutUtils::PenEnabled())
    {
        // Drag is not supported if physics scrolling is in use
        if (CMIDFormPhysics::FeatureEnabled())
        {
            modes |= ETraverseHorizontal | ETraverseVertical |
                     EPointerPress | EPointerRelease;
        }
        else
        {
            modes |= ETraverseHorizontal | ETraverseVertical |
                     EPointerPress | EPointerRelease | EPointerDrag;
        }
    }

    TRAP_IGNORE(modes |= KeyboardInteractionModesL());

    DEBUG_INT("> CMIDCustomItem::InteractionModes = %d", modes);
    return modes;
}

TInt CMIDCustomItem::KeyboardInteractionModesL() const
{
    DEBUG("< CMIDCustomItem::KeyboardInteractionModesL");
    TInt modes = 0;

#ifdef RD_INTELLIGENT_TEXT_INPUT
    RArray<TPtiKeyboardType> keyboards;
    CleanupClosePushL(keyboards);

    CPtiEngine* ptiEngine = CPtiEngine::NewL();
    CleanupStack::PushL(ptiEngine);

    ptiEngine->ListAvailablePhysicalKeyboardsL(keyboards);

    if (keyboards.Count() > 0)
    {
        modes |= ETraverseHorizontal | ETraverseVertical |
                 EKeyPress | EKeyRelease | EKeyRepeat;
    }

    CleanupStack::PopAndDestroy(ptiEngine);
    CleanupStack::PopAndDestroy(&keyboards);
#endif //RD_INTELLIGENT_TEXT_INPUT    

    DEBUG_INT("> CMIDCustomItem::KeyboardInteractionModesL = %d", modes);
    return modes;
}

TBool CMIDCustomItem::SupportsInternalTraversal() const
{
    return iSupportsInternalTraversal;
}

/**
 * This is a return value from Traverse method in Java.
 * It returns whether it should traverse within the CustomItem and Rect that it should
 * traverse to until it returns false and traverse out of CustomItem
 */
void CMIDCustomItem::SetFocusAndScroll(TBool aFocus,const TRect* aRect,TInt aDirection)
{
    DEBUG("< CMIDCustomItem::SetFocusAndScroll");
    // Make sure we know if this CustomItem subclass supports internal traversal.

    // When the item is entered for the very first time, check what the traverse
    // method returned. If false, there is no internal traversal and we will
    // handle this based on aFocus.
    // If true, the item supports internal traversal and additional code is needed
    if (!iEntered)
    {
        // iSupportsInternalTraversal is sampled on the first call only
        iEntered = ETrue;
        iSupportsInternalTraversal = aFocus;
        // this will ensure that we get the focus on first traverse
        aFocus = ETrue;

        // Form must be updated when the focus enters to the non-traversal CustomItem for the first time.
        // The reason is that the default value of iSupportsInternalTraversal is ETrue,
        // which means that CMIDCustomItem::Draw() function has not updated focus before.
        if (iForm && !iSupportsInternalTraversal)
        {

            iForm->DrawNow();
        }
    }

    // Handle focus for items that do not support internal traversal
    if (!iSupportsInternalTraversal)
    {
        // The focus is passed from java side
        if (!aFocus && iForm)
        {
            iForm->Traverse((CMIDForm::TDirection)aDirection);
        }
        return;
    }

    // This calculates top/bottom rect for Form to scroll to.
    if (aRect && aFocus && iForm)
    {
        TRect  view = iForm->GetClientArea();

        if (aRect->iTl.iY < view.iTl.iY) //top is off screen
        {
            // scroll up so that the focused area top is at the screen top
            TInt scrollUp = view.iTl.iY - aRect->iTl.iY ;
            if (iForm)
            {
                iForm->RawScroll(scrollUp);
            }
        }
        else if (aRect->iBr.iY > view.iBr.iY) //bottom is off screen
        {
            // scroll down so that the focused area bottom is at the screen bottom
            TInt scrollDown = view.iBr.iY - aRect->iBr.iY ;
            if (iForm)
            {
                iForm->RawScroll(scrollDown);
            }
        }
    }


    // If losing focus, tell the Form to advance focus
    //
    if (!aFocus && iForm)
    {
        iForm->Traverse((CMIDForm::TDirection)aDirection);
    }

    DEBUG("> CMIDCustomItem::SetFocusAndScroll");
}

CFbsBitmap* CMIDCustomItem::FrameBuffer() const
{
    return iFrameBuffer;
}

void CMIDCustomItem::SetLabelL(const TDesC& aLabel)
{
    CMIDControlItem::SetLabelL(aLabel);
    SetPreferredSizeL(iRequestedPreferredSize);
}

/** */
void CMIDCustomItem::SetPreferredSizeL(const TSize& aSize)
{
    DEBUG("< CMIDCustomItem::SetPreferredSizeL");
    DEBUG_INT2("+ CMIDCustomItem::SetPreferredSizeL - requested size = ( %d x %d )", aSize.iWidth, aSize.iHeight);

    iRequestedPreferredSize = CheckRequestedSize(aSize);

    if (iRequestedPreferredSize.iWidth == -1)
    {
        // Width is not locked
        iPreferredSize.iWidth = Min(FormClientAreaWidth(),
                                    PreferredContentSize().iWidth + iContentMargins.iLeft + iContentMargins.iRight);
    }
    else
    {
        iPreferredSize.iWidth  = Min(FormClientAreaWidth(),
                                     Max(MinimumSize().iWidth, iRequestedPreferredSize.iWidth));
    }

    if (iLabelControl)
    {
        iLabelControl->SetWidthL(iPreferredSize.iWidth);
    }


    if (iRequestedPreferredSize.iHeight == -1)
    {
        // Height is not locked
        iPreferredSize.iHeight = PreferredContentSize().iHeight + iContentMargins.iTop
                                 + iContentMargins.iBottom + LabelHeight();
    }
    else
    {
        iPreferredSize.iHeight = Max(MinimumSize().iHeight, iRequestedPreferredSize.iHeight);
    }

    DEBUG_INT2("+ CMIDCustomItem::SetPreferredSizeL - preferred size = ( %d x %d )", iPreferredSize.iWidth, iPreferredSize.iHeight);

    TRAP_IGNORE(AdjustToSizeL(iPreferredSize));

    DEBUG("> CMIDCustomItem::SetPreferredSizeL");
}

/**
* Parameter aSize includes margins
*/
void CMIDCustomItem::AdjustToSizeL(const TSize& aSize)
{
    DEBUG("< CMIDCustomItem::AdjustToSizeL");

    TInt availableHeight = aSize.iHeight - iContentMargins.iTop - iContentMargins.iBottom;
    DEBUG_INT("+ CMIDCustomItem::AdjustToSizeL - available height = %d", availableHeight);
    TInt requestedHeight = PreferredContentSize().iHeight + LabelHeight();
    DEBUG_INT("+ CMIDCustomItem::AdjustToSizeL - requested height = %d", requestedHeight);

    if (requestedHeight > availableHeight)
    {
        // label + control do not fit
        if (iLabelControl && iLabelControl->Text()->Length() > 0)
        {
            //reserve one line to the control
            TInt heightForLabel =
                ((availableHeight - PreferredContentSize().iHeight) > OneLineLabelHeight()) ?
                availableHeight - PreferredContentSize().iHeight :
                availableHeight - MinimumContentSize().iHeight;

            //By setting a temporary max number of lines and then calling
            //SetWidthL() we limit the number of lines to the temporary max number
            //However then the max number must be resetted
            TInt oldMaxNumLabelLines = iLabelControl->MaxNumLines();
            iLabelControl->SetMaxNumLines(
                (heightForLabel - iLabelControl->ItemLabelMargin()) / iLabelControl->LineHeight());
            iLabelControl->SetWidthL(aSize.iWidth);
            iLabelControl->SetMaxNumLines(oldMaxNumLabelLines);
        }
    }
    DEBUG("> CMIDCustomItem::AdjustToSizeL");
}

/**
 * Updates CustomItem with new MinContentSize and PrefContentSize received
 * from CustomItem subclass
 *
 * Parameters doesn't include margins. iMinimumContentSize and iPreferredContentSize
 * doesn't include margins.
 */
void CMIDCustomItem::Invalidate(const TSize& aMinSize, const TSize& aPrefSize)
{
    DEBUG("< CMIDCustomItem::Invalidate");

    iMinimumContentSize = CheckRequestedSize(aMinSize);

    if (iMinimumContentSize.iWidth < 1)
    {
        iMinimumContentSize.iWidth = 1;
    }
    if (iMinimumContentSize.iHeight < 1)
    {
        iMinimumContentSize.iHeight = 1;
    }

    TInt maxContentWidth = FormClientAreaWidth() - iContentMargins.iLeft -
                           iContentMargins.iRight;

    if (iMinimumContentSize.iWidth > maxContentWidth)
    {
        iMinimumContentSize.iWidth = maxContentWidth;
    }

    iPreferredContentSize = CheckRequestedSize(aPrefSize);

    if (iPreferredContentSize.iWidth < iMinimumContentSize.iWidth)
    {
        iPreferredContentSize.iWidth = iMinimumContentSize.iWidth;
    }
    if (iPreferredContentSize.iHeight < iMinimumContentSize.iHeight)
    {
        iPreferredContentSize.iHeight = iMinimumContentSize.iHeight;
    }

    if (iPreferredContentSize.iWidth > maxContentWidth)
    {
        iPreferredContentSize.iWidth = maxContentWidth;
    }

    if (iForm)
    {
        TRAP_IGNORE(iForm->RequestLayoutL());
    }

    DEBUG("> CMIDCustomItem::Invalidate");
}

MMIDBufferProcessor* CMIDCustomItem::Processor()
{
    return this;
}

void CMIDCustomItem::Dispose()
{
    delete this;
}

void CMIDCustomItem::AddCommandL(MMIDCommand* aCommand)
{
    CMIDItem::AddCommandL(aCommand);
}

void CMIDCustomItem::RemoveCommand(MMIDCommand* aCommand)
{
    CMIDItem::RemoveCommand(aCommand);
}

void CMIDCustomItem::SetDefaultCommand(MMIDCommand* aCommand)
{
    CMIDItem::SetDefaultCommand(aCommand);
}

TSize CMIDCustomItem::PreferredSize() const
{
    return iPreferredSize;
}

TSize CMIDCustomItem::MinimumSize() const
{
    DEBUG("< CMIDCustomItem::MinimumSize");
    TSize minimumSize = MinimumContentSize() +
                        TSize(iContentMargins.iLeft + iContentMargins.iRight,
                              iContentMargins.iTop + iContentMargins.iBottom);

    if (iLabelControl && (iLabelControl->Text()->Length() > 0))
    {
        minimumSize.iHeight += OneLineLabelHeight();
    }
    DEBUG_INT2("> CMIDCustomItem::MinimumSize = ( %d x %d )", minimumSize.iWidth, minimumSize.iHeight);
    return minimumSize;
}

void CMIDCustomItem::SetLayoutL(TLayout aLayout)
{
    CMIDItem::SetLayoutL(aLayout);
}

const TSize& CMIDCustomItem::MinimumContentSize() const
{
    return iMinimumContentSize;
}

const TSize& CMIDCustomItem::PreferredContentSize() const
{
    return iPreferredContentSize;
}

void CMIDCustomItem::AbortAsync()
{
    // nop
}

enum TOpCode
{
    ESync,
    ESyncRect
};

/** Copy the content (iFrameBuffer) into iOffScreenBuffer and then draw.
Either do a full copy or copy only the clipped area.
*/
#ifdef RD_JAVA_NGA_ENABLED
TBool CMIDCustomItem::ProcessL(
    const TMIDBufferOp*& aOp, const TMIDBufferOp* aEnd,
    TInt& /*aCycles*/, java::util::Monitor* /*aMonitor*/)
#else
TBool CMIDCustomItem::ProcessL(
    const TMIDBufferOp*& aOp, const TMIDBufferOp* aEnd,
    TInt& /*aCycles*/, TRequestStatus* /*aStatus*/)
#endif
{
    DEBUG("< CMIDCustomItem::ProcessL");

    // after coping iFrameBuffer to iOffScreenContext
    // iFrameBuffer should be reset in Draw()
    iResetFrameBuffer = ETrue;

    while (aOp< aEnd)
    {
        switch (aOp->OpCode())
        {
        case ESync:
            if (iFrameBuffer && iOffScreenContext)
            {
                iOffScreenContext->BitBlt(TPoint(0,0), iFrameBuffer);
            }
            DrawNow();

            break;

        case ESyncRect:
            TRect clip = *static_cast<const TRect*>(aOp->Data());

            if (iFrameBuffer && iOffScreenContext)
            {
                iOffScreenContext->BitBlt(clip.iTl, iFrameBuffer, clip);
            }

            clip.Move(iContentRect.iTl);
            clip.Intersection(Rect());

            ActivateGc();
            Draw(clip);
            DeactivateGc();

            break;
        }
        aOp += aOp->Size();
    }

    // disable resetting of iFrameBuffer (it has been reset already in Draw())
    iResetFrameBuffer = EFalse;

    DEBUG("> CMIDCustomItem::ProcessL");
    return EFalse;
}

TSize CMIDCustomItem::MinimumSize()
{
    return ((MMIDCustomItem*)this)->MinimumSize();
}

TInt CMIDCustomItem::CountComponentControls() const
{
    return 1;
}

CCoeControl* CMIDCustomItem::ComponentControl(TInt aIndex) const
{
    switch (aIndex)
    {
    case 0:
        return iLabelControl;
    }
    return NULL;
}

#ifndef RD_JAVA_NGA_ENABLED
void CMIDCustomItem::PauseDirectAccess()
{
    if (iDirectAccess)
    {
        StopDirectAccess();
        iDirectPaused = ETrue;
    }
}


void CMIDCustomItem::ResumeDirectAccess()
{
    if (iDirectAccess)
    {
        iDirectPaused = EFalse;
        StartDirectAccess();
    }
}

void CMIDCustomItem::StartDirectAccess()
{
    DEBUG("< CMIDCustomItem::StartDirectAccess");

    if (!iDirectPaused && !iDirectAccess->IsActive())
    {
        TRAPD(err, iDirectAccess->StartL());
        if (KErrNone != err)
        {
            iDirectAccess->Cancel();
            return;
        }

        if (iDirectContent)
        {
            // Check that the dc region is fully visible
            // to prevent covering of external dialogs.
            RRegion* dsaRegion = iDirectAccess->DrawingRegion();

            // Form must have four rectangles on drawing region
            // (form area minus scroll bar)
            if (dsaRegion->Count() == 4)
            {
                TRect itemRect(MdcContentBounds());
                RRegion dcRegion(itemRect);

                dcRegion.Intersect(*dsaRegion);
                dcRegion.Tidy();
                TBool dsaRegionOk = dcRegion.Count() == 1 &&
                                    dcRegion[0] == itemRect;
                dcRegion.Close();

                if (iDirectContent && dsaRegionOk)
                {
                    // Notify DirectContent to resume DSA
                    iDirectContent->MdcResumeDSA();
                }
            }
        }
    }

    DEBUG("> CMIDCustomItem::StartDirectAccess");
}
void CMIDCustomItem::StopDirectAccess()
{
    if (iDirectContent)
    {
        // Notify DirectContent to abort DSA
        iDirectContent->MdcAbortDSA();
    }

    if (iDirectAccess)
    {
        iDirectAccess->Cancel();
    }
}

// MDirectScreenAccess
void CMIDCustomItem::Restart(RDirectScreenAccess::TTerminationReasons /*aReasons*/)
{
    DEBUG("< CMIDCustomItem::Restart");
    if (iDirectAccess)
    {
        StartDirectAccess();
    }
    DEBUG("> CMIDCustomItem::Restart");
}

// MDirectScreenAccess
void CMIDCustomItem::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReasons*/)
{
    DEBUG("< CMIDCustomItem::AbortNow");

    if (iDirectAccess)
    {
        StopDirectAccess();
    }

    DEBUG("> CMIDCustomItem::AbortNow");
}
#endif


/** First draws the skin background and the border. Then draw the
content. The content is double buffered, hence it must have already
been copied into iOffScreenBuffer. Finally reset the frame buffer by
drawing the skin background.

@see: ProcessL() - which copies the content into iOffScreenBuffer
*/
void CMIDCustomItem::Draw(const TRect& aRect) const
{
    DEBUG("< CMIDCustomItem::Draw");

    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    if (!SupportsInternalTraversal())
    {
        // doesn't support internal traversal - CMIDControlItem draws background
        CMIDControlItem::Draw(aRect);
    }
    else
    {
        // does support internal traversal, draw background itself
        AknsDrawUtils::Background(skin, iForm->CurrentDisplayable().BackGroundControlContext(),
                                  this, gc, aRect);
    }

    if (iOffScreenBuffer)
    {
        gc.BitBlt(iContentRect.iTl, iOffScreenBuffer);
    }

    if (iForm && iFrameContext && iResetFrameBuffer)
    {
        MAknsControlContext* cc = !SupportsInternalTraversal() ? AknsDrawUtils::ControlContext(this) :
                                  iForm->CurrentDisplayable().BackGroundControlContext();

        // Don't do the following action in case of something is rendered into offscreen buffer (GIF)
        if (!iDirectContent)
        {
            AknsDrawUtils::DrawBackground(skin, cc,
                                          this, *iFrameContext, TPoint(0,0), iContentRect, KAknsDrawParamDefault);
        }
    }
    DEBUG("> CMIDCustomItem::Draw");
}

#ifdef RD_SCALABLE_UI_V2
void CMIDCustomItem::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
    DEBUG("< CMIDCustomItem::HandlePointerEventL");

    if (AknLayoutUtils::PenEnabled())
    {
        if (!iForm->TryDetectLongTapL(aPointerEvent))
        {
            TRect drawableRect = Rect();
            drawableRect.SetHeight(
                drawableRect.Height() - iContentMargins.iTop -
                iContentMargins.iBottom - iLabelControl->Size().iHeight);
            drawableRect.SetWidth(drawableRect.Width() -
                                  iContentMargins.iLeft - iContentMargins.iRight);
            drawableRect.Move(iContentMargins.iLeft,
                              iContentMargins.iTop + iLabelControl->Size().iHeight);
            TPoint point = aPointerEvent.iPosition;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
            if (!drawableRect.Contains(point) && !iForm->IsFocusChangingWithPen())
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
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

            point.iX = point.iX - drawableRect.iTl.iX;
            point.iY = point.iY - drawableRect.iTl.iY;
            TEventType type;
            switch (aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
            case TPointerEvent::EButton2Down:
            case TPointerEvent::EButton3Down:
                type = EPointerPressed;
                iPointerPressedCoordinates = TPoint(point.iX, point.iY);
                break;
            case TPointerEvent::EButton1Up:
            case TPointerEvent::EButton2Up:
            case TPointerEvent::EButton3Up:
                type = EPointerReleased;
                if (iPointerEventSuppressionOngoing)
                {
                    point.iX = iPointerPressedCoordinates.iX;
                    point.iY = iPointerPressedCoordinates.iY;
                }
                break;
            case TPointerEvent::EDrag:
                type = EPointerDragged;
                break;
            default:
                type = ENoType;
            }

            if (iPointerEventSuppressor->SuppressPointerEvent(aPointerEvent))
            {
                iPointerEventSuppressionOngoing = ETrue;
                return;
            }
            else
            {
                iPointerEventSuppressionOngoing = EFalse;
            }

            iEnv.PostJavaEvent(*this, EItem, type, point.iX, point.iY, 0);
        }
    }

    DEBUG("> CMIDCustomItem::HandlePointerEventL");
}
#endif //RD_SCALABLE_UI_V2


void CMIDCustomItem::HandleCurrentL(TBool aCurrent)
{
    DEBUG("< CMIDCustomItem::HandleCurrentL");
    if (aCurrent)
    {
        PostFocusTransferEvent(ETrue, CMIDForm::ENone);
    }
    DEBUG("> CMIDCustomItem::HandleCurrentL");
}


void CMIDCustomItem::SizeChanged()
{
    DEBUG("< CMIDCustomItem::SizeChanged");

    const TRect oldRect = iContentRect;
    DEBUG_INT2("+ CMIDCustomItem::SizeChanged - old size = ( %d x %d )", oldRect.Width(), oldRect.Height());

    // Rect() returns control area including content, margins and possible label
    // iContentRect is the item content area exluding margins
    iContentRect = Rect();

    if (iLabelControl && iLabelControl->Text()->Length())
    {
        TRect rect(iContentRect);
        rect.iBr.iY = rect.iTl.iY + LabelHeight();
        iLabelControl->SetRect(rect);
        // Content
        iContentRect.iTl.iY += LabelHeight();
    }

    iContentRect.SetRect(
        iContentRect.iTl.iX + iContentMargins.iLeft,
        iContentRect.iTl.iY + iContentMargins.iTop,
        iContentRect.iBr.iX - iContentMargins.iRight,
        iContentRect.iBr.iY - iContentMargins.iBottom);

    if (oldRect != iContentRect)
    {
#ifndef RD_JAVA_NGA_ENABLED
        // Pause DSA during framebuffers are being changed
        PauseDirectAccess();
#endif
        const TSize canvasSize(iContentRect.Size());
        DEBUG_INT2("+ CMIDCustomItem::SizeChanged - new size = ( %d x %d )", canvasSize.iWidth, canvasSize.iHeight);
        if (!iFrameBuffer || canvasSize != iFrameBuffer->SizeInPixels())
        {
            TRAPD(err, CreateBuffersL());

            if (err != KErrNone)
            {
                ResetBuffers();
            }
        }

        CMIDControlItem::SizeChanged();

        //
        // send SizeChanged event only if the size changes
        //
        if (oldRect.Size() != canvasSize)
        {
            iEnv.PostJavaEvent(*this, EItem, ESizeChanged, canvasSize.iWidth , canvasSize.iHeight, 0);
        }
#ifdef RD_TACTILE_FEEDBACK
        UpdateTactileFeedback();
#endif

        if (iDirectContent && iForm)
        {
            // Store the window position to be used later
            // from non-LCDUI ES thread
            iLastWindowPosition = DrawableWindow()->Position();

            if (iDirectAreaAddedToDisplayable)
            {
                // Update the direct content area according the new size
                UpdateDirectContentBounds();
            }

            TRect itemRect(iContentRect);
            itemRect.Move(iLastWindowPosition);
            iDirectContent->MdcItemContentRectChanged(itemRect,MdcContentBounds());
        }

#ifndef RD_JAVA_NGA_ENABLED
        if (MdcContainerVisibility())
        {
            ResumeDirectAccess();
        }
#endif
    }
    DEBUG("> CMIDCustomItem::SizeChanged");
}

void CMIDCustomItem::PositionChanged()
{
    DEBUG("< CMIDCustomItem::PositionChanged");
    iContentRect = Rect();
    iContentRect.iTl.iY += LabelHeight();

    iContentRect.SetRect(
        iContentRect.iTl.iX + iContentMargins.iLeft,
        iContentRect.iTl.iY + iContentMargins.iTop,
        iContentRect.iBr.iX - iContentMargins.iRight,
        iContentRect.iBr.iY - iContentMargins.iBottom);

    if (iDirectContent && iForm)
    {
        // Store the window position to be used later
        // from non-LCDUI ES thread
        iLastWindowPosition = DrawableWindow()->Position();

        if (iDirectAreaAddedToDisplayable)
        {
            // Update the direct content area according the new size
            UpdateDirectContentBounds();
        }

        TRect itemRect(iContentRect);
        itemRect.Move(iLastWindowPosition);
        iDirectContent->MdcItemContentRectChanged(itemRect,MdcContentBounds());
    }
    DEBUG("> CMIDCustomItem::PositionChanged");
}

TKeyResponse CMIDCustomItem::OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType)
{
    DEBUG("< CMIDCustomItem::OfferKeyEventL");

    DEBUG_INT("+ CMIDCustomItem::OfferKeyEventL - received: iCode = %d", aEvent.iCode);
    DEBUG_INT("+ CMIDCustomItem::OfferKeyEventL - received: iModifiers = %d", aEvent.iModifiers);
    DEBUG_INT("+ CMIDCustomItem::OfferKeyEventL - received: iRepeats = %d", aEvent.iRepeats);
    DEBUG_INT("+ CMIDCustomItem::OfferKeyEventL - received: iScanCode = %d", aEvent.iScanCode);

    // map key event data
    TKeyEvent wsEvent = aEvent;
    iEnv.MappingDataForKey(wsEvent, aType);

    TMIDKeyEvent event;
    if (iEnv.TranslateKeyL(event, wsEvent, aType))
    {
        DEBUG_INT("+ CMIDCustomItem::OfferKeyEventL - translated: iEvents = %d", event.iEvents);
        DEBUG_INT("+ CMIDCustomItem::OfferKeyEventL - translated: iKeyCode = %d", event.iKeyCode);
        DEBUG_INT("+ CMIDCustomItem::OfferKeyEventL - translated: iRepeats = %d", event.iRepeats);

        // 9-way Navigation Support
        // Send two arrow events generated when press diagonal direction
        switch (event.iKeyCode)
        {
            //KeyLeftUpArrow
        case EKeyLeftUpArrow:
        {
            TKeyEvent pevent;
            pevent.iCode = EKeyUpArrow;
            pevent.iScanCode = EStdKeyUpArrow;
            pevent.iModifiers = aEvent.iModifiers;
            pevent.iRepeats = event.iRepeats;
            this->OfferKeyEventL(pevent, aType);
            pevent.iCode = EKeyLeftArrow;
            pevent.iScanCode = EStdKeyLeftArrow;
            pevent.iModifiers = aEvent.iModifiers;
            pevent.iRepeats = event.iRepeats;
            this->OfferKeyEventL(pevent, aType);

            break;
        }
        //KeyRightUpArrow
        case EKeyRightUpArrow:
        {
            TKeyEvent pevent;
            pevent.iCode = EKeyUpArrow;
            pevent.iScanCode = EStdKeyUpArrow;
            pevent.iModifiers = aEvent.iModifiers;
            pevent.iRepeats = event.iRepeats;
            this->OfferKeyEventL(pevent, aType);
            pevent.iCode = EKeyRightArrow;
            pevent.iScanCode = EStdKeyRightArrow;
            pevent.iModifiers = aEvent.iModifiers;
            pevent.iRepeats = event.iRepeats;
            this->OfferKeyEventL(pevent, aType);

            break;
        }
        //KeyRightDownArrow
        case EKeyRightDownArrow:
        {
            TKeyEvent pevent;
            pevent.iCode = EKeyDownArrow;
            pevent.iScanCode = EStdKeyDownArrow;
            pevent.iModifiers = aEvent.iModifiers;
            pevent.iRepeats = event.iRepeats;
            this->OfferKeyEventL(pevent, aType);
            pevent.iCode = EKeyRightArrow;
            pevent.iScanCode = EStdKeyRightArrow;
            pevent.iModifiers = aEvent.iModifiers;
            pevent.iRepeats = event.iRepeats;
            this->OfferKeyEventL(pevent, aType);

            break;
        }
        //KeyLeftDownArrow
        case EKeyLeftDownArrow:
        {
            TKeyEvent pevent;
            pevent.iCode = EKeyDownArrow;
            pevent.iScanCode = EStdKeyDownArrow;
            pevent.iModifiers = aEvent.iModifiers;
            pevent.iRepeats = event.iRepeats;
            this->OfferKeyEventL(pevent, aType);
            pevent.iCode = EKeyLeftArrow;
            pevent.iScanCode = EStdKeyLeftArrow;
            pevent.iModifiers = aEvent.iModifiers;
            pevent.iRepeats = event.iRepeats;
            this->OfferKeyEventL(pevent, aType);

            break;
        }
        default:
            iEnv.PostKeyEvent(*this, event);
        }//switch
    }
    //  }

    if (aType != EEventKey)
        return EKeyWasNotConsumed;

    if (!SupportsInternalTraversal() || aEvent.iCode == EKeyDevice3)
    {
        DEBUG("> CMIDCustomItem::OfferKeyEventL - returns EKeyWasNotConsumed");
        return EKeyWasNotConsumed;
    }

    CMIDForm::TDirection direction = TranslateKey(aEvent);
    if (direction != CMIDForm::ENone)
    {
        PostFocusTransferEvent(ETrue, direction);
    }

    DEBUG("> CMIDCustomItem::OfferKeyEventL - returns EKeyWasConsumed");
    return EKeyWasConsumed;
}

TSize CMIDCustomItem::ResetPreferredSize() const
{
    TRAP_IGNORE(const_cast<CMIDCustomItem*>(this)->SetPreferredSizeL(iRequestedPreferredSize));
    return iPreferredSize;
}

/** If the specified height or width or the requested size are bigger that the
screen height or width times KCIMaxScreenSizeFactor then return these limit values.
Otherwise return the size unchanged. @see KCIMaxScreenSizeFactor */
TSize CMIDCustomItem::CheckRequestedSize(const TSize& aRequestedSize) const
{
    DEBUG("< CMIDCustomItem::CheckRequestedSize");
    TSize size = aRequestedSize;

    const TRect formClientAreaRect = FormClientAreaRect();

    size.iWidth = size.iWidth < KCIMaxScreenSizeFactor * formClientAreaRect.Width() ?
                  size.iWidth : KCIMaxScreenSizeFactor * formClientAreaRect.Width();

    size.iHeight = size.iHeight < KCIMaxScreenSizeFactor * formClientAreaRect.Height() ?
                   size.iHeight : KCIMaxScreenSizeFactor * formClientAreaRect.Height();

    DEBUG_INT2("> CMIDCustomItem::CheckRequestedSize = ( %d x %d )", size.iWidth, size.iHeight);
    return size;
}

void CMIDCustomItem::FocusChanged(TDrawNow aDrawNow)
{
    DEBUG("< CMIDCustomItem::FocusChanged");
    if (iForm)
    {
        //Item is on Form:
        //Check if CustomItem changed its visibility (previous state is
        //stored in iPreviousVisibility).
        //current visibility:
        TBool contentRectVisible = MdcContainerVisibility();
        if (iPreviousVisibility != contentRectVisible)
        {
            //Event is sent only if CustomItem changed visibility:
            //1. CustomItem is now at least partially visible,
            //   when it previously had been completely invisible
            //   -> CustomItem gains visibility,
            //      showNotify is invoked on java side
            //2. CustomItem is now completely invisible, when it previously
            //   had been at least partially visible
            //   -> CustomItem loses visibility,
            //      hideNotify is invoked on java side
            iEnv.PostJavaEvent(*this, EItem, EVisible, contentRectVisible);

            if (!iRestoreDirectContentWhenUnfaded)
            {
                ChangeDirectContainerVisibility(contentRectVisible);
            }

            //Current status of visibility is always stored
            iPreviousVisibility = contentRectVisible;
        }
    }

    CMIDControlItem::FocusChanged(aDrawNow);

    DEBUG("> CMIDCustomItem::FocusChanged");
}

void CMIDCustomItem::ChangeDirectContainerVisibility(TBool aVisible)
{
    DEBUG("< CMIDCustomItem::ChangeDirectContainerVisibility");
    if (iForm)
    {
        iLastWindowPosition = DrawableWindow()->Position();
    }

#ifndef RD_JAVA_NGA_ENABLED
    if (iDirectAccess && (iDirectPaused == aVisible))
    {
        if (aVisible)
        {
            ResumeDirectAccess();
        }
        else
        {
            PauseDirectAccess();
        }
    }
#endif

    if (iDirectContent)
    {
        // Restore direct content
        iDirectContent->MdcContainerVisibilityChanged(aVisible);
    }

    DEBUG("> CMIDCustomItem::ChangeDirectContainerVisibility");
}

/** Handles change in displaying item - calls same
events as in focus change.
*/
void CMIDCustomItem::HandleVisibilityChange()
{
    FocusChanged(ENoDrawNow);
}

void CMIDCustomItem::SetForm(CMIDForm* aForm)
{
    DEBUG("< CMIDCustomItem::SetForm");
    CMIDItem::SetForm(aForm);

    if (iForm)
    {
        // Set S60 Selection Key Compatibility
        // to provide MSK key events to canvas
        iS60SelectionKeyCompatibility = iEnv.MidletAttributeIsSetToVal(
                                            LcduiMidletAttributes::KAttribS60SelectionKeyCompatibility,
                                            LcduiMidletAttributeValues::KTrueValue
                                        );
    }
    DEBUG("> CMIDCustomItem::SetForm");
}

void CMIDCustomItem::ItemAddedToFormL()
{
    CMIDItem::ItemAddedToFormL();

    if (iDirectContent)
    {
        iDirectContent->MdcContainerWindowRectChanged(MdcContainerWindowRect());
    }

    if (iPreviousVisibility)
    {
        ChangeDirectContainerVisibility(ETrue);
    }

    if (iUiToBeFixedLater && iForm)
    {
        iForm->CurrentDisplayable().FixOrientation();
        iUiFixed = ETrue;
        iUiToBeFixedLater = EFalse;
    }
}

void CMIDCustomItem::ItemRemovedFromForm()
{
    if (iUiFixed && iForm)
    {
        iForm->CurrentDisplayable().ReleaseOrientation();
        iUiFixed = EFalse;
        iUiToBeFixedLater = EFalse;
    }

    CMIDItem::ItemRemovedFromForm();

    ChangeDirectContainerVisibility(EFalse);
}

void CMIDCustomItem::PostFocusTransferEvent(
    TBool aFocus,CMIDForm::TDirection aDirection)
{
    DEBUG("< CMIDCustomItem::PostFocusTransferEvent");

    TRect customItemRect = iContentRect;

    if (iForm)
    {
        iForm->CurrentDisplayable().SetS60SelectionKeyCompatibility(
            iS60SelectionKeyCompatibility && aFocus
        );
        TRect formRect = iForm->GetClientArea();
        if (customItemRect.Intersects(formRect))
        {
            customItemRect.Intersection(formRect);
        }
        else
        {
            customItemRect.SetRect(0, 0, 0, 0);
        }

        TInt eventData = aFocus << 16 | aDirection;
        TInt eventData1 = customItemRect.iTl.iX << 16 | customItemRect.iTl.iY;
        TInt eventData2 = customItemRect.Width() << 16 | customItemRect.Height();
        iEnv.PostJavaEvent(*this, EItem, EFocusTraversal,
                           eventData, eventData1, eventData2);
    }
    DEBUG("> CMIDCustomItem::PostFocusTransferEvent");
}

CMIDForm::TDirection CMIDCustomItem::TranslateKey(const TKeyEvent& aEvent)
{
    switch (aEvent.iCode)
    {
    case EKeyLeftArrow:
        return CMIDForm::ELeft;
    case EKeyRightArrow:
        return CMIDForm::ERight;
    case EKeyUpArrow:
        return CMIDForm::EUp;
    case EKeyDownArrow:
        return CMIDForm::EDown;
    }
    return CMIDForm::ENone;
}

MDirectContainer& CMIDCustomItem::DirectContainer()
{
    return *this;
}

void CMIDCustomItem::MdcAddContent(MDirectContent* aContent)
{
    // Item can only have one content.
    iDirectContent = aContent;

    // Register this control to observer
    // in order to provide calls to this control from
    // non-LCDUI ES thread.
    // Unregistration is done in MdcRemoveContent.
    iEnv.ToLcduiObserver().RegisterControl(*this);
}

void CMIDCustomItem::MdcRemoveContent(MDirectContent* /*aContent*/)
{
    iDirectContent = NULL;

    // Unregister the component from the observer
    // if it were previously registered from MdcAddContent.
    iEnv.ToLcduiObserver().UnregisterControl(*this);
}

TRect CMIDCustomItem::MdcContainerWindowRect() const
{
    TRect rect;
    if (iForm)
    {
        // Form has always the same size as the RWindow
        rect = TRect(iForm->PositionRelativeToScreen(), iForm->Size());
    }
    return rect;
}

/** Return the item visibility. If the form has lost focus it is probably
because we have a menu or something else in front so we assume we have lost
visibility.
The direct container uses this to start/stop direct screen access.
*/
TBool CMIDCustomItem::MdcContainerVisibility() const
{
    return (iForm ? iForm->IsFocused() : EFalse) &&
           (MdcContentBounds().Size().iHeight > 0);
}

TRect CMIDCustomItem::MdcContentBounds() const
{
    DEBUG("< CMIDCustomItem::MdcContentBounds");

    TRect visibleRect;

    // Cannot use iContentRect because it's not updated
    // if the CustomItem is scrolled out
    TRect contentRect = Rect();
    contentRect.SetRect(
        contentRect.iTl.iX + iContentMargins.iLeft,
        contentRect.iTl.iY + iContentMargins.iTop,
        contentRect.iBr.iX - iContentMargins.iRight,
        contentRect.iBr.iY - iContentMargins.iBottom);

    DEBUG_INT2("+ CMIDCustomItem::MdcContentBounds - contentRect.iTl = [ %d, %d ]", contentRect.iTl.iX, contentRect.iTl.iY);
    DEBUG_INT2("+ CMIDCustomItem::MdcContentBounds - contentRect.iBr = [ %d, %d ]", contentRect.iBr.iX, contentRect.iBr.iY);
    if (iForm)
    {
        TRect formRect = iForm->GetClientArea();
        if (contentRect.Intersects(formRect))
        {
            visibleRect = contentRect;
            visibleRect.Intersection(formRect);

            visibleRect.Move(iLastWindowPosition);
        }
    }

    DEBUG_INT2("+ CMIDCustomItem::MdcContentBounds - visibleRect.iTl = [ %d, %d ]", visibleRect.iTl.iX, visibleRect.iTl.iY);
    DEBUG_INT2("+ CMIDCustomItem::MdcContentBounds - visibleRect.iBr = [ %d, %d ]", visibleRect.iBr.iX, visibleRect.iBr.iY);
    DEBUG("> CMIDCustomItem::MdcContentBounds");
    return visibleRect;
}

void CMIDCustomItem::MdcItemContentRect(TRect& aContentRect,
                                        TRect& aScreenRect) const
{
    DEBUG("< CMIDCustomItem::MdcItemContentRect");

    aScreenRect = MdcContentBounds(); //The clipped rect

    aContentRect = iContentRect; //The item rect
    aContentRect.Move(iLastWindowPosition);

    DEBUG("> CMIDCustomItem::MdcItemContentRect");
}

void CMIDCustomItem::MdcHandlePointerEventL(TPointerEvent& /*aPointerEvent*/)
{
    // This method should not be called
    __ASSERT_DEBUG(EFalse, User::Invariant());
}

void CMIDCustomItem::MdcFlushContainer(const TRect& aRect)
{
    if (iFrameBuffer && iOffScreenContext)
    {
        iOffScreenContext->BitBlt(TPoint(0,0), iFrameBuffer);

        // Flush the framebuffer on screen
        iEnv.ToLcduiObserver().FlushControl(*this, aRect);
    }
}

void CMIDCustomItem::MdcGetDSAResources(MUiEventConsumer& aConsumer)
{
    if (DrawableWindow())
    {
        // May run in non-LCDUI ES thread
        // Invoke the callback from LCDUI ES thread
        iEnv.ToLcduiObserver().InvokeDSAResourcesCallback(*this, aConsumer);
    }
    else
    {
        // Drawable window is not set
        // Callback will be invoked from within
        // SetContainerWindowL method.
        iConsumerWaitingForDSAResourcesCallback = &aConsumer;
    }
}

void CMIDCustomItem::MdcGetUICallback(
    MUiEventConsumer& aConsumer,
    TInt aCallbackId)
{
    // May run in non-LCDUI ES thread
    // Invoke the callback from LCDUI ES thread
    iEnv.ToLcduiObserver().InvokeUICallback(aConsumer, aCallbackId);
}

// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCustomItem::MdcFixUIOrientation(TBool aEnableFix)
// ---------------------------------------------------------------------------
//

void CMIDCustomItem::MdcFixUIOrientation(TBool aEnableFix)
{
    if (aEnableFix)
    {
        iEnv.ToLcduiObserver().InvokeLcduiEvent(*this, EFixUIOrientation);
    }
    else
    {
        iEnv.ToLcduiObserver().InvokeLcduiEvent(*this, EUnFixUIOrientation);
    }
}

void CMIDCustomItem::HandleLcduiEvent(int aType)
{
    switch (aType)
    {
    case EFixUIOrientation:
        if (!iUiFixed)
        {
            if (iForm)
            {
                iForm->CurrentDisplayable().FixOrientation();
                iUiFixed = ETrue;
                iUiToBeFixedLater = EFalse;
            }
            else
            {
                iUiToBeFixedLater = ETrue;
            }
        }
        break;
    case EUnFixUIOrientation:
        if (iForm && iUiFixed)
        {
            iForm->CurrentDisplayable().ReleaseOrientation();
            iUiFixed = EFalse;
            iUiToBeFixedLater = EFalse;
        }
        break;
    }
}
//
// ---------------------------------------------------------------------------
//
void CMIDCustomItem::MdcAddMMAPIContentBounds(const TRect& /*aRect*/)
{
    iDirectAreaAddedToDisplayable = UpdateDirectContentBounds();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDCustomItem::MdcRemoveMMAPIContentBounds(const TRect& /*aRect*/)
{
    if (iForm && iDirectAreaAddedToDisplayable)
    {
        // Remove clipping rect
        Form()->CurrentDisplayable().RemoveDirectContentArea(iDirectArea);
        iDirectAreaAddedToDisplayable = EFalse;
    }
}

void CMIDCustomItem::ResolutionChange(TInt /*aType*/)
{
    UpdateMargins();
    if (iDirectContent)
    {
        iDirectContent->MdcContainerWindowRectChanged(MdcContainerWindowRect());
    }
}

void CMIDCustomItem::HandleWindowFade(TBool aFaded)
{
    if (iDirectContent)
    {
        TBool contentRectVisible = MdcContainerVisibility();
        if ((contentRectVisible))
        {
            ChangeDirectContainerVisibility(!aFaded);
        }
    }
    iRestoreDirectContentWhenUnfaded = aFaded;
}

void CMIDCustomItem::HandleMediaKeyEvent(TMIDKeyEvent& aKeyEvent)
{
    DEBUG_INT3("CMIDCustomItem::HandleMediaKeyEvent - PostKeyEvent() - JAVA code %d type %d repeats %d", aKeyEvent.iKeyCode, aKeyEvent.iEvents, aKeyEvent.iRepeats);
    iEnv.PostKeyEvent(*this, aKeyEvent);
}

#ifdef RD_TACTILE_FEEDBACK

MMIDTactileFeedbackComponent* CMIDCustomItem::TactileFeedbackComponent()
{
    return this;
}

void CMIDCustomItem::UpdateTactileFeedback()
{
    DEBUG("< CMIDCustomItem::UpdateTactileFeedback");
    TInt areaCount = iTactileFeedback->GetAreasCount();

    for (TInt i=0; i < areaCount; i++)
    {
        CMIDTactileFeedbackExtension::FeedbackArea* area = iTactileFeedback->GetArea(i);
        TRect physicalRect = area->rect;
        physicalRect.Move(iContentRect.iTl.iX,
                          iContentRect.iTl.iY);

        iTactileFeedback->SetFeedbackArea(area->id, physicalRect, (TInt)area->style);
    }
    DEBUG("> CMIDCustomItem::UpdateTactileFeedback");
}

void CMIDCustomItem::RegisterFeedbackArea(TInt aId, TRect aRect, TInt aStyle)
{
    iTactileFeedback->RegisterFeedbackArea(aId, aRect, aStyle);
}

void CMIDCustomItem::UnregisterFeedbackArea(TInt aId)
{
    iTactileFeedback->UnregisterFeedbackArea(aId);
}

void CMIDCustomItem::UnregisterFeedbackForControl()
{
    iTactileFeedback->UnregisterFeedbackForControl();
}

void CMIDCustomItem::MoveAreaToFirstPriority(TInt aId)
{
    iTactileFeedback->MoveAreaToFirstPriority(aId);
}
#endif //RD_TACTILE_FEEDBACK


// ---------------------------------------------------------------------------
// CMIDCustomItem::UpdateDirectContentBounds
// Notifies the current displayable about the direct content position update
// ---------------------------------------------------------------------------
//
TBool CMIDCustomItem::UpdateDirectContentBounds()
{
    if (iForm)
    {
        TRect currentDirectArea(MdcContentBounds());

        currentDirectArea.Move(-iLastWindowPosition);

        if (iDirectAreaAddedToDisplayable)
        {
            Form()->CurrentDisplayable().RemoveDirectContentArea(
                iDirectArea);
        }

        iDirectArea = currentDirectArea;

        // Add clipping rect to protect direct content from being redrawn
        this->Form()->CurrentDisplayable().AddDirectContentArea(
            iDirectArea);

        return ETrue;
    }
    return EFalse;
}
