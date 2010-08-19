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


#include <AknGridView.h>
#include <aknlists.h>
#include <avkon.mbg>
#include <gulicon.h>
#include <AknIconArray.h>
#include <AknLayoutFont.h>
#include <AknsBasicBackgroundControlContext.h>
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfx.h>
#endif // RD_UI_TRANSITION_EFFECTS_LIST
#include <eswtcore.rsg>
#include <swtlaffacade.h>
#include "swtcontrolmenu.h"
#include "swtmenuitem.h"
#include "swtgrid.h"
#include "swtlistview.h"
#include "swtcontrolhelper.h"
#include "swtinstancecounts.h"
#include "swtdisplay.h"
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

#ifdef ESWT_NATIVE_UITHREAD_OWN_HEAP
#define DEBUG_CHECK_IMAGES
#endif // ESWT_NATIVE_UITHREAD_OWN_HEAP


// Column indexes of the subcells in the list item format string.
// Gfx cell has to be first for it to be drawn first.
static const TInt KIconColumn       = 0;
static const TInt KTextColumn       = 1;
static const TInt KItemMarkColumn   = 2;

// Constants used for listbox text string formatting
const TInt KSpaceForIndex = 20;
_LIT(KColumnSeparator, "\t");
_LIT(KSeparatorReplacement, " ");

// The indexes for marked and unmarked icons in the icon array
_LIT(KMarkOffIconArrayIndex, "1");
_LIT(KMarkOnIconArrayIndex, "0");

static const TInt KMarkOnIconArrayIndexInt  = 0;
static const TInt KSwtListViewItemGranularity = 4;
static const TInt KRowCalculationPrecisionInPixels = 5;


// ======== LOCAL FUNCTIONS ========


#ifdef DEBUG_CHECK_IMAGES
static void CheckRefImages(RPointerArray<const MSwtImage>& aRefImgArr,
                           CArrayPtrFlat<CGulIcon>& aImgArr,
                           const TSize& aMaxImageSize)
{
    TInt countImg = aImgArr.Count() - 2;
    TInt countRefImg = aRefImgArr.Count();

    if (countImg != countRefImg)
    {
        ASSERT(EFalse);
    }

    for (TInt i = 0; i < countImg; i++)
    {
        const CGulIcon* img = aImgArr[i + 2];
        const MSwtImage* refImg = aRefImgArr[i];
        if (img)
        {
            const CFbsBitmap* bmp = img->Bitmap();
            const CFbsBitmap* mask = img->Mask();

            ASSERT(bmp);
            if (refImg)
            {
                // The item has a valid image.
                // The bitmap is NOT owned.
                ASSERT(img->BitmapsOwnedExternally());
                const CFbsBitmap& bitmap = refImg->Bitmap();
                TSize imageSize = refImg->Bitmap().SizeInPixels();
                imageSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                                imageSize, aMaxImageSize);

                __UHEAP_MARK;
                ASSERT(bmp == &refImg->SubBitmap(imageSize));
                ASSERT(mask == refImg->SubMaskBitmap(imageSize, ETrue));
                // If image is not scaled, the primary refcount of
                // the image >= 2, otherwise it's >= 1 and subrefcount
                // for the given size should also be >= 1.
                if (imageSize == refImg->Bitmap().SizeInPixels())
                {
                    ASSERT(refImg->RefCount() >= 2);
                }
                else
                {
                    ASSERT(refImg->RefCount() >= 1);
                    ASSERT(refImg->SubRefCount(imageSize) >= 1);
                }

                __UHEAP_MARKEND;
            }
            else
            {
                // The item has a blank image (null image set from Java side)
                // The bitmap is owned.
                ASSERT(bmp->SizeInPixels() == TSize(0, 0));
                ASSERT(!mask);
            }
        }
    }
}
#endif // DEBUG_CHECK_IMAGES


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtListView Constructor
// ---------------------------------------------------------------------------
//
CSwtListView::CSwtListView(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtComposite& aParent, TInt aStyle)
        : ASwtScrollableBase(aDisplay, aPeer, &aParent, aStyle),
        iLeftToRight(ETrue),
        iTopToBottom(ETrue),
        iNumOfRows(1),
        iNumOfColumns(1),
        iWidthOfSpaceBetweenItems(0),
        iHeightOfSpaceBetweenItems(0),
        iDensity(EMediumListViewDensity),   // must match Java-side default
        iIsGridCellLayoutNeeded(EFalse),
        iFlickScrollingOngoing(EFalse)

{
    // Default to horizontal layout orientation (vertical scrollbar)
    //if( iStyle & KSwtStyleVertical )
    //    {
    //    iVerticalOrientation = ETrue;
    //    }
    //else
    //    {
    iVerticalOrientation = EFalse;
    iStyle &= ~KSwtStyleHScroll;
    iStyle |= KSwtStyleVScroll;
    //    }
}

// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//
void CSwtListView::ConstructL()
{
    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);

    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());
    SetComponentsToInheritVisibility(ETrue);

    iGrid = new(ELeave) CSwtGrid(this);
    iGrid->SetContainerWindowL(*this);

    if (AknLayoutUtils::LayoutMirrored())
    {
        iLeftToRight = EFalse;
    }

    // Construct either markable or normal grid
    TInt gridFlags = 0;
    if (iStyle & KSwtStyleMulti)
    {
#if defined( RD_SCALABLE_UI_V2)
        // For touch to work the same way as in other lists we need different
        // list styles for the grid.
        gridFlags |= CEikListBox::EMultipleSelection |
                     CEikListBox::ES60StyleMultiselection |
                     CEikListBox::EShiftEnterMarks;
#else
        gridFlags |= EAknListBoxMarkableGrid;
#endif
        // Enabling marking only by pressing enter key
        gridFlags |= CEikListBox::EEnterMarks;

        // Create control menu
        iMenu = CSwtControlMenu::NewL(iDisplay, NULL, 0);

        iMenuItemEditList = CSwtMenuItem::NewL(iDisplay, NULL, *iMenu, 0, 0);
        HBufC* buffer = iCoeEnv->AllocReadResourceLC(R_LISTVIEW_EDIT_LIST);
        TPtrC ptr = buffer->Des();
        iMenuItemEditList->SetTextL(ptr);
        CleanupStack::PopAndDestroy(buffer);

        iSubMenu = CSwtControlMenu::NewL(iDisplay, NULL, 0);
        iMenuItemEditList->SetMenuL(iSubMenu);
        iSubMenu->SetObserver(this);

        iMenuItemMarkUnmark = CSwtMenuItem::NewL(iDisplay, NULL, *iSubMenu, 0, 0, EAknCmdMark);
        iMenuItemMarkAll = CSwtMenuItem::NewL(iDisplay, NULL, *iSubMenu, 1, 0, EAknMarkAll);
        iMenuItemUnmarkAll = CSwtMenuItem::NewL(iDisplay, NULL, *iSubMenu, 2, 0, EAknUnmarkAll);

        iMarkString = iCoeEnv->AllocReadResourceL(R_LISTVIEW_MARK_ONE);
        iUnmarkString = iCoeEnv->AllocReadResourceL(R_LISTVIEW_UNMARK_ONE);
        ptr.Set(iMarkString->Des());
        iIsMarkDisplayed = ETrue;
        iMenuItemMarkUnmark->SetTextL(ptr);

        buffer = iCoeEnv->AllocReadResourceLC(R_LISTVIEW_MARK_ALL);
        ptr.Set(buffer->Des());
        iMenuItemMarkAll->SetTextL(ptr);
        CleanupStack::PopAndDestroy(buffer);

        buffer = iCoeEnv->AllocReadResourceLC(R_LISTVIEW_UNMARK_ALL);
        ptr.Set(buffer->Des());
        iMenuItemUnmarkAll->SetTextL(ptr);
        CleanupStack::PopAndDestroy(buffer);

#ifdef RD_SCALABLE_UI_V2
        iStylusPopupMenu = CSwtControlMenu::NewL(iDisplay, NULL, 0);
        iStylusPopupMenu->SetObserver(this);

        iStylusPopupMarkAll = CSwtMenuItem::NewL(iDisplay, NULL, *iStylusPopupMenu, 0, 0, EAknMarkAll);
        iStylusPopupMarkAll->SetTextL(iMenuItemMarkAll->Text());

        iStylusPopupUnmarkAll = CSwtMenuItem::NewL(iDisplay, NULL, *iStylusPopupMenu, 1, 0, EAknUnmarkAll);
        iStylusPopupUnmarkAll->SetTextL(iMenuItemUnmarkAll->Text());
#endif //RD_SCALABLE_UI_V2
    }

    iOldSelectionArray = new(ELeave) CArrayFixFlat<TInt>(2);
    iSelectionArray = new(ELeave) CArrayFixFlat<TInt>(2);
    iGrid->ConstructL(this, gridFlags);

    CArrayPtr< CGulIcon >* icons = new(ELeave) CAknIconArray(KSwtListViewItemGranularity);
    CleanupStack::PushL(icons);

    // Marked icon
    CGulIcon* markedIcon = GetMarkedItemIconL();
    CleanupStack::PushL(markedIcon);
    icons->AppendL(markedIcon);

    // Unmarked icon
    CGulIcon* unmarkedIcon = CreateIconL(NULL);
    CleanupStack::PushL(unmarkedIcon);
    icons->AppendL(unmarkedIcon);

    // Attach icon array to grid’s item drawer, itemDrawer takes ownership
    iGrid->ItemDrawer()->FormattedCellData()->SetIconArrayL(icons);
    CleanupStack::Pop(3, icons);

    DoGridCellLayoutL(iDensity);

    // This is needed to correct skinned background drawing problems when
    // the control is repositioned.
    iGrid->ItemDrawer()->ColumnData()->SetSkinEnabledL(EFalse);

    // Create non-window owning scrollbars to avoid clipping problems.
    iGrid->CreateScrollBarFrameL();

#ifdef RD_SCALABLE_UI_V2
    // WARNING!!! The expanded touch area does not move correctly togehter with the scrollbars!
    iGrid->ScrollBarFrame()->SetScrollBarFrameFlags(CEikScrollBarFrame::EDisableExpandedTouchArea);
#endif // RD_SCALABLE_UI_V2

    // In eSWT all scrollbars must be non window owing the main reason being that
    // the scrollbars must draw at exact same time with the parent control. This
    // is especially essential in ScrolledComposite. If the scrollbars would be
    // window owing, they would draw too much ahead of the parents creating an
    // ugly visual effect when flicking the ScrolledComposite.
    // The drawback of having non window owing scrollbars is too many paint events
    // which affects on the speed of kinetic scrolling in lists.
    iGrid->ScrollBarFrame()->CreateDoubleSpanScrollBarsL(EFalse, EFalse, ETrue, EFalse);
    iGrid->SetScrollbarVisibility(KSwtStyleVScroll, ETrue);
    iGrid->SetScrollbarVisibility(KSwtStyleHScroll, EFalse);

    UpdateControlMenu();

    // WORKAROUND! This is to avoid CAknGrid crash when its size is 0
    // and the List Transition Effects try to draw the grid. In fact the
    // root cause of the crash is our Shell not being always ready to draw
    // which is what Avkon expects. If that would be the case, the List
    // Transition Effects will never get a chance to draw a size 0 grid.
    iGrid->SetRect(TRect(0, 0, 1, 0));

#ifdef RD_SCALABLE_UI_V2
    iGrid->SetListBoxObserver(this);
#endif // RD_SCALABLE_UI_V2

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    // Effects do not work with non window owning scrollbars.
    CWindowGc* gc = iGrid->View()->ItemDrawer()->Gc();
    MAknListBoxTfx* transApi = CAknListLoader::TfxApi(gc);
    if (transApi)
    {
        transApi->EnableEffects(EFalse);
    }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    // This is indeed necessary, otherwise the background might not be drawn!
    SetBackground(this);

    ActivateL();
}

// ---------------------------------------------------------------------------
// Sets up the grid based on the given density and the style flags received
// at the construction time. Can/must be called again to reset the layout.
// ---------------------------------------------------------------------------
//
void CSwtListView::DoGridCellLayoutL(const TSwtListViewDensity& aDensity)
{
    // Load the layout data for the resolution
    TSize cellSize(0, 0);
    TRect iconSubcell(0, 0, 0, 0);
    TRect selectionMarkSubcell(0, 0, 0, 0);
    TRect textSubcell(0, 0, 0, 0);
    TInt textCellFontId = 0;
    LoadLayouts(aDensity, cellSize, iconSubcell, selectionMarkSubcell, textSubcell, textCellFontId);
    ASSERT(textCellFontId != 0);

    CalcRowsColumns(cellSize, iNumOfColumns, iNumOfRows);

    iGrid->SetLayoutL(iVerticalOrientation,
                      iLeftToRight,
                      iTopToBottom,
                      iNumOfColumns,
                      iNumOfRows,
                      cellSize,
                      iWidthOfSpaceBetweenItems,
                      iHeightOfSpaceBetweenItems);
    iGrid->SetPrimaryScrollingType(CAknGridView::EScrollIncrementLineAndLoops);
    iGrid->SetSecondaryScrollingType(CAknGridView::EScrollIncrementLineAndLoops);

    const CAknLayoutFont* font = AknLayoutUtils::LayoutFontFromId(textCellFontId);
    ASSERT(font != NULL);

    const TInt KTextDefaultColor = 215; // 215 = black, overridden later

    CFormattedCellListBoxItemDrawer* itemDrawer = iGrid->ItemDrawer();

    // Setup the icon subcell
    AknListBoxLayouts::SetupGridFormGfxCell(
        *iGrid,                 // Reference to grid control
        itemDrawer,             // Pointer to the item drawer
        KIconColumn,            // Column index
        iconSubcell.iTl.iX,     // Left position
        iconSubcell.iTl.iY,     // Top position
        0,                      // Right - unused
        0,                      // Bottom - unused
        iconSubcell.Width(),    // Width
        iconSubcell.Height(),   // Height
        iconSubcell.iTl,        // Start position
        iconSubcell.iBr);       // End position

    // Setup the text subcell
    AknListBoxLayouts::SetupFormTextCell(
        *iGrid,                     // Reference to grid
        itemDrawer,                 // Pointer to the item drawer
        KTextColumn,                // Column index
        font,                       // The font
        KTextDefaultColor,          // Color
        textSubcell.iTl.iX,         // Left margin
        0,                          // Right margin - unused
        (textSubcell.iTl.iY + textSubcell.Height() - (font->MaxDescent())),  // Baseline
        textSubcell.Width(),        // Text width
        CGraphicsContext::ECenter,  // Text alignment
        textSubcell.iTl,            // Start position
        textSubcell.iBr);           // End position

    CFormattedCellListBoxData* gridData = itemDrawer->FormattedCellData();
    gridData->SetTransparentSubCellL(KTextColumn, ETrue);

    // Set-up markable grid if required
    if (iStyle & KSwtStyleMulti)
    {
        // Set column index in the format string
        itemDrawer->SetItemMarkPosition(KItemMarkColumn);
        // Set icon array index of marked item icon
        itemDrawer->SetItemMarkReplacement(KMarkOnIconArrayIndex);
        // Don’t display all items as marked initially
        itemDrawer->SetItemMarkReverse(ETrue);
        // Setup the selection mark subcell
        AknListBoxLayouts::SetupFormGfxCell(
            *iGrid,                         // Reference to grid control
            itemDrawer,                     // Pointer to the item drawer
            KItemMarkColumn,                // Column index
            selectionMarkSubcell.iTl.iX,    // Left position
            selectionMarkSubcell.iTl.iY,    // Top position
            0,                              // Right - unused
            0,                              // Bottom - unused
            selectionMarkSubcell.Width(),   // Width
            selectionMarkSubcell.Height(),  // Height
            selectionMarkSubcell.iTl,       // Start position
            selectionMarkSubcell.iBr);      // End position

        gridData->SetTransparentSubCellL(KItemMarkColumn, ETrue);
    }

    TSize maxImageSize = iconSubcell.Size();

    if (iMaxImageSize != maxImageSize)
    {
        UpdateImageSizes(maxImageSize);
        iMaxImageSize = maxImageSize;
    }

    UpdateTextColorL();
}

// ---------------------------------------------------------------------------
// A helper to get the text array of item texts.
// ---------------------------------------------------------------------------
//
CDesCArray* CSwtListView::GetTextArray() const
{
    MDesCArray* array = GetGridModel()->ItemTextArray();
    return static_cast<CDesCArray*>(array);
}

// ---------------------------------------------------------------------------
// A helper to get the array of icons.
// ---------------------------------------------------------------------------
//
CAknIconArray* CSwtListView::GetIconArray() const
{
    CArrayPtr<CGulIcon>* array = iGrid->ItemDrawer()->FormattedCellData()->IconArray();
    return static_cast<CAknIconArray*>(array);
}

// ---------------------------------------------------------------------------
// A helper to get the model of the grid.
// ---------------------------------------------------------------------------
//
CAknGridM* CSwtListView::GetGridModel() const
{
    return static_cast<CAknGridM*>(iGrid->Model());
}

// ---------------------------------------------------------------------------
// A helper to get the view of the grid.
// ---------------------------------------------------------------------------
//
CAknGridView* CSwtListView::GetGridView() const
{
    return static_cast<CAknGridView*>(iGrid->View());
}

// ---------------------------------------------------------------------------
// A helper to get array of logical selection indices.
// ---------------------------------------------------------------------------
//
void CSwtListView::GetSelection(CArrayFix<TInt>* aSelectionArray) const
{
    CAknGridView* gridView = GetGridView();

    if (aSelectionArray && gridView)
    {
        aSelectionArray->Reset();

        TRAP_IGNORE(gridView->GetSelectionIndexesL(aSelectionArray));

        // The returned indexes are not the logical indexes (which take
        // right-to-left ordering into account), make them logical if
        // necessary.
        if (!iLeftToRight)
        {
            for (int i = 0; i < aSelectionArray->Count(); i++)
            {
                aSelectionArray->At(i) =
                    gridView->ActualDataIndex(aSelectionArray->At(i));
            }
        }
    }
}

// ---------------------------------------------------------------------------
// A helper to get logical selection index.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::GetSelectionL(const TInt aIndex) const
{
    if (iLeftToRight)
    {
        return aIndex;
    }

    CAknGridView* gridView = GetGridView();
    if (!gridView)
    {
        User::Leave(ESwtErrorCannotGetSelection);
    }

    return gridView->ActualDataIndex(aIndex);
}

// ---------------------------------------------------------------------------
// A helper to set selection array of logical selection indices.
// ---------------------------------------------------------------------------
//
void CSwtListView::SetSelectionL(const CArrayFix<TInt>* aSelectionArray) const
{
    CAknGridView* gridView = GetGridView();

    if (aSelectionArray && gridView)
    {
        gridView->ClearSelection();

        for (TInt i = 0; i < aSelectionArray->Count(); i++)
        {
            TInt item = aSelectionArray->At(i);

            if (!iLeftToRight)
            {
                item = gridView->ActualDataIndex(item);
            }

            gridView->SelectItemL(item);
        }
    }
}

// ---------------------------------------------------------------------------
// A helper to create an icon out of the given image object.
// ---------------------------------------------------------------------------
//
CGulIcon* CSwtListView::CreateIconL(const MSwtImage* aIcon) const
{
    CGulIcon* icon = NULL;
    if (aIcon)
    {
        CFbsBitmap* bmp = 0;
        CFbsBitmap* mask = 0;

        TSize bitmapSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                               aIcon->Bitmap().SizeInPixels(),
                               iMaxImageSize);

        bmp =  const_cast<CFbsBitmap*>(&aIcon->SubBitmap(bitmapSize));
        mask = const_cast<CFbsBitmap*>(aIcon->SubMaskBitmap(bitmapSize, ETrue));
        aIcon->AddRef();
        aIcon->AddSubRef(bitmapSize);

        icon = CGulIcon::NewL();
        icon->SetBitmapsOwnedExternally(ETrue);
        icon->SetBitmap(bmp);
        icon->SetMask(mask);
    }
    else // Create a blank icon
    {
        CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
        CleanupStack::PushL(bmp);
        User::LeaveIfError(bmp->Create(TSize(0, 0), EColor256));
        ASSERT(bmp);
        icon = CGulIcon::NewL(bmp);
        CleanupStack::Pop(bmp);   // bmp
    }
    return icon;
}

// ---------------------------------------------------------------------------
// Loads the layouts for the current resolution.
// ---------------------------------------------------------------------------
//
void CSwtListView::LoadLayouts(const TSwtListViewDensity& aDensity,
                               TSize& aCellSize,
                               TRect& aIconRect,
                               TRect& aSelRect,
                               TRect& aTextRect,
                               TInt& aTextCellFontId)
{
    // Grid cell variants
    TInt cellVariant;
    TInt subcellVariantG1;
    TInt subcellVariantG2;
    TInt subcellVariantT;
    GetCellVariants(aDensity, cellVariant, subcellVariantG1, subcellVariantG2,
                    subcellVariantT);

    // LAF rectangle IDs
    CSwtLafFacade::TSwtLafFacadeRectId rectIdPane;
    CSwtLafFacade::TSwtLafFacadeRectId rectIdPaneG1;
    CSwtLafFacade::TSwtLafFacadeRectId rectIdPaneG2;
    CSwtLafFacade::TSwtLafFacadeTextId rectIdPaneT1;
    CSwtLafFacade::TSwtLafFacadeFontId rectIdPaneT1Font;
    GetLafFacadeRectIds(aDensity, rectIdPane, rectIdPaneG1, rectIdPaneG2,
                        rectIdPaneT1, rectIdPaneT1Font);

    // Grid cell
    TAknLayoutRect cellLayoutRect = CSwtLafFacade::GetLayoutRect(rectIdPane,
                                    TRect(), cellVariant, 0, 0);
    TRect cellRect = cellLayoutRect.Rect();

    // Coerces cellrects if necessary
    CoerceCellRect(aDensity, cellRect);

    // For mirrored layouts the rect is from left to right (containing negative
    // values), make sure that the rect has positive coordinates.
    cellRect.Move(-cellRect.iTl.iX, 0);

    aCellSize = cellRect.Size();

    // Icon subcell
    TAknLayoutRect iconRect = CSwtLafFacade::GetLayoutRect(rectIdPaneG1,
                              cellRect, subcellVariantG1);
    aIconRect.SetRect(iconRect.Rect().iTl, iconRect.Rect().iBr);

    // Markable grid check mark subcell
    // At this point it looks like the layout for the checkmark is wrong.
    // The bounds of the checkmark icon exceed the bounds of the item.
    // Therefore, we use TRect() here and count only on the height and width.
    TAknLayoutRect selRect = CSwtLafFacade::GetLayoutRect(rectIdPaneG2,
                             TRect(), subcellVariantG2);
    aSelRect.SetRect(selRect.Rect().iTl, selRect.Rect().iBr);

    // Updates sizes get by getIconSize()
    UpdateIconSizes(aDensity, cellRect);

    // Caption text subcell
    TAknLayoutText textRect = CSwtLafFacade::GetLayoutText(rectIdPaneT1,
                              cellRect, subcellVariantT);
    aTextRect.SetRect(textRect.TextRect().iTl, textRect.TextRect().iBr);

    // Layout subcells inside the cell
    LayoutCell(cellRect, aTextRect, aIconRect, aSelRect);

    aTextCellFontId = CSwtLafFacade::GetFontId(rectIdPaneT1Font, subcellVariantT);

    TRect controlRect = BorderInnerRect();
    TSize controlSize(controlRect.iBr.iX - controlRect.iTl.iX,
                      controlRect.iBr.iY - controlRect.iTl.iY);

    // If control size is smaller than cell size, set cell size to 1*1 pixels. This is a workaround
    // to the problem where full sized cells were drawn outside of control's borders when control
    // size was smaller than cell size.
    if (controlSize.iHeight < aCellSize.iHeight ||
            controlSize.iWidth < aCellSize.iWidth)
    {
        aCellSize.iWidth = 1;
        aCellSize.iHeight = 1;
        TInt xCoord = cellRect.iTl.iX;
        TInt yCoord = cellRect.iTl.iY;
        aIconRect = TRect(xCoord, yCoord, xCoord, yCoord);
        aSelRect =  TRect(xCoord, yCoord, xCoord, yCoord);
        aTextRect = TRect(xCoord, yCoord, xCoord, yCoord);

        iIconSizeInLowDensity.SetXY(0, 0);
        iIconSizeInMediumDensity.SetXY(0, 0);
        iIconSizeInHighDensity.SetXY(0, 0);
    }

}

// ---------------------------------------------------------------------------
// Layout text and icon rectangles inside a cell.
// ---------------------------------------------------------------------------
//
void CSwtListView::LayoutCell(const TRect& aCellRect, TRect& aTextRect,
                              TRect& aIconRect, TRect& aSelRect)
{

    TInt cellWidth = aCellRect.iBr.iX - aCellRect.iTl.iX;
    TInt cellHeight = aCellRect.iBr.iY - aCellRect.iTl.iY;
    TInt textWidth = aTextRect.iBr.iX - aTextRect.iTl.iX;
    TInt textHeight = aTextRect.iBr.iY - aTextRect.iTl.iY;
    TInt iconWidth = aIconRect.iBr.iX - aIconRect.iTl.iX;
    TInt iconHeight = aIconRect.iBr.iY - aIconRect.iTl.iY;

    // Center the text and icon horizontally
    TInt textX = (cellWidth - textWidth) / 2;
    TInt textY = aTextRect.iTl.iY;
    TInt iconX = (cellWidth - iconWidth) / 2;
    TInt iconY = aIconRect.iTl.iY;

    if (iNumberOfIcons == 0 && iNumberOfTexts > 0)
    {
        // There's no icon in any item, so center the cell text vertically.
        textY = (cellHeight - textHeight) / 2;
    }
    else if (iNumberOfIcons > 0 && iNumberOfTexts == 0)
    {
        // There's no text in any item, so center the icons vertically.
        iconY = (cellHeight - iconHeight) / 2;
    }


    aTextRect.iTl.SetXY(textX, textY);
    aTextRect.iBr.SetXY(textX + textWidth, textY + textHeight);
    aIconRect.iTl.SetXY(iconX, iconY);
    aIconRect.iBr.SetXY(iconX + iconWidth, iconY + iconHeight);

    if (AknLayoutUtils::LayoutMirrored())
    {
        TInt w = aSelRect.Width();
        TInt h = aSelRect.Height();
        aSelRect.iTl.SetXY(aCellRect.iTl.iX + 1, aCellRect.iTl.iY + 1);
        aSelRect.iBr.SetXY(aCellRect.iTl.iX + 1 + w, aCellRect.iTl.iY + 1 + h);
    }
    else
    {
        TInt w = aSelRect.Width();
        TInt h = aSelRect.Height();
        aSelRect.iTl.SetXY(aCellRect.iBr.iX - 1 - w, aCellRect.iTl.iY + 1);
        aSelRect.iBr.SetXY(aCellRect.iBr.iX - 1, aCellRect.iTl.iY + 1 + h);
    }
}


// ---------------------------------------------------------------------------
// Returns ideal cell size for current grid density.
// ---------------------------------------------------------------------------
//
TSize CSwtListView::GetCellSize() const
{
    // Grid cell variants
    TInt cellVariant;
    TInt subcellVariantG1;
    TInt subcellVariantG2;
    TInt subcellVariantT;
    GetCellVariants(iDensity, cellVariant, subcellVariantG1, subcellVariantG2,
                    subcellVariantT);

    // LAF rectangle IDs
    CSwtLafFacade::TSwtLafFacadeRectId rectIdPane;
    CSwtLafFacade::TSwtLafFacadeRectId rectIdPaneG1;
    CSwtLafFacade::TSwtLafFacadeRectId rectIdPaneG2;
    CSwtLafFacade::TSwtLafFacadeTextId rectIdPaneT1;
    CSwtLafFacade::TSwtLafFacadeFontId rectIdPaneT1Font;
    GetLafFacadeRectIds(iDensity, rectIdPane, rectIdPaneG1, rectIdPaneG2,
                        rectIdPaneT1, rectIdPaneT1Font);

    // Grid cell
    TAknLayoutRect cellLayoutRect = CSwtLafFacade::GetLayoutRect(rectIdPane,
                                    TRect(), cellVariant, 0, 0);
    TRect cellRect = cellLayoutRect.Rect();

    // Coerces cellrects if necessary
    CoerceCellRect(iDensity, cellRect);

    // For arabic layout, rect might be negative, so move it to (0,0) so that we get positive size.
    cellRect.Move(-cellRect.iTl.iX, 0);

    return cellRect.Size();
}

// ---------------------------------------------------------------------------
// A helper to calculate how many cells to have per row/column based on
// the given cell size.
// ---------------------------------------------------------------------------
//
void CSwtListView::CalcRowsColumns(const TSize& aCellSize,
                                   TInt& aXCells,
                                   TInt& aYCells) const
{
    TRect clientRect = ClientRect();

    aXCells = 1;
    aYCells = 1;

    if (aCellSize.iWidth != 0)
    {
        aXCells = clientRect.Width() / aCellSize.iWidth;
    }
    if (aCellSize.iHeight != 0)
    {
        aYCells = clientRect.Height() / aCellSize.iHeight;
        if (clientRect.Height() % aCellSize.iHeight > KRowCalculationPrecisionInPixels)
        {
            aYCells++;
        }
    }

    // 1x1 grid is the minimum, the grid asserts this
    if (aXCells < 1)
    {
        aXCells = 1;
    }
    if (aYCells < 1)
    {
        aYCells = 1;
    }
}

// ---------------------------------------------------------------------------
// A helper to handle adding of a listbox item text.
// ---------------------------------------------------------------------------
//
void CSwtListView::AddTextL(const TDesC& aPtr,
                            const TInt& aPos)
{
    // Untabify the item caption
    RBuf caption;
    caption.CreateL(aPtr);
    caption.CleanupClosePushL();
    UntabifyL(caption);

    // Create the format string for the grid item

    TInt iconIndex = ItemIconIndex(aPos);
    RBuf buf;
    buf.CreateL(KSpaceForIndex +
                KColumnSeparator().Length() * 2 + // 2 = two separators
                caption.Length() +
                KMarkOffIconArrayIndex().Length());
    buf.AppendNum(iconIndex);
    buf.Append(KColumnSeparator);
    buf.Append(caption);
    buf.Append(KColumnSeparator);
    buf.Append(KMarkOffIconArrayIndex);

    // Append the text to the text array
    buf.CleanupClosePushL();
    CDesCArray* textArray = GetTextArray();
    if (textArray)
    {
        textArray->InsertL(aPos, buf);
    }

    if (caption.Length() > 0)
    {
        UpdateSetItemCount(1, 0);
    }

    CleanupStack::PopAndDestroy(2);
}

// ---------------------------------------------------------------------------
// A helper to handle adding of an icon for a list item.
// ---------------------------------------------------------------------------
//
void CSwtListView::AddIconL(const MSwtImage* aImage,
                            const TInt& aPos)
{
    CGulIcon* icon = CreateIconL(aImage);
    CleanupStack::PushL(icon);
    TInt index = ItemIconIndex(aPos);
    GetIconArray()->InsertL(index, icon);

    const MSwtImage* refImg = aImage;
    const TInt refImgCount = iRefImages.Count();
    if (refImgCount > aPos)
    {
        iRefImages.InsertL(refImg, aPos);
    }
    else
    {
        iRefImages.AppendL(refImg);
    }

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iRefImages, *(GetIconArray()), iMaxImageSize);
#endif

    if (aImage)
    {
        UpdateSetItemCount(0, 1);
    }

    CleanupStack::Pop(icon);
}

// ---------------------------------------------------------------------------
// A helper to handle adding of an item. This includes the item text and
// the icon. Also selection/selection events/focus are handled appropriately.
// Can be called with null image.
// ---------------------------------------------------------------------------
//
void CSwtListView::AddItemL(const TDesC& aPtr,
                            const MSwtImage* aImage,
                            const TInt& aPos)
{
    // Store the selection state
    GetSelection(iOldSelectionArray);

    TRAPD(textError, AddTextL(aPtr, aPos));
    if (textError)
    {
        User::Leave(ESwtErrorItemNotAdded);
    }
    TRAPD(iconError, AddIconL(aImage, aPos));
    if (iconError)
    {
        // Remove the text as the icon can't be added
        ASSERT(GetTextArray()->Count() > 0);
        GetTextArray()->Delete(GetTextArray()->Count() - 1);
        User::Leave(ESwtErrorItemNotAdded);
    }

    // Item added successfully

    // Update grid cell layout if neccessary
    DoGridCellLayoutIfNeededL();

    // All the icon indexes after the added item need updating
    TInt textCount = GetTextArray()->Count();
    for (TInt iTxt = aPos + 1; iTxt < textCount; ++iTxt)
    {
        UpdateIconIndexL(iTxt);
    }

    iGrid->HandleItemAdditionL();

    // Restore the selection state updating the indexes.
    UpdateSelectionIndexesL(aPos, aPos, ETrue);

    // Focus must move to the newly added item.
    if (iStyle & KSwtStyleMulti)
    {
        iGrid->SetCurrentDataIndex(aPos);
    }
    SingleSelectL(GetSelectionL(aPos), ETrue);

    UpdateMSKLabelL();

    Redraw();
    iDisplay.PostSelectionEventL(iPeer);
}

// ---------------------------------------------------------------------------
// Updates the icon index in the item text array at the given index.
// Icon index is invalidated when adding or removing items anywhere but
// the end of the array.
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateIconIndexL(const TInt& aIndex)
{
    CDesCArray* textArray = GetTextArray();
    TPtrC itemText = textArray->MdcaPoint(aIndex);
    TInt separatorPos = itemText.Find(KColumnSeparator);
    User::LeaveIfError(separatorPos);

    RBuf buf;
    buf.CreateL(KSpaceForIndex + itemText.Length());
    buf.AppendNum(ItemIconIndex(aIndex));
    buf.Append(itemText.Mid(separatorPos));

    buf.CleanupClosePushL();
    textArray->Delete(aIndex);
    textArray->InsertL(aIndex, buf);

    CleanupStack::PopAndDestroy(1);
}

// ---------------------------------------------------------------------------
// Updates the selection indexes of items starting from the given index.
// Selected item indexes after the given index are decremented or incremented
// depending on if items have been added or removed.
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateSelectionIndexesL(TInt aStart, TInt aEnd, TBool aItemsAdded)
{
    ASSERT(iOldSelectionArray != NULL);
    TInt numberOfItemsInRange = aEnd - aStart + 1;

    // Update the selection indexes
    TInt selectionCount = iOldSelectionArray->Count();
    for (TInt iOld = selectionCount - 1; iOld >= 0; --iOld)
    {
        // First delete the selection indexes for items that have been removed
        if (aItemsAdded == EFalse &&
                (*iOldSelectionArray)[iOld] >= aStart && (*iOldSelectionArray)[iOld] <= aEnd)
        {
            iOldSelectionArray->Delete(iOld);
            continue;
        }
        // Then update the affected selection indexes
        if ((*iOldSelectionArray)[iOld] >= aStart)
        {
            if (aItemsAdded)
            {
                (*iOldSelectionArray)[iOld] += numberOfItemsInRange;
            }
            else // items were removed
            {
                (*iOldSelectionArray)[iOld] -= numberOfItemsInRange;
            }
        }
    }

    // New updated selections to the control
    SetSelectionL(iOldSelectionArray);

    iOldSelectionArray->Reset();
}

// ---------------------------------------------------------------------------
// Recursively replaces the tab characters in the buffer. Tab characters are
// not allowed in the item texts because they are used exclusively as
// field separators.
// ---------------------------------------------------------------------------
//
void CSwtListView::UntabifyL(RBuf& aAll)
{
    TInt separatorPos = aAll.Find(KColumnSeparator);
    if (separatorPos != KErrNotFound)
    {
        RBuf end;
        end.CreateL(aAll.Length());
        end.CleanupClosePushL();
        end.Append(aAll.Mid(separatorPos + 1));
        UntabifyL(end);

        // Now 'end' doesn't have any separators in it

        end.Insert(0, KSeparatorReplacement);
        end.Insert(0, aAll.Left(separatorPos));
        aAll.Copy(end);
        CleanupStack::PopAndDestroy();
    }
}

// ---------------------------------------------------------------------------
// Obtains a pointer descriptor to the part of the item text that contains
// the caption.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::GetCaption(const TInt& aIndex, TPtrC& aTxt) const
{
    ASSERT(KTextColumn == 1);
    ASSERT(GetItemCount() > 0);

    // Get a pointer to the item's complete text
    aTxt.Set(GetGridModel()->ItemText(aIndex));

    // Find the 1st separator from where the caption starts
    TInt separatorPos = aTxt.Find(KColumnSeparator);
    if (separatorPos == KErrNotFound)
    {
        return KErrNotFound;
    }

    // Reset the pointer to point to the beginning of the caption
    TInt newLength = aTxt.Length() - separatorPos;
    aTxt.Set(&aTxt[ separatorPos + 1 ], newLength);

    // Find the 1st (2nd) separator where the caption ends
    separatorPos = aTxt.Find(KColumnSeparator);
    if (separatorPos == KErrNotFound)
    {
        return KErrNotFound;
    }

    // Set length correctly to just reach the end of the caption
    newLength = separatorPos;
    aTxt.Set(&aTxt[ 0 ], newLength);

    return KErrNone;
}

// ---------------------------------------------------------------------------
// View must show as many items as possible. It's not allowed that the last
// row/column would be empty while some of the items would be outside
// the view. The view must scroll to fully utilise the available space to
// show as many rows/columns as fit in the view.
// ---------------------------------------------------------------------------
//
void CSwtListView::ScrollViewToShowAsManyItemsAsPossibleL()
{
    TInt lastItemRowIndex;
    TInt lastItemColIndex;
    TInt firstItemRowIndex;
    TInt firstItemColIndex;
    TBool scrollingNeeded = EFalse;

    if (!(GetItemCount() > 0))
    {
        return;
    }

    // Get the view positions of the first and last list items.
    GetGridView()->CalcRowAndColIndexesFromItemIndex(GetItemCount() -1,
            lastItemRowIndex,
            lastItemColIndex);
    GetGridView()->CalcRowAndColIndexesFromItemIndex(0,
            firstItemRowIndex,
            firstItemColIndex);

    // Check if there is 1 or more complete rows/columns of unused space
    // in the end of the view.
    if (iVerticalOrientation == EFalse)
    {
        if (lastItemRowIndex >= 0 && lastItemRowIndex < iNumOfRows - 1)
        {
            // Last item is in the view and is not on the last row.
            //  Check if the first item is outside the view.
            if (firstItemRowIndex < 0)
            {
                scrollingNeeded = ETrue;
            }
        }
    }
    else
    {
        if (lastItemColIndex >= 0 && lastItemColIndex < iNumOfColumns - 1)
        {
            // Last item is displayed in the view and is not on the last
            // column. Check if the first item is outside the view.
            if (firstItemColIndex < 0)
            {
                scrollingNeeded = ETrue;
            }
        }
    }

    if (scrollingNeeded)
    {
        // Move the focus to the beginning and back to the correct index
        // to reposition the view.
        TInt currentDataIndex = iGrid->CurrentDataIndex();
        GetGridView()->MoveToItemIndexL(0, CListBoxView::ENoSelection);
        GetGridView()->MoveToItemIndexL(currentDataIndex,
                                        CListBoxView::ENoSelection);
    }
}

TInt CSwtListView::ScrollBarBreadth(const CEikScrollBar* aBar)
{
    return iDisplay.UiUtils().ScrollBarBreadth(aBar);
}

// ---------------------------------------------------------------------------
// A helper for selecting items appropriately in single selection grid.
// Called when selection is desired only in single style.
// ---------------------------------------------------------------------------
//
void CSwtListView::SingleSelectL(const TInt& aIndex, TBool aScroll)
{
    if (!(iStyle & KSwtStyleMulti))
    {
        if (aScroll && GetGridView()->ViewRect().Height() >= iGrid->ItemHeight())
        {
            iGrid->View()->VerticalMoveToItemL(aIndex, CListBoxView::ESingleSelection);
        }
        else
        {
            TInt old = iGrid->CurrentItemIndex();
            GetGridView()->SetCurrentItemIndex(aIndex); //we do not call CEikListBox::SetCurrentItemIndex, because we do not want to scroll.
            GetGridView()->UpdateSelectionL(CListBoxView::ESingleSelection);
            if (old != -1)
            {
                GetGridView()->DrawItem(old);
            }
            ASSERT(iGrid->SelectionIndexes()->Count() <= 1);
        }
    }
}

// ---------------------------------------------------------------------------
// A helper for selecting items appropriately in either single or multi
// selection grid. Called when selection is desired in either case.
// ---------------------------------------------------------------------------
//
void CSwtListView::SingleOrMultiSelectL(const TInt& aIndex, TBool aScroll)
{
    CAknGridView* gridView = GetGridView();

    ASSERT(gridView);

    if (iStyle & KSwtStyleMulti)
    {
        gridView->SelectItemL(gridView->ActualDataIndex(aIndex));

        if (aScroll)
        {
            iGrid->ScrollToMakeItemVisible(aIndex);
        }
    }
    else
    {
        SingleSelectL(aIndex, aScroll);
    }
}

// ---------------------------------------------------------------------------
// Updates the colors used to draw text in the grid. The normal text color
// and the highlighted (focused) text color.
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateTextColorL()
{
    CFormattedCellListBoxData::TColors colors;
    TInt error = 0;

    if (iTextColor)
    {
        // Use the user defined color for all text
        colors.iText = TRgb(iTextColor->RgbValue());
        colors.iHighlightedText = TRgb(iTextColor->RgbValue());
    }
    else
    {
        // Get the colors from the skin
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        error = AknsUtils::GetCachedColor(skin, colors.iText, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG9);
        error += AknsUtils::GetCachedColor(skin, colors.iHighlightedText, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG11);
    }
    if (!error)
    {
        iGrid->ItemDrawer()->FormattedCellData()->SetSubCellColorsL(KTextColumn, colors);
        iGrid->ItemDrawer()->SetTextColor(colors.iText);
    }
}

// ---------------------------------------------------------------------------
// Helper to get the marked item icon from the current skin.
// ---------------------------------------------------------------------------
//
CGulIcon* CSwtListView::GetMarkedItemIconL()
{
    CGulIcon* icon = AknsUtils::CreateGulIconL(AknsUtils::SkinInstance(),
                     KAknsIIDQgnIndiMarkedGridAdd,
                     AknIconUtils::AvkonIconFileName(),
                     EMbmAvkonQgn_indi_marked_grid_add,
                     EMbmAvkonQgn_indi_marked_grid_add_mask);
    return icon;
}

// ---------------------------------------------------------------------------
// Helper to update all the scaled images.
// Assumes that iMaxImageSize contains the old maximum image size.
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateImageSizes(const TSize& aNewMaxSize)
{
    TInt count = iRefImages.Count();
    const MSwtImage* img = 0;

    CAknIconArray* iconArray = GetIconArray();

    for (TInt i = 0; i < count; i++)
    {
        img = iRefImages[i];
        if (img)
        {
            TSize imageSize = img->Bitmap().SizeInPixels();
            TSize oldSize, newSize;
            TBool doScaling = ETrue;

            if (imageSize.iHeight > iMaxImageSize.iHeight ||
                    imageSize.iWidth > iMaxImageSize.iWidth)
            {
                // If image size is bigger than the old maximum size, then
                // the image has been also previously scaled.
                oldSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                              imageSize, iMaxImageSize);
            }
            else if (imageSize.iHeight > aNewMaxSize.iHeight ||
                     imageSize.iWidth > aNewMaxSize.iWidth)
            {
                // Image is bigger than the new boundaries, but it did fit
                // inside the old boundaries, so the image is default size.
                oldSize = imageSize;
            }
            else
            {
                doScaling = EFalse;
            }

            if (doScaling)
            {
                newSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                              imageSize, aNewMaxSize);
                CFbsBitmap* bmp = const_cast<CFbsBitmap*>(&img->SubBitmap(newSize));
                CFbsBitmap* mask = const_cast<CFbsBitmap*>(img->SubMaskBitmap(newSize, ETrue));

                img->AddSubRef(newSize);

                // Remove the old reference
                img->RemoveSubRef(oldSize);

                CGulIcon* icon = (*iconArray)[ ItemIconIndex(i)];

                icon->SetBitmap(bmp);
                icon->SetMask(mask);
            }
        }
    }

}

// ---------------------------------------------------------------------------
// Removes reference to the image at a given index. If the image has been
// scaled, removes the correct sized reference.
// ---------------------------------------------------------------------------
//
void CSwtListView::RemoveImageRef(const TInt& aIndex)
{
    const MSwtImage* img = iRefImages[ aIndex ];

    if (img)
    {
        TSize imageSize = img->Bitmap().SizeInPixels();

        // If the original size of the image is bigger than the maximum image
        // size, it has been scaled to fit in iMaxImageSize. Remove reference
        // to the scaled bitmap.
        if (imageSize.iHeight > iMaxImageSize.iHeight ||
                imageSize.iWidth > iMaxImageSize.iWidth)
        {
            imageSize = SwtControlHelper::GetAspectRatioScaledBitmapSize(
                            imageSize, iMaxImageSize);
        }

        img->RemoveSubRef(imageSize);
        img->RemoveRef();
    }
}


// ---------------------------------------------------------------------------
// Leaving version of HandleResourceChange.
// ---------------------------------------------------------------------------
//
void CSwtListView::HandleResourceChangeL(TInt aType)
{
    if (aType == KAknsMessageSkinChange)
    {
        // Refresh the selected item mark icon
        CAknIconArray* iconArray = GetIconArray();
        CGulIcon* icon = (*iconArray)[ KMarkOnIconArrayIndexInt ];
        delete icon;
        (*iconArray)[ KMarkOnIconArrayIndexInt ] = GetMarkedItemIconL();
    }

    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        if (AknLayoutUtils::LayoutMirrored())
        {
            iLeftToRight = EFalse;
        }
        else
        {
            iLeftToRight = ETrue;
        }

        CCoeControl::HandleResourceChange(aType);
        SizeChangedL();
    }
    else if (aType == KEikMessageUnfadeWindows
             || aType == KEikMessageWindowsFadeChange)
    {
        CCoeControl::HandleResourceChange(aType);
    }
    else
    {
        CCoeControl::HandleResourceChange(aType);
        DoGridCellLayoutL(iDensity);
    }
}

// ---------------------------------------------------------------------------
// Leaving version of SizeChanged.
// ---------------------------------------------------------------------------
//
void CSwtListView::SizeChangedL()
{
    DoGridCellLayoutL(iDensity);
    TRect clientRect(BorderInnerRect());

    // WORKAROUND! This is to avoid CAknGrid crash when its size is 0
    // and the List Transition Effects try to draw the grid. In fact the
    // root cause of the crash is our Shell not being always ready to draw
    // which is what Avkon expects. If that would be the case, the List
    // Transition Effects will never get a chance to draw a size 0 grid.
    if (clientRect.Width() <= 0 && clientRect.Height() <= 0)
    {
        clientRect = TRect(0, 0, 1, 0);
    }

    iGrid->SetRect(clientRect);
    iGrid->HandleViewRectSizeChangeL();
}

// ---------------------------------------------------------------------------
// Helper to get the array index of the icon from the item index.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::ItemIconIndex(const TInt& aItemPos)
{
    return (aItemPos + 2);
}

// ---------------------------------------------------------------------------
// Updates the control menu.
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateControlMenu()
{
    if (iStyle & KSwtStyleMulti)
    {
        TInt focusIndex = GetFocusIndex();
        if (focusIndex != -1)
        {
            iMenuItemMarkUnmark->SetEnabled(ETrue);
            if (IsSelected(focusIndex))
            {
                if (iIsMarkDisplayed)
                {
                    iIsMarkDisplayed = EFalse;
                    TPtrC ptr = iUnmarkString->Des();
                    TRAP_IGNORE(iMenuItemMarkUnmark->SetTextL(ptr));
                }
            }
            else
            {
                if (!iIsMarkDisplayed)
                {
                    iIsMarkDisplayed = ETrue;
                    TPtrC ptr = iMarkString->Des();
                    TRAP_IGNORE(iMenuItemMarkUnmark->SetTextL(ptr));
                }
            }
        }
        else
        {
            iMenuItemMarkUnmark->SetEnabled(EFalse);
        }

        if (GetGridView()->SelectionIndexes()->Count() == 0)
        {
            iMenuItemUnmarkAll->SetEnabled(EFalse);
#ifdef RD_SCALABLE_UI_V2
            iStylusPopupUnmarkAll->SetEnabled(EFalse);
#endif //RD_SCALABLE_UI_V2
        }
        else
        {
            iMenuItemUnmarkAll->SetEnabled(ETrue);
#ifdef RD_SCALABLE_UI_V2
            iStylusPopupUnmarkAll->SetEnabled(ETrue);
#endif //RD_SCALABLE_UI_V2
        }

        if (GetGridView()->SelectionIndexes()->Count() == GetGridModel()->NumberOfItems())
        {
            iMenuItemMarkAll->SetEnabled(EFalse);
#ifdef RD_SCALABLE_UI_V2
            iStylusPopupMarkAll->SetEnabled(EFalse);
#endif //RD_SCALABLE_UI_V2
        }
        else
        {
            iMenuItemMarkAll->SetEnabled(ETrue);
#ifdef RD_SCALABLE_UI_V2
            iStylusPopupMarkAll->SetEnabled(ETrue);
#endif //RD_SCALABLE_UI_V2
        }
    }
}

// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//
CSwtListView* CSwtListView::NewL(MSwtDisplay& aDisplay,
                                 TSwtPeer aPeer,
                                 MSwtComposite& aParent,
                                 TInt aStyle)
{
    CSwtListView* self = CSwtListView::NewLC(aDisplay, aPeer, aParent, aStyle);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//
CSwtListView* CSwtListView::NewLC(MSwtDisplay& aDisplay,
                                  TSwtPeer aPeer,
                                  MSwtComposite& aParent,
                                  TInt aStyle)
{
    CSwtListView* self = new(ELeave) CSwtListView(aDisplay,
            aPeer,
            aParent,
            aStyle);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->InitControlBaseL();
    return self;
}

// ---------------------------------------------------------------------------
// The destructor
// ---------------------------------------------------------------------------
//
CSwtListView::~CSwtListView()
{
    if (iMenuItemMarkUnmark)
    {
        iMenuItemMarkUnmark->Dispose();
    }

    if (iMenuItemMarkAll)
    {
        iMenuItemMarkAll->Dispose();
    }

    if (iMenuItemUnmarkAll)
    {
        iMenuItemUnmarkAll->Dispose();
    }

    if (iSubMenu)
    {
        iSubMenu->Dispose();
    }

    if (iMenuItemEditList)
    {
        iMenuItemEditList->Dispose();
    }

    if (iMenu)
    {
        iMenu->Dispose();
    }

#ifdef RD_SCALABLE_UI_V2
    if (iStylusPopupMarkAll)
    {
        iStylusPopupMarkAll->Dispose();
    }

    if (iStylusPopupUnmarkAll)
    {
        iStylusPopupUnmarkAll->Dispose();
    }

    if (iStylusPopupMenu)
    {
        iStylusPopupMenu->Dispose();
    }
#endif //RD_SCALABLE_UI_V2

    delete iMarkString;
    delete iUnmarkString;

    delete iOldSelectionArray;
    iOldSelectionArray = NULL;

    delete iSelectionArray;
    iSelectionArray = NULL;

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iRefImages, *(GetIconArray()), iMaxImageSize);
#endif

    delete iGrid;
    iGrid = NULL;

    TInt count = iRefImages.Count();
    for (TInt i = 0; i < count; i++)
    {
        RemoveImageRef(i);
    }

    iRefImages.Close();
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtListView::ComponentControl(TInt /*aIndex*/) const
{
    return iGrid;
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::CountComponentControls() const
{
    return 1; // there's only the grid
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::SizeChanged()
{
    TRAP_IGNORE(SizeChangedL());
    HandleSizeChanged();
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::PositionChanged()
{
    iGrid->SetPosition(BorderInnerRect().iTl);
    TRAP_IGNORE(iGrid->HandleViewRectSizeChangeL());
    HandlePositionChanged();
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::HandleResourceChange(TInt aType)
{
    TRAP_IGNORE(HandleResourceChangeL(aType));
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::FocusChanged(TDrawNow aDrawNow)
{
    TBool isFocused = IsFocused();
#ifdef RD_JAVA_S60_RELEASE_9_2
    EnableFocusHighlight(isFocused);
#endif //RD_JAVA_S60_RELEASE_9_2
    iGrid->SetFocus(isFocused);
    HandleFocusChanged(aDrawNow);
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Only horizontal layout (vertical scrolling) is supported.
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtListView::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    ASSERT(iVerticalOrientation == EFalse);

    TBool traversalDoIt = ETrue;
    TInt firstItemRowIndex;
    TInt firstItemColIndex;
    TInt lastItemRowIndex;
    TInt lastItemColIndex;
    TInt currentItemRowIndex;
    TInt currentItemColIndex;
    TSwtTraversal detail = ESwtTraverseNone;
    TBool isMirrored = AknLayoutUtils::LayoutMirrored();

    if (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK ||
            aKeyEvent.iCode == EKeyEscape)
    {
        traversalDoIt = EFalse;
    }

    if (GetItemCount() > 0)
    {
        if (aKeyEvent.iCode == EKeyDownArrow ||
                aKeyEvent.iCode == EKeyUpArrow ||
                aKeyEvent.iCode == EKeyLeftArrow ||
                aKeyEvent.iCode == EKeyRightArrow)
        {
            traversalDoIt = EFalse;
            GetGridView()->CalcRowAndColIndexesFromItemIndex(0,
                    firstItemRowIndex,
                    firstItemColIndex);
            GetGridView()->CalcRowAndColIndexesFromItemIndex(GetItemCount() - 1,
                    lastItemRowIndex,
                    lastItemColIndex);
            GetGridView()->CalcRowAndColIndexesFromItemIndex(iGrid->CurrentDataIndex(),
                    currentItemRowIndex,
                    currentItemColIndex);
        }
        if (aKeyEvent.iCode == EKeyDownArrow)
        {
            if (currentItemRowIndex == lastItemRowIndex)
            {
                detail = ESwtTraverseArrowNext;
            }
        }
        else if (aKeyEvent.iCode == EKeyUpArrow)
        {
            if (currentItemRowIndex == firstItemRowIndex)
            {
                detail = ESwtTraverseArrowPrevious;
            }
        }
        else if ((isMirrored  && aKeyEvent.iCode == EKeyRightArrow) ||
                 (!isMirrored && aKeyEvent.iCode == EKeyLeftArrow))
        {
            if (iGrid->CurrentDataIndex() == 0)
            {
                detail = ESwtTraverseArrowPrevious;
            }
        }
        else if ((isMirrored  && aKeyEvent.iCode == EKeyLeftArrow) ||
                 (!isMirrored && aKeyEvent.iCode == EKeyRightArrow))
        {
            if (iGrid->CurrentDataIndex() == GetItemCount() - 1)
            {
                detail = ESwtTraverseArrowNext;
            }
        }
    }
    if (detail != ESwtTraverseNone &&
            (GetShell().FindTraversalTargetL(detail, *this)))
    {
        traversalDoIt = ETrue;
    }
    return HandleKeyL(aKeyEvent, aType, traversalDoIt);
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::MakeVisible(TBool aVisible)
{
    CCoeControl::MakeVisible(aVisible);
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::SetDimmed(TBool aDimmed)
{
    CCoeControl::SetDimmed(aDimmed);
    if (iGrid)
    {
        iGrid->SetDimmed(aDimmed);
    }
    FocusabilityChanged();
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CSwtListView::MopSupplyObject(TTypeUid aId)
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

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Overriding Draw just to set correct clipping rect for the item drawer of
// the contained grid. The contained grid will be drawn immediately after this.
// ---------------------------------------------------------------------------
//
void CSwtListView::Draw(const TRect& /*aRect*/) const
{
    if (GetShell().UrgentPaintControl() == this)
        return;

    TRect clipRect(ClipToVisibleRect(iGrid->View()->ViewRect()));
    if (clipRect != iLastViewVisibleRect)
    {
        iGrid->SetItemDrawerClippingRect(clipRect);
        iLastViewVisibleRect = clipRect;
    }
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
MSwtScrollable* CSwtListView::Scrollable()
{
    return this;
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// This is called when the Java application calls ListView.add without
// the index parameter
// ---------------------------------------------------------------------------
//
void CSwtListView::AppendL(const TDesC& aPtr, const MSwtImage* aImage)
{
    AddItemL(aPtr, aImage, GetItemCount());
    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// This is called when the Java application calls ListView.add with
// the index parameter.
// ---------------------------------------------------------------------------
//
void CSwtListView::InsertL(TInt aPos,
                           const TDesC& aPtr,
                           const MSwtImage* aImage)
{
    if (aPos < 0 || aPos > GetItemCount())
    {
        User::Leave(ESwtErrorInvalidRange);
    }
    AddItemL(aPtr, aImage, aPos);
    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::DeselectItem(TInt aIndex)
{
    // Ignore if not applicable
    TInt count(GetItemCount());
    if (!(iStyle & KSwtStyleMulti) ||               // not multi select
            count == 0 ||                               // empty
            aIndex >= count ||                          // no items in range
            aIndex < 0 ||                               // no items in range
            iGrid->SelectionIndexes()->Count() == 0)    // no selected items
    {
        return;
    }

    CAknGridView* gridView = GetGridView();

    ASSERT(gridView);

    gridView->DeselectItem(gridView->ActualDataIndex(aIndex));
    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::DeselectRange(TInt aStart, TInt aEnd)
{
    ASSERT(aStart <= aEnd);   // Checked in the Java method

    // Ignore if not applicable
    TInt count(GetItemCount());
    if (!(iStyle & KSwtStyleMulti) ||               // not multi select
            count == 0 ||                               // empty
            aStart >= count ||                          // no items in range
            iGrid->SelectionIndexes()->Count() == 0)    // no selected items
    {
        return;
    }

    // Validate range
    if (aStart < 0)
    {
        aStart = 0;
    }
    if (aEnd >= count)
    {
        aEnd = count - 1;
    }

    TRAP_IGNORE(GetGridView()->DeselectRangeL(aStart, aEnd));
    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::DeselectItems(const TInt* aIndices, TInt aCount)
{
    // Ignore if not applicable
    TInt count(GetItemCount());
    if (!aIndices ||
            aCount == 0 ||
            !(iStyle & KSwtStyleMulti) ||               // not multi select
            count == 0 ||                               // empty
            iGrid->SelectionIndexes()->Count() == 0)    // no selected items
    {
        return;
    }

    for (TInt i = 0; i < aCount; ++i)
    {
        if (GetGridModel()->IndexContainsData(aIndices[i]))
        {
            GetGridView()->DeselectItem(aIndices[i]);
        }
    }
    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::DeselectAll()
{
    // Ignore if not applicable
    TInt count(GetItemCount());
    if (!(iStyle & KSwtStyleMulti) ||               // not multi select
            count == 0 ||                               // empty
            iGrid->SelectionIndexes()->Count() == 0)    // no selected items
    {
        return;
    }
    iGrid->ClearSelection();
    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::GetFocusIndex() const
{
    if (GetGridModel()->NumberOfItems() == 0)
    {
        return -1;
    }
    return iGrid->CurrentDataIndex();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::GetItemL(TInt aItemIndex, TPtrC& aString) const
{
    if (GetGridModel()->IndexContainsData(aItemIndex))
    {
        GetCaption(aItemIndex, aString);
    }
    else
    {
        User::Leave(ESwtErrorInvalidRange);
    }
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
TPoint CSwtListView::GetIconSize(TInt aStyle) const
{
    ASSERT(aStyle == ELowListViewDensity ||
           aStyle == EHighListViewDensity ||
           aStyle == EMediumListViewDensity);   // checked in Java side

    if (aStyle == ELowListViewDensity)
    {
        return iIconSizeInLowDensity;
    }
    else
    {
        if (aStyle == EHighListViewDensity)
        {
            return iIconSizeInHighDensity;
        }
        else
        {
            return iIconSizeInMediumDensity;
        }
    }
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::GetItemCount() const
{
    return GetGridModel()->NumberOfItems();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
CPtrCArray* CSwtListView::GetItemsL() const
{
    TInt count(GetItemCount());
    if (count == 0)
    {
        return NULL;
    }

    // Allocate space for the items, deleted by the jni export function
    CPtrCArray* itemsArray = new(ELeave) CPtrC16Array(KSwtListViewItemGranularity);
    CleanupStack::PushL(itemsArray);

    for (TInt i = 0; i < count; ++i)
    {
        TPtrC caption;
        GetCaption(i, caption);
        itemsArray->AppendL(caption);
    }

    CleanupStack::Pop();
    return itemsArray;
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
CPtrCArray* CSwtListView::GetSelectionL() const
{
    if (!(GetItemCount() > 0))
    {
        return NULL;
    }

    const CAknGridView::CSelectionIndexArray* selArray = NULL;
    if (iStyle & KSwtStyleMulti)
    {
        selArray = iGrid->SelectionIndexes();
        if (!selArray || selArray->Count() == 0)
        {
            return NULL;
        }
    }

    // Allocate space for the items, deleted by the jni export function
    CPtrCArray* itemsArray = new(ELeave) CPtrC16Array(KSwtListViewItemGranularity);
    CleanupStack::PushL(itemsArray);

    if (iStyle & KSwtStyleMulti)
    {
        for (TInt i = 0; i < selArray->Count(); ++i)
        {
            ASSERT(selArray->At(i) < GetItemCount());
            TPtrC caption;
            GetItemL(GetGridView()->ActualDataIndex(selArray->At(i)), caption);
            itemsArray->AppendL(caption);
        }
    }
    else // single style
    {
        TPtrC caption;
        GetItemL(iGrid->CurrentDataIndex(), caption);
        itemsArray->AppendL(caption);
    }

    CleanupStack::Pop();
    return itemsArray;
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::GetSelectionCount() const
{
    return iGrid->SelectionIndexes()->Count();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
const CArrayFix<TInt>* CSwtListView::GetSelectionIndices() const
{
    GetSelection(iSelectionArray);

    return iSelectionArray;
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::GetTopIndex() const
{
    if (GetItemCount() <= 0)
    {
        return -1;
    }
    else
    {
        return iGrid->TopItemIndex();
    }
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::IndexOf(const TDesC& aString, TInt& aStart) const
{
    TInt index = -1;
    CDesCArray* itemTextArray = GetTextArray();

    // Range check
    if (!(aStart < 0 || aStart >= GetItemCount()))
    {
        for (TInt i = aStart; i < itemTextArray->Count(); ++i)
        {
            TPtrC caption;
            TInt result = GetCaption(i, caption);
            if (result == KErrNotFound)
            {
                break;
            }
            if (aString.Compare(caption) == KErrNone)
            {
                index = i;
                break;
            }
        }
    }
    return index;
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
TBool CSwtListView::IsSelected(TInt aIndex) const
{
    return GetGridView()->ItemIsSelected(GetGridView()->ActualDataIndex(aIndex));
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::RemoveItemL(TInt aIndex)
{
    if (aIndex < 0 || aIndex >= GetItemCount())
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    // Store the selection state
    GetSelection(iOldSelectionArray);

    // Check if the focused and/or the last item is being removed
    TBool focused = EFalse;
    if (aIndex == iGrid->CurrentDataIndex())
    {
        focused = ETrue;
    }
    TBool last = EFalse;
    if (aIndex == GetItemCount() - 1)
    {
        last = ETrue;
    }

    UpdateRemovedItemCountersL(aIndex);

    // Remove the item text
    CDesCArray* itemTextArray = GetTextArray();
    itemTextArray->Delete(aIndex);
    itemTextArray->Compress();

    // All the icon indexes after the removed item need updating
    TInt textCount = GetTextArray()->Count();
    for (TInt i = aIndex; i < textCount; ++i)
    {
        UpdateIconIndexL(i);
    }

    // Remove the icon
    CAknIconArray* iconArray = GetIconArray();
    CGulIcon* icon = (*iconArray)[ ItemIconIndex(aIndex)];
    iconArray->Delete(ItemIconIndex(aIndex));
    iconArray->Compress();
    delete icon;

    // Remove the reference
    RemoveImageRef(aIndex);
    iRefImages.Remove(aIndex);
    iRefImages.Compress();

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iRefImages, *(GetIconArray()), iMaxImageSize);
#endif

    // Notify the grid
    iGrid->HandleItemRemovalL();

    // Restore the selection state updating the indexes.
    UpdateSelectionIndexesL(aIndex, aIndex, EFalse);

    // If last and focused item was removed then focus moves to the first
    if (GetItemCount() > 0)
    {
        if (focused && last)
        {
            iGrid->SetCurrentDataIndex(0);
            iGrid->ScrollToMakeItemVisible(0);
        }

        SingleSelectL(GetSelectionL(iGrid->CurrentDataIndex()), ETrue);
        ScrollViewToShowAsManyItemsAsPossibleL();
    }
    UpdateControlMenu();
    UpdateMSKLabelL();
    Redraw();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::RemoveRangeL(TInt aStart, TInt aEnd)
{
    ASSERT(aStart <= aEnd);   // Checked in the Java method

    if (aStart < 0 || aEnd >= GetItemCount())
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    // Store the selection state
    GetSelection(iOldSelectionArray);

    // Remove texts
    CDesCArray* itemTextArray = GetTextArray();

    for (TInt i = aStart; i < aEnd; ++i)
    {
        UpdateRemovedItemCountersL(i);
    }

    TInt itemsInRange = (aEnd - aStart) + 1;
    itemTextArray->Delete(aStart, itemsInRange);
    itemTextArray->Compress();

    // All the icon indexes after the 1st removed item need updating
    TInt textCount = GetTextArray()->Count();
    for (TInt i = aStart; i < textCount; ++i)
    {
        UpdateIconIndexL(i);
    }

    // Delete icons and references
    CAknIconArray* iconArray = GetIconArray();

    // Must go backwards here for correct removal of references
    for (TInt i = aEnd; i >= aStart; i--)
    {
        CGulIcon* icon = (*iconArray)[ItemIconIndex(i)];
        delete icon;
        RemoveImageRef(i);
        iRefImages.Remove(i);
    }
    iconArray->Delete(ItemIconIndex(aStart), itemsInRange);
    iconArray->Compress();
    iRefImages.Compress();

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iRefImages, *(GetIconArray()), iMaxImageSize);
#endif

    // Notify grid
    iGrid->HandleItemRemovalL();

    // Restore the selection state updating the indexes.
    UpdateSelectionIndexesL(aStart, aEnd, EFalse);

    if (GetItemCount() > 0)
    {
        SingleSelectL(GetSelectionL(iGrid->CurrentDataIndex()), ETrue);
        ScrollViewToShowAsManyItemsAsPossibleL();
    }

    UpdateControlMenu();
    UpdateMSKLabelL();
    Redraw();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::RemoveItemsL(const TInt* aIndices, TInt aCount)
{
    if (!aIndices)
    {
        return;
    }

    // Store the selection state
    GetSelection(iOldSelectionArray);

    TInt count = GetItemCount();

    // Elements are ordered from high to low in Java side.
    TInt min = aIndices[ aCount - 1 ];
    TInt max = aIndices[ 0 ];
    if (min < 0 || max >= count)
    {
        User::Leave(ESwtErrorInvalidRange);
    }

    CDesCArray* textArray = GetTextArray();
    CAknIconArray* iconArray = GetIconArray();
    TInt prevIndex = -1;

    for (TInt i = 0; i < aCount; ++i)
    {
        // Assert that items are in high-to-low order
        ASSERT(i + 1 < aCount ? aIndices[ i + 1 ] <= aIndices[ i ] : ETrue);

        if (aIndices[i] != prevIndex)  // not the same index many times
        {
            UpdateRemovedItemCountersL(aIndices[i]);
            textArray->Delete(aIndices[ i ]);
            CGulIcon* icon = (*iconArray)[ ItemIconIndex(aIndices[ i ])];
            iconArray->Delete(ItemIconIndex(aIndices[ i ]));
            delete icon;
            const MSwtImage* refImg = iRefImages[aIndices[ i ]];
            RemoveImageRef(aIndices[ i ]);
            iRefImages.Remove(aIndices[ i ]);
            prevIndex = aIndices[ i ];
        }
    }

    textArray->Compress();
    iconArray->Compress();
    iRefImages.Compress();

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iRefImages, *(GetIconArray()), iMaxImageSize);
#endif

    // All the icon indexes starting from the first removed item
    // need updating
    TInt textCount = GetTextArray()->Count();
    for (TInt i = min; i < textCount; ++i)
    {
        UpdateIconIndexL(i);
    }

    // Notify the grid
    iGrid->HandleItemRemovalL();

    // Restore the selection state updating the indexes.
    prevIndex = -1;
    for (TInt i = 0; i < aCount; ++i)
    {
        // Assert that items are in high-to-low order
        ASSERT(i + 1 < aCount ? aIndices[ i + 1 ] <= aIndices[ i ] : ETrue);

        if (aIndices[i] != prevIndex)  // not the same index many times
        {
            // Restore the selection state updating the indexes.
            UpdateSelectionIndexesL(aIndices[i], aIndices[i], EFalse);

            GetSelection(iOldSelectionArray);   //was reseted by former method

            prevIndex = aIndices[ i ];
        }
    }

    iOldSelectionArray->Reset();

    if (GetItemCount() > 0)
    {
        SingleSelectL(GetSelectionL(iGrid->CurrentDataIndex()), ETrue);
        ScrollViewToShowAsManyItemsAsPossibleL();
    }

    UpdateControlMenu();
    UpdateMSKLabelL();
    Redraw();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::RemoveAllL()
{
    TInt itemCount = GetItemCount();
    if (itemCount > 0)
    {
        RemoveRangeL(0, itemCount - 1);
    }
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::SelectItemL(TInt aIndex, TBool aScroll)
{
    // Checked on Java side
    ASSERT(aIndex >= 0);

    // Range check
    if (aIndex >= GetItemCount())
    {
        return;
    }

    SingleOrMultiSelectL(aIndex, aScroll);

    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::SelectRangeL(TInt aStart, TInt aEnd)
{
    // Checked on Java side
    ASSERT(iStyle & KSwtStyleMulti || aStart != aEnd);
    ASSERT(aEnd >= 0);
    ASSERT(aStart <= aEnd);

    // Ignore if not applicable
    TInt count(GetItemCount());
    if (count == 0 ||           // empty
            aStart >= count)        // no items in range
    {
        return;
    }

    // Ignore items out of range
    if (aEnd >= count)
    {
        aEnd = count - 1;
    }

    for (TInt i = aStart; i <= aEnd; ++i)
    {
        SingleOrMultiSelectL(i, EFalse);
    }

    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::SelectAllL()
{
    // Checked on Java side
    ASSERT(iStyle & KSwtStyleMulti);

    // Ignore if not applicable
    TInt count(GetItemCount());
    if (count == 0)
    {
        return;
    }
    SelectRangeL(0, count - 1);
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::SetFocusIndex(TInt aIndex)
{
    ASSERT(iGrid);

    // By now, we only call this method in multiple selection.
    // In single selection, we assume that setting the focus means selecting.
    ASSERT(iStyle & KSwtStyleMulti);

    if (aIndex < 0 || aIndex >= iGrid->Model()->NumberOfItems())
    {
        return;
    }

    TInt old = iGrid->CurrentItemIndex();
    if (old != aIndex)
    {
        iGrid->SetCurrentItemIndex(aIndex);
        iGrid->View()->DrawItem(aIndex);
        if (old != -1)
        {
            iGrid->View()->DrawItem(old);
        }
    }
    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::SetItemsL(MDesCArray* aStringArray, RPointerArray<MSwtImage>& aImagesArray)
{
    // Null argument and invalid range are covered in Java side.
    // If there are no icons (client passed null) then we have an empty array here.
    ASSERT(aStringArray);
    TInt texts = aStringArray->MdcaCount();
    TInt icons = aImagesArray.Count();
    ASSERT(texts == icons || icons == 0);

    // Remove the existing items
    RemoveAllL();

    // Add the new items
    for (TInt i = 0; i < texts; ++i)
    {
        // Add the text
        TRAPD(textError, AddTextL(aStringArray->MdcaPoint(i), i));
        if (textError)
        {
            User::Leave(ESwtErrorItemNotAdded);
        }

        // Add the icon
        TInt iconError;
        if (icons)
        {
            TRAP(iconError, AddIconL(aImagesArray[ i ], i));
        }
        else
        {
            TRAP(iconError, AddIconL(NULL, i));
        }
        if (iconError)
        {
            // Remove the text as the icon can't be added
            ASSERT(GetTextArray()->Count() > 0);
            GetTextArray()->Delete(GetTextArray()->Count() - 1);
            User::Leave(ESwtErrorItemNotAdded);
        }

        // Update grid cell layout if neccessary
        DoGridCellLayoutIfNeededL();
    }

    // All items added successfully

    iGrid->HandleItemAdditionL();

    // Focus must move to the newly added item.
    iGrid->SetCurrentDataIndex(texts - 1);
    SingleSelectL(texts - 1, ETrue);

    UpdateControlMenu();
    UpdateMSKLabelL();

    Redraw();
    iDisplay.PostSelectionEventL(iPeer);
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// Only the horizontal orientation (vertical scrolling)
// is supported currently. No need to do anything here.
// ---------------------------------------------------------------------------
//
void CSwtListView::SetLayoutL(TInt /*aLayout*/)
{
    //TBool currentOrientation = iVerticalOrientation;
    //if( aLayout == KSwtStyleVertical)
    //    iVerticalOrientation = ETrue;
    //if( aLayout == KSwtStyleHorizontal )
    //    iVerticalOrientation = EFalse;
    //if( iVerticalOrientation != currentOrientation )
    //    DoGridCellLayoutL( iDensity );
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::SetLayoutDensityL(TInt aDensity)
{
    if (aDensity == iDensity)
    {
        return;
    }

    switch (aDensity)
    {
    case 1:
    {
        iDensity = ELowListViewDensity;
        break;
    }
    case 2:
    {
        iDensity = EMediumListViewDensity;
        break;
    }
    case 3:
    {
        iDensity = EHighListViewDensity;
        break;
    }
    default:
    {
        break;
    }
    }
    DoGridCellLayoutL(iDensity);
    ScrollViewToShowAsManyItemsAsPossibleL();
    Redraw();
    UpdateControlMenu();
}

// ---------------------------------------------------------------------------
// From class MSwtListView.
// ---------------------------------------------------------------------------
//
void CSwtListView::ShowSelection()
{
    const CAknGridView::CSelectionIndexArray& indexArray = *(iGrid->SelectionIndexes());
    if (indexArray.Count() > 0)
    {
        // Make the first selected item visible in case there are many
        iGrid->ScrollToMakeItemVisible(indexArray[ 0 ]);
    }

    UpdateControlMenu();
    Redraw();
}

// ---------------------------------------------------------------------------
// From class ASwtScrollableBase
// ---------------------------------------------------------------------------
//
CEikScrollBarFrame* CSwtListView::SbFrame() const
{
    return (iGrid ? iGrid->ScrollBarFrame() : NULL);
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
CCoeControl& CSwtListView::CoeControl()
{
    return *this;
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
const CCoeControl& CSwtListView::CoeControl() const
{
    return *this;
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aType == EEventKeyDown)
    {
        // After panning focus highlight was disabled, so enabling again
        EnableFocusHighlight(ETrue);
    }
#endif //RD_JAVA_S60_RELEASE_9_2

    if (GetItemCount() == 0)
    {
        iGrid->OfferKeyEventL(aKeyEvent, aType);
        return;
    }

    if (!(iStyle & KSwtStyleMulti))
    {
        if (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
        {
            // Send default selection event.
            iDisplay.PostDefaultSelectionEventL(iPeer);
        }
        else
        {
            iPrevFocusIndex = GetSelectionL(iGrid->CurrentDataIndex());

            iGrid->OfferKeyEventL(aKeyEvent, aType);

            // Post an event if the selected item has changed
            if (iPrevFocusIndex != GetSelectionL(iGrid->CurrentDataIndex()))
            {
                SingleSelectL(GetSelectionL(iGrid->CurrentDataIndex()), ETrue);
                // its workaround to fix the problem in ListView items name changing when
                //they get focus in mirrored layout. it should be fixed in avkon
                // CListBoxView::VerticalMoveToItemL. Remove below Redraw() after fixing.
                if (!iLeftToRight)
                {
                    Redraw();
                }
                iDisplay.PostSelectionEventL(iPeer);
            }
        }
    }
    else  // Multi-style
    {
        GetSelection(iOldSelectionArray);

        iGrid->OfferKeyEventL(aKeyEvent, aType);

        if (aType == EEventKeyUp)
        {
            UpdateControlMenu();
        }

        // Post an event if selection has changed.
        if (iOldSelectionArray->Count() != iGrid->SelectionIndexes()->Count())
        {
            iDisplay.PostSelectionEventL(iPeer);
        }
        else
        {
            for (TInt i = 0; i < iOldSelectionArray->Count(); ++i)
            {
                if (!IsSelected(iOldSelectionArray->At(i)))
                {
                    iDisplay.PostSelectionEventL(iPeer);
                    return;
                }
            }
        }
    }
    iOldSelectionArray->Reset();
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// Returns the color set by the user if it exists.
// ---------------------------------------------------------------------------
//
TRgb CSwtListView::GetBackground() const
{
    if (iBgColor)
    {
        return TRgb(iBgColor->RgbValue());
    }
    else
    {
        return ASwtControlBase::GetBackground();
    }
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// The normal text color is returned.
// Highlighted (focused) text might have a different color.
// ---------------------------------------------------------------------------
//
TRgb CSwtListView::GetForeground() const
{
    return iGrid->ItemDrawer()->FormattedCellData()->SubCellColors(KTextColumn).iText;
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// Background color is not currently used for anything besides being returned
// from GetBackGround.
// ---------------------------------------------------------------------------
//
void CSwtListView::SetBackgroundL(const MSwtColor* aColor)
{
    iBgColor = aColor;
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::SetForegroundL(const MSwtColor* aColor)
{
    iTextColor = aColor;
    UpdateTextColorL();
    Redraw();
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// The preferred size is the top-level-shell's client area size.
// If hints are given then the preferred size is the closest full column
// and/or row. But not larger than the top-level-shell client area and not
// smaller than 1x1. Parent size or contents don't affect the preferred
// size.
// ---------------------------------------------------------------------------
//
TSize CSwtListView::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    TSize preferredSize(aWHint, aHHint);

    if (aWHint != KSwtDefault && aHHint != KSwtDefault)
    {
        return preferredSize;
    }

    TSize cellSize = GetCellSize();
    TUint itemCount = GetItemCount();
    TUint columnCount = 0;
    TUint rowCount = 0;

    if (aHHint != KSwtDefault)
    {
        // Height is set, calculate needed number of columns.
        if (cellSize.iHeight != 0)
        {
            rowCount = aHHint / cellSize.iHeight;
        }

        columnCount = GetRowColumnCount(itemCount, rowCount);

        preferredSize.iWidth = columnCount * cellSize.iWidth;
        preferredSize.iWidth += GetBorderSize(KSwtStyleHorizontal);
    }
    else if (aWHint != KSwtDefault)
    {
        // Width is set, calculate needed number of rows.
        if (cellSize.iWidth != 0)
        {
            columnCount = aWHint / cellSize.iWidth;
        }

        rowCount = GetRowColumnCount(itemCount, columnCount);

        preferredSize.iHeight = rowCount * cellSize.iHeight;
        preferredSize.iHeight += GetBorderSize(KSwtStyleVertical);
    }
    else
    {
        // Width and height are not set
        columnCount = ISqrt(itemCount);

        rowCount = GetRowColumnCount(itemCount, columnCount);

        preferredSize.iWidth = columnCount * cellSize.iWidth;
        preferredSize.iWidth += GetBorderSize(KSwtStyleHorizontal);
        preferredSize.iHeight = rowCount * cellSize.iHeight;
        preferredSize.iHeight += GetBorderSize(KSwtStyleVertical);
    }

    // Workaround for case where listview in a layout has no borders and no items.
    // In such a case preferred size is 0*0 pixels, which causes problems when
    // laying out the layout.
    if (preferredSize.iWidth <= 0 && preferredSize.iHeight <= 0)
    {
        preferredSize.iHeight = 1;
        preferredSize.iWidth = 1;
    }

    return preferredSize;
}

// ---------------------------------------------------------------------------
// Returns number of rows/columns needed on a list with aItemCount items and
// aRowColumnCount rows/columns.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::GetRowColumnCount(const TInt& aItemCount, const TInt& aRowColumnCount) const
{
    TInt rowsColumns = 0;

    if (aRowColumnCount > 0)
    {
        rowsColumns = (aItemCount / aRowColumnCount);
        if ((aItemCount % aRowColumnCount) * GetCellSize().iWidth > KRowCalculationPrecisionInPixels)
        {
            // itemCount/rowColumnCount division returns the number of
            // full columns, so an additional one may be needed.
            rowsColumns++;
        }
    }

    return rowsColumns;
}


void CSwtListView::UpdateMSKLabelL()
{
    if (AknLayoutUtils::MSKEnabled() && !HasDoNotDrawFlag())
    {
        MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
        if (commandArranger)
        {
            commandArranger->UpdateMSKLabelL();
        }
    }
}

// ---------------------------------------------------------------------------
// Returns horizontal or vertical border size for a list cell.
// ---------------------------------------------------------------------------
//
TInt CSwtListView::GetBorderSize(const TInt& aAlignment) const
{
    ASSERT(aAlignment == KSwtStyleHorizontal || aAlignment == KSwtStyleVertical);

    TInt borderSize = GetBorderWidth() + GetBorderWidth();

    if (aAlignment == KSwtStyleHorizontal)
    {
        // Horizontal borders, i.e. vertical scrollbar

        // Vertical scrollbar exists always
        //if( SbFrame()->ScrollBarExists( CEikScrollBar::EVertical ) &&
        //    SbFrame()->ScrollBarVisibility( CEikScrollBar::EVertical) != CEikScrollBarFrame::EOff )
        //    {
        borderSize += iDisplay.UiUtils().ScrollBarBreadth(SbFrame()->VerticalScrollBar());
        //    }
    }
    else
    {
        if (SbFrame()->ScrollBarExists(CEikScrollBar::EHorizontal) &&
                SbFrame()->ScrollBarVisibility(CEikScrollBar::EHorizontal) != CEikScrollBarFrame::EOff)
        {
            borderSize += iDisplay.UiUtils().ScrollBarBreadth(
                              SbFrame()->GetScrollBarHandle(CEikScrollBar::EHorizontal));
        }
    }

    return borderSize;
}

// ---------------------------------------------------------------------------
// From MSwtControl.
// ---------------------------------------------------------------------------
//
TBool CSwtListView::IsKeyUsed(TUint aKeyCode) const
{
    // Does not use the Backspace but uses OK key
    if (aKeyCode == EKeyBackspace)
    {
        return EFalse;
    }
    else if (aKeyCode == EKeyOK || aKeyCode == EKeyEnter)
    {
        MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
        if (commandArranger)
        {
            if (iStyle & KSwtStyleSingle &&
                    commandArranger->IsContextSensitiveOperationSet())
            {
                return EFalse;
            }
        }
        return ETrue;
    }
    return ETrue;
}

TBool CSwtListView::MSKSelCmdEnabled() const
{
    return ETrue;
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// Return the control menu to the menu arranger
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtListView::GetControlMenu() const
{
    return iMenu;
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateDoNotDrawFlag()
{
    ASwtControlBase::UpdateDoNotDrawFlag();

    // Update redrawing state of the compound control also.
    if (iGrid)
    {
        GetGridView()->SetDisableRedraw(HasDoNotDrawFlag());
    }
}

// ---------------------------------------------------------------------------
// From class MSwtControl.
// ---------------------------------------------------------------------------
//
HBufC* CSwtListView::MSKLabelL() const
{
    if (!(GetItemCount() > 0))
    {
        return ASwtControlBase::MSKLabelL();
    }

    if ((iStyle & KSwtStyleSingle))
    {
        HBufC* label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
        return label;
    }
    return ASwtControlBase::MSKLabelL();
}

// ---------------------------------------------------------------------------
// From class MEikCommandObserver
// ---------------------------------------------------------------------------
void CSwtListView::ProcessCommandL(TInt aCmdId)
{
    TInt focusIndex = GetFocusIndex();
    switch (aCmdId)
    {
    case EAknCmdMark:
        if (focusIndex != -1)
        {
            if (IsSelected(focusIndex))
            {
                DeselectItem(focusIndex);
            }
            else
            {
                SelectItemL(focusIndex, true);
            }
            iDisplay.PostSelectionEventL(iPeer);
        }
        break;
    case EAknMarkAll:
        SelectAllL();
        iDisplay.PostSelectionEventL(iPeer);
        break;
    case EAknUnmarkAll:
        DeselectAll();
        iDisplay.PostSelectionEventL(iPeer);
        break;
    default:
        ASSERT(EFalse);
        break;
    }
}

// ---------------------------------------------------------------------------
// CSwtListView::HandleListBoxEventL
// From MEikListBoxObserver
// Handles default selection for touch. Unable to handle the selection events
// here since EEventItemClicked is not always sent for every tap.
// The selection events are handled in HandlePointerEvent.
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSwtListView::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
{
    if (aListBox != iGrid)
    {
        return;
    }

    UpdateFlickScrollingState(aEventType);

    switch (aEventType)
    {
        // On 5.0, drawing trough Java gives simply a better fps.
#ifdef RD_JAVA_S60_RELEASE_9_2
    case EEventPanningStarted:
        GetShell().SetUrgentPaintControl(this);
        break;
    case EEventFlickStopped:
        GetShell().SetUrgentPaintControl(NULL);
        break;
#endif // RD_JAVA_S60_RELEASE_9_2
#ifdef RD_JAVA_S60_RELEASE_9_2
    case EEventItemSingleClicked:
#else
    case EEventItemDoubleClicked:
#endif // RD_JAVA_S60_RELEASE_9_2
        if (!(iStyle & KSwtStyleMulti))
        {
            if (!iDisplay.RevertPointerEvent())
            {
                // Ensure the selection and focus index are in sync.
                SingleSelectL(GetSelectionL(iGrid->CurrentDataIndex()), ETrue);

                iDisplay.PostDefaultSelectionEventL(iPeer);
            }
        }
        break;
    default:
        // Do nothing
        break;
    }
}
#else //RD_SCALABLE_UI_V2
void CSwtListView::HandleListBoxEventL(CEikListBox*, TListBoxEvent)
{
}
#endif //RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
// A fairly fast integer square root algorithm
// ---------------------------------------------------------------------------
TUint CSwtListView::ISqrt(TUint x)
{
    TUint r, nr, m;

    r = 0;
    m = 0x40000000;

    do
    {
        nr = r + m;
        if (nr <= x)
        {
            x -= nr;
            r = nr + m;
        }
        r >>= 1;
        m >>= 2;
    }
    while (m != 0);

    if (x > r)
    {
        r++;
    }

    return r;
}

// ---------------------------------------------------------------------------
// Update text and icon counters for a given item.
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateRemovedItemCountersL(const TInt& aIndex)
{
    ASSERT(aIndex >= 0 && aIndex < GetItemCount());

    TInt textChange = 0;
    TInt iconChange = 0;
    TPtrC caption;
    CGulIcon* icon = (*GetIconArray())[ ItemIconIndex(aIndex)];

    // Does the item have a caption
    GetCaption(aIndex, caption);
    if (caption.Length() != 0)
    {
        textChange = -1;
    }

    // Does the item have an icon
    TSize iconSize = icon->Bitmap()->SizeInPixels();
    if (iconSize.iHeight != 0 && iconSize.iWidth != 0)
    {
        // Icon size is not (0,0) conclude that icon was set.
        iconChange = -1;
    }

    if (textChange || iconChange)
    {
        UpdateSetItemCount(textChange, iconChange);
        DoGridCellLayoutIfNeededL();
    }
}

// ---------------------------------------------------------------------------
// Update counters for items that have icon/text, and set update cell layout
// status.
// ---------------------------------------------------------------------------
void CSwtListView::UpdateSetItemCount(const TInt& aTextChange, const TInt& aIconChange)
{
    TBool layoutNeeded = EFalse;

    // An item that has a text has changed (added or removed)
    if (aTextChange)
    {
        iNumberOfTexts += aTextChange;

        if (iNumberOfTexts < 0)
        {
            // Just in case..
            iNumberOfTexts = 0;
        }

        // If we are going from 0->n or from n->0, layout is needed.
        if ((aTextChange < 0 && iNumberOfTexts == 0) ||
                (aTextChange > 0 && iNumberOfTexts == 1))
        {
            layoutNeeded = ETrue;
        }
    }

    // An item that has an icon has changed (added or removed)
    if (aIconChange)
    {
        iNumberOfIcons += aIconChange;

        if (iNumberOfIcons < 0)
        {
            iNumberOfIcons = 0;
        }

        if ((aIconChange < 0 && iNumberOfIcons == 0) ||
                (aIconChange > 0 && iNumberOfIcons == 1))
        {
            layoutNeeded = ETrue;
        }
    }

    // Layout change is needed if we have e.g. gone from a state where there
    // are no items that have an icon on the list (i.e. texts are centered on
    // list items) to a state where an item has an icon (and texts need to be
    // laid out to the bottom of the items leaving space for the icon).
    iIsGridCellLayoutNeeded = layoutNeeded;
}


// ---------------------------------------------------------------------------
// Update cell layout if needed
// ---------------------------------------------------------------------------
//
void CSwtListView::DoGridCellLayoutIfNeededL()
{
    if (iIsGridCellLayoutNeeded)
    {
        DoGridCellLayoutL(iDensity);
        iIsGridCellLayoutNeeded = EFalse;
    }
}

// ---------------------------------------------------------------------------
// Gets cell and subcell LAF variants for current orientation and density
// ---------------------------------------------------------------------------
//
void CSwtListView::GetCellVariants(const TSwtListViewDensity& aDensity,
                                   TInt& aCellVariant, TInt& aSubcellVariantG1, TInt& aSubcellVariantG2,
                                   TInt& aSubcellVariantT) const
{
    if (CSwtLafFacade::IsLandscapeOrientation())
    {
        // Landscape orientation
#ifdef RD_JAVA_S60_RELEASE_9_2
        switch (aDensity)
        {
        case EHighListViewDensity:
            // 5x4
            aCellVariant = 6;
            aSubcellVariantG1 = 6;
            aSubcellVariantG2 = 6;
            aSubcellVariantT = 5;
            break;
        case ELowListViewDensity:
            // 3x2
            aCellVariant = 2;
            aSubcellVariantG1 = 1;
            aSubcellVariantG2 = 1;
            aSubcellVariantT = 1;
            break;
        default:
            // 6x2
            aCellVariant = 3;
            aSubcellVariantG1 = 3;
            aSubcellVariantG2 = 1;
            aSubcellVariantT = 3;
            break;
        }
#else
        switch (aDensity)
        {
        case EHighListViewDensity:
            // 5x4
            aCellVariant = 6;
            aSubcellVariantG1 = 6;
            aSubcellVariantG2 = 6;
            aSubcellVariantT = 5;
            break;
        case ELowListViewDensity:
            // 3x2
            aCellVariant = 2;
            aSubcellVariantG1 = 1;
            aSubcellVariantG2 = 1;
            aSubcellVariantT = 1;
            break;
        default:
            // 4x3
            aCellVariant = 4;
            aSubcellVariantG1 = 0;
            aSubcellVariantG2 = 0;
            aSubcellVariantT = 0;
            break;
        }
#endif
    }
    else
    {
        // Portrait orientation
        switch (aDensity)
        {
        case EHighListViewDensity:
            // 4x5
            aCellVariant = 5;
            aSubcellVariantG1 = 6;
            aSubcellVariantG2 = 6;
            aSubcellVariantT = 5;
            break;
        case ELowListViewDensity:
            // 2x3
            aCellVariant = 1;
            aSubcellVariantG1 = 1;
            aSubcellVariantG2 = 1;
            aSubcellVariantT = 1;
            break;
        default:
            // 3x4
            aCellVariant = 3;
            aSubcellVariantG1 = 0;
            aSubcellVariantG2 = 0;
            aSubcellVariantT = 0;
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Store the sizes for all densities for getIconSize
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateIconSizes(const TSwtListViewDensity& aDensity,
                                   const TRect& aCellRect)
{
    TAknLayoutRect iconRect;
    CSwtLafFacade::TSwtLafFacadeRectId rectIdPaneG1 = CSwtLafFacade::ECellHcAppPaneG1;

    // Low density
    TInt subcellVariantG1Low = 1;
    iconRect = CSwtLafFacade::GetLayoutRect(rectIdPaneG1, aCellRect,
                                            subcellVariantG1Low);
    iIconSizeInLowDensity.SetXY(iconRect.Rect().Width(),
                                iconRect.Rect().Height());

    // High density
    TInt subcellVariantG1High = 6;
    iconRect = CSwtLafFacade::GetLayoutRect(rectIdPaneG1, aCellRect,
                                            subcellVariantG1High);
    iIconSizeInHighDensity.SetXY(iconRect.Rect().Width(),
                                 iconRect.Rect().Height());

    // Medium density
    TInt subcellVariantG1Medium = 0;

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (CSwtLafFacade::IsLandscapeOrientation())
    {
        switch (aDensity)
        {
        case EMediumListViewDensity:
            subcellVariantG1Medium = 1;
            rectIdPaneG1 = CSwtLafFacade::ECellAppPaneG1;
            break;
        }
    }
#else
    (void)aDensity; // Suppress compilation warning
#endif

    iconRect = CSwtLafFacade::GetLayoutRect(rectIdPaneG1, aCellRect,
                                            subcellVariantG1Medium);
    iIconSizeInMediumDensity.SetXY(iconRect.Rect().Width(),
                                   iconRect.Rect().Height());
}

// ---------------------------------------------------------------------------
// Gets layout rectangle IDs
// ---------------------------------------------------------------------------
//
void CSwtListView::GetLafFacadeRectIds(
    const TSwtListViewDensity& aDensity,
    CSwtLafFacade::TSwtLafFacadeRectId& aRectIdPane,
    CSwtLafFacade::TSwtLafFacadeRectId& aRectIdPaneG1,
    CSwtLafFacade::TSwtLafFacadeRectId& aRectIdPaneG2,
    CSwtLafFacade::TSwtLafFacadeTextId& aRectIdPaneT1,
    CSwtLafFacade::TSwtLafFacadeFontId& aRectIdPaneT1Font) const
{
    aRectIdPane = CSwtLafFacade::ECellHcAppPane;
    aRectIdPaneG1 = CSwtLafFacade::ECellHcAppPaneG1;
    aRectIdPaneG2 = CSwtLafFacade::ECellHcAppPaneG2;
    aRectIdPaneT1 = CSwtLafFacade::ECellHcAppPaneT1;
    aRectIdPaneT1Font = CSwtLafFacade::ECellHcAppPaneT1Font;

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (CSwtLafFacade::IsLandscapeOrientation())
    {
        switch (aDensity)
        {
        case EMediumListViewDensity:
            aRectIdPane = CSwtLafFacade::ECellAppPane;
            aRectIdPaneG1 = CSwtLafFacade::ECellAppPaneG1;
            aRectIdPaneG2 = CSwtLafFacade::ECellAppPaneG2;
            aRectIdPaneT1 = CSwtLafFacade::ECellAppPaneT1;
            aRectIdPaneT1Font = CSwtLafFacade::ECellAppPaneT1Font;
            break;
        }
    }
#else
    (void)aDensity; // Suppress compilation warning
#endif
}


// ---------------------------------------------------------------------------
// Coerces cell rectangle to match native one
// ---------------------------------------------------------------------------
//
void CSwtListView::CoerceCellRect(const TSwtListViewDensity& aDensity, TRect& aCellRect) const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (CSwtLafFacade::IsLandscapeOrientation())
    {
        switch (aDensity)
        {
        case EMediumListViewDensity:
            aCellRect.iBr.iX -= 2;
            break;
        }
    }
#else
    (void)aDensity; // Suppress compilation warning
    (void)aCellRect; // Suppress compilation warning
#endif
}


// ---------------------------------------------------------------------------
// CSwtListView::SetVisibleScrollBar
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
void CSwtListView::SetVisibleScrollBar(TInt aStyle, TBool aVisible)
{
    iGrid->SetScrollbarVisibility(aStyle, aVisible);
    ASwtScrollableBase::SetVisibleScrollBar(aStyle, aVisible);
}

// ---------------------------------------------------------------------------
// CSwtListView::ScrollBarDestroyed
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
void CSwtListView::ScrollBarDestroyed(const MSwtScrollBar* aScrollBar)
{
    if (aScrollBar == iHScroll)
    {
        iGrid->SetScrollbarVisibility(KSwtStyleHScroll, EFalse);
    }
    else
    {
        iGrid->SetScrollbarVisibility(KSwtStyleVScroll, EFalse);
    }
    ASwtScrollableBase::ScrollBarDestroyed(aScrollBar);
}

#if defined( RD_SCALABLE_UI_V2)
// ---------------------------------------------------------------------------
// CSwtListView::HandleScrollEventL
// From ASwtScrollableBase
// ---------------------------------------------------------------------------
//
void CSwtListView::HandleScrollEventL(CEikScrollBar* aScrollBar,
                                      TEikScrollEvent aEventType)
{
    // On 5.0, drawing trough Java gives simply a better fps.
#ifdef RD_JAVA_S60_RELEASE_9_2
    switch (aEventType)
    {
    case EEikScrollThumbDragVert:
        GetShell().SetUrgentPaintControl(this);
        break;
    case EEikScrollThumbReleaseVert:
        GetShell().SetUrgentPaintControl(NULL);
        break;
    default:
        break;
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    iGrid->DoHandleScrollEventL(aScrollBar, aEventType);
    ASwtScrollableBase::HandleScrollEventL(aScrollBar, aEventType);
}

// ---------------------------------------------------------------------------
// CSwtListView::HandlePointerEventL
// From ASwtControlBase
// ---------------------------------------------------------------------------
//
void CSwtListView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    ASSERT(iGrid);

    CEikScrollBarFrame* sbFrame = iGrid->ScrollBarFrame();
    CEikScrollBar* vsb = sbFrame ? sbFrame->GetScrollBarHandle(CEikScrollBar::EVertical) : NULL;

    // Check if we should start scrollbar grabbing
    if (aPointerEvent.iType == TPointerEvent::EButton1Down && !iVScrollBarGrabsPointerEvents)
    {
        if (vsb && vsb->Rect().Contains(aPointerEvent.iPosition))
        {
            iVScrollBarGrabsPointerEvents = ETrue;
        }
    }

    // Deliver event to scrollbar
    if (iVScrollBarGrabsPointerEvents && vsb)
    {
        if (!iFlickScrollingOngoing
                && aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            // Scrollbar was tapped after scrolling stopped
            // by itself, so no need to redirect events
            iScrollbarPointerEventToListbox = EFalse;
        }

        if (iScrollbarPointerEventToListbox)
        {
            // Stops kinetic scrolling when scrollbar is tapped
            iGrid->ForwardPointerEventL(aPointerEvent);
            // Continue delivering events until button up appears to prevent
            // some unexpected behavior in both scrollbar and listbox
            switch (aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Up:
                iScrollbarPointerEventToListbox = EFalse;
                break;
            }
        }
        else
        {
            // Handles scrollbar behavior
            vsb->HandlePointerEventL(aPointerEvent);
        }
    }

    // Deliver event to list
    if (!iVScrollBarGrabsPointerEvents)
    {
        // Store the selection on first pointer event.
        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            if (iStyle & KSwtStyleMulti)
            {
                // Multi lists
                ASSERT(iOldSelectionArray);
                GetSelection(iOldSelectionArray);
            }
            iPrevFocusIndex = GetSelectionL(iGrid->CurrentDataIndex());
        }

        iGrid->HandlePointerEventL(aPointerEvent);

#ifdef RD_JAVA_S60_RELEASE_9_2
        // When flicking CEikListBox sets selection to 0 (during EDrag event).
        // So eSWT has to reload the iPrevFocusIndex, otherwise it might scroll
        // to the top of the ListView
        if (aPointerEvent.iType == TPointerEvent::EDrag)
        {
            iPrevFocusIndex = GetSelectionL(iGrid->CurrentDataIndex());
        }
#endif //RD_JAVA_S60_RELEASE_9_2

        if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
            TInt focusIndex = GetSelectionL(iGrid->CurrentDataIndex());

            if (iStyle & KSwtStyleMulti)
            {
                // Multi lists
                if ((iGrid->SelectionIndexes()->Count() != iOldSelectionArray->Count()))
                {
                    if (iDisplay.RevertPointerEvent())
                    {
                        // Revert.
                        if (iGrid->View()->ItemIsSelected(focusIndex))
                        {
                            iGrid->View()->DeselectItem(focusIndex);
                        }
                        else
                        {
                            iGrid->View()->SelectItemL(focusIndex);
                        }
                    }
                    else
                    {
                        iDisplay.PostSelectionEventL(iPeer);
                    }
                }
            }
            else
            {
                // Single lists
                // Compare the focus index after tap or drag ended
                if (focusIndex != iPrevFocusIndex)
                {
                    // Ensure the selection and focus index are in sync.
                    SingleSelectL(GetSelectionL(iGrid->CurrentDataIndex()), ETrue);

                    // Item focus highlight moved, send selection event.
                    // Changing the focus index is not subject to reverting.
                    iDisplay.PostSelectionEventL(iPeer);
                }

                // The default selection is sent from HandleListBoxEventL!
            }
        }
    }

    // Stop scrollbar grabbing
    if (iVScrollBarGrabsPointerEvents
            && aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        iVScrollBarGrabsPointerEvents = EFalse;
    }

    PostMouseEventL(aPointerEvent);
}

// ---------------------------------------------------------------------------
// CSwtListView::GetStylusPopupControlMenu()
// From MSwtControl
// ---------------------------------------------------------------------------
//
const MSwtMenu* CSwtListView::GetStylusPopupControlMenu() const
{
    return iStylusPopupMenu;
}
#endif // RD_SCALABLE_UI_V2


#ifdef RD_JAVA_S60_RELEASE_9_2
// ---------------------------------------------------------------------------
// CSwtListView::EnableFocusHighlight
// From MSwtControl
// ---------------------------------------------------------------------------
//
void CSwtListView::EnableFocusHighlight(TBool aEnable)
{
    ASSERT(iGrid);
    ASSERT(iGrid->View());

    CListItemDrawer* itemDrawer = iGrid->View()->ItemDrawer();
    if (itemDrawer)
    {
        TInt disabledHighlight =
            itemDrawer->Flags() & CListItemDrawer::EDisableHighlight;

        if (aEnable && disabledHighlight)
        {
            itemDrawer->ClearFlags(CListItemDrawer::EDisableHighlight);
        }
        else if (!aEnable && !disabledHighlight)
        {
            itemDrawer->SetFlags(CListItemDrawer::EDisableHighlight);
        }
    }
}
#endif //RD_JAVA_S60_RELEASE_9_2

TInt CSwtListView::FocusBackgroundPolicy() const
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    return ENoFocusBackgroundInCaptionedControl;
#else
    return EDefaultFocusBackground;
#endif // RD_JAVA_S60_RELEASE_9_2
}

void CSwtListView::PrepareForTraverse()
{
    ASSERT(iGrid);
    ASSERT(iGrid->View());

#ifdef RD_JAVA_S60_RELEASE_9_2
    // AvKon enables highlight only when key event is recieved.
    // When traversing, no key event is sent to AvKon, so we
    // have to enable highlight by ourselves.
    CListItemDrawer* itemDrawer = iGrid->View()->ItemDrawer();
    if (itemDrawer)
    {
        itemDrawer->ClearFlags(CListItemDrawer::ESingleClickDisabledHighlight);
    }
#endif //RD_JAVA_S60_RELEASE_9_2
}

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
void CSwtListView::DoControlSpecificFeedback(
    const TBool& aFirstTap,
    const TBool& aTappedToChildRect,
    const TPointerEvent& aPointerEvent) const
{
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback && !aTappedToChildRect)
    {
        switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
            if (aFirstTap)
            {
                feedback->InstantFeedback(ETouchFeedbackSensitiveList);
            }
            break;
        }
    }
}
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// ---------------------------------------------------------------------------
// CSwtTableListBox::UpdateFlickScrollingState
// Updates flick scrolling status based on received listbox event.
// ---------------------------------------------------------------------------
//
void CSwtListView::UpdateFlickScrollingState(TListBoxEvent aEventType)
{
    switch (aEventType)
    {
    case EEventFlickStarted:
        iFlickScrollingOngoing = ETrue;
        iScrollbarPointerEventToListbox = ETrue;
        break;
    case EEventFlickStopped:
        iFlickScrollingOngoing = EFalse;
        break;
    }
}
