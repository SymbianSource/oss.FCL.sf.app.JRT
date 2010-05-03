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

#ifndef CSMSPLATFORMSERVICES60IMPL_H
#define CSMSPLATFORMSERVICES60IMPL_H

#include <gsmubuf.h>
#include <gsmuelem.h>
#include <smsuaddr.h>


#include "smsplatformservice.h"

class CSmsMessage;

namespace java
{
namespace wma
{
/**
 *  CJavaMessageConnection implements  CJavaMessageConnectionBase.
 *  It offers methods for sending , to get the
 *  number of data segments required to send the message and to
 *  receive the message.
 */
class CSmsPlatformServiceS60Impl: public CBase,public SmsPlatformService
{

public:
    //Constructor
    CSmsPlatformServiceS60Impl(std::wstring aUri);
    //Destructor
    ~CSmsPlatformServiceS60Impl();
    /**
     * Gets the Time stamp of the received message.
     * @return jlong The time value as jlong.
     */
    jlong getTimeStamp();
    /**
     * Gets the Remote Host Address.
     * @param[in] aJni Jni interface pointer.
     * @return jstring The RemoteHostAddress as jstring.
     */

    jstring getHostAddress(JNIEnv& aJni);
    /**
     * gets the received message as jobject to java side.
     * @param[in]  aJni  Jni interface pointer.
     * @param[in] aMsgType Message type of the received message.
     * @return jobject The received message as jobject.
     */
    jobject getMessageData(JNIEnv& aJni, const int aMessageType);
    /**
     * Method to send the message.
     * @return int The success or Failure value.
     */

    int send();
    /**
     * Gets the Message type of the received Message.
     * @param[in] aServerConn - reference to server connection
     * @return int The type of the Message either Binary/Text Message.
     */

    int receivedMessageType(ServerConnectionBase& aServerConn);
    /**
     * Initializing function
     */
    void initialize();
    /**
     * function that calculates and returns numberofDataSegments required to send
     * the Message provided.
     * @param[in] aMesgtype  MessageType
     * @param[in] aLength length of the message
     * @param[in] aport port number
     * @param[in] ahostaddress Destination address
     * @param[in] adata Message data
     * @return int number of data segments.
     */
    int getSmsDataSegments(const int aMesgType, const int aLength,
                           const int aHostPort,const char* ahostAddress, const char*);

private:

    /**
     * Converts the CString to Descriptor
     * @param[in] aData Message Data
     * @param[in] aMsgType Message type
     * @param[in] aLength Message length
     */
    HBufC* convertToDes(const char* aData, const int aMsgType,
                        const int aLength);
    /**
     * Gets the best Message encoding for the message provided.
     * @param[in] asmsData  Message data for which the best encoding to be found
     * @param[in] amsgType Message type of the message provided
     * @param[in] aFs handle to File server
     * @return TSmsAlphabet Best Message encoding type.
     */
    TSmsDataCodingScheme::TSmsAlphabet getEncodingSchemeL(const TDesC& asmsData,
            const TInt amsgType,
            RFs& aFs);
private:
    //sending port
    int mSendPort;
    //Uri of the opened connection.
    std::wstring mUri;
    //handle to RSocket
    RSocket mSmsSendSocket;
    // handle to socket server
    RSocketServ mSocketServer;
    //SMS address for a socket.
    TSockAddr mSmsSendAddress;
    //Represents a complete SMS message.
    CSmsMessage* mSendMessage;
    // buffer to be used when sending
    TPckgBuf<TUint> mSmsSendDummyBuf;
    //Buffer to hold the sms message parameters
    TJavaMessageParametersBuf* mMessageParameters;

};
} //namespace wma
} //namespace java
#endif // CSMSPLATFORMSERVICES60IMPL_H
