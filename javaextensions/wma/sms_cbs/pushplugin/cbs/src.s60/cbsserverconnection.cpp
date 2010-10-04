/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <fstream>

#include "logger.h"
#include "monitor.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "applicationinfo.h"
#include "cbsserverconnection.h"
#include "connectionlistener.h"
#include "cbsserverconnectionfactory.h"

//custom error code for filter mismatches
const TInt KErrDoesNotMatchMessageId = -20012;

// Default array granularity, ~half of 15 pages
const TInt KDefaultArrayGranularity = 8;

using namespace java::util;
namespace java
{
namespace wma
{
OS_EXPORT CbsServerConnection::CbsServerConnection(const std::wstring& aUri,
        const std::wstring& aFilter)
        :CActive(EPriorityStandard),ServerConnectionBase(aUri, aFilter),
        mCbsMessageReceived(EFalse),mCbsPartiallyReceived(EFalse),
        mBroadcastMsgAttributePackage(mBroadcastMsgAttributes),
        mBroadcastPhoneCapsPckg(mBroadcastPhoneCaps)
{
    JELOG2(EWMA);
    std::wstring port = aUri.substr(KPortFieldStartIndex); // "cbs://:"
    mPort = JavaCommonUtils::wstringToInt(port);
    LOG1(EWMA, EInfo, "created CbsServerConnection on ID %d", mPort);
}

ServerConnection* ServerConnectionBase::getServerConnection(
    const std::wstring& aUri, const std::wstring& aFilter)
{
    JELOG2(EWMA);
    CbsServerConnection* cbsConn = new CbsServerConnection(aUri, aFilter);
    return cbsConn;
}

OS_EXPORT CbsServerConnection::~CbsServerConnection()
{
    JELOG2(EWMA);
}

void CbsServerConnection::initializeL()
{
    JELOG2(EWMA);
    TUint32 modemNo = 0;
    // get the name of the TSY
    mTsyName = HBufC16::NewL(KCommsDbSvrMaxFieldLength);
    TPtr tsyPtr(mTsyName->Des());

    CCommsDatabase* database = CCommsDatabase::NewL(EDatabaseTypeUnspecified);
    CleanupStack::PushL(database);

    database->GetGlobalSettingL(TPtrC(MODEM_PHONE_SERVICES_SMS), modemNo);

    // Now read the TSY name from the modem table record
    CCommsDbTableView* dbTable = database->OpenViewMatchingUintLC(TPtrC(MODEM),
                                 TPtrC(COMMDB_ID), modemNo);

    User::LeaveIfError(dbTable->GotoFirstRecord());
    dbTable->ReadTextL(TPtrC(MODEM_TSY_NAME), tsyPtr);
    CleanupStack::PopAndDestroy(dbTable);
    CleanupStack::PopAndDestroy(database);


    // Make a connection to RTelServer
    User::LeaveIfError(mTelServer.Connect());
    User::LeaveIfError(mTelServer.ShareAuto());

    User::LeaveIfError(mTelServer.LoadPhoneModule(*mTsyName));

    // Get the name of the first phone
    TInt phones;
    User::LeaveIfError(mTelServer.EnumeratePhones(phones));

    // Find the one which has the cbs functionality

    while (phones--)
    {
        TName tsyPhone;
        User::LeaveIfError(mTelServer.GetTsyName(phones, tsyPhone));

        if (tsyPhone.CompareF(*mTsyName) == KErrNone)
        {
            // Get the phone info
            RTelServer::TPhoneInfo tPhone;
            User::LeaveIfError(mTelServer.GetPhoneInfo(phones, tPhone));
            User::LeaveIfError(mMobilePhone.Open(mTelServer, tPhone.iName));

            // No need to look through any more of the available phones
            break;
        }
    }
    mCbsMessagePagesData = new(ELeave) CDesCArrayFlat(KDefaultArrayGranularity);
    mOpenMonitor = Monitor::createMonitor();
    int result = pthread_mutex_init(&mMutex, 0);
    if (result == 0)
    {
        result = pthread_cond_init(&mCondVar, 0);
    }
    if (0 != result)
    {
        User::Leave(result);
    }
}

OS_EXPORT void CbsServerConnection::open(ConnectionListener* aListener,
                               bool aIsAppLaunched)
{
    JELOG2(EWMA);
    mIsAppLaunched = aIsAppLaunched;
    CbsServerConnection::open(aListener);
}

OS_EXPORT void CbsServerConnection::open(ConnectionListener* aListener)
{
    JELOG2(EWMA);
    int error = 0;
    mListener = aListener;
    //If it is push launched Message store will have received cbs messages
    //notify the listener
    if (mMessagesOnStore > 0)
    {
        mListener->msgArrived();
        CbsServerConnectionFactory::getFactory().setPendingMsgFlag(mUri, false);
    }
    else if (!mIsListening)
    {
        TRAP(error, initializeL());
        if (error != KErrNone)
        {
            ELOG1(EWMA, "CBS : Error while Initializing = %d", error);
            std::string errTxt("ERROR!!! CBS : Error while Initializing");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                                __FUNCTION__, __LINE__);
        }
        mInitialized = ETrue;
        // Set up RBroadcastMessaging
        error = mBroadcastMessage.Open(mMobilePhone);
        if (error != KErrNone)
        {
            ELOG1(EWMA, "CBS : RBroadcastMessaging Open failed : %d", error);
            std::string errTxt("ERROR!!! CBS :RBroadcastMessaging Open failed");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                                __FUNCTION__, __LINE__);
        }
        // Get the phone capabilities to find out if it is a GSM or WCDMA
        // based phone
        error = mBroadcastMessage.GetCaps(mBroadcastPhoneCapsPckg);
        if (error != KErrNone)
        {
            ELOG1(EWMA, "CBS : Get Capabilities failed : %d", error);
            std::string errTxt("ERROR!!! CBS: Get Capabilities failed");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                                __FUNCTION__, __LINE__);
        }
        // If the phone supports CDMA networks, then return KErrNotSupported
        if ((mBroadcastPhoneCaps.iModeCaps
                & RMobileBroadcastMessaging::KCapsCdmaTpduFormat)
                == RMobileBroadcastMessaging::KCapsCdmaTpduFormat)
        {
            ELOG(EWMA, "CBS : CDMA networks - Not Supported");
            std::string errTxt("ERROR!!!CBS : CDMA networks - Not Supported");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                                __FUNCTION__, __LINE__);
        }
        // Create Message Store
        const java::runtime::ApplicationInfo& appInf =
            java::runtime::ApplicationInfo::getInstance();
        const std::wstring& root = appInf.getRootPath();
        // At this point mIsAppLaunched will be true if it not push connection.
        error = createMessageStore(root + CBS_STORE_PATH, mIsAppLaunched);
        if (error != KErrNone)
        {
            ELOG1(EWMA,"CBS : create message store failed : %d",error);
            std::string errTxt("ERROR!!! CBS : create message store failed");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR,errTxt,__FILE__,
                                __FUNCTION__,__LINE__);
        }
        mState = ECbsOpen;
        //Create the listening thread which listens for Cbs messages
        error = pthread_create(&mThreadId, NULL,
                               CbsServerConnection::listenThread, this);

        if (error != KErrNone)
        {
            ELOG1(EWMA,"CBS : listenThread creation failed : %d",error);
            std::string errTxt("ERROR!!! CBS : listenThread creation failed");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR,errTxt,__FILE__,
                                __FUNCTION__,__LINE__);
        }

        mOpenMonitor->wait();
        mIsListening = true;
    }
}


void* CbsServerConnection::listenThread(void* aParams)
{
    JELOG2(EWMA);
    CTrapCleanup* tc = CTrapCleanup::New();
    CbsServerConnection* aCbsSrvConn =
        reinterpret_cast<CbsServerConnection*>(aParams);
    // As there is no possibility of active scheduler being installed to this
    // thread prior to this , there is no need for any checks.
    CActiveScheduler* activeScheduler = new CActiveScheduler();
    CActiveScheduler::Install(activeScheduler);
    CActiveScheduler::Add(aCbsSrvConn);
    aCbsSrvConn->setFilterSettings();
    aCbsSrvConn->SetActive();
    (aCbsSrvConn->mOpenMonitor)->notify();
    activeScheduler->Start();
    delete activeScheduler;
    delete tc;
    return 0;
}

void CbsServerConnection::setFilterSettings()
{
    JELOG2(EWMA);
    mBroadcastMessage.SetFilterSetting(iStatus,
                                       RMobileBroadcastMessaging::EBroadcastAcceptAll);
}


void CbsServerConnection::startCBSListening()
{
    JELOG2(EWMA);
    mState = ECbsReceivingActive;
    mBroadcastMessage.ReceiveMessage(iStatus, mBroadcastPageData,
                                     mBroadcastMsgAttributePackage);
}

void CbsServerConnection::DoCancel()
{
    JELOG2(EWMA);
    // Only cancel the appropriate asynchronous request
    switch (mState)
    {
    case ECbsOpen:
    {
        mBroadcastMessage.CancelAsyncRequest(
            EMobileBroadcastMessagingSetFilterSetting);
        break;
    }
    default:
    {
        mBroadcastMessage.CancelAsyncRequest(
            EMobileBroadcastMessagingReceiveMessage);
        break;
    }
    }
}

OS_EXPORT int CbsServerConnection::retrieveMessage(TJavaMessageParametersBuf& aCbsBuf)
{
    JELOG2(EWMA);
    TCBSParametersBuf cbsParametersBuf;
    std::wstring path;
    char* messagePath =0;
    path += mMessageStoreDirName;
    try
    {
        path += JavaCommonUtils::intToWstring(mFirstMessageInStore);
        messagePath = JavaCommonUtils::wstringToUtf8(path);
        // Read the CBS file contents
        readStream.exceptions(std::ifstream::failbit|std::ifstream::badbit);

        readStream.open(messagePath, std::ios::in | std::ios::binary);
        readStream.read((char*) cbsParametersBuf.Ptr(), cbsParametersBuf.Size());
        readStream.read((char*)(aCbsBuf().mData).Ptr(),
                        cbsParametersBuf().mDataSize);
        readStream.close();
        (aCbsBuf().mData).SetLength((cbsParametersBuf().mDataSize)>>1);
        LOG1(EWMA,EInfo,"WMA : CBS Message Data Length %d",
             cbsParametersBuf().mDataSize);
        aCbsBuf().mAddress = cbsParametersBuf().mAddress;
        aCbsBuf().mPort = 0;
        // Get encoding
        aCbsBuf().mEncoding = cbsParametersBuf().mEncoding;
        deleteMessage();
        delete[] messagePath;
    }
    catch (std::ifstream::failure e)
    {
        ELOG(EWMA,"CBS : Exception while opening/reading file");
        delete[] messagePath;
        readStream.exceptions(std::ofstream::goodbit);
        readStream.close();
        return KErrGeneral;
    }
    catch (ExceptionBase &ex)
    {
        delete[] messagePath;
        return KErrGeneral;
    }
    return KErrNone;
}


OS_EXPORT void CbsServerConnection::close()
{
    JELOG2(EWMA);
    // the close and RunL are synchronized to make it SMP safe.
    if (mInitialized)
    {
        pthread_mutex_lock(&mMutex);
        mMessagesOnStore = 0;
        mInitialized = EFalse;
        mIsListening = false;
        mIsAppLaunched = false;
        mCbsMessageReceived = EFalse;
        mCbsPartiallyReceived = EFalse;
        if (mState != ECbsReceivingDeactive)
        {
            // Cancel blocks, so it is not called. Instead we check for any
            // outstanding request and call DoCancel().
            if (IsActive())
            {
                DoCancel();
            }
            mState = ECbsReceivingDeactive;
            if (mRunning)
            {
                pthread_cond_wait(&mCondVar, &mMutex);
            }
            mBroadcastMessage.Close();
            mMobilePhone.Close();
            mTelServer.Close();
            mCbsMessagePagesRef.Reset();
            delete mCbsMessagePagesData; // invokes Reset() if necessary
            delete mOpenMonitor;
            delete mTsyName;
        }
        pthread_mutex_unlock(&mMutex);
        pthread_mutex_destroy(&mMutex);
        pthread_cond_destroy(&mCondVar);
    }
}

TInt CbsServerConnection::RunError(TInt aError)
{
    JELOG2(EWMA);
    mListener->error(mUri, aError, "CBS :Error Receiving Broadcast Message");
    mRunning = EFalse;
    pthread_cond_signal(&mCondVar);
    pthread_mutex_unlock(&mMutex);
    CActiveScheduler::Stop();
    return KErrNone;
}

void CbsServerConnection::RunL()
{
    JELOG2(EWMA);
    pthread_mutex_lock(&mMutex);
    if (iStatus.Int() < KErrNone && iStatus.Int() != KErrCancel
            && mState != ECbsReceivingDeactive)
    {
        ELOG1(EWMA,"CBS : Error Receiving Broadcast Message:%d",iStatus.Int());
        mListener->error(mUri, iStatus.Int(),
                         "CBS : Error Receiving Broadcast Message");
        if (mIsAppLaunched)
        {
            startCBSListening();
            SetActive();
        }
        else
        {
            mRunning = EFalse;
            pthread_cond_signal(&mCondVar);
            CActiveScheduler::Stop();
        }
        pthread_mutex_unlock(&mMutex);
        return;
    }
    switch (mState)
    {
    case ECbsOpen:
    {
        mRunning = ETrue;
        //start listening for cbs messages
        startCBSListening();
        SetActive();
        break;
    }
    case ECbsReceivingActive:
    {
        TRAPD(error, readMessageFromStackL());
        if (error != KErrNone)
        {
            // port number does not match
            if (error != KErrDoesNotMatchMessageId)
            {
                // Otherwise report back to client as a system error occurred
                mListener->error(mUri, error,
                                 "CBS : Error Receiving Broadcast Message");
            }
        }
        else
        {
            if (mCbsMessageReceived)
            {
                mCbsMessageReceived = EFalse;
                // we have a complete message, so notify client
                mMessagesOnStore++;
                // If listening by push setPendingMsgFlag to true.
                // This is required as per push framework , so that
                // listConnections & other push calls works fine.
                if (!mIsAppLaunched)
                {
                    CbsServerConnectionFactory::getFactory().setPendingMsgFlag(
                        mUri, true);
                }
                mListener->msgArrived();
            }
        }
        startCBSListening();
        SetActive();
        break;
    }
    case ECbsReceivingDeactive:
    {
        mRunning = EFalse;
        pthread_cond_signal(&mCondVar);
        CActiveScheduler::Stop();
        break;
    }
    default:
    {
        //Nothing to do
    }
    }
    pthread_mutex_unlock(&mMutex);
}

void CbsServerConnection::readMessageFromStackL()
{
    JELOG2(EWMA);
    // Generate a message decoder
    CJavaCbsDecoder* cbsDecoder = new CJavaCbsDecoder();
    CleanupStack::PushL(cbsDecoder);

    TCBSDataBuf messageData = decodeCbsMessageL(cbsDecoder , mBroadcastPageData);
    // Filter the message based on the message Id
    if (mCbsMessageId != mPort)
    {
        CleanupStack::PopAndDestroy(cbsDecoder);
        User::LeaveIfError(KErrDoesNotMatchMessageId);
    }

    // Handle the possiblity of the message being a multipage message

    if (mCbsTotalNumPages != 1)
    {
        // This is a part of a multipage message

        // Get information about the message
        TInt currentPage = mCbsPageNumber;
        TUint messageCode = cbsDecoder->getMessageCode();

        if (mCbsPartiallyReceived)
        {
            // check whether or not this is the same one of already received
            // partial message.
            if ((messageCode != mCbsMultipageMessageCode) ||(mCbsMessageId
                    != mCbsMultipageMessageId))
            {
                // This is a new message, so ignore the old one
                // and setup for a new multipage message

                mCbsMessagePagesData->Reset();
                mCbsMessagePagesRef.Reset();
                mPageCounter = 0;
                mCbsPartiallyReceived = EFalse;
            }
        }
        // Adding more data to a partially saved message

        // For WCDMA messages, it is assumed that the pages will arrive
        // in the correct order.  mPageCounter is used to
        // keep track of the current page number.
        if (currentPage == KUnknownPageNumber)
        {
            currentPage = ++mPageCounter;
        }
        if (!mCbsPartiallyReceived)
        {
            // Start recording a new multipage message
            mCbsReceivedPages = (1 << (currentPage - 1));
            // Set some useful flags
            mCbsMultipageMessageCode = messageCode;
            mCbsMultipageMessageId = mCbsMessageId;
            mCbsPartiallyReceived = ETrue;
        }
        // Store the received page number, such that the message can
        // be reconstructed in the correct order later
        mCbsMessagePagesRef.AppendL(currentPage);

        // Store the actual content of the message
        mCbsMessagePagesData->AppendL(messageData);

        // Record which page has been received as a bitmask
        mCbsReceivedPages |= (1 << (currentPage - 1));

        // Check if all of the pages have been received
        if (mCbsReceivedPages == ((static_cast<TUint>
                                   (1 << mCbsTotalNumPages)) - 1))
        {
            // Have received all pages
            mCbsMessageReceived = ETrue;
        }
    }
    else
    {
        // This is a single-paged message

        if (mCbsPartiallyReceived)
        {
            // Reset any existing partially stored multipage message
            mCbsMessagePagesRef.Reset();
            mCbsMessagePagesData->Reset();
            mPageCounter = 0;
            mCbsPartiallyReceived = EFalse;
        }

        mCbsMessageReceived = ETrue;
    }
    if (mCbsMessageReceived)
    {
        saveCbsMessageL(cbsDecoder->getEncoding(),messageData);
    }
    CleanupStack::PopAndDestroy(cbsDecoder);
}

TCBSDataBuf CbsServerConnection::decodeCbsMessageL(CJavaCbsDecoder* aDecoder,
        const TDesC8& aData)
{
    JELOG2(EWMA);
    TUint8 codingScheme =0;
    TUint16 serialNum =0;
    const TUint8* data = NULL;
    //Check whether phone supports GSM/WCDMA
    if (mBroadcastMsgAttributes.iFormat ==
            RMobileBroadcastMessaging::EFormatGsmTpdu)
    {
        //Get the coding scheme as TUint8 from received cbs message
        codingScheme = aData[KDataCodingSchemeIndex];
        //Find the curent PageNumber & TotalNumPages from the message pageInfo.
        aDecoder->findPageInfo(aData[KPageInfoIndex],mCbsPageNumber,
                               mCbsTotalNumPages);
        serialNum = aData[KSerialNumberUpperpart] << 8;
        serialNum = serialNum | aData[KSerialNumberLowerpart];
        // Get the Message Identifier for GSM
        mCbsMessageId = aData[KMessageIdUpperpart] << 8;
        mCbsMessageId = mCbsMessageId | aData[KMessageIdLowerpart];
        //Get the Message Data for decode without message Header
        TInt len = aData.Length();
        TPtrC8 cbsData = aData.Mid(KGsmHeaderSize, len - KGsmHeaderSize);
        data = cbsData.Ptr();
    }
    else if (mBroadcastMsgAttributes.iFormat ==
             RMobileBroadcastMessaging::EFormatWcdmaTpdu)
    {
        //Get the coding scheme as TUint8 from received cbs message
        codingScheme = mBroadcastMsgAttributes.iDCS;
        serialNum = mBroadcastMsgAttributes.iSerialNum;
        // Get the Message Identifier for wcdma
        mCbsMessageId = mBroadcastMsgAttributes.iMessageId;
        //Find the curent PageNumber & TotalNumPages for the Cbs Message
        mCbsTotalNumPages = mBroadcastMsgAttributes.iNumberOfPages;
        mCbsPageNumber = KUnknownPageNumber;
        data = aData.Ptr();
        // Leave iff total number of messages is greater then Maximum number of
        // pages supported .
        if (mCbsTotalNumPages > KMaximumNumOfPages)
        {
            User::Leave(KErrGeneral);
        }
    }
    else
    {
        //Others are not supported
        User::Leave(KErrNotSupported);
    }

    //Find the message code of the cureent cbs message
    aDecoder->findMessageCode(serialNum);

    TCBSDataBuf cbsData;
    //Decode the message
    aDecoder->decodeMessageL((TPtrC8)data, cbsData,codingScheme,mFs);
    return cbsData;
}

void CbsServerConnection::saveCbsMessageL(TSmsDataCodingScheme::TSmsAlphabet
        aEncoding,TCBSDataBuf& aMessageData)
{
    JELOG2(EWMA);
    TCBSParametersBuf cbsParams;

    HBufC* multipageMessage = NULL;

    if (!mCbsPartiallyReceived)
    {
        // Set the messageData size parameter
        cbsParams().mDataSize = aMessageData.Size();

    }
    else
    {
        // The multipage cbs message may arrive in the incorrect order.
        // Calculate the total length for reconstructed  message
        TInt totalLength = 0;
        TUint index =1;
        while (index <= mCbsTotalNumPages)
        {
            TInt pos = mCbsMessagePagesRef.FindL(index);
            totalLength += (*mCbsMessagePagesData)[pos].Length();
            index++;
        }
        // Create an buffer of sufficient size to hold the entire message
        multipageMessage = HBufC::NewLC(totalLength);
        TPtr multipagePtr = multipageMessage->Des();

        // construct the multipage message
        index =1;
        while (index <= mCbsTotalNumPages)
        {
            TInt pos = mCbsMessagePagesRef.FindL(index);
            multipagePtr.Append((*mCbsMessagePagesData)[pos]);
            index++;
        }
        cbsParams().mDataSize = multipageMessage->Size();
    }

    // Set the message encoding scheme
    cbsParams().mEncoding = aEncoding;

    std::wstring path(mMessageStoreDirName);
    char* messagePath = 0;
    std::ofstream writeStream;
    writeStream.exceptions(std::ofstream::failbit|std::ofstream::badbit);
    try
    {
        path += JavaCommonUtils::intToWstring(mNextMessageInStore);
        messagePath = JavaCommonUtils::wstringToUtf8(path);
        writeStream.open(messagePath, std::ios::out | std::ios::binary);
        writeStream.write((char*) cbsParams.Ptr(), cbsParams.Size());
        if (mCbsPartiallyReceived)
        {
            // Write the generated message to the file
            writeStream.write((char*) multipageMessage->Ptr(),
                              multipageMessage->Size());
        }
        else
        {
            // write the (single-paged) message
            writeStream.write((char*) aMessageData.Ptr(), aMessageData.Size());
        }
        writeStream.close();
    }
    catch (std::ofstream::failure e)
    {
        ELOG(EWMA,"CBS : Exception while creating/writing file");
        delete[] messagePath;
        writeStream.exceptions(std::ofstream::goodbit);
        writeStream.close();
        User::Leave(KErrGeneral);
    }
    catch (ExceptionBase &ex)
    {
        User::Leave(KErrGeneral);
    }
    delete[] messagePath;
    if (mCbsPartiallyReceived)
    {
        // Cleanup the  received multipage message
        CleanupStack::PopAndDestroy(multipageMessage);
        mCbsMessagePagesRef.Reset();
        mCbsMessagePagesData->Reset();
        mPageCounter = 0;
        mCbsPartiallyReceived = EFalse;
    }

    if (mFirstMessageInStore == KErrNotFound)
    {
        mFirstMessageInStore = mNextMessageInStore;
    }
    mNextMessageInStore++;
}

} // end of namespace wma
} // end of namespace java
