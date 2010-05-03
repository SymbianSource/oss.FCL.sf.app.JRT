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
* Description:  ?Description
 *
*/

#ifndef MMSPLATFORMSERVICE_H
#define MMSPLATFORMSERVICE_H

#include <jni.h>
#include "connectionlistener.h"
/**
 * CMMSPlatformService is an abstract method that declares
 * the interface methods that are defined by the
 * OS Specific Class.
 */
namespace java
{
namespace wma
{

class MMSPlatformService
{

public:
    /**
     * Virtual Destructor
     */
    virtual ~MMSPlatformService()
    {
    }

    /**
     * Converts the Message to the Native Format and sends it.
     * @param aJni - jni interface pointer
     * @param aJavaBuffer - the serialized message buffer from java side.
     * @param aOffset - offset
     * @param aLength - length of the message.
     * @param aDestination - destination address
     */
    virtual int send(JNIEnv& aJni, jbyteArray aJavaBuffer, jint aOffset,
                     jint aLength, jstring aDestination) = 0;

    /**
     * Retrieves the received message.
     * @param aJni - jni interface pointer.
     */
    virtual jbyteArray retrieveMessage(JNIEnv& aJni) = 0;

    /**
     * Closes the connections.
     * @param aJni - jni interface pointer
     * @param aPeer - java side object reference.
     */
    virtual void closeConnection(JNIEnv& aJni, jobject aPeer)=0;
    /**
     * Gets number of messages
     */
    virtual int getNumberOfMessages() = 0;
    /**
     * Gets the instance of OS Specific class which implements this interface
     * @param aJni - jni interface pointer
     * @param aPeer - java side peer reference.
     * @param aServerConnection - boolean value to identify client/server
     *                            connection.
     * @param aUri - uri on which the connection to be opened.
     * @param aListener - listener to be notified about incoming messages.
     */
    static MMSPlatformService* getInstance(JNIEnv& aJni, jobject aPeer,
                                           const bool aServerConnection,
                                           const std::wstring& aUri,
                                           java::push::ConnectionListener* aListener);
};
} //namespace wma
} //namespace java
#endif // MMSPLATFORMSERVICE_H
