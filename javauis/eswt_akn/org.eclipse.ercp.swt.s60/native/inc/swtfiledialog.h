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


#ifndef SWTFILEDIALOG_H
#define SWTFILEDIALOG_H


#include "eswtwidgetscore.h"


/**
 * CSwtFileDialog
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtFileDialog)
        : public CBase
        , public MSwtFileDialog
{
public:
    /**
     * Two phase constructor.
     * @param aInitPath full path of the file that will be selected by
     *        default when the dialog opens.
     * @param aTitle title of the dialog.
     * @param aStyle Supported styles: KSwtStyleOpen or KSwtStyleSave.
     * @return CSwtFileDialog pointer
     */
    static CSwtFileDialog* NewL(const TDesC& aInitPath,
    const TDesC& aTitle, TInt aStyle);

protected:
    /**
     * Second phase constructor.
     * @param aInitPath initial path to the file that will be selected
     *        by default when the dialog is opened.
     * @param aTitle title of the dialog.
     */
    void ConstructL(const TDesC& aInitPath, const TDesC& aTitle);

    /**
     * First phase Constructor.
     * @param aStyle Supported styles: KSwtStyleOpen or KSwtStyleSave.
     */
    CSwtFileDialog(TInt aStyle);

    /**
     * Destructor.
     */
    virtual ~CSwtFileDialog();

// From MSwtFileDialog
public:
    HBufC* OpenLD();

private: // Data
    /**
     * Path to the selected file.
     * Own
     */
    HBufC* iPath;

    /**
     * The title of the file selection dialog.
     * Own
     */
    HBufC* iTitle;

    /**
     * Supported styles: KSwtStyleOpen or KSwtStyleSave.
     */
    TInt iStyle;
};

#endif // SWTFILEDIALOG_H
