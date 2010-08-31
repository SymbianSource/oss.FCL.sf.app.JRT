/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <memory>

#include <javausermessages.rsg>

#include <stringresourcereader.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <securitydefs.h>
#else
#include <securitydefs.h>
#include <securitydefsconst.h>
#endif

#include "javacertstoreimpl.h"
#include "javacertstoretoken.h"
#include "logger.h"
#include "javasymbianoslayer.h"
#include "comms.h"
#include "javacertstoretoken.h"
#include "javacommonutils.h"
#include "s60commonutils.h"
#include "securitycommsmessagedefs.h"

#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
#include "confirmquery.h"
#else
#include <avkon.rsg>
#include <AknGlobalConfirmationQuery.h>
#endif


//_LIT(KMIDP2TrustRoot, "J2ME MIDP2 Trust Root");
_LIT(KMIDP2TrustRoot, "Java Trust Root");
_LIT(KJavaUserMessagesResourceFileName, "javausermessages.rsc");
//_LIT( KRuntimeSecPolicyResourceFileName, "z:midp2runtimesecuritypolicy.rsc" );

//const TUid KCertManUIViewTrustJavaInstallingId = { 0x101F9B28 };
//const TUid KCertManUIViewTrustJavaInstallingId = { KMidletInstallApplicabilityUi };

using namespace java::security;
using namespace java::comms;

/**
 *
 */
CJavaCertStoreImpl::CJavaCertStoreImpl(CJavaCertStoreToken& aToken)
        : CActive(EPriorityNormal),mToken(aToken),mClientStatus(NULL),mState(EInitial),mTempCertData(0)
{
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    mQuery.reset(0);
#endif        
}

/**
 *
 */
CJavaCertStoreImpl::~CJavaCertStoreImpl()
{

    if (IsActive())
    {
        Cancel();
    }

    mComms.unregisterDefaultListener(this);
    mComms.disconnect();
    mCertsData.ResetAndDestroy();
    mUiThread.Close();
}

/**
 *
 */
CJavaCertStoreImpl* CJavaCertStoreImpl::NewL(CJavaCertStoreToken& aToken)
{
    CJavaCertStoreImpl* impl = new(ELeave) CJavaCertStoreImpl(aToken);
    CleanupStack::PushL(impl);
    impl->ConstructL();
    CleanupStack::Pop(impl);
    return impl;
}

/**
 *
 */
void CJavaCertStoreImpl::ConstructL()
{

    CActiveScheduler::Add(this);

    int err = mComms.registerDefaultListener(this);
    if (0 != err)
    {
        std::string errTxt("Registering comms listener failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(err));
        ELOG1(EJavaSecurity, "ERROR!!! %s",errTxt.c_str());
        User::Leave(KErrCouldNotConnect);
    }
    err = mComms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    if (0 != err)
    {
        std::string errTxt("Comms connection failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(err));
        ELOG1(EJavaSecurity, "ERROR!!! %s",errTxt.c_str());
        User::Leave(KErrCouldNotConnect);
    }
    err = mUiThread.Open(RThread().Id());
    if (KErrNone != err)
    {
        std::string errTxt("Initialization of thread failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(err));
        ELOG1(EJavaSecurity, "ERROR!!! %s",errTxt.c_str());
        User::Leave(err);
    }
}

/**
 *
 */
MCTToken& CJavaCertStoreImpl::Token()
{
    return mToken;
}

/**
 *
 */
void CJavaCertStoreImpl::processMessage(java::comms::CommsMessage& aMessage)
{
    //Request has been cancelled if it is not active anymore.
    if (!IsActive())
        return;
    mReceivedCommsMsg = aMessage;
    //ActiveScheduler calls RunL() operation after request has been completed.
    TRequestStatus* stat = &iStatus;
    mUiThread.RequestComplete(stat,KErrNone);
}

/**
 *
 */
void CJavaCertStoreImpl::RunL()
{
    TInt status = iStatus.Int();

    switch (mState)
    {
    case EListing:
        mState = EInitial;
        //In current implementation status cannot be any than KErrNone
        //in this state.
        if (KErrNone == status)
        {
            FillCertificateInfos(*mClientList);
        }
        break;

    case EPreDisabling:
        SendDisableMsg(status);
        return;

    case EPreDeleting:
        SendDeleteMsg(status);
        return;

    default:
        //Do nothing.
        break;
    }

    User::RequestComplete(mClientStatus,status);
}

/**
 *
 */
void CJavaCertStoreImpl::DoCancel()
{
    if (IsActive() && mClientStatus)
    {
        User::RequestComplete(mClientStatus,KErrCancel);
    }
}

/**
 *
 */
TInt CJavaCertStoreImpl::RunError(TInt /*aError*/)
{
    return KErrNone;
}

/**
 *
 */
void CJavaCertStoreImpl::Add(const TDesC& /*aLabel*/, TCertificateFormat,
                             TCertificateOwnerType,
                             const TKeyIdentifier* /*aSubjectKeyId*/,
                             const TKeyIdentifier* /*aIssuerKeyId*/,
                             const TDesC8& /*aCert*/, TRequestStatus& aStatus)
{
    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;
    User::RequestComplete(pRequestStatus,KErrNotSupported);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelAdd()
{
}

/**
 *
 */
void CJavaCertStoreImpl::Remove(const CCTCertInfo& aCertInfo,TRequestStatus& aStatus)
{
    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    TBool flag = CheckCapability(ECapabilityWriteDeviceData,pRequestStatus);
    if (!flag)
    {
        return;
    }

    CJavaCertData* certData = 0;
    flag = GetCertDataObj(aCertInfo,pRequestStatus,certData);
    if (!flag)
    {
        return;
    }

    if (!certData->mCanDelete)
    {
        User::RequestComplete(pRequestStatus,KErrArgument);
        return;
    }

    mState = EPreDeleting;
    mTempCertData = certData;
    HandleDeleteDisableQuery(aStatus, false /* disableCertQuery */);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelRemove()
{
}

/**
 *
 */
void CJavaCertStoreImpl::SetApplicability(const CCTCertInfo& aCertInfo,
        const RArray<TUid>& aApplications,
        TRequestStatus &aStatus)
{

    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    TBool flag = CheckCapability(ECapabilityWriteDeviceData,pRequestStatus);
    if (!flag)
    {
        return;
    }

    TState state = EPreDisabling;
    for (TInt i = 0; i < aApplications.Count(); i++)
    {
        if (KMidletInstallApplicabilityUid == aApplications[i])
        {
            state = EPreEnabling;
            break;
        }
    }

    CJavaCertData* certData = 0;
    //GetCertDataObj() operation checks whether cert is deleted.
    flag = GetCertDataObj(aCertInfo,pRequestStatus,certData);
    if (!flag)
    {
        return;
    }

    if ((EPreDisabling == state) && (!certData->mCanDisable))
    {
        User::RequestComplete(pRequestStatus,KErrArgument);
        return;
    }

    if (EPreDisabling == state)
    {
//RD_JAVA_S60_RELEASE_5_0_IAD flag is needed because otherwise "disable cert" confirmation
//prompt is shown twice. First time by unicertstore plugin of legacy java and second
//time by unicertstore plugin of OMJ.
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
        HandleSendingEnableDisableMsg(pRequestStatus,EPreDisabling,*certData);
#else
        mState = state;
        mTempCertData = certData;
        HandleDeleteDisableQuery(aStatus, true /* disableCertQuery */);
#endif
        return;
    }
    HandleSendingEnableDisableMsg(pRequestStatus,EPreEnabling,*certData);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelSetApplicability()
{
}

/**
 *
 */
void CJavaCertStoreImpl::SetTrust(const CCTCertInfo& aCertInfo, TBool aTrusted,
                                  TRequestStatus& aStatus)
{

    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    TBool flag = CheckCapability(ECapabilityWriteDeviceData,pRequestStatus);
    if (!flag)
    {
        return;
    }

    CJavaCertData* certData = 0;
    flag = GetCertDataObj(aCertInfo,pRequestStatus,certData);
    if (!flag)
    {
        return;
    }

    if (!aTrusted)
    {
        User::RequestComplete(pRequestStatus,KErrArgument);
        return;
    }

    User::RequestComplete(pRequestStatus,KErrNone);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelSetTrust()
{
}

/**
 *
 */
void CJavaCertStoreImpl::List(RMPointerArray<CCTCertInfo>& aCerts,
                              const CCertAttributeFilter& aFilter,
                              TRequestStatus& aStatus)
{

    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    if (IsActive())
    {
        User::RequestComplete(pRequestStatus,KErrInUse);
        return;
    }

    TBool validRequest = validateCertAttrFilter(aFilter);
    if (!validRequest)
    {
        LOG(EJavaSecurity, EInfo, "No certificates matching the filter supplied");
        User::RequestComplete(pRequestStatus,KErrNone);
        return;
    }

    if (0 < mCertsData.Count())
    {
        TRAPD(err,FillCertInfoFromCacheL(aCerts));
        User::RequestComplete(pRequestStatus,err);
        return;
    }

    mClientStatus = &aStatus;

    mClientList = &aCerts;
    iStatus = KRequestPending;
    SetActive();

    CommsMessage commsMsg;
    commsMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    commsMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    commsMsg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    commsMsg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    commsMsg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    commsMsg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    commsMsg << JAVA_CERT_STORE_FILTER_ID_STATE
    << (JAVA_CERT_STORE_STATE_ENABLED | JAVA_CERT_STORE_STATE_DISABLED);
    int err = mComms.send(commsMsg);
    if (0 != err)
    {
        std::string errTxt("Sending a comms request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(err));
        ELOG1(EJavaSecurity, "ERROR!!! %s",errTxt.c_str());
        User::RequestComplete(mClientStatus, KErrCommsBreak);
    }
    mState = EListing;
}

/**
 *
 */
void CJavaCertStoreImpl::CancelList()
{
    if (IsActive())
    {
        Cancel();
    }
}

/**
 *
 */
void CJavaCertStoreImpl::GetCert(CCTCertInfo*& aCertInfo,
                                 const TCTTokenObjectHandle& aHandle,
                                 TRequestStatus& aStatus)
{

    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    TInt index = GetIndexToCertData(aHandle);
    if (-1 == index)
    {
        User::RequestComplete(pRequestStatus,KErrArgument);
        return;
    }

    CJavaCertData* certData = mCertsData[index];
    TRAPD(err,
          aCertInfo = CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                        ECACertificate,certData->mSizeOfCert,
                                        NULL,NULL,Token(),index,certData->mCanDelete);
         ); // TRAPD

    User::RequestComplete(pRequestStatus,err);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelGetCert()
{
}

/**
 *
 */
void CJavaCertStoreImpl::Applications(const CCTCertInfo& aCertInfo,RArray<TUid>& aApplications,
                                      TRequestStatus& aStatus)
{

    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    //GetIndexToCertData() operation checks deleted case.
    CJavaCertData* certData = 0;
    TBool flag = GetCertDataObj(aCertInfo,pRequestStatus,certData);
    if (!flag)
    {
        return;
    }

    TInt err = KErrNone;
    if (!certData->mIsDisabled)
    {
        err = aApplications.Append(KMidletInstallApplicabilityUid);
    }

    User::RequestComplete(pRequestStatus, err);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelApplications()
{
}

/**
 *
 */
void CJavaCertStoreImpl::IsApplicable(const CCTCertInfo& aCertInfo,TUid aApplication,
                                      TBool& aIsApplicable,TRequestStatus& aStatus)
{

    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    if (KMidletInstallApplicabilityUid != aApplication)
    {
        aIsApplicable = EFalse;
        User::RequestComplete(pRequestStatus,KErrNone);
        return;
    }

    TCTTokenObjectHandle handle = aCertInfo.Handle();
    TInt certDataIndex = -1;
    if ((handle.iObjectId >= 0) && (handle.iObjectId < mCertsData.Count()))
    {
        if (mCertsData[handle.iObjectId]->mDeleted)
        {
            aIsApplicable = EFalse;
            User::RequestComplete(pRequestStatus,KErrNone);
            return;
        }
        certDataIndex = handle.iObjectId;
    }
    if (-1 == certDataIndex)
    {
        User::RequestComplete(pRequestStatus,KErrArgument);
        return;
    }
    CJavaCertData* certData = mCertsData[certDataIndex];

    aIsApplicable = ETrue;
    if (certData->mIsDisabled)
    {
        aIsApplicable = EFalse;
    }
    User::RequestComplete(pRequestStatus,KErrNone);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelIsApplicable()
{
}

/**
 *
 */
void CJavaCertStoreImpl::Trusted(const CCTCertInfo& aCertInfo,
                                 TBool& aTrusted,TRequestStatus& aStatus)
{

    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    CJavaCertData* certData = 0;
    //GetCertDataObj() operation returns error if cert has been deleted.
    TBool flag = GetCertDataObj(aCertInfo,pRequestStatus,certData);
    if (!flag)
    {
        return;
    }

    //Java certificates can be always trusted because this operation is never called
    //for java certificate.
    aTrusted = ETrue;
    User::RequestComplete(pRequestStatus,KErrNone);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelTrusted()
{
}

/**
 *
 */
void CJavaCertStoreImpl::Retrieve(const CCTCertInfo& aCertInfo,
                                  TDes8& aEncodedCert,TRequestStatus& aStatus)
{

    aStatus = KRequestPending;
    TRequestStatus* pRequestStatus = &aStatus;

    TBool flag = CheckCapability(ECapabilityReadUserData,pRequestStatus);
    if (!flag)
    {
        return;
    }

    if (IsActive())
    {
        User::RequestComplete(pRequestStatus,KErrInUse);
        return;
    }

    //mClientStatus = pRequestStatus;

    CJavaCertData* certData = 0;
    flag = GetCertDataObj(aCertInfo,pRequestStatus,certData);
    if (!flag)
    {
        return;
    }

    // copy certData->mCertContent into aEncodedCert
    //aEncodedCert = (const TUint8*)certData->mCertContent.c_str();
    TPtr8 ptr8((TUint8 *)(certData->mCertContent.c_str()), certData->mSizeOfCert);
    ptr8.SetLength(certData->mSizeOfCert);
    aEncodedCert.Copy(ptr8);

    User::RequestComplete(pRequestStatus, KErrNone);
}

/**
 *
 */
void CJavaCertStoreImpl::CancelRetrieve()
{
    if (IsActive())
    {
        Cancel();
    }
}

/**
 *
 */
TBool CJavaCertStoreImpl::validateCertAttrFilter(const CCertAttributeFilter& aFilter) const
{

    //0x101F9B28 - Midlet suite signing certificates
    //UID  taken from from security subsystem
    //(Applications for certificates from this cert store)
    TBool validRequest = ETrue;
    if (aFilter.iUidIsSet && aFilter.iUid.iUid != KMidletInstallApplicabilityUid.iUid)
    {
        validRequest = EFalse;
    }
    if (aFilter.iFormatIsSet && validRequest)
    {
        validRequest = (aFilter.iFormat == EX509Certificate);
    }
    if (aFilter.iOwnerTypeIsSet && validRequest)
    {
        validRequest = (aFilter.iOwnerType == ECACertificate);
    }
    if (aFilter.iSubjectKeyIdIsSet && validRequest)
    {
        validRequest = EFalse;//We do not have a subject key id
    }
    if (aFilter.iIssuerKeyIdIsSet && validRequest)
    {
        validRequest = EFalse;//We do not have a issuer key id
    }
    if (aFilter.iLabelIsSet && validRequest)
    {
        validRequest = (aFilter.iLabel == KMIDP2TrustRoot);
    }
    return validRequest;
}

/**
 *
 */
void CJavaCertStoreImpl::FillCertificateInfos(RMPointerArray<CCTCertInfo>& aInfoArray)
{

    std::vector<std::string> paths(10);
    long long certSize;
    TInt i = 0;
    CCTCertInfo* tmpCertInfo = 0;

    //Comms message contains the content's length, the content, the hash and the state
    while ((mReceivedCommsMsg >> certSize && certSize != 0))
    {
        std::string certContent;
        std::string idStr;
        int encodedState = -1;
        mReceivedCommsMsg >> certContent;
        mReceivedCommsMsg >> idStr;
        mReceivedCommsMsg >> encodedState;
        if (0 == idStr.size())
            continue;
        if (-1 == encodedState)
            continue;

        std::auto_ptr<CJavaCertData> certData
        (new CJavaCertData(certContent,certSize,idStr,encodedState));
        TEntry entry;
        int err;
        TRAP(err,tmpCertInfo = CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                               ECACertificate,certSize,
                               NULL,NULL,Token(),i,
                               certData->mCanDelete));
        if (KErrNone != err)
            continue;
        err = mCertsData.Insert(certData.release(),i);
        if (KErrNone != err)
        {
            tmpCertInfo->Release();
            continue;
        }
        err = aInfoArray.Append(tmpCertInfo);
        if (KErrNone != err)
        {
            tmpCertInfo->Release();
            delete mCertsData[i];
            mCertsData.Remove(i);
            continue;
        }
        i++;
    }//end while
}

/**
 *
 */
void CJavaCertStoreImpl::FillCertInfoFromCacheL(RMPointerArray<CCTCertInfo>& aInfoArray)
{

    CJavaCertData* certData = NULL;
    for (TInt i = 0; i < mCertsData.Count(); ++i)
    {
        certData = mCertsData[i];
        if (certData->mDeleted)
            continue;

        CCTCertInfo* certInfo = CCTCertInfo::NewL(KMIDP2TrustRoot,EX509Certificate,
                                ECACertificate,certData->mSizeOfCert,
                                NULL,NULL,Token(),i,certData->mCanDelete);
        TInt err = aInfoArray.Append(certInfo);
        User::LeaveIfError(err);
    }//end for
}

/**
 *
 */
TInt CJavaCertStoreImpl::GetIndexToCertData(const TCTTokenObjectHandle& aHandle)
{

    if ((aHandle.iObjectId >= 0) && (aHandle.iObjectId < mCertsData.Count()))
    {
        if (mCertsData[aHandle.iObjectId]->mDeleted)
            return -1;
        return aHandle.iObjectId;
    }
    return -1;
}

/**
 *
 */
TBool CJavaCertStoreImpl::GetCertDataObj(const CCTCertInfo& aCertInfo,
        TRequestStatus* aRequestStatus,
        CJavaCertData*& aOutCertDataObj)
{

    TCTTokenObjectHandle handle = aCertInfo.Handle();
    TInt certDataIndex = GetIndexToCertData(handle);
    if (-1 == certDataIndex)
    {
        User::RequestComplete(aRequestStatus,KErrArgument);
        return EFalse;
    }

    aOutCertDataObj = mCertsData[certDataIndex];
    return ETrue;
}

/**
 *
 */
TBool CJavaCertStoreImpl::SendDisableEnableCommsMsg(const std::string& aId,
        TState aState,
        TRequestStatus* aRequestStatus)
{

    CommsMessage commsMsg;
    commsMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    commsMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    commsMsg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    commsMsg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    if (EPreEnabling == aState)
        commsMsg << JAVA_CERT_STORE_OPERATION_ENABLE_CERT;
    else
        commsMsg << JAVA_CERT_STORE_OPERATION_DISABLE_CERT;
    commsMsg << aId;
    int err = mComms.send(commsMsg);
    if (0 != err)
    {
        std::string errTxt("Sending a comms request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(err));
        ELOG1(EJavaSecurity, "ERROR!!! %s",errTxt.c_str());
        User::RequestComplete(aRequestStatus,KErrCommsBreak);
        return EFalse;
    }
    return ETrue;
}

/**
 *
 */
TBool CJavaCertStoreImpl::SendDeleteCommsMsg(const std::string& aId,
        TRequestStatus* aRequestStatus)
{
    CommsMessage commsMsg;
    commsMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    commsMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    commsMsg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    commsMsg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    commsMsg << JAVA_CERT_STORE_OPERATION_DELETE_CERT;
    commsMsg << aId;
    int err = mComms.send(commsMsg);
    if (0 != err)
    {
        std::string errTxt("Sending a comms request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(err));
        ELOG1(EJavaSecurity, "ERROR!!! %s",errTxt.c_str());
        User::RequestComplete(aRequestStatus,KErrCommsBreak);
        return EFalse;
    }

    return ETrue;
}

/**
 *
 */
TBool CJavaCertStoreImpl::CheckCapability(const TCapability& aCapability,TRequestStatus* aRequestStatus)
{
    RThread thread;
    if (!thread.HasCapability(aCapability,0))
    {
        ELOG(EJavaSecurity, "ERROR!!! List(): Not enough capabilities to call this operation");
        User::RequestComplete(aRequestStatus,KErrPermissionDenied);
        return EFalse;
    }
    return ETrue;
}

/**
 *
 */
void CJavaCertStoreImpl::HandleDeleteDisableQuery(TRequestStatus &aRequestStatus, bool disableCertQuery)
{
    TInt leaveStatus = KErrNone;
    if (disableCertQuery)
    {
#ifndef RD_JAVA_S60_RELEASE_5_0_IAD
    TRAP(leaveStatus,ShowQueryL(R_JAVA_SECUR_CERT_DISABLING));
#endif
    }
    else
    {
    TRAP(leaveStatus,ShowQueryL(R_JAVA_SECUR_CERT_DELETING));
    }
    if (KErrNone == leaveStatus)
    {
        mClientStatus = &aRequestStatus;
        SetActive();
        return;
    }
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    delete mQuery.release();
#endif        
    mState = EInitial;
    TRequestStatus* pRequestStatus = &aRequestStatus;
    User::RequestComplete(pRequestStatus,KErrCancel);
}

/**
 *
 */
void CJavaCertStoreImpl::ShowQueryL(TInt resourceId)
{
    TFileName resourceFileName = java::util::S60CommonUtils::ResourceLanguageFileNameL(
                                     KJavaUserMessagesResourceFileName);

    std::auto_ptr<CStringResourceReader> reader(CStringResourceReader::NewL(resourceFileName));
    std::auto_ptr<HBufC> queryPrompt(reader->ReadResourceString(resourceId).AllocL());

#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    mQuery.reset(CAknGlobalConfirmationQuery::NewL());
    mQuery->ShowConfirmationQueryL(iStatus,queryPrompt->Des(),R_AVKON_SOFTKEYS_OK_CANCEL);
#else
    int completeStatus = KErrCancel;
    ConfirmQuery* query = new ConfirmQuery();
    QString queryText = QString::fromStdWString(
            std::wstring((wchar_t*) queryPrompt->Ptr(), 
                    queryPrompt->Length())); 
    if (query->accept(queryText))
    {
        completeStatus = KErrNone;
    }
    delete query; query = NULL;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, completeStatus);
#endif
    
}

/**
 *
 */
void CJavaCertStoreImpl::SendDisableMsg(TInt aStatus)
{

    mState = EInitial;
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    delete mQuery.release();
#endif        
    mState = EInitial;
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    if (EAknSoftkeyOk != aStatus)
#else
    if (KErrNone != aStatus)
#endif        
    {
        User::RequestComplete(mClientStatus,KErrCancel);
        return;
    }
    HandleSendingEnableDisableMsg(mClientStatus,EPreDisabling,*mTempCertData);
}

/**
 *
 */
void CJavaCertStoreImpl::SendDeleteMsg(TInt aStatus)
{

    mState = EInitial;
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    delete mQuery.release();
#endif        
    mState = EInitial;
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    if (EAknSoftkeyOk != aStatus)
#else
    if (KErrNone != aStatus)
#endif        
    {
        User::RequestComplete(mClientStatus,KErrCancel);
        return;
    }
    HandleSendingDeleteMsg(mClientStatus,*mTempCertData);
}

/**
 *
 */
void CJavaCertStoreImpl::HandleSendingEnableDisableMsg(TRequestStatus* aRequestStatus,
        TState aState,CJavaCertData& aCertDataObj)
{

    TBool flag = SendDisableEnableCommsMsg(aCertDataObj.mId,aState,aRequestStatus);
    if (!flag)
    {
        //SendDisableEnableCommsMsg() operation calls RequestComplete()
        //operation in the error situation.
        return;
    }
    if (EPreEnabling == aState)
    {
        aCertDataObj.mIsDisabled = EFalse;
    }
    else if (EPreDisabling == aState)
    {
        aCertDataObj.mIsDisabled = ETrue;
    }

    User::RequestComplete(aRequestStatus,KErrNone);
}

/**
 *
 */
void CJavaCertStoreImpl::HandleSendingDeleteMsg(TRequestStatus* aRequestStatus,
    CJavaCertData& aCertDataObj)
{

    TBool flag = SendDeleteCommsMsg(aCertDataObj.mId,aRequestStatus);
    if (!flag)
    {
        //SendDeleteCommsMsg() operation calls RequestComplete()
        //operation in the error situation.
        return;
    }
    aCertDataObj.mDeleted = ETrue;

    User::RequestComplete(aRequestStatus,KErrNone);
}

/////////////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION OF CJavaCertData CLASS
//
//////////////////////////////////////////////////////////////////////////////////

CJavaCertData::CJavaCertData(const std::string& aCertContent,
                             long long aCertSize,
                             const std::string& aId,TInt aEncodedState)
        : mCertContent(aCertContent),mId(aId),mDeleted(EFalse),mSizeOfCert(aCertSize)
{
    mIsDisabled = ((aEncodedState & 0x1) == 0x1); // trust root is disabled if first bit is set
    mCanDisable = ((aEncodedState & 0x2) == 0x2);   // trust root can be disabled if second bit is set
    mCanDelete = ((aEncodedState & 0x4) == 0x4);    // trust root can be deleted if third bit is set
}

CJavaCertData::~CJavaCertData()
{
}

