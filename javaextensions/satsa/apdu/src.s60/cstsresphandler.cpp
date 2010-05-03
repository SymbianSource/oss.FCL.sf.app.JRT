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


// INCLUDE FILES
#include "cstsresphandler.h"
#include "cstsapduevent.h"
#include "logger.h"

namespace java
{
namespace satsa
{

CSTSRespHandler::CSTSRespHandler()
{

}

void CSTSRespHandler::ConstructL(JNIEnv& aJni, jobject aNotifyObject,
                                 jmethodID aHandleEventMethod)
{
    iApduEvent
    = new(ELeave) CSTSApduEvent(aJni, aNotifyObject, aHandleEventMethod);
}

CSTSRespHandler* CSTSRespHandler::NewLC(JNIEnv& aJni, jobject aNotifyObject,
                                        jmethodID aHandleEventMethod)
{
    LOG(ESATSA, EInfo, "+ CSTSRespHandler::NewLC");
    CSTSRespHandler* self = new(ELeave) CSTSRespHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aJni, aNotifyObject, aHandleEventMethod);
    LOG(ESATSA, EInfo, "+ CSTSRespHandler::NewLC");
    return self;
}

CSTSRespHandler* CSTSRespHandler::NewL(JNIEnv& aJni, jobject aNotifyObject,
                                       jmethodID aHandleEventMethod)
{
    CSTSRespHandler* self = CSTSRespHandler::NewLC(aJni, aNotifyObject,
                            aHandleEventMethod);
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSRespHandler::~CSTSRespHandler()
{
    delete iApduEvent;
}

// --------------------------------------------
// CSTSRespHandler::SetOperationId
// Setter for operation id
// --------------------------------------------
void CSTSRespHandler::SetOperationId(TInt aId)
{
    LOG(ESATSA, EInfo, "+ CSTSRespHandler::SetOperationId");
    iApduEvent->SetId(aId);
}

void CSTSRespHandler::OperationComplete(JNIEnv* aJni, jobject aPeer,
                                        HBufC8* aResponse, TInt aError)
{
    LOG(ESATSA, EInfo, "+ CSTSRespHandler::OperationComplete");
    TInt error = aError;
    iApduEvent->SetData(aResponse);
    iApduEvent->SetErrorCode(error);
    iApduEvent->Dispatch(aJni, aPeer);
    LOG(ESATSA, EInfo, "-- CSTSRespHandler::OperationComplete");
}

} // namespace satsa
} // namespace java
//  End of File
