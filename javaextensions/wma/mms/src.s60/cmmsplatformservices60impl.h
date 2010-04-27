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


#ifndef CMMSPLATFORMSERVICES60IMPL_H
#define CMMSPLATFORMSERVICES60IMPL_H

// INCLUDE FILES
#include <msvids.h>
#include <mtclreg.h>
#include <mmsclient.h>
#include <mmsapplicationadapter.h>

#include "monitor.h"
#include "functionserver.h"
#include "cmmsmessageinformation.h"
#include "cjavammsmessagehandler.h"
#include "mmsplatformservice.h"

using namespace java::push;
using namespace java::util;
_LIT8(KSlash, "/");

const TInt KArrayGranularity = 2;
const TInt KSlashLength = 1;

// CLASS DECLARATION
/**
 *   to create and send the MMS messages
 *
 *
 *
 */
namespace java
{
namespace wma
{

class CMMSPlatformServiceS60Impl: public CActive,
        public MMSPlatformService,
        public FunctionServer,
        public MMsvSessionObserver,
        public MMsvEntryObserver
{
public:

    /**
     * Destructor.
     */
    virtual ~CMMSPlatformServiceS60Impl();

    static CMMSPlatformServiceS60Impl* New(JNIEnv& aJni, jobject aPeer,
                                           const bool aServerConnection,
                                           const std::wstring& aUri,
                                           ConnectionListener* aListener);
    /**
     * Sends a multimedia message to its recipients.
     * @param aJni JNI environment
     * @param jbyteArray message byte array
     * @param aOffset
     * @param aLength message length
     * @param aDestination destination address
     * @return int status of the send functionality.
     */
    int send(JNIEnv& aJni, jbyteArray aJavaBuffer, int aOffset, int aLength,
             jstring aDestination);

    /**
     * Retrieves the first message from the queue.
     * @param aJni JNI environment
     * @return A byte array containing the first multimedia message received
     */
    jbyteArray retrieveMessage(JNIEnv& aJni);
    /**
     * closes the connection
     */
    void closeConnection(JNIEnv& aJni, jobject aPeer);

    int getNumberOfMessages();

    int getError();

private:
    /**
     * C++ default constructor.
     * @param aServerConnection for handling server connection
     * @param aServerApplicationId to register for incoming messages
     * @return
     */
    CMMSPlatformServiceS60Impl(const bool aServerConnection,
                               HBufC* aServerApplicationId, ConnectionListener* aListener);
    // From FunctionServer
    virtual void vmAttached();

    //From FunctionServer
    virtual void doServerSideInit();
    /**
     * Notifies java side send about the status of sending mms message
     * @param aJni - Jni interface pointer
     * @param aPeer - Jni object reference
     * @param aError - status of send functionality
     */
    void NotifySend(JNIEnv& aJni, jobject aPeer, int aError);
    /**
     * Send the Message
     * @param aMidMessageInformation contain the message information and
     *        attachments
     * @return
     */

    void SendMultiPartMessageL();

    /**
     * From MMsvSessionObserver
     * @param aEvent: Event type
     * @param aArg1: contain different values for different aEvent
     * @param aArg2: contain different values for different aEvent
     * @param aArg3: contain different values for different aEvent
     * @return
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);

    /**
     * Clears the message send array. If there is some message that is not
     *  send yet.Delete it and do not send the messages.
     *
     * @return
     */
    void ClearSendMessageArray();
    /**
     * ReceivedMessage
     * get the received message
     *
     * @param aMessageId: Received message Id
     *
     * @return CMidMessageInformation: received message
     */
    CMMSMessageInformation* ReceivedMessage(TMsvId aMessageId);

    /**
     * NotifyReceivedMessageL
     * Get the URL check for the register folder. If already register
     * then check for the messages in the register folder. If there are messages
     * in the folder then send notification to the connection for the messages.
     *
     *
     * @return
     */

    void NotifyReceivedMessageL();

    /**
     * HandleEntryEventL
     * From MMsvEntryObserver
     *
     * @param aEvent: TMsv entry event
     * @param aArg1:
     * @param aArg2:
     * @param aArg3:
     *
     * @return
     */
    void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* aArg2,
                           TAny* aArg3);

    /**
     * ScanRegisterFolderAndNotifyL
     * Scan the register folder and Notify the listener.
     *
     *
     * @return
     */
    void ScanRegisterFolderAndNotifyL();

    /**
     * create MMS messsage in draft folder.
     *
     *
     * @return TMsvId: Id of the newly created message.
     */
    TMsvId CreateMessageL();

    /**
     * Load the message by TMsvId
     *
     * @param aMessageEntry:
     *
     * @return
     */
    void LoadMessageL(TMsvId aMessageEntry);

    /**
     * Set the subject of newly created MMS message.
     *
     * @param aSubject:
     *
     * @return
     */
    void SetSubjectL(const TDesC& aSubject);

    /**
     * add receipient addresses by type to the newly created message.
     *
     * @param aAddress:
     * @param aAddressType:
     *
     * @return
     */
    void AddRecipientL(const TDesC& aAddress, TMmsRecipients aAddressType);

    /**
     * add attachment by name to the newly created MMS message.
     *
     * @param aFileName:
     * @param aMimeType:
     *
     * @return
     */
    void AddAttchmentL(TDesC& aFileName, TDesC8& aMimeType);

    /**
     * Set application-id in the newly created message.
     *
     * @param aApplicationId:
     *
     * @return
     */
    void SetApplicationIdL(const TDesC& aApplicationId);

    /**
     * Set reply-to -application-id in the newly created message.
     *
     * @param aReplyToApplicationId:
     *
     * @return
     */
    void SetReplyToApplicationIdL(const TDesC& aReplyToApplicationId);

    /**
     * Set the date of the newly created message.
     *
     * @param aLong:
     *
     * @return
     */
    void SetMessageSendDate(TInt64& aLong);

    /**
     * Set the setting of the newly created message.
     *
     *
     * @return
     */
    void SetMessageSettingL();

    /**
     * create and send the MMS messge from CMidMessageInformation
     *
     * @param aMidMessageInformation:
     *
     * @return
     */
    // send MMS message from send array
    void SendMmsMessageL(CMMSMessageInformation* aMessageInformation);

    /**
     * Read the message Setting of received message.
     *
     *
     * @return
     */
    void ReadSettingL();

    /**
     * Delete the message by TMsvId
     *
     * @param aMsvId:
     *
     * @return
     */
    void DeleteMmsMessageL(TMsvId aMsvId);

    /**
     * Read the newly received message. Client connection should not call
     *  this method.It will panic if invoked on client connection
     *
     * @param aMsvId:
     *
     * @return
     */
    void ReceiveMessageL(TMsvId aMsvId);

    /**
     * Send the message from by TMsvId
     *
     * @param aSendMmsMsgId:
     *
     * @return
     */
    void SendMmsMessageL(TMsvId aSendMmsMsgId);

    /**
     * close the connection
     *
     *
     * @return
     */
    void Close();
    /**
     * add attachment to newly created entry in the message store.
     *
     * @param aMidMessageInformation:
     *
     * @return
     */
    void AddAttachmentsL(CMMSMessageInformation* aMessageInformation);

    /**
     * During construction of this method create the send message array.
     *
     *
     * @return
     */
    void CreateSendMessageArrayL();

    /**
     * Clear and Destroy the message send array. If there is some message
     * that is not send yet. Delete it and do not send the messages.
     *
     *
     * @return
     */
    void ClearAndDestroySendMessageArray();

    /**
     * Delete the message of given Id.
     *
     * @param aMsvId: Id of the messagethat should be deleted.
     *
     * @return
     */
    void DeleteSentMessageL(TMsvId aMsvId);

    /**
     * Received the message one by one.
     *
     * @param aMsvId: Id of the message that is received in the device.
     *
     * @return
     */
    void DoReceiveMessageL(TMsvId aMsvId);

    /**
     * ReadAttachmentsL
     * Read the attachmnets and add it to aMidMessageInformation
     *
     * @param aMidMessageInformation:
     *
     * @return
     */
    void ReadAttachmentsL(CMMSMessageInformation& aMessageInformation);

    /**
     * Read data from the attached files
     *
     * @param aRFile: Handle of the file to read the data from
     * @param aDataArray: Copy the data to the dataArray
     *
     * @return
     */
    void ReadDataFromAttachFileL(RFile& aRFile, TDesC8*& aDataArray);
    /**
     * Sends notifications towards java about the number of messages arrived.
     *
     * @param aDescription
     * @param messages
     */
    void NotifyJava(const TDesC& aDescription, int aMessages);

protected:

    /**
     * default Symbian 2nd phase constructor
     * @return
     */
    void ConstructL();
    void mmsRequestComplete(int result);
    /**
     * from CActive
     *
     *
     * @return
     */
    void RunL();

    /**
     * from CActive
     *
     *
     * @return
     */
    void DoCancel();

    /**
     * from CActive
     *
     * @param aError: error value
     *
     * @return TInt
     */
    TInt RunError(TInt aError);

private:
    // flag used to indicate that 'send' operation is ongoing
    bool mIsSending;

    //flag used to indicate Client/Server connection
    bool mServerConnection;

    int mError;
    //number of new messages
    int mNewMessages;

    // handle to the file system
    RFs mRFs;

    RArray<TMsvId> mSendTMsvIdArray;

    // array containing the IDs of the received MMS messages
    RArray<TMsvId> mReceivedMMSIDs;

    //message sent callback method id
    jmethodID mSendCallBackMethodId;

    // The delivery date
    TTime mMessageDeliveryDate;

    TMmsMessagePriority mMmsMessagePriority;

    //Own the mServerApplicationId
    HBufC* mServerApplicationId;

    // buffer to be used when sending
    CBufFlat* mSendBuf;

    //Own the mSession
    CMsvSession* mSession;

    //Own the mMmsClient
    CMmsClientMtm* mMmsClient;

    //Own the mServerMmsMtm
    CMmsClientMtm* mServerMmsMtm;

    CMsvEntry* mRegisterCMsvEntry;

    //listener to be notified for an incoming message
    ConnectionListener* mMessageListener;

    //Own the ClientMtmRegistry
    CClientMtmRegistry* mClientMtmRegistry;

    CJavaMMSMessageHandler* mMessageHandler;

    //Own the mMmsApplicationAdapter
    CMmsApplicationAdapter* mMmsApplicationAdapter;

    //Own the mSendMsgArray
    CArrayPtrSeg<CMMSMessageInformation>* mSendMsgArray;

    // Own the mReceivedMsgArray
    CArrayPtrSeg<CMMSMessageInformation>* mReceivedMsgArray;

};

inline int CMMSPlatformServiceS60Impl::getError()
{
    return mError;
}
///////////////////////////////////////////////////////////
} //namespace wma
} //namespace java
#endif // CMMSPLATFORMSERVICES60IMPL_H
// End of File
