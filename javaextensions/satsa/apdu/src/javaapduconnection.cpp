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


#include "logger.h"
#include "javaapduconnection.h"

namespace java
{
namespace satsa
{

JavaApduConnection::JavaApduConnection() :
        mApduConnBase(0)
{

}

JavaApduConnection* JavaApduConnection::getInstance()
{
    JELOG2(ESATSA);
    JavaApduConnection* apduConn = new JavaApduConnection();
    apduConn->Initialize();
    return apduConn;

}

void JavaApduConnection::Initialize()
{
    JELOG2(ESATSA);
    mApduConnBase = JavaApduConnectionBase::getInstance();

}

JavaApduConnection::~JavaApduConnection()
{
    JELOG2(ESATSA);
    delete mApduConnBase;
    mApduConnBase = NULL;

}

int JavaApduConnection::Create(JNIEnv& aJni, jstring aUri, jstring aName,
                               jstring aVendor, jstring aVersion, jstring aDomain,
                               jstring aRootCertHash, jstring aSigningCertHash, int* TypeArray,
                               jobject aPeer, jmethodID aHandleEventMethod)
{
    JELOG2(ESATSA);
    return mApduConnBase->CreateConn(aJni, aPeer, aHandleEventMethod, aUri,
                                     aName, aVendor, aVersion, aDomain, aRootCertHash, aSigningCertHash,
                                     TypeArray);
}

int JavaApduConnection::CreateSlot(JNIEnv& aJni, jobject aPeer)
{
    JELOG2(ESATSA);
    return mApduConnBase->CreateSlot(aJni, aPeer);
}

int JavaApduConnection::Open(JNIEnv& aJni, jclass aPeer, int aHandle)
{
    JELOG2(ESATSA);
    return mApduConnBase->Open(aJni, aPeer, aHandle);
}

int JavaApduConnection::DoPinMethod(JNIEnv& aJni, jclass aPeer, int aHandle,
                                    int aPinMethodType, int aPinID)
{
    JELOG2(ESATSA);
    return mApduConnBase->DoPinMethod(aJni, aPeer, aHandle, aPinMethodType,
                                      aPinID);
}

int JavaApduConnection::ExchangeAPDU(JNIEnv& aJni, jclass aPeer, int aHandle,
                                     jbyteArray aCommandAPDU)
{
    JELOG2(ESATSA);
    return mApduConnBase->ExchangeAPDU(aJni, aPeer, aHandle, aCommandAPDU);
}

int JavaApduConnection::UnblockPin(JNIEnv& aJni, jclass aPeer, int aHandle,
                                   int aBlockedPinID, int aUnblockingPinID)
{
    JELOG2(ESATSA);
    return mApduConnBase->UnblockPin(aJni, aPeer, aHandle, aBlockedPinID,
                                     aUnblockingPinID);
}

std::wstring JavaApduConnection::GetATR()
{
    JELOG2(ESATSA);
    return mApduConnBase->GetATR();
}

std::wstring JavaApduConnection::GetSlot()
{
    JELOG2(ESATSA);
    return mApduConnBase->GetSlot();
}

int JavaApduConnection::Close(JNIEnv& aJni, jclass aPeer, int aHandle)
{
    JELOG2(ESATSA);
    return mApduConnBase->Close(aJni, aPeer, aHandle);
}

} // namespace satsa
} // namespace java


