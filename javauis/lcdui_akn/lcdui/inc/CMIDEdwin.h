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
* Description:  Implementation class for TextField and TextBox
*
*/


#ifndef CMIDEDWIN_H
#define CMIDEDWIN_H

#include <eikedwin.h>
#include <lafmain.h>

#include "CMIDForm.h"
#include "CMIDDisplayable.h"
// MTickerObserver interface in class declaration
#include "CMIDTicker.h"
// for MMIDCommandObserver interface in class declaration
#include "CMIDCommand.h"
#include "CMIDEdwinUtils.h"
#include "CMIDUtils.h"

// FORWARD DECLARATIONS
class CMIDTextFieldItem;
class CMIDTextFieldEdwin;

// Abstract class for TextField and TextBox.

class CMIDEdwin : public CEikEdwin, public MMIDTextBox,
        public MCoeControlObserver, public MEikEdwinObserver,
        public MMIDTickerObserver, public MMIDCommandObserver
{
public:

    ~CMIDEdwin();
    //
    // From MMIDTextBox
    //
    void DeleteTextL(TInt aOffset,TInt aLength);
    void SetTextL(const TDesC& aText);
    void InsertTextL(const TDesC& aText,TInt aPosition);
    void SetConstraintsL(TUint aConstraints);
    TInt SetMaxSizeL(TInt aSize);
    TInt GetMaxSize();
    TInt Size();
    TInt GetCaretPosition();
    HBufC* GetTextL();
    void SetInitialInputModeL(const TDesC& aCharacterSubset);
    //
    // From MMIDComponent
    //
    inline CBase* Ptr();
    inline void Dispose();
    //
    // From CCoeControl
    //
    virtual void SetContainerWindowL(const CCoeControl& aContainer);
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    virtual void FocusChanged(TDrawNow aDrawNow);
    //
    // From MEikEdwinObserver
    //
    void HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType);
    //
    // From MTickerObserver
    //
    void TickerShowing(TBool aShowing);

    void SetTextWithNewConstraintsL(HBufC* aText);

    /**
     * Layouts the Edwin control relative to the parent
     *
     * @note aLayout.iNumberOfLinesShown is changed in case of TextFieldItem
     * @param aRect the parent rect
     * @param aLayout multi line layout info
     */
    virtual void DoLayout(const TRect& aRect) = 0;

    void ProcessModifiers();
    inline void SetTextFieldItem(CMIDTextFieldItem* aTextFieldItem);
    void HandleCurrentL(TBool aActivate);
    //
    // From MCoeControlObserver
    //
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
    //
    // From MMIDCommandObserver
    //
    TBool ProcessCommandL(CMIDCommand* aCommand);

    /**
    * Determines if languge conversion for numbers is needed based
    * on constraints, language settings, display language setting
    * and general setting.
    * @returns ETrue to indicate whether digit conversions need to be used.
    * @returns EFalse to indicate that no conversion is needed.
    */
    TBool IsNumberConversionNeeded();
public:
    inline TUint GetConstraints() const;
    inline TBool IsConstraintSet(TUint aConstraints);
    /**
     * Detects if Edwin control is in partial formatting mode
     * When the text is longer than a specific limit (e.g. 1900),
     * the Edwin formats the non-visible text lines in background.
     * <p>
     * @note This is useful only for TextFieldItem
     * @see CEikEdwin::LowerPartialFormattingLength()
     * @return ETrue if Edwin formats text in partial mode, and EFalse otherwise
     */
    inline TBool IsPartialFormatting() const;

protected:
    /**
     * Constructor
     */
    CMIDEdwin(TUint aConstraints, MMIDDisplayable* aDisplayable,
              CMIDTextFieldItem* aTextFieldItem);

    /**
     * Second phase constructor
     * @param aText a text to be set in the field
     * @param aMaxSize maximum number of characters to allow
     */
    void ConstructL(const TDesC& aText,TInt aMaxSize);

protected:
    virtual void AddCommandToEdwinL(MMIDCommand& aCommand) = 0;

private:

    void CMIDEdwin::HandleMinusCharEventL(TEdwinEvent aEventType);
    void CreatePasswordTextL();
    void SetFEPModeAndCharFormat();

    virtual void CreateNonMidletCommandsL();
    void AddCommandToEdwinL(TInt aCommandResId, TInt aCommandId);
    void AddCommandToEdwinL(TInt aCommandResIdShort,
                            TInt aCommandResIdLong,
                            TInt aCommandId);
    void RemoveNonMidletCommands();

    /*
     * Initiates the phone call.
     * After the phone call is ended, the form is brought to foreground.
     * AiwDialApi takes care of switching to the window back.
     * The MIDlet's window group is needed
     *
     * @note The current implementation considers that making phone calls
     *      is not so often use-case, and therefore all needed data is initialized
     *      in this call
     */
    void CreatePhoneCallL();

    TPtrC Read() const;

    void UpdateTextCapacityIndicatorValueL();
    void HandleTextPastedL(TInt aStartPos,TInt& aLength);
    void ResetNumericContentL();

protected:
    CMIDDisplayable* iDisplayable;
    CMIDUIManager* iUIManager;
    // used to pass EEventStateChanged back to CMIDTextFieldEdwin
    CMIDTextFieldItem* iTextFieldItem;
    TUint iConstraints;
    TBool iStateChanged;

private:
    /**
     * Pointer to CMIDEdwinUtils
     */
    CMIDEdwinUtils*             iEdwinUtils;

    TBool iKeyEventsPending;
    TInt iMaxSize;
    TInt iInitialCurrentCase;       // text case (abc/ABC/etc.) to be set when getting focus
    TInt iInitialCurrentInputMode;  // input mode (text/numeric) to be set when getting focus
    TLanguage iInitialCurrentLanguage;

    TInt iThisMultitapKey;          // stores keycode of the current key; used in multitap transaction exceeding editor limit
    TInt iLastMultitapKey;          // stores keycode of the last key; used in multitap transaction exceeding editor limit

    TChar iDecimalSeparator;

    /** True when we are displaying the text capacity indicator */
    TBool iDisplayingCapacityIndic;
    CMIDAiwPbk2Client* iAiwPbkClient;
    TBool useEnterKey;      // used for EnterKey
};

inline TUint CMIDEdwin::GetConstraints() const
{
    return iConstraints;
}

inline TBool CMIDEdwin::IsConstraintSet(TUint aConstraints)
{
    return (iConstraints & MMIDTextField::EConstraintMask) == aConstraints ;
}

inline TBool CMIDEdwin::IsPartialFormatting() const
{
    return TextLength() >= LowerPartialFormattingLength();
}

inline CBase* CMIDEdwin::Ptr()
{
    return this;
}

inline void CMIDEdwin::Dispose()
{
    delete this;
}

inline void CMIDEdwin::SetTextFieldItem(CMIDTextFieldItem* aTextFieldItem)
{
    iTextFieldItem = aTextFieldItem;
}

#endif // CMIDEDWIN_H
