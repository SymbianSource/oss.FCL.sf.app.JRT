/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The Ticker MIDP Java UI component
*
*/


#ifndef CMIDTICKER_H
#define CMIDTICKER_H

/**
 * This file contains the Series 60 implementation of the native
 * peer to the javax.microedition.lcdui.Ticker class.
 *
 * This implementation uses a single timer to drive the currently
 * active (visible) ticker. If no ticker is present on the current
 * screen the timer is not started.
 *
 * Unlike in the reference implementation, which has separate model
 * and view, whic is a CCoeControl, in this implementation CMIDTicker
 * draws the text into the Navi Pane. However, most of the framework
 * of the reference implementation, includin MTickerObserver interface,
 * has been preserved. In some cases this may look a bit overkill but
 * the aim was to minimize differences to the reference implementation.
 */

// Includes
#include <coecntrl.h>
#include <biditext.h>
// MMIDTicker interface
#include <lcdui.h>

// utils to layout ticker and text
#include <AknUtils.h>

// CAknPictographInterface* iPictographInterface
#include <AknPictographInterface.h>
// MAknPictographAnimatorCallBack interface
#include <AknPictographDrawerInterface.h>

/** The default ticker pause interval for displayables that need to
temporarly disable the ticker in order to show their indicators, eg.
edit indicators like in the case of CMIDEdwin and CMIDDateFieldItem */
#define TICKER_PAUSE_INTERVAL 5000000 // time in micro seconds

class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknIndicatorContainer;

// Forward declarations
class CMIDTicker;
class CMIDTickerControl;
class CMIDDisplayable;

class CMIDScrollingText;
class CMIDNaviPaneController;
class CMIDUIManager;
class CMIDTickerTimer;

/** Editors need to observe the ticker, to make sure
they do not try to display edit indicators in the navi
pane when the ticker is showing. */
class MMIDTickerObserver
{
public:
    /**
     * @since s60
     */
    virtual void TickerShowing(TBool /*aShowing*/) { }
};

/**
 * Controller for navi pane and in there expecially for ticker handling
 */
NONSHARABLE_CLASS(CMIDNaviPaneController) : public CBase
{
public:
    friend class CMIDTickerTimer;

    enum TTickerEvent
    {
        ENone,
        EModelChanged,
        ETextChanged,
        EPositionChanged
    };

    /**
     * Double ctor
     * @return CMIDTickerController instance
     * @since s60
     */
    static CMIDNaviPaneController* NewL();

    /**
     * Dtor
     * @since s60
     */
    ~CMIDNaviPaneController();

    /**
     * Update the text capacity indicator in the default edit indicator. Do this
     * only if no ticker is currently showing, see AddEditCapacityTextToNaviPaneL().
     * The text capacity indicator is stored in iEditCapacityText so that if there is
     * an active ticker, it will reduce the size of its scrolling text and display
     * the capacity indicator.
     *
     * @param aCount
     * @since s60
     */
    void UpdateTextInputIndicatorL(TInt aCount);

    TBidiText* EditCapacityText() const;

    /**
     * Add observer
     * @param aObserver Observer
     * @since s60
     */
    void AddTickerObserverL(MMIDTickerObserver* aObserver);

    /**
     * Remove observer
     * @param aObserver Observer
     * @since s60
     */
    void RemoveTickerObserver(MMIDTickerObserver* aObserver);

    /**
     * @see CMIDTickerTimer::SetTickerL
     */
    void SetTickerL(CMIDTicker* aTicker);

    /**
     * @see CMIDTickerTimer::ShowTickerL
     */
    void ShowTickerL(TBool aShow);

    /**
     * @see CMIDTickerTimer::PauseTickerL
     */
    void PauseTickerL(TInt aInterval, MMIDTickerObserver* aPausingObserver);

    /**
     * @see CMIDTickerTimer::TickerChangedL
     */
    void TickerChangedL(CMIDTicker* aTicker, TTickerEvent aEvent);

    /**
     * @see CMIDTickerTimer::LayoutTickerL
     */
    void LayoutTickerL();

    /**
     * @see CMIDTickerTimer::ResetScrollingSpeedL
     */
    void ResetTickerScrollingSpeedL();

private:

    /**
     * Ctro
     */
    CMIDNaviPaneController();

    /**
     *
     */
    void ConstructL();

    /**
     * Notify ticker observers. Called by CMIDTickerTimer instance.
     * @param aShowing Boolean info concerning visibility
     * @since s60
     */
    void NotifyTickerObservers(TBool aShowing);

    /**
     * Add the capacity text to the default avkon edit indicator.
     * @since s60
     */
    void AddEditCapacityTextToNaviPaneL();

private:
    RArray< MMIDTickerObserver* > iTickerObservers;
    /** The last observer that requested a pause in the ticker */
    MMIDTickerObserver* iPausingObserver;
    CMIDTickerTimer* iTickerTimer;
    TBidiText* iEditCapacityText;
};

/**
 * Timer driving ticker animation - provides
 * primitive (first order?) attempt to lock the phase.
 */
NONSHARABLE_CLASS(CMIDTickerTimer) : public CTimer
{
public:
    friend class CMIDNaviPaneController;

    /**
     * Double ctor
     * @param aCtrl Parent controller
     */
    static CMIDTickerTimer* NewL(CMIDNaviPaneController* aCtrl);

    /**
     * Dtor
     */
    ~CMIDTickerTimer();


    /**
     * Set the current ticker. aTicker may be NULL.
     * If set ticker is the same as actual one, ticker is paused.
     * If aTicker is not NULL and the timer is in
     * the started state, this will start the timer
     * running immediately.
     * @param aTicker Ticker
     * @since s60
     */
    void SetTickerL(CMIDTicker* aTicker);

    /**
     * Show of hide ticker
     * @param aShow
     * @since s60
     */
    void ShowTickerL(TBool aShow);

    /**
     * Pause ticker for aInterval micro seconds
     * @param aInterval Interval
     * @param aPausingObserver Observer
     * @see CMIDTickerTimer::PauseTickerL
     * @since s60
     */
    void PauseTickerL(TInt aInterval, MMIDTickerObserver* aPausingObserver);

    /**
     * State of Ticker is changed
     * @param aTicker Ticker
     * @param aEvent Event
     * @since s60
     */
    void TickerChangedL(CMIDTicker* aTicker, CMIDNaviPaneController::TTickerEvent aEvent);

    /**
     * Relayout Ticker (ScrollingText) in case resolution change.
     * RestScrollingParameters is called because velocity depends on screen size.
     * @since s60
     */
    void LayoutTickerL();

private:

    /**
     * Ctor
     * @param aCtrl Parent controller
     */
    CMIDTickerTimer(CMIDNaviPaneController* aCtrl);

    /**
     *
     */
    void ConstructL();

    /**
     *
     * @since s60
     */
    void RunL();

    /**
     * Start the ticker. First of all Cancel() any previous timer operation. Then notify
     * the observers that the ticker is starting so that they may disable any navi pane indicators
     * such as edit indicators.
     * Then start the ticker, see CMIDTicker::StartL() and start the timer for updating the ticker,
     * see CheckedStart()
     * @since s60
     */
    void StartL();

    /**
     * Pause the current ticker. This does not reset the text position of the ticker. So
     * when the ticker is started again the text scrolling will resume from where it left. Only
     * setting a different ticker will resume text scrolling. Make sure to notify the observers
     * so that they may re-establish any navi pane indicators, eg edit indicators. Also add any
     * existing capacity text indicator to the default edit indicator, see
     * AddEditCapacityTextToNaviPaneL().
     * @since s60
     */
    void PauseL();

    /**
     * Stop the current ticker. This does not reset the text position of the ticker, but call to
     * CMIDTicker::StartL() does. Make sure to notify the observers so that they may re-establish
     * any navi pane indicators, eg edit indicators. Also add any existing capacity text indicator
     * to the default edit indicator, see AddEditCapacityTextToNaviPaneL().
     * @since s60
     */
    void StopL();

    /**
     * Start the timer for updating the ticker
     * @since s60
     */
    void CheckedStart();

    /**
     *
     * @since s60
     */
    void Kick();

    /**
     * Adjust the scrolling velocity. This methos is called once at start-up and when there is
     * a resolution change because the velocity depends on the screen resolution
     * @since s60
     */
    void ResetScrollingSpeedL();

private:
    CMIDNaviPaneController* iCtrl;
    CMIDTicker* iTicker;
    TBool iEnabled;
    TBool iIsPaused;
    TInt iIncrement;
};

/**
 * Ticker model implements MMIDTicker interface.
 */
NONSHARABLE_CLASS(CMIDTicker) : public CBase, public MMIDTicker
{
public:
    /**
     * Construct and return a new ticker component to
     * display string aText.
     */
    static MMIDTicker* NewL(const TDesC& aText);

    /**
     *@see MMIDTicker
     */
    void SetTextL(const TDesC& aText);

    /**
     *@see MMIDComponent
     */
    void Dispose();

    /**
     * Increment reference count.
     *
     * Note : ticker instances may be shared between displayables.
     */
    void AddRef();

    /**
     * Decrement reference count - may delete instance.
     *
     * Note: ticker instacnes may be shared between displayables.
     */
    void RemoveRef();

    /**
     * Increment the tick count of this ticker. Intended
     * to be called on a timer.
     */
    void TickL(TInt aCount);

    void SetController(CMIDNaviPaneController* aController);
    CMIDNaviPaneController* Controller() const;

public:
    void StartL();
    void PauseL();
    void StopL();
    void RenderL();

    void SetDisplayableL(CMIDDisplayable* aDisplayable, TInt aAdd);
    void ReportEventL(CMIDNaviPaneController::TTickerEvent aEvent);
    void LayoutL();
private:
    CMIDTicker();
    void ConstructL(const TDesC& aText);
    ~CMIDTicker();

private:
    void RemoveDisplayableFromArrayL(CMIDDisplayable* aDisplayable);
    void CreateNaviControlsL();

private:
    TInt                    iRefCount;

    CMIDNaviPaneController*     iController;        // Not owned

    // require to tell the displayable that ticker is deleted
    RArray<CMIDDisplayable*> iDisplayableRArray;

    CAknNavigationDecorator* iNaviDecorator;
    CMIDScrollingText*       iNaviScrollingText;
};

/** A coe control class for drawing scrolling text in the navi pane */
NONSHARABLE_CLASS(CMIDScrollingText) : public CCoeControl, public MAknPictographAnimatorCallBack
{
public:
    friend class CMIDNaviPaneController;

    static CMIDScrollingText* NewL(CMIDTicker* aParent, const TDesC& aText);
    ~CMIDScrollingText();

    void UpdateTickerLayoutL();
    void UpdateTextPositionL(TInt aScrollAmount);
    void SetTextL(const TDesC& aText);
    void SetPaused(TBool aPause);
    static TInt ScrollingWidthL();

private:
    CMIDScrollingText(CMIDTicker* aParent);
    void ConstructL(const TDesC& aText);

private: //CCoeControl
    void Draw(const TRect& aRect) const;
    void HandleResourceChange(TInt aType);
    void SizeChanged();

private: //MAknPictographAnimatorCallBack
    void DrawPictographArea();

private: //implementation
    inline TRgb TextColor() const;

    inline TInt EditCapacityWidth(const CFont* aFont) const;
    inline TInt LeftTextPosition(const CFont* aFont) const;
    inline TInt RightTextPosition(const CFont* aFont) const;

    void InitialiseScrollingTextPosition();

private:
    TInt                     iTextPosition;
    TBidiText*               iText;
//  TSize                    iSize; // not constant
    TInt                     iBaseLineOffset;
    TInt                     iTextWidth;
    TBool                    iPaused;
    CAknPictographInterface* iPictographInterface;
    CMIDTicker*              iParent;       // Not owned
};

#endif // CMIDTICKER_H

// End of File
