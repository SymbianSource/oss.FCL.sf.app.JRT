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


#include<jni.h>
#include <string>

namespace java
{
namespace satsa
{

/**
 *  JavaApduConnectionBase abstract class which defines
 *  the interface method that are to be implemented by the os specific class
 *
 */
class JavaApduConnectionBase
{

public:
    virtual ~JavaApduConnectionBase()
    {

    }

    /**
     * Method to perform various pin related functions(enable pin, disable pin etc)
     *
     * @param[in]
     * @param[out]
     * @return int The success or Failure value.
     * @exception ?
     */

    virtual int DoPinMethod(JNIEnv& aJni, jclass aPeer, int aHandle,
                            int aPinMethodType, int aPinID)=0;
    /**
     * Method to exchange apdu commands.
     *
     *
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    virtual int ExchangeAPDU(JNIEnv& aJni, jclass aPeer, int aHandle,
                             jbyteArray aCommandAPDU)=0;

    /**
     * This function unblocks the blocing Pin.
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    virtual int UnblockPin(JNIEnv& aJni, jclass aPeer, int aHandle,
                           int aBlockedPinID, int aUnblockingPinID)=0;

    /**
     * This function gets the answer to reset operation
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    virtual std::wstring GetATR()=0;

    /**
     * This function creates the connection object
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    virtual int CreateConn(
        JNIEnv& aJni,
        jobject aPeer,
        jmethodID aHandleEventMethod,
        jstring aUri, jstring aName, jstring aVendor, jstring aVersion,
        jstring aDomain, jstring aRootCertHash, jstring aSigningCertHash,
        int* TypeArray)=0;

    /**
     * This function creates a slot object
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    virtual int CreateSlot(JNIEnv& aJni, jobject aPeer)=0;

    /**
     * This function returns Slot Information
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    virtual std::wstring GetSlot()=0;

    /**
     * This function opens a connection to a smart card application
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    virtual int Open(JNIEnv& aJni, jclass aPeer, int aHandle)=0;

    /**
     * This function closes all connection to a smart card
     * @param[in]
     * @param[out]
     * @return
     * @exception ?
     */
    virtual int Close(JNIEnv& aJni, jclass aPeer, int aHandle)=0;

    static JavaApduConnectionBase* getInstance();

};

} // namespace satsa
} // namespace java


