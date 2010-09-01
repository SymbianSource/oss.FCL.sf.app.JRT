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


#ifndef SWTTEXT_H
#define SWTTEXT_H


#include <txtglobl.h>
#include <eikedwob.h>
#include "swttextbase.h"


/**
 * CSwtText
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtText)
        : public CSwtTextBase
        , public MSwtText
        , public MSwtVerifyEventObserver
        , public MEikEdwinObserver
{
private:
    class CPasswordText;

public:
    /**
     * Two phase constructor.
     * @param aDisplay parent display
     * @param aPeer java peer
     * @param aParen composite parent
     * @param aStyle supported styles
     */
    static CSwtText* NewL(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle);

protected:
    /**
     * First phase constructor.
     * @param aDisplay parent display
     * @param aPeer java peer
     * @param aParen composite parent
     * @param aStyle supported styles
     */
    inline CSwtText(
        MSwtDisplay& aDisplay,
        TSwtPeer aPeer,
        MSwtComposite& aParent,
        TInt aStyle);

    /**
     * Destructor.
     */
    ~CSwtText();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Compute alignment based on the control's style.
     * @return computed alignment.
     */
    CParaFormat::TAlignment ComputeAlignment() const;

    /**
     * Set alignment base on the control's style.
     */
    void SetAlignmentL();

    /**
     * Process line brakes based on style.
     * Length of the processed string might change.
     * @param aText string to be processed.
     */
    void ProcessLineBrakes(TDes& aText) const;

    /**
     * Remove anything beyound the first line brake.
     * Length of the processed string might change.
     * @param aText string to be processed.
     */
    static void CropToFirstLine(TDes& aText);

    /**
     * Replace legacy ASCII line brakes with Unicode ones.
     * Length of the processed string might change.
     * @param aText string to be processed.
     */
    static void ConvertToUnicodeBreaks(TDes& aText);

    /**
     * Post a Verify event with string.
     * @param aStart start of selection.
     * @param aEnd end of selection.
     * @param aText string to be inserted.
     */
    virtual void PostVerifyEventL(
        TInt aStart,
        TInt aEnd,
        TDes& aText);

    /**
     * Post Verify event with char.
     * @param aStart start of selection.
     * @param aEnd end of selection.
     * @param aChar char to be inserted.
     */
    void PostVerifyEventL(
        TInt aStart,
        TInt aEnd,
        TInt aChar);

    /**
     * Send char to the editor in the form of a key event.
     * param aChar char to be sent.
     */
    void SendCharacterL(TText aChar);

// From CSwtTextBase
public:
    void HandleResourceChange(TInt aType);

// From CCoeControl
public:
    void SizeChanged();
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType);

// From CCoeControl
protected:
    void Draw(const TRect& aRect) const;

// From MSwtControl
public:
    void ProcessKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType);

// From MSwtText
public:
    MSwtScrollable* Scrollable();
    HBufC* GetTextL() const;
    HBufC* GetTextRangeL(TInt aStart, TInt aEnd) const;
    TBool SetTextL(TDes& aText);
    TBool AppendL(TDes& aText);
    TBool InsertL(TDes& aText);
    TInt GetCharCount() const;
    TBool SetTextLimitL(TInt aLimit);
    TInt GetCaretLineNumber() const;
    TPoint GetCaretLocationL() const;
    TInt GetCaretPosition() const;
    TPoint GetSelection() const;
    HBufC* GetSelectionTextL() const;
    void SetSelectionL(TInt aStart, TInt aEnd);
    void ClearSelectionL();
    void SelectAllL();
    void ShowSelectionL();
    //TBool CutL();
    void CopyL();
    TBool PasteL();
    TBool GetEditable() const;
    void SetEditable(TBool aEditable);
    TInt GetLineCount() const;
    TInt GetLineHeightL() const;
    TInt GetTopIndexL() const;
    void SetTopIndexL(TInt aIndex);
    TInt GetTopPixelL() const;
    TText GetEchoChar() const;
    void SetEchoCharL(TText aEcho);
    void SetDoubleClickEnabled(TBool aEnabled);
    TInt GetOrientation() const;
    void SetOrientationL(TInt aOrientation);

// From MSwtVerifyEventObserver
public:
    void ProcessVerifiedTextL(
        TInt aStart,
        TInt aEnd,
        TDes& aText);

// From MEikEdwinObserver
public:
    void HandleEdwinEventL(
        CEikEdwin *aEdwin,
        TEdwinEvent aEventType);

    void SetVisibleScrollBar(TInt aStyle, TBool aVisible);
// Data
private:
    /**
     * Maximum length of the text.
     */
    TInt iTextLimit;

    /**
     * The word to post to a Java Verify event
     */
    //RBuf iBufferToVerify;

    /**
     * Last view visible rectangle.
     */
    mutable TRect iLastViewVisibleRect;
};


inline CSwtText::CSwtText(
    MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite& aParent,
    TInt aStyle)
        : CSwtTextBase(aDisplay, aPeer, aParent, aStyle)
{
}

NONSHARABLE_CLASS(CSwtText::CPasswordText)
        : public CGlobalText
{
public:
    static CPasswordText* NewL(
        const CParaFormatLayer* aParaLayer,
        const CCharFormatLayer* aCharLayer,
        CEikEdwin* aEditor,
        TBool aPaswdEnable);

    ~CPasswordText();
private:
    inline CPasswordText(
        const CParaFormatLayer* aParaLayer,
        const CCharFormatLayer* aCharLayer,
        CEikEdwin* aEdwin,
        TBool aPaswdEnable);
    void ConstructL();
    static TInt TimerCallback(TAny* aThis);
    void DoTimerCallbackL(TBool aNotifyFormat);
    void SaveClearTextL(TInt aPos, const TDesC& aBuf);

// From CGlobalText
public:
    void Reset();
    void InsertL(TInt aPos, const TChar& aChar);
    void InsertL(TInt aPos, const TDesC& aBuf) ;
    TBool DeleteL(TInt aPos, TInt aLength);
    TInt PasteFromStoreL(const CStreamStore& aStore,
                         const CStreamDictionary& aDictionary,
                         TInt aPos);
    void CopyToStoreL(CStreamStore& aStore,
                      CStreamDictionary& aDictionary,
                      TInt aPos,
                      TInt aLength) const;
    void Extract(TDes& aBuf, TInt aPos = 0) const;
    void Extract(TDes& aBuf, TInt aPos, TInt aLength) const;
    void SetEchoCharL();

// Data
public:
    TText iEchoChar;
private:

    // The timer used to show the temporary clear character
    CPeriodic* iTimer;

    // A reference to the text editor that owns us
    CEikEdwin* iEditor;

    // The position of the temporary clear character
    TInt iClearCharPos;

    // The clear text
    HBufC* iClearText;

    // checking for password text or not
    TBool iPswdEnable;
};

#endif // SWTTEXT_H
