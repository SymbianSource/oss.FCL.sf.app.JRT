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
* Description:  ?Description
*
*/


// ========== INCLUDE FILES ================================

#include <e32def.h>
#include <aknenv.h>
#include <aknappui.h>
// using Layout_Meta_Data::IsLandscapeOrientation for retrieving info about orientation
#include <layoutmetadata.cdl.h>
// LAF
#include <aknlayoutscalable_avkon.cdl.h>

#include <lcdui.rsg>
#include <j2me/jdebug.h>

#include "CMIDTextBoxControl.h"
// used when creating a new text box
#include "CMIDTextBoxEdwin.h"
#include "CMIDTextBoxEdwinCustomDraw.h"
#include "CMIDCommand.h"


// ========== MEMBER FUNCTIONS =============================

CMIDTextBoxControl* CMIDTextBoxControl::NewL(TInt aConstraints,
        const TDesC& aText, TInt aMaxSize,MMIDDisplayable* aDisplayable)
{
    CMIDTextBoxControl* self=new(ELeave) CMIDTextBoxControl();
    CleanupStack::PushL(self);
    self->ConstructL(aConstraints, aText, aMaxSize, aDisplayable);
    CleanupStack::Pop(self);

    return self;
}

CMIDTextBoxControl::CMIDTextBoxControl()
{
}

void CMIDTextBoxControl::ConstructL(TInt aConstraints, const TDesC& aText,
                                    TInt aMaxSize, MMIDDisplayable* aDisplayable)
{
    iDisplayable = static_cast<CMIDDisplayable*>(aDisplayable);

    SetContainerWindowL(*iDisplayable);

    iTextBox = CreateTextBoxL(aText, aMaxSize, aConstraints);
    iConstraints=aConstraints;
    iCursorPos = iTextBox->CursorPos();

    SetRect(iDisplayable->Rect());
    aDisplayable->SetComponentL(*this); //will call ActivateL() on us
}

CMIDTextBoxControl::~CMIDTextBoxControl()
{
    delete iTextBox;

    // Displayable is notified about content control deletion
    if (iDisplayable)
    {
        iDisplayable->NotifyContentDestroyed();
    }

}

//
// methods from MMIDTextBox
//

void CMIDTextBoxControl::DeleteTextL(TInt aOffset,TInt aLength)
{
    iTextBox->DeleteTextL(aOffset, aLength);
}

void CMIDTextBoxControl::SetTextL(const TDesC& aText)
{
    iTextBox->SetTextL(aText);
}

void CMIDTextBoxControl::InsertTextL(const TDesC& aText,TInt aPosition)
{
    iTextBox->InsertTextL(aText, aPosition);
}

void CMIDTextBoxControl::SetConstraintsL(TUint aConstraints)
{
    iConstraints=aConstraints;
    TBool convNeededOld = iTextBox->IsNumberConversionNeeded();
    RecreateTextBoxL(aConstraints);
    TBool convNeededNew = iTextBox->IsNumberConversionNeeded();
    if (convNeededNew != convNeededOld)
    {
        HBufC* text = iTextBox->GetTextL();
        TPtr ptr = text->Des();
        AknTextUtils::LanguageSpecificNumberConversion(ptr);
        CleanupStack::PushL(text);
        iTextBox->SetTextL(ptr);
        CleanupStack::PopAndDestroy(text);
    }
}

TInt CMIDTextBoxControl::SetMaxSizeL(TInt aSize)
{
    return iTextBox->SetMaxSizeL(aSize);
}

TInt CMIDTextBoxControl::GetMaxSize()
{
    return iTextBox->GetMaxSize();
}

TInt CMIDTextBoxControl::Size()
{
    return iTextBox->Size();
}

TInt CMIDTextBoxControl::GetCaretPosition()
{
    return iTextBox->GetCaretPosition();
}

HBufC* CMIDTextBoxControl::GetTextL()
{
    return iTextBox->GetTextL();
}

void CMIDTextBoxControl::SetInitialInputModeL(const TDesC& aCharacterSubset)
{
    iTextBox->SetInitialInputModeL(aCharacterSubset);
}

//
// methods from CCoeControl
//

TInt CMIDTextBoxControl::CountComponentControls() const
{
    return 1;  //  iTextBox
}

CCoeControl* CMIDTextBoxControl::ComponentControl(TInt aIndex) const
{
    if (aIndex == 0)
    {
        return iTextBox;
    }
    return NULL;
}

TKeyResponse CMIDTextBoxControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    //
    //  Enter Key
    //

    if (aType == EEventKey)
    {
        iUsedKeyEvent = &aKeyEvent;
    }

    // If TextBox have constraints below params
    // Any and Password
    // Numeric or Numeric and Password or
    // Decimal or Decimal and Password  or
    // PhoneNumber or PhoneNumber and Password or
    // MailAddr or MailAddr and Password or
    // PhoneNumber Uneditable or MailAddr Uneditable or
    // Url or Url and Password
    // then show menu or activate one command.
    if (((aType == EEventKey && aKeyEvent.iCode == EKeyEnter) ||
            (aType == EEventKeyUp && aKeyEvent.iScanCode ==   EStdKeyEnter &&
             aKeyEvent.iCode == EKeyEnter && iUsedKeyEvent!=&aKeyEvent)))
    {

        if ((iConstraints == MMIDTextField::EAny + MMIDTextField::EPassword) ||

                (iConstraints == MMIDTextField::ENumeric ||
                 (iConstraints == MMIDTextField::ENumeric  + MMIDTextField::EPassword) ||
                 iConstraints == MMIDTextField::EDecimal ||
                 (iConstraints == MMIDTextField::EDecimal + MMIDTextField::EPassword)) ||

                (iConstraints == MMIDTextField::EPhoneNumber ||
                 (iConstraints == MMIDTextField::EPhoneNumber + MMIDTextField::EPassword) ||
                 iConstraints == MMIDTextField::EMailAddr ||
                 (iConstraints == MMIDTextField::EMailAddr  + MMIDTextField::EPassword))  ||

                ((iConstraints == MMIDTextField::EPhoneNumber + MMIDTextField::EUneditable) ||
                 (iConstraints == MMIDTextField::EMailAddr  + MMIDTextField::EUneditable)) ||

                (iConstraints == MMIDTextField::EUrl ||
                 (iConstraints == MMIDTextField::EUrl + MMIDTextField::EPassword)))
        {

            // counter OK and Item elements from options menu
            TInt numOpt = iDisplayable->NumCommandsForOkOptionsMenu();

            //open menu
            if (numOpt > 1)
            {
                iDisplayable->MenuHandler()->ShowMenuL(CMIDMenuHandler::EOkMenu);
            }
            else
            {
                CMIDCommand* cmdTmp = NULL;

                // counter of all elements from options menu
                TInt numItemCommands = iDisplayable->MainCommandList()->Count();

                for (TInt i = 0; i < numItemCommands; i++)
                {
                    // for each command from menu
                    // count offset index only for commands
                    // without FetchPhoneNumber, EmailAddress, PhoneCall
                    cmdTmp = iDisplayable->MainCommandList()->At(i).iCommand;

                    if (cmdTmp)
                    {
                        if ((cmdTmp->Id() != CMIDEdwinUtils::EMenuCommandFetchPhoneNumber) &&
                                (cmdTmp->Id() != CMIDEdwinUtils::EMenuCommandFetchEmailAddress) &&
                                (cmdTmp->Id() != CMIDEdwinUtils::EMenuCommandCreatePhoneCall))
                        {
                            break;
                        }
                    }
                }

                // execute command
                if (cmdTmp)
                {
                    if ((cmdTmp->CommandType() == MMIDCommand::EOk ||
                            cmdTmp->CommandType() == MMIDCommand::EItem ||
                            cmdTmp->CommandType() == MMIDCommand::EScreen))
                    {
                        // get command index from option menu
                        TInt offsetIdx = iDisplayable->MainCommandList()->FindCommandIndex(cmdTmp);

                        if (offsetIdx != KErrNotFound &&
                                iDisplayable->MainCommandList()->At(offsetIdx).iCommand == cmdTmp)
                        {
                            iDisplayable->ProcessCommandL(iDisplayable->MainCommandList()->CommandOffset() + offsetIdx);
                        }

                    }
                }
            }
            return EKeyWasConsumed;
        }
    }

    //
    //  End Enter Key
    //

    // msk: this should be obsolete, but is needed if MSK is not enabled on the device
    if ((aType == EEventKey) && (aKeyEvent.iScanCode == EStdKeyDevice3))
    {
        iDisplayable->ShowOkOptionsMenuL();
        return EKeyWasConsumed;
    }
    // end msk
    else
    {
        return iTextBox->OfferKeyEventL(aKeyEvent, aType);
    }
}

void CMIDTextBoxControl::FocusChanged(TDrawNow /*aDrawNow*/)
{
    if (iTextBox)
    {
        if (IsFocused())
        {
            iTextBox->SetFocus(ETrue);
        }
        else
        {
            iTextBox->SetFocus(EFalse);
        }
    }
}

void CMIDTextBoxControl::HandleCurrentL(TBool aActivate)
{
    iTextBox->HandleCurrentL(aActivate);
    if (!aActivate)
    {
        iCursorPos = iTextBox->CursorPos();
    }
}

void CMIDTextBoxControl::ActivateL()
{
    CCoeControl::ActivateL();
    // This is needed because CEikEdwin doesn't leave the cursor
    // at its current position when the CEikEdwin is re-activated.
    iTextBox->SetCursorPosL(iCursorPos, EFalse);
    SizeChanged(); // needed to be called explicitly to layout controls as ActivateL() will not call it if size has not changed
}

/**
 * @note SizeChanged() won't be called so often for CMIDTextBoxControl
 * (i.e. called when resolution changes)
 * No need to set the layout info as members of this class.
 * The control rect is without the left and right margins from
 * list_scroll_midp_pane.
 */
void CMIDTextBoxControl::SizeChanged()
{
    TAknWindowLineLayout listScrollPaneLayout =
        AknLayoutScalable_Avkon::listscroll_midp_pane(0).LayoutLine();

    TRect listScrollPaneRect = Rect();
    listScrollPaneRect.iTl.iX += listScrollPaneLayout.il;
    listScrollPaneRect.iBr.iX -= listScrollPaneLayout.ir;

    // the pane for displaying the actual Edwin, without scrollbar
    TAknLayoutRect edwinPane;
    edwinPane.LayoutRect(
        listScrollPaneRect,
        AknLayoutScalable_Avkon::list_midp_pane(0).LayoutLine());

    if (iTextBox)
    {
        // layout the scroll bar
        LayoutScrollBar(listScrollPaneRect);

        // layout the Edwin control
        iTextBox->DoLayout(edwinPane.Rect());
    }
}


void CMIDTextBoxControl::HandleResourceChange(TInt aType)
{
    CCoeControl::HandleResourceChange(aType);

    // Skin Changed
    if (aType == KEikColorResourceChange || aType == KAknsMessageSkinChange ||
            aType == KUidValueCoeColorSchemeChangeEvent)
    {
        CMIDTextBoxEdwinCustomDraw* customDraw = NULL;
        CMIDTextBoxEdwin* textBoxEdwin = reinterpret_cast<CMIDTextBoxEdwin*>(iTextBox);
        if (textBoxEdwin)
        {
            // Getting of CMIDTextBoxCustomDraw, which will be updated.
            customDraw = textBoxEdwin->GetCustomDraw();
        }

        if (customDraw)
        {
            // Updating of line icon in CMIDTextBoxCustomDraw.
            TRAPD(ignoreErr, customDraw->UpdateIconL());
            if (ignoreErr != KErrNone)
            {
                DEBUG_INT("CMIDTextBoxControl::HandleResourceChange - UpdateIconL leaved. Error = %d", ignoreErr);
            }
            // Calculating of layout, it is necessary for correct set of size
            // of line icon in CMIDTextBoxCustomDraw.
            SizeChanged();
        }
    }

    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        // relayout the control
        SizeChanged();
        // Changing system fonts height could also affect layouting of textbox.
        // Because this change doesn't affect rectangle, displayable is not
        // redrawn (see CMIDDisplayable::Layout()). So that redrawing has to be
        // initiated here.
        if (IsVisible())
        {
            DrawDeferred();
        }
    }
}


/** Create a new text box. This is called by ConstrctL() and by RecreateTextBoxL() */
CMIDEdwin* CMIDTextBoxControl::CreateTextBoxL(const TDesC& aText,
        TInt aMaxSize, TUint aConstraints)
{
    return CMIDTextBoxEdwin::NewL(aConstraints, aText, aMaxSize, iDisplayable);
}

/** Deletes the existing text box and recreates a new one. This is called by SetConstraints().
If we fail creating the new text box we make sure the old one is still
there. Makes sure the focus position is preserved. */
void CMIDTextBoxControl::RecreateTextBoxL(TUint aConstraints)
{
    ASSERT(iTextBox);

    HBufC* text = iTextBox->GetTextL();
    CleanupStack::PushL(text);

    TInt maxSize = iTextBox->GetMaxSize();
    iCursorPos = iTextBox->CursorPos();

    // TextBox's focus state is stored because after recreating
    // TextBox the focus information is lost.
    TBool hasFocus = IsFocused();

    CMIDEdwin* textBox = CreateTextBoxL(KNullDesC, maxSize, aConstraints);

    delete iTextBox;
    iTextBox = textBox;

    iTextBox->SetTextWithNewConstraintsL(text);
    CleanupStack::PopAndDestroy(text);

    iTextBox->HandleCurrentL(iDisplayable->IsActive());

    // Check if the text size has changed and reposition cursor if needed
    if (iCursorPos > iTextBox->TextLength())
    {
        iCursorPos = iTextBox->TextLength();
    }
    iTextBox->SetCursorPosL(iCursorPos, EFalse);

    // Activate new iTextBox with CoeControl
    ActivateL();

    // If TextBox had focus before recreation, the focus is restored.
    if (hasFocus)
    {
        iTextBox->SetFocus(ETrue);
    }

}

void CMIDTextBoxControl::LayoutScrollBar(const TRect& aRect)
{
    // get the Edwin's scrollbar
    CEikScrollBarFrame* sbFrame = iTextBox->ScrollBarFrame();

    TAknWindowLineLayout scrollPane =
        AknLayoutScalable_Avkon::scroll_pane_cp16(0).LayoutLine();

    if (sbFrame &&
            sbFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan)
    {
        AknLayoutUtils::LayoutVerticalScrollBar(sbFrame, aRect, scrollPane);
    }
}


// End of File
