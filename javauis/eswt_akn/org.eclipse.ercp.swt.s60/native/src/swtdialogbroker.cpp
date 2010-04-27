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


#include "utils.h"
#include "swtdialogbroker.h"
#include "swtdisplay.h"
#include "eswtwidgetscore.h"
#include "eswtwidgetsexpanded.h"
#include "eswtmobileextensions.h"


// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CSwtDialogBroker::CSwtDialogBroker
// Description: CSwtDialogBroker constructer
// -----------------------------------------------------------------------------
//
CSwtDialogBroker::CSwtDialogBroker(CSwtDisplay* aDisplay, TRequestType aRequest, TInt aPriority)
        :CActive(aPriority), iDisplay(aDisplay), iRequest(aRequest)
{
    CActiveScheduler::Add(this);
}

//-----------------------------------------------------------------------------
// CSwtDialogBroker:: CSwtDialogBroker
// Description: CSwtDialogBroker Destructer
// -----------------------------------------------------------------------------
//
CSwtDialogBroker::~CSwtDialogBroker()
{
    Cancel();
    delete iText1;
    delete iText2,
    delete iTitle;
}



//-----------------------------------------------------------------------------
// CSwtDialogBroker::RequestFunction
// Description: Request Function for CSwtDialogBroker
// ----------------------------------------------------------------------------
//
void CSwtDialogBroker::RequestRunDialogL(TSwtPeer aPeer, TInt aRequest,
        TInt aStyle, const TDesC& aTitle, const TDesC& aText1, const TDesC& aText2)
{
    iPeer = aPeer;
    iRequest = static_cast< TRequestType >(aRequest);
    delete iText1;
    iText1 = NULL;
    iText1 = aText1 .AllocL();
    delete iText2;
    iText2 = NULL;
    iText2 = aText2 .AllocL();
    delete iTitle;
    iTitle = NULL;
    iTitle = aTitle.AllocL();
    iStyle = aStyle;

    iStatus = KRequestPending;
    if (!IsActive())
    {
        SetActive();
    }
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
}

void CSwtDialogBroker::SetExtraData(const TInt aInt1, const TInt aInt2, const TInt aInt3)
{
    iInt1 = aInt1;
    iInt2 = aInt2;
    iInt3 = aInt3;
}


//-----------------------------------------------------------------------------
// CSwtDialogBroker::DoCancel
// ----------------------------------------------------------------------------
//
void CSwtDialogBroker::DoCancel()
{
}

TInt CSwtDialogBroker::RunError(TInt aError)
{
    delete iTitle;
    iTitle = NULL;
    delete iText1;
    iText1 = NULL;
    delete iText2;
    iText2 = NULL;

    // notify the error
    if (iPeer)
    {
        TRAP_IGNORE(iDisplay->PostDialogResultEventL(this, iPeer, -2, aError, 0));
    }

    // Complete with error
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aError);
    return KErrNone;
}


//-----------------------------------------------------------------------------
// CSwtDialogBroker::RunL
// Description: CActive::RunL implementation which will start a dialog
//-----------------------------------------------------------------------------
//
void CSwtDialogBroker::RunL()
{
    switch (iRequest)
    {
    case EFileDialog:
        RunFileDialogL();
        break;
    case EMessageBox:
        RunMessageBoxL();
        break;
    case EDirectoryDialog:
        RunDirectoryDialogL();
        break;
    case EColorDialog:
        RunColorDialogL();
        break;
    case EQueryDialog:
        RunQueryDialogL();
        break;
    case ETimedMessageBox:
        RunTimedMessageBoxL();
        break;
    case ENoRequest:
    default:
        break;
    }
}

//-----------------------------------------------------------------------------
// CSwtDialogBroker::RunFileDialogL
// Description: run FileDialog and send result to tis java counterpart
//-----------------------------------------------------------------------------
//
void CSwtDialogBroker::RunFileDialogL()
{
    MSwtFileDialog* dialog = iDisplay->Factory().NewFileDialogL(*iText1, *iTitle, iStyle);
    HBufC* resultPath(NULL);
    resultPath = dialog->OpenLD();
    if (!resultPath)
    {
        resultPath = HBufC::NewL(0);
    }
    CleanupStack::PushL(resultPath);

    // this event is to notify java side with dialog result
    iDisplay->PostDialogResultEventL(this, iPeer, *resultPath);

    CleanupStack::PopAndDestroy(resultPath);

    delete iText1;
    iText1 = NULL;
    delete iTitle;
    iTitle = NULL;
}


//-----------------------------------------------------------------------------
// CSwtDialogBroker::RunMessageBoxL
// Description: run MesssageBox and send result to tis java counterpart
//-----------------------------------------------------------------------------
//
void CSwtDialogBroker::RunMessageBoxL()
{
    TInt res = iDisplay->UiUtils().RunMessageBoxDlgLD(*iDisplay, *iTitle, *iText1, iStyle);

    // this event is to notify java side with dialog result
    iDisplay->PostDialogResultEventL(this, iPeer, res);

    delete iText1;
    iText1 = NULL;
    delete iTitle;
    iTitle = NULL;
}

//-----------------------------------------------------------------------------
// CSwtDialogBroker::DirectoryDialogL
// Description: run DirectoryDialog and send result to tis java counterpart
//-----------------------------------------------------------------------------
//
void CSwtDialogBroker::RunDirectoryDialogL()
{
    MSwtDirectoryDialog* dialog = iDisplay->Factory().NewDirectoryDialogL(*iText1, *iTitle);

    // Open the dialog
    HBufC* resultPath = NULL;
    resultPath = dialog->OpenLD();
    if (!resultPath)
    {
        resultPath = HBufC::NewL(0);
    }
    CleanupStack::PushL(resultPath);

    // this event is to notify java side with dialog result
    iDisplay->PostDialogResultEventL(this, iPeer, *resultPath);

    CleanupStack::PopAndDestroy(resultPath);

    delete iTitle;
    iTitle = NULL;
    delete iText1;
    iText1 = NULL;
}

//-----------------------------------------------------------------------------
// CSwtDialogBroker::RunColorDialogL
// Description: run RunColorDialog and send result to tis java counterpart
//-----------------------------------------------------------------------------
//
void CSwtDialogBroker::RunColorDialogL()
{
    TRgb* defaultColor(NULL);
    TRgb* selectedColor(NULL);
    if (iInt1 > -1)
    {
        defaultColor = new(ELeave) TRgb(iInt1, iInt2, iInt3);
        CleanupStack::PushL(defaultColor);
    }

    selectedColor = iDisplay->Factory().RunColorDialogL(defaultColor);

    if (defaultColor)
    {
        CleanupStack::PopAndDestroy(defaultColor);
    }

    TInt resRed(-1);
    TInt resGreen(-1);
    TInt resBlue(-1);
    if (selectedColor)
    {
        resRed = selectedColor->Red();
        resGreen = selectedColor->Green();
        resBlue = selectedColor->Blue();
        delete selectedColor;
    }

    // this event is to notify java side with dialog result
    iDisplay->PostDialogResultEventL(this, iPeer, resRed, resGreen, resBlue);
}

//-----------------------------------------------------------------------------
// CSwtDialogBroker::RunQueryDialogL
// Description: run RunQueryDialog and send result to tis java counterpart
//-----------------------------------------------------------------------------
//
void CSwtDialogBroker::RunQueryDialogL()
{
    HBufC* result(NULL);
    result = iDisplay->Factory().RunQueryDialogL(*iTitle,
             iStyle, iInt1, *iText1, *iText2, iInt2, iInt3);

    if (!result)
    {
        result = HBufC::NewL(0);
    }
    CleanupStack::PushL(result);

    // this event is to notify java side with dialog result
    iDisplay->PostDialogResultEventL(this, iPeer, *result);

    CleanupStack::PopAndDestroy(result);
    delete iTitle;
    iTitle = NULL;
    delete iText1;
    iText1 = NULL;
    delete iText2;
    iText2 = NULL;
}

//-----------------------------------------------------------------------------
// CSwtDialogBroker::RunTimedMessageBoxL()
// Description: run RunTimedMessageBox
//-----------------------------------------------------------------------------
//
void CSwtDialogBroker::RunTimedMessageBoxL()
{
    MSwtImage* image = reinterpret_cast< MSwtImage* >(iInt1);
    iDisplay->Factory().RunTimedMessageBoxDlgLD(*iTitle, *iText1, iStyle, image);
    // this event is to notify java side
    iDisplay->PostDialogResultEventL(this, iPeer, 0);
}

