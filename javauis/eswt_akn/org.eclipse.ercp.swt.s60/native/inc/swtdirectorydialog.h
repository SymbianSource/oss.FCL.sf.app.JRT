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


#ifndef SWTDIRECTORYDIALOG_H
#define SWTDIRECTORYDIALOG_H


#include "eswtwidgetsexpanded.h"


/**
 * CSwtDirectoryDialog
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtDirectoryDialog)
        : public CBase
        , public MSwtDirectoryDialog
{
public:
    /**
     * Two phase constructor.
     * @param aInitPath full path of the directory that will be selected by
     *        default when the dialog opens.
     * @param aTitle title of the dialog.
     * @return CSwtDirectoryDialog pointer
     */
    static CSwtDirectoryDialog* NewL(const TDesC& aInitPath,
    const TDesC& aTitle);

protected:
    /**
     * Second phase constructor.
     * @param aInitPath full path of the directory that will be selected by
     *        default when the dialog opens.
     * @param aTitle title of the dialog.
     * @return CSwtDirectoryDialog pointer
     */
    void ConstructL(const TDesC& aInitPath, const TDesC& aTitle);

    /**
     * First phase constructor.
     */
    CSwtDirectoryDialog();

    /**
     * Destructor.
     */
    ~CSwtDirectoryDialog();

// From MSwtDirectoryDialog
public:
    HBufC* OpenLD();

// From MSwtDirectoryDialog unused in S60
public:
    void Dispose();
    void SetTitleFilterPathAndMessage(const TDesC* aTitle,
                                      const TDesC* aFilterPath, const TDesC* aMessage);
    TBool OpenL();
    const TDesC* GetFilterPath() const;
// Data
private:
    /**
     * Path to the selected directory.
     * Own
     */
    HBufC* iPath;

    /**
     * The title of the directory selection dialog.
     * Own
     */
    HBufC* iTitle;
};

#endif // SWTDIRECTORYDIALOG_H
