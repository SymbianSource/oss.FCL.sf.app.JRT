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


#ifndef JAVAAPDUCONNECTION_H
#define JAVAAPDUCONNECTION_H

#include <jni.h>
#include <errno.h>
#include "com_nokia_mj_impl_satsa_APDUConnectionImpl.h"
#include "com_nokia_mj_impl_properties_satsa_SmartCardSlots.h"
#include "javaapduconnectionbase.h"  // Abstract interface class to bifurcate the implementation
namespace java
{
namespace satsa
{

/**
 *  JavaApduConnection offers methods for openning connection,
 *  exchanging apdu commands and manage pins and closing connection
 */

class JavaApduConnection
{

public:
    void Initialize();
    /**
     * Method to perform various pin related functions(enable pin, disable pin etc)
     *
     * @param[in]
     * @param[out]
     * @return int The success or Failure value.
     * @exception ?
     */

    int DoPinMethod(JNIEnv& aJni, jclass aPeer, int aHandle,
                    int aPinMethodType, int aPinID);
    /**
     * Method to exchange apdu commands.
     *
     *
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    int ExchangeAPDU(JNIEnv& aJni, jclass aPeer, int aHandle,
                     jbyteArray aCommandAPDU);

    /**
     * This function unblocks the blocing Pin.
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    int UnblockPin(JNIEnv& aJni, jclass aPeer, int aHandle, int aBlockedPinID,
                   int aUnblockingPinID);

    /**
     * This function gets the answer to reset operation
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    std::wstring GetATR();

    /**
     * This function creates the connection object
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    int Create(JNIEnv& aJni, jstring aUri, jstring aName, jstring aVendor,
               jstring aVersion, jstring aDomain, jstring aRootCertHash,
               jstring aSigningCertHash, int* TypeArray, jobject aPeer,
               jmethodID aHandleEventMethod);

    /**
     * This function creates the slot connection
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    int CreateSlot(JNIEnv& aJni, jobject aPeer);

    /**
     * This function fetches the slot information
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    std::wstring GetSlot();

    /**
     * This function opens a connection to a smart card application
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    int Open(JNIEnv& aJni, jclass aPeer, int aHandle);

    /**
     * This function closes all connections to a smart card
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    int Close(JNIEnv& aJni, jclass aPeer, int aHandle);

    // Destructor
    ~JavaApduConnection();

    static JavaApduConnection* getInstance();

private:
    // Constructor
    JavaApduConnection();

private:

    JavaApduConnectionBase* mApduConnBase; // the abstract interface

};

} // namespace satsa
} // namespace java

#endif // JAVAAPDUCONNECTION_H

