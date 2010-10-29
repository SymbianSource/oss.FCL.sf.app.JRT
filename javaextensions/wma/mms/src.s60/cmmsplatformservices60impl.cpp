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


// INCLUDE FILES
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>
#include <mmsvattachmentmanager.h>

#include "logger.h"
#include "fs_methodcall.h"
#include "s60commonutils.h"
#include "cmmsmessageattachment.h"
#include "cmmsplatformservices60impl.h"
#include "mmsserverconnection.h"
#include "mmsserverconnectionfactory.h"
//constants
const TInt KMaxAppIDLength = 34;

using namespace java::push;
using namespace java::util;

namespace java
{
namespace wma
{
/* ================= MEMBER FUNCTIONS =======================
 * constructor
 *------------------------------------------------------------------------------
 * private CMMSPlatformServiceS60Impl
 *
 *------------------------------------------------------------------------------
 */
CMMSPlatformServiceS60Impl::CMMSPlatformServiceS60Impl(
    const bool aServerConnection,
    HBufC* aServerApplicationId,
    ConnectionListener* aListener)
        :CActive(EPriorityNormal), java::util::FunctionServer("CMMSPlatformService"),
        mIsSending(false), mServerConnection(aServerConnection), mNewMessages(0),
        mServerApplicationId(aServerApplicationId)
{
    JELOG2(EWMA);
    //Creates a new thread and starts active scheduler
    //This results in call to doServerSideInit()
    createServerToNewThread();
    mSendMsgArray = NULL;
    mMessageListener = aListener;
}
/**
 * Gets the instance of OS Specific class which implements the interface
 * CMMSPlatformService.
 * @param aJni - jni interface pointer
 * @param aPeer - java side peer reference.
 * @param aServerConnection - boolean value to identify client/server
 *                            connection.
 * @param aUri - uri on which the connection to be opened.
 * @param aListener - listener to be notified about incoming messages.
 */
MMSPlatformService* MMSPlatformService::getInstance(JNIEnv& aJni,
        jobject aPeer,const bool aServerConnection,const std::wstring& aUri,
        ConnectionListener* aListener)
{
    return CMMSPlatformServiceS60Impl::New(aJni,aPeer,aServerConnection,aUri,
                                           aListener);
}


CMMSPlatformServiceS60Impl* CMMSPlatformServiceS60Impl::New(JNIEnv& aJni,
        jobject aPeer,const bool aServerConnection,const std::wstring& aUri,
        ConnectionListener* aListener)
{
    JELOG2(EWMA);
    HBufC* serverApplicationId = NULL;
    int error = KErrNone;
    if (aServerConnection)
    {
        serverApplicationId = S60CommonUtils::wstringToDes(
                                  (aUri.substr(7)).c_str());
    }
    CMMSPlatformServiceS60Impl * self = new CMMSPlatformServiceS60Impl(
        aServerConnection, serverApplicationId, aListener);
    error = self->getError();
    if (KErrNone == error)
    {
        //Attaches the function server thread to VM, to make callbacks to java
        //This results in call to vmAttached()
        self->attachToVm(aJni, aPeer);
        LOG(EWMA, EInfo, "CMMSPlatformServiceS60Impl Successfully attached to VM");
        TRAP(error, self->ConstructL());
    }
    if (error != KErrNone)
    {
        ELOG1(EWMA, "CMMSPlatformServiceS60Impl : Error while opening %d", error);
        // If there is any error while opening close the connection
        self->closeConnection(aJni,aPeer);
        delete self;
        throw error;
    }
    return self;
}

/**------------------------------------------------------------------------------
 * EPOC default constructor can leave.
 * private ConstructL
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::ConstructL()
{
    JELOG2(EWMA);
    User::LeaveIfError(mRFs.Connect());
    User::LeaveIfError(mRFs.ShareProtected());
    CreateSendMessageArrayL(); // create send array
    mMessageDeliveryDate = TTime(0);
    mMmsMessagePriority = EMmsPriorityHigh;
    mMessageHandler = CJavaMMSMessageHandler::NewL(mRFs);
    // If server connection then check for messages already available in MMS
    // message store.
    if (mServerConnection)
    {
        LOG(EWMA, EInfo , "CMMSPlatformServiceS60Impl::ConstructL() Check for"
            "any messages availble in message store");
        CallMethodL(this, &CMMSPlatformServiceS60Impl::NotifyReceivedMessageL,
                    this);
    }
}


void CMMSPlatformServiceS60Impl::vmAttached()
{
    JELOG2(EWMA);
    jclass sessionClass = mJniEnv->FindClass(
                              "com/nokia/mj/impl/mms/MMSConnectionImpl");
    mSendCallBackMethodId = mJniEnv->GetMethodID(sessionClass,
                            "messageSentCallback", "(I)V");

}
/**
 * Returns total number of messages that are arrived
 * from opening the connection
 */
int CMMSPlatformServiceS60Impl::getNumberOfMessages()
{
    JELOG2(EWMA);
    return mNewMessages;
}
/**
 * Does the MMS Engine side initializations .Registers with MMS Engine
 * for notification about incoming message in case of server connection.
 */
void CMMSPlatformServiceS60Impl::doServerSideInit()
{
    JELOG2(EWMA);
    FunctionServer::doServerSideInit();
    CActiveScheduler::Add(this);
    TRAPD(error,
    {
        mSession = CMsvSession::OpenSyncL(*this);
        mClientMtmRegistry = CClientMtmRegistry::NewL(*mSession);
        mMmsClient = (CMmsClientMtm *) mClientMtmRegistry->NewMtmL(
                         KUidMsgTypeMultimedia);
        mSendTMsvIdArray = RArray<TMsvId>(KArrayGranularity);
        mMmsApplicationAdapter = CMmsApplicationAdapter::NewL();
        if (mServerConnection)
        {
            mServerMmsMtm = (CMmsClientMtm *) mClientMtmRegistry->NewMtmL(
                                KUidMsgTypeMultimedia);
            mReceivedMsgArray = new(ELeave) CArrayPtrSeg
            <CMMSMessageInformation>(KArrayGranularity);
            TBool isRegister = EFalse;
            // If launched by push the application would have already registered
            // with mms engine . check before registering.
            LOG(EWMA, EInfo , "Checking whether application is already"
                "registered or not");
            isRegister = mMmsApplicationAdapter->RegisteredL
                         (*mServerApplicationId);
            // If not registered , register now.
            if (!isRegister)
            {
                TMsvId folderId;
                LOG(EWMA, EInfo , "Registering the AppId with MMS engine");
                mMmsApplicationAdapter->RegisterL(*mServerApplicationId,folderId);
                isRegister = true;
            }

            TMsvId tmsvId = mMmsApplicationAdapter->FolderIdL(
                                *mServerApplicationId);
            if (0 != tmsvId)
            {
                mRegisterCMsvEntry = mSession->GetEntryL(tmsvId);
                // Add listener to the created entry for that application Id
                mRegisterCMsvEntry->AddObserverL(*this);
            }

        }
    });
    mError = error;
}

/* -----------------------------------------------------------------------------
 * CMMSPlatformServiceS60Impl::Send
 * Sends a multimedia message to its recipients.
 * -----------------------------------------------------------------------------
 */
int CMMSPlatformServiceS60Impl::send(JNIEnv& aJniEnv,
                                     jbyteArray aBytes, int aOffset, int aLength, jstring /*aDestination*/)
{
    JELOG2(EWMA);
    // put the jbyteArray into the mSendBuffer
    delete mSendBuf; // avoid the memory leak
    mSendBuf = NULL;
    TRAPD(error,
    {
        mSendBuf = CBufFlat::NewL(aLength);

        // Extends the size of the buffer by the required size
        mSendBuf->ExpandL(aOffset, aLength);

        // Transfer the data from java
        TPtr8 sendBufPtr = mSendBuf->Ptr(0);
        S60CommonUtils::CopyToNative(aJniEnv, aBytes, aOffset, aLength,sendBufPtr);
        // If the object is active then it is currently sending a message
        // it will also send the newly added message.
        iStatus = KRequestPending;
        SetActive();
        CallMethodL(this, &CMMSPlatformServiceS60Impl::mmsRequestComplete,
                    KErrNone,this);
    });
    ELOG1(EWMA, "CMMSPlatformServiceS60Impl : status of send %d",error);
    return error;
}

/*------------------------------------------------------------------------------
 * protected SetObjectActive locally
 * .
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::mmsRequestComplete(int result)
{
    JELOG2(EWMA);
    TRequestStatus *status = &iStatus;
    User::RequestComplete(status, result);
}

/*------------------------------------------------------------------------------
 * Notifies java side about success or failure of sending MMS message
 * .
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::NotifySend(JNIEnv& aJni,jobject aPeer,
        int aError)
{
    JELOG2(EWMA);
    aJni.CallVoidMethod(aPeer, mSendCallBackMethodId, aError);
    mIsSending = false;
}

/*------------------------------------------------------------------------------
 * RunL from CActive
 * .
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::RunL()
{
    JELOG2(EWMA);
    if (iStatus.Int() ==KErrNone)
    {
        mIsSending = true;
        SendMultiPartMessageL();
    }
}

/*------------------------------------------------------------------------------
 * protected DoCancel
 * Cancel the sending of the message. If the message is in the creation phase
 * we can cancel the sending of the message. If the deviec is actually sending
 * the message we cannot cancel it.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::DoCancel()
{
    // clean up the entry
    JELOG2(EWMA);
}
/*------------------------------------------------------------------------------
 * protected RunError
 * .
 *------------------------------------------------------------------------------
 */
TInt CMMSPlatformServiceS60Impl::RunError(TInt aError)
{
    JELOG2(EWMA);
    ELOG1(EWMA, "+ In runerror %d this %d", aError);
    NotifySend(*mJniEnv, mJavaPeerObject, aError);
    return KErrNone;
}
/* -----------------------------------------------------------------------------
 * CMMSPlatformServiceS60Impl::RetrieveMessageL
 * Retrieves the first received message from the queue. After delivering the
 * message to the caller, the message is removed from the queue.
 * -----------------------------------------------------------------------------
 */
jbyteArray CMMSPlatformServiceS60Impl::retrieveMessage(JNIEnv& aJni)
{
    JELOG2(EWMA);
    // check if there is something in the queue
    if (mReceivedMMSIDs.Count() <= 0)
    {
        return NULL;
    }
    // retrieve the first message whose ID is in the queue
    TMsvId id = mReceivedMMSIDs[0];
    mReceivedMMSIDs.Remove(0);

    CMMSMessageInformation* mms = ReceivedMessage(id);
    if (!mms)
    {
        return NULL;
    }
    CleanupStack::PushL(mms);

    // serialize the received MMS
    LOG(EWMA, EInfo , "Serializing the message before giving to java");
    CBufFlat& receiveBuffer = mMessageHandler->SerializeLC(*mms);
    // receiveBuffer is on the cleanup stack
    TPtr8 receiveBufferPtr = receiveBuffer.Ptr(0);
    //get the jbyteArray
    TInt dataLength = receiveBuffer.Capacity();
    jbyteArray array = aJni.NewByteArray(dataLength);
    S60CommonUtils::CopyToJava(aJni, receiveBufferPtr, array, 0,dataLength);
    CleanupStack::PopAndDestroy(&receiveBuffer); // receiveBuffer
    CleanupStack::PopAndDestroy(mms);
    return array;
}

/*------------------------------------------------------------------------------
 * public CreateMessageL
 * create an empty messsage in draft folder. Later we will add the information
 * (attachments addresses, subject etc)
 *------------------------------------------------------------------------------
 */
TMsvId CMMSPlatformServiceS60Impl::CreateMessageL()
{
    JELOG2(EWMA);
    TMsvId serviceId = mMmsClient->DefaultServiceL();
    // Context must be set to parent folder for CreateMessageL
    mMmsClient->SwitchCurrentEntryL(KMsvDraftEntryId);
    mMmsClient->CreateMessageL(serviceId);
    CMsvEntry& cmsvEntry = mMmsClient->Entry();
    TMsvId msvId = cmsvEntry.EntryId();
    return msvId;
}

/*------------------------------------------------------------------------------
 * private DeleteSentMessage
 * When the message is gone from the device. Delete the message from
 * the sent folder.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::DeleteSentMessageL(TMsvId aMsvId)
{
    JELOG2(EWMA);
    TInt index = mSendTMsvIdArray.Find(aMsvId);
    if (KErrNotFound == index)
    {
        return;
    }
    mSendTMsvIdArray.Remove(index);
    DeleteMmsMessageL(aMsvId);
}

/*------------------------------------------------------------------------------
 * public DeleteMessage
 * Delete the message from message store. We are deleting two type of
 * multimedia messages.Messages that we send from Java side. When they
 * are gone from the device. We delete the message from the sent folder.
 * Second is when we receive the message we delete it after
 * reading it from Java side.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::DeleteMmsMessageL(TMsvId aMsvId)
{
    JELOG2(EWMA);
    LoadMessageL(aMsvId);
    CMsvEntry& cmsvEntry = mMmsClient->Entry();
    TMsvEntry tmsvEntry(cmsvEntry.Entry());

    if ((tmsvEntry.iMtm != KUidMsgTypeMultimedia)||(tmsvEntry.InPreparation()))
    {
        // It is not MMS  or the entry is not ready
        return;
    }
    //Taking a handle to the Sent folder...
    TMsvSelectionOrdering sort = cmsvEntry.SortType();
    sort.SetShowInvisibleEntries(ETrue);
    // we want to handle also the invisible entries
    // Take a handle to the parent entry
    CMsvEntry* parentEntry = CMsvEntry::NewL(mMmsClient->Session(),
                             tmsvEntry.Parent(), sort);
    CleanupStack::PushL(parentEntry); //
    TRAPD(err, parentEntry->DeleteL(aMsvId));
    if (err != KErrNone)
    {
        mSession->RemoveEntry(aMsvId);
    }
    CleanupStack::PopAndDestroy(parentEntry);
}
/*------------------------------------------------------------------------------
 * public LoadMessage
 * laod the message  from the given TMsvId,
 * After this call message will be current message
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::LoadMessageL(TMsvId aMessageEntry)
{
    JELOG2(EWMA);
    mMmsClient->SwitchCurrentEntryL(aMessageEntry);
    mMmsClient->LoadMessageL();
}
/*------------------------------------------------------------------------------
 * AddAttachments
 * add attachment, location, contentId, and set the message root here
 *
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::AddAttachmentsL(
    CMMSMessageInformation* aMidMessageInformation)
{
    JELOG2(EWMA);
    // Add attachments also set the started ID
    CMsvStore* store = mMmsClient->Entry().EditStoreL();
    CleanupStack::PushL(store);
    TInt count = aMidMessageInformation->AttachmentCount();
    for (int i = 0; i < count; i++)
    {
        CMMSMessageAttachment& messageAttachment =
            aMidMessageInformation->Attachment(i);
        TDesC& fileName = messageAttachment.AttachmentName();
        TDesC8& mimeType8 = messageAttachment.AttachmentType();
        TDesC8& contentLocation8 = messageAttachment.ContentLocationL();
        TDesC8& contentId8 = messageAttachment.ContentIdL();
        TInt charEncoding = messageAttachment.Encoding();
        TDesC* contentLocation = NULL;
        S60CommonUtils::ConvertNarrowToWiderLC(contentLocation8,contentLocation);

        CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewLC();
        // set the attachment mime type in the CMsvMimeHeaders
        if (&mimeType8)
        {
            TInt position = mimeType8.Find(KSlash);
            if (position > 0 && position < mimeType8.Length() - 1)
            {
                mimeHeaders->SetContentTypeL(mimeType8.Left(position));
                mimeHeaders->SetContentSubTypeL(mimeType8.Mid(position + 1));
            }
            else
            {
                mimeHeaders->SetContentTypeL(mimeType8);
            }
        }
        CMsvAttachment* attachment = CMsvAttachment::NewL(
                                         CMsvAttachment::EMsvFile);
        if (contentLocation)
        {
            mimeHeaders->SetContentLocationL(*contentLocation);
        }
        // contentId8 can be NULL
        if (&contentId8)
        {
            mimeHeaders->SetContentIdL(contentId8);
        }
        // set Charset in attachment
        if (charEncoding >= 0)
        {
            mimeHeaders->SetMimeCharset(charEncoding);
        }
        RFile attaFile;
        User::LeaveIfError(attaFile.Open(mRFs, fileName,
                                         EFileShareReadersOnly | EFileRead));
        CleanupClosePushL(attaFile);
        TMsvAttachmentId attaId = 0;
        // attachment ownerhip will be transferred to Attachment Manager
        // It must not be pushed onto the cleanupStack before calling
        // CreateAttachment2L
        HBufC8* emptyMime = NULL;
        if (&mimeType8)
        {
            mMmsClient->CreateAttachment2L(*store, // edit store
                                           attaFile, // open file handle
                                           mimeType8,
                                           // combination type like image/jpeg
                                           *mimeHeaders, attachment, attaId);
        }
        else
        {
            emptyMime = HBufC8::NewLC(0);
            mMmsClient->CreateAttachment2L(*store, // edit store
                                           attaFile, // open file handle
                                           *emptyMime,
                                           // combination type like image/jpeg
                                           *mimeHeaders, attachment, attaId);
        }
        // Now Attachment Manager owns the attachment object, so assign it NULL.
        attachment = NULL;
        // check also isStarted Id
        if (messageAttachment.IsStarterContentId())
        {
            mMmsClient->SetMessageRootL(attaId);
        }
        if (emptyMime)
        {
            CleanupStack::PopAndDestroy(emptyMime);
        }
        CleanupStack::PopAndDestroy(&attaFile); // attaFile.Close()
        CleanupStack::PopAndDestroy(mimeHeaders); // mimeHeaders
        CleanupStack::PopAndDestroy(contentLocation); // contentLocation
    } // end of add attachment for loop

    store->CommitL();
    CleanupStack::PopAndDestroy(store); // store
}
/*------------------------------------------------------------------------------
 * public SetSubject
 * LoadMessage is called before this method call other wise subject can be set
 * to wrong TMsvId
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::SetSubjectL(const TDesC& aSubject)
{
    JELOG2(EWMA);
    mMmsClient->SetSubjectL(aSubject);
}
/*------------------------------------------------------------------------------
 * public AddRecipient
 * LoadMessage is called before this method call other wise address can be set
 * to wrong TMsvId
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::AddRecipientL(const TDesC& aAddress,
        TMmsRecipients aAddressType)
{
    JELOG2(EWMA);
    mMmsClient->AddAddresseeL(aAddressType, aAddress);
}
/*------------------------------------------------------------------------------
 * public SetApplicationId
 * LoadMessage is called before this method call, other wise applicationId can
 * be set to wrong TMsvId.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::SetApplicationIdL(const TDesC& aApplicationId)
{
    JELOG2(EWMA);
    if (aApplicationId.Length() > 0)
    {
        TMsvId mvsEntry = mMmsClient->Entry().EntryId();
        mMmsApplicationAdapter->SetApplicationIdL(mvsEntry,aApplicationId);
    }
}
/*------------------------------------------------------------------------------
 * SetReplyToApplicationId
 * LoadMessage is called before this method call, other wise
 * ReplyToApplicationId can be set to wrong TMsvId.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::SetReplyToApplicationIdL(
    const TDesC& aReplyToApplicationId)
{
    JELOG2(EWMA);
    if (aReplyToApplicationId.Length() > 0)
    {
        TMsvId mvsEntry = mMmsClient->Entry().EntryId();
        mMmsApplicationAdapter->SetReplyToApplicationIdL(mvsEntry,
                aReplyToApplicationId);
    }
}
/*------------------------------------------------------------------------------
 * public SendMultiPartMessageL
 * ASyn method call to send the message.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::SendMultiPartMessageL()
{
    JELOG2(EWMA);
    // convert the byte array into a native multimedia message representation
    LOG(EWMA, EInfo , "Converting the byte array into a native multimedia"
        "message representation");
    CMMSMessageInformation& mms = mMessageHandler->DeSerializeL(*mSendBuf);
    // send it towards the MMS engine
    mSendMsgArray->AppendL(&mms);
    while (mSendMsgArray->Count() > 0)
    {
        CMMSMessageInformation* midMessageInformation =
            mSendMsgArray->At(0);
        CleanupStack::PushL(midMessageInformation);
        SendMmsMessageL(midMessageInformation);
        mSendMsgArray->Delete(0);
        CleanupStack::Pop(midMessageInformation); // midMessageInformation
        delete midMessageInformation;
        midMessageInformation = NULL;
    }
}
/*------------------------------------------------------------------------------
 * public SendMultiMediaMessageL
 * Message is send in the following steps
 * 1) Create an empty message in the draft folder
 * 2) Get the multimeida messsage information from the CMMSMessageInformation
 *    object and set that information to the newly created multimedia message
 *    in the following order
 *        i) Set the subject of the message (Optional)
 *        ii)Add the destination addresses (to, cc, bcc)
 *        iii)Add attachments to the message. Attachments are saved in the
 *          message store.
 *        vi)Set message setting (Date, time, expiry time, priority, visibility
 *         of the sender)
 * 3) Save the formed message in the message store
 * 4) Send the message by using mmsMTMClient.
 * 5) When the message is gone from the device delete the message from the sent
 *    folder.
 *-----------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::SendMmsMessageL(
    CMMSMessageInformation* aMidMessageInformation)
{
    JELOG2(EWMA);
    TMsvId msvId = CreateMessageL();
    // Add subject
    TDesC8& str8 = aMidMessageInformation->MessageSubjectL();
    TDesC* str = NULL;
    if (NULL != &str8)
    {
        S60CommonUtils::ConvertNarrowToWiderL(str8, str);
        SetSubjectL(*str);
        delete str;
        str = NULL;
    }
    // Add EMsvRecipientTo addresses
    TInt i = 0;
    TInt count = aMidMessageInformation->AddressesCount(EMsvRecipientTo);
    for (i = 0; i < count; i++)
    {
        AddRecipientL(aMidMessageInformation->MessageAddressL(
                          EMsvRecipientTo, i), EMsvRecipientTo);
    }
    // Add EMsvRecipientCc addresses
    count = aMidMessageInformation->AddressesCount(EMsvRecipientCc);
    for (i = 0; i < count; i++)
    {
        AddRecipientL(aMidMessageInformation->MessageAddressL(
                          EMsvRecipientCc, i), EMsvRecipientCc);
    }
    // Add EMsvRecipientBcc addresses
    count = aMidMessageInformation->AddressesCount(EMsvRecipientBcc);
    for (i = 0; i < count; i++)
    {
        AddRecipientL(aMidMessageInformation->MessageAddressL(
                          EMsvRecipientBcc, i), EMsvRecipientBcc);
    }
    // Add attachments also set the started ID
    AddAttachmentsL(aMidMessageInformation);
    // set date and time
    TInt64 time = aMidMessageInformation->MessageTime();
    SetMessageSendDate(time);
    mMmsMessagePriority = aMidMessageInformation->MessagePriority();
    // set priorty, delivery time, message class, ExpiryInterval,
    // sender visibility and delivery report
    SetMessageSettingL();

    // set application-id and reply-to-application-id
    TDesC8& id8 = aMidMessageInformation->ApplicationIdL();
    TDesC* id = NULL;
    if (NULL != &id8)
    {
        S60CommonUtils::ConvertNarrowToWiderL(id8, id);
        SetApplicationIdL(*id);
        delete id;
        id = NULL;
    }
    // Set reply-to-application-Id
    TDesC8& rId8 = aMidMessageInformation->ReplyToApplicationIdL();
    if (NULL != &rId8)
    { // add reply-to-application-id to currently loaded message
        S60CommonUtils::ConvertNarrowToWiderL(rId8, id);
        SetReplyToApplicationIdL(*id);
        delete id;
        id = NULL;
    }
    SendMmsMessageL(msvId); // send it now
    LOG(EWMA, EInfo, "CMMSPlatformServiceS60Impl::SendMmsMessageL Notifying"
        "java about the message send status");
    NotifySend(*mJniEnv, mJavaPeerObject, KErrNone);
}
/*-----------------------------------------------------------------------------
 * private SendMmsMessageL
 * Send the message from the device. This method is called when the
 * message is ready to send. All the necessary fields are filled.
 * message is send by using mmsClient mtm.
 *-----------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::SendMmsMessageL(TMsvId aSendMmsMsgId)
{
    JELOG2(EWMA);
    TMsvEntry tmvsEntry = mMmsClient->Entry().Entry();
    tmvsEntry.SetInPreparation(EFalse);
    tmvsEntry.SetVisible(ETrue);
    mMmsClient->Entry().ChangeL(tmvsEntry);
    // Save the message here to save the last changes made above.
    mMmsClient->SaveMessageL();
    // left on CleanupStack
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    wait->iStatus = KRequestPending;
    CMsvOperation* op = NULL;
    op = mMmsClient->SendL(wait->iStatus);
    wait->Start();
    CleanupStack::PushL(op);
    CActiveScheduler::Start();
    mSendTMsvIdArray.AppendL(aSendMmsMsgId);
    // The following is to ignore the completion of other active objects.
    // It is not needed if the app has a command absorbing control.
    while (wait->iStatus.Int() == KRequestPending)
    {
        CActiveScheduler::Start();
    }
    CleanupStack::PopAndDestroy(op);
    CleanupStack::PopAndDestroy(wait);
}
/*------------------------------------------------------------------------------
 * protected Set Message Setting
 * Set the class, ExpiryInterval, delivery time interval, priority and
 * visibility of the sender.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::SetMessageSettingL()
{
    JELOG2(EWMA);
    mMmsClient->RestoreSettingsL();

    mMmsClient->SetMessageClass(EMmsClassPersonal);
    // X-Mms-Delivery-Time
    mMmsClient->SetDeliveryDate(mMessageDeliveryDate);

    // X-Mms-Priority
    mMmsClient->SetMessagePriority(mMmsMessagePriority);
    mMmsClient->SetSenderVisibility(EMmsSenderVisibilityShow);
    mMmsClient->SetDeliveryReport(EMmsDeliveryReportYes);
}
/*------------------------------------------------------------------------------
 * public SetMessageSendDate
 * Set message time in Java format
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::SetMessageSendDate(TInt64& aLong)
{
    JELOG2(EWMA);
    if (aLong != 0)
    {
        mMessageDeliveryDate = S60CommonUtils::JavaTimeToTTime(aLong);
    }
    else
    {
        mMessageDeliveryDate = TTime(0);
    }
}
/*------------------------------------------------------------------------------
 * public HandleSessionEventL
 * EMsvEntriesMoved,
 *
 *    One or more entries have been moved.
 *
 *    aArg1 is a CMsvEntrySelection containing the IDs of the moved entries.
 *    aArg2 is the TMsvId of the new parent.
 *    aArg3 is the TMsvId of the old parent entry.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
{
    JELOG2(EWMA);
    switch (aEvent)
    {
    case EMsvEntriesMoved:
    {
        TMsvId parentId = KMsvNullIndexEntryId;
        parentId = *(TMsvId*) aArg2;

        if (parentId == KMsvSentEntryId)
        {
            // We take the created entries into a selection
            CMsvEntrySelection* entries =
                static_cast<CMsvEntrySelection*>(aArg1);
            TInt count = entries->Count();
            LOG1(EWMA, EInfo, "CMMSPlatformServiceS60Impl::HandleSessionEventL"
                 "Count = %d ", count);
            for (TInt i = 0; i < count; i++)
            {
                LOG1(EWMA, EInfo, "CMMSPlatformServiceS60Impl::"
                     "HandleSessionEventL number = %d ", i);
                TRAP_IGNORE(DeleteSentMessageL(entries->At(i)));
            }// end of for loop
        } // end of parentid if
        break;
    }
    default:
        break;
    }
}
/*------------------------------------------------------------------------------
 * private CreateSendMessageArray
 * Create the send message TMsvId array. It will be later used to delete the
 * messages. We should NOT be deleting the messages send by other application.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::CreateSendMessageArrayL()
{
    JELOG2(EWMA);
    ClearAndDestroySendMessageArray();
    mSendMsgArray = new(ELeave) CArrayPtrSeg<CMMSMessageInformation>
    (KArrayGranularity);
}
/*------------------------------------------------------------------------------
 * private ClearAndDestroySendMessageArray
 *
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::ClearAndDestroySendMessageArray()
{
    JELOG2(EWMA);
    if (mSendMsgArray)
    {
        mSendMsgArray->ResetAndDestroy();
    }
    delete mSendMsgArray;
    mSendMsgArray = NULL;
}

/*------------------------------------------------------------------------------
 * public ClearSendMessageArray
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::ClearSendMessageArray()
{
    JELOG2(EWMA);
    if (mSendMsgArray)
    {
        mSendMsgArray->Reset();
    }
}

/*------------------------------------------------------------------------------
 * public NotifyReceivedMessage
 * Message arrive in the device. User was prompted for the MMS message.
 * But user do not want to start the midlet. Later (after some time)
 * user start the midlet.
 * MIDlet will receive all the messages. Following method will notify
 * the listener for the received messages.
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::NotifyReceivedMessageL()
{
    JELOG2(EWMA);
    TMsvId folderId = KMsvNullIndexEntryId;
    folderId = mMmsApplicationAdapter->FolderIdL(*mServerApplicationId);

    if (KMsvNullIndexEntryId != folderId)
    {
        CMsvEntry* cEntry = mSession->GetEntryL(folderId);
        // Show invisible entries
        // Sort the entries according to date;
        cEntry->SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,
                             EMsvSortByDate, ETrue));
        //get the selection array of the messages in sorted order by date
        CMsvEntrySelection* msvEntrySelection =
            cEntry->ChildrenWithTypeL(KUidMsvMessageEntry);

        //Process each entry, one at a time.
        TInt count = msvEntrySelection->Count();
        for (TInt i = 0; i < count; i++)
        {
            // this checks the entry and handles it if it is targeted
            // to midp app we are in startup we do not want to leave here
            TRAP_IGNORE(DoReceiveMessageL(msvEntrySelection->At(i)));
        }// end of for loop
        delete msvEntrySelection;
        msvEntrySelection = NULL;
        delete cEntry;
        cEntry = NULL;
    }
} // end of the method NotifyReceivedMessageL

/*------------------------------------------------------------------------------
 * public HandleEntryEventL
 * observer the entry for the specific application Id.
 * if a new message is arrived in this folder notify the listener
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::HandleEntryEventL(
    TMsvEntryEvent aEvent, TAny* aArg1, TAny*, TAny*)
{
    JELOG2(EWMA);
    switch (aEvent)
    {
    case MMsvSessionObserver::EMsvEntriesChanged:
    {
        // We take the created entries into a selection
        CMsvEntrySelection* entries =
            static_cast<CMsvEntrySelection*>(aArg1);
        //Process each created entry, one at a time.
        for (TInt i = 0; i < entries->Count(); i++)
        { // this checks the entry and handles it if it is targeted
            // to midp app
            TRAP_IGNORE(DoReceiveMessageL(entries->At(i)));
        } // end of for loop
    }
    break;
    }
}

/*------------------------------------------------------------------------------
 * private DoReceiveMessageL
 * ReceiveMessageL MUST be call from DoReceiveMessageL
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::DoReceiveMessageL(TMsvId aMsvId)
{
    JELOG2(EWMA);
    ReceiveMessageL(aMsvId);
}
/*------------------------------------------------------------------------------
 * protected ReceiveMessageL
 *
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::ReceiveMessageL(TMsvId aMsvId)
{
    JELOG2(EWMA);
    CMsvEntry* csvEntry = mSession->GetEntryL(aMsvId);
    TMsvEntry msvEntry = csvEntry->Entry();

    if (msvEntry.iMtm != KUidMsgTypeMultimedia)
    { // It is not MMS, free up the memory of the CMsvEntry
        delete csvEntry;
        csvEntry = NULL;
        return;
    }

    if (msvEntry.InPreparation() > 0)
    { // message is not ready free up the memory of the CMsvEntry
        delete csvEntry;
        csvEntry = NULL;
        return; // entry is not ready to read
    }

    mServerMmsMtm->SwitchCurrentEntryL(aMsvId);
    mServerMmsMtm->LoadMessageL();

    // Check application ID
    TPtrC applicationId = mMmsApplicationAdapter->ApplicationIdL(aMsvId);
    // Local Copy of the Application ID
    TDesC *localCopy_AppId = NULL;
    S60CommonUtils::CopyWiderL(applicationId, (TDesC16*&) localCopy_AppId);
    // AppId and replyToAppId is not working yet so cannot be tested.
    if (mServerApplicationId->Compare(*localCopy_AppId) != 0)
    { // received message is not for this connector
        delete csvEntry;
        csvEntry = NULL; // free the memory
        return;
    }
    TPtrC replyToappId = mMmsApplicationAdapter->ReplyToApplicationIdL(aMsvId);
    // Local Copy of reply-to-Application-Id  //TDesC16*&
    TDesC *localCopy_Reply_To_AppId = NULL;
    S60CommonUtils::CopyWiderL(replyToappId,(TDesC16*&)localCopy_Reply_To_AppId);

    // receive message date
    TInt64 dateTime;
    if (mServerMmsMtm->SendingDate() != TTime(0))
    {
        dateTime = S60CommonUtils::TTimeToJavaTime(
                       mServerMmsMtm->SendingDate());
    }
    else
    {
        dateTime = S60CommonUtils::TTimeToJavaTime(msvEntry.iDate);
    }
    TDesC *localCopySender = NULL;
    // only for emulator in local mode to pass the test on the emulator
#if(defined (__WINS__) || defined(__WINSCW__))
    {
        const CMsvRecipientList& recipientList =mServerMmsMtm->AddresseeList();
        TInt recipientCount = recipientList.Count();
        for (TInt j=0; j < recipientCount; j++)
        {
            const TDesC& address = recipientList[j];
            TMsvRecipientType addressType = recipientList.Type(j);
            if (EMsvRecipientTo == addressType)
            {
                TInt len = S60CommonUtils::CopyWiderL(address,(TDesC16*&)
                                                      localCopySender);
                break;
            }
        }
    }
#else
    {
        const TPtrC sender = mServerMmsMtm->Sender();
        S60CommonUtils::CopyWiderL(sender, (TDesC16*&) localCopySender);
    }
#endif

    TInt size = mServerMmsMtm->MessageSize();
    TDesC8* applicationId8 = NULL;
    CMMSMessageInformation* receiveMsgInformation =
        CMMSMessageInformation::NewL(*localCopySender, dateTime,
                                     *applicationId8, aMsvId);
    CleanupStack::PushL(receiveMsgInformation);

    const CMsvRecipientList& recipientList = mServerMmsMtm->AddresseeList();
    TInt recipientCount = recipientList.Count();
    TDesC16* messageAddress;
    for (TInt j = 0; j < recipientCount; j++)
    {
        messageAddress = NULL;
        const TDesC& address = recipientList[j];
        TMsvRecipientType addressType = recipientList.Type(j);
        S60CommonUtils::CopyWiderL(address, messageAddress);
        receiveMsgInformation->AddAddressL((TMsvRecipientTypeValues)addressType,
                                           messageAddress);
    }

    receiveMsgInformation->SetApplicationIdL(*localCopy_AppId);
    receiveMsgInformation->SetReplyToApplicationIdL(*localCopy_Reply_To_AppId);
    receiveMsgInformation->SetMessageSize(size);
    TPtrC subject = mServerMmsMtm->SubjectL();
    receiveMsgInformation->SetMessageSubjectL(subject);
    TMmsMessagePriority priority =
        (TMmsMessagePriority) mServerMmsMtm->MessagePriority();
    receiveMsgInformation->SetMessagePriority(priority);
    ReadAttachmentsL(*receiveMsgInformation);

    mReceivedMsgArray->AppendL(receiveMsgInformation);
    CleanupStack::Pop(receiveMsgInformation);

    delete csvEntry;
    csvEntry = NULL;
    delete localCopy_Reply_To_AppId;
    localCopy_Reply_To_AppId = NULL;
    delete localCopy_AppId;
    localCopy_AppId = NULL;
    delete localCopySender;
    localCopySender = NULL;

    HBufC* buf = HBufC::NewLC(KMaxAppIDLength);
    TPtr ptr = buf->Des();
    ptr.SetLength(0); // make sure that there is no garbage
    ptr.AppendNum(aMsvId);
    NotifyJava(*buf, 1);
    CleanupStack::PopAndDestroy(buf); //buf
    TRAP_IGNORE(DeleteMmsMessageL(aMsvId));
}

/*------------------------------------------------------------------------------
 * Notifies the message listener about the incoming message
 * .
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::NotifyJava(const TDesC& aDescription,
        int aMessages)
{
    JELOG2(EWMA);
    TLex lex(aDescription);
    TMsvId ID;
    lex.Val(ID);
    TRAP_IGNORE(mReceivedMMSIDs.AppendL(ID));
    mNewMessages += aMessages;
    mMessageListener->msgArrived();
}
/*-------------------------------------------------------------------------------
 * void ReadAttachmentsL(CMMSMessageInformation& aMidMessageInformation)
 *
 *-------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::ReadAttachmentsL(
    CMMSMessageInformation& aMidMessageInformation)
{
    JELOG2(EWMA);
    // Find root attachment
    TMsvAttachmentId rootId = mServerMmsMtm->MessageRootAttachment();
    CMsvStore* store = mServerMmsMtm->Entry().ReadStoreL();
    CleanupStack::PushL(store);
    MMsvAttachmentManager& attaMan = store->AttachmentManagerL();
    CMsvAttachment* msvAttachment = NULL;
    TInt attaCount = attaMan.AttachmentCount();

    for (TInt i = 0; i < attaCount; i++)
    {
        msvAttachment = attaMan.GetAttachmentInfoL(i);
        CleanupStack::PushL(msvAttachment);
        // open other message part
        // get mime header
        CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewLC();
        mimeHeaders->RestoreL(*msvAttachment);

        TMsvAttachmentId attachmentId = msvAttachment->Id();
        RFile attaFile = attaMan.GetAttachmentFileL(attachmentId);
        TDesC8* dataArray8 = NULL;
        ReadDataFromAttachFileL(attaFile, dataArray8);
        CleanupStack::PushL(dataArray8);
        attaFile.Close();
        // mimetype and encoding from mime header
        const TDesC8& attachmentType8 = mimeHeaders->ContentType();
        const TDesC8& attachmentSubType8 = mimeHeaders->ContentSubType();
        HBufC8* attachmentTypeBuf8 = HBufC8::NewLC(attachmentType8.Length() +
                                     attachmentSubType8.Length()
                                     + KSlashLength +
                                     KSlashLength);
        TPtr8 ptr = attachmentTypeBuf8->Des();
        ptr.SetLength(0);
        ptr.Append(attachmentType8);
        ptr.Append(KSlash);
        ptr.Append(attachmentSubType8);
        // Get the values of content-id, content-location
        const TDesC8& contentId8 = mimeHeaders->ContentId();
        const TDesC& location = mimeHeaders->ContentLocation();
        TDesC8* location8 = NULL;
        // Check for the content location if we have the same string that
        // we use for sending remove the content location.
        if ((&location) && location.Find(KTempMmsAttachmentName)== KErrNotFound)
        {
            S60CommonUtils::ConvertWiderToNarrowL(location, location8);
        }
        TInt size = msvAttachment->Size();

        CMMSMessageAttachment* midAttachment =
            CMMSMessageAttachment::NewL(mRFs, *dataArray8,
                                        *attachmentTypeBuf8, contentId8, *location8,
                                        false);

        CleanupStack::PushL(midAttachment);

        TUint charEncoding = mimeHeaders->MimeCharset();
        midAttachment->SetEncoding(charEncoding);

        if (attachmentId == rootId)
        {
            midAttachment->SetStarterContentId(true);
        }
        aMidMessageInformation.InsertAttachmentL(midAttachment);
        CleanupStack::Pop(midAttachment); // midAttachment

        delete location8;
        location8 = NULL; // location8
        // attachmentTypeBuf8
        CleanupStack::PopAndDestroy(attachmentTypeBuf8);
        CleanupStack::Pop(dataArray8); // dataArray8
        // mimeHeaders, attaInfo
        CleanupStack::PopAndDestroy(mimeHeaders);
        // mimeHeaders, attaInfo
        CleanupStack::PopAndDestroy(msvAttachment);
    }
    CleanupStack::PopAndDestroy(store); // store
}
/*-------------------------------------------------------------------------------
 *
 *-------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::ReadDataFromAttachFileL(RFile& aRFile,
        TDesC8*& aDataArray)
{
    JELOG2(EWMA);
    TInt size = 0;
    TInt error = KErrNotFound;
    error = aRFile.Size(size);
    if (error == KErrNone)
    {
        HBufC8* buffer = HBufC8::NewL(size);
        if (buffer)
        {
            TPtr8 ptr = buffer->Des();
            error = aRFile.Read(ptr, size); //read the data in the file
        }
        aDataArray = buffer;
    }
}

/*------------------------------------------------------------------------------
 * public ReceivedMessage(TMsvId aMessageId)
 *
 *------------------------------------------------------------------------------
 */
CMMSMessageInformation* CMMSPlatformServiceS60Impl::ReceivedMessage(
    TMsvId aMessageId)
{
    JELOG2(EWMA);
    CMMSMessageInformation* msgInfo = NULL;
    TInt count = mReceivedMsgArray->Count();
    for (TInt i = 0; i < count; i++)
    {
        CMMSMessageInformation* mi =(CMMSMessageInformation*)
                                    mReceivedMsgArray->At(i);
        if (aMessageId == mi->MessageMsvId()) //check for the message Id
        {
            msgInfo = mi; // that is the message we are looking for.
            mReceivedMsgArray->Delete(i);
            break;
        }
    }
    return msgInfo;
}

/*------------------------------------------------------------------------------
 * public destructor
 *
 *------------------------------------------------------------------------------
 */
CMMSPlatformServiceS60Impl::~CMMSPlatformServiceS60Impl()
{
    JELOG2(EWMA);
    detachFromVm();
    stopServer();
}
void CMMSPlatformServiceS60Impl::closeConnection(JNIEnv& aJni, jobject aPeer)
{
    JELOG2(EWMA);
    // If sending of a message is being carried out unblock the java side with
    // KErrCancel.
    if (mIsSending)
    {
        NotifySend(aJni, aPeer, KErrCancel);
    }
    // Close() Needs to be called in function server thread
    TRAP_IGNORE(CallMethodL(this, &CMMSPlatformServiceS60Impl::Close, this));
}
/*------------------------------------------------------------------------------
 * private close
 *
 *------------------------------------------------------------------------------
 */
void CMMSPlatformServiceS60Impl::Close()
{
    JELOG2(EWMA);
    if (mServerConnection)
    {
        mReceivedMMSIDs.Close();
        if (mRegisterCMsvEntry)
        {
            mRegisterCMsvEntry->RemoveObserver(*this);
            delete mRegisterCMsvEntry;
            mRegisterCMsvEntry = NULL;
        }
        if (mServerMmsMtm)
        {
            delete mServerMmsMtm;
            mServerMmsMtm = NULL;
        }
        if (mReceivedMsgArray)
        {
            mReceivedMsgArray->ResetAndDestroy();
            delete mReceivedMsgArray;
            mReceivedMsgArray = NULL;
        }
        ServerConnectionFactory* serverConnectionFactory = 0;
        ServerConnection* serverConn = 0;
        serverConnectionFactory = &MmsServerConnectionFactory::getFactory();
        std::wstring Uri(L"mms://:");
        Uri += std::wstring((wchar_t*)mServerApplicationId->Ptr(),mServerApplicationId->Length());
        serverConn = serverConnectionFactory->getPushConnection(Uri);
        if (0 == serverConn)
        {
           // Unregistering from MMS Engine.
           LOG(EWMA, EInfo , "CMMSPlatformServiceS60Impl::Close() Unregistering"
               "from MMS Engine");
           mMmsApplicationAdapter->UnregisterL(*mServerApplicationId);
        }
        if (mServerApplicationId)
        {
            delete mServerApplicationId;
            mServerApplicationId = NULL;
        }
    }
    if (mMmsClient)
    {
        delete mMmsClient;
        mMmsClient = NULL;
    }

    if (mClientMtmRegistry)
    {
        delete mClientMtmRegistry;
        mClientMtmRegistry = NULL;
    }

    if (mSession)
    {
        delete mSession;
        mSession = NULL;
    }

    if (mMmsApplicationAdapter)
    {
        delete mMmsApplicationAdapter;
        mMmsApplicationAdapter = NULL;
    }
    mSendTMsvIdArray.Close();

    if (mSendMsgArray)
    {
        mSendMsgArray->ResetAndDestroy();
        delete mSendMsgArray;
        mSendMsgArray = NULL;
    }
    if (mMessageHandler)
    {
        delete mMessageHandler;
        mMessageHandler = NULL;
    }
    mRFs.Close();
}
// End of File
} //namespace wma
} //namespace java

