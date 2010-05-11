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


#ifndef SWTDATEEDITOR_H
#define SWTDATEEDITOR_H


#include <eikmfne.h>
#include "eswtmobileextensions.h"
#include "swtcontrolbase.h"


class CSwtButtonProxy;
class CSwtEditIndicator;


const TInt KSwtSecondsInOneDay = 86399;  // 23:59:59 = 86399 seconds


/**
 * CSwtDateEditor
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtDateEditor)
        : public CAknControl
        , public ASwtControlBase
        , public MSwtDateEditor
        , public MEikCalendarObserver
#if defined( RD_SCALABLE_UI_V2)
        , public MCoeControlObserver
#endif
{
public:
    /**
     * 1st phase constructor.
     * @param aDisplay parent display
     * @param aPeer java peer
     * @param aParen composite parent
     * @param aStyle supported styles
     */
    static CSwtDateEditor* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

private:
    /**
     * Default C++ constructor.
     * @param aDisplay parent display
     * @param aPeer java peer
     * @param aParen composite parent
     * @param aStyle supported styles
     */
    CSwtDateEditor(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                   MSwtComposite& aParent, TInt aStyle);

    /**
     * Destructor.
     */
    ~CSwtDateEditor();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Prepares the field for lost or gaining of focus
     */
    void PrepareForFocusChangeL();

    /**
     * Checks the field is AmPm
     * @param fieldIndex index of the field
     * @returns true if the field is AmPm
     */
    TBool IsFieldAmPmField(TInt fieldIndex) const;

    /**
     * Checks the field is Offset style
     * @param fieldIndex index of the field
     * @returns true if the field is Offset style.
     */
    TBool IsFieldOffsetStyleField(TInt fieldIndex) const;

    /**
     * Update default font.
     */
    void RetrieveDefaultFontL();

    /**
     * Update font.
     */
    void ProcessFontUpdate();

    /**
     * Resource change leaving helper
     */
    void SwtHandleResourceChangeL(TInt aType);

    /**
     * Checks wheter event notifying about editor's value change should be posted.
     * @param aKeyCode Code of the last pressed key
     * @param aWasFieldChangedAlready Indicates whether field was changed already
     * @returns true if "modified" event should be posted.
     */
    TBool IsModifyEventAllowed(TInt aKeyCode, TBool aWasFieldChangedAlready);

    /**
     * Checks whether current field was change or not
     * @param aIsFieldNumeric indicates whether field is numeric field
     * @returns true if field was modified, false otherwise.
     */
    TBool IsCurrentFieldChanged(TBool aIsFieldNumeric);

    /**
     * Updates current field counters if necessary.
     * @param aIsFieldNumeric Indicates whether field is numeric field
     * @param aIsFieldChanged Indicates whether field was changed already
     * @returns true if "modified" event should be posted.
     */
    void UpdateFieldCounters(TBool aIsFieldNumeric, TBool aIsFieldChanged);

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void MakeVisible(TBool aVisible);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void SetDimmed(TBool aDimmed);
    void HandleResourceChange(TInt aType);
#if defined( RD_SCALABLE_UI_V2)
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif

protected:
    void FocusChanged(TDrawNow aDrawNow);
    void SizeChanged();
    void PositionChanged();

private:
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    void  ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    void  SetFontL(const MSwtFont* aFont);
    void  SetBackgroundL(const MSwtColor* aColor);
    void  SetForegroundL(const MSwtColor* aColor);
    TBool IsKeyUsed(TUint aKeyCode) const;

// From ASwtControlBase
protected:
    const MSwtFont& DefaultFont() const;
    HBufC* MSKLabelL() const;

// From MSwtDateEditor
public:
    MSwtControl* Control();
    TInt64 GetDate();
    TInt GetOffset() const;
    void SetDate(const TInt64& aJavaDate);
    void SetDateEditorStyleL(TInt aDateEditorStyle);
    void SetOffset(TInt aSeconds);

// From MEikCalendarObserver
public:
    void GetMinimumAndMaximumAndInitialDatesForCalendarL(TTime &aMinimumDate,
            TTime &aMaximumDate, TTime &aInitialDate) const;
    void SetDateFromCalendarAndDrawNow(const TTime &aDate);

// From MCoeControlObserver
#if defined( RD_SCALABLE_UI_V2)
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
#endif

private:
    /**
     * The editor which will create different CEikEditors depending the style
     * @Own
     */
    CEikMfne* iEditor;

    /**
     * The style of the editor: DATE, TIME, DATE_TIME, DURATION, OFFSET
     */
    TInt iDateEditorStyle;

    /**
     * Compact or Full
     */
    TInt iDateEditorFormat;

    /**
     * Number of microseconds since January 1st, 1970.
     */
    TInt64 iJavaDateInMicroSec;

    /**
     * Number of microseconds from January 1st, 0AD to January 1st, 1970.
     */
    TInt64 iEpocDateInMicroSec;

    /**
     * The default font.
     * Owned.
     */
    MSwtFont* iDefaultFont;

    /**
     * Edit indicator for multipage dialog
     * Own.
     */
    CSwtEditIndicator* iIndicator;

    /**
     * Length of the current field of date/time editor
     */
    TInt iCurrentFieldLength;

    /**
     * Number of valid decimal places in current field of date/time editor
     */
    TInt iCurrentFieldDecimalPlacesCount;

    /**
     * Index of last field edited of date/time editor
     */
    TInt iLastField;

    /**
     * User set colors.
     */
    TBool iBgColorIsCustom;
    TBool iFgColorIsCustom;
};


#endif // SWTDATEEDITOR_H
