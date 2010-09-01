/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Pop-up Textbox control implementation for S60
*
*/


#ifndef CMIDTEXTBOXDIALOGCONTROL_H
#define CMIDTEXTBOXDIALOGCONTROL_H

// INCLUDES
#include <lcdui.h>
#include <AknQueryDialog.h>
#include "CMIDDisplayable.h"
#include "CMIDTextBoxQueryDialog.h"

NONSHARABLE_CLASS(CMIDTextBoxDialogControl) : public CCoeControl, public MMIDTextBox
{
public:     // construction
    static CMIDTextBoxDialogControl* NewL(TInt aConstraints,
    const TDesC& aText,
    TInt aMaxSize,
    MMIDDisplayable* aDisplayable
                                         );
    ~CMIDTextBoxDialogControl();
private:
    void ConstructL(TInt aConstraints,
                    const TDesC& aText,
                    TInt aMaxSize,
                    MMIDDisplayable* aDisplayable);

    CMIDTextBoxDialogControl();

    /**
     * Creates and initializes the dialog.
     *
     * @param aDialog               pointer to the new dialog wihich is to be created
     * @param aConstraints          TextBox constraints
     * @param aMaxSize              TextBox max size
     * @param aText                 TextBox text content
     *
     * @return pointer to new CMIDTextBoxQueryDialog instance.
     *
     * @since S60 5.0
     */
    void CreateTextBoxQueryDialogL(CMIDTextBoxQueryDialog*& aDialog,
                                   TInt aConstraints,
                                   TInt aMaxSize,
                                   const TDesC& aText);

    /**
     * Recreates iDialog based on the existing dialog.
     * Needs to be called when text input contsraints of existing dialog is changed.
     *
     * @param aConstraints          TextBox Contsraints.
     * @since S60 5.0
     */
    void RecreateTextBoxL(TUint aConstraints);

public:
    /**
     * Called when pop-up TextBox either comes to current displayable,
     * or is removed from the screen. Adds or removes the dialog from control stack.
     *
     * @param aCurrent      Whether the TextBox is to shown or hided
     *
     * @since S60 5.0
     */
    void HandleCurrentL(TBool aCurrent);

    //
    // From MMIDTextBox
    // These functions are native counterparts of Java MIDP LCDUI TextBox methods.
    //
    /**
     * Deletes text.
     *
     * @param aOffset   the beginning of the region to be deleted
     * @param aLength   the number of characters to be deleted
     *
     * @since S60 5.0
     */
    void DeleteTextL(TInt aOffset,TInt aLength);

    /**
     * Sets the contents of the TextBox, replacing the previous contents.
     *
     * @param aText   the new value of the TextBox
     *
     * @since S60 5.0
     */
    void SetTextL(const TDesC& aText);

    /**
     * Inserts a string into the contents of the TextBox.
     *
     * @param aText   the text to be inserted in the TextBox
     * @param aPosition   the position at which insertion is to occur
     *
     * @since S60 5.0
     */
    void InsertTextL(const TDesC& aText,TInt aPosition);

    /**
     * Sets the input constraints of the TextBox.
     *
     * @param aConstraints   Input constraints of the TextBox
     *
     * @since S60 5.0
     */
    void SetConstraintsL(TUint aConstraints);

    /**
     * Sets the maximum size (number of characters) that can be contained in this TextBox.
     *
     * @param aSize          the new maximum size
     *
     * @return TInt          Indicates assigned maximum capacity - may be smaller than requested.
     *
     * @since S60 5.0
     */
    TInt SetMaxSizeL(TInt aSize);

    /**
     * Returns the maximum size (number of characters) that can be stored in this TextBox.
     *
     * @return TInt          the maximum size in characters
     *
     * @since S60 5.0
     */
    TInt GetMaxSize();

    /**
     * Gets the number of characters that are currently stored in this TextBox.
     *
     * @return TInt          the number of characters
     *
     * @since S60 5.0
     */
    TInt Size();

    /**
     * Gets the current input position.
     *
     * @return TInt          the current caret position, 0 if at the beginning
     *
     * @since S60 5.0
     */
    TInt GetCaretPosition();

    /**
     * Gets the contents of the TextBox as a string value.
     *
     * @return HBufC*          the current contents
     *
     * @since S60 5.0
     */
    HBufC* GetTextL();

    /**
     * Sets initial input mode for TextBox.
     *
     * @param aCharacterSubset         a string naming a Unicode character subset
     *
     * @since S60 5.0
     */
    void SetInitialInputModeL(const TDesC& aCharacterSubset);

    // From MMIDComponent
    //
    inline void Dispose();

    //
    // From CCoeControl
    //
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt) const;
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void FocusChanged(TDrawNow aDrawNow);
    void ActivateL();

    /**
     * Sets new title for TextBox dialog.
     *
     * @param aString         new title
     *
     * @since S60 5.0
     */
    void SetTitleL(const TDesC* aString);

    /**
     * Returns pointer to TextBox dialog.
     *
     * @return CMIDTextBoxQueryDialog*     pointer to TextBox dialog
     *
     * @since S60 5.0
     */
    CMIDTextBoxQueryDialog* Dialog();

private:
    /**
     * Stores pointer to text content of the TextBox
     */
    HBufC*                  iCurrentText;

    /**
     * Stores current cursor position of the TextBox
     */
    TInt                    iCursorPosition;

    /**
     * Pointer to Displayable
     */
    CMIDDisplayable*        iDisplayable;

    /**
     * Pointer to actual TextBox implementation
     */
    CMIDTextBoxQueryDialog* iDialog;

    /**
     * Needed when creating CAknTextQueryDialog
     */
    RBuf                    iTextContent;
};

inline void CMIDTextBoxDialogControl::Dispose()
{
    delete this;
}

#endif // CMIDTEXTBOXDIALOGCONTROL_H
