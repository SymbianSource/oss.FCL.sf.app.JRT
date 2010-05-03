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


#include <AknCommonDialogs.h>
#include <CAknCommonDialogsBase.h>
#include <CAknFileSelectionDialog.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <CAknFileNamePromptDialog.h>
#include <f32file.h>
#include <coemain.h>
#include <eswtcore.rsg>
#include "swtcommonfiledialogs.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// SwtCommonFileDialogs::RunFileDialogLD
// ---------------------------------------------------------------------------
//
TBool SwtCommonFileDialogs::RunFileDialogLD(
    TSwtFileDialogType aFileDialogType,
    const TDesC& aTitle,
    HBufC* aPath)
{
    ASSERT(aPath);
    TBool res = EFalse;
    TDriveNumber memSelected;
    HBufC* path = HBufC::NewLC(KMaxPath);
    TPtr pathPtr = path->Des();
    HBufC* filename = HBufC::NewLC(KMaxPath);
    TPtr filenamePtr = filename->Des();
    HBufC* temp = HBufC::NewLC(KMaxPath);
    TPtr tempPtr = temp->Des();

    // Starting path for all modes
    TBool useMemDlg = ETrue;
    TParse pathParse;
    pathParse.Set(*aPath, NULL, NULL);
    if (pathParse.DrivePresent())
    {
        pathPtr.Append(pathParse.DriveAndPath());
        useMemDlg = EFalse;
    }

    // Starting filename for save mode only
    if (pathParse.NameOrExtPresent())
    {
        filenamePtr.Append(pathParse.NameAndExt());
    }

    CAknMemorySelectionDialogMultiDrive* memDlg = NULL;
    CAknFileSelectionDialog* fileDlg = NULL;
    CAknFileNamePromptDialog* promptDlg = NULL;
    TBool dlgRes = EFalse;

    // Choose an Avkon file selection dialog type
    TCommonDialogType memDlgType;
    TCommonDialogType fileDlgType;
    switch (aFileDialogType)
    {
    case ESwtFileDialogFolderSelect:
        memDlgType = ECFDDialogTypeBrowse;
        fileDlgType = ECFDDialogTypeBrowse;
        break;

    case ESwtFileDialogFileSave:
        memDlgType = ECFDDialogTypeSave;
        fileDlgType = ECFDDialogTypeSave;
        break;

    case ESwtFileDialogFileSelect:
        // Fall trough
    default:
        memDlgType = ECFDDialogTypeBrowse;
        fileDlgType = ECFDDialogTypeSelect;
        break;
    }

    // Memory selection dialog loop
    TBool runMemDlgLoop = ETrue;
    while (runMemDlgLoop)
    {
        if (useMemDlg)
        {
            // Run memory selection dialog if there was no starting path.
            memDlg = CAknMemorySelectionDialogMultiDrive::NewL(memDlgType, EFalse);
            CleanupStack::PushL(memDlg);
            memDlg->SetTitleL(aTitle);
            dlgRes = memDlg->ExecuteL(memSelected, &pathPtr, &tempPtr);
            CleanupStack::PopAndDestroy(memDlg);

            // Exit loop if memory selection cancelled.
            if (!dlgRes)
            {
                break;
            }
        }
        else
        {
            // No use of memory selection dialog.
            runMemDlgLoop = EFalse;
        }

        // Create file selection dialog
        if (aFileDialogType == ESwtFileDialogFolderSelect)
        {
            fileDlg = CAknFileSelectionDialog::NewL(
                          fileDlgType,
                          R_SWT_FOLDER_SELECTION_DIALOG);
        }
        else
        {
            fileDlg = CAknFileSelectionDialog::NewL(fileDlgType);
        }
        CleanupStack::PushL(fileDlg);
        fileDlg->SetTitleL(aTitle);

        // Set right soft key to 'Back' if memory selection is used
        if (useMemDlg)
        {
            CCoeEnv* coeEnv = CCoeEnv::Static();
            HBufC* rskTxt = coeEnv->AllocReadResourceLC(
                                R_SWT_SOFTKEY_TEXT_BACK);
            fileDlg->SetRightSoftkeyRootFolderL(*rskTxt);
            CleanupStack::PopAndDestroy(rskTxt);
        }

        // Run file selection dialog
        dlgRes = fileDlg->ExecuteL(pathPtr);
        if (dlgRes)
        {
            // File selection dialog submitted, no need for going
            // back to memory selection.
            runMemDlgLoop = EFalse;

            // Run filename prompt dialog if save style is used
            if (fileDlgType == ECFDDialogTypeSave)
            {
                promptDlg = CAknFileNamePromptDialog::NewL();
                CleanupStack::PushL(promptDlg);
                promptDlg->SetTitleL(aTitle);
                promptDlg->SetPathL(pathPtr);
                dlgRes = promptDlg->ExecuteL(filenamePtr);
                CleanupStack::PopAndDestroy(promptDlg);
                if (dlgRes)
                {
                    pathPtr.Append(filenamePtr);
                }
            }
        }

        // Destroy file selection dialog
        CleanupStack::PopAndDestroy(fileDlg);
    }

    // Return the selected path or an empty string
    if (dlgRes)
    {
        res = ETrue;
        *aPath = pathPtr;
    }
    else
    {
        res = EFalse;
        aPath = NULL;
    }

    CleanupStack::PopAndDestroy(temp);
    CleanupStack::PopAndDestroy(filename);
    CleanupStack::PopAndDestroy(path);

    return res;
}
