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


#include <aknenv.h>
#include <eswtcore.rsg>
#include "swtcolordialog.h"


const TInt  KColorDialogNumberOfColors = 16;
const TBool KUseNone = EFalse;


// ======== MEMBER FUNCTIONS ========


TRgb* CSwtColorDialog::RunDlgL(TRgb* aDefaultColor)
{
    CSwtColorDialog* dlg = new(ELeave) CSwtColorDialog(aDefaultColor);
    TRgb* rgb = dlg->OpenLD();
    return rgb;
}

CSwtColorDialog::CSwtColorDialog(TRgb* aDefaultColor):
        iDefaultColor(aDefaultColor)
{
}

CSwtColorDialog::~CSwtColorDialog()
{
    iDefaultColor=NULL;
}

TRgb* CSwtColorDialog::OpenLD()
{
    CArrayFixFlat<TRgb>* colors = new(ELeave) CArrayFixFlat<TRgb>(KColorDialogNumberOfColors);
    CleanupStack::PushL(colors);

    CDesCArrayFlat* indexes = CEikonEnv::Static()->ReadDesCArrayResourceL(R_COLORDIALOG_COLORS); // codescanner::eikonenvstatic
    CleanupStack::PushL(indexes);

    TInt cid;
    TInt cnt = indexes->Count();
    TBool defaultFound(EFalse);
    for (TInt i = 0; i < cnt ; i++)
    {
        TLex lex(indexes->MdcaPoint(i));
        User::LeaveIfError(lex.Val(cid));
        TRgb rgb = AKN_LAF_COLOR(cid);
        colors->AppendL(AKN_LAF_COLOR(cid));
        if ((iDefaultColor) && (defaultFound==EFalse) && (rgb==*iDefaultColor))
        {
            defaultFound=ETrue;
        }
    }

    TRgb* c;
    if (iDefaultColor)
    {
        if (defaultFound == EFalse)
        {
            colors->InsertL(0,*iDefaultColor);
        }
        c = new(ELeave) TRgb(iDefaultColor->Red(), iDefaultColor->Green(), iDefaultColor->Blue());
    }
    else
    {
        c=new(ELeave) TRgb(0,0,0);
    }
    CleanupStack::PushL(c);

    TBool okSelected(EFalse);
    CAknColourSelectionGrid *grid= CAknColourSelectionGrid::NewL(colors, KUseNone, okSelected, *c);
    okSelected = grid->ExecuteLD();

    CleanupStack::Pop(c);
    if (!okSelected)
    {
        delete c;
    }
    CleanupStack::PopAndDestroy(indexes);
    CleanupStack::PopAndDestroy(colors);
    delete this;
    return okSelected ? c : NULL ;
}
