/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef GLOBALMSGOBSERVER_H
#define GLOBALMSGOBSERVER_H

#include <e32base.h>
#include <e32def.h>

namespace java
{
namespace runtimeui
{

class GlobalMsgObserver :
        public CActive
{
public:
    IMPORT_C static GlobalMsgObserver* NewL();
    IMPORT_C static GlobalMsgObserver* NewLC();
    IMPORT_C int getAnswer();
    virtual ~GlobalMsgObserver();

private:
    inline GlobalMsgObserver() : CActive(CActive::EPriorityStandard) {};
    void ConstructL();
    // From CActive
    void DoCancel();
    void RunL();
    void Complete();
    CActiveScheduler* iActiveScheduler;
    CActiveSchedulerWait* iActiveSchedulerWait;
};

} //end namespace runtimeui
} //end namespace java

#endif // GLOBALMSGOBSERVER_H
