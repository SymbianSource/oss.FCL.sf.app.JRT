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


#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknLayoutFont.h>
#include <AknBidiTextUtils.h>
#include <gulutil.h>
#include "swtcontrolhelper.h"
#include "swtimagetextitem.h"
#include "swttable.h"
#include "swttableitemdrawer.h"
#include "swtdisplay.h"


#ifdef ESWT_NATIVE_UITHREAD_OWN_HEAP
#define DEBUG_CHECK_IMAGES
#endif // ESWT_NATIVE_UITHREAD_OWN_HEAP


// ======== LOCAL FUNCTIONS ========


#ifdef DEBUG_CHECK_IMAGES
static void CheckRefImages(RPointerArray<CGulIcon>& aImages,
                           RPointerArray<const MSwtImage>& aRefImages,
                           const TSize& aSize)
{
    TInt countImg = aImages.Count();
    TInt countRefImg = aRefImages.Count();

    if (countImg != countRefImg)
    {
        ASSERT(EFalse);
    }

    for (TInt i = 0; i < countImg; i++)
    {
        const CGulIcon* img = aImages[i];
        const MSwtImage* refImg = aRefImages[i];
        if (img)
        {
            const CFbsBitmap* bmp = img->Bitmap();
            const CFbsBitmap* mask = img->Mask();
            ASSERT(refImg);
            ASSERT(bmp);
            ASSERT(img->BitmapsOwnedExternally());
            TSize size(SwtControlHelper::GetAspectRatioScaledBitmapSize(
                           refImg->Bitmap().SizeInPixels(), aSize));
            __UHEAP_MARK;
            ASSERT(bmp == &refImg->SubBitmap(size));
            ASSERT(mask == refImg->SubMaskBitmap(size));
            ASSERT(refImg->RefCount() >= 1);
            ASSERT(refImg->SubRefCount(size) >= 1);
            __UHEAP_MARKEND;
        }
    }
}
#endif // DEBUG_CHECK_IMAGES


// ======== MEMBER FUNCTIONS ========


CSwtImageTextItem* CSwtImageTextItem::NewL(MSwtDisplay& aDisplay,
        MSwtControl& aParent,
        const TSize& aImageSize,
        const TInt& aColumnCount)
{
    CSwtImageTextItem* self = new(ELeave) CSwtImageTextItem(aDisplay, aParent, aImageSize);
    CleanupStack::PushL(self);
    self->ConstructL(aColumnCount);
    CleanupStack::Pop(self);
    return self;
}


CSwtImageTextItem::~CSwtImageTextItem()
{
    // beware that GetColumnCount returns iText.Count()
    // so don't reset iText before calling GetColumnCount
    TInt nbColumn(GetColumnCount());

    iTexts.ResetAndDestroy();

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iImages, iRefImages, iImageSize);
#endif

    for (TInt i = 0; i < nbColumn; i++)
    {
        DeleteIcon(i);
    }
    iRefImages.Close();
    iImages.ResetAndDestroy();

    for (TInt i = 0; i < nbColumn; i++)
    {
        if (iFonts[i])
        {
            iFonts[i]->RemoveRef();
        }
    }
    iFonts.Close();

    if (iCommonFont)
    {
        iCommonFont->RemoveRef();
    }

    iBackgrounds.Close();
    iForegrounds.Close();

    iColumnWidths       = NULL;
    iColumnAlignments   = NULL;
    iColumnImageCounts  = NULL;
    iCommonFont         = NULL;
}


const MSwtColor* CSwtImageTextItem::GetBackground(const TInt& aColumnIndex) const
{
    TSwtTableColor rgbToUse = GetBackgroundRgb(aColumnIndex, ETrue);
    MSwtColor* color = NULL;
    // If this fails then user will get a Java Color object with a null handle.
    // I.e. isDisposed() will return true.
    TRAP_IGNORE(color = NewColorNativeObjectL(rgbToUse.iRgb));
    return color;
}


const MSwtColor* CSwtImageTextItem::GetForeground(const TInt& aColumnIndex) const
{
    TRgb rgbToUse = GetForegroundRgb(aColumnIndex);
    MSwtColor* color = NULL;
    // If this fails then user will get a Java Color object with a null handle.
    // I.e. isDisposed() will return true.
    TRAP_IGNORE(color = NewColorNativeObjectL(rgbToUse));
    return color;
}


void CSwtImageTextItem::SetBackground(const TInt& aColumnIndex, const MSwtColor* aColor)
{
    if (aColor)
    {
        iBackgrounds[aColumnIndex].iRgb = aColor->RgbValue();
        iBackgrounds[aColumnIndex].iIsSet = ETrue;
    }
    else
    {
        iBackgrounds[aColumnIndex].iIsSet = EFalse;
    }
    iParent.Redraw();
}


void CSwtImageTextItem::SetBackgroundForAll(const MSwtColor* aColor)
{
    if (aColor)
    {
        iCommonBackground.iRgb = aColor->RgbValue();
        iCommonBackground.iIsSet = ETrue;
    }
    else
    {
        iCommonBackground.iIsSet = EFalse;
    }
    iParent.Redraw();
}


void CSwtImageTextItem::SetForeground(const TInt& aColumnIndex, const MSwtColor* aColor)
{
    if (aColor)
    {
        iForegrounds[aColumnIndex].iRgb = aColor->RgbValue();
        iForegrounds[aColumnIndex].iIsSet = ETrue;
    }
    else
    {
        iForegrounds[aColumnIndex].iIsSet = EFalse;
    }
    iParent.Redraw();
}


void CSwtImageTextItem::SetForegroundForAll(const MSwtColor* aColor)
{
    if (aColor)
    {
        iCommonForeground.iRgb = aColor->RgbValue();
        iCommonForeground.iIsSet = ETrue;
    }
    else
    {
        iCommonForeground.iIsSet = EFalse;
    }
    iParent.Redraw();
}


TRect CSwtImageTextItem::GetBounds(const TRect& aItemRect, const TInt& aColumnIndex) const
{
    if (!iColumnWidths)
    {
        return aItemRect;
    }

    TRect bounds(aItemRect);
    TInt distanceFromLeadingEdge = 0;
    TInt width = 0;
    distanceFromLeadingEdge += GetMargins().iLeft;
    if (iIsMultiSelect)
    {
        distanceFromLeadingEdge += GetCheckBoxSize().iWidth;
    }
    for (TInt i = 0; i < aColumnIndex; i++)
    {
        width = (*iColumnWidths)[i];
        distanceFromLeadingEdge += width;
    }
    width = (*iColumnWidths)[aColumnIndex];
    if (AknLayoutUtils::LayoutMirrored())
    {
        bounds.iBr.iX -= distanceFromLeadingEdge;
        bounds.iTl.iX = bounds.iBr.iX - width;
    }
    else
    {
        bounds.iTl.iX += distanceFromLeadingEdge;
        bounds.iBr.iX = bounds.iTl.iX + width;
    }
    return bounds;
}


TRect CSwtImageTextItem::GetImageBounds(const TRect& aItemRect, const TInt& aColumnIndex) const
{
    TRect cellBounds(GetBounds(aItemRect, aColumnIndex));
    TSwtImageTextItemLayout itemCoord;
    LayoutCell(cellBounds, aColumnIndex, itemCoord, AknLayoutUtils::LayoutMirrored());

    const CGulIcon* image = iImages[aColumnIndex];
    if (!image)
    {
        // Size of the bound is null.
        return TRect(itemCoord.iImagePos, itemCoord.iImagePos);
    }

    return TRect(itemCoord.iImagePos, iImageSize);
}

const MSwtFont* CSwtImageTextItem::GetFont(const TInt& aColumnIndex) const
{
    const MSwtFont* font = iFonts[ aColumnIndex ];
    if (!font)
    {
        if (iCommonFont)
        {
            font = iCommonFont;
        }
        else
        {
            font = iParent.GetFont();
        }
    }
    return font;
}


TInt CSwtImageTextItem::SetFont(const TInt& aColumnIndex, const MSwtFont* aFont)
{
    if (iFonts[aColumnIndex])
    {
        iFonts[ aColumnIndex ]->RemoveRef();
    }

    iFonts[ aColumnIndex ] = aFont;

    if (aFont)
    {
        aFont->AddRef();
    }

    const MSwtFont* font = GetFont(aColumnIndex);
    ASSERT(font);

    return font->Font().FontMaxHeight();
}


void CSwtImageTextItem::SetFontForAll(const MSwtFont* aFont)
{
    if (iCommonFont)
    {
        iCommonFont->RemoveRef();
    }
    iCommonFont = aFont;
    if (iCommonFont)
    {
        iCommonFont->AddRef();
    }
}


// ---------------------------------------------------------------------------
// Grayed state is not supported. The state is only stored .
// ---------------------------------------------------------------------------
//
TBool CSwtImageTextItem::GetGrayed() const
{
    return iIsGrayed;
}


// ---------------------------------------------------------------------------
// Grayed state is not supported. The state is only stored .
// ---------------------------------------------------------------------------
//
void CSwtImageTextItem::SetGrayed(const TBool& aIsGrayed)
{
    iIsGrayed = aIsGrayed;
    iParent.Redraw();
}


TBool CSwtImageTextItem::IsChecked() const
{
    TBool checked(EFalse);
    if (iIsMultiSelect)
    {
        const CSwtTable& table = static_cast<const CSwtTable&>(iParent);
        checked = table.IsChecked(table.IndexOfTableItem(this));
    }
    return checked;
}


void CSwtImageTextItem::SetColumnAlignmentsArray(const RArray<TInt>& aColumnAlignments)
{
    iColumnAlignments = &aColumnAlignments;
}


void CSwtImageTextItem::SetColumnImageCounts(const RArray<TInt>& aColumnImageCounts)
{
    iColumnImageCounts = &aColumnImageCounts;
}


void CSwtImageTextItem::SetColumnWidthsArray(const RArray<TInt>& aColumnWidths)
{
    iColumnWidths = &aColumnWidths;
}


// ---------------------------------------------------------------------------
// Called by the TableItemDrawer DoDrawItemText.
// ---------------------------------------------------------------------------
void CSwtImageTextItem::Draw(CWindowGc& aGc, const CListItemDrawer& aItemDrawer,
                             const TRect& aItemRect, const TRect& aClippingRect,
                             TBool aItemIsCurrent, TBool aItemIsSelected) const
{
    TSwtTableMargins margins = GetMargins();
    const TBool layoutMirrored = AknLayoutUtils::LayoutMirrored();

    //  Draw the background for the whole row
    TSwtTableColor backgroundRgb = GetBackgroundRgb();
    TRect bgRect(aItemRect);
    ReduceRectToVisibleArea(bgRect);
    if ((!aItemIsCurrent && backgroundRgb.iIsSet) ||
            HideSelection() && backgroundRgb.iIsSet)
    {
        aGc.SetBrushColor(backgroundRgb.iRgb);
        aGc.SetPenStyle(CGraphicsContext::ENullPen);
        aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        aGc.DrawRect(bgRect);
    }
    else
    {
        MAknsControlContext* controlContext =
            static_cast<const CSwtTable*>(&iParent)->SwtMopSupplyCtrlContext();
        AknsDrawUtils::Background(AknsUtils::SkinInstance(),
                                  controlContext,
                                  &(iParent.CoeControl()),
                                  aGc,
                                  bgRect);
    }

    // Draw the full-row highlight for the current item
    if (aItemIsCurrent && !HideSelection())
    {
        // Draw skinned highlight background
        TRect outerRect(aItemRect);
        outerRect.iTl.iY += iGridLineWidth;
        ReduceRectToVisibleArea(outerRect,
                                margins.iLeft,
                                margins.iRight,
                                margins.iTop,
                                margins.iBottom);
        TRect innerRect(outerRect);
        innerRect.iTl.iX += margins.iLeft;
        innerRect.iTl.iY += margins.iTop;
        innerRect.iBr.iX -= margins.iRight;
        innerRect.iBr.iY -= margins.iBottom;
        TBool highlightDrawn =
            AknsDrawUtils::DrawFrame(AknsUtils::SkinInstance(),
                                     aGc,
                                     outerRect,
                                     innerRect,
                                     KAknsIIDQsnFrList,
                                     KAknsIIDQsnFrListCenter);
    }

    TRect cellRect(aItemRect);
    LeaveVerticalMargin(cellRect);
    LeaveHorizontalMargin(cellRect);

    // Draw the checkboxes before the first column
    if (iIsMultiSelect)
    {
        TRect checkBoxRect = CheckBoxRect(cellRect, layoutMirrored);
        DrawCheckBox(aGc, checkBoxRect, aItemIsSelected);

        // Move the cellRect to after the checkbox where the cells will be drawn
        if (layoutMirrored)
        {
            cellRect.iTl.iX = checkBoxRect.iTl.iX;
        }
        else
        {
            cellRect.iTl.iX = checkBoxRect.iBr.iX;
        }
    }
    else
    {
        if (layoutMirrored)
        {
            cellRect.iTl.iX = cellRect.iBr.iX;
        }
    }

    // Draw the cells

    TInt columnWidth(aItemRect.Width());
    TInt columnIndex;
    TInt nbColumn(GetColumnCount());
    for (columnIndex = 0; columnIndex < nbColumn; ++columnIndex)
    {
        columnWidth = ((*iColumnWidths)[columnIndex]);
        cellRect.SetWidth(columnWidth);

        // Move to the next column left if drawing mirrored
        if (layoutMirrored)
        {
            cellRect.Move(-columnWidth, 0);

            // Stop drawing if no more cells are visible.
            if (cellRect.iBr.iX < aItemRect.iTl.iX)
            {
                break;
            }
        }

        // Draw the cell if it's in the visible area
        if (cellRect.Intersects(aClippingRect))
        {
            if (!aItemIsCurrent || HideSelection())
            {
                TSwtTableColor cellBackgroundRgb = GetBackgroundRgb(columnIndex);
                if (cellBackgroundRgb.iIsSet)
                {
                    aGc.SetBrushColor(cellBackgroundRgb.iRgb);
                    aGc.SetPenStyle(CGraphicsContext::ENullPen);
                    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
                    TRect cellBgRect(cellRect);
                    cellBgRect.iTl.iY -= margins.iTop;
                    cellBgRect.iBr.iY += margins.iBottom;
                    aGc.DrawRect(cellBgRect);
                }
            }
            DrawCell(aGc, aItemDrawer, cellRect, aClippingRect, columnIndex,
                     aItemIsCurrent, layoutMirrored);
        }

        // Move to the next column right unless drawing mirrored
        if (!layoutMirrored)
        {
            cellRect.iTl.iX += columnWidth;
            // Stop drawing if no more cells are visible.
            if (cellRect.iTl.iX > aItemRect.iBr.iX)
            {
                break;
            }
        }
    }
}


void CSwtImageTextItem::SetTextArrayL(const CDesCArray* aTextArray)
{
    TInt columnCount(GetColumnCount());
    TInt nbText(aTextArray->Count() < columnCount ? aTextArray->Count() : columnCount);
    for (TInt i = 0; i < nbText; i++)
    {
        delete iTexts[i];
        iTexts[i] = NULL;
        iTexts[i] = aTextArray->MdcaPoint(i).AllocL();
    }
    iParent.Redraw();
}


void CSwtImageTextItem::SetImageArray(const RPointerArray<MSwtImage>& aImages)
{
    TInt columnCount(GetColumnCount());
    TInt imageCount(aImages.Count() < columnCount ? aImages.Count() : columnCount);

    for (TInt index = 0; index < imageCount; index++)
    {
        SetImageByIndex(index, aImages[index]);
    }
    iParent.Redraw();
}


void CSwtImageTextItem::ReduceRectToVisibleArea(TRect& aDrawRect,
        const TInt& aFrameLeft /* = 0 */,
        const TInt& aFrameRight /* = 0 */,
        const TInt& aFrameTop /* = 0 */,
        const TInt& aFrameBottom /* = 0 */) const
{
    TRect originalRect(aDrawRect);
    aDrawRect.Intersection(iParent.ClientRect());
    if (aDrawRect.IsEmpty())
    {
        return;
    }
    // If only a part of the frame is in the visible area then the original
    // position is kept because changing it would make the frame to appear
    // in a different position.
    // If the entire side of the frame is outside the visible area then
    // it's placed just outside the area to make the drawing rectangle as small
    // as possible without changing what is seen in the visible area.
    if (aFrameLeft > 0 && originalRect.iTl.iX != aDrawRect.iTl.iX)
    {
        TInt widthDelta = aDrawRect.iTl.iX - originalRect.iTl.iX;
        if (widthDelta < aFrameLeft)
        {
            aDrawRect.iTl.iX = originalRect.iTl.iX;
        }
        else
        {
            aDrawRect.iTl.iX -= aFrameLeft;
        }
    }
    if (aFrameRight > 0 && originalRect.iBr.iX != aDrawRect.iBr.iX)
    {
        TInt widthDelta = originalRect.iBr.iX - aDrawRect.iBr.iX;
        if (widthDelta < aFrameRight)
        {
            aDrawRect.iBr.iX = originalRect.iBr.iX;
        }
        else
        {
            aDrawRect.iBr.iX += aFrameLeft;
        }
    }
    if (aFrameTop > 0 && originalRect.iTl.iY != aDrawRect.iTl.iY)
    {
        TInt heightDelta = aDrawRect.iTl.iY - originalRect.iTl.iY;
        if (heightDelta < aFrameTop)
        {
            aDrawRect.iTl.iY = originalRect.iTl.iY;
        }
        else
        {
            aDrawRect.iTl.iY -= aFrameTop;
        }
    }
    if (aFrameBottom > 0 && originalRect.iBr.iY != aDrawRect.iBr.iY)
    {
        TInt heightDelta = originalRect.iBr.iY - aDrawRect.iBr.iY;
        if (heightDelta < aFrameBottom)
        {
            aDrawRect.iBr.iY = originalRect.iBr.iY;
        }
        else
        {
            aDrawRect.iBr.iY += aFrameBottom;
        }
    }
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtImageTextItem::SetGridLineWidth(const TInt& aPixelWidth)
{
    iGridLineWidth = aPixelWidth;
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
TSize CSwtImageTextItem::GetPreferredSize() const
{
    TInt itemWidth = 0;
    TInt itemHeight = 0;

    TInt nbColumn(GetColumnCount());
    for (TInt i = 0; i < nbColumn; i++)
    {
        TSize size(GetPreferredSize(i));
        itemWidth += size.iWidth;
        if (size.iHeight>itemHeight)
        {
            itemHeight = size.iHeight;
        }
    }

    return TSize(itemWidth,itemHeight);
}


// ---------------------------------------------------------------------------
// The side margins and checkbox column are not included in the column
// sizes but are outside of them. This is because in S60 the checkboxes are
// actually the only way to do multiselection and they must never be hidden
// in a multi-style table (e.g. if the first column is zero-sized).
// ---------------------------------------------------------------------------
//
TSize CSwtImageTextItem::GetPreferredSize(const TInt& aColumnIndex) const
{
    // Cumulative width
    TInt itemWidth = 0;

    // Height alternatives
    TInt checkBoxHeight = 0;
    TInt imageHeight = 0;
    TInt textHeight = 0;

    // Grid line height
    checkBoxHeight += iGridLineWidth;
    imageHeight += iGridLineWidth;
    textHeight += iGridLineWidth;

    // Grid line width. No grid line at the left of the first column, but
    // let's add the line width anyway so that this column can be packed at
    // index 0, and that width is enough to display the content even if this
    // column would get pushed to another index (by inserting new columns to index 0).
    itemWidth += iGridLineWidth;

    TSwtTableMargins margins = GetMargins();

    // Image
    if ((iColumnImageCounts && (*iColumnImageCounts)[aColumnIndex] > 0) ||
            iImages[aColumnIndex])
    {
        // If imageSize is set but there are no images in the array then it means
        // that images are in the header only. In that case don't leave space
        // for the images in the items.
        TSize imageSize(0, 0);
        if ((*iColumnImageCounts)[ aColumnIndex ] != 0)
        {
            imageSize = iImageSize;
        }

        // Add to combined width
        itemWidth += imageSize.iWidth;
        itemWidth += margins.iBetweenItems;

        // Add to height required by the image.
        imageHeight += imageSize.iHeight;
    }

    // Text

    // Add to height required by the text
    // Whatever the text is (NULL or not), we have to return the height according to the font

    const MSwtFont* font = GetFont(aColumnIndex);
    TInt fontHeight = font->Font().FontMaxHeight();
    textHeight += fontHeight;

    // Add to combined width
    if (iTexts[aColumnIndex])
    {
        itemWidth += font->TextWidthInPixels(*(iTexts[ aColumnIndex ]));

        // Margins before and after the text
        itemWidth += margins.iBetweenItems * 2;
    }

    // Return the combined width and the largest height
    return TSize(itemWidth, Max(Max(checkBoxHeight, imageHeight), textHeight)
                 + margins.iTop + margins.iBottom);
}


TSwtTableMargins CSwtImageTextItem::GetMargins() const
{
    return static_cast<const CSwtTable&>(iParent).GetMargins();
}


TInt CSwtImageTextItem::GetPreferredListModeItemWidth()
{
    TSwtTableMargins margins = GetMargins();
    TInt width = GetPreferredSize(0).iWidth;
    width += (margins.iLeft + margins.iRight);
    if (iIsMultiSelect)
    {
        width += GetCheckBoxSize().iWidth;
    }
    return width;
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtImageTextItem::InsertNewColumnL(const TInt& aIndex)
{
    // If one Leaves then we synchronize the arrays.
    User::LeaveIfError(iImages.Insert(NULL, aIndex));

    TInt error(KErrNone);
    error = iRefImages.Insert(NULL, aIndex);
    if (error != KErrNone)
    {
        iRefImages.Remove(aIndex);
        User::Leave(error);
    }

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iImages, iRefImages, iImageSize);
#endif

    error = iTexts.Insert(NULL, aIndex);
    if (error != KErrNone)
    {
        iImages.Remove(aIndex);
        iRefImages.Remove(aIndex);
        User::Leave(error);
    }

    error = iFonts.Insert(NULL, aIndex);
    if (error != KErrNone)
    {
        iTexts.Remove(aIndex);
        iImages.Remove(aIndex);
        iRefImages.Remove(aIndex);
        User::Leave(error);
    }

    error = iBackgrounds.Insert(TSwtTableColor(), aIndex);
    if (error != KErrNone)
    {
        iFonts.Remove(aIndex);
        iTexts.Remove(aIndex);
        iImages.Remove(aIndex);
        iRefImages.Remove(aIndex);
        User::Leave(error);
    }

    error = iForegrounds.Insert(TSwtTableColor(), aIndex);
    if (error != KErrNone)
    {
        iForegrounds.Remove(aIndex);
        iFonts.Remove(aIndex);
        iTexts.Remove(aIndex);
        iImages.Remove(aIndex);
        iRefImages.Remove(aIndex);
        User::Leave(error);
    }
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtImageTextItem::RemoveColumn(const TInt& aIndex)
{
    DeleteIcon(aIndex);
    iImages.Remove(aIndex);
    iRefImages.Remove(aIndex);

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iImages, iRefImages, iImageSize);
#endif

    if (iFonts[ aIndex ])
    {
        iFonts[aIndex]->RemoveRef();
    }
    iFonts.Remove(aIndex);

    HBufC *text=iTexts[ aIndex ];
    iTexts.Remove(aIndex);
    delete text;

    iBackgrounds.Remove(aIndex);
    iForegrounds.Remove(aIndex);
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtImageTextItem::SetTextL(const TInt& aColumnIndex, const TDesC& aText)
{
    if (iTexts[aColumnIndex])
    {
        delete iTexts[aColumnIndex];
        iTexts[aColumnIndex] = NULL;
    }
    iTexts[aColumnIndex] = aText.AllocL();

    if (static_cast<CSwtTable&>(iParent).ListMode())
    {
        static_cast<CSwtTable&>(iParent).PackColumn(0);     // Does a redraw
    }
    else
    {
        iParent.Redraw();
    }
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
const TDesC& CSwtImageTextItem::GetText(const TInt& aColumnIndex) const
{
    if (!iTexts[aColumnIndex])
    {
        return KNullDesC();
    }
    return *(iTexts[aColumnIndex]);
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
TBool CSwtImageTextItem::IsImageSet(const TInt& aColumnIndex) const
{
    if (iImages[ aColumnIndex ])
    {
        return ETrue;
    }
    return EFalse;
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtImageTextItem::SetImageByIndex(const TInt& aColumnIndex,
                                        const MSwtImage* aImage)
{
    // Delete old icon
    DeleteIcon(aColumnIndex);

    // Create new icon (scaled if not the first)
    CSwtTable& table = static_cast<CSwtTable&>(iParent);
    CGulIcon* newIcon = NULL;
    TRAP_IGNORE(newIcon = CreateIconL(aImage));

    // Insert new icon
    if (newIcon)
    {
        // Add CGulIcon
        iImages[aColumnIndex] = newIcon;

        // Add subreference
        const MSwtImage* newRefImg = aImage;
        if (newRefImg)
        {
            newRefImg->AddSubRef(newIcon->Bitmap()->SizeInPixels());
        }
        iRefImages[aColumnIndex] = newRefImg;
    }
    else
    {
        // Count icons
        TInt columnCount(iColumnImageCounts->Count());
        TInt imageCount(0);
        for (TInt i = 0; i < columnCount; ++i)
        {
            imageCount += (*iColumnImageCounts)[ i ];
        }
        if (imageCount == 0)
        {
            // Last image was removed.
            table.UpdateItemHeight(0);
        }
    }

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iImages, iRefImages, iImageSize);
#endif

    iParent.Redraw();
}


CSwtImageTextItem::CSwtImageTextItem(MSwtDisplay& aDisplay, MSwtControl& aParent,
                                     const TSize& aImageSize)
        :   iDisplay(aDisplay),
        iParent(aParent),
        iIsGrayed(EFalse),
        iIsMultiSelect((aParent.Style() & KSwtStyleMulti) ||
                       (aParent.Style() & KSwtStyleCheck)),
        iImageSize(aImageSize)
{
}


void CSwtImageTextItem::ConstructL(TInt aColumnCount)
{
    for (TInt i = 0; i < aColumnCount; i++)
    {
        InsertNewColumnL(0);
    }
    TruncateText(EFalse);
}


TSize CSwtImageTextItem::GetCheckBoxSize() const
{
    return static_cast<const CSwtTable&>(iParent).GetCheckBoxSize();
}


TInt CSwtImageTextItem::GetColumnCount() const
{
    // All arrays have same count
    return iTexts.Count();
}


void CSwtImageTextItem::GetCheckBoxBitmaps(const CFbsBitmap** aBitmap,
        const CFbsBitmap** aMask,
        const TBool& aItemIsSelected) const
{
    if (!iIsMultiSelect)
    {
        *aBitmap = NULL;
        *aMask  = NULL;
        return;
    }

    if (aItemIsSelected)
    {
        static_cast<const CSwtTable&>(iParent).GetCheckBoxBitmaps(ETrue, aBitmap, aMask);
    }
    else
    {
        static_cast<const CSwtTable&>(iParent).GetCheckBoxBitmaps(EFalse, aBitmap, aMask);
    }
}


void CSwtImageTextItem::DrawCheckBox(CWindowGc&    aGc,
                                     const TRect&  aDrawRect,
                                     const TBool&  aItemIsSelected) const
{
    const CFbsBitmap* bitmap = NULL;
    const CFbsBitmap* mask   = NULL;
    GetCheckBoxBitmaps(&bitmap, &mask, aItemIsSelected);
    ASSERT(bitmap);

    TRect rect(aDrawRect);
    if (mask)
        aGc.BitBltMasked(rect.iTl, bitmap, TRect(bitmap->SizeInPixels()), mask, ETrue);
    else
        aGc.BitBlt(rect.iTl, bitmap, TRect(bitmap->SizeInPixels()));
}


void CSwtImageTextItem::DrawCell(CWindowGc& aGc,
                                 const CListItemDrawer& aItemDrawer,
                                 const TRect& aCellRect,
                                 const TRect& aItemClippingRect,
                                 const TInt& aColumnIndex,
                                 const TBool& aItemIsCurrent,
                                 const TBool& aLayoutMirrored) const
{
    TSwtImageTextItemLayout itemCoord;
    LayoutCell(aCellRect, aColumnIndex, itemCoord, aLayoutMirrored);

    TRgb textColor;

    if (aItemIsCurrent)
    {
        textColor = GetHighlightForegroundRgb(aColumnIndex);
    }
    else
    {
        textColor = GetForegroundRgb(aColumnIndex);
    }

    // Draw the image
    const CGulIcon* image = iImages[ aColumnIndex ];
    if (image)
    {
        TRect imageClippingRect(aCellRect);
        imageClippingRect.Intersection(aItemClippingRect);
        imageClippingRect.Intersection(TRect(itemCoord.iImagePos, iImageSize));
        aGc.SetClippingRect(imageClippingRect);

        TSize bmpSize(image->Bitmap()->SizeInPixels());
        TPoint imgPos(itemCoord.iImagePos.iX + iImageSize.iWidth / 2 - bmpSize.iWidth / 2,
                      itemCoord.iImagePos.iY + iImageSize.iHeight / 2 - bmpSize.iHeight / 2);
        if (image->Mask())
        {
            aGc.BitBltMasked(imgPos,
                             image->Bitmap(),
                             bmpSize,
                             image->Mask(),
                             EFalse);
        }
        else
        {
            aGc.BitBlt(imgPos, image->Bitmap());
        }

        aGc.SetClippingRect(aItemClippingRect);
    }

    // Draw the text
    if (iTexts[aColumnIndex])
    {
        CGraphicsContext::TTextAlign alignment = Alignment(aColumnIndex, aLayoutMirrored);
        static_cast<const CSwtTableItemDrawer&>(aItemDrawer).DrawTableBidiText(
            aGc,
            &(GetFont(aColumnIndex)->Font()),
            textColor,
            *iTexts[ aColumnIndex ],
            itemCoord.iTextRect,
            alignment);
    }
}


void CSwtImageTextItem::TruncateText(const TBool& aIsUsing)
{
    iTruncateText = aIsUsing;
}


void CSwtImageTextItem::LayoutCell(const TRect& aDrawRect,
                                   const TInt& aColumnIndex,
                                   TSwtImageTextItemLayout& aItemCoordResult,
                                   const TBool& aLayoutMirrored) const
{
    TRect itemRect(aDrawRect);
    TSwtTableMargins margins = GetMargins();

    // If imageSize is set but there are no images in the array then it means
    // that images are in the header only. In that case don't leave space
    // for the images in the items.
    TSize imageSize(0, 0);
    if ((*iColumnImageCounts)[ aColumnIndex ] != 0)
    {
        imageSize = iImageSize;
    }

    // Leave space for the gridline
    itemRect.iTl.iX += iGridLineWidth;

    // The image
    const CGulIcon* image = iImages[aColumnIndex];
    TPoint imagePos;
    if (aLayoutMirrored)
    {
        imagePos.SetXY(itemRect.iBr.iX - imageSize.iWidth, itemRect.iTl.iY);
    }
    else
    {
        imagePos = itemRect.iTl;
    }

    if (image)
    {
        TInt itemHeight = itemRect.Height();
        if (imageSize.iHeight < itemHeight)
        {
            imagePos.iY += (itemHeight - imageSize.iHeight) / 2;
        }
    }

    // Set image coordinates (even if image is null, its size is zero but it still has a position)
    aItemCoordResult.iImagePos.SetXY(imagePos.iX, imagePos.iY);

    if ((iColumnImageCounts && (*iColumnImageCounts)[ aColumnIndex ] > 0) || image)
    {
        // A margin is left only if there is an image
        if (aLayoutMirrored)
        {
            aItemCoordResult.iImagePos.iX -= margins.iBetweenItems;
        }
        else
        {
            aItemCoordResult.iImagePos.iX += margins.iBetweenItems;
        }

        const TInt columnCount(GetColumnCount());

        // Set-up the location for the next element
        if (aLayoutMirrored)
        {
            itemRect.iBr.iX = aItemCoordResult.iImagePos.iX;
        }
        else
        {
            itemRect.iTl.iX = aItemCoordResult.iImagePos.iX + imageSize.iWidth;
        }
    }

    // The text
    if (aLayoutMirrored)
    {
        itemRect.iBr.iX -= margins.iBetweenItems;
        itemRect.iTl.iX += margins.iBetweenItems;
    }
    else
    {
        itemRect.iTl.iX += margins.iBetweenItems;
        itemRect.iBr.iX -= margins.iBetweenItems;
    }
    aItemCoordResult.iTextRect.SetRect(itemRect.iTl, itemRect.iBr);
}

// ---------------------------------------------------------------------------
// Some checkbox graphics include empty space in the graphic itself.
// For that reason there might be empty space inside the check box rectangle
// which causes it to appear misplaced even when the placement here is
// correct.
// ---------------------------------------------------------------------------
TRect CSwtImageTextItem::CheckBoxRect(const TRect& parentRect,
                                      const TBool& aLayoutMirrored) const
{
    TRect checkBoxRect;

    // Set-up location and size
    TSize checkBoxSize(GetCheckBoxSize());
    TPoint checkBoxLocation;
    if (aLayoutMirrored)
    {
        checkBoxLocation.SetXY(parentRect.iBr.iX - checkBoxSize.iWidth, parentRect.iTl.iY);
    }
    else
    {
        checkBoxLocation = parentRect.iTl;
    }
    checkBoxRect.SetRect(checkBoxLocation, checkBoxSize);

    // Center the checkbox vertically
    TInt verticalOffset = (parentRect.Height() - checkBoxSize.iHeight) / 2;
    checkBoxRect.iTl.iY += verticalOffset;

    return checkBoxRect;
}


void CSwtImageTextItem::LeaveHorizontalMargin(TRect& aRowRect) const
{
    TSwtTableMargins margins = GetMargins();
    aRowRect.iBr.iX -= margins.iLeft;
    aRowRect.iTl.iX += margins.iRight;
}


void CSwtImageTextItem::LeaveVerticalMargin(TRect& aRowRect) const
{
    TSwtTableMargins margins = GetMargins();
    aRowRect.iTl.iY += margins.iTop;
    aRowRect.iBr.iY -= margins.iBottom;
}


CGraphicsContext::TTextAlign CSwtImageTextItem::Alignment(const TInt& aColumnIndex,
        const TBool& aLayoutMirrored) const
{
    CGraphicsContext::TTextAlign alignment(CGraphicsContext::ELeft);
    if (iColumnAlignments)
    {
        TInt columnAlignment = (*iColumnAlignments)[ aColumnIndex ];
        switch (columnAlignment)
        {
        case CGraphicsContext::ELeft:
            alignment = aLayoutMirrored ? CGraphicsContext::ERight : CGraphicsContext::ELeft;
            break;
        case CGraphicsContext::ERight:
            alignment = aLayoutMirrored ? CGraphicsContext::ELeft : CGraphicsContext::ERight;
            break;
        case CGraphicsContext::ECenter:
            alignment = CGraphicsContext::ECenter;
            break;
        default:
            alignment = aLayoutMirrored ? CGraphicsContext::ERight : CGraphicsContext::ELeft;
            break;
        }
    }
    return alignment;
}


// ---------------------------------------------------------------------------
// Creates a new native Color object. This is used to return a native
// Color object to the Java side where it will be used to initialise a
// Java Color object.
// ---------------------------------------------------------------------------
//
MSwtColor* CSwtImageTextItem::NewColorNativeObjectL(const TRgb& aRgb) const
{
    return iDisplay.Factory().NewColorL(iDisplay.Device(), aRgb);
}


TRgb CSwtImageTextItem::GetForegroundRgb(const TInt& aColumnIndex) const
{
    TRgb rgb;
    if (iForegrounds[ aColumnIndex ].iIsSet)
    {
        rgb = iForegrounds[ aColumnIndex ].iRgb;
    }
    else if (iCommonForeground.iIsSet)
    {
        rgb = iCommonForeground.iRgb;
    }
    else
    {
        rgb = static_cast<const CSwtTable&>(iParent).GetForeground();
    }
    return rgb;
}


TRgb CSwtImageTextItem::GetHighlightForegroundRgb(const TInt& aColumnIndex) const
{
    TRgb rgb;
    if (iForegrounds[ aColumnIndex ].iIsSet)
    {
        rgb = iForegrounds[ aColumnIndex ].iRgb;
    }
    else if (iCommonForeground.iIsSet)
    {
        rgb = iCommonForeground.iRgb;
    }
    else
    {
        if (HideSelection())
        {
            rgb = GetForegroundRgb(aColumnIndex);
        }
        else
        {
            rgb = static_cast<const CSwtTable&>(iParent).GetHighlightTextRgb();
        }
    }
    return rgb;
}


TSwtTableColor CSwtImageTextItem::GetBackgroundRgb(const TInt& aColumnIndex /* = -1 */,
        const TBool& aAlsoReturnRgbForWallPaper /* = EFalse */) const
{
    TSwtTableColor rgb;
    if (aColumnIndex >= 0 && iBackgrounds[aColumnIndex].iIsSet)
    {
        rgb.iRgb = iBackgrounds[ aColumnIndex ].iRgb;
        rgb.iIsSet = ETrue;
    }
    else if (iCommonBackground.iIsSet)
    {
        rgb.iRgb = iCommonBackground.iRgb;
        rgb.iIsSet = ETrue;
    }
    else if (static_cast<const CSwtTable&>(iParent).BackgroundSet() ||
             aAlsoReturnRgbForWallPaper)
    {
        rgb.iRgb = static_cast<const CSwtTable&>(iParent).GetBackground();
        rgb.iIsSet = ETrue;
    }
    return rgb;
}


TBool CSwtImageTextItem::HideSelection() const
{
    return (!static_cast< const CSwtTable& >(iParent).IsFocused() &&
            (iParent.Style() & KSwtStyleHideSelection));
}

// ---------------------------------------------------------------------------
// The icon returned is scaled preserving the aspect ratio.
// Its real size might not be always iImageSize!
// ---------------------------------------------------------------------------
CGulIcon* CSwtImageTextItem::CreateIconL(const MSwtImage* aImage)
{
    CGulIcon* img = NULL;

    if (aImage)
    {
        // Common image size must be set before images are scaled
        ASSERT(iImageSize != TSize(0, 0));
        const TSize size(SwtControlHelper::GetAspectRatioScaledBitmapSize(
                             aImage->Bitmap().SizeInPixels(), iImageSize));
        CFbsBitmap* bmp = const_cast<CFbsBitmap*>(&aImage->SubBitmap(size));
        CFbsBitmap* mask = const_cast<CFbsBitmap*>(aImage->SubMaskBitmap(size));
        img = CGulIcon::NewL();
        img->SetBitmapsOwnedExternally(ETrue);
        img->SetBitmap(bmp);
        if (mask)
        {
            img->SetMask(mask);
        }
    }

    return img;
}

void CSwtImageTextItem::DeleteIcon(const TInt& aColumnIndex)
{
    CGulIcon* oldIcon = iImages[aColumnIndex];
    if (oldIcon)
    {
        // Remove subreference and clear the entry in the referenced images array
        CFbsBitmap* oldBmp = oldIcon->Bitmap();
        const MSwtImage* oldRefImg = iRefImages[aColumnIndex];
        if (oldRefImg)
        {
            oldRefImg->RemoveSubRef(oldBmp->SizeInPixels());
        }
        iRefImages[aColumnIndex] = NULL;

        // Free CGulIcon and clear the entry in the icon array
        delete oldIcon;
        iImages[aColumnIndex] = NULL;
    }
}
