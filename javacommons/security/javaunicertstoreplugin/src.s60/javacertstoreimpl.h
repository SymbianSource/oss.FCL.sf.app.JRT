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

#ifndef JAVACERTSTOREIMPL_H
#define JAVACERTSTOREIMPL_H

#include <e32base.h>
#include <mctwritablecertstore.h>
#include "commslistener.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"

class CAknGlobalConfirmationQuery;

namespace java
{
namespace security
{

class CJavaCertStoreToken;

class CJavaCertData: public CBase
{
public:

    /**
     * @param aEncodedState First bit indicates whether cert is already disabled.
     *                      Second bit indicates whether cert can be disabled.
     *                      Third bit indicates whether cert can be deleted.
     */
    CJavaCertData(const std::string& aCertContent, const long long aCertSize,
                  const std::string& aId,TInt aEncodedState);

    virtual ~CJavaCertData();

    std::string mCertContent;
    std::string mId;
    //This flag is used to indicate that cert has been deleted from cert store.
    //I.e. Remove() operation has been called in this session.
    TBool mDeleted;
    //These three boolean values has been read from aEncodeState argument.
    TBool mIsDisabled;
    TBool mCanDisable;
    TBool mCanDelete;
    //This datamember tells size of the certificate.
    long long mSizeOfCert;

private:

    //Not implemented.
    CJavaCertData(const CJavaCertData &x);
    CJavaCertData &operator=(const CJavaCertData &x);

};

class CJavaCertStoreImpl: public CActive, public MCTWritableCertStore,
        public java::comms::CommsListener
{
public:

    static CJavaCertStoreImpl* NewL(CJavaCertStoreToken& aToken);

    virtual ~CJavaCertStoreImpl();

    virtual MCTToken& Token();

    //CommsListener
    virtual void processMessage(java::comms::CommsMessage& aMessage);

    //CActive
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

    //MCTWritableCertStore
    virtual void Add(const TDesC& aLabel, TCertificateFormat aFormat,
                     TCertificateOwnerType aCertificateOwnerType,
                     const TKeyIdentifier* aSubjectKeyId,
                     const TKeyIdentifier* aIssuerKeyId,
                     const TDesC8& aCert, TRequestStatus& aStatus);
    virtual void CancelAdd();
    virtual void Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus);
    virtual void CancelRemove();
    virtual void SetApplicability(const CCTCertInfo& aCertInfo,
                                  const RArray<TUid>& aApplications,
                                  TRequestStatus &aStatus);
    virtual void CancelSetApplicability();
    virtual void SetTrust(const CCTCertInfo& aCertInfo, TBool aTrusted,
                          TRequestStatus& aStatus);
    virtual void CancelSetTrust();
    virtual void List(RMPointerArray<CCTCertInfo>& aCerts,
                      const CCertAttributeFilter& aFilter, TRequestStatus& aStatus);
    virtual void CancelList();
    virtual void GetCert(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle,
                         TRequestStatus& aStatus);
    virtual void CancelGetCert();
    virtual void Applications(const CCTCertInfo& aCertInfo, RArray<TUid>& aApplications,
                              TRequestStatus& aStatus);
    virtual void CancelApplications();
    virtual void IsApplicable(const CCTCertInfo& aCertInfo, TUid aApplication,
                              TBool& aIsApplicable, TRequestStatus& aStatus);
    virtual void CancelIsApplicable();
    virtual void Trusted(const CCTCertInfo& aCertInfo, TBool& aTrusted,
                         TRequestStatus& aStatus);
    virtual void CancelTrusted();
    virtual void Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert,
                          TRequestStatus& aStatus);
    virtual void CancelRetrieve();

private:

    enum TState
    {
        EInitial,
        EListing,
        EPreDisabling,
        EPreEnabling,
        EPreDeleting
    };

    //Datamembers.
    CJavaCertStoreToken& mToken;
    java::comms::CommsClientEndpoint mComms;
    TRequestStatus* mClientStatus;
    //This class does not have ownership into this object.
    RMPointerArray<CCTCertInfo>* mClientList;
    java::comms::CommsMessage mReceivedCommsMsg;
    RPointerArray<CJavaCertData> mCertsData;
    TState mState;
    RThread mUiThread;
    std::auto_ptr<CAknGlobalConfirmationQuery> mQuery;
    //This class does not have ownership into this object.
    CJavaCertData* mTempCertData;

    //Internal operations.
    CJavaCertStoreImpl(CJavaCertStoreToken& aToken);
    void ConstructL();
    TBool validateCertAttrFilter(const CCertAttributeFilter& aFilter) const;
    void FillCertificateInfos(RMPointerArray<CCTCertInfo>& aInfoArray);
    void FillCertInfoFromCacheL(RMPointerArray<CCTCertInfo>& aInfoArray);
    TInt GetIndexToCertData(const TCTTokenObjectHandle& aCertInfo);
    TBool GetCertDataObj(const CCTCertInfo& aCertInfo,
                         TRequestStatus* aRequestStatus,
                         CJavaCertData*& aOutCertDataObj);
    TBool SendDisableEnableCommsMsg(const std::string& aId,
                                    TState aState,
                                    TRequestStatus* aRequestStatus);
    TBool SendDeleteCommsMsg(const std::string& aId,
                             TRequestStatus* aRequestStatus);
    TBool CheckCapability(const TCapability& aCapability,TRequestStatus* aRequestStatus);
    void HandleDeleteDisableQuery(TRequestStatus &aRequestStatus, bool disableCertQuery);
    void ShowQueryL(TInt resourceId);
    void SendDisableMsg(TInt aStatus);
    void SendDeleteMsg(TInt aStatus);
    void HandleSendingEnableDisableMsg(TRequestStatus* aRequestStatus,
                                       TState aState,CJavaCertData& aCertDataObj);
    void HandleSendingDeleteMsg(TRequestStatus* aRequestStatus,
                                CJavaCertData& aCertDataObj);

    //Not implemented.
    CJavaCertStoreImpl(const CJavaCertStoreImpl &x);
    CJavaCertStoreImpl &operator=(const CJavaCertStoreImpl &x);

};


} // end namespace security
} // end namespace java

#endif // JAVACERTSTOREIMPL_H
