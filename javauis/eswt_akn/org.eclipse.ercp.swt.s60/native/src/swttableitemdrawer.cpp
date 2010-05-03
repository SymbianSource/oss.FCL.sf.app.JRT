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


#include <eiklbi.h>
#include <eiktxlbm.h>
#include <eiklbd.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknBidiTextUtils.h>
#include "eswtwidgetsexpanded.h"
#include "swtcontrolhelper.h"
#include "swttableitemdrawer.h"
#include "swttablelistbox.h"
#include "swtimagetextitem.h"


// ======== MEMBER FUNCTIONS ========


CSwtTableItemDrawer::CSwtTableItemDrawer(MTextListBoxModel* aTextListBoxModel,
        const CFont* aFont,
        CSwtTableListBox& aTableListBox,
        CSwtTable &aTable)
        : CTextListItemDrawer(aTextListBoxModel, aFont),
        iTableListBox(aTableListBox),
        iTable(aTable)
{
    ClearFlags(SLafListBox::EItemDrawOnlyActiveSelection);
}


CSwtTableItemDrawer::~CSwtTableItemDrawer()
{
}


void CSwtTableItemDrawer::DrawTableBidiText(CWindowGc& aGc,
        const CFont* aFont,
        const TRgb& aTextColor,
        const TDesC& aText,
        const TRect& aTextRect,
        const CGraphicsContext::TTextAlign& aAlignment) const
{
    if (!aFont)
    {
        return;
    }
    aGc.UseFont(aFont);
    aGc.SetPenStyle(CGraphicsContext::ESolidPen);
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
    aGc.SetPenColor(aTextColor);

    // Vertically center the text inside the text rect
    const TInt excessSpace = aTextRect.Height() - aFont->FontMaxHeight();
    const TInt baseLineY = (excessSpace / 2) + aFont->FontMaxAscent();

    TPtrC textToDraw(aText.Ptr(), aText.Length());
    RBuf convertedText;

    TInt error = convertedText.Create(aText.Length() + KAknBidiExtraSpacePerLine);
    if (error == KErrNone)
    {
        TInt availableWidth(aTextRect.Width());
        TBool clipped = AknBidiTextUtils:: ConvertToVisualAndClip(
                            aText,
                            convertedText,
                            *aFont,
                            availableWidth,
                            availableWidth);

        textToDraw.Set(convertedText.Ptr(), convertedText.Length());
    }

    CFont::TMeasureTextInput::TFlags order = CFont::TMeasureTextInput::EFVisualOrder;
    if (AknLayoutUtils::LayoutMirrored())
    {
        order = CFont::TMeasureTextInput::EFVisualOrderRightToLeft;
    }

    TInt textWidth = AknBidiTextUtils::MeasureTextBoundsWidth(*aFont,
                     textToDraw,
                     order);
    TInt alignmentSpace = aTextRect.Width() - textWidth;
    TPoint textLocation(aTextRect.iTl.iX, aTextRect.iTl.iY + baseLineY);
    if (aAlignment == CGraphicsContext::ECenter)
    {
        textLocation.iX += (alignmentSpace / 2);
    }
    else if (aAlignment == CGraphicsContext::ERight)
    {
        textLocation.iX += alignmentSpace;
    }
    aGc.DrawText(textToDraw, textLocation);
    convertedText.Close();
}


TRect CSwtTableItemDrawer::ColumnRectFromWholeRect(const TRect& aRect) const
{
    TSwtTableMargins margins = iTable.GetMargins();
    TRect columnRect(aRect);
    columnRect.iTl.iY += margins.iTop;
    columnRect.iBr.iY -= margins.iBottom;
    if (AknLayoutUtils::LayoutMirrored())
    {
        columnRect.iBr.iX -= margins.iLeft;
        columnRect.iTl.iX += margins.iRight;
        if (iTable.Style() & KSwtStyleCheck || iTable.Style() & KSwtStyleMulti)
        {
            columnRect.iBr.iX -= iTable.GetCheckBoxSize().iWidth;
        }
    }
    else
    {
        columnRect.iBr.iX -= margins.iRight;
        columnRect.iTl.iX += margins.iLeft;
        if (iTable.Style() & KSwtStyleCheck || iTable.Style() & KSwtStyleMulti)
        {
            columnRect.iTl.iX += iTable.GetCheckBoxSize().iWidth;
        }
    }
    return columnRect;
}


// ---------------------------------------------------------------------------
// From class CTextListItemDrawer.
// ---------------------------------------------------------------------------
TInt CSwtTableItemDrawer::ItemWidthInPixels(TInt /*aItemIndex*/) const
{
    const RArray<TInt>& columnWidths=iTable.GetColumnWidths();
    TInt width(0);
    TInt nbColumn = columnWidths.Count();
    for (TInt i = 0; i < nbColumn; i++)
    {
        width += (columnWidths[ i ]) + iTable.GetGridLineWidth();
    }
    TSwtTableMargins margins(iTable.GetMargins());
    width += margins.iLeft + margins.iRight;
    if ((iTable.Style() & KSwtStyleMulti) || (iTable.Style() & KSwtStyleCheck))
    {
        width += iTable.GetCheckBoxSize().iWidth;
    }
    return width;
}


// ---------------------------------------------------------------------------
// From class CTextListItemDrawer.
// ---------------------------------------------------------------------------
TSize CSwtTableItemDrawer::MinimumCellSize() const
{
    TSwtTableMargins margins(iTable.GetMargins());
    TSize size(
        ItemWidthInPixels(0),
        iTableListBox.ItemHeight() + iTable.GetGridLineWidth() + margins.iTop + margins.iBottom);
    return size;
}


// ---------------------------------------------------------------------------
// From class CTextListItemDrawer.
// ---------------------------------------------------------------------------
void CSwtTableItemDrawer::DoDrawItemText(const TDesC&/* aDes*/,
        const TRect& aItemTextRect,
        TBool aItemIsCurrent,
        TBool /*aViewIsEmphasized*/,
        TBool aItemIsSelected,
        TInt aItemIndex) const
{
    const RPointerArray< CSwtImageTextItem >& imageTextItems = iTable.GetImageTextItems();
    ASSERT(aItemIndex < imageTextItems.Count());

    const CSwtImageTextItem* itemToDraw = (iTable.GetImageTextItems())[ aItemIndex ];

    // Set clipping rectangle for all item drawing
    // We use visible rectangle, otherwise items would be drawn
    // outside of parent composite.
    TRect clippingRect(iTable.VisibleRect(EFalse));
    if (!clippingRect.Intersects(aItemTextRect))
    {
        return;
    }
    iGc->SetClippingRect(clippingRect);

    // Add a clipping region in case there are composite children
    RRegion* childClippingRegion =
        iTable.SetupChildClippingRegion(iTable.CompositeInterface(),
                                        *iGc,
                                        clippingRect);

    TRect rect(aItemTextRect);

#ifdef RD_TACTILE_FEEDBACK
    if (aItemIsCurrent)
    {
        TRect feedbackRect(rect);
        feedbackRect.Intersection(clippingRect);
        iTableListBox.SetTouchFeedbackRect(feedbackRect);
    }
#endif // RD_TACTILE_FEEDBACK

    // Draw the item/row
    itemToDraw->Draw(*iGc,
                     *this,
                     rect,
                     clippingRect,
                     aItemIsCurrent,
                     aItemIsSelected);

    // Draw the gridlines
    if (iTable.GetLinesVisible())
    {
        TRect clippingRectForGridOnItem(aItemTextRect);
        clippingRectForGridOnItem.Intersection(clippingRect);
        iTableListBox.DrawGrid(*iGc, clippingRectForGridOnItem);
    }

    iTable.CancelChildClippingRegion(childClippingRegion, *iGc);
}
