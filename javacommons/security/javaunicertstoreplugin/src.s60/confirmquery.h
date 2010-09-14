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
#ifndef CONFIRMQUERY_H
#define CONFIRMQUERY_H

#include <QEventLoop>

namespace java
{
namespace security
{

class ConfirmQuery: public QObject
{
    Q_OBJECT

public:
    ConfirmQuery();
    bool accept(const QString& aQuery);
    virtual ~ConfirmQuery();

private slots:
    void queryDismissed(int dismissAction);

private:
    // own event loop used for "translating" asynchronous calls into
    // synchronous calls
    QEventLoop* mAsyncToSyncCallEventLoop;
    // boolean for carying the user's input in between two async calls
    bool mQueryAccepted;
};

} // end namespace security
} // end namespace java

#endif //CONFIRMQUERY_H 
