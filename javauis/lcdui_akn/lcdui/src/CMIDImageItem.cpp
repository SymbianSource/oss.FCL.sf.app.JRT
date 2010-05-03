/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <eikenv.h>
#include <eiklabel.h>
// using API for (highlighted) hyperlink drawing (in Draw function)
#include <AknsDrawUtils.h>
// skin for (highlighted) hyperlink drawing
// SkinLayout::List_highlight_skin_placing__apps_specific__Line_1
#include <skinlayout.cdl.h>
#include <AknUtils.h>
// using CAknButton API for iButton
#include <aknbutton.h>
// icon bitmap for iButton
#include <gulicon.h>

#include <data_caging_path_literals.hrh>
#include <s60commonutils.h>

// LAF
#include <aknlayoutscalable_avkon.cdl.h>
// using TMifLcdui enumeration, constants used in (highlighted) hyperlink drawing
#include <lcdui.mbg>

#include "CMIDImageItem.h"
// needed API for retrieving commands count
#include "CMIDCommandList.h"
// API needed for iLabelControl (inherited from CMIDControlItem)
#include "CMIDItemLabel.h"

#ifdef __SCALABLE_ICONS
_LIT(KLcduiBitmapResourceFileName, "lcdui.mif");
#else
_LIT(KLcduiBitmapResourceFileName, "lcdui.mbm");
#endif



#include <j2me/jdebug.h>

/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDDisplayable.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDImageItem* CMIDImageItem::NewL(
    const TDesC& aLabel,MMIDImage* aImage,TLayout aLayout,
    TAppearance aAppearance,const TDesC& aAltText, CMIDUIManager* aUIManager)
{
    CMIDImageItem* item = new(ELeave) CMIDImageItem(aLayout, aAppearance, aUIManager);
    CleanupStack::PushL(item);
    item->ConstructL(aLabel,aImage,aAltText);
    CleanupStack::Pop(item);
    return item;
}

void CMIDImageItem::SetImageL(MMIDImage* aImage)
{

    TSize oldSize = iImageSize;

    if (iBitmapImage)
    {
        iBitmapImage->RemoveRef();
        iBitmapImage = NULL;
    }

    if (aImage)
    {
        iBitmapImage = aImage->BitmapImage();
    }

    if (iAppearance == MMIDItem::EButton)
    {
        CFbsBitmap* bitmap = NULL;
        TSize size(0,0);
        if (iBitmapImage)
        {
            bitmap = iBitmapImage->ColorBitmap();
            size = bitmap->SizeInPixels();
            iButtonIcon->SetBitmap(bitmap);
        }
        iButton->SetSize(size);
        UpdateButtonPosition();
    }

    UpdateImageSize();

    UpdatePreferredSizeL();

    if (iForm && (!iBitmapImage || (iImageSize != oldSize)))
    {
        iForm->RequestLayoutL();
    }
    else if (iForm)
    {
        iForm->DrawDeferred();
    }
}

void CMIDImageItem::UpdateButtonPosition()
{
    TRect rect = Rect();

    // button is centered horizontally
    TSize buttonSize = iButton->Size();
    TInt bx = rect.iTl.iX + (rect.Width() - buttonSize.iWidth) / 2;
    TInt by = rect.iTl.iY + iImageMargins.iTop;
    iButton->SetExtent(TPoint(bx, by), buttonSize);
}

void CMIDImageItem::SetAltTextL(const TDesC& /*aAltText*/)
{
    if (iAppearance == MMIDItem::EButton)
    {
        // tooltips are not used in lcdui
        // iButton->State()->SetHelpTextL(aAltText);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDImageItem::CMIDImageItem(
    TLayout aLayout, TAppearance aAppearance, CMIDUIManager* aUIManager)
        : CMIDControlItem(aLayout, aUIManager),iAppearance(aAppearance),
        iBitmapImage(NULL)
{
    iMMidItem = this;
}

void CMIDImageItem::ConstructL(const TDesC& aLabel,MMIDImage* aImage,const TDesC& aAltText)
{
    CMIDControlItem::ConstructL();

    if (iAppearance == MMIDItem::EButton)
    {
        iButtonIcon = CGulIcon::NewL();
        iButtonIcon->SetBitmapsOwnedExternally(ETrue); // the icon does not own bitmap
        iButton = CAknButton::NewL(iButtonIcon,   // aIcon
                                   NULL,         // aDimmedIcon
                                   NULL,         // aPressedIcon
                                   NULL,         // aHoverIcon
                                   KNullDesC,    // empty text
                                   KNullDesC,    // alt text
                                   KAknButtonNoFrame, // no frames for button
                                   0);
    }
    else
    {
        // Hyperlink frame graphics loading
        TFileName fileName;
        fileName.Append(KDC_APP_BITMAP_DIR);
        fileName.Append(KLcduiBitmapResourceFileName);
        fileName = java::util::S60CommonUtils::VerifiedFileNameL(fileName);

        MAknsSkinInstance* skin = AknsUtils::SkinInstance();

        TRAP_IGNORE(
            AknsUtils::CreateIconL(skin, KAknsIIDQgnGrafLinePrimaryHorizontal,
                                   iHyperlinkHorizontalLine, iHyperlinkHorizontalLineMask, fileName,
                                   EMbmLcduiQgn_graf_line_primary_horizontal,
                                   EMbmLcduiQgn_graf_line_primary_horizontal_mask));

        TRAP_IGNORE(
            AknsUtils::CreateIconL(skin, KAknsIIDQgnGrafLinePrimaryVertical,
                                   iHyperlinkVerticalLine, iHyperlinkVerticalLineMask, fileName,
                                   EMbmLcduiQgn_graf_line_primary_vertical,
                                   EMbmLcduiQgn_graf_line_primary_vertical_mask));
    }

    SetLabelL(aLabel);
    SetAltTextL(aAltText);
    SetImageL(aImage);
    SetFocusing(ETrue);



#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif


    UpdateMarginsAndImageSize();
}

void CMIDImageItem::UpdateMarginsAndImageSize()
{

    TAknWindowLineLayout layout;

    MMIDItem::TAppearance appearance = RenderedAppearance();

    if (appearance == EPlain)
    {
        layout = AknLayoutScalable_Avkon::form2_midp_image_pane(0).LayoutLine();
    }
    else if (appearance == EHyperLink)
    {
        layout = AknLayoutScalable_Avkon::form2_midp_image_pane(2).LayoutLine();
    }
    else // button appearance mode
    {
        layout = AknLayoutScalable_Avkon::form2_midp_image_pane(1).LayoutLine();
    }

    iImageMargins.iTop    = layout.it;
    iImageMargins.iBottom = layout.ib;
    iImageMargins.iLeft   = layout.il;
    iImageMargins.iRight  = layout.ir;

    UpdateImageSize();
}

void CMIDImageItem::UpdateImageSize()
{

    iImageSize = iBitmapImage ? iBitmapImage->ColorBitmap()->SizeInPixels() : TSize(0,0);
    TInt maxImageWidth = FormClientAreaWidth() - iImageMargins.iLeft - iImageMargins.iRight;
    iImageSize.iWidth = iImageSize.iWidth > maxImageWidth ? maxImageWidth : iImageSize.iWidth;

    if (iBitmapImage)
    {
        iBitmapImage->ColorBitmap()->Resize(iImageSize);
    }

    // iItemHeightWithoutLabel is also updated because it's relative to the margins
    iItemHeightWithoutLabel = iImageSize.iHeight + iImageMargins.iTop + iImageMargins.iBottom;

    if (RenderedAppearance() == MMIDItem::EButton)
    {
        iButton->SetSize(iImageSize);
        UpdateButtonPosition();
    }
}

void CMIDImageItem::SetLabelL(const TDesC& aLabel)
{
    CMIDControlItem::SetLabelL(aLabel);
    if (iLabelControl)
    {
        iLabelControl->SetCentered(ETrue);
    }

    if (iForm)
    {
        iForm->RequestLayoutL();
    }
}

void CMIDImageItem::SetPreferredSizeL(const TSize& aSize)
{
    iRequestedPreferredSize = CheckRequestedSize(aSize);
    UpdatePreferredSizeL();
}

/**
 Makes sure that the label text has the right width and can wrap appropriately

 Note that as MIDP spec defines the preferredSize includes margins.
*/
void CMIDImageItem::UpdatePreferredSizeL()
{
    iPreferredSize = MinimumSize();

    if (iPreferredSize.iWidth == 0 && iPreferredSize.iHeight == 0)
    {// it means we will not show up at all
        return;
    }

    if (iRequestedPreferredSize.iWidth > iPreferredSize.iWidth)
    {
        iPreferredSize.iWidth = iRequestedPreferredSize.iWidth;
    }

    if (iRequestedPreferredSize.iHeight > iPreferredSize.iHeight)
    {
        iPreferredSize.iHeight = iRequestedPreferredSize.iHeight;
    }

    if (iRequestedPreferredSize.iWidth < 0)
    { // Width is not locked
        if (iLabelControl && iLabelControl->Text()->Length() > 0
                && iPreferredSize.iWidth < iLabelControl->PreferredWidth())
        {
            iPreferredSize.iWidth = iLabelControl->PreferredWidth();
        }
    }

    if (iPreferredSize.iWidth > FormClientAreaWidth())
    {
        iPreferredSize.iWidth = FormClientAreaWidth();
    }

    if (iRequestedPreferredSize.iHeight < 0)
    {//height not locked, wrap text and increate height accordingly

        if (iLabelControl && iLabelControl->Text()->Length() > 0)
        {
            iLabelControl->SetWidthL(iPreferredSize.iWidth);
        }

        iPreferredSize.iHeight = LabelHeight() + iItemHeightWithoutLabel;
    }
    AdjustToSizeL(iPreferredSize);
}

TBool CMIDImageItem::IsSelectable() const
{
    return (CommandList()->Count() > 0);
}

TSize CMIDImageItem::MinimumSize()
{
    TInt height = iItemHeightWithoutLabel;

    if (!iLabelControl || (iLabelControl->Text()->Length() == 0))
    {
        if (!iBitmapImage)
        { //if no label and no image then make sure you do not show up
            return TSize(0,0);
        }
    }
    else
    { // Item has label
        height += OneLineLabelHeight();
    }

    TInt width = Max(iLabelControl->MinimumSize().iWidth,
                     (iImageSize.iWidth + iImageMargins.iLeft + iImageMargins.iRight));

    return TSize(width, height);
}

TInt CMIDImageItem::CountComponentControls() const
{
    if (RenderedAppearance() == MMIDItem::EButton)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

CCoeControl* CMIDImageItem::ComponentControl(TInt aIndex) const
{
    switch (aIndex)
    {
    case 0:
        return iLabelControl;
    case 1:
        return iButton;
    }
    return NULL;
}


void CMIDImageItem::DrawHyperlinkIndication(const TRect& parentRect) const
{
    ASSERT(RenderedAppearance() == MMIDItem::EHyperLink);

    CWindowGc& gc = SystemGc();

    // top side line drawing
    TAknLayoutRect frameRect;
    frameRect.LayoutRect(parentRect,
                         AknLayoutScalable_Avkon::form2_midp_field_pane_g1().LayoutLine());

    TRect lineRect = frameRect.Rect();

    gc.BitBltMasked(TPoint(lineRect.iTl.iX, lineRect.iTl.iY),
                    iHyperlinkHorizontalLine,
                    TRect(iHyperlinkHorizontalLine->SizeInPixels()),
                    iHyperlinkHorizontalLineMask, EFalse);

    // bottom side line drawing
    frameRect.LayoutRect(parentRect,
                         AknLayoutScalable_Avkon::form2_midp_field_pane_g2().LayoutLine());

    lineRect = frameRect.Rect();

    gc.BitBltMasked(TPoint(lineRect.iTl.iX, lineRect.iTl.iY),
                    iHyperlinkHorizontalLine,
                    TRect(iHyperlinkHorizontalLine->SizeInPixels()),
                    iHyperlinkHorizontalLineMask, EFalse);

    // left side line drawing
    frameRect.LayoutRect(parentRect,
                         AknLayoutScalable_Avkon::form2_midp_field_pane_g3().LayoutLine());

    lineRect = frameRect.Rect();

    gc.BitBltMasked(TPoint(lineRect.iTl.iX, lineRect.iTl.iY),
                    iHyperlinkVerticalLine,
                    TRect(iHyperlinkVerticalLine->SizeInPixels()),
                    iHyperlinkVerticalLineMask, EFalse);

    // right side line drawing
    frameRect.LayoutRect(parentRect,
                         AknLayoutScalable_Avkon::form2_midp_field_pane_g4().LayoutLine());

    lineRect = frameRect.Rect();

    gc.BitBltMasked(TPoint(lineRect.iTl.iX, lineRect.iTl.iY),
                    iHyperlinkVerticalLine,
                    TRect(iHyperlinkVerticalLine->SizeInPixels()),
                    iHyperlinkVerticalLineMask, EFalse);

    gc.Reset();
}

void CMIDImageItem::Draw(const TRect& /*aRect*/) const
{
    TRect rect = Rect();
    CMIDControlItem::Draw(rect);

    if (rect.Width() && rect.Height())
    {
        if (RenderedAppearance() == MMIDItem::EHyperLink)
        { // hyperlink label highlighting

            if (IsFocused() && (iLabelControl && iLabelControl->Text()->Length() > 0))
            { // Text of focused hyperlink highlight

                CWindowGc& gc = SystemGc();

                TRect tmpRect = iLabelControl->Rect();
                TAknLayoutRect skinCenter;
                skinCenter.LayoutRect(tmpRect,
                                      SkinLayout::List_highlight_skin_placing__apps_specific__Line_1(tmpRect));
                AknsDrawUtils::DrawFrame(AknsUtils::SkinInstance(),
                                         gc, tmpRect, skinCenter.Rect(),
                                         KAknsIIDQsnFrList, KAknsIIDQsnFrListCenter);
                gc.Reset();
            }
        }
    }

    if (RenderedAppearance() != MMIDItem::EButton)
    {
        if (iBitmapImage)
        {
            // image is centered
            TInt centeringX = (rect.Width() - iImageMargins.iLeft - iImageMargins.iRight -
                               iImageSize.iWidth) / 2;
            TInt centeringY = (rect.Height() - iImageMargins.iTop - iImageMargins.iBottom -
                               LabelHeight() - iImageSize.iHeight) / 2;

            TInt x = rect.iTl.iX + iImageMargins.iLeft + centeringX;
            TInt y = rect.iTl.iY + iImageMargins.iTop + centeringY;

            iBitmapImage->Draw(SystemGc(), TPoint(x,y),
                               TRect(TSize(iImageSize.iWidth, iImageSize.iHeight)));

            if (RenderedAppearance() == MMIDItem::EHyperLink)
            { // hyperlink frame drawing
                TRect parentRect = TRect(
                                       x - iImageMargins.iLeft, // TL.iX
                                       y - iImageMargins.iTop,  // TL.iY
                                       x + iImageSize.iWidth + iImageMargins.iRight,  // BR.iX
                                       y + iImageSize.iHeight + iImageMargins.iBottom);  // BR.iY
                DrawHyperlinkIndication(parentRect);
            }
        }
    }
}

void CMIDImageItem::SizeChanged()
{
    TRect rect = Rect();
    if (iLabelControl && iLabelControl->Text()->Length() > 0)
    {
        // label is centered
        TInt x = rect.iTl.iX + (rect.Width() - iLabelControl->Size().iWidth) / 2;

        TInt centeringY = (rect.Height() - iImageMargins.iTop - iImageMargins.iBottom -
                           LabelHeight() - iImageSize.iHeight) / 2;
        TInt y = rect.iTl.iY + centeringY + iImageMargins.iTop +
                 iImageSize.iHeight + iImageMargins.iBottom;

        iLabelControl->SetExtent(TPoint(x,y), iLabelControl->Size());
    }

    if (RenderedAppearance() == MMIDItem::EButton)
    {
        UpdateButtonPosition();
    }

    if (RenderedAppearance() == MMIDItem::EHyperLink)
    {
        // Hyperlink indication size setting
        // We are only interested in the size, the position of the parent rect doesn't matter here
        TRect parentRect = TRect(TSize(
                                     iImageSize.iWidth + iImageMargins.iLeft + iImageMargins.iRight,
                                     iItemHeightWithoutLabel));

        TAknLayoutRect frameRect;
        frameRect.LayoutRect(parentRect,
                             AknLayoutScalable_Avkon::form2_midp_field_pane_g1().LayoutLine());

        AknIconUtils::SetSize(iHyperlinkHorizontalLine, frameRect.Rect().Size(),
                              EAspectRatioNotPreserved);

        frameRect.LayoutRect(parentRect,
                             AknLayoutScalable_Avkon::form2_midp_field_pane_g3().LayoutLine());

        AknIconUtils::SetSize(iHyperlinkVerticalLine, frameRect.Rect().Size(),
                              EAspectRatioNotPreserved);
    }

    CMIDControlItem::SizeChanged();
}

void CMIDImageItem::FocusChanged(TDrawNow aDrawNow)
{
    CMIDControlItem::FocusChanged(aDrawNow);

    if (EDrawNow == aDrawNow && DrawableWindow())
    {
        DrawNow();
    }
}

#ifdef RD_SCALABLE_UI_V2
void CMIDImageItem::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
#ifdef RD_TACTILE_FEEDBACK
            // RenderedAppearance returns EHyperlink for initially hyperlink-created ImageItem,
            // or for plain-created ImageItem with commands added later.
            if (RenderedAppearance() == EHyperLink)
            {
                // if focus is changing, tactile feedback is given already in Form
                if (!iForm->IsFocusChangingWithPen() &&
                        (!HasLabel() || TappingActionRect().Contains(aPointerEvent.iPosition)))
                {
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
                    iFeedback->InstantFeedback(ETouchFeedbackSensitiveButton);
#else
                    iFeedback->InstantFeedback(ETouchFeedbackBasic);
#endif // RD_JAVA_ADVANCED_TACTILE_FEEDBACK
                }
            }
#endif // RD_TACTILE_FEEDBACK
        }

        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
            iPointerDownOnContentArea = RenderedAppearance() != MMIDItem::EPlain &&
                                        TappingActionRect().Contains(aPointerEvent.iPosition);
        }

        // pass the event to the button
        if (RenderedAppearance() == EButton)
        {
            iButton->HandlePointerEventL(aPointerEvent);
        }

        TBool consumed = iForm->TryDetectLongTapL(aPointerEvent);
        if (aPointerEvent.iType == TPointerEvent::EButton1Up && !consumed)
        {
            if (RenderedAppearance() != EButton)
            {
                CMIDControlItem::HandlePointerEventL(aPointerEvent);
            }

            if (RenderedAppearance() != MMIDItem::EPlain && iPointerDownOnContentArea &&
                    TappingActionRect().Contains(aPointerEvent.iPosition))
            {
                if (iForm->PhysicsEnabled())
                {
                    if (iForm->PhysicsScrolling())
                    {
                        // do nothing;
                    }
                    else
                    {
                        iForm->HandleTouchControlEventL(this,
                                                        MCoeControlObserver::EEventStateChanged);
                    }
                }
                else
                {
                    iForm->HandleTouchControlEventL(this,
                                                    MCoeControlObserver::EEventStateChanged);
                }
            }
            else
            {
                // the pen was drugged out of button
            }
        }
    }
}
#endif // RD_SCALABLE_UI_V2

#ifdef RD_SCALABLE_UI_V2
//
// The area sensitive to actions is the image itself for hyperlink and button appearance
//
TRect CMIDImageItem::TappingActionRect()
{
    if (iAppearance != MMIDItem::EButton)
    {
        TRect rect(Rect());
        TRect myRect;
        // we should take care of hits in the image only, not in space
        TInt centeringX = (rect.Width()
                           - iImageMargins.iLeft
                           - iImageMargins.iRight
                           - iImageSize.iWidth) / 2;
        TInt centeringY = (rect.Height()
                           - iImageMargins.iTop
                           - iImageMargins.iBottom
                           - LabelHeight() - iImageSize.iHeight) / 2;

        TInt x = rect.iTl.iX + iImageMargins.iLeft + centeringX;
        TInt y = rect.iTl.iY + iImageMargins.iTop + centeringY;
        myRect = TRect(TPoint(x, y), iImageSize);

        return myRect;
    }
    else
    {
        return iButton->Rect();
    }
}
#endif // RD_SCALABLE_UI_V2

TInt CMIDImageItem::ItemPreferredHeightWithoutLabel()
{
    return iItemHeightWithoutLabel;
}

TSize CMIDImageItem::ResetPreferredSize() const
{
    CMIDImageItem* self = const_cast<CMIDImageItem*>(this);
    TRAP_IGNORE(self->SetPreferredSizeL(iRequestedPreferredSize));
    return iPreferredSize;
}

MMIDItem::TAppearance CMIDImageItem::RenderedAppearance() const
{
    if (CommandList()->Count())
    {
        return (iAppearance == EButton) ? EButton : EHyperLink;
    }
    return EPlain;
}
//
CMIDImageItem::~CMIDImageItem()
{
    if (iBitmapImage)
    {
        iBitmapImage->RemoveRef();
        iBitmapImage = NULL;
    }
    delete iHyperlinkVerticalLine;
    delete iHyperlinkVerticalLineMask;
    delete iHyperlinkHorizontalLine;
    delete iHyperlinkHorizontalLineMask;
    delete iButton;
}

void CMIDImageItem::SetContainerWindowL(const CCoeControl& aContainer)
{
    CMIDControlItem::SetContainerWindowL(aContainer);

    if (iAppearance == EButton)
    {
        iButton->SetContainerWindowL(aContainer);
    }
    ActivateL();
}

/** Makes sure the label does not go beyond available size */
void CMIDImageItem::AdjustToSizeL(const TSize& aSize)
{
    TInt availableHeightForLabel = aSize.iHeight - iItemHeightWithoutLabel;

    if (iLabelControl && iLabelControl->Text()->Length() > 0)
    {
        TInt oldNumLabelLines = iLabelControl->MaxNumLines();
        iLabelControl->SetMaxNumLines(
            (availableHeightForLabel - iLabelControl->ItemLabelMargin()) /
            iLabelControl->LineHeight());
        iLabelControl->SetWidthL(aSize.iWidth);
        iLabelControl->SetMaxNumLines(oldNumLabelLines);
    }

    if (RenderedAppearance() == MMIDItem::EButton)
    {
        TSize buttonSize = iButton->Size();
        // we want button to be image size or smaller, not wider
        if (buttonSize.iWidth > aSize.iWidth)
        {
            buttonSize.iWidth = aSize.iWidth;
            iButton->SetSize(buttonSize);
            UpdateButtonPosition();
        }
    }
}

void CMIDImageItem::Dispose()
{
    delete this;
}

void CMIDImageItem::AddCommandL(MMIDCommand* aCommand)
{
    CMIDItem::AddCommandL(aCommand);
    UpdateMarginsAndImageSize();
}

void CMIDImageItem::RemoveCommand(MMIDCommand* aCommand)
{
    CMIDItem::RemoveCommand(aCommand);
    UpdateMarginsAndImageSize();
}

void CMIDImageItem::SetDefaultCommand(MMIDCommand* aCommand)
{
    CMIDItem::SetDefaultCommand(aCommand);
}


TSize CMIDImageItem::PreferredSize() const
{
    return ResetPreferredSize();
}

TSize CMIDImageItem::MinimumSize() const
{
    CCoeControl* control = const_cast<CMIDImageItem*>(this);
    return control->MinimumSize();
}

void CMIDImageItem::SetLayoutL(TLayout aLayout)
{
    CMIDItem::SetLayoutL(aLayout);
    UpdateMarginsAndImageSize();
}

void CMIDImageItem::ResolutionChange(TInt /*aType*/)
{
    UpdateMarginsAndImageSize();
}
// End of File
