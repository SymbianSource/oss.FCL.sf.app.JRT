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


#include <swtcommonfiledialogs.h>
#include "swtdirectorydialog.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtDirectoryDialog::NewL
// ---------------------------------------------------------------------------
//
CSwtDirectoryDialog* CSwtDirectoryDialog::NewL(
    const TDesC& aInitPath,
    const TDesC& aTitle)
{
    CSwtDirectoryDialog* self = new(ELeave) CSwtDirectoryDialog();
    CleanupStack::PushL(self);
    self->ConstructL(aInitPath, aTitle);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtDirectoryDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtDirectoryDialog::ConstructL(
    const TDesC& aInitPath,
    const TDesC& aTitle)
{
    // Path
    iPath = HBufC::NewL(KMaxPath);
    *iPath = aInitPath;

    // Title
    iTitle = HBufC::NewL(aTitle.Length());
    *iTitle = aTitle;
}

// ---------------------------------------------------------------------------
// CSwtDirectoryDialog::CSwtDirectoryDialog
// ---------------------------------------------------------------------------
//
CSwtDirectoryDialog::CSwtDirectoryDialog()
{
}

// ---------------------------------------------------------------------------
// CSwtDirectoryDialog::~CSwtDirectoryDialog
// ---------------------------------------------------------------------------
//
CSwtDirectoryDialog::~CSwtDirectoryDialog()
{
    if (iPath)
    {
        delete iPath;
        iPath = NULL;
    }
    if (iTitle)
    {
        delete iTitle;
        iTitle = NULL;
    }
}

// ---------------------------------------------------------------------------
// From MSwtDirectoryDialog
// CSwtDirectoryDialog::OpenLD
// ---------------------------------------------------------------------------
//
HBufC* CSwtDirectoryDialog::OpenLD()
{
    TBool dlgSubmited = SwtCommonFileDialogs::RunFileDialogLD(
                            ESwtFileDialogFolderSelect,
                            *iTitle, iPath);
    HBufC* result = NULL;
    if (dlgSubmited)
    {
        ASSERT(iPath);
        result = iPath->AllocL();
    }
    delete this;
    return result;
}
