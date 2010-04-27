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


#ifndef SWTMOBILESHELL_H
#define SWTMOBILESHELL_H


#include <aknnavi.h>
#include "eswtmobileextensions.h"
#include "eswtwidgetscore.h"
#include "swtshell.h"


NONSHARABLE_CLASS(CSwtMobileShell)
        : public CSwtShell
        , public MSwtMobileShell
{
// Own methods
public:
    /**
     * Two phase constructor.
     * @param aDisplay parent display
     * @param aPeer java peer of this class
     * @param aParent shell parent
     * @param aStyle shell style
     * @return pointer to an object of this class
     */
    static CSwtMobileShell* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtShell* aParent, TInt aStyle);

protected:
    /**
     * Destructor.
     */
    ~CSwtMobileShell();

    /**
     * 1st phase constructor.
     * @param aDisplay parent display
     * @param aPeer java peer of this class
     * @param aParent shell parent
     * @param aStyle shell style
     */
    inline CSwtMobileShell(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                           MSwtShell* aParent, TInt aStyle);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Gets the key state for a key that is present twice on keyboard.
     * @param aLeftScanCode
     * @param aRightScanCode
     */
    TBool GetLeftAndRightKeyState(TInt aLeftScanCode, TInt aRightScanCode);

// From CCoeControl
public:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                TEventCode aType);

// From MSwtShell
public:
    TRect DefaultBounds() const;
    MSwtShell::TSwtStatusPaneStyle StatusPaneStyle() const
    {
        return iStatusPaneStyle;
    };
    MSwtMobileShell* MobileShellInterface()
    {
        return this;
    }
    const MSwtMobileShell* MobileShellInterface() const
    {
        return this;
    }
    TBool IsMobileShell() const
    {
        return ETrue;
    }

// From MSwtMobileShell
public:
    void AddKeyStateL(TInt aScanCode);
    void ChangeTrimL(TInt aStyle, TInt aStatusStyle);
    const MSwtShell* Shell() const;
    TBool GetKeyState(TInt aSwtKeyCode);
    void SetStatusTextL(const TDesC& aText);
    const TDesC* GetStatusText() const
    {
        return iStatusTextBuf;
    };

private:
    /**
     * Contains the key pressed. When GetKeyState() method is called
     * for a Key code, this key code is removed from this array, if present.
     */
    RArray<TInt> iKeyCodeStatus;

    /**
     * Style of the status pane.
     */
    MSwtShell::TSwtStatusPaneStyle iStatusPaneStyle;

    /**
     * The string of status text.
     */
    HBufC* iStatusTextBuf;

    /**
     * Used to delay construction till the status pane style is received.
     */
    TBool iConstructed;
};

#endif // SWTMOBILESHELL_H
