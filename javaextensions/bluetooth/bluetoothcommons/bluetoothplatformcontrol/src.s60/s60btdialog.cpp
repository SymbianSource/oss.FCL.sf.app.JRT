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

#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
#include <hbdevicemessageboxsymbian.h>
#include <hbpopup.h>
#include <QtCore/qvariant.h>

#define QUERY_BUF_LEN 512
#endif

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
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    User::LeaveIfError(mNotifier.Connect());
#endif
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
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    self->ConstructL();
#endif
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

#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    DoCancel();
    mNotifier.Close();
#endif
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


#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    mQueryParamsPckg().iMessageType = aQueryType;
    mQueryParamsPckg().iNameExists = ETrue;
    mQueryParamsPckg().iName.Copy(aAppName);
    mNotifier.StartNotifierAndGetResponse(mStatus, KBTGenericQueryNotifierUid,
                                          mQueryParamsPckg, mQueryResult);
    User::WaitForRequest(mStatus);

#else
    _LIT(KAppInfo,"Application %S cannot  be used when  Bluetooth is off Activate Bluetooth ?");
    _LIT(KAppInfomsg,"Change 'My phone's visibility' setting to 'Show to all'? Application '%S' cannot be used when the visibility is set as 'Hidden'.");

    TBuf<QUERY_BUF_LEN> queryBuf;
    TInt err = KErrNone;
    TBool userOpt;

    queryBuf.Zero();
    if (aQueryType == EBTIsOffJavaQuery)
    {
        queryBuf.Format(KAppInfo,&aAppName);
    }
    else if (aQueryType == EBTIsNotShownQuery)
    {
        queryBuf.Format(KAppInfomsg,&aAppName);
    }

    TRAP(err, userOpt = ShowBTQueryHbDialogL(queryBuf));
    LOG1(EJavaBluetooth, EInfo,"  CS60BTDialog::ShowBTQueryDialog(): mQueryResult = %d", err);

    if (err != KErrNone )
        mQueryResult = EFalse;
    else
        mQueryResult = userOpt;
#endif

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
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    mNotifier.CancelNotifier(KBTGenericQueryNotifierUid);
#endif
    LOG(EJavaBluetooth, EInfo, "- CS60BTDialog::DoCancel()");
}

#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS

TBool CS60BTDialog::ShowBTQueryHbDialogL(const TDesC& aBufData)
{
    TBool result = EFalse;

    CHbDeviceMessageBoxSymbian* messageBox
    = CHbDeviceMessageBoxSymbian::NewL(CHbDeviceMessageBoxSymbian::EQuestion);
    CleanupStack::PushL(messageBox);

    messageBox->SetTextL(aBufData);
    messageBox->SetTimeout(HbPopup::NoTimeout);

    // Read localised versions instead of hard coded values.
    _LIT(KAllowButtonText, "Yes");
    _LIT(KDenyButtonText, "No");

    messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::EAcceptButton, KAllowButtonText);
    messageBox->SetButton(CHbDeviceMessageBoxSymbian::EAcceptButton, ETrue);
    messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::ERejectButton, KDenyButtonText);
    messageBox->SetButton(CHbDeviceMessageBoxSymbian::ERejectButton, ETrue);

    if (messageBox->ExecL() == CHbDeviceMessageBoxSymbian::EAcceptButton)
        result = ETrue;
    else
        result = EFalse;

    messageBox->Close();
    CleanupStack::PopAndDestroy(messageBox);

    return result;
}
#endif
