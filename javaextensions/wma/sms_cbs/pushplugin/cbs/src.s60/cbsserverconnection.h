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

#ifndef CBSSERVERCONNECTION_H
#define CBSSERVERCONNECTION_H

#include <cdbover.h>

#include "cjavacbsdecoder.h"
#include "serverconnectionbase.h"

namespace java
{
namespace wma
{
const wchar_t CBS_STORE_PATH[] = L"cbs\\";
/**
 *  CbsServerConnection implement  ServerConnectionBase.
 *  It is responsible to start the midlet when the cbs message arrives in the
 *  device for a registered midlet and when the midlet is not running.It is
 *  also responsible to receive cbs message when the midlet is running
 *  @lib smsserverconnection.dll
 *  @
 */
class CbsServerConnection:  public CActive,public ServerConnectionBase
{

public:
    //Constructor
    OS_IMPORT CbsServerConnection(const std::wstring& aUri,
                                  const std::wstring& aFilter);
    OS_IMPORT virtual ~CbsServerConnection();
    /**
     * from ServerConnection
     * Opens the connection. It registers the push URI to the messaging system
     * for receiving messages.
     * Throws COMMON_SRV_CONN_PLUGIN_ERROR exception if there is an error while
     * registering the connection.
     * @param aListener: Listener to be notified when message arrives.
     *
     * @return
     */
    OS_IMPORT virtual void open(java::push::ConnectionListener* aListener);

    /**
     * from ServerConnectionBase
     * Opens the connection. It registers the server URI to the messaging system
     *  for receiving messages.
     * Throws COMMON_SRV_CONN_PLUGIN_ERROR exception if there is an error while
     * registering the connection.
     * @param[in] aListener: Listener to be notified when message arrives.
     * @param[in] aAppLaunched : bool to notify listening by midlet or push.
     */
    OS_IMPORT virtual void open(java::push::ConnectionListener* aListener,
                                bool aAppLaunched);
    /**
     *from ServerConnection
     * Closes the connection
     * @
     *
     * @return
     */
    OS_IMPORT virtual void close();
    /**
     * RetrieveMessage from ServerConnectionBase
     * get the received message
     * @
     * @param aCbsBuf:the message buffer into which the received message is
     * written into.
     * @return
     */
    OS_IMPORT virtual int retrieveMessage(TJavaMessageParametersBuf& aCbsBuf);
    /**
     * Method which initialize the server object.
     */
    void initializeL();
private:
    //Constructor
    CbsServerConnection();

    /**
     * ReadMessageFromStackL
     * Reads the arrived cbs message and stores it.
     * @
     * @param :
     *
     * @return
     */
    void readMessageFromStackL();
    /**
     *
     * thread function.which listens for incoming Cbs messages and informs the
     * listener about the incoming message.
     * @param :aParams - the server object
     *
     * @return
     */
    static void* listenThread(void* aParams);
    /*
     * sets the filter settings for listening CBS messages
     */
    void setFilterSettings();
    /*
     * starts listening for cbs messages
     */
    void startCBSListening();
    void saveCbsMessageL(TSmsDataCodingScheme::TSmsAlphabet
                         aEncoding,TCBSDataBuf& aMessageData);
    TCBSDataBuf decodeCbsMessageL(CJavaCbsDecoder* aDecoder, const TDesC8& aData);
private: // from CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

protected:
    enum TState
    {
        ECbsOpen,
        ECbsReceivingActive,
        ECbsReceivingDeactive,
    };
    // Handle to file system
    RFs mFs;
    // thread running state
    TBool mRunning;
    // Initialization status
    TBool mInitialized;
    // Multipage messsage receive status
    TBool mCbsMessageReceived;
    // It is a part of multipage message
    TBool mCbsPartiallyReceived;
    // Current multipage message ID
    TUint mCbsMultipageMessageId;
    // Current multipage messageCode
    TUint mCbsMultipageMessageCode;
    //Number of pages Received
    TUint mCbsReceivedPages;
    // Cbs messageId
    TUint mCbsMessageId;
    //Total number of pages in Cbs Message
    TUint mCbsTotalNumPages;

    // Current Cbs Message page number
    TInt mCbsPageNumber;
    // Counter for WCDMA multlipage message pages
    TInt mPageCounter;

    // the ongoing operation which is executed by the server connection object.
    TState mState;
    HBufC* mTsyName;
    //A handle to a thread
    pthread_t mThreadId;
    //Input stream interface
    std::ifstream readStream;
    // monitor object
    java::util::Monitor* mOpenMonitor;
    //Root telephony server session
    RTelServer mTelServer;
    //Provides client access to mobile phone functionality provided by TSY.
    RMobilePhone mMobilePhone;
    //listener to be notified for incoming messages
    java::push::ConnectionListener* mListener;
    // To synchronize threads
    pthread_mutex_t  mMutex;
    // To synchronize threads
    pthread_cond_t   mCondVar;

    // Telephony data
    //Handel to broadcast message services
    RMobileBroadcastMessaging mBroadcastMessage;
    RMobileBroadcastMessaging::TBroadcastPageData mBroadcastPageData;
    RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 mBroadcastMsgAttributes;
    RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg
    mBroadcastMsgAttributePackage;
    RMobileBroadcastMessaging::TMobileBroadcastCapsV1 mBroadcastPhoneCaps;
    RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg mBroadcastPhoneCapsPckg;

    // Message receipt data members
    CDesCArrayFlat* mCbsMessagePagesData;
    RArray<TInt> mCbsMessagePagesRef;

};

}// end of namespace wma
} // end of namespace java

#endif // CBSSERVERCONNECTION_H
