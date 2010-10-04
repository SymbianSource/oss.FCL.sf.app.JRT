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

#ifndef S60MMSSERVERCONNECTION_H
#define S60MMSSERVERCONNECTION_H

#include <mmsclient.h>
#include <mmsapplicationadapter.h>

#include "monitor.h"
#include "mmsserverconnection.h"

using namespace java::push;

namespace java
{
namespace wma
{
/**
 *  MmsServerConnection implement  ServerConnectionBase.
 *  It is responsible to start the midlet when the message arrive in the device
 *  for a registered midlet  when the midlet is not running.It is also
 *  responsible to receive message when the midlet is running
 *  @lib mmsserverconnection.dll
 *  @
 */
class S60MmsServerConnection: public CActive,
        public MmsServerConnection,
        public MMsvSessionObserver,
        public MMsvEntryObserver
{

public:
    //Constructor
    OS_IMPORT S60MmsServerConnection(const std::wstring& aUri,
                                     const std::wstring& aFilter);
    OS_IMPORT virtual ~S60MmsServerConnection();
    /**
     * from ServerConnection
     * Opens the connection. It registers the push URI to the mms engine for
     * any incoming mms  message on this AppId.
     * Throws COMMON_SRV_CONN_PLUGIN_ERROR exception if there is an error while
     * registering the connection.
     * @param aListener: Listener to be notified when message arrives.
     *
     * @return
     */
    OS_IMPORT virtual void open(ConnectionListener* aListener);
    /**
     *from ServerConnection
     * Closes the connection
     * @return
     */
    OS_IMPORT virtual void close();
    /**
     * from ServerConnection
     * Return the push register URI
     * @return std::wstring
     */
    OS_IMPORT virtual std::wstring getUri() const;
    /**
     * from ServerConnection
     * sets the filter string
     * @ aFilter - filter string
     */
    OS_IMPORT virtual void setFilter(const std::wstring& aFilter);
    /**
     * from ServerConnection
     * Return the filter string
     * @return std::wstring the filter string
     */
    OS_IMPORT virtual std::wstring getFilter() const;
    /**
     * SetOpen()
     * It is used to Distinguish whether Open is called from Midlet context
     * or Push Controller .This function is called first when the server
     * connection is called from MIDlet context.
     * @return
     */
    OS_IMPORT void setOpen();
    void setUnregister();
    void clearUnregister();
private:
    //Constructor
    S60MmsServerConnection();
    void InitializeL();
    void MmsRequestCompleted();

    /**
     * Check that if the received MMS is valid message
     * If the message is valid Push launch the midlet.
     * @param aMsvId: Id of the received message
     *
     * @return .
     */
    void CheckForValidMessageAndPushL(TMsvId aMsgId);
    /**
     * Check that if the received MMS is ready to read
     *
     * @param aMsvId: Id of the received message
     *
     * @return positive TInt if the message is ready to read otherwise zero.
     */
    TBool IsMmsReadyToReadL(TMsvId aMsvId);
    /**
     *
     * thread function.
     * @
     * @param :
     *
     * @return
     */
    static int listenThread(S60MmsServerConnection* aServerConnection);
    /**
     * HandleSessionEventL from MMsvSessionObserver
     * We are not handling any thing in this method.
     * @
     * @param aEvent:
     * @param aArg1:
     * @param aArg2:
     * @param aArg3:
     *
     * @return
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent,TAny* aArg1,
                             TAny* aArg2,
                             TAny* aArg3);
    /**
     * HandleEntryEventL
     * From MMsvEntryObserver
     * @
     * @param aEvent: TMsv entry event
     * @param aArg1:
     * @param aArg2:
     * @param aArg3:
     *
     * @return
     */
    void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1,
                           TAny* aArg2,
                           TAny* aArg3);
private: // from CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

protected:
    enum TOperation
    {
        EReceivingMessageForNotify,
        EReceivingMessageForListen,
        ENotifyingReadMessageSucceeded,
        ENotifyingClose,
    };

    int mError;
    //thread id
    RThread mThread;
    // Uri of the push connection
    std::wstring mUri;
    // the ongoing operation which is executed by the server connection object.
    TOperation mState;
    // Apllication id
    std::wstring mAppId;
    //defines the acceptable remote host.
    std::wstring mFilter;
    CMsvSession* mSession;
    // Application ID
    HBufC* mApplicationId;
    // flag to indicate connection is active or not, useful during close.
    bool mActiveConnection;
    // To synchronize threads
    pthread_mutex_t  mMutex;
    pthread_cond_t   mCondVar;
    CMsvEntry* mClientEntry;
    // flag to indicate listening by push or application
    bool mIsAppLaunched;
    //listener to be notified for incoming messages
    ConnectionListener* mListener;
    bool mUnregister;
    // handle to monitor object
    java::util::Monitor* mOpenMonitor;
    // reference to application adapter
    CMmsApplicationAdapter* mMmsApplicationAdapter;
};

}//end of namespace wma
}//end of namespace java

#endif // S60MMSSERVERCONNECTION_H
