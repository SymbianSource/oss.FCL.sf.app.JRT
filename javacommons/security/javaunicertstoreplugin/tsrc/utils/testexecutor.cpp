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

#include "testexecutor.h"
#include "logger.h"

CTestExecutor::CTestExecutor()
        : CActive(CActive::EPriorityStandard),mErrCode(0)
{
    CActiveScheduler::Add(this);
    SetActive();
}

CTestExecutor::~CTestExecutor()
{
    if (IsActive())
    {
        Cancel();
    }
}

void CTestExecutor::RunL()
{
    JELOG2(EJavaSecurity);
    mErrCode = iStatus.Int();
    CActiveScheduler::Stop();
}

void CTestExecutor::DoCancel()
{
    JELOG2(EJavaSecurity);
    //if(IsActive())
    //{
    //  CActiveScheduler::Stop();
    //  mErrCode = KErrCancel;
    //}
}

TInt CTestExecutor::RunError(TInt /*aError*/)
{
    JELOG2(EJavaSecurity);
    return KErrNone;
}

TRequestStatus& CTestExecutor::GetRequestStatus()
{
    return iStatus;
}

TInt CTestExecutor::GetErrorCode() const
{
    return mErrCode;
}

void CTestExecutor::Activate()
{
    SetActive();
}

