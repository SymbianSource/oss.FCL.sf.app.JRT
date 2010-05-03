/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java RemCon Observer Active Object
*
*/


#include "javaremconobserver.h"
#include <jdebug.h>

const TInt KMaxQueueSize(32);

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CJavaRemConObserver::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRemConObserver* CJavaRemConObserver::NewL(
    MRemConCoreApiTargetObserver& aObserver)
{
    CJavaRemConObserver* self = new(ELeave) CJavaRemConObserver(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRemConObserver::CJavaRemConObserver()
// ---------------------------------------------------------------------------
//
CJavaRemConObserver::CJavaRemConObserver(
    MRemConCoreApiTargetObserver& aObserver) :
        CActive(CActive::EPriorityStandard),
        iObserver(aObserver)
{
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// CJavaRemConObserver::ConstructL()
// ---------------------------------------------------------------------------
//
void CJavaRemConObserver::ConstructL()
{
    iQueue.CreateLocal(KMaxQueueSize);
    IssueRequest();
}

// ---------------------------------------------------------------------------
// CJavaRemConObserver::IssueRequest()
// ---------------------------------------------------------------------------
//
void CJavaRemConObserver::IssueRequest()
{
    iQueue.NotifyDataAvailable(iStatus);
    SetActive();
}

// ---------------------------------------------------------------------------
// CJavaRemConObserver::~CJavaRemConObserver()
// ---------------------------------------------------------------------------
//
CJavaRemConObserver ::~CJavaRemConObserver()
{
    Cancel();
    iQueue.Close();
}

// ---------------------------------------------------------------------------
// CJavaRemConObserver::DoCancel()
// ---------------------------------------------------------------------------
//
void CJavaRemConObserver::DoCancel()
{
    iQueue.CancelDataAvailable();
}

// ---------------------------------------------------------------------------
// CJavaRemConObserver::AddPlayMessage()
// ---------------------------------------------------------------------------
//
void CJavaRemConObserver::AddPlayMessage(TRemConCoreApiPlaybackSpeed aSpeed,
        TRemConCoreApiButtonAction aButtonAct)
{
    DEBUG_INT2("CJavaRemConObserver::SetPlayParameters - aSpeed = %d, aButtonAct = %d", aSpeed, aButtonAct);
    TJavaRemConMessage message;
    message.iMethod = EMethodMrccatoPlay;
    message.iSpeed = aSpeed;
    message.iButtonAct = aButtonAct;
    iQueue.Send(message);
}

// ---------------------------------------------------------------------------
// CJavaRemConObserver::AddCommandMessage()
// ---------------------------------------------------------------------------
//
void CJavaRemConObserver::AddCommandMessage(
    TRemConCoreApiOperationId aOperationId,
    TRemConCoreApiButtonAction aButtonAct)
{
    DEBUG_INT2("CJavaRemConObserver::SetCommandParameters - aOperationId = %d, aButtonAct = %d", aOperationId, aButtonAct);
    TJavaRemConMessage message;
    message.iMethod = EMethodMrccatoCommand;
    message.iOperationId = aOperationId;
    message.iButtonAct = aButtonAct;
    iQueue.Send(message);
}

// ---------------------------------------------------------------------------
// CJavaRemConObserver::RunL()
// ---------------------------------------------------------------------------
//
void CJavaRemConObserver::RunL()
{
    DEBUG("+CJavaRemConObserver::RunL() ");
    TJavaRemConMessage message;
    iQueue.Receive(message);
    switch (message.iMethod)
    {
    case EMethodNone:
        DEBUG("CJavaRemConObserver::RunL - EMethodNone");
        break;
    case EMethodMrccatoCommand:
        DEBUG_INT2("CJavaRemConObserver::RunL - EMethodMrccatoCommand: aOperationId = %d, aButtonAct = %d", message.iOperationId, message.iButtonAct);
        iObserver.MrccatoCommand(message.iOperationId,
                                 message.iButtonAct);
        break;
    case EMethodMrccatoPlay:
        DEBUG_INT2("CJavaRemConObserver::RunL - EMethodMrccatoPlay: iSpeed = %d, aButtonAct = %d", message.iSpeed, message.iButtonAct);
        iObserver.MrccatoPlay(message.iSpeed,
                              message.iButtonAct);
        break;
    default:
        break;
    }
    IssueRequest();
    DEBUG("-CJavaRemConObserver::RunL() ");
}
