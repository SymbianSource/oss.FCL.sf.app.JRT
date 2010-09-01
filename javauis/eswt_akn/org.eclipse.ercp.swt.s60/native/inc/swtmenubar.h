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


#ifndef SWTMENUBAR_H
#define SWTMENUBAR_H


#include <eikmenub.h>


class CSwtMenuArranger;


/**
 * CSwtMenuBar
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtMenuBar)
        : public CCoeControl
        , public MEikCommandObserver
{
// Methods
public:
    static CSwtMenuBar* NewL(CSwtMenuArranger& aMenuArranger);
    virtual ~CSwtMenuBar();
    CEikMenuBar* EikMenuBar();
    void TryDisplayMenuBarL(TBool aDisplayContextMenu, TBool aHasRadioGroup = EFalse);

    /**
     * This should be used as the only way to add the control to stack.
     * Cannot be enforced tough.
     */
    void AddToControlStackL();

    /**
     * This should be used as the only way to remove the control from stack.
     * Cannot be enforced tough.
     */
    void RemoveFromControlStack();

    /**
     * Tells wheter the control is on stack.
     * Works as long as the above wrappers are used.
     */
    TBool IsOnControlStack() const
    {
        return iIsOnControlStack;
    }

private:
    inline CSwtMenuBar(CSwtMenuArranger& aMenuArranger);
    void ConstructL();

// From CCoeControl
public:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                TEventCode aType);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
protected:
    void FocusChanged(TDrawNow aDrawNow);
    void SizeChanged();

// From MEikCommandObserver
protected:
    void ProcessCommandL(TInt aCommandId);

// Data
private:
    /**
     * Contained menu bar.
     * Own.
     */
    CEikMenuBar* iEikMenuBar;

    /**
     * Related menu arranger (and menu observer).
     */
    CSwtMenuArranger& iMenuArranger;

    /**
     * Tells wheter set on control stack by menu arranger.
     */
    TBool iIsOnControlStack;
};


#endif // SWTMENUBAR_H
