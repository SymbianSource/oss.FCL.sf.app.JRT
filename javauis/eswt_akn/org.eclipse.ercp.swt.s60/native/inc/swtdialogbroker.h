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


#ifndef SWTDIALOGBROKER_H
#define SWTDIALOGBROKER_H


#include <e32base.h>
#include <jni.h>


class CSwtDisplay;


/**
 * CSwtDialogBroker
 * Dialog broker implementation
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtDialogBroker)
        : public CActive
{
public:
    enum TRequestType {ENoRequest,
        EFileDialog,
        EMessageBox,
        EDirectoryDialog,
        EQueryDialog,
        EColorDialog,
        ETimedMessageBox } ;

public:
    /**
     * Constructor.
     * @param Pointer to the display
     * @param aRequest the type of the dialog under the request
     * @param aStyle aPriority the priority of the active object
     */
    CSwtDialogBroker(CSwtDisplay* aDisplay, TRequestType aRequest = ENoRequest,
                     TInt aPriority = EPriorityStandard);
    ~CSwtDialogBroker();

    /**
     * Request function
     * @param aPeer java peer of this class
     * @param aRequest the type of the dialog under request
     * @param aStyle supported styles.
     * @param aTitle title of the dialog
     * @param aText1 Dialog parameter
     * @param aText2 Dialog parameter
     * @return void
     */
    void RequestRunDialogL(TSwtPeer aPeer, TInt aRequest,
                           TInt aStyle, const TDesC& aTitle,
                           const TDesC& aText1,
                           const TDesC& aText2);

    /**
     * Set extra dialog data
     * @param aInt1 Dialog parameter
     * @param aInt2 Dialog parameter
     * @param aInt3 Dialog parameter
     * @return void
     */
    void SetExtraData(const TInt aInt1, const TInt aInt2, const TInt aInt3);

    // from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    /**
     * Open FileDialog
     */
    void RunFileDialogL();

    /**
     * Open MessageBox
     */
    void RunMessageBoxL();

    /**
     * Open DirectoryDialog
     */
    void RunDirectoryDialogL();

    /**
     * Open ColorDialog
     */
    void RunColorDialogL();

    /**
     * Open QueryDialog
     */
    void RunQueryDialogL();

    /**
     * Open TimedMessageBox
     */
    void RunTimedMessageBoxL();

private: // data

    /**
     * Pointer to the display
     * Not own
     */
    CSwtDisplay* iDisplay;

    /**
     * The type of the dialog under request
     */
    TRequestType iRequest;

    /**
     * java peer
     */
    TSwtPeer iPeer;

    /**
     * dialog parameter,
     * Own
     */
    const HBufC* iText1;


    /**
     * dialog parameter,
     * Own
     */
    const HBufC* iText2;

    /**
     * file dialog parameter
     * Own
     */
    HBufC* iTitle;

    /**
     * dialog parameter
     */
    TInt  iStyle;
    /**
     * dialog parameter
     */
    TInt  iInt1;

    /**
     * dialog parameter
     */
    TInt  iInt2;

    /**
     * dialog parameter
     */
    TInt  iInt3;
};

#endif // SWTDIALOGBROKER_H
