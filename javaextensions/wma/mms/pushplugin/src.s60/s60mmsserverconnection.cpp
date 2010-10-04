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


#include "logger.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "s60commonutils.h"
#include "connectionlistener.h"
#include "s60mmsserverconnection.h"
#include "mmsserverconnectionfactory.h"

_LIT16(KThreadName, "MmsServerThread");
const TInt KAppIdStartIndex = 7 ;

using namespace std;
using namespace java::util;

namespace java
{
namespace wma
{

OS_EXPORT S60MmsServerConnection::S60MmsServerConnection(const wstring& aUri,
        const wstring& aFilter)
        :CActive(EPriorityStandard), mUri(aUri), mFilter(aFilter),
        mActiveConnection(false), mIsAppLaunched(false),mListener(0),
		mUnregister(0)

{
    JELOG2(EWMA);
    mAppId = aUri.substr(KAppIdStartIndex); // "mms://:"
    LOG1(EWMA,EInfo,"created MmsServerConnection on ID %S", mAppId.c_str());
    mOpenMonitor = java::util::Monitor::createMonitor();
}

ServerConnection* MmsServerConnection::getServerConnection(const wstring& aUri,
        const wstring& aFilter)
{
    JELOG2(EWMA);
    S60MmsServerConnection* mmsConn = new S60MmsServerConnection(aUri,aFilter);
    return mmsConn;
}

OS_EXPORT S60MmsServerConnection::~S60MmsServerConnection()
{
    JELOG2(EWMA);
    delete mOpenMonitor;
}

OS_EXPORT void S60MmsServerConnection::setOpen()
{
    JELOG2(EWMA);
    mIsAppLaunched = true;
    MmsServerConnectionFactory::getFactory().setPendingMsgFlag(
        mUri, false);
}

void S60MmsServerConnection::setUnregister()
{
    LOG(EWMA, EInfo,"S60MmsServerConnection::setUnregister");
    mUnregister = true;
}

void S60MmsServerConnection::clearUnregister()
{
    LOG(EWMA, EInfo, "S60MmsServerConnection::clearUnregister");
    mUnregister = false;
}

OS_EXPORT void S60MmsServerConnection::open(ConnectionListener* aListener)
{
    JELOG2(EWMA);
    mListener = aListener;
    if (!mIsAppLaunched)
    {
        TBuf<50> threadName;
        threadName.Append(KThreadName);
        threadName.Append('_');
        TTime time;
        time.HomeTime();
        threadName.AppendNum(time.Int64());
        mState = EReceivingMessageForNotify;
        int error = mThread.Create(threadName,
                                   reinterpret_cast<TThreadFunction>(listenThread),
                                   KDefaultStackSize, NULL, this);
        if (error != KErrNone)
        {
            ELOG1(EWMA,"S60MmsServerConnection Thread create failed : %d",error);
            string errTxt("ERROR!!! Open Failed");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                                __FUNCTION__, __LINE__);
        }

        mThread.Resume();
        mOpenMonitor->wait();
        if (mError != KErrNone)
        {
            std::string errTxt("ERROR!!! Open Failed");
            throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR, errTxt, __FILE__,
                                __FUNCTION__, __LINE__);
        }
        mActiveConnection = true;
    }
}

void S60MmsServerConnection::InitializeL()
{
    JELOG2(EWMA);
    mMmsApplicationAdapter = CMmsApplicationAdapter::NewL();
    mSession = CMsvSession::OpenSyncL(*this);
    mApplicationId = S60CommonUtils::wstringToDes(mAppId.c_str());
    TMsvId folderId;
    TBool isRegistered = mMmsApplicationAdapter->RegisteredL(*mApplicationId);
    if (!isRegistered)
    { 
       mMmsApplicationAdapter->RegisterL(*mApplicationId, folderId);
    }
    TMsvId tmsvId = mMmsApplicationAdapter->FolderIdL(*mApplicationId);
    if (tmsvId != KErrNone)
    {
        // get the
        mClientEntry = mSession->GetEntryL(tmsvId);
        // Add listener to the created entry for that application Id
        mClientEntry->AddObserverL(*this);
    }
    int error = pthread_mutex_init(&mMutex, 0);
    if (error == 0)
    {
        error = pthread_cond_init(&mCondVar, 0);
    }
    if (0 != error)
    {
        User::Leave(error);
    }
}

int S60MmsServerConnection::listenThread(S60MmsServerConnection* aMmsSrvConn)
{
    JELOG2(EWMA);
    int error;
    CTrapCleanup* tc = CTrapCleanup::New();
    // As there is no possibility of active scheduler being installed to this
    // thread prior to this , there is no need for any checks.
    CActiveScheduler* activeScheduler = new CActiveScheduler();
    CActiveScheduler::Install(activeScheduler);
    CActiveScheduler::Add(aMmsSrvConn);
    TRAP(error, aMmsSrvConn->InitializeL());
    if (error != KErrNone)
    {
        ELOG1(EWMA, "error Creating CRetrieveMobilePhoneBroadcastIdList =%d",
              error);
        aMmsSrvConn->mError = error;
        (aMmsSrvConn->mOpenMonitor)->notify();
        delete activeScheduler;
        delete tc;
        (aMmsSrvConn->mThread).Close();
        return 0;
    }
    aMmsSrvConn->iStatus = KRequestPending;
    aMmsSrvConn->SetActive();
    (aMmsSrvConn->mOpenMonitor)->notify();
    activeScheduler->Start();
    delete activeScheduler;
    delete tc;
    return 0;
}

void S60MmsServerConnection::HandleSessionEventL(TMsvSessionEvent,
        TAny*, TAny*, TAny*)
{

}

void S60MmsServerConnection::HandleEntryEventL(TMsvEntryEvent aEvent,
        TAny* aArg1, TAny*, TAny*)
{
    JELOG2(EWMA);
    if (mIsAppLaunched)
    {
        // midlet is already running, Server connection can read the messages
        return;
    }

    switch (aEvent)
    {
    case MMsvSessionObserver::EMsvEntriesChanged:
    case MMsvEntryObserver::EMsvEntryChanged:
    {
        CMsvEntrySelection* msvEntrySelection =
            static_cast<CMsvEntrySelection*>(aArg1);
        //Process each entry, one at a time.
        TInt count = msvEntrySelection->Count();
        CleanupStack::PushL(msvEntrySelection);
        for (TInt i = 0; i < count; i++)
        {
            TMsvId msvId = msvEntrySelection->At(i);
            CheckForValidMessageAndPushL(msvId);

        }// end of for loop
        CleanupStack::Pop(msvEntrySelection); // entries
    }
    break;
    default:
        //  Nothing to do
        break;
    }
}
//--------------------------------------------------------------------------------------------
// CheckForValidMessageAndPush
//--------------------------------------------------------------------------------------------
void S60MmsServerConnection::CheckForValidMessageAndPushL(TMsvId aMsgId)
{
    JELOG2(EWMA);
    if (!IsMmsReadyToReadL(aMsgId))
    {

        return; // message is not reday
    }

    TPtrC appId = mMmsApplicationAdapter->ApplicationIdL(aMsgId);
    if (mApplicationId->Compare(appId) == 0)
    {
        MmsRequestCompleted();
    }
}

TBool S60MmsServerConnection::IsMmsReadyToReadL(TMsvId aMsvId)
{
    JELOG2(EWMA);
    TBool error = EFalse;
    CMsvEntry* csvEntry = mSession->GetEntryL(aMsvId);
    TMsvEntry tmsvEntry = csvEntry->Entry();

    if (tmsvEntry.iMtm != KUidMsgTypeMultimedia)
    {
        delete csvEntry;
        csvEntry = NULL;
    }
    // Check if the entry is ready to be read
    else if (!tmsvEntry.InPreparation())
    { //Not in preparation
        error = ETrue;
        delete csvEntry;
        csvEntry = NULL;
    }
    return error;
}

void S60MmsServerConnection::DoCancel()
{
    JELOG2(EWMA);
    // Only cancel the appropriate asynchronous request
    TRequestStatus *status = &iStatus;
    mThread.RequestComplete(status, KErrCancel);
}

OS_EXPORT wstring S60MmsServerConnection::getUri() const
{
    JELOG2(EWMA);
    return mUri;
}

OS_EXPORT void S60MmsServerConnection::setFilter(const std::wstring& aFilter)
{
    JELOG2(EWMA);
    mFilter = aFilter;
}

OS_EXPORT wstring S60MmsServerConnection::getFilter() const
{
    JELOG2(EWMA);
    return mFilter;
}

OS_EXPORT void S60MmsServerConnection::close()
{
    JELOG2(EWMA);
    // the close and RunL are synchronized to make it SMP safe.
    if (mActiveConnection)
    {
        pthread_mutex_lock(&mMutex);
        mActiveConnection = false;
        mState = ENotifyingClose;
        MmsRequestCompleted();
        pthread_cond_wait(&mCondVar, &mMutex);
        pthread_mutex_unlock(&mMutex);
        pthread_mutex_destroy(&mMutex);
        pthread_cond_destroy(&mCondVar);
    }
    mIsAppLaunched = false;
    delete mApplicationId;
    mApplicationId = 0;
}

TInt S60MmsServerConnection::RunError(TInt aError)
{
    JELOG2(EWMA);
    mListener->error(mUri, aError, "Error Receiving MultiMedia Message");
    pthread_cond_signal(&mCondVar);
    pthread_mutex_unlock(&mMutex);
    return KErrNone;
}

void S60MmsServerConnection::RunL()
{
    JELOG2(EWMA);
    pthread_mutex_lock(&mMutex);
    switch (mState)
    {
    case EReceivingMessageForNotify:
    case EReceivingMessageForListen:
    {
        mState = ENotifyingReadMessageSucceeded;
        MmsServerConnectionFactory::getFactory().setPendingMsgFlag(
            mUri, true);
        mListener->msgArrived();
        break;
    }
    case ENotifyingReadMessageSucceeded:
    {
        break;
    }
    case ENotifyingClose:
    {
        if (mClientEntry)
        {
           mClientEntry->RemoveObserver(*this);
           if (mMmsApplicationAdapter)
           {
              if (mUnregister)
              {
                  LOG(EWMA, EInfo, "Unregistering");
                  mMmsApplicationAdapter->UnregisterL(*mApplicationId);
              }
              delete mMmsApplicationAdapter;
              mMmsApplicationAdapter = NULL;
           }
        
            delete mClientEntry;
            mClientEntry = NULL;
        }
        if (mSession)
        {
            delete mSession;
            mSession = NULL;
        }
        pthread_cond_signal(&mCondVar);
        CActiveScheduler::Stop();
        break;
    }
    default:
    {
        ELOG(EWMA,"MMS : Not a Valid Case");
    }
    }
    if (mState != ENotifyingClose)
    {
        iStatus = KRequestPending;
        SetActive();
    }
    pthread_mutex_unlock(&mMutex);
}

//-----------------------------------------------------------------------------
// MmsRequestCompleted
// Start a push midlet register for the mms push
//-----------------------------------------------------------------------------
void S60MmsServerConnection::MmsRequestCompleted()
{
    if (IsActive())
    {
        TRequestStatus *status = &iStatus;
        mThread.RequestComplete(status, KErrNone);
    }
}

}//end of namespace wma
}//end of namespace java

