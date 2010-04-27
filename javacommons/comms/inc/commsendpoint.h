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
* Description:  This is an abstract base class for Comms endpoints.
*
*/

#ifndef COMMSENDPOINT_H
#define COMMSENDPOINT_H

#include <pthread.h>
#include <map>
#include <jni.h>

#include "javaosheaders.h"
#include "scopedlocks.h"

#include "transport.h"

namespace java
{
namespace comms
{

class CommsMessage;
class CommsListener;

class ListenerWrapper;

/**
* This is an abstract base class for Comms endpoints. It provides implementation for
* registering listeners and for synchronous message sending.
*
* Comms endpoint provides interprocess communication functionality. Comms communication
* is point-to-point meaning that client endpoint can be connected to a single server at a time.
* Communication is message based, where users wrap their payload to CommsMessage.
* Comms endpoints support synchronous and asynchronous message sending.
*
* Messages can be sent asynchronously or synchronously. Synchronous message sending blocks until
* a reply is received or when specified timeout occurs.
*
* CommsListener provides a way to receive messages asynchronously. Listeners are registered
* to endpoint. When message belonging to listener is received, will Comms endpoint make a
* callback for listener. Application can register as many listeners as need.
*
* Application can register also a default listener, which gets called if there is no registered
* listener for received message.
*
* CommsEndpoint is thread safe.
*
* Usage:
* It is recomended that at least default listener is registered before CommsServerEndpoint
* is started. This ensures that no messages are lost.
* @code
* CommsServerEndpoint comms;
* comms.registerDefaultListener(listener);
* comms.start(MY_SERVER_NAME);
* ...
* comms.stop();
* @endcode
*/

/**
* This constant can be used when timeout functionality of synchronous sendReceive is not needed.
* SendReceive method will return when reply is received or when connection is closed.
* @see CommsEndpoint::sendReceive()
*/
const int WAIT_FOR_EVER = -1;

class CommsEndpoint : public IpcListener
{
public:
    /**
    * A constructor.
    */
    OS_IMPORT CommsEndpoint();

    /**
    * A destructor.
    */
    OS_IMPORT virtual ~CommsEndpoint();

    /**
    * Registers message handler for given module id.
    * CommsListener provides a way to receive messages asynchronously.
    * Module id identifies to which component message belongs. Every message will contain module id
    * information that is used to route message to correct listener.
    *
    * There can be one registered listener for each module id and same listener can be registered
    * for multiple module ids.
    *
    * Listener registration remains valid during connect/disconnect and start/stop.
    * @code
    * CommsClientEndpoint comms;
    * comms.registerListener(MODULE_ID_A, listenerA);
    * comms.registerListener(MODULE_ID_B, listenerB);
    * comms.connect(MY_SERVER_NAME);
    * @endcode
    * @param[in] aModuleId Module id, messages with that module id are forwarded to given listener
    * @param[in] aListener Listener that will be called when message with matching module id is received
    * @see unregisterListener()
    * @see send()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int registerListener(int aModuleId, CommsListener* aListener);

    /**
    * Unregisters message handler for given module id.
    * @code
    * CommsClientEndpoint comms;
    * comms.registerListener(MODULE_ID_A, listenerA);
    * comms.unregisterListener(MODULE_ID_A, listenerA);
    * @endcode
    * @param[in] aModuleId Module id, identifies listener that will be removed
    * @param[in] aListener Listener that will be removed
    * @see unregisterListener()
    * @see send()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int unregisterListener(int aModuleId, CommsListener* aListener);

    /**
    * Registers default listener.
    * There can be only one default listener. Default listener is called if there is no registered listener for received message.
    * If there is no default listener then received messages without listener will be discarded.
    * @param[in] aDefaultListener Default message handler that will be called if no other suitable handlers are found
    * @see unregisterDefaultListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT   virtual int registerDefaultListener(CommsListener* aDefaultListener);

    /**
    * Unregisters default listener.
    * @param[in] aDefaultListener Listener that will be removed
    * @see registerDefaultListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int unregisterDefaultListener(CommsListener* aDefaultListener);

    /**
    * Sends a message.
    * Message recipient must set by CommsMessage::setReceiver() method. It is important that
    * message header is properly filled because header information is used in message routing.
    * @code
    * CommsMessage msg;
    *
    * // set the headers
    * msg.setMessageId(MY_MESSAGE_ID);
    * msg.setModuleId(MY_MODULE_ID);
    * msg.setReceiver(CLIENT_ADDRESS);
    *
    * // set the payload
    * msg << "my payload";
    *
    * // send message
    * int rc = comms.send(msg);
    * @endcode
    * @param[in] aMessage A message to be sent
    * @see sendReceive()
    * @see registerListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int send(CommsMessage& aMessage) = 0;

    /**
    * Sends and receives message syncronously.
    * Method blocks until response message is received or timeout expires.
    * Message recipient must set by CommsMessage::setReceiver() method. It is important that
    * message header is properly filled because header information is used in message routing.
    * @code
    * CommsMessage msg;
    *
    * // set the headers
    * msg.setMessageId(MY_MESSAGE_ID);
    * msg.setModuleId(MY_MODULE_ID);
    *
    * CommsMessage receivedMessage;
    * int timeout = 10; //10s
    *
    * // send and receive message
    * int rc = comms.sendReceive(msg, receivedMessage, timeout);
    * if(rc!=0)
    * {
    * LOG("sendReceive failed");
    * return;
    * }
    * int i;
    * receivedMessage >> i;
    * @endcode
    * @param[in] aMessage A message to be sent
    * @param[out] aReceivedMessage Received message (message content will be empy if method call fails)
    * @param[in] aTimeoutInSecs Timeout in seconds for how long reply is waited
    * @see send()
    * @return 0 in success
    * @return errno in failure and timeout cases
    */
    OS_IMPORT virtual int sendReceive(CommsMessage& aMessage, CommsMessage& aReceivedMessage, int aTimeoutInSecs);

protected:
    /**
    * Routes received message to waiting sendReceive thread or message listener
    * @param[in] aMessage Received message
    * @see -
    * @return -
    */
    OS_IMPORT virtual void handleIpcMessage(CommsMessage& aMessage);

    /**
    * Dispatches received message to listener or default listener
    * @param[in] aMessage Received message
    * @see -
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int handleMessage(CommsMessage& aMessage);

    /**
    * Generates unique message reference
    * @return message reference
    */
    OS_IMPORT virtual int generateMessageReference();

    // IpcListener methods
    /**
    * Callback that is called when underlaying transport stack receives a message
    * Called in the context of message loop thread.
    * @param[out] aMsg Received message
    * @return -
    */
    OS_IMPORT virtual void processMessage(const ipcMessage_t* aMsg);
    /**
    * Callback that is called when underlaying transport stack's message loop thread starts.
    * Called in the context of message loop thread.
    * @param -
    * @return -
    */
    OS_IMPORT virtual void onStart();
    /**
    * Callback that is called when underlaying transport stack's message loop thread exits.
    * Called in the context of message loop thread.
    * @param -
    * @return -
    */
    OS_IMPORT virtual void onExit();

private:
    ListenerWrapper* mDefaultListener;

    typedef std::map<const int, ListenerWrapper*> listeners_t;
    listeners_t mListeners;
    java::util::ScopedMutex mListenersMutex;

    int mMessageRefCounter;
    java::util::ScopedMutex mMessageRefMutex;

    // sendReceive related stuff
    int isSendReceiveMessage(CommsMessage& message);

    typedef struct sendReceiveMessage_s
    {
        CommsMessage*   p_message;
        int*            p_done;
        int*            p_result;
    } sendReceiveMessage_t;

    typedef std::map<int, sendReceiveMessage_t> sendReceiveListeners_t;
    sendReceiveListeners_t mSendReceiveListeners; // access protected by mSendReceiveMutex

    pthread_mutex_t mSendReceiveMutex;
    pthread_cond_t  mSendReceiveCondition;

public:
    /**
    * Registers Java message handler for given module id.
    * @param[in] aModuleId Module id, messages with that module id are forwarded to given listener
    * @param[in] aListener Listener that will be called when message with matching module id is received
    * @param[in] aEnv JNI context
    * @see unregisterJavaListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int registerJavaListener(int aModuleId, jobject aListener, JNIEnv* aEnv);

    /**
    * Unregisters Java message handler for given module id.
    * @param[in] aModuleId Module id
    * @param[in] aListener Listener to be removed
    * @param[in] aEnv JNI context
    * @see registerJavaListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int unregisterJavaListener(int aModuleId, jobject aListener, JNIEnv* aEnv);

    /**
    * Registers default listener.
    * There can be only one default listener. Default listener is called if there is no registered listener for received message.
    * If there is no default listener then received messages without listener will be discarded.
    * @param[in] aDefaultListener Default message handler
    * @see unregisterDefaultJavaListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int registerDefaultJavaListener(jobject aListener, JNIEnv* aEnv);

    /**
    * Unregisters default listener.
    * @param[in] aDefaultListener Listener that will be removed
    * @see registerDefaultJavaListener()
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int unregisterDefaultJavaListener(jobject aListener, JNIEnv* aEnv);

    /**
    * Attach internal message loop thread to JVM.
    * @param[in] aEnv JNI context
    * @see detachFromVm
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int attachToVm(JNIEnv* aEnv);

    /**
    * Detach internal message loop thread from JVM.
    * @param -
    * @see attachToVm
    * @return 0 in success, errno in failure
    */
    OS_IMPORT virtual int detachFromVm();

protected:
    JavaVM* mVm;
    JNIEnv* mJNIEnv;

};

} // namespace comms
} // namespace java


#endif // COMMSENDPOINT_H
