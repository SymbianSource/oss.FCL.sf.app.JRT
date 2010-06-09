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
* Description:  Pop-up Textbox dialog implementation for S60
*
*/


#ifndef CMIDTEXTBOXQUERYDIALOG_H
#define CMIDTEXTBOXQUERYDIALOG_H

// INCLUDES

#include <lcdui.h>
#include <AknQueryDialog.h>
#include "CMIDDisplayable.h"

#include <eikedwob.h>
// for MCoeCaptionRetrieverForFep
#include <fepbase.h>

#include "CMIDEdwinUtils.h"

// CLASS DECLARATION
NONSHARABLE_CLASS(CMIDTextBoxQueryDialog)
        : public CAknTextQueryDialog, MMIDTextBox, public MMIDCommandObserver,
        public MEikEdwinObserver, public MCoeCaptionRetrieverForFep
{

public:
    static CMIDTextBoxQueryDialog* NewLC(TInt aConstraints,
    TDes& aResult,
    TInt aMaxSize,
    CMIDDisplayable* aDisplayable,
    const TDesC& aText);
    ~CMIDTextBoxQueryDialog();

private:
    void ConstructL(TInt aConstraints,
                    TInt aMaxSize,
                    CMIDDisplayable* aDisplayable,
                    const TDesC& aText);

    CMIDTextBoxQueryDialog(TDes& aResult);

public:
    //
    //From CAknTextQueryDialog
    //
    TInt RouseSleepingDialog();
    void ExitSleepingDialog();
    void TryExitL(TInt aButtonId);

    //
    // From MMIDCommandObserver
    //
    TBool ProcessCommandL(CMIDCommand* aCommand);

    //
    // From MMIDTextBox
    //
    /**
     * Deletes editor's text.
     *
     * @param aOffset   the beginning of the region to be deleted
     * @param aLength   the number of characters to be deleted
     *
     * @since S60 5.0
     */
    void DeleteTextL(TInt aOffset,TInt aLength);

    /**
     * Sets the contents of the editor in dialog, replacing the previous contents.
     *
     * @param aText   the new value of the editor
     *
     * @since S60 5.0
     */
    void SetTextL(const TDesC& aText);

    /**
     * Inserts a string into the contents of the editor.
     *
     * @param aText   the text to be inserted in the editor
     * @param aPosition   the position at which insertion is to occur
     *
     * @since S60 5.0
     */
    void InsertTextL(const TDesC& aText,TInt aPosition);

    /**
    * Sets constraints for the text editor
    *
    * @param aConstraints   new input constraints
    */
    void SetConstraintsL(TUint aConstraints);

    /**
     * Sets the maximum size (number of characters) that can be contained in editor.
     *
     * @param aSize          the new maximum size
     *
     * @return TInt          Indicates assigned maximum capacity - may be smaller than requested.
     *
     * @since S60 5.0
     */
    TInt SetMaxSizeL(TInt aSize);

    /**
     * Returns the maximum size (number of characters) that can be stored in editor.
     *
     * @return TInt          the maximum size in characters
     *
     * @since S60 5.0
     */
    TInt GetMaxSize();

    /**
     * Gets the number of characters that are currently stored in editor.
     *
     * @return TInt          the number of characters
     *
     * @since S60 5.0
     */
    TInt Size();

    /**
     * Gets the current input position of the editor.
     *
     * @return TInt          the current caret position, 0 if at the beginning
     *
     * @since S60 5.0
     */
    TInt GetCaretPosition();

    /**
     * Gets a new buffer containing a copy of the whole text of the editor.
     * Note: Caller is responsible of deleting the buffer.
     *
     * @return HBufC*          A new copy of the text.
     *
     * @since S60 5.0
     */
    HBufC* GetTextL();

    /**
     * Sets initial input mode for the editor.
     *
     * @param aCharacterSubset         a string naming a Unicode character subset
     *
     * @since S60 5.0
     */
    void SetInitialInputModeL(const TDesC& aCharacterSubset);

    /**
     * Sets prompt (title) for dialog. Accepts null, which indicates empty string.
     *
     * @param aString         new title
     *
     * @since S60 5.0
     */
    void SetTitleL(const TDesC* aString);

    /**
     * Moves cursor position of the editor.
     *
     * @param aString         new position, if bigger than editor max size then
     *                                      cursor is set at the end of the editor.
     *
     * @since S60 5.0
     */
    void SetCursorPositionL(TInt aPosition);

    /**
     * Size of the dialog.
     *
     * @return TSize          Size of the dialog.
     *
     * @since S60 5.0
     */
    TSize ContentSize() const;

    // From MMIDComponent
    //
    inline void Dispose();

    //
    // From CCoeControl
    void FocusChanged(TDrawNow aDrawNow);
    void MakeVisible(TBool aVisible);
    void SizeChanged();
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void HandleResourceChange(TInt aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2
    TCoeInputCapabilities InputCapabilities() const;

    // From MEikEdwinObserver
    void HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType);

    // From MCoeCaptionRetrieverForFep
    void GetCaptionForFep(TDes& aCaption) const;

    /**
     * Set text  input modifiers according to TextBox constraints.
     *
     * @since S60 5.0
     */
    void ProcessModifiers();

    /**
     * Either makes dialog visible or hides the dialog.
     *
     * @param TBool          Show or hide the dialog.
     *
     * @since S60 5.0
     */
    void ShowL(TBool aShow);

    /**
     * Tries to add replace text in editor when conmstraints have been changed.
     * Should be called after changing constraints.
     *
     * @param TBool          Show or hide the dialog.
     *
     * @since S60 5.0
     */
    void SetTextWithNewConstraintsL(HBufC* aText);

    /**
     * Initializes the editor
     *
     * @since S60 5.0
     */
    void SetEditorL();

    /**
     * Returns TextBox current intput constraints
     *
     * @param TInt         input constraints
     *
     * @since S60 5.0
     */
    TInt Constraints();

    /**
     * Returns whether dialog is currently visible on the display
     *
     * @param TBool         iShowing
     *
     * @since S60 5.0
     */
    TBool Showing();

protected:
    //
    //From CAknTextQueryDialog
    //
    void UpdateLeftSoftKeyL();
    TBool OkToExitL(TInt aButtonId);

private:
    /**
     * Sets the input constraints of the editor.
     *
     * @since S60 5.0
     */
    void SetFEPModeAndCharFormat();

    void UpdateCbasL();

    TBool IsNumberConversionNeeded();

    void HandleMinusCharEventL(MEikEdwinObserver::TEdwinEvent aEventType);

    /**
     * Prevents changes that would result in an illegal string
     *
     * @param TEdwinEvent aEventType
     */
    void HandleTextUpdateL(TEdwinEvent aEventType);

    TPtrC Read() const;

    void CreateNonMidletCommandsL();
    void AddCommandToEdwinL(TInt aCommandResIdShort,
                            TInt aCommandResIdLong,
                            TInt aCommandId);
    void AddCommandToEdwinL(TInt aCommandResId, TInt aCommandId);
    void RemoveNonMidletCommands();
    void AddCommandToEdwinL(MMIDCommand& aCommand);
    void CreatePhoneCallL();

    void CreatePasswordTextL();

    /**
     * Updates editor's input mode. Must be called after changing input modes
     * and after the dialog is set visible.
     *
     * @since S60 5.0
     */
    void UpdateInitialInputModes();

    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    void CMIDTextBoxQueryDialog::SetRightScrollBarPosition();

private:
    inline TUint GetConstraints() const;
    inline TBool IsConstraintSet(TUint aConstraints);

private:
    /**
     * Pointer to CMIDEdwinUtils
     */
    CMIDEdwinUtils*             iEdwinUtils;

    /**
     * Pointer to the editor
     */
    CEikEdwin*                  iEditor;

    /**
     * Boolean value indicating whether the dialog is currently visible or not.
     */
    TBool                       iShowing;

    /**
     * Maximum size (number of characters) of the editor.
     */
    TInt                        iMaxSize;

    /**
     * Text input constraints
     */
    TUint                       iConstraints;

    /**
     * Pointer to Displayable
     */
    CMIDDisplayable*            iDisplayable;

    /**
     * Text case (abc/ABC/etc.) to be set when coming visible
     */
    TInt                        iInitialCurrentCase;

    /**
     * Input mode (text/numeric) to be set when coming visible
     */
    TInt                        iInitialCurrentInputMode;

    /**
     * Input mode language to be set when coming visible
     */
    TLanguage                   iInitialCurrentLanguage;

    /**
     * Stores keycode of the current key; used in multitap transaction exceeding editor limit.
     */
    TInt                        iThisMultitapKey;

    /**
     * stores keycode of the last key; used in multitap transaction exceeding editor limit
     */
    TInt                        iLastMultitapKey;

    /**
     * Decimal separator character based on locale
     */
    TChar                       iDecimalSeparator;

    /**
     * Needed when accessing to phonebook
     */
    CMIDAiwPbk2Client*          iAiwPbkClient;

    /**
     * Boolean value indicating whether key events are pending.
     */
    TBool                       iKeyEventsPending;
    /**
     * Used key event
     */
    const TKeyEvent*            iusedKeyEvent;
    /**
     * Count line of text in iEditor
     */
    TInt                        iLastCountLine;
    /**
     * Rect of Editor
     */
    TRect iEditorRect;

    /**
     * Used when ConstraintsValidForText decide
     * whatever to test text using stric options or not
     */
    TBool iStrict;
};


inline void CMIDTextBoxQueryDialog::Dispose()
{
    delete this;
}

inline TUint CMIDTextBoxQueryDialog::GetConstraints() const
{
    return iConstraints;
}

inline TBool CMIDTextBoxQueryDialog::IsConstraintSet(TUint aConstraints)
{
    return (iConstraints & MMIDTextField::EConstraintMask) == aConstraints ;
}
#endif // CMIDTEXTBOXQUERYDIALOG_H
