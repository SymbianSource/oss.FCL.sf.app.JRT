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


#ifndef SWTMESSAGEBOX_H
#define SWTMESSAGEBOX_H


#include "eswt.h"


class CSwtPopupFormProxy;


/**
 * CSwtMessageBox
 * MessageBox peer implementation. Uses CAknPopupForm.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtMessageBox)
        : CBase
        , public MSwtResourceChangeObserver
{
// Own methods
public:
    /**
     * Dialog executor.
     * @param aTitle title
     * @param aMessage message
     * @param aStyle dialog style
     */
    static TInt RunDlgLD(MSwtDisplay& aDisplay, const TDesC& aTitle,
    const TDesC& aMessage, TInt aStyle);

    /**
     * Two phase constructor.
     * @param aTitle title
     * @param aMessage message
     * @param aStyle dialog style
     */
    static CSwtMessageBox* NewLC(MSwtDisplay& aDisplay,  const TDesC& aTitle,
                                 const TDesC& aMessage, TInt aStyle);

    /**
     * Destructor.
     */
    virtual ~CSwtMessageBox();

public:
    TInt RunL();

// From MSwtResourceChangeObserver
    void OfferResourceChangeL(TInt aType);

// Own methods
private:
    /**
     * First phase constructor.
     * @param aTitle title
     * @param aStyle dialog style
     */
    CSwtMessageBox(MSwtDisplay& aDisplay, TInt aStyle);

    /**
     * Second phase constructor.
     */
    void ConstructL(const TDesC& aTitle, const TDesC& aMessage);

    /**
     * Button id converter.
     * @param aButtonId
     * @return TInt
     */
    TInt ConvertButtonId(TInt aButtonId) const;

    /**
     * Animation id getter.
     */
    TInt Animation() const;

    /**
     * Button id getter.
     */
    TInt Button() const;

// Data
private:

    /**
     * Pupup from.
     */
    CSwtPopupFormProxy* iPopupForm;

    /**
     * Parent display.
     */
    MSwtDisplay& iDisplay;

    /**
     * Dialog style.
     */
    TInt iStyle;

    /**
     * Dialog animation.
     */
    TInt iAnimation;
};

#endif // SWTMESSAGEBOX_H
