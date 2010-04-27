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
#include "serverconnectionbase.h"

namespace java
{
namespace wma
{
/**
 *  CJavaMessageConnectionBase abstract class which defines
 *  the interface method that are to be implemented by the os specific class.
 */
class SmsPlatformService
{

public:
    virtual ~SmsPlatformService()
    {

    }
    /**
     * Gets the Time stamp of the received message.
     * @return jlong The time value as jlong.
     */
    virtual jlong getTimeStamp() = 0;
    /**
     * Gets the Remote Host Address.
     * @param[in]  aJni Jni interface pointer
     * @return jstring The RemoteHostAddress as jstring.
     */
    virtual jstring getHostAddress(JNIEnv& aJni) =0;
    /**
     * gets the received message as jobject to java side.
     * @param[in]  aJni  Jni interface pointer.
     * @param[in] aMsgType Message type of the received message.
     * @return jobject The received message as jobject.
     */
    virtual jobject getMessageData(JNIEnv& aJni, const int aMessageType) =0;
    /**
     * Method to send the message.
     * @return int The success or Failure value.
     */
    virtual int send() =0;
    /**
     * Gets the Message type of the received Message
     * @param[in] aServerConn - server connection object.
     * @return int The type of the Message either Binary/Text Message.
     */
    virtual int receivedMessageType(ServerConnectionBase& aServerConn) =0;
    /**
     * function that calculates and returns numberofDataSegments of the Message.
     * @param[in]  aMesgType  MessageType
     * @param[in] aLength length of the message
     * @param[in] aHostPort port number
     * @param[in] ahostaddress Destination address
     * @param[in] adata Message data
     * @return int number of data segments.
     */
    virtual int getSmsDataSegments(const int aMesgType, const int aLength,
                                   const int aHostPort, const char* ahostAddress, const char*) =0;
    /**
     * Gets the new instance of OS Specific class which implements this interface
     * @param aUri - uri on which the connection to be opened.
     */
    static SmsPlatformService* getNewInstance(std::wstring aUri);

};
}
}

