/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TESTEXECUTOR_H
#define TESTEXECUTOR_H

#include <e32base.h>

class CTestExecutor : public CActive
{
public:

    CTestExecutor();

    virtual ~CTestExecutor();

    //CActive
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

    TRequestStatus& GetRequestStatus();

    TInt GetErrorCode() const;

    void Activate();

private:

    TInt mErrCode;
};

#endif // TESTEXECUTOR_H

