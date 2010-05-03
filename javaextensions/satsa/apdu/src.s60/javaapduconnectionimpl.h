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

#ifndef JAVAAPDUCONNECTIONIMPL_H
#define JAVAAPDUCONNECTIONIMPL_H

#include "javaapduconnectionbase.h"
#include "cstsconnection.h"
#include "cstsresphandler.h"
#include "cstsmidletinfo.h"
#include "functionserver.h"

namespace java
{
namespace satsa
{

/**
 *  JavaApduConnectionImpl implements  JavaApduConnectionBase.
 *  It offers methods for openning connection,
 *  exchanging apdu commands and manage pins and closing connection
 */
class JavaApduConnectionImpl: public JavaApduConnectionBase
{

public:
    JavaApduConnectionImpl();
    ~JavaApduConnectionImpl();

    // Method to perform various pin related functions(enable pin, disable pin etc)
    int DoPinMethod(JNIEnv& aJni, jclass aPeer, int aHandle,
                    int aPinMethodType, int aPinID);

    //Method to exchange apdu commands.
    int ExchangeAPDU(JNIEnv& aJni, jclass aPeer, int aHandle,
                     jbyteArray aCommandAPDU);

    // This function unblocks the blocing Pin.
    int UnblockPin(JNIEnv& aJni, jclass aPeer, int aHandle, int aBlockedPinID,
                   int aUnblockingPinID);

    // This function gets the answer to reset operation
    std::wstring GetATR();

    // This function creates the connection object
    int CreateConn(JNIEnv& aJni, jobject aPeer,
                   jmethodID aHandleEventMethod, jstring aUri, jstring aName,
                   jstring aVendor, jstring aVersion, jstring aDomain,
                   jstring aRootCertHash, jstring aSigningCertHash,
                   TInt* aReturnValueArray);

    // This function creates the connection object
    int CreateSlot(JNIEnv& aJni, jobject aPeer);

    // This function creates the connection object
    std::wstring GetSlot();

    // This function opens a connection to a smart card application
    int Open(JNIEnv& aJni, jclass aPeer, int aHandle);

    // This function closes all connection to a smart card
    int Close(JNIEnv& aJni, jclass aPeer, int aHandle);

private:
    /**
     * Does ChangePinL, DisablePinL, EnablePinL or EnterPinL operation
     * to the given connection object depending on the pin method type
     * parameter.
     *
     * @param aConnection Pointer to connection.
     * @param aPinMethodType Type of the pin method, which will be called
     * @param aPinID PinID number
     */
    void DoPinMethodL(CSTSConnection* aConnection, TInt aPinMethodType,
                      TInt aPinID);

    void Create(JNIEnv& aJni, jobject aPeer, jmethodID aHandleEventMethod,
                jstring aUri, jstring aName, jstring aVendor, jstring aVersion,
                jstring aDomain, jstring aRootCertHash, jstring aSigningCertHash,
                TInt* aReturnValueArray);

    void CreateSlotConn(JNIEnv& aJni, jobject aPeer);

private:

    CSTSConnection* iConnection;
    CSTSConnection* mSlotConn;

};

} // namespace satsa
} // namespace java
#endif // JAVAAPDUCONNECTIONIMPL_H

