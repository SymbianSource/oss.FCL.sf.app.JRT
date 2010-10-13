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


#include <eikcapc.h>
//
#include "CMIDTextFieldItem.h"
// CMIDEdwin API for iTextField and CMIDTextFieldEdwin::NewL
#include "CMIDTextFieldEdwin.h"
// API for iLabelControl
#include "CMIDItemLabel.h"
// CAknEdwinDrawingModifier API for iDrawingModifier
#include <aknedwindrawingmodifier.h>

// LAF in UpdateLayout
#include <aknlayoutscalable_avkon.cdl.h>
// information to decorate text
#include <AknTextDecorationMetrics.h>

#include <j2me/jdebug.h>

/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDTextFieldItem.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);


MMIDTextField* CMIDTextFieldItem::NewL(
    const TDesC& aLabel, const TDesC& aText, TInt aConstraints,
    TInt aMaxSize, CMIDUIManager* aUIManager)
{
    CMIDTextFieldItem* textFieldItem = new(ELeave)CMIDTextFieldItem(aUIManager);
    CleanupStack::PushL(textFieldItem);
    textFieldItem->ConstructL(aLabel,aText,aConstraints,aMaxSize);
    CleanupStack::Pop(textFieldItem);
    return textFieldItem;
}

CMIDTextFieldItem::~CMIDTextFieldItem()
{
    if (iText)
    {
        delete iText;
        iText = NULL;
    }
    if (iTextField)
    {
        delete iTextField;
        iTextField = NULL;
    }
    if (iDrawingModifier)
    {
        delete iDrawingModifier;
        iDrawingModifier = NULL;
    }
}

void CMIDTextFieldItem::DeleteTextL(TInt aOffset,TInt aLength)
{
    TextComponent()->DeleteTextL(aOffset,aLength);
}

void CMIDTextFieldItem::SetTextL(const TDesC& aText)
{
    ASSERT(iTextField);
    // Position of cursor is temporally set to start of text.
    // This is prevetion of panic caused cursor posited off rectagle of CMIDTextField.
    iTextField->SetCursorPosL(0, EFalse);
    iTextField->SetTextL(aText);
    // Setting position to right position.
    iTextField->SetCursorPosL(aText.Length(), EFalse);
}

void CMIDTextFieldItem::InsertTextL(const TDesC& aText,TInt aPosition)
{
    TextComponent()->InsertTextL(aText,aPosition);
}

void CMIDTextFieldItem::SetConstraintsL(TUint aConstraints)
{
    HBufC* text = iTextField->GetTextL();
    CleanupStack::PushL(text);
    TInt maxSize = iTextField->GetMaxSize();

    delete iTextField;
    iTextField = NULL;

    CreateTextFieldL(KNullDesC, aConstraints, maxSize);
    iTextField->SetTextWithNewConstraintsL(text);

    if (iForm && IsFocused())
    {//in case non-midlet cmds have been added or removed
        iForm->CurrentDisplayable().InitializeCbasL();
    }

    if (iForm)
    {
        // the newly created textbox needs to be layouted, request layout for the whole form
        // as applying new constraints may change the size of the textfield
        iForm->RequestLayoutL();
    }

    CleanupStack::PopAndDestroy(text);
}

void CMIDTextFieldItem::CreateTextFieldL(const TDesC& aText, TUint aConstraints, TInt aMaxSize)
{
    ASSERT(!iTextField);

    iTextField = CMIDTextFieldEdwin::NewL(aConstraints, aText, aMaxSize, NULL, this);
    iTextField->SetFocusing(ETrue);
    iTextField->SetEdwinSizeObserver(this);
    iTextField->SetEdwinObserver(this);

    // Get cursor position set by CMIDEdwin::NewL. This is needed since SetContainerWindowL
    // and ActivateL calls lead into cursor position resetting, i.e. setting it to 0 pos.
    TInt cursorPos = iTextField->CursorPos();

    if (IsFocused())
    {
        iTextField->SetFocus(ETrue);
    }

    if (DrawableWindow())
    {
        // do not set container window if the fieldItem does not have CCoeControl::iWin set
        iTextField->SetContainerWindowL(*this);
    }

    if (IsActivated())
    {
        iTextField->ActivateL();
    }

    // Restore original cursor position
    iTextField->SetCursorPosL(cursorPos, EFalse);
}

void CMIDTextFieldItem::UpdateLayout()
{
    TAknWindowComponentLayout textPaneLayout =
        AknLayoutScalable_Avkon::form2_midp_text_pane(0);

    TAknTextComponentLayout textLayout =
        AknLayoutScalable_Avkon::form2_mdip_text_pane_t1(0);

    // set the top margin for Edwin
    iEdwinMarginTop = textPaneLayout.LayoutLine().it;

    // set the minimum height of the Edwin (one line text)
    TAknTextDecorationMetrics decorationMetrics(textLayout.LayoutLine().iFont);
    TInt topMargin, bottomMargin;

    decorationMetrics.GetTopAndBottomMargins(topMargin, bottomMargin);

    iEdwinMinHeight = textPaneLayout.LayoutLine().iH + topMargin + bottomMargin;
}


TInt CMIDTextFieldItem::SetMaxSizeL(TInt aMaxSize)
{
    return TextComponent()->SetMaxSizeL(aMaxSize);
}

TInt CMIDTextFieldItem::GetMaxSize()
{
    return TextComponent()->GetMaxSize();
}

TInt CMIDTextFieldItem::Size()
{
    return TextComponent()->Size();
}

TInt CMIDTextFieldItem::GetCaretPosition()
{
    return TextComponent()->GetCaretPosition();
}

HBufC* CMIDTextFieldItem::GetTextL()
{
    return TextComponent()->GetTextL();
}

void CMIDTextFieldItem::SetInitialInputModeL(const TDesC& aCharacterSubset)
{
    TextComponent()->SetInitialInputModeL(aCharacterSubset);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDTextFieldItem::CMIDTextFieldItem(CMIDUIManager* aUIManager)
        : CMIDControlItem(EDefault, aUIManager)
{
    iMMidItem = this;
    SetFocusing(ETrue);
}

/** Construct the item.

We chose to inhibit redrawing of the edwin on reformat,
see CEikEdwin::OnReformatL() - this is called when laying-out the edwin.
This is done via iDrawingModifier. This avoids drawing of edwin in the wrong
position when modifying the form, or changing focus between items and resizing
the item when inserting or removing a line.
*/
void CMIDTextFieldItem::ConstructL(const TDesC& aLabel,const TDesC& aText,
                                   TInt aConstraints, TInt aMaxSize)
{
    CMIDControlItem::ConstructL();

    TInt formWidth = FormClientAreaWidth();
    SetSize(TSize(formWidth, 0));

    SetLabelL(aLabel);
    iLabelControl->SetWidthL(formWidth);

    iDrawingModifier = new(ELeave)CAknEdwinDrawingModifier();
    iDrawingModifier->SetInhibitNotifyNewFormatDrawing(ETrue);

    CreateTextFieldL(aText, aConstraints, aMaxSize);

    UpdateLayout();
}

CMIDEdwin* CMIDTextFieldItem::TextComponent() const
{
    return iTextField;
}

/** The minimum size is usually the full form width and the height of the label, plus
    the minimum height of the editor (usually one line) plus 3 times the vertical margin. */
TSize CMIDTextFieldItem::MinimumSize()
{
    TInt minHeight =
        iEdwinMarginTop +
        iEdwinMinHeight +
        ItemContentBottomMargin();

    if (iLabelControl && iLabelControl->Text()->Length())
        minHeight += OneLineLabelHeight();

    return TSize(FormClientAreaWidth(), minHeight);
}

TInt CMIDTextFieldItem::CountComponentControls() const
{
    return 2;
}

CCoeControl* CMIDTextFieldItem::ComponentControl(TInt aIndex) const
{
    switch (aIndex)
    {
    case 0:
        return iLabelControl;
    case 1:
        return iTextField;
    }
    return NULL;
}

void CMIDTextFieldItem::Draw(const TRect& aRect) const
{
    CMIDControlItem::Draw(aRect);
}

void CMIDTextFieldItem::SizeChanged()
{
    TPoint position = Position();

    TRect labelRect(position, TSize(FormClientAreaWidth(), LabelHeight()));

    TPoint textTl = TPoint(position.iX,
                           LabelHeight() > 0 ? labelRect.iBr.iY : position.iY);
    TPoint textBr = Rect().iBr - TPoint(0, ItemContentBottomMargin());

    TRect textRect(textTl, textBr);

    if (LabelHeight() > 0)
    {
        iLabelControl->SetRect(labelRect);
    }

    if (iTextField)
    {
        iTextField->DoLayout(textRect);
    }

    CMIDControlItem::SizeChanged();
}

void CMIDTextFieldItem::SetContainerWindowL(const CCoeControl& aContainer)
{
    CMIDControlItem::SetContainerWindowL(aContainer);

    // Save the original cursor position. This is needed since SetContainerWindowL
    // and ActivateL call lead into cursor position resetting, i.e. setting it to 0 pos.
    TInt cursorPos = iTextField->CursorPos();

    iTextField->SetContainerWindowL(*this);

    SetObserver(iForm);

    ActivateL();

    // Restore original cursor position
    iTextField->SetCursorPosL(cursorPos, EFalse);

}

void CMIDTextFieldItem::FocusChanged(TDrawNow aDrawNow)
{
    TBool cursorVisibility = EFalse;
    if (IsFocused())
    {
        // Setting focus to iTextField changes the cursor visibility. This might cause
        // a cursor flashing on the screen, if the focus is set to a TF that is not in the
        // visible form area currently when a form is switched from background
        // to foreground. To prevent this EAvkonDisableCursor is set temporarily.
        iTextField->AddFlagToUserFlags(CEikEdwin::EAvkonDisableCursor);
        iTextField->SetFocus(ETrue);
        iTextField->RemoveFlagFromUserFlags(CEikEdwin::EAvkonDisableCursor);
        cursorVisibility = IsVisible();
    }
    else
    {
        iTextField->SetFocus(EFalse);
        cursorVisibility = EFalse;
    }

    CMIDControlItem::FocusChanged(aDrawNow);
    // DoLayout and change text color when focused
    SizeChanged();
    TRAP_IGNORE(UpdateTextColorsL());

    SetCursorVisibility(cursorVisibility);
}

void CMIDTextFieldItem::HandleCurrentL(TBool aCurrent)
{
    // set initial input modes when TextField becomes the focused item
    iTextField->HandleCurrentL(aCurrent);
}

void CMIDTextFieldItem::ResolutionChange(TInt /*aType*/)
{
    UpdateLayout();
}

void CMIDTextFieldItem::PostFocusTransferEvent(TBool aFocus, CMIDForm::TDirection /*aDirection*/)
{
    TRAP_IGNORE(HandleCurrentL(aFocus));
}

/** Basically pass the key event to the text box. Also check if the text has been
modified and if so report an item state change event. Attention must be taken here
because the text box may eat the EEventKey type so we must store the existing text
in the key down event and compare it in the key up event. */
TKeyResponse CMIDTextFieldItem::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{

    if (aType == EEventKeyDown)
    {
        delete iText;
        iText = NULL;

        iText = iTextField->GetTextInHBufL();
    }
    return iTextField->OfferKeyEventL(aKeyEvent, aType);
}

#ifdef RD_SCALABLE_UI_V2
void CMIDTextFieldItem::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
        if (!iForm->PhysicsScrolling() && !iTextField->IsReadOnly())
        {
            SetCursorVisibility(ETrue);
        }

        //update focus
        if (!iForm->TryDetectLongTapL(aPointerEvent))
        {
#ifdef RD_JAVA_S60_RELEASE_9_2
            // In single click UI VKB is opened with first tap.
            if (!iForm->PhysicsScrolling())
#else
            // In non-single click UI,
            // VKB is opened when tapping already focused item.
            if (!iForm->IsFocusChangingWithPen() && IsFocused() && !iForm->PhysicsScrolling())
#endif // RD_JAVA_S60_RELEASE_9_2
            {
                TPointerEvent pEvent = aPointerEvent;
                // If label area is tapped the coordinates are transferred to editor area.
                // By doing coordinate switching VKB is launched also when label is tapped.
                if (iLabelControl->Rect().Contains(pEvent.iPosition))
                {
                    pEvent.iPosition.iY = iTextField->Rect().iTl.iY;
                }
                CCoeControl::HandlePointerEventL(pEvent);
            }
            //else do not forward pointer event to CCoeControl as do not want to have any action
            //on unfocused TextField.
        }
    }
}
#endif // RD_SCALABLE_UI_V2

void CMIDTextFieldItem::MakeVisible(TBool aVisible)
{
    CMIDControlItem::MakeVisible(aVisible);
    SetCursorVisibility(aVisible);
}

TSize CMIDTextFieldItem::ResetPreferredSize() const
{
    CMIDTextFieldItem* self = const_cast<CMIDTextFieldItem*>(this);
    TRAP_IGNORE(self->SetPreferredSizeL(iRequestedPreferredSize));
    return iPreferredSize;
}

/** This is called before the form is re-laid-out. See ResetPreferredSize().
    If we have a requested height we select the max between this height and our
    minimum height. The width is always the form or screen width. Then we do layout.

    If we don't have a requested height, we do layout with a negative height
    (-1) and this signals to CMIDEdwin to use whatever height correcsponds to the
    number of formatted lines. See CMIDEdwin::DoLayout(). So, in this case iPreferredSize
    is set after doing layout.

    The editor can be resized in HandleEdwinSizeEventL() only if the preferred height has
    not been given.

    @note SetPreferredSize() always sets TextField's preferred size
    (i.e. how much space it needs so all control (label+text) is visible)
*/
void CMIDTextFieldItem::SetPreferredSizeL(const TSize& aSize)
{
    if (!iTextField)
        return;

    iRequestedPreferredSize = CheckRequestedSize(aSize);
    iPreferredSize = iRequestedPreferredSize;

    iPreferredSize.iWidth = FormClientAreaWidth(); //This is our fixed width, no less, no more

    TSize editorSize(iPreferredSize);

    // calculate the preferred size case
    TRect rect(TPoint(0,0), editorSize);
    iTextField->DoLayout(rect);

    iPreferredSize.iHeight = LabelHeight() + ItemPreferredHeightWithoutLabel();
}

TInt CMIDTextFieldItem::ItemPreferredHeightWithoutLabel()
{
    TInt height = ((CCoeControl*)iTextField)->Size().iHeight +
                  iEdwinMarginTop + ItemContentBottomMargin();

    return height;
}

TRect CMIDTextFieldItem::FocusableRect()
{
    return iTextField->Rect();
}

/** We set the new height of the textbox and form item by calling SetSize(). If we
are inside a form (which we most likely are) we then call CMIDForm::RefreshItemL(this).
This will take care of re-positioning all other items and updating the scroll bars.

We will not resize the editor if the use has specified a preferrred height
(iRequestedPreferredSize.iHeight) in which case the editor must be either this height
or the minimum height. Also, if iPreferredSize.iHeight is -1, it means we are inside
a SetPreferredSizeL() call, so we mustn't call  iForm->RequestLayoutL() because this
method ends up calling SetPreferredSizeL() and so we end up in infinite recursive calls.
So iPreferredSize.iHeight == -1 means do not do anything basically. */
TBool CMIDTextFieldItem::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aEventType*/, TSize aDesirableEdwinSize)
{
    if (iTextField->IsPartialFormatting())
    {
        // ignore the asynchronous HandleEdwinSizeEvents (e.g TextLength() > 1900 chars)
        // that will occur much later when Edwin finishes formatting.
        // This is a protection against requesting form layout while form might be scrolling.
        // In this partial formatting mode, the preferred size reflects only those number of lines
        // that were reformatted when quering for the preferred size.
        // This way, TextFieldItem will continue to have the preferred size (which does not show all visible lines).

        return EFalse;
    }

    if (iPreferredSize.iHeight != -1)
    {
        TInt desiredHeight =
            LabelHeight() +
            iEdwinMarginTop +
            aDesirableEdwinSize.iHeight +
            ItemContentBottomMargin();

        if (iPreferredSize.iHeight != desiredHeight)
        {
            if (iForm)
            {//new layout for form, hence new edwin size will be taken into account
                iForm->RequestLayoutL(ETrue);
            }
        }
    }

    return EFalse;
}


TTypeUid::Ptr CMIDTextFieldItem::MopSupplyObject(TTypeUid aId)
{
    // Return the edwin drawing modifier, CAknEdwinDrawingModifier, if ID matches, see
    // HandleEdwinSizeEventL(). Otherwise call CMIDControlItem::MopSupplyObject().
    if (aId.iUid == CAknEdwinDrawingModifier::ETypeId)
        return aId.MakePtr(iDrawingModifier);

    return CMIDControlItem::MopSupplyObject(aId);
}

void CMIDTextFieldItem::HandleEdwinEventL(CEikEdwin* /*aEdwin*/,TEdwinEvent /*aEventType*/)
{
    DoHandleEdwinEventL();
}

void CMIDTextFieldItem::HandleControlEventL(CCoeControl* /*aSource*/,
        MCoeControlObserver::TCoeEvent aEventType)
{
    if (aEventType == MCoeControlObserver::EEventStateChanged)
    {
        DoHandleEdwinEventL();

        // check if text has changed and eventually notify Java via
        // itemStateChanged()
        HBufC* newText = iTextField->GetTextInHBufL();
        TBool textChanged = EFalse;

        if ((!iText && newText) || (iText && !newText) ||
                (iText && newText && (*iText != *newText)))
        {
            textChanged = ETrue;
        }

        delete iText;
        iText = newText;

        if (textChanged)
        {
            ReportEventL(MCoeControlObserver::EEventStateChanged);
        }
    }
    else if (aEventType == MCoeControlObserver::EEventRequestFocus)
    {
        static_cast<MCoeControlObserver*>(iForm)->HandleControlEventL(this, aEventType);
    }
}

void CMIDTextFieldItem::DoHandleEdwinEventL()
{
    TPoint point;
    TBool success = GetCursorPosL(point);
    if (success && iForm)
    {
        if (point.iY < 0)
        {
            iForm->ScrollRelative(-(point.iY - 10));
            iForm->UpdatePhysics();
        }
        else if (point.iY >= iForm->Height())
        {
            iForm->ScrollRelative((iForm->Height() - point.iY) - 10);
            iForm->UpdatePhysics();
        }
    }
}

TBool CMIDTextFieldItem::GetCursorPosL(TPoint& aPoint)
{
    TInt pos = iTextField->CursorPos();
    CTextView* tv = iTextField->TextView();
    return tv->DocPosToXyPosL(pos, aPoint);
}

void CMIDTextFieldItem::SetLabelL(const TDesC& aLabel)
{
    CMIDControlItem::SetLabelL(aLabel);
}

void CMIDTextFieldItem::Dispose()
{
    delete this;
}

void CMIDTextFieldItem::SetLayoutL(TLayout aLayout)
{
    CMIDItem::SetLayoutL(aLayout);
}

void CMIDTextFieldItem::AddCommandL(MMIDCommand* aCommand)
{
    CMIDItem::AddCommandL(aCommand);
}

void CMIDTextFieldItem::RemoveCommand(MMIDCommand* aCommand)
{
    CMIDItem::RemoveCommand(aCommand);
}

void CMIDTextFieldItem::SetDefaultCommand(MMIDCommand* aCommand)
{
    CMIDItem::SetDefaultCommand(aCommand);
}

TSize CMIDTextFieldItem::PreferredSize() const
{
    return iPreferredSize;
}

TSize CMIDTextFieldItem::MinimumSize() const
{
    CCoeControl* control = const_cast<CMIDTextFieldItem*>(this);
    return control->MinimumSize();
}

TCoeInputCapabilities CMIDTextFieldItem::InputCapabilities() const
{
    TCoeInputCapabilities inputCapabilities(TCoeInputCapabilities::ENone, NULL,
                                            const_cast<CMIDTextFieldItem*>(this));
    inputCapabilities.MergeWith(iTextField->InputCapabilities());
    return inputCapabilities;
}

void CMIDTextFieldItem::RestoreInnerFocus()
{
    // Restore active textfield line (with cursor) on screen by resetting
    // the text selection and the cursor position
    if (iTextField)
    {
        TCursorSelection sel = iTextField->Selection();
        // Exceptions can be ignored, because failing of SetSelectionL may cause
        // only a cosmetic visual problems
        TRAP_IGNORE(iTextField->SetSelectionL(sel.iCursorPos, sel.iAnchorPos));
    }
}

void CMIDTextFieldItem::SetCursorVisibility(TBool aVisible)
{
    if (IsFocused())
    {
        TCursor::TVisibility textCursor =
            aVisible ? TCursor::EFCursorFlashing : TCursor::EFCursorInvisible;

        if (iTextField->IsReadOnly())
        {
            textCursor = TCursor::EFCursorInvisible;
        }

        // lineCursor is not used in TextField, so it is set to TCursor::EFCursorInvisible always
        TRAP_IGNORE(iTextField->TextView()->SetCursorVisibilityL(TCursor::EFCursorInvisible,
                    textCursor));
    }
}

void CMIDTextFieldItem::UpdateTextColorsL()
{
    if (iTextField && !iTextField->IsReadOnly())
    {
        // Set color for content text according to item highlight
        // (logical color constants are defined in lcdui.h)
        if (iHighlighted)
        {
            // Text colour from skin - highlighted
            iTextField->SetTextSkinColorIdL(KHighlightedItemTextColor);
        }
        else
        {
            // Text colour from skin - unfocused
            iTextField->SetTextSkinColorIdL(KNonHighlightedItemTextColor);
        }
    }
}
