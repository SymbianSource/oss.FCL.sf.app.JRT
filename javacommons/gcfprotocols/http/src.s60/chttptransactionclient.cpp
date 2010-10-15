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
* Description:  CHttpTransactionClient
*
*/


#include <http/rhttpresponse.h>
#include <http/rhttpheaders.h>
#include <httpstringconstants.h>
#include <http/thttphdrval.h>
#include <http/mhttpauthenticationcallback.h>
#include <httperr.h>
#include <stringpool.h>
#include <e32svr.h>
#include <x509cert.h>
#include <ssl.h>
#include "logger.h"
#include "chttptransactionclient.h"
#include "httpsessionclient.h"
#include "chttpscertinfo.h"
#include "mrefhttpclientobserver.h"


_LIT8(KSeperator , ";;");
_LIT8(KComma , ",");
_LIT8(KDot , ".");
_LIT8(KCommaSpace , ", ");
_LIT8(KSemiColonSpace , "; ");
_LIT8(KEquals , "=");
_LIT8(KDRMHeader1,"application/vnd.oma.drm.content");
_LIT8(KDRMHeader2,"application/vnd.oma.drm.message");

// A maximum length of the cipher suite description strings,
// see CipherSuiteDescription().
const TUint8 KMaxCipherSuiteDescriptionLength = 40;

const TInt KCookieBufferAllocLength = 4096;


CHttpTransactionClient::CHttpTransactionClient(HttpSessionClient& aSession, MRefHttpClientObserver* aObserver)
        :iHttpSession(aSession) , iObserver(aObserver), iReleasedData(ETrue),iClosing(EFalse)
{}


CHttpTransactionClient* CHttpTransactionClient::NewL(HttpSessionClient& aSession,
        MRefHttpClientObserver* aObserver,
        const TDesC* aUri,
        const TDesC* aRequestMethod)
{
    CHttpTransactionClient* self =  new(ELeave) CHttpTransactionClient(aSession,aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aUri, aRequestMethod);
    CleanupStack::Pop(self);
    return self;
}

void CHttpTransactionClient::ConstructL(const TDesC* aUri, const TDesC* aRequestMethod)
{
    //open the transaction
    LOG(ESOCKET,EInfo, "CHttpTransactionClient::ConstructL ");
    iFlag = 0;
    iDrmBuf = HBufC8::NewL(256);
    iEndOfRequest = false;
    OpenTransactionL(aUri , aRequestMethod);
}

void CHttpTransactionClient::OpenTransactionL(const TDesC* aUri, const TDesC* aRequestMethod)
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::OpenTransactionL + ");
    TUriParser8 uri;
    HBufC8* uriBuf= HBufC8::NewLC(aUri->Length());
    TPtr8 uriPtr = uriBuf->Des();
    uriPtr.Copy(*aUri);
    uri.Parse(*uriBuf);
    //Get Request Method String
    TBuf8<32> reqestType; //GET //POST // HEAD // Only options available in Midp 2.0 Http

    LOG(ESOCKET,EInfo,"Open TransactionL : Transaction Uri  ");
    reqestType.Copy(*aRequestMethod);
    LOG(ESOCKET,EInfo,"Open TransactionL : ReqestType ");

    RStringF requestMethoStr = iHttpSession.Session().StringPool().OpenFStringL(reqestType);
    CleanupClosePushL(requestMethoStr);
    iTransaction = iHttpSession.Session().OpenTransactionL(uri,*this,requestMethoStr);

    CleanupStack::PopAndDestroy(2);//requestMethoStr// uriBuf
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::OpenTransactionL - ");
}

void CHttpTransactionClient::SubmitL(RPointerArray<HBufC8>* aRawHeaders , TDesC8* aPostData, int aResponseTimeout, bool aPartialDataFlag)
{
    //ELOG(ESOCKET,"CHttpTransactionClient::SubmitL + ");
    //retrieve the headers
    iPartialPostData = aPartialDataFlag;
    TInt respTimeOut = aResponseTimeout;
    delete iBuf;
    iBuf=NULL;
    if (aPostData!=NULL && aPostData->Length()>0)
    {
        HBufC8* buf = aPostData->AllocL();
        iBuf=buf;
        LOG(ESOCKET,EInfo,"SubmitL : Posting Data ");
    }

    // Disable pipelining on this transaction
    RStringPool stringPool = iHttpSession.Session().StringPool();
    RStringF property = stringPool.StringF(HTTP::EHttpPipelining, RHTTPSession::GetTable());
    THTTPHdrVal disablePipelining = stringPool.StringF(HTTP::EDisablePipelining, RHTTPSession::GetTable());
    iTransaction.PropertySet().SetPropertyL(property, disablePipelining);

    if (respTimeOut != -1)
    {
        LOG1(ESOCKET,EInfo,"Setting response timeout to http stack with value = %d ",aResponseTimeout);
        property = stringPool.StringF(HTTP::EReceiveTimeOutValue, RHTTPSession::GetTable());
        THTTPHdrVal responseTimeout(respTimeOut);
        iTransaction.PropertySet().SetPropertyL(property, responseTimeout);
    }

    //Submit the transaction
    RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
    TInt headerCount = aRawHeaders->Count();
    for (TInt ii=0; ii < headerCount; ++ii)
    {
        SetHeaderL(hdr, (*aRawHeaders)[ii]);
    }

    //iHttpSession.CustomiseHeadersL(hdr);

    //Set Data
    if (iBuf && iBuf->Length() > KErrNone)
        iTransaction.Request().SetBody(*this);
    /* The data will be requested from MHttpDataSupplier*/
    iJavaWaitingOnCallBack=ETrue;
    iJavaWaitingOnReadCallBack=ETrue; // Assume somebody opened an input Stream
    LOG1(ESOCKET,EInfo,"CHttpTransactionClient::SubmitL :iTransaction.Id %d ",iTransaction.Id());
    TRAP_IGNORE(iTransaction.SubmitL());
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::SubmitL - ");
}


void CHttpTransactionClient::SetHeaderL(RHTTPHeaders& aHeaders, const TDesC8* aRawField)
{
    /*Raw Headers are passed in from java of the format
    * ValueKey ;; property
    * HTTP requires all request properties which can legally have
    * multiple instances with the same key to use a comma-separated list
    * syntax which enables multiple properties to be appended into a
    * single property.
    */
    //Find the value field
    //Seperated from values in Java by KSeperator
    TInt valueIndex = aRawField->Find(KSeperator);
    User::LeaveIfError(valueIndex);
    TPtrC8 value =aRawField->Left(valueIndex);
    RStringF valueF = iHttpSession.Session().StringPool().OpenFStringL(value);
    CleanupClosePushL(valueF);
    TPtrC8 propertyRaw= aRawField->Mid(valueIndex+2);
    LOG(ESOCKET,EInfo,"CHttpTransactionClient Add header value ");
    aHeaders.SetRawFieldL(valueF, propertyRaw , KComma);
    CleanupStack::PopAndDestroy();//valueF
}

void CHttpTransactionClient::GetResponseL(RPointerArray<HBufC8>* aRawHeaders)
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::GetResponseL(RPointerArray<HBufC8>* aRawHeaders)");
    RStringPool strP = iTransaction.Session().StringPool();
    //First element in response array is the status line of the form
    //HTTP-Version (major.minor) ;; Status-Code ;; Reason-Phrase
    LOG1(ESOCKET,EInfo,"CHttpTransactionClient::GetResponseL :iTransaction.Id %d ",iTransaction.Id());
    RHTTPResponse response = iTransaction.Response();

    TInt statusLineLength=0;

    //HTTP-Version
    TVersion serverVersion = response.Version();
    TBuf<16> major;
    major.Num(serverVersion.iMajor);
    TBuf<16> minor;
    minor.Num(serverVersion.iMinor);
    statusLineLength+=major.Length();
    statusLineLength+=1;//KDot
    statusLineLength+=minor.Length();

    //Status code
    TBuf<16> stausCode;
    TInt serverStatusCode = response.StatusCode();
    stausCode.Num(serverStatusCode);
    statusLineLength+=stausCode.Length();

    //Status Text
    RStringF serverResponseTextF = response.StatusText();
    statusLineLength+= serverResponseTextF.DesC().Length();

    //Sepearators KSeperator *2
    statusLineLength+=4;//

    HBufC8* responseText = HBufC8::NewLC(statusLineLength);
    //Create Status Line
    //Version
    responseText->Des().Append(major);
    responseText->Des().Append(KDot);
    responseText->Des().Append(minor);
    //Status Code
    responseText->Des().Append(KSeperator);
    responseText->Des().Append(stausCode);
    //Status Text
    responseText->Des().Append(KSeperator);
    responseText->Des().Append(serverResponseTextF.DesC());
    aRawHeaders->Append(responseText);
    CleanupStack::Pop(responseText);

    //Package up all the transaction headers in to an array of java strings
    RHTTPHeaders headers =response.GetHeaderCollection();
    THTTPHdrFieldIter it = headers.Fields();
    RStringTokenF token;
    RStringF fieldNameStr;
    TPtrC8 rawFldPtr;
    HBufC8* rawHeader = NULL; // field name  and values
    TInt rawFieldLength=0;
    do
    {
        token = it();
        fieldNameStr=strP.StringF(token);
        //Get the field data assume we get comma seperated values and
        //don't have to use aPartIdx

        RStringF setCookie = strP.StringF(HTTP::ESetCookie,RHTTPSession::GetTable());
        if (fieldNameStr == setCookie)
        {
            // Buffer for storing a single cookie
            HBufC8* cookieBuf = HBufC8::NewLC(KCookieBufferAllocLength);

            cookieBuf->Des().Append(fieldNameStr.DesC());
            cookieBuf->Des().Append(KSeperator);
            iMinLength = cookieBuf->Des().Length();

            TInt totalCookiesLength = 0;
            TInt cookieCount = headers.FieldPartsL(setCookie);

            if (cookieCount != 0)
            {
                for (TInt ii = 0; ii < cookieCount; ii++)
                {
                    iDiscardCookie = false;
                    StoreOneCookieL(cookieBuf, headers, ii, setCookie);
                    if (ii < cookieCount-1)
                    {
                        // Replace last semicolon with comma
                        if (iDiscardCookie!=true)
                        {
                            cookieBuf->Des().SetLength(cookieBuf->Des().Length() - KSemiColonSpace().Length());
                            cookieBuf->Des().Append(KCommaSpace);
                        }
                        else
                        {
                            cookieBuf->Des().Append(KCommaSpace);
                        }
                    }

                    if (ii > 0)
                    {
                        totalCookiesLength += cookieBuf->Des().Length();
                        rawHeader = rawHeader->ReAllocL(totalCookiesLength);
                        rawHeader->Des().Append(*cookieBuf);
                    }
                    else
                    {
                        rawHeader = cookieBuf->AllocL();
                        totalCookiesLength = rawHeader->Length();
                    }

                    cookieBuf->Des().Zero();
                }
            }
            else
            {
                LOG(ESOCKET,EInfo,"CHttpTransactionClient::GetResponseL 'Set-Cookie:' header field is empty");
                rawHeader = cookieBuf->AllocL();
            }

            CleanupStack::PopAndDestroy(cookieBuf);
            aRawHeaders->Append(rawHeader);
        }
        else
        {
            TInt err = headers.GetRawField(fieldNameStr,rawFldPtr);

            if (err==KErrNone)
            {
                rawFieldLength += fieldNameStr.DesC().Length();
                rawFieldLength += 2;//KSeperator Length;
                rawFieldLength += rawFldPtr.Length();
                rawHeader = HBufC8::NewL(rawFieldLength);
                rawHeader->Des().Append(fieldNameStr.DesC());
                rawHeader->Des().Append(KSeperator);
                rawHeader->Des().Append(rawFldPtr);
                aRawHeaders->Append(rawHeader);
                rawFieldLength=0;
            }
        }
        ++it;
    }
    while (!it.AtEnd());
}


/*
* Returns the total number of bytes read into the buffer,
* or -1 if there is no more data because the end of the stream
* has been reached.
*/
TInt CHttpTransactionClient::ReadBytes(TDes8& aBytesPtr)
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient:: ReadBytes");
    TInt maxBufLength = aBytesPtr.MaxLength();
    TInt bodyDataPartLength = 0;
    if (iDrmDownload == false)
    {
        bodyDataPartLength = iBuf->Length();
    }
    else
    {
        bodyDataPartLength = iDrmBuf->Length();
    }
    LOG1(ESOCKET,EInfo,"CHttpTransactionClient::ReadBytes: maxRead: %d," , maxBufLength);
    LOG1(ESOCKET,EInfo,"CHttpTransactionClient::ReadBytes: bodyDataPartLength:: %d " ,bodyDataPartLength);
    TInt bytesRead = (maxBufLength+iJavaReadOffset > bodyDataPartLength) ? (bodyDataPartLength - iJavaReadOffset) : maxBufLength;
    if (bytesRead>0)
    {
        LOG1(ESOCKET,EInfo,"Bytes read : %d " ,bytesRead);

        if (iDrmDownload == false)
        {
            TPtrC8 bufPtr = iBuf->Mid(iJavaReadOffset,bytesRead);
            aBytesPtr.Copy(bufPtr);
        }
        else
        {
            TPtrC8 bufPtr = iDrmBuf->Mid(iJavaReadOffset,bytesRead);
            aBytesPtr.Copy(bufPtr);
        }

        LOG1(ESOCKET,EInfo,"CHttpTransactionClient::ReadBytes: iJavaReadOffset:: %d " ,iJavaReadOffset);
        if (iDrmDownload == false)
        {
            if ((iJavaReadOffset+bytesRead) == bodyDataPartLength && iLastChunk)
            {
                //All body data from the http transaction has been read
                LOG(ESOCKET,EInfo,"CHttpTransactionClient::ReadBytes: EOF iLastChunk");
                iRead = ETrue;

                iJavaWaitingOnReadCallBack=EFalse;
                TRAP_IGNORE(iObserver->DataReadyForRead(-1));
            }
        }
        else
        {
            if ((iJavaReadOffset+bytesRead) == iDrmBuf->Length() && iLastChunk)
            {
                //All DRM body data from the http transaction has been read
                LOG(ESOCKET,EInfo,"CHttpTransactionClient::ReadBytes: EOF iLastChunk");
                iRead = ETrue;

                iJavaWaitingOnReadCallBack=EFalse;
                TRAP_IGNORE(iObserver->DataReadyForRead(-1));
            }
        }
    } // if(bytesread > 0)
    if ((iJavaReadOffset+bytesRead)<=bodyDataPartLength)
    {

        if (iJavaReadOffset+bytesRead < bodyDataPartLength)
        {
            //Java hasn't read all the current body data buffer, inform java
            //amount of bytes left to read in buffer.

            iJavaReadOffset+=bytesRead;
            TInt  bytesLeftInBuffer = bodyDataPartLength-iJavaReadOffset;
            LOG1(ESOCKET,EInfo," CHttpTransactionClient::ReadBytes: bytesLeftInBuffer: %d",bytesLeftInBuffer);
            //Unblock java so it can read the remaining buffered data
            iJavaWaitingOnReadCallBack=ETrue;
            TRAP_IGNORE(iObserver->DataReadyForRead(bytesLeftInBuffer));
        }
        else
        {
            // in this call, the iBuf data was completely read, so call release data to get the next chunk(part) of body from
            // the native stack

            if (iDrmDownload == false)
            {
                // Non-Drm case, offset is reset to 0, starting reading newly from iBuf from the next call
                iJavaReadOffset=0;
                LOG(ESOCKET,EInfo,"CHttpTransactionClient:: ReadBytes ReleaseData!");
                iReleasedData=ETrue;
                if (iBuf)
                {
                    delete iBuf;
                    iBuf = NULL;
                }
                iRespBody->ReleaseData();   // tell the stack to send the next response body data, stack will call mhfrunl after this
            }
            else
            {
                // DRM file download, index is never reset to 0 , continue reading from drm buffer
                iJavaReadOffset+=bytesRead;

            }

        }

    }

    LOG1(ESOCKET,EInfo,"CHttpTransactionClient::ReadBytes: return bytesRead %d " , bytesRead);
    return bytesRead;
}



void CHttpTransactionClient::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::MHFRunL +");

    if (iTransaction== aTransaction)
    {
        LOG1(ESOCKET,EInfo,"before switch aEvent.iStatus = %d ",aEvent.iStatus);
        switch (aEvent.iStatus)
        {
        case THTTPEvent::ESubmit:
            LOG1(ESOCKET,EInfo,"MHFRunL::ESubmit : aEvent.iStatus = %d",aEvent.iStatus);
            iStatus= CHttpTransactionClient::ESubmitted;
            //Only send an event to Java when we've got a response
            break;
        case THTTPEvent::ECancel:
            LOG(ESOCKET,EInfo,"MHFRunL::ECancel");
            iStatus =CHttpTransactionClient:: ECancelled;
            if (iJavaWaitingOnCallBack)
            {
                iObserver->SubmitComplete(iStatus);
                LOG(ESOCKET,EInfo,"CHttpTransactionClient::MHFRunL ECancel ");
                iJavaWaitingOnCallBack=EFalse;
            }

            if (iJavaWaitingOnReadCallBack)
            {
                LOG(ESOCKET,EInfo,"Notifiy Read EOF / Cancel");
                iJavaWaitingOnCallBack=EFalse;
                iObserver->DataReadyForRead(-1);
            }

            break;
        case THTTPEvent::EClosed:
            LOG(ESOCKET,EInfo,"MHFRunL::EClosed");
            iStatus = CHttpTransactionClient::EClosed;
            if (iJavaWaitingOnCallBack)
            {
                iJavaWaitingOnCallBack=EFalse;
                LOG(ESOCKET,EInfo,"CHttpTransactionClient::MHFRunL EClosed");
                iObserver->SubmitComplete(iStatus);
            }
            if (iJavaWaitingOnReadCallBack)
            {
                LOG(ESOCKET,EInfo,"Notifiy Read EOF / Cancel");
                iJavaWaitingOnCallBack=EFalse;
                iObserver->DataReadyForRead(-1);
            }

            break;
            /*Receiving transaction events*/
        case THTTPEvent::EGotResponseTrailerHeaders:
        {
            LOG(ESOCKET,EInfo,"MHFRunL  EGotResponseTrailerHeaders");
            iStatus= CHttpTransactionClient::EGotResponse;
            break;
        }
        case THTTPEvent::ESucceeded:
        {
            LOG(ESOCKET,EInfo,"MHFRunL ESucceeded");
            break;
        }
        case THTTPEvent::EGotResponseHeaders:
        {
            LOG(ESOCKET,EInfo,"MHFRunL EGotResponseHeaders");
            iStatus = CHttpTransactionClient::EGotResponse;
            //If  we are secure attempt to get the security information.
            //At the moment the native HTTP stack only allows us to retrieve
            //Server cert info after and a submission and before the transaction is
            //complete, ignore any warnings as we may not be in sercure mode

            RStringPool strP = iTransaction.Session().StringPool();
            RHTTPResponse response = iTransaction.Response();
            RHTTPHeaders headers =response.GetHeaderCollection();
            THTTPHdrFieldIter it = headers.Fields();
            RStringTokenF token;
            RStringF fieldNameStr;
            TPtrC8 rawFldPtr;
            do
            {
                token = it();
                fieldNameStr=strP.StringF(token);
                // Work around for DRM download
                RStringF contentType = strP.StringF(HTTP::EContentType,RHTTPSession::GetTable());
                if (fieldNameStr == contentType)
                {
                    //LOG(ESOCKET,EInfo,"content type header is  present");
                    TInt err = headers.GetRawField(fieldNameStr,rawFldPtr);
                    TInt index = rawFldPtr.Match(KDRMHeader1);
                    //  LOG1(ESOCKET,EInfo,"rawFldPtr = %s ",rawFldPtr.str());
                    //LOG1(ESOCKET,EInfo,"index of drm header is = %d",index);
                    if (index == 0)
                    {
                        iDrmDownload = true;
                    }
                    else
                    {
                        index = rawFldPtr.Match(KDRMHeader2);
                        if (index == 0)
                            iDrmDownload = true;
                    }
                }
                ++it;
            }
            while (!it.AtEnd());

            GetSecurityInfo();
            if (iJavaWaitingOnCallBack)
            {
                iJavaWaitingOnCallBack=EFalse;
                //LOG(ESOCKET,EInfo,"CHttpTransactionClient::MHFRunL Notifiy Java Got Response");
                iObserver->SubmitComplete(iStatus);
            }
            break;
        }
        case THTTPEvent::EResponseComplete:
        {
            LOG(ESOCKET,EInfo,"MHFRunL EResponseComplete");
            if (iJavaWaitingOnReadCallBack)
            {
                LOG(ESOCKET,EInfo,"Notifiy Read EOF / Cancel");
                iJavaWaitingOnCallBack=EFalse;
                iStatus=CHttpTransactionClient::EComplete;
                if (iDrmDownload == false)
                    iObserver->DataReadyForRead(-1);
            }

            break;
        }
        case THTTPEvent::EUnrecoverableError:
        case THTTPEvent::ETooMuchRequestData:
        case THTTPEvent::EFailed:
        {
            LOG(ESOCKET,EInfo, "MHFRunL EUnrecoverableError/EFailed");
            iStatus=CHttpTransactionClient::EFailed;
            NotifyErrorL(iStatus);
            break;
        }
        case THTTPEvent::EGotResponseBodyData:
        {
            LOG(ESOCKET,EInfo,"MHFRunL EGotResponseBodyData");
            iReleasedData=EFalse;
            //More data is ready for Java to read.
            iRespBody = aTransaction.Response().Body();

            TPtrC8 bodyData;
            iStatus=KErrNotReady;
            //We may be closing if so we don't want to request more data.
            if (!iClosing)
            {
                if (iDrmDownload == true)
                {
                    iFlag = iFlag + 1;
                    iLastChunk = iRespBody->GetNextDataPart(bodyData);
                    TInt curLen = iDrmBuf->Length();
                    TInt reqLen = curLen + bodyData.Length();
                    LOG1(ESOCKET,EInfo," CHttpTransactionClient::MHFRunL DRM case  bodyData.Length() = %d",bodyData.Length());
                    TRAP_IGNORE((iDrmBuf = iDrmBuf->ReAllocL(reqLen + bodyData.Length())));
                    TPtr8 bodyPtr1 = iDrmBuf->Des();
                    //HBufC8* bodyBuffer1 = HBufC8::NewLC(bodyData.Length());
                    LOG1(ESOCKET,EInfo,"length of iDrmBuf = %d", iDrmBuf->Length());
                    bodyPtr1.Append(bodyData);
                    iRespBody->ReleaseData();


                    iStatus = CHttpTransactionClient::EGotResponseBodyData;
                    //if(iFlag == 1)
                    TInt bodyDataPartLength;
                    bodyDataPartLength = iDrmBuf->Length();
                    TInt  bytesLeftInBuffer = bodyDataPartLength-iJavaReadOffset;
                    if (bytesLeftInBuffer > 0)
                        iObserver->DataReadyForRead(bytesLeftInBuffer);
                }
                else
                {
                    iLastChunk = iRespBody->GetNextDataPart(bodyData);
                    HBufC8* bodyBuffer = HBufC8::NewLC(bodyData.Length());
                    LOG1(ESOCKET,EInfo," CHttpTransactionClient::MHFRunL after getnextdatapart ,bodyData.Length() = %d",bodyData.Length());
                    TPtr8 bodyPtr = bodyBuffer->Des();
                    bodyPtr.Append(bodyData);
                    if (iBuf)
                    {
                        delete iBuf;
                        iBuf = NULL;
                    }
                    CleanupStack::Pop(bodyBuffer);
                    iBuf = bodyBuffer;
                    iStatus = CHttpTransactionClient::EGotResponseBodyData;
                    LOG(ESOCKET,EInfo," CHttpTransactionClient::MHFRunL Notify got more data");
                    iObserver->DataReadyForRead(iStatus);
                }
            }

            break;
        }
        case THTTPEvent::ENotifyNewRequestBodyPart:
        {
            LOG(ESOCKET,EInfo,"MHFRunL ENotifyNewRequestBodyPart");
            iStatus = CHttpTransactionClient::ERequestNextBodayData;
            break;
        }

        case THTTPEvent::EReceiveTimeOut:
        {
            ELOG(ESOCKET,"MHFRunL EReceiveTimeOut");
            NotifyErrorL(KErrTimedOut);  // send timeout error to java
            break;

        }
        /*
        * -j2me expects the http stack to be able to post body data with no content type
        *   the native stack default validation filter does not allow this.
        * -j2me expects the http stack to allow post requests to have body data
        *   the native stack default validation filter does not allow this. */
        case KErrHttpEntityHeaderMissingContentType:
        case KErrHttpInvalidHeaderInRequest:
        case KErrHttpGeneralHeaderMissingHost:
            break;

        case THTTPEvent::ERedirectRequiresConfirmation:
        {
            LOG(ESOCKET,EInfo,"MHFRunL ERedirectRequiresConfirmation");
        }
        case THTTPEvent::ENeedTunnel:
        {
            LOG(ESOCKET,EInfo,"MHFRunL ENeedTunnel");
        }
        case THTTPEvent::EGetCipherSuite:
        {
            LOG(ESOCKET,EInfo,"MHFRunL EGetCipherSuite");
        }
        default:
        {
            LOG(ESOCKET,EInfo,"MHFRunL Unkown Event");
            //__ASSERT_DEBUGCOMP( EFalse, User::Panic(_L("Unhandled native Http event"), aEvent.iStatus) );
            NotifyErrorL(aEvent.iStatus);
            break;
        }
        };
    }
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::MHFRunL -");
}

TInt CHttpTransactionClient::MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& /*aEvent*/)
{
    LOG1(ESOCKET,EInfo," CHttpTransactionClient::MHFRunError: %d ",aError);
    if (aError!=KErrNone && iTransaction== aTransaction)
    {
        iStatus=CHttpTransactionClient::EFailed;
        if (iJavaWaitingOnCallBack)
            iObserver->SubmitComplete(iStatus);


        if (iJavaWaitingOnReadCallBack)
        {
            LOG(ESOCKET,EInfo,"Notifiy Read EOF / Cancel");
            iJavaWaitingOnCallBack=EFalse;
            iObserver->DataReadyForRead(-1);
        }

    }
    return aError;
}

/*
* Java may be waiting on an async call back
* If this is the case notify of the error code so we don't block java.
*/

void CHttpTransactionClient::NotifyErrorL(TInt aErrorCode)
{
    ELOG1(ESOCKET," CHttpTransactionClient::NotifyErrorL: %d ",aErrorCode);
    if ((aErrorCode  ==  KErrNotReady))
    {
        iTransaction.Cancel();
        iHttpSession.RestartConnection();

    }
    if (iJavaWaitingOnCallBack)
    {
        iJavaWaitingOnCallBack=EFalse;
        LOG(ESOCKET,EInfo,"CHttpTransactionClient::MHFRunL  Notifiy Java unknown Event");
        iObserver->SubmitComplete(aErrorCode);
    }
    if (iJavaWaitingOnReadCallBack)
    {
        LOG(ESOCKET,EInfo,"Notifiy Read EOF / Cancel");
        iJavaWaitingOnCallBack=EFalse;
        //Notify Java of error if it is waiting for a read
        iObserver->DataReadyForRead(aErrorCode);
    }

}


void CHttpTransactionClient::CloseTransaction()
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::CloseTransaction + ");
    iClosing=ETrue;
    if (!iReleasedData)
    {
        LOG(ESOCKET,EInfo,"CHttpTransactionClient::CloseTransaction NOT RELEASED DATA!!! release and close");
        if (iDrmDownload == false)
            iRespBody->ReleaseData();
        iReleasedData=ETrue;
    }
    iTransaction.Close();
    iClosing=EFalse;
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::CloseTransaction - ");
}

CHttpTransactionClient::~CHttpTransactionClient()
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::~CHttpTransactionClient");
    //CloseTransaction();
    if (iBuf)
    {
        delete iBuf;
        iBuf=NULL;
    }
    if (iDrmBuf)
    {
        delete iDrmBuf;
        iDrmBuf=NULL;
    }
    delete iCertInfo;
}


/** Obtain a data part from the supplier.  The data is guaranteed
    to survive until a call is made to ReleaseData().
    @param aDataPart - the data part
    @return ETrue if this is the last part. EFalse otherwise */
TBool CHttpTransactionClient::GetNextDataPart(TPtrC8& aDataPart)
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::GetNextDataPart");
    TBool lastPart=ETrue;

    if (iBuf!=NULL)
    {
        // in-case of non-chunked, this is the end of POST data
        aDataPart.Set(iBuf->Des());
    }
    if (iPartialPostData)
    {
        lastPart = iEndOfRequest;
    }
    LOG1(ESOCKET,EInfo,"CHttpTransactionClient::GetNextDataPart, returning : %d", lastPart);
    return lastPart;
}

/** Release the current data part being held at the data
    supplier.  This call indicates to the supplier that the part
    is no longer needed, and another one can be supplied, if
    appropriate.  */
void CHttpTransactionClient::ReleaseData()
{
    LOG(ESOCKET,EInfo," CHttpTransactionClient::ReleaseData: ");
    if (iBuf)
    {
        delete iBuf;
        iBuf=NULL;
    }
    if (iPartialPostData)
    {
        // inform the java side that post data is consumed
        TRAP_IGNORE(iObserver->DoPostCallBack());
    }

}

/** Obtain the overall size of the data being supplied, if known
    to the supplier.  Where a body of data is supplied in several
    parts this size will be the sum of all the part sizes. If
    the size is not known, KErrNotFound is returned; in this case
    the client must use the return code of GetNextDataPart to find
    out when the data is complete.

    @return A size in bytes, or KErrNotFound if the size is not known.  */
TInt CHttpTransactionClient::OverallDataSize()
{
    TInt overallSize = KErrNotFound;
    if (iPartialPostData == true)
    {
        overallSize = KErrNotFound;
    }
    else if (iBuf)
    {
        overallSize=iBuf->Length();
    }
    LOG1(ESOCKET,EInfo," CHttpTransactionClient::OverallDataSize: %d",overallSize);
    return overallSize;
}

void CHttpTransactionClient::PostDataL(HBufC8* aPostData, const bool aEndOfRequest)
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::PostData ");
    iEndOfRequest = aEndOfRequest;
    if (aPostData!=NULL && aPostData->Length()>0)
    {
        iBuf=aPostData;
    }
    else
    {
        HBufC8* buf = HBufC8::NewL(1);      // descriptor of length 0
        iBuf = buf;
    }
    iTransaction.NotifyNewRequestBodyPartL();  // notify the stack that new request body data is available


}

/** Reset the data supplier.  This indicates to the data supplier that it should
    return to the first part of the data.  This could be used in a situation where
    the data consumer has encountered an error and needs the data to be supplied
    afresh.  Even if the last part has been supplied (i.e. GetNextDataPart has
    returned ETrue), the data supplier should reset to the first part.

    If the supplier cannot reset it should return an error code; otherwise it should
    return KErrNone, where the reset will be assumed to have succeeded*/
TInt CHttpTransactionClient::Reset()
{
    LOG(ESOCKET,EInfo,"CHttpTransactionClient::Reset");
    ELOG1(ESOCKET, "Reset() , Tran = %d", iTransaction.Id());
    TInt err = 0;
    if(iPartialPostData == false)
    {
        err = KErrNone; 
    }
    else
    {
        err = KErrNotSupported;
    }
    return err;
}


MNativeSecureConnectionInformation* CHttpTransactionClient::GetSecurityInfo()
{
    //LOG(ESOCKET,EInfo,"CHttpTransactionClient::GetSecurityInfo");
    MNativeSecureConnectionInformation* handle=NULL;
    if (!iCertInfo)
    {
        TRAP_IGNORE(GetCertInfoL());
        //If we fail to read the cert java is returned a null handle.
    }
    if (iCertInfo)
    {
        handle = iCertInfo;
    }
    return handle;
}

void CHttpTransactionClient::GetCertInfoL()
{
    //LOG(ESOCKET,EInfo,"CHttpTransactionClient::GetCertInfoL ");
    TCertInfo info;

    //Get the server certificate
    if (iCertInfo!=NULL)
    {
        delete iCertInfo;
        iCertInfo= NULL;
    }
    const CX509Certificate* cert = static_cast<const CX509Certificate*>(iTransaction.ServerCert());

    TInt err = iTransaction.ServerCert(info);
    if (err==KErrNone)
    {
        //Get the cipher suite
        const TDesC8& cipherCode = iTransaction.CipherSuite().DesC();
        if (!cipherCode.Length())
        {
            //LOG(ESOCKET,EInfo, "CHttpTransactionClient::GetCertInfoL No Cipher Suite Name found in Transaction");
            User::Leave(KErrNotFound);
        }

        HBufC* cipherSuiteBuf = HBufC::NewLC(KMaxCipherSuiteDescriptionLength);

        const TDesC8& cipherDesc = CipherSuiteDescription(cipherCode);
        if (cipherDesc.Length())
        {
            cipherSuiteBuf->Des().Copy(cipherDesc);
        }
        else
        {
            // We will use the code as no description is available.
            //LOG(ESOCKET,EInfo, "CHttpTransactionClient::GetCertInfoL No Cipher Suite description found");
            cipherSuiteBuf->Des().Copy(cipherCode);
        }

        CleanupStack::Pop(cipherSuiteBuf);
        iCertInfo= CHttpsCertInfo::NewL(*cert,cipherSuiteBuf);
    }
}

TInt CHttpTransactionClient::Available()
{
    TInt avail=KErrNone;
    if (iBuf)
    {
        avail=(iBuf->Length()-iJavaReadOffset);
    }
    LOG1(ESOCKET,EInfo," CHttpTransactionClient::Available: %d ",avail);
    return avail;
}

void CHttpTransactionClient::SetHTTPAuthenticationCallbackL(MHTTPAuthenticationCallback& aCallBack)
{
    aCallBack.InstallAuthenticationL(iHttpSession.Session());
}


// NOTE : When adding a new cipher description string,
// make sure KMaxCipherSuiteDescriptionLength is greater than the length.
const TDesC8& CHttpTransactionClient::CipherSuiteDescription(const TDesC8& aCode)
{
    TInt length = aCode.Length();
    if (length < 2)
    {
        // This is not a TLS code we recognise.
        //LOG(ESOCKET,EInfo,"CHttpTransactionClient::CipherSuiteDescription, unknown cipher suite");
        return KNullDesC8;
    }

    TInt code = aCode[1];

    //LOG1(ESOCKET,EInfo,"CHttpTransactionClient::CipherSuiteDescription, code=%d", code);

    switch (code)
    {
// The Cipher Suite codes and matching string descriptions are taken from the TLS specification
// (see A.5. The CipherSuite, The TLS Protocol, RFC 2246)
// The list below contains those currently supported by Symbian. See tlstypedef.h

    case 0x03:
        _LIT8(KTls_03, "TLS_RSA_EXPORT_WITH_RC4_40_MD5");
        return KTls_03();
    case 0x04:
        _LIT8(KTls_04, "TLS_RSA_WITH_RC4_128_MD5");
        return KTls_04();
    case 0x05:
        _LIT8(KTls_05, "TLS_RSA_WITH_RC4_128_SHA");
        return KTls_05();
    case 0x08:
        _LIT8(KTls_08, "TLS_RSA_EXPORT_WITH_DES40_CBC_SHA");
        return KTls_08();
    case 0x09:
        _LIT8(KTls_09, "TLS_RSA_WITH_DES_CBC_SHA");
        return KTls_09();
    case 0x0A:
        _LIT8(KTls_0A, "TLS_RSA_WITH_3DES_EDE_CBC_SHA");
        return KTls_0A();
    case 0x11:
        _LIT8(KTls_11, "TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA");
        return KTls_11();
    case 0x12:
        _LIT8(KTls_12, "TLS_DHE_DSS_WITH_DES_CBC_SHA");
        return KTls_12();
    case 0x13:
        _LIT8(KTls_13, "TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA");
        return KTls_13();
    case 0x14:
        _LIT8(KTls_14, "TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA");
        return KTls_14();
    case 0x16:
        _LIT8(KTls_16, "TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA");
        return KTls_16();
    case 0x19:
        _LIT8(KTls_19, "TLS_DH_anon_EXPORT_WITH_DES40_CBC_SHA");
        return KTls_19();

// from RFC 3268 : Advanced Encryption Standard (AES) Ciphersuites for Transport Layer Security (TLS)
    case 0x2F:
        _LIT8(KTls_2F, "TLS_RSA_WITH_AES_128_CBC_SHA");
        return KTls_2F();
    case 0x35:
        _LIT8(KTls_35, "TLS_RSA_WITH_AES_256_CBC_SHA");
        return KTls_35();
    default:
        return KNullDesC8;
    }   // end switch
}

void CHttpTransactionClient::StoreOneCookieL(HBufC8*& aRawHeader, RHTTPHeaders aHeaders, TInt aPartIndex, RStringF aFieldName)
{
    THTTPHdrVal name, value;
    RStringPool strP = iTransaction.Session().StringPool();

    RStringF cookieName = strP.StringF(HTTP::ECookieName, RHTTPSession::GetTable());
    RStringF cookieValue= strP.StringF(HTTP::ECookieValue, RHTTPSession::GetTable());
    RStringF cookiePath= strP.StringF(HTTP::EPath, RHTTPSession::GetTable());
    RStringF cookieDomain= strP.StringF(HTTP::EDomain, RHTTPSession::GetTable());
    RStringF cookieMaxAge= strP.StringF(HTTP::EMaxAge, RHTTPSession::GetTable());
    RStringF cookiePort= strP.StringF(HTTP::ECookiePort, RHTTPSession::GetTable());
    RStringF cookieComment= strP.StringF(HTTP::EComment, RHTTPSession::GetTable());
    RStringF cookieCommentURL= strP.StringF(HTTP::ECommentURL, RHTTPSession::GetTable());
    RStringF cookieSecure= strP.StringF(HTTP::ESecure, RHTTPSession::GetTable());
    RStringF cookieDiscard= strP.StringF(HTTP::EDiscard, RHTTPSession::GetTable());
    RStringF cookieVersion= strP.StringF(HTTP::EVersion, RHTTPSession::GetTable());
    RStringF cookieExpires= strP.StringF(HTTP::EExpires, RHTTPSession::GetTable());

    aHeaders.GetParam(aFieldName, cookieName , name, aPartIndex);
    aHeaders.GetParam(aFieldName, cookieValue, value, aPartIndex);

    TInt len1 = name.Str().DesC().Length();
    TInt len2 = value.Str().DesC().Length();
    HBufC8* temp = HBufC8::NewLC(KCookieBufferAllocLength);
    if (((temp->Des().Length()+1+len1+len2)) <(KCookieBufferAllocLength-iMinLength))
    {
        temp->Des().Append(name.Str().DesC());
        temp->Des().Append(KEquals);
        temp->Des().Append(value.Str().DesC());
        temp->Des().Append(KSemiColonSpace);
    }
    else
    {
        iDiscardCookie = true;
    }

    THTTPHdrVal hdrVal;

    if (aHeaders.GetParam(aFieldName, cookieComment, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookieComment.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookieCommentURL, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookieCommentURL.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookieDiscard, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookieDiscard.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookieDomain, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookieDomain.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookieMaxAge, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookieMaxAge.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookiePath, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookiePath.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookiePort, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookiePort.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookieSecure, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookieSecure.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookieVersion, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookieVersion.DesC(), hdrVal.StrF().DesC());

    if (aHeaders.GetParam(aFieldName, cookieExpires, hdrVal, aPartIndex) == KErrNone)
        AddParam(temp, cookieExpires.DesC(), hdrVal.StrF().DesC());

// If the cookie size is more than 4K, the cookie is discarded
    if (iDiscardCookie != true)
        aRawHeader->Des().Append(*temp);

    CleanupStack::PopAndDestroy(temp);
}

void CHttpTransactionClient::AddParam(HBufC8*& aTemp, const TDesC8& aName, const TDesC8& aValue)
{
    TInt len = aName.Length()+aValue.Length();
    if ((aTemp->Des().Length()+len+1) < (KCookieBufferAllocLength - iMinLength))
    {
        aTemp->Des().Append(aName);
        aTemp->Des().Append(KEquals);
        aTemp->Des().Append(aValue);
        aTemp->Des().Append(KSemiColonSpace);
    }
    else
    {
        iDiscardCookie = true;
    }
}
