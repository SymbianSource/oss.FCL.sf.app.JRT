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
* Description:  Implements the Form LCDUI component.
*
*/


#ifndef CMIDFORM_H
#define CMIDFORM_H

// CCoeControl inheritance in class declaration
#include <coecntrl.h>
// MEikScrollBarObserver interface in class declaration
#include <eiksbobs.h> //MEikScrollBarObserver
#include <lcdui.h>
// using Timer to do defer laying out (iLayoutTimer member)
#include <TimeOutTimer.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif


// Layout of Rect of midp_form_pane
#include <AknUtils.h>

class CEikScrollBarFrame;
class CEikLabel;

class CMIDItem;
class CMIDItemLabel;
class CMIDControlItem;
class CMIDDisplayable;
class CMIDSpacer;
class CMIDStringItem;
class CMIDCommandList;
class CMIDFormRow;
class CAknsListBoxBackgroundControlContext;
class CAknsFrameBackgroundControlContext;
class CMIDCommand;
class CMIDPopupNoteController;
class CMIDFormPhysics;


/**
 * A form class as defined in MMIDForm. Manages items (MMIDItems) using rows, CMIDFormRow.
 *
 * @see CMIDFormRow, CMIDItem, CMIDControlItem and item derived classes.
 */
#ifdef RD_SCALABLE_UI_V2
NONSHARABLE_CLASS(CMIDForm) : public CCoeControl, public MCoeControlObserver,
        public MMIDForm, public MTimeOutNotify, public MEikScrollBarObserver
#else
NONSHARABLE_CLASS(CMIDForm) : public CCoeControl, public MCoeControlObserver,
        public MMIDForm, public MTimeOutNotify
#endif //RD_SCALABLE_UI_V2
{
public:
    enum TDirection
    {
        EDown=6,
        EUp=1,
        ELeft=2,
        ERight=5,
        ENone=0
    };

    enum TCommandType
    {
        EInsert,
        EDelete,
        EDeleteAll,
        ESet,
        ERefresh
    };

    struct TFormCommand
    {
        TCommandType iCommand;
        TInt iIndex;
        MMIDItem* iItem;
    };

public:

    static CMIDForm* NewL(MMIDEnv& aEnv,MMIDDisplayable& aDisplayable);

    ~CMIDForm();

public:
    void RequestLayoutL(TBool aImmediately = EFalse);

    void HandleCurrentL(TBool aCurrent);
    TBool TryDetectLongTapL(const TPointerEvent &aPointerEvent);

    void SetFocusedItemIfNone(CMIDItem* aItem);
    void RemoveFocusIfFocused(CMIDItem* aItem);

    void Traverse(TDirection aDirection);
    void ScrollRelative(TInt aScroll);

    /** Scrolls to the location specified by the parameter. Does not finalize the scroll. */
    void RawScroll(TInt aScroll);

    TRect GetClientArea();

    void UpdateItemCommands(CMIDCommandList* aCommandList, CMIDCommand* aMSKCommand);

    void RefreshItemL(TInt aIndex);
    void RefreshItemL(MMIDItem* aItem);
    void RemoveDeletedItem(CMIDControlItem& aItem);

    inline MMIDItem::TLayout InitialAlignment();
    TBool RectPartiallyVisible(const TRect& aRect);
    TBool RectFullyVisible(const TRect& aRect);

    //Touch event handling from Items
    void HandleTouchControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    static TInt FormWidth();

    // FormRect method has 2 implementations: non-static and static
    static TRect StaticFormRect();
    inline TRect FormRect();

    inline TInt Width();
    inline TInt Height();

    inline CMIDDisplayable& CurrentDisplayable();
    MMIDItem* CurrentItem() const;

    // Width of navi bar position only for ( iAlignment == MMIDItem::ERight )
    TInt GetMidpNaviPos();

    /**
     * Checks if the current item on the form is the one passed as a parameter.
     * This method is needed because CCoeControl::IsFocused() cannot be used
     * in case a menu is open.
     *
     * @param aItem     Pointer to a form item.
     * @return          ETrue, if aItem is the curren item on the form.
     */
    TBool IsCurrentItem(CMIDItem* aItem) const;

    /**
    * Handles event when application switches to/from the foreground.
    *
    * @param aForeground ETrue to switch to the foreground. EFalse to switch
    *   from the foreground.
    * @note Event is sent to gauge items.
    */
    void HandleForegroundL(TBool aForeground);

    /**
     * @return pointer to CMIDPopupNoteController instance
     */
    CMIDPopupNoteController* GetPopupNoteController() const;

    CAknsFrameBackgroundControlContext* GetHighlightBackgroundCc() const;

#ifdef RD_SCALABLE_UI_V2
    /**
     * Checks if the StringItem which is split across several label container items contains a point
     * @param aStringItem a string item to look for
     * @param sPoint a point to check
     */
    TBool StringItemContainsPoint(CMIDStringItem* aStringItem, const TPoint& aPoint) const;

    /**
     * Checks if focus has changed between TPointerEvent::EButton1Down and TPointerEvent::EButton1Up events
     * @return ETrue if form has changed focus for a component with a pen event
     */
    TBool IsFocusChangingWithPen() const;

    /**
     * Returns true if the form was scrolled when pointer was pressed down, i.e.
     * pointer down occured on item that was partially visible on screen.
     *
     * @return  ETrue, if the form was scrolled when pointer pressed down.
     */
    TBool IsScrolledOnPointerDown() const;

public: // from MEikScrollBarObserver
    void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);
#endif //RD_SCALABLE_UI_V2

#ifdef RD_TACTILE_FEEDBACK
private:
    MTouchFeedback* iFeedback;
#endif

private:
    CMIDForm(MMIDEnv& aEnv, CMIDDisplayable& aDisplayable);
    void ConstructL();
    void UpdateMemberVariables();
private: // MMIDForm
    void SetAllItemsL(const RArray<MMIDItem*>& aItems);
    void SetItemL(MMIDItem& aItem,TInt aIndex);
    void InsertItemL(MMIDItem& aItem,TInt aIndex);
    void DeleteItemL(TInt aIndex);
    void DeleteAllItemsL();

    TBool IsItemVisible(TInt aIndex);
    void SetCurrentItemL(TInt aIndex);

private: // MMIDComponent
    inline void Dispose();

private: // MCoeControlObserver
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

private: // MTimeOutNotify
    void TimerExpired();

private: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void FocusChanged(TDrawNow aDrawNow);
    void HandleResourceChange(TInt aType);
    void SizeChanged();
    TCoeInputCapabilities InputCapabilities() const;
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif

private: // implementation items

#ifdef RD_SCALABLE_UI_V2
    /**
     * Handles the pointer drag events in form.
     *
     * @param aPointerEvent     The pointer event
     */
    void HandleDragEventL(const TPointerEvent& aPointerEvent, TBool aForItem);

    /**
     * Does the actual scrolling in timed scrolling cases (=EButtonRepeatEvent).
     * Calculates the number of pixels to scroll, scrolls the form and moves the focus to another
     * item if needed.
     *
     * @param aDirection   The direction of the scroll, EUp or EDown
     */
    void TimedScroll(TDirection aDirection);

    /**
     * Checks if the form can be scrolled downwards.
     *
     * @return ETrue if the form can be scrolled downwards, EFalse otherwise
     */
    TBool CanScrollDown() const;

    /**
     * Checks if the form can be scrolled upwards.
     *
     * @return  ETrue if the form can be scrolled upwards, EFalse otherwise
     */
    TBool CanScrollUp() const;

    /**
     * Checks if the currently focused item contains the point given as a
     * parameter.
     *
     * @param aPoint   The point
     * @return  ETrue if there is a focused item on the form currently and the focused
     *          item contains aPoint, EFalse otherwise.
     */
    TBool FocusedItemContainsPoint(const TPoint& aPoint) const;

    /**
     * Returns a pointer to the control item that contains the point
     * given as a parameter. Returns NULL if there is no control item
     * at the given point.
     *
     * @param aPoint    The point
     * @return  A pointer to the control item or NULL
     */
    CMIDControlItem* ControlItemAtPoint(const TPoint& aPoint) const;

    /**
     * Returns the index of the row that contains the point given as a parameter.
     * KErrNotFound is returned if none of the rows contains the point.
     *
     * @param aPoint    The point
     * @return  Index of the row or KErrNotFound if no row is found
     */
    TInt RowAtPoint(const TPoint& aPoint) const;

    /**
     * Requests EButtonRepeat event from RWindow. Checks that aPointerEvent.iPosition
     * is out of the form area before the request is made.
     *
     * @param aPointerEvent     The pointer event after which the repeat event is requested.
     */
    void RequestPointerRepeat(const TPointerEvent& aPointerEvent);
#endif

    TBool TryMovingFocusHorizontally(TDirection aDirection);
    TBool TryMovingFocusVertically(TDirection aDirection,
                                   TBool aSelectClosest, TBool aScroll = ETrue);
    CMIDControlItem* SelectClosestFocussableItem(TInt aRowIndex) const;
    TInt IndexOfFirstFocusableItemOnScreen(TDirection aDirection);
    /** Scrolls to the focused item and finalizes the scroll. */
    void ScrollToFocused(TDirection aDirection = EDown, TBool aReset = ETrue);
    /**
     * Returns how much need to be scrolled to get tapped item fully visible.
     *
     * @return      amount of pixels to be scrolled.
     * @return      0, If Item is already fully visible or if either ends of the
     *              item are not fully visible (middle part of the item is shown).
     */
    TInt ScrollDistanceToTappedItem(CMIDControlItem* aPointedControl);
    /** Scrolls to the specified rect. Does not finalize the scroll. */
    void RawScrollToRect(const TRect& aRect, TDirection aDirection, TBool aReset = EFalse);
    /** Scrolls to the specified form row. Does not finalize the scroll. */
    void RawScrollToRow(TInt aIdx, TDirection aDirection);
    /** Scrolls the amount specified by the parameter. Does not finalize the scroll. */
    void RawScrollRelative(TInt aScroll);
    /** Finalizes any unfinalized raw scroll. Finalizing includes updating scrollbars and requesting drawing. */
    void RawScrollFinalize(TBool forseAction, TBool aDraw = ETrue);

    TRect GetControlRect(TInt aIdx) const;
    TRect GetUnconstrainedStringItemRect(CMIDStringItem* aStringItem) const;

    TInt FirstRowWithBottomBelowScreen();
    TInt LastRowWithTopAboveScreen();
    TInt NextPartiallyVisibleRowWithFocusableItem(TDirection aDirection);
    TInt FirstRowOnScreen(TDirection aDirection);
    TInt NextFocusableHorizontalIdx(TDirection aDirection);
    void ReplaceFocusedItem();
    void SetFocusedItem(TInt aFocusIdx, TDirection aDirection = ENone, TBool aDoScroll = ETrue);
    void UpdateScrollBar();
    CMIDItem& Item(TInt aIndex) const;
    CMIDControlItem& ControlItem(TInt aIndex) const;
    static CMIDControlItem& ControlItem(MMIDItem* aMMidItem);
    TInt Index(CCoeControl* aControl) const;
    void HandleItemVisibilityChange();
    // msk: this method should be obsolete, but it is needed if msk is not enabled
    TBool PostDefaultItemCommandEvent(TInt aIndex);
    void AddItemToFormL(CMIDControlItem& aControlItem);
    void RemoveItemFromForm(CMIDControlItem& aControlItem);

    // msk: this method should be obsolete, but it is needed if msk is not enabled
    TBool DoDefaultCommand();
    TInt Row(const CMIDControlItem& aItem) const;
    TInt Row(const CMIDControlItem& aItem, CMIDFormRow*& aRowPtr) const;
    TDirection ConvertKeyToDirection(TUint aCode);
    TInt ItemIndex(CMIDControlItem& aItem);

    TInt FirstRowAboveUnconstrainedStringItem(CMIDStringItem& aStringItem) const;
    TInt FirstRowOfUnconstrainedStringItem(CMIDStringItem& aStringItem) const;
    TInt FirstRowBelowUnconstrainedStringItem(CMIDStringItem& aStringItem) const;
    TInt FirstFocusableItemBeforeStartOfUnconstrainedStringItemIdx(CMIDStringItem& aStringItem);
    TInt FirstFocusableItemOnLastLineOfUnconstrainedStringItemIdx(CMIDStringItem& aStringItem);

    TBool ControlBelongsToSameUnconstrainedItem(CMIDControlItem* aCi,
            CMIDStringItem& aStringItem) const;


    void ResetItemsPreferredSizes();
    void SetItemsFormPointersToVal(CMIDForm* aVal);
    void SetRowExtentsWithRespectToScrolledLocation();
    void SetRowPositionsWithRespectToScrolledLocation();

    TBool ItemIsUnconstrained(TInt aIndex) const;
    TBool LayoutMatchesCurrentAlignment(CMIDControlItem& aControlItem,
                                        MMIDItem::TLayout aCurrentAlignment);
    TBool LayoutDefault(CMIDControlItem& aControlItem);
    TBool LayoutNewLineBefore(CMIDControlItem& aControlItem);
    TBool LayoutNewLineAfter(CMIDControlItem& aControlItem);

public:
    static TBool LayoutShrink(CMIDControlItem& aControlItem);
    static TBool LayoutExpand(CMIDControlItem& aControlItem);
    static TBool LayoutVerticalShrink(CMIDControlItem& aControlItem);
    static TBool LayoutVerticalExpand(CMIDControlItem& aControlItem);
    static TBool LayoutVerticalCenter(CMIDControlItem& aControlItem);
    static TBool LayoutBottom(CMIDControlItem& aControlItem);
    static TBool IsStringItem(CMIDControlItem& aControlItem);
    static TBool IsStringItemHyperlink(CMIDControlItem& aControlItem);
    static TBool IsStringItemButton(CMIDControlItem& aControlItem);
    static TBool IsCustomItem(CMIDControlItem& aControlItem);
    static TBool IsGaugeItem(CMIDControlItem& aControlItem);
    static TBool IsInteractiveGaugeItem(CMIDControlItem& aControlItem);
    static TBool IsTextFieldItem(CMIDControlItem& aControlItem);
    static TBool IsChoiceGroup(CMIDControlItem& aControlItem);
    static TBool IsDateField(CMIDControlItem& aControlItem);
    static TBool IsLabelContainerItem(CMIDControlItem& aControlItem);
    static TBool IsSpacerUsedForFormatting(CMIDControlItem& aControlItem);
    static TBool IsImageItem(CMIDControlItem& aControlItem);

#ifdef RD_JAVA_S60_RELEASE_9_2
public:
    void PostPendingUpEventL();
#endif // RD_JAVA_S60_RELEASE_9_2

private:
    TInt NumNewLinesBefore(CMIDControlItem& aControlItem);
    TInt NumNewLinesAfter(CMIDControlItem& aControlItem);
    void SizeItemsInFormRowsL();
    void DeleteRows();

    TInt HeightOfAllItems();
    void UpdateHeightOfAllItems();

    void Draw(const TRect& aRect) const;

    void LayoutFormL();
    TBool LayoutPending() const;
    void  LayoutAndDrawL();
    TInt  TrappedLayoutAndDraw(TBool aOnlyIfPending = EFalse);
    /**
     * Draws the "No data" string in the middle of the form container.
     * This is drawn when the form has no items.
     *
     * @note The text is aligned horizontaly not considering the scroll bar width
     */
    void  DrawNoDataStringL() const;

private: //AssignItemsToRowsL() and its helpers
    void AssignItemsToRowsL();

    TBool SkipNullItem(TInt aIndex);
    void CreateNewRowAndSetAsCurrentL();
    void VerifyRowAlignmentL(TInt aIndex);
    void InsertNewlinesL(TInt aNumNewlines, CMIDControlItem*  aControlItem);
    void CreateAndAddLabelContainerItemL(CMIDStringItem& aStringItem, CEikLabel& aLabel,
                                         TBool aIsStringItemContent);
    void InsertContainerItemsL(CMIDStringItem& aStringItem, CMIDItemLabel& aTextControl,
                               TBool aIsStringItemContent);
    void AddUnconstrainedStringItemL(TInt& aIndex);
    void AddDefaultItemL(TInt& aIndex);
    TBool OkToMoveToNextRow(const TSize& aItemSize) const;

public:
    /**
     *  Updates Physics model
     */
    void UpdatePhysics();
private:
    /**
     * Specialized pointer event handling for physics.
     */
    void HandlePhysicsPointerEventL(const TPointerEvent& aPointerEvent);
    /**
     * Forwards pointer event to Items
     */
    void ForwardPointerEventToItemL(const TPointerEvent& aPointerEvent);
    /**
     * Handles highlight transisition.
     * Called by HighlightTimerCallBack function
     */
    void HandleHighlightTimer();

    void SetHighlightBackgroundRects();
    void UpdateHighlightBackgroundPosition();
public:

    static TInt HighlightTimerCallback(TAny* aPtr);

    /*
     * Scrolls the Form content according to physics model.
     * Called by FormPhysics.
     */
    void HandlePhysicsScroll(TInt aScroll, TBool aDrawNow, TUint aFlags);
    /*
     * Called by FormPhysics when physics emulation ends
     */
    void PhysicsScrollingEnd();
    /*
     * Returns true if kinetic scrolling is enabled
     */
    TBool PhysicsEnabled();
    /*
     * Returns true if kinetic scrolling (flick or panning) is ongoing.
     */
    TBool PhysicsScrolling();
    /*
     * Returns the latest scroll delta
     */
    TInt ScrollDelta();

    void SetPhysicsScrollingDisabled(TBool aDisabled);

    /**
     * Function does feedback when focus is changed.
     */
    void DoFeedbackOnFocusChange(CMIDControlItem& aControlItem);

private:
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    /**
     * Function for feedback on kinetic scrolling .
     */
    void UpdateTactileFeedbackDensity();
    void DoScrollingFeedback();
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#ifdef RD_JAVA_S60_RELEASE_9_2
    void UpdateFormForSplitScreenChange();
#endif // RD_JAVA_S60_RELEASE_9_2

private: //data
    CMIDDisplayable& iDisplayable;
    MMIDEnv* iEnv;
    CTimeOutTimer* iLayoutTimer; // we use this Timer do defer laying out in some situations (this is for performance reasons).
    TTime iLayoutRequestTime;
    RArray<MMIDItem*> iItems;
    TInt iScroll; // the scroll position (in pixels)
    TInt iFocused; // index of the item that has the focus (KErrNotFound if we are not focused)
    MMIDItem::TLayout iInitialAlignment; // ELeft for left to right layout, otherwise ERight
    RArray<CMIDFormRow*> iRows;
    TInt iInnerWidth;
    TInt iInnerHeight;
    TBool iHasScrolled;

    CEikScrollBarFrame* iSBFrame;

    CMIDFormRow* iCurrentRow; //only a reference
    MMIDItem::TLayout iCurrentAlignment; //The alignment of the current row

    HBufC* iNoDataText; /** Text to be displayed if form is empty */

    /** The same skinned background used by AVKON forms and listboxes */
    CAknsListBoxBackgroundControlContext* iBackgroundControlContext;

    /** The total height of the form, this is the sum of the rows height */
    TInt iTotalFormHeight;

    TAknLayoutRect iMidpFormRect; // Layout of Rect of midp_form_pane (scroll bar included)
    TAknLayoutRect iFormRect; // Layout of Rect of form2_midp_field_pane (scroll bar excluded)
    TInt iFormHeight; // The height of the form

    CMIDPopupNoteController* iPopupController;
#ifdef RD_SCALABLE_UI_V2
    // Indicates if changing the focused item with pointer drag is currently possible.
    // EButton1Down must happen outside the focused item in order this to be possible.
    TBool iCanDragFocus;

    // true if focus has changed between TPointerEvent::EButton1Down and TPointerEvent::EButton1Up events
    TBool iFocusChangingWithPen;

    // True, if form has been scrolled between TPointerEvent::EButton1Down and TPointerEvent::EButton1Up events
    TBool iScrollOnPointerDown;

    TInt iLastFadeMessage;

    TInt iIndexPointedControl; // index of pointed Control
    TPointerEvent iLastValidPointerEvent;
    TPoint iLastValidPointedItemPosition;
#endif

    // Needed by kinetic scrolling
    CMIDControlItem*    iPointedControl;
    CMIDFormPhysics*    iPhysics;
    TTime               iStartTime;
    CPeriodic*          iHighlightTimer;
    TPointerEvent       iLastPointerDownEvent;
    TPoint              iStartPosition;
    TPoint              iLastDragPosition;
    TBool               iFlickOngoing;
    TBool               iPanningOngoing;
    TBool               iUpEventSent;
    TInt                iScrollDelta;
    TBool               iPreventPhysicsScrolling;
    TBool               iFlickStoppedOnDownEvent;
    TBool               iFeedbackExecutedInPointerDown;

    // The background control context for when the item is highlighted
    CAknsFrameBackgroundControlContext* iHighlightedBackgroundCc;

    /**
     * The startup trace should be done only once.
     */
    mutable TBool iStartUpTraceDone;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    /**
     * Kinetic scrolling tactile feedback
     */
    TInt iLastTactileFeedbackPos;
    TInt iTactileFeedbackDensity;
    TInt iLastPointerEventType;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#ifdef RD_JAVA_S60_RELEASE_9_2
    TBool iPartialScreenOpened;
    TBool iRemovedEditingItem;
#endif // RD_JAVA_S60_RELEASE_9_2

    // Storing index to last pointed control
    // before Form layout is performed
    TInt iLastPointedControl;

    TInt iLastGaugeValue;
};

inline void CMIDForm::Dispose()
{
    delete this;
}


inline TRect CMIDForm::FormRect()
{
    return iFormRect.Rect();
}

inline TInt CMIDForm::Width()
{
    return iInnerWidth; // form2_midp_field_pane.Width
}

inline TInt CMIDForm::Height()
{
    return iFormHeight; // midp_form_pane.Height
}

inline MMIDItem::TLayout CMIDForm::InitialAlignment()
{
    return iInitialAlignment;
}

inline CMIDDisplayable& CMIDForm::CurrentDisplayable()
{
    return iDisplayable;
}

inline CAknsFrameBackgroundControlContext* CMIDForm::GetHighlightBackgroundCc() const
{
    return iHighlightedBackgroundCc;
}

#ifdef RD_SCALABLE_UI_V2
inline TBool CMIDForm::IsScrolledOnPointerDown() const
{
    return iScrollOnPointerDown;
}
#endif //RD_SCALABLE_UI_V2

#endif // CMIDFORM_H
