/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "confirmquery.h"
#include <hbmessagebox.h>
#include <hblabel.h>

using namespace java::security;

ConfirmQuery::ConfirmQuery()
    : mAsyncToSyncCallEventLoop(0), mQueryAccepted(false)
{
    mAsyncToSyncCallEventLoop = new QEventLoop();
}
    
bool ConfirmQuery::accept(const QString& aQuery)
{
    HbMessageBox confirmQuery(HbMessageBox::MessageTypeQuestion);
    confirmQuery.setDismissPolicy(HbDialog::NoDismiss);
    confirmQuery.setTimeout(HbDialog::NoTimeout);
    confirmQuery.setIconVisible(false);
    confirmQuery.setText(aQuery);
    confirmQuery.setAttribute(Qt::WA_DeleteOnClose);
    confirmQuery.setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);
    confirmQuery.open(this,SLOT(queryDismissed(int)));
    mAsyncToSyncCallEventLoop->exec();
    return mQueryAccepted;
}

void ConfirmQuery::queryDismissed(int dismissAction)
{
    if (dismissAction == HbMessageBox::Yes) {
        mQueryAccepted = true;
    }
    mAsyncToSyncCallEventLoop->exit();
}

ConfirmQuery::~ConfirmQuery()
{
    delete mAsyncToSyncCallEventLoop;
}

#include "moc_confirmquery.cpp"
