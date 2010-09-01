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


#ifndef SWTTIMEDMESSAGEBOX_H
#define SWTTIMEDMESSAGEBOX_H


class CSwtPopupFormProxy;


/**
 * CSwtTimedMessageBox
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTimedMessageBox)
        : public CBase
{
// Own methods
public:
    /**
     * Dialog executor.
     * @param aTitle title
     * @param aMessage message
     * @param aStyle dialog style
     * @param aImage image
     */
    static void RunDlgLD(const TDesC& aTitle, const TDesC& aMessage,
    TInt aStyle, const MSwtImage* aImage);

    /**
     * Two phase constructor.
     * @param aStyle dialog style
     * @return CSwtTimedMessageBox*
     */
    static CSwtTimedMessageBox* NewLC(const TDesC& aTitle, const TDesC& aMessage,
                                      TInt aStyle, const MSwtImage* aImage);

    /**
     * Destructor.
     */
    virtual ~CSwtTimedMessageBox();

    void RunL();

// Own methods
private:
    /**
     * First phase constructor.
     * @param aStyle dialog style
     */
    CSwtTimedMessageBox(TInt aStyle);

    /**
     * Second phase constructor.
     */
    void ConstructL(const TDesC& aTitle, const TDesC& aMessage, const MSwtImage* aImage);

    /**
     * Animation getter.
     */
    TInt Animation() const;

// Data
private:
    /**
     * Popup form.
     */
    CSwtPopupFormProxy* iPopupForm;

    /**
     * Dialog style.
     */
    TInt iStyle;
};

#endif // SWTTIMEDMESSAGEBOX_H
