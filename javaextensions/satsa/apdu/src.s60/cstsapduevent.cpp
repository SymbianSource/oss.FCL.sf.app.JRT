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


//  INCLUDE FILES
#include "s60commonutils.h"
#include "cstsapduevent.h"
#include "logger.h"

using namespace java::util;

namespace java
{
namespace satsa
{

CSTSApduEvent::CSTSApduEvent(JNIEnv& aJni, jobject aNotifyObject,
                             jmethodID aHandleEventMethod) :
        iJni(aJni), iListenerObject(aNotifyObject), iHandleEventMethod(
            aHandleEventMethod)
{
}

CSTSApduEvent::~CSTSApduEvent()
{
}

// -----------------------------------------------------------------------------
// CSTSApduEvent::SetId
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduEvent::SetId(TInt aId)
{
    LOG(ESATSA, EInfo, "CSTSApduEvent::SetId");
    iId = aId;
}

// -----------------------------------------------------------------------------
// CSTSApduEvent::SetErrorCode
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduEvent::SetErrorCode(TInt aError)
{
    LOG(ESATSA, EInfo, "CSTSApduEvent::SetErrorCode");
    iError = aError;
}

// -----------------------------------------------------------------------------
// CSTSApduEvent::SetData
// Sets member variable to point to the response data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSApduEvent::SetData(HBufC8* aResponseData)
{
    LOG(ESATSA, EInfo, "CSTSApduEvent::SetData");
    iResponseData = aResponseData;
}

// -----------------------------------------------------------------------------
// CSTSApduEvent::Dispatch
// from CJavaEvent. Copies response data to java side and calls java side method
// with parameters set before.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSApduEvent::Dispatch(JNIEnv* aJni, jobject aPeer)
{
    LOG(ESATSA, EInfo, "+ CSTSApduEvent::Dispatch");

    // create java byte array
    jbyteArray byteArray;
    if (iResponseData)
    {
        LOG(ESATSA, EInfo, "CSTSApduEvent::Dispatch: Responsedata is ready");
        byteArray = aJni->NewByteArray(iResponseData->Size());
        if (byteArray)
        {
            LOG(ESATSA, EInfo, "CSTSApduEvent::Dispatch: create byteArray now");
            S60CommonUtils::CopyToJava((*aJni), *iResponseData, byteArray, 0,
                                       iResponseData->Size());
        }
    }
    else
    {
        LOG(ESATSA, EInfo, "CSTSApduEvent::Dispatch: responseData is NOT ready!");
        byteArray = aJni->NewByteArray(0);
    }

    if (!byteArray)
    {
        ELOG(ESATSA, "STS::CSTSApduEvent::Dispatch - Failed to create ByteArray");
        iError = KErrNoMemory;
    }
    LOG(ESATSA, EInfo, "call java side method with parameters");
    // call java side method with parameters
    jclass sessionClass = aJni->FindClass(
                              "com/nokia/mj/impl/satsa/APDUConnectionImpl");
    jmethodID mOperationCompleteCallBack = aJni->GetMethodID(sessionClass,
                                           "operationComplete", "(II[B)V");
    aJni->CallVoidMethod(aPeer, mOperationCompleteCallBack, iId, iError,
                         byteArray);

}

} // namespace satsa
} // namespace java
//  END OF FILE
