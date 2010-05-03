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


#ifndef SWTCOMMONFILEDIALOGS_H
#define SWTCOMMONFILEDIALOGS_H


#include <e32def.h>
#include <e32cmn.h>


enum TSwtFileDialogType
{
    ESwtFileDialogFileSelect = 0,
    ESwtFileDialogFileSave,
    ESwtFileDialogFolderSelect
};


/**
 * SwtCommonFileDialogs
 * @lib eswt
 */
NONSHARABLE_CLASS(SwtCommonFileDialogs)
{
public:
    /**
     * Run file or folder selection dialog(s). If the given path is
     * empty a memory selection dialog is run first. Next a file or
     * folder selection dialog will be run. In the case of a file
     * save dialog a prompt dialog will be run in the end.
     * @param aFileDialogType See TSwtFileDialogType.
     * @param aPath Cannot be NULL. The path to the selected folder
     *        or file goes here. If initial path is given, the dialog
     *        will be browsing only from that path onwords.
     * @param aTitle title to be applied to the dialog(s).
     * @return ETrue if dialog submitted, EFalse if dialog cancelled
     */
    static TBool RunFileDialogLD(
        TSwtFileDialogType aFileDialogType,
        const TDesC& aTitle,
        HBufC* aPath);
};

#endif // SWTCOMMONFILEDIALOGS_H
