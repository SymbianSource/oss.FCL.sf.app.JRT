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


#include <fstream>
#include <smsustrm.h> // For RSmsSocketWriteStream
#include <gsmubuf.h>
#include <smsuaddr.h>

#include "logger.h"
#include "monitor.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "s60commonutils.h"
#include "applicationinfo.h"
#include "fileutilities.h"
#include "javacommonutils.h"
#include "connectionlistener.h"
#include "smsserverconnection.h"
#include "smsserverconnectionfactory.h"

using namespace java::util;
using namespace java::fileutils;

namespace java
{
namespace wma
{
const TInt KErrDoesNotMatchFilter = -20003;

OS_EXPORT SmsServerConnection::SmsServerConnection(const std::wstring& aUri,
        const std::wstring& aFilter)
        :CActive(EPriorityStandard),ServerConnectionBase(aUri, aFilter),
        mSocketServerOpened(EFalse),mState(EReceivingMessageForNotify)
{
    JELOG2(EWMA);
    std::wstring port = aUri.substr(KPortFieldStartIndex); // "sms://:"
    mPort = JavaCommonUtils::wstringToInt(port);
    LOG1(EWMA, EInfo, "created SmsServerConnection on port %d", mPort);
}

ServerConnection* ServerConnectionBase::getServerConnection(
    const std::wstring& aUri, const std::wstring& aFilter)
{
    JELOG2(EWMA);
    SmsServerConnection * serverConn = new SmsServerConnection(aUri, aFilter);
    return serverConn;
}

OS_EXPORT SmsServerConnection::~SmsServerConnection()
{
    JELOG2(EWMA);
    // As per internal spec the message store should be removed only when
    // Application is uninstalled / UnRegistered from push
    removeDir(mMessageStoreDirName);
    delete mMessage;
    delete mFilterDes;
    delete mOpenMonitor;
}

void SmsServerConnection::open(ConnectionListener* aListener,
                               bool aIsAppLaunched)
{
    JELOG2(EWMA);
    mIsAppLaunched = aIsAppLaunched;
    SmsServerConnection::open(aListener);
}

OS_EXPORT void SmsServerConnection::open(ConnectionListener* aListener)
{
    JELOG2(EWMA);
    int error = 0;
    mListener = aListener;
    // If we previously have messages on store while opening,
    // we immediately inform the newly opened Java WMA connection
    // of the number of messages on the queue. Otherwise we start listening.
    if (mMessagesOnStore > 0)
    {
        mListener->msgArrived();
        SmsServerConnectionFactory::getFactory().setPendingMsgFlag(mUri, false);
    }
    else if (!mIsListening)
    {
        if (0 == mOpenMonitor)
        {
            mOpenMonitor = Monitor::createMonitor();
        }
        TRAP(error,
        {
            getSocketServerL();
            if (0 == mFilterDes)
            {
                mFilterDes = S60CommonUtils::wstringToDes(mFilter.c_str());
            }
            if (0 == mMessage)
            {
                CSmsBuffer* smsBuffer = CSmsBuffer::NewL();
                CleanupStack::PushL(smsBuffer);
                mMessage = CSmsMessage::NewL(mFs, CSmsPDU::ESmsDeliver, smsBuffer);
                CleanupStack::Pop(smsBuffer);
            }
            error = pthread_mutex_init(&mMutex, 0);
            if (error == 0)
            {
                error = pthread_cond_init(&mCondVar, 0);
            }
            if (0 != error)
            {
                User::Leave(error);
            }
        });
        if (KErrNone != error)
        {
            ELOG1(EWMA,"SMS server connection open failed %d",error);
            std::string errTxt("ERROR!!! SMS server conn : Open Failed");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR,errTxt,__FILE__,
                                __FUNCTION__,__LINE__);
        }
        LOG(EWMA, EInfo, "SMS : Open a SMS Server Socket For Listening");
        // Need to bind socket
        error = mSocket.Open(mSocketServer, KSMSAddrFamily, KSockDatagram,
                             KSMSDatagramProtocol);
        if (KErrNone == error)
        {
            TSockAddr smsReceiveAddress;
            smsReceiveAddress.SetFamily(ESmsAddrApplication16BitPort);
            smsReceiveAddress.SetPort(mPort);
            LOG1(EWMA, EInfo, "SMS: Binding the Socket on Port %d",mPort);
            error = mSocket.Bind(smsReceiveAddress);
            if (KErrNone == error)
            {
                //create a message store
                const java::runtime::ApplicationInfo& appInf =
                    java::runtime::ApplicationInfo::getInstance();
                const std::wstring& root = appInf.getRootPath();
                LOG1(EWMA, EInfo,"SMS Store path %S",root.c_str());
                error = createMessageStore(root + SMS_STORE_PATH);
                mState = EReceivingMessageForNotify;
                if (KErrNone == error)
                {
                    error = pthread_create(&mThreadId, NULL,
                                           SmsServerConnection::listenThread, this);
                }
            }
        }
        if (KErrNone != error)
        {
            ELOG1(EWMA,"SMS server connection open failed %d",error);
            std::string errTxt("ERROR!!! SMS server conn : Open Failed");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR,errTxt,__FILE__,
                                __FUNCTION__,__LINE__);
        }
        mOpenMonitor->wait();
        if (mMessagesOnStore > 0)
        {
            mListener->msgArrived();
        }
        mIsListening = true;
    }
}

void* SmsServerConnection::listenThread(void* aParams)
{
    JELOG2(EWMA);
    CTrapCleanup* tc = CTrapCleanup::New();
    SmsServerConnection* serverConn =
        reinterpret_cast<SmsServerConnection*>(aParams);
    // As there is no possibility of active scheduler being installed to this
    // thread prior to this , there is no need for any checks.
    CActiveScheduler* activeScheduler = new CActiveScheduler();
    CActiveScheduler::Install(activeScheduler);
    CActiveScheduler::Add(serverConn);
    serverConn->mIoctlBuf() = KSockSelectRead;
    LOG(EWMA, EInfo, "SMS : Starting For Listening for incoming Messages");
    (serverConn->mSocket).Ioctl(KIOctlSelect, serverConn->iStatus,
                                &(serverConn->mIoctlBuf), KSOLSocket);
    serverConn->SetActive();
    (serverConn->mOpenMonitor)->notify();
    serverConn->mIsRunning = ETrue;
    activeScheduler->Start();
    delete activeScheduler;
    delete tc;
    return 0;
}

void SmsServerConnection::RunL()
{
    JELOG2(EWMA);
    int err = KErrNone;
    err = iStatus.Int();
    pthread_mutex_lock(&mMutex);
    if (err < KErrNone && mState != EExit)
    {
        // Otherwise report back to client as a system error occurred
        mListener->error(mUri, err, "Error Receiving Message");
        mState = EReceivingMessageForNotify;
        mIoctlBuf() = KSockSelectRead;
        mSocket.Ioctl(KIOctlSelect, iStatus, &mIoctlBuf, KSOLSocket);
        SetActive();
        return;
    }
    switch (mState)
    {
    case EReceivingMessageForNotify:
    {
        mState = ENotifyingReadSucceeded;
        int ioctl = KIoctlReadMessageSucceeded;

        TRAP(err,readMessageFromStackL());

        if (err != KErrNone)
        {
            // If there was an system error in saving the message,
            // we report the failure to the SMS stack, so that it will
            // return the same message next time
            if (err != KErrDiskFull && err != KErrDoesNotMatchFilter)
            {
                ioctl = KIoctlReadMessageFailed;
            }
            mState = ENotifyingReadFailed;
        }

        mSocket.Ioctl(ioctl, iStatus, &mIoctlBuf, KSolSmsProv);
        SetActive();
        break;
    }
    case ENotifyingReadSucceeded:
    {
        mMessagesOnStore++;
        // If listening by push setPendingMsgFlag to true.
        // This is required as per push framework , so that
        // listConnections & other push calls works fine.
        if (!mIsAppLaunched)
        {
            SmsServerConnectionFactory::getFactory().setPendingMsgFlag(
                mUri, true);
        }
        //Notify the listener
        mListener->msgArrived();
        mState = EReceivingMessageForNotify;
        // Start a new listen immediately
        mSocket.Ioctl(KIOctlSelect, iStatus, &mIoctlBuf, KSOLSocket);
        SetActive();
        break;
    }
    case ENotifyingReadFailed:
    {
        if (err != KErrDiskFull || err != KErrDoesNotMatchFilter)
        {
            // Report back to client as a system error occurred
            mListener->error(mUri, err, "Error Receiving Message");
        }
        mState = EReceivingMessageForNotify;
        // Start a new listen immediately.
        mSocket.Ioctl(KIOctlSelect, iStatus,
                      &mIoctlBuf, KSOLSocket);
        SetActive();
        break;
    }
    case EExit:
    {
        pthread_cond_signal(&mCondVar);
        mIsRunning = EFalse;
        CActiveScheduler::Stop();
    }
    default:
    {
        LOG(EWMA, EInfo, "SMS : Not a valid case");
        break;
    }
    }
    pthread_mutex_unlock(&mMutex);
}

void SmsServerConnection::getSocketServerL()
{
    JELOG2(EWMA);
    if (!mSocketServerOpened)
    {
        mSocketServerOpened = ETrue;
        LOG(EWMA, EInfo, "Sms :: Connecting to socket server");
        User::LeaveIfError(mSocketServer.Connect());
        User::LeaveIfError(mSocketServer.ShareAuto());
    }
}


void SmsServerConnection::close()
{
    JELOG2(EWMA);
    // the close and RunL are synchronized to make it SMP safe.
    if (mState != EExit && mIsListening)
    {
        pthread_mutex_lock(&mMutex);
        mState = EExit;
        mMessagesOnStore = 0;
        mIsListening = false;
        mIsAppLaunched = false;
        if (IsActive())
        {
            DoCancel();
        }
        if (mIsRunning)
        {
            pthread_cond_wait(&mCondVar, &mMutex);
        }
        pthread_mutex_unlock(&mMutex);
        pthread_mutex_destroy(&mMutex);
        pthread_cond_destroy(&mCondVar);
    }
}

void SmsServerConnection::DoCancel()
{
    JELOG2(EWMA);
    mSocket.CancelIoctl();
    mSocket.Close();
    mSocketServer.Close();
    mSocketServerOpened = EFalse;
}

TInt SmsServerConnection::RunError(TInt aError)
{
    JELOG2(EWMA);
    mListener->error(mUri, aError, "SMS :Error Receiving Message");
    pthread_cond_signal(&mCondVar);
    pthread_mutex_unlock(&mMutex);
    mIsRunning = EFalse;
    CActiveScheduler::Stop();
    return KErrNone;
}

OS_EXPORT int SmsServerConnection::retrieveMessage(
    TJavaMessageParametersBuf& aSmsBuf)
{
    JELOG2(EWMA);
    std::wstring path;
    path += mMessageStoreDirName;
    char* messagePath =0;
    // Read the SMS file contents
    readStream.exceptions(std::ifstream::failbit|std::ifstream::badbit);
    try
    {
        path += JavaCommonUtils::intToWstring(mFirstMessageInStore);
        messagePath = JavaCommonUtils::wstringToUtf8(path);
        readStream.open(messagePath, std::ios::in | std::ios::binary);
        readStream.read((char*) aSmsBuf.Ptr(), mSmsParameters.Size());
        readStream.read((char*)(aSmsBuf().mData).Ptr(), aSmsBuf().mDataSize * 2);
        readStream.close();
        (aSmsBuf().mData).SetLength(aSmsBuf().mDataSize);
        deleteMessage();
        delete[] messagePath;
    }
    catch (std::ifstream::failure e)
    {
        ELOG(EWMA,"SMS : Exception while opening/reading file");
        delete[] messagePath;
        readStream.close();
        return KErrGeneral;
    }
    catch (ExceptionBase ex)
    {
        delete[] messagePath;
        return KErrGeneral;
    }
    return KErrNone;
}

TBool SmsServerConnection::match()
{
    JELOG2(EWMA);
    if (mSmsParameters().mAddress.Match(*mFilterDes) != KErrNotFound)
    {
        return ETrue;
    }
    return EFalse;
}

void SmsServerConnection::readMessageFromStackL()
{
    JELOG2(EWMA);
    // We have to read the message from socket before we leave
    RSmsSocketReadStream readstream(mSocket);
    readstream >> *mMessage;
    readstream.Close();
    CSmsPDU& pdu = mMessage->SmsPDU();
    mSmsParameters().mAddress = pdu.ToFromAddress();
    //check whether we are intended to receive messages from this host
    if (0 == mFilterDes || match())
    {
        // Check available space and delete old messages if necessary
        long long availableSpace = FileUtilities::availableSize(
                                       mMessageStoreDirName);
        while ((availableSpace >= 0) && (availableSpace < KSmsMaxDataSize)
                && (mMessagesOnStore > 0))
        {
            LOG(EWMA, EInfo, "SMS : Deleting old message...");
            deleteMessage();
            availableSpace = FileUtilities::availableSize(mMessageStoreDirName);
        }
        if (availableSpace < KSmsMaxDataSize)
        {
            User::LeaveIfError(KErrDiskFull);
        }
        // Get the port that the message was sent from
        int receivedToPort;
        int receivedFromPort;
        TBool is16Bit;
        TBool portAddressing = pdu.ApplicationPortAddressing(receivedToPort,
                               receivedFromPort, &is16Bit);
        mSmsParameters().mPort = static_cast<TUint16>(receivedFromPort);
        // Get encoding
        mSmsParameters().mEncoding = (mMessage->SmsPDU()).Alphabet();
        // Get timestamp
        mSmsParameters().mTimestamp = mMessage->Time();
        // Copy SMS data into buffer to transfer back
        CSmsBufferBase& smsBuffer = mMessage->Buffer();
        mSmsParameters().mDataSize = smsBuffer.Length();
        smsBuffer.Extract(mSmsDataBuf, 0, mSmsParameters().mDataSize);
        std::wstring path(mMessageStoreDirName);
        char* messagePath = 0;
        // Write message to file
        LOG(EWMA, EInfo,"SmsPushPlugin , Writing the message to message store");
        std::ofstream writeStream;
        writeStream.exceptions(std::ofstream::failbit|std::ofstream::badbit);
        try
        {
            path += JavaCommonUtils::intToWstring(mNextMessageInStore);
            messagePath = JavaCommonUtils::wstringToUtf8(path);
            writeStream.open(messagePath, std::ios::out | std::ios::binary);
            writeStream.write((char*) mSmsParameters.Ptr(), mSmsParameters.Size());
            writeStream.write((char*) mSmsDataBuf.Ptr(), mSmsDataBuf.Size());
            writeStream.close();
        }
        catch (std::ofstream::failure e)
        {
            ELOG(EWMA,"SMS : Exception while creating/writing file");
            delete[] messagePath;
            writeStream.close();
            User::Leave(KErrGeneral);
        }
        catch (ExceptionBase ex)
        {
            User::Leave(KErrGeneral);
        }
        // Record we have another SMS
        if (mFirstMessageInStore == -1)
        {
            mFirstMessageInStore = mNextMessageInStore;
        }
        mNextMessageInStore++;
        delete[] messagePath;
    }
    else
    {
        User::LeaveIfError(KErrDoesNotMatchFilter);
    }
}

}// end of namespace wma
}// end of namespace java
