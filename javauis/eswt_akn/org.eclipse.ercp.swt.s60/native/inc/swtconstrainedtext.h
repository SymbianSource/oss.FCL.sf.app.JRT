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


#ifndef SWTCONSTRAINEDTEXT_H
#define SWTCONSTRAINEDTEXT_H


#include <coecntrl.h>
#include <eikedwob.h>
#include "eswtmobileextensions.h"
#include "swttextbase.h"


/**
 * CSwtConstrainedText
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtConstrainedText)
        : public CSwtTextBase
        , public MSwtConstrainedText
        , public MEikEdwinObserver
{
public:
    /**
      * 1st phase constructor.
      * @param aDisplay parent display
      * @param aPeer java peer
      * @param aParen composite parent
      * @param aStyle supported styles
      */
    static CSwtConstrainedText* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

private:
    /**
     * Default C++ constructor.
     * @param aDisplay parent display
     * @param aPeer java peer
     * @param aParen composite parent
     * @param aStyle supported styles
     */
    inline CSwtConstrainedText(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                               MSwtComposite& aParent, TInt aStyle);

    /**
     * Destructor.
     */
    ~CSwtConstrainedText();

    /**
     * Checks Navigation key.
     * @param aKeyEvent type of key event
     * @return navigation key
     */
    TBool IsNavigationKey(const TKeyEvent& aKeyEvent) const;

    /**
     * Checks Hot key.
     * @param aKeyEvent type of key event
     * @return one of CEikEdwin::TEdwinHotKeys or -1 if cannot translate.
     */
    TBool IsHotKeyL(const TKeyEvent& aKeyEvent) const;

    /**
    * Checks the content will remain valid if the specified text replaces
    * the given range
    * @param aStart starting point of the text
    * @param aEnd ending point of the text
    * @param aText Text to replace
    * @return KErrNone for valid text else KErrArgument
    */
    TInt LooseCheckReplacementL(TInt aStart, TInt aEnd, const TDesC& aText) const;

    /**
     * Checks the content will remain valid if the specified character replaces
     * the given range.
     * @param aStart starting point of the character
     * @param aEnd ending point of the character
     * @param aChar character to replace
     * @return KErrNone for valid character else KErrArgument
     */
    TInt LooseCheckReplacementL(TInt aStart, TInt aEnd, TInt aChar) const;

    /**
     * Checks the content will remain valid if the specified text replaces
     * the current selection.
     * @param text to be replaced
     * @return KErrNone for valid character else KErrArgument
     */
    TInt StrictCheckReplacementL(const TDesC& aText) const;

    /**
     * Checks the content will remain valid if the specified text replaces
     * the given range.
     * @param aStart starting point of the text
     * @param aEnd ending point of the text
     * @param aText Text to replace
     * @return KErrNone for valid text else KErrArgument
     */
    TInt CheckReplacementL(TInt aStart, TInt aEnd, const TDesC& aText,
                           TBool aStrict) const;

    /**
     * Checks whether a given text is an acceptable for all styles
     * @param text to be validate
     * @return KErrNone for valid text else KErrArgument
     */
    TInt LooseValidateText(const TDesC& aText) const;

    /**
     * Strick Checks whether a given text is an acceptable for all styles
     * @param text to be validate
     * @return KErrNone for valid text else KErrArgument
     */
    TInt StrictValidateText(const TDesC& aText) const;

    /**
     * Checks whether a given text is an acceptable Numeric.
     * It must be made of digits only and an optional leading minus sign.
     * If the maximum text size is 1, then no minus sign is allowed.
     * @param text to be validate
     * @return KErrNone for valid text else KErrArgument
     */
    TInt LooseValidateNumeric(const TDesC& aText) const;

    /**
     *  Strict version of the NUMERIC check. Same as the loose one except a "-" is
     *  no longer accepted.
     *  @param aText text to be validate
     *  @return KErrNone for valid text else KErrArgument
     */
    TInt StrictValidateNumeric(const TDesC& aText) const;

    /**
     * Checks whether a text is an acceptable DECIMAL. It consists of an
     * optional minus sign '-', followed by one or more whole-number
     * decimal digits, followed by an optional fraction separator,
     * followed by zero or more decimal fraction digits. The whole-number
     * decimal digits may be omitted if the fraction separator and one or
     * more decimal fraction digits are present.
     * @param text to be validate
     * @return KErrNone for valid text else KErrArgument
     */
    TInt LooseValidateDecimal(const TDesC& aText) const;

    /**
     * Strict version of the DECIMAL check. Same as the loose one except "-", "."
     * and "-." are no longer accepted.
     * @param text to be validate
     * @return KErrNone for valid text else KErrArgument
     */
    TInt StrictValidateDecimal(const TDesC& aText) const;

    /**
     * Checks if a character is acceptable in a phone number
     * @param aAllowedChars one of the other allowed chars
     * @param aChar character to be validate@return returns valid phone Number
     * @return returns valid phone Number
     */
    static TBool IsPhoneNumberChar(const TChar &aChar, const TDesC& aAllowedChars);

    /**
     * Checks if a text is a correct PHONENUMBER. it contains only
     * allowed characters, no syntax checking.
     * @param aText text to be validate
     * @return KErrNone for valid text else KErrArgument
     */
    static TInt  ValidatePhoneNumber(const TDesC& aText);

    /**
     * Convertes '.' or any decimal seperator to localised decimal seperator
     * @param aText text or text which containes decimal seperator to convert localised
     * @return HBufC* converted text for localised decimal seperator
     */
    HBufC* ConvertToLocalizedDecimalLC(const TDesC& aText);

    /**
     * Convertes local decimal seperator to '.'
     * @param aText text or text which containes local decimal seperator to convert '.'
     * @return void nothing to return.
     */
    void ConvertFromLocalizedDecimal(HBufC* buf) const;

// From CCoeControl
public:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

// From CCoeControl
protected:
    void Draw(const TRect& aRect) const;

// From MSwtControl
public:
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

// From MSwtConstrainedText
public:
    MSwtControl* Control();
    void  SetConstraintL(TConstraint aConstraint);
    HBufC* GetTextL() const;
    TBool SetTextL(const TDesC& aText);
    TBool AppendL(const TDesC& aText);
    TBool InsertL(const TDesC& aText);
    TInt  GetCharCount() const;
    TBool SetTextLimitL(TInt aLimit);
    TInt  GetCaretPosition() const;
    void  SetSelectionL(TInt aStart, TInt aEnd);
    void  ClearSelectionL();
    void  SelectAllL();
    void  ShowSelectionL();
    TBool CutL();
    void  CopyL();
    TBool PasteL();
    TBool GetEditable() const;
    void  SetEditable(TBool aEditable);

// From MEikEdwinObserver
public:
    void HandleEdwinEventL(
        CEikEdwin *aEdwin,
        TEdwinEvent aEventType);
// Data
private:
    /**
     * The styles of the ConstrainedText
     */
    TConstraint iConstraint;

    /**
     * Text Limit of the Editor
     */
    TInt iTextLimit;

    /**
     * Rectangle of the Editor
     */
    TRect iRect;
    /**
     * For Localised decimal seperator
     */
    TChar iDecimalSeparator;

    /**
     * Last view visible rectangle.
     */
    mutable TRect iLastViewVisibleRect;
};


#endif // SWTCONSTRAINEDTEXT_H
