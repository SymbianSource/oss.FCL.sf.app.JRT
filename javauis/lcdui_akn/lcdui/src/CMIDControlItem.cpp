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
* Description:  ?Description
*
*/


#include <coemain.h>
#include <eikcapc.h>

#include "CMIDControlItem.h"
#include "CMIDGc.h"
#include "CMIDUtils.h"
#include "CMIDCommand.h"
#include "CMIDForm.h"
#include "CMIDCommandList.h"
// needed API for iLabelControl
#include "CMIDItemLabel.h"
// API for retrieving background control context
#include "CMIDDisplayable.h"
#include "CMIDMenuHandler.h"

#include <AknUtils.h>
#include <AknsDrawUtils.h>
// using API for CAknsFrameBackgroundControlContext (member iHighlightedBackgroundCc)
#include <AknsFrameBackgroundControlContext.h>
// LAF
// AknLayoutScalable_Avkon::form2_midp_content_pane
#include <aknlayoutscalable_avkon.cdl.h>

#include <j2me/jdebug.h>

#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDControlItem.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);

/** This constant limits the maximum number of lines in an item label */
const TInt KMaxNumLines  = 10;

///////////////////////////////////////////////////////////////////////////////////////////////////
// class CMIDControlItem
///////////////////////////////////////////////////////////////////////////////////////////////////

CMIDControlItem::CMIDControlItem(MMIDItem::TLayout aLayout, CMIDUIManager* aUIManager)
        : CMIDItem(aLayout, aUIManager)
        , iCoeEnv(CCoeEnv::Static())
        , iHighlighted(EFalse)
{
}

CMIDControlItem::~CMIDControlItem()
{
    delete iLabelControl;
    if (iForm)
    {
        iForm->RemoveDeletedItem(*this);
    }
}

void CMIDControlItem::ConstructL()
{
    CMIDItem::ConstructL();

    iLabelControl = CMIDItemLabel::NewL(FormClientAreaWidth(),
                                        Type() != MMIDComponent::EImageItem,
                                        KMaxNumLines);

    if (iLabel)
    {
        iLabelControl->SetTextL(*iLabel);
    }
}

TInt CMIDControlItem::FormClientAreaWidth() const
{
    return iForm ? iForm->Width(): CMIDForm::FormWidth();
}

TRect CMIDControlItem::FormClientAreaRect() const
{
    return iForm ? iForm->FormRect(): CMIDForm::StaticFormRect();
}

// Returns bottom margin of form2_midp_content_pane
TInt CMIDControlItem::ItemContentBottomMargin() const
{

    TAknLayoutRect form2MidpContentPaneRect;
    TRect formRect;
    if (iForm)
    {
        formRect = iForm->FormRect();
    }
    else
    {
        formRect = CMIDForm::StaticFormRect();
    }
    form2MidpContentPaneRect.LayoutRect(formRect,
                                        AknLayoutScalable_Avkon::form2_midp_content_pane(0).LayoutLine());
    return formRect.iBr.iY - form2MidpContentPaneRect.Rect().iBr.iY;
}

/**
 * Resets the preferred size if the minimum size has changed.
 * The minimum size will change if the label or content is modified.
 */
TSize CMIDControlItem::ResetPreferredSize() const
{
    CMIDControlItem* self = const_cast<CMIDControlItem*>(this);
    self->SetPreferredSize(iRequestedPreferredSize,self->MinimumSize());
    return iPreferredSize;
}

/** This method is called by the form row when sizing the items
once it has determined the width available to the item. The item
will typically wrap its label to this width so that its height
will then be modified accordingly. See CMIDFormRow::SizeItemsL().

Items that have other width dependent behavious may want to extend
this method. For example string items may have to wrap their content
as well as the label to the given width.
*/
void CMIDControlItem::AdjustToNewWidthL(TInt aWidth)
{
    ASSERT(aWidth >= 0);

    if (iLabelControl && iLabelControl->Text()->Length() > 0)
    {

        iLabelControl->SetWidthL(aWidth);

        if (iRequestedPreferredSize.iHeight < 0)
        {
            iPreferredSize.iHeight = LabelHeight() + ItemPreferredHeightWithoutLabel();
        }
    }
}

/** After having called AdjustToNewWidthL() and done its calculations, the form
row knows what size should be assigned to items and calls this method after having
called SetSizeQuiet. The elements inside an item should made to fit this size.
Typically this size is the same as the requested size (the width passed to  AdjustToNewWidthL()
plus the resulting height. However if layout shrink applies, this may well be the minimum size
instead. This method is equally called by some items after having calculated their preferred
size. See SetPreferredSizeL().

Items that do not implement this method (text field, date field, choice group and gauge)
are such that their minimum size can always fit the entire label and any other element. This
is typical of items that fill the entire form width, basically they always get the size they
want in this case (the entire width plus as much height as needed).

Finally, we do not provide a common implementation that adjusts the label because we
do not know how much space of the given size is reserved to the label. */
void CMIDControlItem::AdjustToSizeL(const TSize& /*aSize*/)
{

}

TRect CMIDControlItem::FocusableRect()
{
    return Rect();
}

TBool CMIDControlItem::IsSelectable() const
{
    return (!IsNonFocusing() || (CommandList() ? CommandList()->Count() > 0 : 0));
}

void CMIDControlItem::SetLabelL(const TDesC& aLabel)
{
    CMIDItem::SetLabelL(aLabel);
    if (iLabelControl && iLabel)
    {
        iLabelControl->SetTextL(*iLabel);
    }
}

void CMIDControlItem::SetContainerWindowL(const CCoeControl& aContainer)
{
    CCoeControl::SetContainerWindowL(aContainer);
    if (iLabelControl)
    {
        iLabelControl->SetContainerWindowL(*this);
    }
}

void CMIDControlItem::SetFocus(TBool aFocus, TDrawNow aDrawNow)
{
    CCoeControl::SetFocus(aFocus, aDrawNow);
}

void CMIDControlItem::FocusChanged(TDrawNow aDrawNow)
{
    if (!iForm)
    {
        return;
    }

    SetLabelColor(iLabelControl);

    if (aDrawNow && DrawableWindow())
    {
        iLabelControl->DrawNow();
    }
}

TTypeUid::Ptr CMIDControlItem::MopSupplyObject(TTypeUid aId)
{
    if (aId.iUid == MAknsControlContext::ETypeId && iForm)
    {
        if (iHighlighted)
        {
            // Highlighted background control context must be returned
            // no matter if item is focused or not. Only then highlighted item
            // will be displayed correctly, if Form is faded
            // (e.g. Menu is shown).
            return MAknsControlContext::SupplyMopObject(aId,
                    iForm->GetHighlightBackgroundCc());
        }
        else
        {
            return MAknsControlContext::SupplyMopObject(aId,
                    iForm->CurrentDisplayable().BackGroundControlContext());
        }
    }
    return SupplyMopObject(aId, iMenuHandler->Cba(), iMenuHandler->MenuBar());
}

void CMIDControlItem::GetCaptionForFep(TDes& aCaption) const
{
    const TDesC* labelText = iLabelControl->Text();
    if (labelText)
    {
        const TInt maximumLength = aCaption.MaxLength();
        if (labelText->Length() > maximumLength)
        {
            aCaption = labelText->Left(maximumLength);
        }
        else
        {
            aCaption = *labelText;
        }
    }
    else
    {
        aCaption = KNullDesC;
    }
}

void CMIDControlItem::SetLabelColor(CMIDItemLabel* aLabelControl)
{
    TRgb color;

    // Set color for label text according to item highlight
    // (logical color constants are defined in lcdui.h)
    TInt labelColor = (iHighlighted
                       && Type() != MMIDComponent::ECustomItem
                       && Type() != MMIDComponent::EImageItem)
                      ? KHighlightedItemTextColor : KNonHighlightedItemTextColor;

    // Get color from skin
    if ((AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), color,
                                   KAknsIIDQsnTextColors, labelColor) == KErrNone) && iLabelControl)
    {//data entry text color
        TRAP_IGNORE(aLabelControl->SetColorL(color));
    }
}

TInt CMIDControlItem::LabelHeight() const
{
    if (iLabelControl && iLabelControl->Text()->Length())
    {
        return iLabelControl->Size().iHeight;
    }

    return 0;
}

/** Returns height of a label that has only one line of text */
TInt CMIDControlItem::OneLineLabelHeight() const
{
    return iLabelControl->LineHeight() + iLabelControl->ItemLabelMargin();
}

void CMIDControlItem::ResolutionChange(TInt /*aType*/)
{

}

void CMIDControlItem::ColorChange(TInt /*aType*/)
{

}

TBool CMIDControlItem::HasLabel()
{
    return iLabelControl && (iLabelControl->Text()->Length() > 0);
}

void CMIDControlItem::PostFocusTransferEvent(TBool /*aFocus*/, CMIDForm::TDirection /*aDirection*/)
{
    // override the default implementation if needed
}

void CMIDControlItem::RestoreInnerFocus()
{
    // override the default implementation if needed
}

void CMIDControlItem::HandleWindowFade(TBool /*aFaded*/)
{
    // override the default implementation if needed
}
