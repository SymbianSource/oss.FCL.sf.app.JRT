/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#include <e32base.h>
#include <badesca.h>

#include "s60btdialog.h"
#include "logger.h"


using namespace java::bluetooth;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor just sets the priority of this active object to standard priority.
// ---------------------------------------------------------------------------
//
CS60BTDialog::CS60BTDialog()
{
}

// ---------------------------------------------------------------------------
// Connect to notifier.
// ---------------------------------------------------------------------------
//
void CS60BTDialog::ConstructL()
{
    LOG(EJavaBluetooth, EInfo, "+ CS60BTDialog::ConstructL()");
    User::LeaveIfError(mNotifier.Connect());
    LOG(EJavaBluetooth, EInfo, "- CS60BTDialog::ConstructL()");
}


// ---------------------------------------------------------------------------
// Standard implementation
// ---------------------------------------------------------------------------
//
CS60BTDialog* CS60BTDialog::NewLC()
{
    CS60BTDialog* self = new(ELeave) CS60BTDialog;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}


// ---------------------------------------------------------------------------
// Cancel asynchronous request if it is still active
// ---------------------------------------------------------------------------
//
CS60BTDialog::~CS60BTDialog()
{
    JELOG2(EJavaBluetooth);
    LOG(EJavaBluetooth, EInfo, "+ CS60BTDialog::~CS60BTDialog()");
    DoCancel();
    mNotifier.Close();
    LOG(EJavaBluetooth, EInfo, "- CS60BTDialog::~CS60BTDialog()");
}

// ---------------------------------------------------------------------------
// Displays BT query dialog.
// Returns boolean telling user response to query.
// ---------------------------------------------------------------------------
//
TBool CS60BTDialog::ShowBTQueryDialog(
    const TDesC& aAppName, const TBool /*aInstalling*/,
    TBTGenericQueryNoteType aQueryType)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,"+ CS60BTDialog::ShowBTQueryDialog(): aQueryType = %d", aQueryType);

    mQueryParamsPckg().iMessageType = aQueryType;
    mQueryParamsPckg().iNameExists = ETrue;
    mQueryParamsPckg().iName.Copy(aAppName);
    mNotifier.StartNotifierAndGetResponse(mStatus, KBTGenericQueryNotifierUid,
                                          mQueryParamsPckg, mQueryResult);

    User::WaitForRequest(mStatus);

    LOG1(EJavaBluetooth, EInfo,"- CS60BTDialog::ShowBTQueryDialog(): mQueryResult = %d", mQueryResult());
    return mQueryResult();
}


// ---------------------------------------------------------------------------
// Store information about cancel to member variable.
// Cancel asynchronous request.
// Stop the inner active scheduler loop.
// ---------------------------------------------------------------------------
//
void CS60BTDialog::DoCancel()
{
    LOG(EJavaBluetooth, EInfo, "+ CS60BTDialog::DoCancel()");
    mNotifier.CancelNotifier(KBTGenericQueryNotifierUid);
    LOG(EJavaBluetooth, EInfo, "- CS60BTDialog::DoCancel()");
}
