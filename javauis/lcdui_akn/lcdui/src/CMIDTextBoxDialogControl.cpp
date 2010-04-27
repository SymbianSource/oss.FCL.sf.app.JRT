/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Pop-up TextBox dialog control implementation for S60
*
*/

#include <eikenv.h>
#include "CMIDTextBoxDialogControl.h"

#include "lcdui.hrh"
#include <lcdui.rsg>
#include <EIKCOCTL.rsg>

#include <j2me/jdebug.h>


CMIDTextBoxDialogControl* CMIDTextBoxDialogControl::NewL(
    TInt aConstraints,
    const TDesC& aText,
    TInt aMaxSize,
    MMIDDisplayable* aDisplayable)
{
    CMIDTextBoxDialogControl* self =
        new(ELeave) CMIDTextBoxDialogControl();

    CleanupStack::PushL(self);
    self->ConstructL(aConstraints, aText, aMaxSize, aDisplayable);
    CleanupStack::Pop(self);

    return self;
}

CMIDTextBoxDialogControl::~CMIDTextBoxDialogControl()
{
    if (iDialog)
    {
        delete iDialog;
    }

    iTextContent.Close();
    delete iCurrentText;

    // Displayable is notified about content control deletion
    if (iDisplayable)
    {
        iDisplayable->NotifyContentDestroyed();
    }
}

void CMIDTextBoxDialogControl::ConstructL(
    TInt aConstraints,
    const TDesC& aText,
    TInt aMaxSize,
    MMIDDisplayable* aDisplayable)
{
    iDisplayable = static_cast< CMIDDisplayable* >(aDisplayable);
    iTextContent.CreateL(KNullDesC, aMaxSize);

    CreateTextBoxQueryDialogL(iDialog, aConstraints, aMaxSize, aText);

    iDisplayable->SetPopupTextBox(ETrue);
    SetRect(iDisplayable->Rect());
    iDisplayable->SetComponentL(*this);

    SetTextL(aText);
    SetContainerWindowL(*iDisplayable);

    iCurrentText = GetTextL();

    if (iCurrentText)
    {
        iCursorPosition = iCurrentText->Length();
    }
    else
    {
        iCursorPosition = 0;
    }
}

CMIDTextBoxDialogControl::CMIDTextBoxDialogControl()
{
}

void CMIDTextBoxDialogControl::CreateTextBoxQueryDialogL(
    CMIDTextBoxQueryDialog*& aDialog,
    TInt aConstraints, TInt aMaxSize,
    const TDesC& aText)
{
    CMIDTextBoxQueryDialog* dialog = CMIDTextBoxQueryDialog::NewLC(
                                         aConstraints,
                                         iTextContent,
                                         aMaxSize,
                                         iDisplayable,
                                         aText);

    dialog->MakeVisible(EFalse);
    dialog->SetEditorL();
    aDialog = dialog;
    CleanupStack::Pop(dialog);
}

/** Deletes the existing text box dialog and recreates a new one. This is called by SetConstraints().
If we fail creating the new text box we make sure the old one is still
there. Makes sure the focus position is preserved. */
void CMIDTextBoxDialogControl::RecreateTextBoxL(TUint aConstraints)
{
    ASSERT(iDialog);
    HandleCurrentL(EFalse);
    HBufC* text = iDialog->GetTextL();
    CleanupStack::PushL(text);

    TInt maxSize = iDialog->GetMaxSize();
    iCursorPosition = iDialog->GetCaretPosition();

    // TextBox's focus state is stored because after recreating
    // TextBox the focus information is lost.
    TBool hasFocus = IsFocused();
    iTextContent.Close();
    iTextContent.CreateL(KNullDesC, maxSize);

    CMIDTextBoxQueryDialog* dialog = NULL;
    CreateTextBoxQueryDialogL(dialog, aConstraints, maxSize, KNullDesC);
    delete iDialog;
    iDialog = dialog;

    if (text)
    {
        iDialog->SetTextWithNewConstraintsL(text);
    }

    CleanupStack::PopAndDestroy(text);

    delete iCurrentText;
    iCurrentText = NULL;
    iCurrentText = GetTextL();

    // Check if the text size has changed and reposition cursor if needed
    if (iCursorPosition > iDialog->Size())
    {
        iCursorPosition = iDialog->Size();
    }

    // Activate new iTextBox with CoeControl
    ActivateL();

    // If TextBox had focus before recreation, the focus is restored.
    if (hasFocus)
    {
        iDialog->SetFocus(ETrue);
    }

    iDialog->SetTitleL(iDisplayable->Title());
    HandleCurrentL(iDisplayable->IsActive());
}

void CMIDTextBoxDialogControl::HandleCurrentL(TBool aCurrent)
{
    if (aCurrent)
    {
        iEikonEnv->EikAppUi()->AddToStackL(iDialog, ECoeStackPriorityMenu);

        // Set dialog cursor to zero and back again after it returns from
        // iDialog->ShowL.

        TInt curPosition = iCursorPosition;
        iCursorPosition = 0;
        iDialog->SetCursorPositionL(0);
        iDialog->ShowL(ETrue);

        if (iCurrentText)
        {
            SetTextL(*iCurrentText);
        }
        iCursorPosition = curPosition;
        // Set cursor position back to original value.
        iDialog->SetCursorPositionL(iCursorPosition);
    }
    else
    {
        delete iCurrentText;
        iCurrentText = NULL;
        iCurrentText = GetTextL();
        iCursorPosition = iDialog->GetCaretPosition();
        iDialog->ShowL(EFalse);
        iEikonEnv->EikAppUi()->RemoveFromStack(iDialog);
    }
}

//
// methods from MMIDTextBox
//

void CMIDTextBoxDialogControl::DeleteTextL(TInt aOffset, TInt aLength)
{
    if (iDialog)
    {
        iDialog->DeleteTextL(aOffset, aLength);
        iCursorPosition = GetCaretPosition();
        delete iCurrentText;
        iCurrentText = NULL;
        iCurrentText = GetTextL();
    }
}

void CMIDTextBoxDialogControl::SetTextL(const TDesC& aText)
{
    if (iDialog)
    {
        iDialog->SetTextL(aText);
        iCursorPosition = GetCaretPosition();
        delete iCurrentText;
        iCurrentText = NULL;
        iCurrentText = GetTextL();
    }
}

void CMIDTextBoxDialogControl::InsertTextL(
    const TDesC& aText, TInt aPosition)
{
    if (iDialog)
    {
        iDialog->InsertTextL(aText, aPosition);
        iCursorPosition = GetCaretPosition();
        delete iCurrentText;
        iCurrentText = NULL;
        iCurrentText = GetTextL();
    }
}

void CMIDTextBoxDialogControl::SetConstraintsL(TUint aConstraints)
{
    RecreateTextBoxL(aConstraints);
    delete iCurrentText;
    iCurrentText = NULL;
    iCurrentText = GetTextL();
}

TInt CMIDTextBoxDialogControl::SetMaxSizeL(TInt aSize)
{
    TInt maxSize = iDialog->SetMaxSizeL(aSize);
    iCursorPosition = GetCaretPosition();
    delete iCurrentText;
    iCurrentText = NULL;
    iCurrentText = GetTextL();
    return maxSize;
}

TInt CMIDTextBoxDialogControl::GetMaxSize()
{
    return iDialog->GetMaxSize();
}

TInt CMIDTextBoxDialogControl::Size()
{
    return iDialog->Size();
}

TInt CMIDTextBoxDialogControl::GetCaretPosition()
{
    return iDialog->GetCaretPosition();
}

HBufC* CMIDTextBoxDialogControl::GetTextL()
{
    return iDialog->GetTextL();
}

void CMIDTextBoxDialogControl::SetInitialInputModeL(
    const TDesC& aCharacterSubset)
{
    iDialog->SetInitialInputModeL(aCharacterSubset);
    delete iCurrentText;
    iCurrentText = NULL;
    iCurrentText = GetTextL();
}

TInt CMIDTextBoxDialogControl::CountComponentControls() const
{
    return 1;  //  iTextBox
}

CCoeControl* CMIDTextBoxDialogControl::ComponentControl(TInt aIndex) const
{
    if (aIndex == 0)
    {
        return iDialog;
    }

    return NULL;
}

TKeyResponse CMIDTextBoxDialogControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
{
    // If midlet is closed from applications menu or by pressing end key, then
    // 'esc' key event is generated. In that case we must close the dialog.
    if (aKeyEvent.iCode == EKeyEscape &&
            aKeyEvent.iModifiers == 0 &&
            aType == EEventKey)
    {
        HandleCurrentL(EFalse);

        return EKeyWasConsumed;
    }

    return EKeyWasNotConsumed;
}

void CMIDTextBoxDialogControl::FocusChanged(TDrawNow /*aDrawNow*/)
{
    if (iDialog)
    {
        if (IsFocused())
        {
            iDialog->SetFocus(ETrue);
        }
        else
        {
            iDialog->SetFocus(EFalse);
        }
    }
}

void CMIDTextBoxDialogControl::ActivateL()
{
    CCoeControl::ActivateL();

    // Cursor position must be set, because CEikEdwin doesn't leave the cursor
    // at its current position when the CEikEdwin is re-activated.
    iDialog->SetCursorPositionL(iCursorPosition);

    // needed to be called explicitly to layout controls as ActivateL() will not call it if size has not changed
    iDialog->SizeChanged();
}

void CMIDTextBoxDialogControl::SetTitleL(const TDesC* aString)
{
    if (aString)
    {
        iDialog->SetTitleL(aString);
    }
    else
    {
        iDialog->SetTitleL(&KNullTitle);
    }

    // By recreating the dialog we ensure that the layout is correct.
    // Recreation is not needed when dialog is visible.
    if (iDialog->Showing())
    {
        RecreateTextBoxL(iDialog->Constraints());
        delete iCurrentText;
        iCurrentText = NULL;
        iCurrentText = GetTextL();
    }
}

CMIDTextBoxQueryDialog* CMIDTextBoxDialogControl::Dialog()
{
    return iDialog;
}

// End of file
