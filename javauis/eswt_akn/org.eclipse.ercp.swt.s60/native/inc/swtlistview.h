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


#ifndef SWTLISTVIEW_H
#define SWTLISTVIEW_H


#include <eiklbo.h>
#include "eswtmobileextensions.h"
#include "swtscrollablebase.h"
#include "swtscrollbar.h"


class CAknIconArray;
class CSelectionIndexArray;
class CSwtGrid;
class CAknGridM;
class CAknGridView;
class CSwtControlMenu;
class CSwtMenu;
class CSwtMenuItem;


/**
 * CSwtListView
 * This class implements the eSWT ListView on S60 platform
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtListView)
        : public CAknControl
        , public ASwtScrollableBase
        , public MSwtListView
        , public MEikCommandObserver
        , public MEikListBoxObserver
{
public:
    static CSwtListView* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);
    static CSwtListView* NewLC(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                               MSwtComposite& aParent, TInt aStyle);
    virtual ~CSwtListView();
    void ScrollViewToShowAsManyItemsAsPossibleL();
    TInt ScrollBarBreadth(const CEikScrollBar* aBar);

// From CCoeControl
    CCoeControl*    ComponentControl(TInt aIndex) const;
    TInt            CountComponentControls() const;
    void            SizeChanged();
    void            PositionChanged();
    void            HandleResourceChange(TInt aType);
    void            FocusChanged(TDrawNow aDrawNow);
    TKeyResponse    OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void            MakeVisible(TBool aVisible);
    void            SetDimmed(TBool aDimmed);
    TTypeUid::Ptr   MopSupplyObject(TTypeUid aId);

private:
    void            Draw(const TRect& aRect) const;

// From MSwtListView
public:
    MSwtScrollable* Scrollable();
    void            AppendL(const TDesC& aPtr, const MSwtImage* aImage);
    void            InsertL(TInt aPos, const TDesC& aPtr, const MSwtImage* aImage);
    void            DeselectItem(TInt aIndex);
    void            DeselectRange(TInt aStart, TInt aEnd);
    void            DeselectItems(const TInt* aIndices, TInt aCount);
    void            DeselectAll();
    TInt            GetFocusIndex() const;
    void            GetItemL(TInt aItemIndex, TPtrC& aString) const;
    TPoint          GetIconSize(TInt aStyle) const;
    TInt            GetItemCount() const;
    CPtrCArray*     GetItemsL() const;
    CPtrCArray*     GetSelectionL() const;
    TInt            GetSelectionCount() const;
    const           CArrayFix<TInt>* GetSelectionIndices() const;
    TInt            GetTopIndex() const;
    TInt            IndexOf(const TDesC& aString, TInt& aStart) const;
    TBool           IsSelected(TInt aIndex) const;
    void            RemoveItemL(TInt aIndex);
    void            RemoveRangeL(TInt aStart, TInt aEnd);
    void            RemoveItemsL(const TInt* aIndices, TInt aCount);
    void            RemoveAllL();
    void            SelectItemL(TInt aIndex, TBool aScroll);
    void            SelectRangeL(TInt aStart, TInt aEnd);
    void            SelectAllL();
    void            SetItemsL(MDesCArray* aStringArray, RPointerArray<MSwtImage>& aImagesArray);
    void            SetLayoutL(TInt aLayout);
    void            SetLayoutDensityL(TInt aDensity);
    void            ShowSelection();
    void            SetFocusIndex(TInt aIndex);

// From ASwtScrollableBase
    CEikScrollBarFrame* SbFrame() const;
    void SetVisibleScrollBar(TInt aStyle, TBool aVisible);
    void ScrollBarDestroyed(const MSwtScrollBar* aScrollBar);
#if defined( RD_SCALABLE_UI_V2)
    void HandleScrollEventL(CEikScrollBar* aScrollBar,
                            TEikScrollEvent aEventType);
#endif

// From MSwtControl
    CCoeControl&    CoeControl();
    const           CCoeControl& CoeControl() const;
    void            ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TRgb            GetBackground() const;
    TRgb            GetForeground() const;
    void            SetBackgroundL(const MSwtColor* aColor);
    void            SetForegroundL(const MSwtColor* aColor);
    TSize           ComputeSizeL(TInt aWHint, TInt aHHint);
    TBool           IsKeyUsed(TUint aKeyCode) const;
    TBool           MSKSelCmdEnabled() const;
    const MSwtMenu* GetControlMenu() const;
    void            UpdateDoNotDrawFlag();
    HBufC*          MSKLabelL() const;
#if defined( RD_SCALABLE_UI_V2)
    void            HandlePointerEventL(const TPointerEvent& aPointerEvent);
    const MSwtMenu* GetStylusPopupControlMenu() const;
#endif

// From MEikCommandObserver
public:
    void ProcessCommandL(TInt aCmdId);

// From MEikListBoxObserver
public:
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

// Own methods
private:
    CSwtListView(MSwtDisplay& aDisplay, TSwtPeer aPeer, MSwtComposite& aParent, TInt aStyle);
    void ConstructL();

    enum TSwtListViewDensity
    {
        ELowListViewDensity     = 1,
        EMediumListViewDensity  = 2,
        EHighListViewDensity    = 3
    };

    void            ApplyGridCellLayoutL();
    CDesCArray*     GetTextArray() const;
    CAknIconArray*  GetIconArray() const;
    CAknGridM*      GetGridModel() const;
    CAknGridView*   GetGridView() const;
    CGulIcon*       CreateIconL(const MSwtImage* aIcon) const;
    void            DoGridCellLayoutL(const TSwtListViewDensity& aDensity);
    void            LoadLayouts(const TSwtListViewDensity& aDensity, TSize& aCellSize, TRect& aIconRect, TRect& aSelRect, TRect& aTextRect, TInt& aTextCellFontId);
    void            CalcRowsColumns(const TSize& aCellSize, TInt& aXCells, TInt& aYCells) const;
    void            AddTextL(const TDesC& aPtr, const TInt& aPos);
    void            AddIconL(const MSwtImage* aImage, const TInt& aPos);
    void            AddItemL(const TDesC& aPtr, const MSwtImage* aImage, const TInt& aPos);
    void            UpdateIconIndexL(const TInt& arrayIndex);
    void            UpdateSelectionIndexesL(TInt aStart, TInt aEnd, TBool aItemsAdded);
    void            UntabifyL(RBuf& aAll);
    TInt            GetCaption(const TInt& aIndex, TPtrC& aTxt) const;
    void            SingleSelectL(const TInt& aIndex, TBool aScroll);
    void            SingleOrMultiSelectL(const TInt& aIndex, TBool aScroll);
    void            UpdateTextColorL();
    CGulIcon*       GetMarkedItemIconL();
    void            HandleResourceChangeL(TInt aType);
    void            SizeChangedL();
    TInt            ItemIconIndex(const TInt& aItemPos);
    void            UpdateControlMenu();
    TInt            GetBorderSize(const TInt& aAlignment) const;
    TSize           GetCellSize() const;
    TUint           ISqrt(TUint x);
    TInt            GetRowColumnCount(const TInt& aItemCount, const TInt& aRowColumnCount) const;
    void            UpdateMSKLabelL();
    void            LayoutCell(const TRect& aCellRect, TRect& aTextRect, TRect& aIconRect, TRect& aSelRect);
    void            UpdateSetItemCount(const TInt& textChange, const TInt& iconChange);
    void            DoGridCellLayoutIfNeededL();
    void            UpdateRemovedItemCountersL(const TInt& aIndex);
    void            GetSelection(CArrayFix<TInt>* aSelectionArray) const;
    TInt            GetSelectionL(const TInt aIndex) const;
    void            SetSelectionL(const CArrayFix<TInt>* aSelectionArray) const;
    void            UpdateImageSizes(const TSize& aNewMaxSize);
    void            RemoveImageRef(const TInt& aIndex);
    void            GetCellVariants(const TSwtListViewDensity& aDensity,
                                    TInt& aCellVariant, TInt& aSubcellVariantG1,
                                    TInt& aSubcellVariantG2, TInt& aSubcellVariantT) const;
    void            UpdateIconSizes(const TSwtListViewDensity& aDensity,
                                    const TRect& aCellRect);
    void            GetLafFacadeRectIds(
        const TSwtListViewDensity& aDensity,
        CSwtLafFacade::TSwtLafFacadeRectId& aRectIdPane,
        CSwtLafFacade::TSwtLafFacadeRectId& aRectIdPaneG1,
        CSwtLafFacade::TSwtLafFacadeRectId& aRectIdPaneG2,
        CSwtLafFacade::TSwtLafFacadeTextId& aRectIdPaneT1,
        CSwtLafFacade::TSwtLafFacadeFontId& aRectIdPaneT1Font
    ) const;
    void            CoerceCellRect(const TSwtListViewDensity& aDensity,
                                   TRect& aCellRect) const;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
public:
    void DoControlSpecificFeedback(
        const TBool& aFirstTap,
        const TBool& aTappedToChildRect,
        const TPointerEvent& aPointerEvent) const;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

private: // data

    /**
     * Cell layout params for the grid.
     */
    TBool   iVerticalOrientation;
    TBool   iLeftToRight;
    TBool   iTopToBottom;
    TInt    iNumOfRows;
    TInt    iNumOfColumns;
    TInt    iWidthOfSpaceBetweenItems;
    TInt    iHeightOfSpaceBetweenItems;

    /**
     * The current density.
     */
    TSwtListViewDensity iDensity;

    /**
     * Icon sizes for getIconSize
     */
    TPoint iIconSizeInLowDensity;
    TPoint iIconSizeInMediumDensity;
    TPoint iIconSizeInHighDensity;

    /**
     * Number of non-NULL icons and texts
     */
    TInt iNumberOfTexts;
    TInt iNumberOfIcons;

    /**
     * The control menu.
     * Owned.
     */
    CSwtControlMenu* iMenu;

    /**
     * The control menu submenu.
     * Owned.
     */
    CSwtControlMenu* iSubMenu;

    /**
    * The control menu items.
    * Owned.
    */
    CSwtMenuItem* iMenuItemEditList;
    CSwtMenuItem* iMenuItemMarkUnmark;
    CSwtMenuItem* iMenuItemMarkAll;
    CSwtMenuItem* iMenuItemUnmarkAll;

    /**
     * The strings for mark/unmark command and its state.
     */
    TBool iIsMarkDisplayed;
    HBufC* iMarkString;
    HBufC* iUnmarkString;

    /**
     * The Selection array before a key event, for multiselection only.
     * Own.
     */
    CArrayFix<TInt>* iOldSelectionArray;

    /**
     * The Selection array for handling selection indexes with mirrored layout.
     * Own.
     */
    CArrayFix<TInt>* iSelectionArray;

    /**
     * The colors used to draw text and background.
     * Not own.
     */
    const MSwtColor* iTextColor;
    const MSwtColor* iBgColor;

    /**
     * Referenced images as the ListView does not copy the bitmaps it uses.
     */
    RPointerArray<const MSwtImage> iRefImages;

    /**
     * Maximum image size for the current layout.
     */
    TSize iMaxImageSize;

    /**
     * The grid control.
     * Own.
     */
    CSwtGrid* iGrid;

    /**
     * Deliver all pointer event to the scrollbar when this on.
     */
    TBool iVScrollBarGrabsPointerEvents;

#ifdef RD_SCALABLE_UI_V2
    /**
     * The control menu for stylus popup.
     * Owned.
     */
    CSwtControlMenu* iStylusPopupMenu;

    /**
     * The control menu items fro stylus popup.
     * Owned.
     */
    CSwtMenuItem* iStylusPopupMarkAll;
    CSwtMenuItem* iStylusPopupUnmarkAll;
#endif //RD_SCALABLE_UI_V2    

    /**
     * Stores last known focus index. Use only during paint and key handlers.
     */
    TInt iPrevFocusIndex;

    /**
     * Stores grid cell layout update status
     */
    TBool iIsGridCellLayoutNeeded;

    /**
     * View visible rect at last draw
     */
    mutable TRect iLastViewVisibleRect;
};


#endif // SWTLISTVIEW_H
