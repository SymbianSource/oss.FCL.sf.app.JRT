/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the Ticker component
*
*/


#include <eikenv.h>
#include <coecntrl.h>
#include <coeccntx.h>
#include <coecobs.h>
// needed API for iAvkonAppUi
#include <aknappui.h>
#include <AknsUtils.h>
#include <aknconsts.h>

//#include <aknlayoutdef.h>
//#include <aknlayout.lag>
// CAknLayoutFont API for font layouting
#include <AknLayoutFont.h>

#include <aknenv.h>
// CAknIndicatorContainer API for updating text capacity indicator
#include <AknIndicatorContainer.h>
// MAknEditingStateIndicator API for updating text capacity indicator
// and setting state to edit indicators
#include <aknEditStateIndicator.h>
#include <aknnavi.h>
// CAknNavigationDecorator* iNaviDecorator
#include <aknnavide.h>

#include <AknsDrawUtils.h>
#include <j2me/jdebug.h>
#include "CMIDTicker.h"
// needed for setting and removing displayables (iDisplayableRArray)
#include "CMIDDisplayable.h"
// CMIDUtils::MapJavaToETextChars in CMIDScrollingText::SetTextL
#include "CMIDUtils.h"
#include "CMIDUIManager.h"

/**
 * To enable ticker tracing, uncomment the follwing two lines and link
 * to estlib.lib:
 *
 *#include <stdio.h>
 *#define TRACE_TICKER 1
 */

/** Time between each tick.*/
const TInt KTickPeriod = 40000;

/** Scrolling width in pixels where increment increases*/
const TInt KScrollingWidthLimit = 200;

/** Increment values for two different scrolling widths*/
const TInt KOnePixel = 1;
const TInt KTwoPixels = 2;

_LIT(KLeftSquareBracket, "[");
_LIT(KRightSquareBracket, "]");
_LIT(KTextCapacity, "(999) ");

_LIT(KLeftSmalllBracket, "(");
_LIT(KRightSmallBracket, ")");

#ifdef _DEBUG
#define DEBUG_ARG(x) x
#else
#define DEBUG_ARG(x)
#endif

//
// Private helper functions
//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CleanupTicker(TAny* aTicker)
{
    static_cast< CMIDTicker* >(aTicker)->Dispose();
}


// ---------------------------------------------------------------------------
// Return the navi pane container. The method trapped should not leave.
// ---------------------------------------------------------------------------
//
CAknNavigationControlContainer* NaviContainerL()
{
    CEikStatusPane *sp = iAvkonAppUi->StatusPane();
    CAknNavigationControlContainer* naviC =
        static_cast<CAknNavigationControlContainer*>(sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));

    return naviC;
}

// ---------------------------------------------------------------------------
// Return a text layout defined in the platform LAF
// for text to be shown in the navi pane. According to the
// type of navi pane control the text layout may be different.
// We have chosen to use the same layout as the edit indicator
// text (the text showing how many chars are left in an editor).
// To use the layout used by navi pane labels use
// Navi_pane_texts_Line_1. This layout information determines the
// font and the text color. Text positioning is not taken from
// layout, the text is vertically centered and scrolled horizontally
// instead, h aligned to the left or right of the navi pane according
// to the directionality.
// ---------------------------------------------------------------------------
//
const TAknTextLineLayout Layout()
{
    return AknLayout::Small_status_pane_texts_Line_1(0);
}

// ---------------------------------------------------------------------------
// Return the text alignment according to locale alignment
// ---------------------------------------------------------------------------
//
CGraphicsContext::TTextAlign TextAlign()
{
    return AknLayoutUtils::LayoutMirrored() ? CGraphicsContext::ERight : CGraphicsContext::ELeft;
}

//
// class CMIDTickerController
//

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDNaviPaneController* CMIDNaviPaneController::NewL()
{
    CMIDNaviPaneController* self=new(ELeave) CMIDNaviPaneController();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDNaviPaneController::~CMIDNaviPaneController()
{
    delete iTickerTimer;
    iTickerTimer = NULL;
    delete iEditCapacityText;
    iTickerObservers.Close();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDNaviPaneController::CMIDNaviPaneController()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::ConstructL()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::UpdateTextInputIndicatorL(TInt aCount)
{
    delete iEditCapacityText;
    iEditCapacityText = NULL;

    if (aCount >= 0 && aCount <= 999)
    {
        const CFont* font = AknLayoutUtils::FontFromId(Layout().FontId());
        TBuf<10> text;

        text.Append(KLeftSmalllBracket);
        text.AppendNum(aCount);
        text.Append(KRightSmallBracket);

        // set text localized (non Latin numbers)
        AknTextUtils::LanguageSpecificNumberConversion(text);

        iEditCapacityText = TBidiText::NewL(text,1);
        iEditCapacityText->WrapText(font->TextWidthInPixels(KTextCapacity), *font, NULL);

        AddEditCapacityTextToNaviPaneL();
    }
    else
    {
        // to prevent panic if state indicator is NULL (it can happen when 3rd party FEP is active)
        MAknEditingStateIndicator* indicator = CAknEnv::Static()->EditingStateIndicator();
        if (indicator)
        {
            indicator->IndicatorContainer()->SetIndicatorState(
                TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength),
                EAknIndicatorStateOff);
        }
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBidiText* CMIDNaviPaneController::EditCapacityText() const
{
    return iEditCapacityText;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::AddTickerObserverL(
    MMIDTickerObserver* aObserver)
{
    ASSERT(aObserver);
    if (iTickerObservers.Find(aObserver) == KErrNotFound)
    {
        User::LeaveIfError(iTickerObservers.Append(aObserver));
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::RemoveTickerObserver(
    MMIDTickerObserver* aObserver)
{
    ASSERT(aObserver);
    TInt index = iTickerObservers.Find(aObserver);
    if (index != KErrNotFound)
    {
        iTickerObservers.Remove(index);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::SetTickerL(CMIDTicker* aTicker)
{
    if (!iTickerTimer && !aTicker)
    {
        // CMIDTickerTimer instance is created only when
        // a ticker object is set
        return;
    }
    if (!iTickerTimer)
    {
        iTickerTimer = CMIDTickerTimer::NewL(this);
    }
    iTickerTimer->SetTickerL(aTicker);
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::ShowTickerL(TBool aShow)
{
    if (iTickerTimer)
    {
        iTickerTimer->ShowTickerL(aShow);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::PauseTickerL(
    TInt aInterval, MMIDTickerObserver* aPausingObserver)
{
    if (aInterval > 0)
    {
        // Set last observer to pause the ticker
        iPausingObserver = aPausingObserver;
    }
    if (iTickerTimer)
    {
        iTickerTimer->PauseTickerL(aInterval, aPausingObserver);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::TickerChangedL(CMIDTicker* aTicker, TTickerEvent aEvent)
{
    if (iTickerTimer)
    {
        iTickerTimer->TickerChangedL(aTicker, aEvent);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::LayoutTickerL()
{
    if (iTickerTimer)
    {
        iTickerTimer->LayoutTickerL();
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::ResetTickerScrollingSpeedL()
{
    if (iTickerTimer)
    {
        iTickerTimer->ResetScrollingSpeedL();
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::NotifyTickerObservers(TBool aShowing)
{
    TInt numObservers = iTickerObservers.Count();
    for (TInt index = 0; index < numObservers; index++)
    {
        iTickerObservers[ index ]->TickerShowing(aShowing);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDNaviPaneController::AddEditCapacityTextToNaviPaneL()
{
    if (iEditCapacityText != NULL &&
            (!iTickerTimer ||
             !iTickerTimer->iEnabled ||
             !iTickerTimer->iTicker))
    {
        // to prevent panic if state indicator is NULL (it can happen when 3rd party FEP is active)
        MAknEditingStateIndicator* indicator = CAknEnv::Static()->EditingStateIndicator();
        if (indicator)
        {
            indicator->IndicatorContainer()->SetIndicatorState(
                TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength), EAknIndicatorStateOn);
            indicator->IndicatorContainer()->SetIndicatorValueL(
                TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength), iEditCapacityText->Text());
        }
    }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDTickerTimer* CMIDTickerTimer::NewL(CMIDNaviPaneController* aController)
{
    CMIDTickerTimer* self=new(ELeave) CMIDTickerTimer(aController);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDTickerTimer::~CMIDTickerTimer()
{
    DEBUG("CMIDTickerTimer::~CMIDTickerTimer - calling StopL");
    TRAPD(ignore,StopL());
    if (ignore != KErrNone)
    {
        DEBUG_INT("CMIDTickerTimer::~CMIDTickerTimer - Exception from StopL. Error = %d", ignore);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDTickerTimer::CMIDTickerTimer(CMIDNaviPaneController* aCtrl)
        : CTimer(CActive::EPriorityHigh)
        , iCtrl(aCtrl)
        , iTicker(NULL)
        , iEnabled(EFalse)
        , iIncrement(KOnePixel)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::ConstructL()
{
    CTimer::ConstructL();
    ResetScrollingSpeedL();
    CActiveScheduler::Add(this);
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::RunL()
{
    if (iIsPaused)
    {
        iIsPaused = EFalse;
        StartL();
        return;
    }

    if (iTicker)
    {
        iTicker->TickL(iIncrement);
        Kick();
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::TickerChangedL(
    CMIDTicker* aTicker, CMIDNaviPaneController::TTickerEvent aEvent)
{
    ASSERT(aTicker == iTicker);
    (void)aTicker; // Just to suppress warning in release build
    switch (aEvent)
    {
    case CMIDNaviPaneController::ENone:
    case CMIDNaviPaneController::EModelChanged:
        // ignore
        break;
    case CMIDNaviPaneController::ETextChanged:
        break;

    case CMIDNaviPaneController::EPositionChanged:
        iTicker->RenderL();
        break;
    }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::ShowTickerL(TBool aShow)
{
    if (aShow)
    {
        StartL();
    }
    else
    {
        StopL();
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::SetTickerL(CMIDTicker* aTicker)
{
    if (iTicker != aTicker)
    {
        if (iTicker)
        {
            iTicker->StopL();
            iTicker->SetController(NULL);
            iTicker->RemoveRef();
        }

        iTicker = aTicker;

        if (iTicker)
        {
            iTicker->AddRef();
            iTicker->SetController(iCtrl);
        }
    }
    else
    {
        if (iTicker)
        {
            iTicker->PauseL();
        }
    }

    // If the ticker is paused for textfield and textbox and displayable is changed
    // cancel the pause timer and  start the ticker
    if (iIsPaused)
    {
        Cancel();
    }

    CheckedStart();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::PauseTickerL(TInt aInterval, MMIDTickerObserver* aPausingObserver)
{
    if (aInterval <= 0)
    {
        if (iCtrl->iPausingObserver == aPausingObserver)
        { //only last requesting pausing observer can annulate request
            iIsPaused = EFalse;
            StartL();
        }
    }
    else
    {
        PauseL();
        After(aInterval);
        iIsPaused = ETrue;
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::StartL()
{
    Cancel();

    iEnabled = ETrue;
    iCtrl->NotifyTickerObservers(iEnabled && iTicker);

    if (iTicker)
    {
        iTicker->StartL();
    }

    iCtrl->iPausingObserver = NULL;
    iIsPaused = EFalse;

    CheckedStart();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::PauseL()
{
    Cancel();

    iEnabled = EFalse;
    iCtrl->NotifyTickerObservers(iEnabled);

    if (iTicker)
    {
        iTicker->PauseL();
    }

    iCtrl->AddEditCapacityTextToNaviPaneL();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::StopL()
{
    iEnabled = EFalse;
    iCtrl->NotifyTickerObservers(iEnabled);

    if (iTicker)
    {
        iTicker->StopL();
    }

    iCtrl->AddEditCapacityTextToNaviPaneL();

    Cancel();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::CheckedStart()
{
    if (!iEnabled)
    {
        return;
    }
    if (NULL == iTicker)
    {
        return;
    }

    if (!IsActive())
    {
        Kick();
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::Kick()
{
    ASSERT(iEnabled);
    ASSERT(iTicker);
    if (!IsActive())
    {
        After(KTickPeriod);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::LayoutTickerL()
{
    ResetScrollingSpeedL();
    if (iTicker)
    {
        iTicker->LayoutL();
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTickerTimer::ResetScrollingSpeedL()
{
    if (CMIDScrollingText::ScrollingWidthL() <= KScrollingWidthLimit)
    {
        iIncrement = KOnePixel;
    }
    else
    {
        iIncrement = KTwoPixels;
    }
}

//
// class CMIDTicker
//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MMIDTicker* CMIDTicker::NewL(const TDesC& aText)
{
    CMIDTicker* ticker = new(ELeave) CMIDTicker;
    CleanupStack::PushL(TCleanupItem(CleanupTicker, ticker));
    ticker->ConstructL(aText);
    CleanupStack::Pop();
    return ticker;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDTicker::CMIDTicker() :
        iRefCount(1)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDTicker::~CMIDTicker()
{
    DEBUG("CMIDTicker::~CMIDTicker - calling StopL and RemoveDisplayableFromArrayL");
    TRAPD(ignore, StopL(); RemoveDisplayableFromArrayL(NULL));
    if (ignore != KErrNone)
    {
        DEBUG_INT("CMIDTicker::~CMIDTicker - Exception from StopL or RemoveDisplayableFromArrayL. Error = %d", ignore);
    }
    iDisplayableRArray.Reset();
    iDisplayableRArray.Close();

    delete iNaviDecorator;
    iNaviScrollingText = NULL; //deleted by navi decorator
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTicker::ConstructL(const TDesC& aText)
{
    CAknNavigationControlContainer* naviC = NaviContainerL();

    iNaviScrollingText = CMIDScrollingText::NewL(this,aText);
    iNaviScrollingText->SetContainerWindowL(*naviC);
    iNaviScrollingText->SetExtent(TPoint(0,0), naviC->Size());

    iNaviDecorator = CAknNavigationDecorator::NewL(naviC, iNaviScrollingText, CAknNavigationDecorator::ENotSpecified);
    iNaviDecorator->SetContainerWindowL(*naviC);
    iNaviDecorator->MakeScrollButtonVisible(EFalse);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTicker::Dispose()
{
    if (iController)
    {
        TRAPD(ignore, iController->SetTickerL(NULL));
        if (ignore != KErrNone)
        {
            DEBUG_INT("CMIDTicker::Dispose - Exception from CMIDNaviPaneController::SetTickerL(NULL). Error = %d", ignore);
        }
    }
    delete this;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTicker::SetTextL(const TDesC& aText)
{
    iNaviScrollingText->SetTextL(aText);
}

void CMIDTicker::AddRef()
{
    ++iRefCount;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTicker::RemoveRef()
{
    ASSERT(iRefCount);
    --iRefCount;
    if (0 == iRefCount)
    {
        delete this;
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDTicker::SetDisplayableL(CMIDDisplayable* aDisplayable, TInt aAdd)
{
    ASSERT(aDisplayable);

    if (aAdd)
    { //KAddDisplayable:
        iDisplayableRArray.AppendL(aDisplayable);
    }
    else
    {
        RemoveDisplayableFromArrayL(aDisplayable);
    }
}

// ---------------------------------------------------------------------------
// if aDisplayable is not null then remove it
// if aDisplayable is NULL mean remove all
// ---------------------------------------------------------------------------
//
void CMIDTicker::RemoveDisplayableFromArrayL(CMIDDisplayable* aDisplayable)
{
    // if aDisplayable is not null then remove it.
    if (aDisplayable)
    {
        TInt index = iDisplayableRArray.Find(aDisplayable);
        if (index != KErrNotFound)
        {
            CMIDDisplayable* displayable = iDisplayableRArray[index];
            iDisplayableRArray.Remove(index);
            displayable->SetTickerL(NULL);
        }
    }
    else
    {// if aDisplayable is null mean remove all
        TInt count = iDisplayableRArray.Count();
        for (TInt ii = 0; ii<count; ii++)
        {
            // Always remove the Zeroth element as we are removing
            // the element array size is changing (reducing).
            CMIDDisplayable* displayable = iDisplayableRArray[0]; // index zero is required
            iDisplayableRArray.Remove(0);// index zero is required
            displayable->SetTickerL(NULL);
        }
    }
}

// ---------------------------------------------------------------------------
// Start the ticker: we must disable any edit indicators (this is done by observers
// but there is an observer, CDateFieldItem, which uses an editor (CEikMfne) which is not
// FEP aware and hence cannot disable the edit indicators at the right time. So we have to
// do it in here instead. The push the navi decorator on the navi pane. This will assure
// our scrolling text will be drawn. */
// ---------------------------------------------------------------------------
//
void CMIDTicker::StartL()
{
    if (iNaviScrollingText->Size() == TSize(0,0))
    {
        iNaviScrollingText->SetExtent(TPoint(0,0), NaviContainerL()->Size());
        iNaviScrollingText->UpdateTickerLayoutL();
    }

    // to prevent panic if state indicator is NULL (it can happen when 3rd party FEP is active)
    MAknEditingStateIndicator* indicator = CAknEnv::Static()->EditingStateIndicator();
    if (indicator)
    {
        indicator->SetState(EStateNone);
    }
    NaviContainerL()->PushL(*iNaviDecorator);
    iNaviScrollingText->SetPaused(EFalse);
}

// ---------------------------------------------------------------------------
// Pause the ticker, inform ScrollingText about pause mode, and remove our
// navi decorator from the navi pane. */
// ---------------------------------------------------------------------------
//
void CMIDTicker::PauseL()
{
    iNaviScrollingText->SetPaused(ETrue);
    NaviContainerL()->Pop(iNaviDecorator);
}


// ---------------------------------------------------------------------------
// Stop the ticker: just remove our navi decorator from the navi pane. */
// ---------------------------------------------------------------------------
//
void CMIDTicker::StopL()
{
    NaviContainerL()->Pop(iNaviDecorator);
    iNaviScrollingText->SetPaused(EFalse);
}

// ---------------------------------------------------------------------------
// The text position is updated every tick, this achieves text scrolling. */
// ---------------------------------------------------------------------------
//
void CMIDTicker::TickL(TInt aIncrement)
{
    iNaviScrollingText->UpdateTextPositionL(aIncrement);
}

void CMIDTicker::SetController(CMIDNaviPaneController* aController)
{
    iController = aController;
}

CMIDNaviPaneController* CMIDTicker::Controller() const
{
    return iController;
}

void CMIDTicker::ReportEventL(CMIDNaviPaneController::TTickerEvent aEvent)
{
    if (iController)
    {
        iController->TickerChangedL(this, aEvent);
    }
}

// ---------------------------------------------------------------------------
// Relayout ScrollingText, cancel pause mode.
// ---------------------------------------------------------------------------
//
void CMIDTicker::LayoutL()
{
    iNaviScrollingText->SetPaused(EFalse);
    iNaviScrollingText->UpdateTickerLayoutL();
}

// ---------------------------------------------------------------------------
// Redraw the status pane (out scrolling text is transparent so we need
// the status pane to draw the navi pane background for us)
// ---------------------------------------------------------------------------
//
void CMIDTicker::RenderL()
{
    iAvkonAppUi->StatusPane()->DrawNow();
}

//
// CMIDScrollingText
//

CMIDScrollingText* CMIDScrollingText::NewL(CMIDTicker* aParent, const TDesC& aText)
{
    CMIDScrollingText* self = new(ELeave) CMIDScrollingText(aParent);
    CleanupStack::PushL(self);
    self->ConstructL(aText);
    CleanupStack::Pop(self);
    return self;
}

CMIDScrollingText::~CMIDScrollingText()
{
    delete iPictographInterface;
    delete iText;
}

void CMIDScrollingText::UpdateTickerLayoutL()
{
    const CFont* font = AknLayoutUtils::FontFromId(Layout().FontId());
    const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull(font);

    if (layoutFont)
    {
        iBaseLineOffset = (iSize.iHeight - layoutFont->TextPaneHeight())/2 + layoutFont->TextPaneTopToBaseline();
    }
    else
    {
        iBaseLineOffset = (iSize.iHeight - font->HeightInPixels())/2 + font->AscentInPixels();
    }

    iTextWidth = font->TextWidthInPixels(iText->Text());

    if (!iPaused)
    {
        InitialiseScrollingTextPosition();
    }
}

void CMIDScrollingText::SetPaused(TBool aPause)
{
    iPaused = aPause;
}

void CMIDScrollingText::SetTextL(const TDesC& aText)
{
    HBufC* buf = aText.AllocL();
    CMIDUtils::MapJavaToETextChars(buf,EFalse);
    delete iText;
    iText = NULL;
    iText = TBidiText::NewL(buf->Des(),1);

    UpdateTickerLayoutL();
}

// ---------------------------------------------------------------------------
// Static method returning an estimate of the scrolling width -
// this must be the same for all tickers
// ---------------------------------------------------------------------------
//
TInt CMIDScrollingText::ScrollingWidthL()
{
    const CFont* font = AknLayoutUtils::FontFromId(Layout().FontId());

    TInt w = NaviContainerL()->Size().iWidth;
    w -= (font->TextWidthInPixels(KLeftSquareBracket) + font->TextWidthInPixels(KRightSquareBracket));

    return w;
}

CMIDScrollingText::CMIDScrollingText(CMIDTicker* aParent) : iParent(aParent)
{
}

void CMIDScrollingText::ConstructL(const TDesC& aText)
{
    SetTextL(aText);

    iPictographInterface = CAknPictographInterface::NewL(*this,*this);
}

// ---------------------------------------------------------------------------
// Draw the text. Either "[ ... ]" or "(999) [ .. ]" where ... or .. is the
// scrolling text. Or if device is Right to Left: either "[ ... ]" or "[..] (999)" */
// ---------------------------------------------------------------------------
//
void CMIDScrollingText::Draw(const TRect& aRect) const
{
    TAknTextLineLayout layout = Layout();

    CMIDNaviPaneController* controller = iParent->Controller();
    const CFont* font = AknLayoutUtils::FontFromId(layout.FontId());
    CWindowGc& gc = SystemGc();

    gc.SetPenColor(TextColor());
    gc.UseFont(font);


    //draw edit capacity text, eg "(999)" if needed
    if (controller && controller->EditCapacityText() != NULL)
    {
        gc.DrawText(controller->EditCapacityText()->Text(), aRect, iBaseLineOffset, TextAlign(), 0);
    }

    //draw "["
    gc.DrawText(KLeftSquareBracket, TPoint(LeftTextPosition(font)
                                           - font->TextWidthInPixels(KLeftSquareBracket),iBaseLineOffset));

    //draw "]"
    gc.DrawText(KRightSquareBracket,TPoint(RightTextPosition(font),iBaseLineOffset));


    //clip area for scrolling text
    gc.SetClippingRect(TRect(TPoint(LeftTextPosition(font), 0), TPoint(RightTextPosition(font), iSize.iHeight)));

    //draw scrolling text
    iText->WrapText(iTextWidth, *font,0);
    if (iPictographInterface)
    {
        MAknPictographDrawer* aknPictographDrawer = iPictographInterface->Interface();
        aknPictographDrawer->DrawText(gc, *font, iText->DisplayText(),
                                      TPoint(iTextPosition,iBaseLineOffset));
    }
    else
    {
        iText->DrawText(gc, TPoint(iTextPosition,iBaseLineOffset));
    }

    gc.CancelClippingRect();
}

void CMIDScrollingText::HandleResourceChange(TInt aType)
{
    CCoeControl::HandleResourceChange(aType);
    if (aType==KEikDynamicLayoutVariantSwitch || aType == KEikColorResourceChange)
    {
        if (aType == KEikDynamicLayoutVariantSwitch && iParent && iParent->Controller())
        {//change scrolling velocity if needed
            DEBUG("CMIDScrollingText::HandleResourceChange - calling iParent->Controller()->ResetScrollingSpeedL");

            TRAPD(ignore, iParent->Controller()->ResetTickerScrollingSpeedL());
            if (ignore != KErrNone)
            {
                DEBUG_INT("CMIDScrollingText::HandleResourceChange - Exception from iParent->Controller()->ResetScrollingSpeedL. Error = %d", ignore);
            }
        }

        //reset position and layout
        iPaused = false;
        DEBUG("CMIDScrollingText::HandleResourceChange - calling UpdateTickerLayoutL");
        TRAPD(ignore,UpdateTickerLayoutL());
        if (ignore != KErrNone)
        {
            DEBUG_INT("CMIDScrollingText::HandleResourceChange - Exception from UpdateTickerLayoutL. Error = %d", ignore);
        }

        //and finally draw
        DrawNow();
    }
}

void CMIDScrollingText::SizeChanged()
{
    // call parent implementation
    CCoeControl::SizeChanged();

    // update layout
    TRAP_IGNORE(UpdateTickerLayoutL());
}

void CMIDScrollingText::DrawPictographArea()
{
    // Does nothing - Ticker is updating himself
}

// ---------------------------------------------------------------------------
// Scroll the text of aScrollAmount, the direction depends on the text directionality. The min
// and max text position are given by LeftTextPosition() and RightTextPosition().
// ---------------------------------------------------------------------------
//
void CMIDScrollingText::UpdateTextPositionL(TInt aScrollAmount)
{
    const CFont* font = AknLayoutUtils::FontFromId(Layout().FontId());

    if (iPaused)
    {
        return;
    }

    if (iText->Directionality() == TBidiText::ELeftToRight)
    {
        iTextPosition -= aScrollAmount;
        if (iTextPosition < LeftTextPosition(font) - iTextWidth)
        {
            iTextPosition = RightTextPosition(font);
        }
    }
    else
    {
        iTextPosition += aScrollAmount;
        if (iTextPosition > RightTextPosition(font))
        {
            iTextPosition = LeftTextPosition(font) - iTextWidth;
        }
    }

    iParent->ReportEventL(CMIDNaviPaneController::EPositionChanged);
}


// ---------------------------------------------------------------------------
// The text color, this is taken from the layout + any skinning should be taken into account.
// ---------------------------------------------------------------------------
//
inline TRgb CMIDScrollingText::TextColor() const
{
    TRgb color = AKN_LAF_COLOR(Layout().iC);

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    // Ignore error, color is not updated if it not found from the skin
    AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG2);

    return color;
}

// ---------------------------------------------------------------------------
// The space needed to draw "(999) " in pixels.
// ---------------------------------------------------------------------------
//
inline TInt CMIDScrollingText::EditCapacityWidth(const CFont* aFont) const
{
    CMIDNaviPaneController* controller = iParent->Controller();

    if (controller && controller->EditCapacityText() != NULL)
    {
        return aFont->TextWidthInPixels(KTextCapacity);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// The left position for scrolling text. According to the device directionality
// we must add or not the edit capacity width.
// ---------------------------------------------------------------------------
//
inline TInt CMIDScrollingText::LeftTextPosition(const CFont* aFont) const
{
    return aFont->TextWidthInPixels(KLeftSquareBracket) +
           (TextAlign() == CGraphicsContext::ELeft ? EditCapacityWidth(aFont) : 0);
}

// ---------------------------------------------------------------------------
// The right position for scrolling text. According to the device directionality
// must add or not the edit capacity width.
// ---------------------------------------------------------------------------
//
inline TInt CMIDScrollingText::RightTextPosition(const CFont* aFont) const
{
    return iSize.iWidth - aFont->TextWidthInPixels(KRightSquareBracket) -
           (TextAlign() == CGraphicsContext::ELeft ? 0 : EditCapacityWidth(aFont));
}

// ---------------------------------------------------------------------------
// Sets the initial scrolling text position
// ---------------------------------------------------------------------------
//
void CMIDScrollingText::InitialiseScrollingTextPosition()
{
    const CFont* font = AknLayoutUtils::FontFromId(Layout().FontId());
    if (iText->Directionality() == TBidiText::ELeftToRight)
    {
        iTextPosition = RightTextPosition(font);
    }
    else
    {
        iTextPosition = LeftTextPosition(font) - iTextWidth;
    }
}

// End of File





















