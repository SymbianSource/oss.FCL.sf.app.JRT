/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CHTTPTRANSACTIONCLIENT_H
#define CHTTPTRANSACTIONCLIENT_H

#include <http/rhttpsession.h>
#include <http/rhttptransaction.h>
#include <http/mhttptransactioncallback.h>
#include <http/mhttpdatasupplier.h>


class MNativeSecureConnectionInformation;
class CX509Certificate;
class HttpSessionClient;
class CHttpsCertInfo;
class MRefHttpClientObserver;
class MHTTPAuthenticationCallback;

class CHttpTransactionClient : public CBase , public MHTTPTransactionCallback , public MHTTPDataSupplier
{
public:
    static CHttpTransactionClient* NewL(HttpSessionClient & aSession,
                                        MRefHttpClientObserver* aObserver,
                                        const TDesC* aUri,
                                        const TDesC* aRequestMethod);
    /*
    * After the headers and other details have been set up,
    * call this function to make the request.
    * Takes ownership of aPostData
    */
    void SubmitL(RPointerArray<HBufC8>* aRawHeaders , TDesC8* aPostData, int aResponseTimeout, bool aPartialDataFlag);
    /*
    * Get the response headers back from the transaction
    */
    void GetResponseL(RPointerArray<HBufC8>* aRawHeaders);

    //RHTTPResponse GetResponseL();
    /*
    * Call from Jave to attempt to read aLength of Bytes
    * Returns the total number of bytes read into the buffer,
    * or -1 if there is no more data because the end of the stream
    * has been reached.
    * Returns 0 if native buffer is empty but still more data to read;
    */
    TInt ReadBytes(TDes8& aBytesPtr);
    /*
    *   CloseTransaction
    */
    void CloseTransaction();
    /*
    * Get the secutiry information
    */
    MNativeSecureConnectionInformation* GetSecurityInfo();
    /*
    * Close the transaction and dispose
    */
    ~CHttpTransactionClient();

    /*
    * Returns the amount of bytes avaible for read in our buffer.
    */
    TInt Available();

    /*
     * Pre-Condition
     *The session is opened, but no transactions have been created on it
     */
    void SetHTTPAuthenticationCallbackL(MHTTPAuthenticationCallback& aCallBack);

    void PostDataL(HBufC8* aPostData, const bool aEndOfRequest);

private:
    enum TTransactionState
    {
        EFailed                 =-1,
        ENone                   = 0,
        ESubmitted              = 1,
        EGotResponse            = 2,
        EComplete               = 3,
        ECancelled              = 4,
        EGotResponseBodyData    = 5,
        ERequestNextBodayData   = 6,
        EClosed                 = 7
    };
private://from MHTTPTransactionCallback
    // Called when the filter's registration conditions are satisfied for events that   occur on a transaction.
    void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
    //Called when RunL leaves from a transaction event
    TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
private: // from MHTTPDataSupplier
    //Obtain a data part from the supplier.
    virtual TBool GetNextDataPart(TPtrC8& aDataPart);
    // Release the current data part being held
    virtual void ReleaseData();
    //Obtain the overall size of the data being supplied
    virtual TInt OverallDataSize();
    ///Reset the data supplier.
    virtual TInt Reset();
private://from CJavaEventSource
    void FinalizeSvr();
private:
    CHttpTransactionClient(HttpSessionClient& aSession, MRefHttpClientObserver* aObserver);
    void ConstructL(const TDesC* aUri, const TDesC* aRequestMethod);
    void SetHeaderL(RHTTPHeaders& aHeaders, const TDesC8* aRawField);
    void NotifyErrorL(TInt aErrorCode);
    void GetCertInfoL();
    void OpenTransactionL(const TDesC* aUri, const TDesC* aRequestMethod);
    const TDesC8& CipherSuiteDescription(const TDesC8& aCode);
    void StoreOneCookieL(HBufC8*& aRawHeader, RHTTPHeaders aHeaders, TInt aPartIndex, RStringF aFieldName);
    void AddParam(HBufC8*& aRawHeader, const TDesC8& aName, const TDesC8& aValue);


private:
    HttpSessionClient& iHttpSession;
    RHTTPTransaction iTransaction;
    MHTTPDataSupplier* iRespBody;
    MRefHttpClientObserver* iObserver;
    CHttpsCertInfo* iCertInfo;
    HBufC8* iBuf;
    HBufC8* iDrmBuf;
    TInt iStatus;
    TInt iJavaReadOffset;
    TBool iLastChunk;
    TBool iJavaWaitingOnCallBack;
    TBool iJavaWaitingOnReadCallBack;
    TBool iReleasedData;
    TBool iTransactionOpen;
    TBool iRead;
    TBool iClosing;
    TBool iDiscardCookie;
    TInt iMinLength;
    TBool iDrmDownload;
    TInt iFlag;
    bool iPartialPostData;
    bool iEndOfRequest;

};

#endif // CHTTPTRANSACTIONCLIENT_H
