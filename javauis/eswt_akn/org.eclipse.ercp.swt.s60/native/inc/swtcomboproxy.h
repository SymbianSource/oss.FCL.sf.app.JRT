/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTCOMBOPROXY_H
#define SWTCOMBOPROXY_H


#include <AknControl.h>
#include <coecobs.h>


class CAknChoiceList;


/**
 * CSwtComboProxy
 * eSWT's CSwtCombo access to S60's CAknChoiceList
 * @lib eswtapifacade
 */
class CSwtComboProxy
        : public CAknControl
        , public MCoeControlObserver
{
// Types
public:
    enum TSwtComboFlags
    {
        EComboWithCurrentSelection = 0x01,
        EComboWithoutCurrentSelection = 0x02,
        EComboPositionLeft = 0x04,
        EComboPositionRight = 0x08,
        EComboPositionBottom = 0x10
    };
// Own methods
public:
    IMPORT_C static CSwtComboProxy* NewL(CCoeControl* aParent, CDesCArray* aItemArray);
    IMPORT_C virtual ~CSwtComboProxy();

// From CAknChoiceList
public:
    IMPORT_C TInt ShowChoiceListL();
    IMPORT_C void HideChoiceList();
    IMPORT_C void SetSelectedIndex(const TInt aIndex);
    IMPORT_C TInt SelectedIndex() const;
    IMPORT_C void SetItems(CDesCArray* aArray);
    IMPORT_C TInt AddItemL(const TDesC* aDesC);
    IMPORT_C void RemoveItem(const TInt aIndex);
    IMPORT_C void SetFlags(const TInt aFlags);
    IMPORT_C TInt Flags() const;

// From CCoeControl
public:
    IMPORT_C CCoeControl* ComponentControl(TInt aIndex) const;
    IMPORT_C TInt CountComponentControls() const;
    IMPORT_C TSize MinimumSize();
    IMPORT_C void SizeChanged();
    IMPORT_C void HandleResourceChange(TInt aType);
    IMPORT_C void SetBackground(const MCoeControlBackground* aBackground);
    IMPORT_C TBool IsFocused() const;
    IMPORT_C void SetFocus(TBool aFocus, TDrawNow aDrawNow=ENoDrawNow);

// From MCoeControlObserver
public:
    IMPORT_C void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

// Own methods
private:
    CSwtComboProxy();
    void ConstructL(CCoeControl* aParent, CDesCArray* aItemArray);

// Data
private:
    CAknChoiceList* iDelegate; // own
};


#endif // SWTCOMBOPROXY_H
