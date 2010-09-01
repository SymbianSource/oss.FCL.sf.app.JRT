/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <AknsDrawUtils.h>
#include <swtlaffacade.h>
#include <gulicon.h>
#include "swtbuttonproxy.h"
#include "swttableheader.h"
#include "swttable.h"
#include "swtimagetextitem.h"
#include "swtdisplay.h"


#ifdef ESWT_NATIVE_UITHREAD_OWN_HEAP
#define DEBUG_CHECK_IMAGES
#endif // ESWT_NATIVE_UITHREAD_OWN_HEAP


// ======== LOCAL FUNCTIONS ========


#ifdef DEBUG_CHECK_IMAGES
static void CheckRefImages(RPointerArray<CSwtButtonProxy>& aButtons,
                           RPointerArray<const MSwtImage>& aRefImages)
{
    TInt countImg = aButtons.Count();
    TInt countRefImg = aRefImages.Count();

    if (countImg != countRefImg)
    {
        ASSERT(EFalse);
    }

    for (TInt i = 0; i < countImg; i++)
    {
        const CGulIcon* img = aButtons[i]->State()->Icon();
        const MSwtImage* refImg = aRefImages[i];
        if (img)
        {
            const CFbsBitmap* bmp = img->Bitmap();
            const CFbsBitmap* mask = img->Mask();
            ASSERT(refImg);
            ASSERT(bmp);
            ASSERT(img->BitmapsOwnedExternally());
            __UHEAP_MARK;
            ASSERT(bmp == &(refImg->Bitmap()));
            ASSERT(mask == refImg->MaskBitmap(ETrue));
            ASSERT(refImg->RefCount() >= 2);
            __UHEAP_MARKEND;
        }
    }
}
#endif // DEBUG_CHECK_IMAGES


// ======== MEMBER FUNCTIONS ========


CSwtTableHeader* CSwtTableHeader::NewL(CSwtTable& aTable)
{
    CSwtTableHeader* self = new(ELeave) CSwtTableHeader(aTable);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


CSwtTableHeader::~CSwtTableHeader()
{
#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iColumnHeaderButtons, iRefImages);
#endif

    iColumnHeaderButtons.ResetAndDestroy();

    TInt count = iRefImages.Count();
    const MSwtImage* img = NULL;
    for (TInt i = 0; i < count; i++)
    {
        img = iRefImages[i];
        if (img)
        {
            img->RemoveRef();
        }
    }
    iRefImages.Close();

    delete iDummyButton;
    iDummyButton = NULL;
}


void CSwtTableHeader::SetHScrollOffset(const TInt& aHScrollOffset)
{
    iHScrollOffset = aHScrollOffset;
    RepositionButtons();
}


void CSwtTableHeader::UpdateColumnWidth(const TInt& aColumnIndex,
                                        const TInt& aWidth)
{
    TSize size = iColumnHeaderButtons[ aColumnIndex ]->Size();
    size.iWidth = aWidth;
    // The first button is wider than the column width to span
    // the are top of margins and checkboxes.
    if (aColumnIndex == 0)
    {
        size.iWidth += LeadingExtraWidth();
    }
    iColumnHeaderButtons[ aColumnIndex ]->SetSize(size);

    // Always update the dummy button width.
    UpdateDummyButtonWidth();

    // Re-calculate the positions for the buttons
    RepositionButtons();
}


void CSwtTableHeader::UpdateColumnAlignment(const TInt& aColumnIndex,
        const TInt& aAlignment)
{
    CGraphicsContext::TTextAlign alignment =
        static_cast<CGraphicsContext::TTextAlign>(aAlignment);

    // For us ELeft means leading and ERight trailing alignment.
    // For CAknButton left is always left so we must convert it here to
    // have correct results in mirrored layout.
    if (AknLayoutUtils::LayoutMirrored())
    {
        if (alignment == CGraphicsContext::ELeft)
        {
            alignment = CGraphicsContext::ERight;
        }
        else if (alignment == CGraphicsContext::ERight)
        {
            alignment = CGraphicsContext::ELeft;
        }
    }

    iColumnHeaderButtons[ aColumnIndex ]->SetTextHorizontalAlignment(alignment);
}


void CSwtTableHeader::SetFont(const CFont* aFont)
{
    iFont = aFont;
    for (TInt i = 0; i < iColumnHeaderButtons.Count(); ++i)
    {
        iColumnHeaderButtons[ i ]->SetTextFont(iFont);
    }
}


void CSwtTableHeader::SetForegroundL(const MSwtColor* aColor)
{
    TRgb color(KRgbBlack);
    if (aColor)
    {
        color = aColor->RgbValue();
    }
    else
    {
        // Reset to default color
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                                  color,
                                  KAknsIIDQsnTextColors,
                                  EAknsCIQsnTextColorsCG63); // Messaging touch buttons
    }
    for (TInt i = 0; i < iColumnHeaderButtons.Count(); ++i)
    {
        iColumnHeaderButtons[i]->OverrideColorL(EColorButtonText, color);
    }
}


void CSwtTableHeader::SetForegroundL(const TRgb* aRgb, TInt aIndex)
{
    TRgb color(KRgbBlack);
    if (aRgb)
    {
        color = *aRgb;
    }
    else
    {
        // Reset to default color
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
                                  color,
                                  KAknsIIDQsnTextColors,
                                  EAknsCIQsnTextColorsCG63); // Messaging touch buttons
    }
    iColumnHeaderButtons[aIndex]->OverrideColorL(EColorButtonText, color);
}


void CSwtTableHeader::SetColumnHeaderObserver(MCoeControlObserver* aObserver,
        const TInt& aIndex)
{
    iColumnHeaderButtons[ aIndex ]->SetObserver(aObserver);
}


// ---------------------------------------------------------------------------
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtTableHeader::CountComponentControls() const
{
    return iColumnHeaderButtons.Count()
           + 1; // Dummy button
}


// ---------------------------------------------------------------------------
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtTableHeader::ComponentControl(TInt aIndex) const
{
    if (aIndex < iColumnHeaderButtons.Count())
    {
        return static_cast<CCoeControl*>(iColumnHeaderButtons[ aIndex ]);
    }
    else
    {
        return static_cast<CCoeControl*>(iDummyButton);
    }
}


// ---------------------------------------------------------------------------
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTableHeader::SizeChanged()
{
    // Update column header button heights
    for (TInt i = 0; i < iColumnHeaderButtons.Count(); ++i)
    {
        UpdateButtonHeight(i);
    }
    // Update the dummy button height
    TSize size = iDummyButton->Size();
    size.iHeight = Size().iHeight;
    iDummyButton->SetSize(size);

    UpdateDummyButtonWidth();
}


// ---------------------------------------------------------------------------
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTableHeader::PositionChanged()
{
    RepositionButtons();
}


// ---------------------------------------------------------------------------
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtTableHeader::Draw(const TRect& aRect) const
{
    // Draw the background in case the button graphics are transparent.
    TRect bgRect(aRect);
    bgRect.Intersection(Rect());
    MAknsControlContext* cc = iTable.SwtMopSupplyCtrlContext();
    AknsDrawUtils::Background(AknsUtils::SkinInstance(), cc, this, SystemGc(), bgRect);
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
TSize CSwtTableHeader::GetPreferredSize() const
{
    TInt itemWidth = 0;
    TInt itemHeight = 0;

    TInt nbColumn(iColumnHeaderButtons.Count());
    for (TInt i = 0; i < nbColumn; i++)
    {
        TSize size(GetPreferredSize(i));
        itemWidth += size.iWidth;
        if (size.iHeight > itemHeight)
        {
            itemHeight = size.iHeight;
        }
    }

    return TSize(itemWidth, itemHeight);
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
TSize CSwtTableHeader::GetPreferredSize(const TInt& aColumnIndex) const
{
    TSize prefSize(0, 0);

    const CGulIcon* icon = NULL;
    TInt textLength(0);
    TSize minimumSize(0,0);

    if (iColumnHeaderButtons[ aColumnIndex ])
    {
        icon = iColumnHeaderButtons[ aColumnIndex ]->State()->Icon();
        textLength = iColumnHeaderButtons[ aColumnIndex ]->State()->Text().Length();
        minimumSize = iColumnHeaderButtons[ aColumnIndex ]->MinimumSize();
    }

    TBool hasIcon = (icon) ? ETrue: EFalse;
    TBool hasText = (textLength > 0) ? ETrue: EFalse;

    TSize prefIconSize(0, 0);
    if (hasIcon)
    {
        if (icon->Bitmap())
        {
            prefIconSize = icon->Bitmap()->SizeInPixels();
        }
    }

    TSize prefTextSize(0, 0);
    if (hasText || !hasIcon)
    {
        prefTextSize = minimumSize;

        TInt newHeight(iTable.GetFont()->Font().FontMaxHeight());

        // WORKAROUND: compute of baseline in CAknButton::Draw is not performed correctly.
        // So, the descent is added to the compute of preferred height.
        newHeight += iTable.GetFont()->Font().DescentInPixels();
        // END WORKAROUND

        if (newHeight > prefTextSize.iHeight)
        {
            prefTextSize.iHeight = newHeight;
        }
    }

    TInt frameWidth = 0;
    TInt frameHeight = 0;

    TAknLayoutRect tl =
        CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EToolbarButtonPaneG2, TRect(), 0);
    TAknLayoutRect br =
        CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EToolbarButtonPaneG5, TRect(), 0);

    frameWidth = tl.Rect().Width() + br.Rect().Width();
    frameHeight = tl.Rect().Height() + br.Rect().Height();

    prefSize.iWidth = prefIconSize.iWidth + prefTextSize.iWidth;
    prefSize.iHeight = Max(prefIconSize.iHeight + frameHeight, prefTextSize.iHeight);

    prefSize.SetSize(prefSize.iWidth + frameWidth, prefSize.iHeight);

    // The first column has some extra space above the margins and
    // checkboxes.
    TInt extraWidth(0);
    if (aColumnIndex == 0)
    {
        extraWidth = LeadingExtraWidth();
    }
    if (prefSize.iWidth > extraWidth)
    {
        prefSize.iWidth -= extraWidth;
    }

    return prefSize;
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtTableHeader::InsertNewColumnL(const TInt& aIndex)
{
    InsertButtonL(aIndex);
    SetForegroundL(iTable.CustomForeground(), aIndex);
    UpdateColumnWidth(aIndex, 0);
    UpdateButtonHeight(aIndex);
    UpdateColumnAlignment(aIndex, CGraphicsContext::ELeft);
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtTableHeader::RemoveColumn(const TInt& aIndex)
{
    ASSERT(aIndex < iColumnHeaderButtons.Count());
    RemoveButton(aIndex);
    RepositionButtons();
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtTableHeader::SetTextL(const TInt& aColumnIndex, const TDesC& aText)
{
    iColumnHeaderButtons[ aColumnIndex ]->State()->SetTextL(aText);

    if (iTable.ListMode())
    {
        iTable.PackColumn(0);   // Does a redraw
    }
    else
    {
        iTable.Redraw();
    }
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
const TDesC& CSwtTableHeader::GetText(const TInt& aColumnIndex) const
{
    return iColumnHeaderButtons[ aColumnIndex ]->State()->Text();
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
TBool CSwtTableHeader::IsImageSet(const TInt& aColumnIndex) const
{
    if (iColumnHeaderButtons[ aColumnIndex ]->State()->Icon())
    {
        return ETrue;
    }
    return EFalse;
}


// ---------------------------------------------------------------------------
// From MSwtTableColumnar
// ---------------------------------------------------------------------------
//
void CSwtTableHeader::SetImageByIndex(const TInt& aColumnIndex, const MSwtImage* aImage)
{
    CGulIcon* icon = NULL;
    TRAP_IGNORE(icon = CreateIconL(aImage));

    // Ownership taken by the CAknButtonState
    // In case the icon creation failed we set NULL here which unsets the icon.
    iColumnHeaderButtons[ aColumnIndex ]->State()->SetIcon(icon);
    const MSwtImage* oldRefImg = iRefImages[aColumnIndex];
    if (oldRefImg)
    {
        oldRefImg->RemoveRef();
    }
    const MSwtImage* newRefImg = aImage;
    if (newRefImg)
    {
        newRefImg->AddRef();
    }
    iRefImages[aColumnIndex] = newRefImg;

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iColumnHeaderButtons, iRefImages);
#endif
}


CSwtTableHeader::CSwtTableHeader(CSwtTable& aTable) : iTable(aTable)
{
}


void CSwtTableHeader::ConstructL()
{
    SetContainerWindowL(iTable);
    SetParent(&iTable);
    SetComponentsToInheritVisibility(ETrue);

    // Create a dummy button that will be used to cover the area
    // tailing the columns. It looks nicer than empty area if
    // the columns are not wide enough to fill all available space
    // in the control.
    iDummyButton = CreateHeaderButtonL();

    ActivateL();
}


CSwtButtonProxy* CSwtTableHeader::CreateHeaderButtonL()
{
    CSwtButtonProxy* button = CSwtButtonProxy::NewL(this, NULL, KNullDesC,
                              KAknButtonSizeFitText);
    CleanupStack::PushL(button);
    button->SetTextFont(iFont);
    button->ActivateL();
    CleanupStack::Pop(button);
    return button;
}


void CSwtTableHeader::InsertButtonL(const TInt& aColumnIndex)
{
    CSwtButtonProxy* button = CreateHeaderButtonL();
    CleanupStack::PushL(button);
    User::LeaveIfError(iColumnHeaderButtons.Insert(button, aColumnIndex));
    CleanupStack::Pop(button);
    TInt error = iRefImages.Insert(NULL, aColumnIndex);
    if (error != KErrNone)
    {
        iRefImages.Remove(aColumnIndex);
        User::Leave(error);
    }

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iColumnHeaderButtons, iRefImages);
#endif
}


void CSwtTableHeader::RemoveButton(const TInt& aColumnIndex)
{
    CSwtButtonProxy* button = iColumnHeaderButtons[ aColumnIndex ];
    iColumnHeaderButtons.Remove(aColumnIndex);
    delete button;
    const MSwtImage* refImg = iRefImages[aColumnIndex];
    if (refImg)
    {
        refImg->RemoveRef();
    }
    iRefImages.Remove(aColumnIndex);

#ifdef DEBUG_CHECK_IMAGES
    CheckRefImages(iColumnHeaderButtons, iRefImages);
#endif
}


void CSwtTableHeader::RepositionButtons()
{
    TInt xOffset(-iHScrollOffset);
    TRect tableClientRect(iTable.ClientRect());

    if (AknLayoutUtils::LayoutMirrored())
    {
        TInt scrollbarWidth(TailingExtraWidth() - iTable.GetMargins().iRight);

        iDummyButton->SetPosition(
            TPoint(tableClientRect.iTl.iX - scrollbarWidth + xOffset,
                   tableClientRect.iTl.iY));
        xOffset += iDummyButton->Size().iWidth;

        for (TInt i = iColumnHeaderButtons.Count() - 1; i >= 0 ; --i)
        {
            iColumnHeaderButtons[ i ]->SetPosition(
                TPoint(tableClientRect.iTl.iX - scrollbarWidth + xOffset,
                       tableClientRect.iTl.iY));
            xOffset += iColumnHeaderButtons[ i ]->Size().iWidth;
        }
    }
    else
    {
        for (TInt i = 0; i < iColumnHeaderButtons.Count(); ++i)
        {
            iColumnHeaderButtons[ i ]->SetPosition(
                TPoint(tableClientRect.iTl.iX + xOffset, tableClientRect.iTl.iY));
            xOffset += iColumnHeaderButtons[ i ]->Size().iWidth;
        }
        iDummyButton->SetPosition(
            TPoint(tableClientRect.iTl.iX + xOffset, tableClientRect.iTl.iY));
    }
}


void CSwtTableHeader::UpdateButtonHeight(const TInt& aColumnIndex)
{
    TSize size = iColumnHeaderButtons[ aColumnIndex ]->Size();
    size.iHeight = Size().iHeight;
    iColumnHeaderButtons[ aColumnIndex ]->SetSize(size);
}


void CSwtTableHeader::UpdateDummyButtonWidth()
{
    // Check how wide are all the buttons combined
    TInt columnCount(iColumnHeaderButtons.Count());
    TInt widthOfAllColumnHeaderButtons(0);
    for (TInt i = 0; i < columnCount; ++i)
    {
        widthOfAllColumnHeaderButtons +=
            iColumnHeaderButtons[ i ]->Size().iWidth;
    }
    // Use the dummy button to cover the area not covered
    // by the column header buttons.
    TInt tailingExtraWidth(TailingExtraWidth());
    TInt scrollbarWidth(tailingExtraWidth - iTable.GetMargins().iRight);
    TInt newWidth =
        Max(iTable.ClientRect().Width() - widthOfAllColumnHeaderButtons + scrollbarWidth,
            TailingExtraWidth());

    iDummyButton->SetSize(TSize(newWidth,
                                iDummyButton->Size().iHeight));
}


TInt CSwtTableHeader::LeadingExtraWidth() const
{
    TInt width(0);
    // The space above the leading margin.
    width += iTable.GetMargins().iLeft;
    // The space above the checkboxes.
    if (iTable.Style() & KSwtStyleCheck || iTable.Style() & KSwtStyleMulti)
    {
        width += iTable.GetCheckBoxSize().iWidth;
    }
    return width;
}


TInt CSwtTableHeader::TailingExtraWidth() const
{
    TInt width(0);

    // The space above the tailing margin.
    width += iTable.GetMargins().iRight;
    // The space above the scrollbar.
    TRect rect(Rect());
    TRect sbOuterRect(iTable.SbOuterRect(rect));
    width += Max(sbOuterRect.Width() - rect.Width(), 0);
    return width;
}

CGulIcon* CSwtTableHeader::CreateIconL(const MSwtImage* aImage)
{
    CGulIcon* img = NULL;

    if (aImage)
    {
        CFbsBitmap* bmp = const_cast<CFbsBitmap*>(&(aImage->Bitmap()));
        CFbsBitmap* mask = const_cast<CFbsBitmap*>(aImage->MaskBitmap(ETrue));     // inverted
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
