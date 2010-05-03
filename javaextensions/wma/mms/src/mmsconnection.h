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


#ifndef MMSCONNECTION_H
#define MMSCONNECTION_H

#include "monitor.h"
#include "connectionlistener.h"
#include "mmsplatformservice.h"
#include "mmsserverconnection.h"
#include "serverconnectionfactory.h"

using namespace java::push;
using namespace java::util;
namespace java
{
namespace wma
{
/**
 * CJavaMmsConnection implements ConnectionListener.
 * It offers the functionality of sending the message and
 * to retrieve the received message.
 */
class MmsConnection: public ConnectionListener
{
public:
    /**
     * Private Contructor.
     * @param aServerConnection - boolean variable which indicates
     *                            server connection or client connection
     * @param aUri - string which contains the uri on which the connection is
     *               opened.
     */
    MmsConnection(bool aServerConnection, std::wstring aUri);
    /**
     * Destructor
     */
    ~MmsConnection();
    /**
     * checks for any missed messages while opening a connection.
     * waits for incoming message & notifies the java side.
     * @param[in] aJni Jni interface pointer
     * @param[in] apeer jobject
     */
    void open(JNIEnv& aJni, jobject aPeer);

    /**
     * Sending the Multimedia Message.
     * param aJni - Jni interface pointer
     * param aMsg - multimedia message as byte array
     */
    jint sendMMS(JNIEnv& aJni, jbyteArray aMsg, jint aOffset, jint aMsgLength,
                 jstring aAddress);

    /**
    * This method initializes the object
    * @param aJni - jni interface pointer
    * @param aPeer - java side peer object
    */
    void initialize(JNIEnv& aJni, jobject aPeer);

    /**
     * Retrieves the Multimedia Message.
     * @param aJni - Jni interface pointer
     */
    jbyteArray retrieveMMSMessage(JNIEnv& aJni);

    /**
     * from ConnectionListener.
     * This function is called when a Multimedia Message
     * arrives at the registered Application ID.
     */
    void msgArrived(const std::wstring&);

    /**
     * Method to close the connection.
     */
    void close(JNIEnv& aJni, jobject aPeer);

    /**
     * from ConnectionListener.
     * This function is called when the error has occured.
     * while receiving the message.
     */
    void error(const std::wstring& aUri, int aErrCode,
               const std::string& aErrText);

private:

    //Error variable to track errors while receiving
    int mError;
    //Uri string on which the connection is opened
    std::wstring mUri;
    //boolean variable to identify server/client connection
    bool mIsServerConnection;
    //boolean variable to control the notifier thread
    bool mReceiveOperation;
    MmsServerConnection* mServerConn;
    java::util::Monitor* mMessageMonitor;
    ServerConnectionFactory* mServerConnectionFactory;
    MMSPlatformService* mMmsPlatformService;
};
} //namespace wma
} //namespace java
#endif // MMSCONNECTION_H
