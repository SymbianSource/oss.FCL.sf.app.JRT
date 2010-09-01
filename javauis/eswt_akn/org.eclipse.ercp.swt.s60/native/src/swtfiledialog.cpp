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


#include "swtcommonfiledialogs.h"
#include "swtfiledialog.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtFileDialog::NewL
// ---------------------------------------------------------------------------
//
CSwtFileDialog* CSwtFileDialog::NewL(
    const TDesC& aInitPath,
    const TDesC& aTitle,
    TInt aStyle)
{
    CSwtFileDialog* self = new(ELeave) CSwtFileDialog(aStyle);
    CleanupStack::PushL(self);
    self->ConstructL(aInitPath, aTitle);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtFileDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtFileDialog::ConstructL(
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
// CSwtFileDialog::CSwtFileDialog
// ---------------------------------------------------------------------------
//
CSwtFileDialog::CSwtFileDialog(TInt aStyle)
        : iStyle(aStyle)
{
    if (!((iStyle & KSwtStyleOpen) || (iStyle & KSwtStyleSave)))
    {
        iStyle = KSwtStyleOpen;
    }
}

// ---------------------------------------------------------------------------
// CSwtFileDialog::~CSwtFileDialog
// ---------------------------------------------------------------------------
//
CSwtFileDialog::~CSwtFileDialog()
{
    if (iPath != NULL)
    {
        delete iPath;
        iPath = NULL;
    }
    if (iTitle != NULL)
    {
        delete iTitle;
        iTitle = NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtFileDialog::OpenLD
// From MSwtFileDialog
// ---------------------------------------------------------------------------
//
HBufC* CSwtFileDialog::OpenLD()
{
    TBool dlgSubmited = SwtCommonFileDialogs::RunFileDialogLD(
                            iStyle & KSwtStyleSave ?
                            ESwtFileDialogFileSave :
                            ESwtFileDialogFileSelect,
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
