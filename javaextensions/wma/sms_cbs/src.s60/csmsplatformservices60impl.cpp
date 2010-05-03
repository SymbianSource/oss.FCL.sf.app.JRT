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

#include <memory>
#include <errno.h>
#include <f32file.h>
#include <gsmumsg.h>
#include <smsustrm.h>
#include <charconv.h>

#include "logger.h"
#include "s60commonutils.h"
#include "javacommonutils.h"
#include "javasymbianoslayer.h"
#include "csmsplatformservices60impl.h"
#include "cservicecentrenumretrievers60impl.h"

namespace java
{
namespace wma
{
//Constructor
CSmsPlatformServiceS60Impl::CSmsPlatformServiceS60Impl(std::wstring aUri)
        :mUri(aUri)
{
    JELOG2(EWMA);
}

//Destructor
CSmsPlatformServiceS60Impl::~CSmsPlatformServiceS60Impl()
{
    JELOG2(EWMA);
    if (mUri.find(L"cbs://:") == std::wstring::npos)
    {
        mSmsSendSocket.CancelAll();
        mSmsSendSocket.Close();
        delete mSendMessage;
    }
    delete mMessageParameters;
}

SmsPlatformService* SmsPlatformService::getNewInstance(std::wstring aUri)
{
    JELOG2(EWMA);
    CSmsPlatformServiceS60Impl* msgConn = new CSmsPlatformServiceS60Impl(aUri);
    if (aUri.find(L"cbs://:") == std::wstring::npos)
    {
        msgConn->initialize();
    }
    else
    {
        // Its a Cbs Connection nothing to be initialized.
    }
    return msgConn;
}
/**
 * Initializing function
 */
void CSmsPlatformServiceS60Impl::initialize()
{
    JELOG2(EWMA);
    int err = KErrNone;
    err = mSocketServer.Connect();
    if (KErrNone == err)
    {
        err = mSocketServer.ShareAuto();
    }
    if (err == KErrNone)
    {
        err = mSmsSendSocket.Open(mSocketServer, KSMSAddrFamily,
                                  KSockDatagram, KSMSDatagramProtocol);
        if (err == KErrNone)
        {
            mSmsSendAddress.SetFamily(ESmsAddrApplication16BitPort);
            mSmsSendAddress.SetPort(mSendPort);
            err = mSmsSendSocket.Bind(mSmsSendAddress);
            if (err != KErrNone)
            {
                ELOG1(EWMA, "SMS : error while binding socket : %d", err);
                throw err;
            }
            else
            {
                // No error nothing to do
            }
        }
        else
        {
            ELOG1(EWMA, "SMS : error while opening socket : %d", err);
            throw err;
        }
    }
    else
    {
        ELOG1(EWMA, "SMS : error while connecting to socketserver : %d", err);
        throw err;
    }
}
/**
 * Converts the CString to Descriptor
 */
HBufC* CSmsPlatformServiceS60Impl::convertToDes(const char* aData,
        const int aMsgType,
        const int aLength)
{
    HBufC* smsData = 0;
    smsData = HBufC::New(aLength+1);
    if (aData != NULL)
    {
        if (aMsgType == MESSAGE_TEXT)
        {
            TPtr ptr = smsData->Des();
            TPtr16 ptr16((TUint16 *)aData,aLength);
            ptr16.SetLength(aLength);
            ptr.Copy(ptr16);
            ptr.ZeroTerminate();
        }
        else
        {

            TPtr ptr = smsData->Des();
            TPtr8 ptr8((TUint8 *)aData,aLength);
            ptr8.SetLength(aLength);
            ptr.Copy(ptr8);
            ptr.ZeroTerminate();
        }
    }
    return smsData;
}
/**
 * Calculates and returns numberofDataSegments of the Message.
 */
int CSmsPlatformServiceS60Impl::getSmsDataSegments(const int aMsgType,
        const int aLength,
        const int aHostPort,
        const char* aHostAddress,
        const char* aData)
{
    JELOG2(EWMA);
    int numberOfSegments =0;
    TBool PortAddressing = EFalse;
    TInt receiveIntoPort = 0;
    if (mUri.find(L"sms://:") != std::wstring::npos)
    {
        receiveIntoPort = java::util::JavaCommonUtils::wstringToInt(
                              mUri.substr(KPortFieldStartIndex));
    }
    else
    {
        // It is a client connection, Nothing to do
    }
    TRAPD(error,
    {
        HBufC* smsData = convertToDes(aData,aMsgType,aLength);
        std::auto_ptr<HBufC> smsAddress(stringToDes(aHostAddress));
        // The sms message takes ownership of the buffer
        CSmsBuffer* buffer = CSmsBuffer::NewL();
        delete mSendMessage;
        mSendMessage = 0;
        RFs fileServer;
        mSendMessage = CSmsMessage::NewL(fileServer,CSmsPDU::ESmsSubmit,buffer);
        if (smsAddress->Length()> 0)
        {
            // set the destination address
            mSendMessage->SetToFromAddressL((const TPtrC&)*smsAddress);
            // Set SMS port addresing
            if (aHostPort != 0)
            {
                PortAddressing = ETrue;
            }
            else
            {
                PortAddressing = EFalse;
            }
            (mSendMessage->SmsPDU()).SetApplicationPortAddressingL(
                PortAddressing,aHostPort,receiveIntoPort,KUse16BitPortAddressing);
        }
        else
        {
            // Nothing to do
        }

        // Get the encoding for the message that is to be sent
        TSmsDataCodingScheme::TSmsAlphabet smsEncoding = getEncodingSchemeL(
                    *smsData,aMsgType,fileServer);
        // sets the encoding

        (mSendMessage->SmsPDU()).SetAlphabet(smsEncoding);
        (mSendMessage->SmsPDU()).SetTextCompressed(EFalse);
        (mSendMessage->SmsPDU()).SetTextConcatenatedL(ETrue,ETrue);

        (mSendMessage->Buffer()).Reset();
        (mSendMessage->Buffer()).InsertL(0,*smsData);

        mSendMessage->OptimizeSettingsL(CSmsMessage::
                                        ESmsFlagOptimizeConcatenation16BitReference);
        // calculate the number of message PDUs required to send this message
        numberOfSegments = (int)mSendMessage->NumMessagePDUsL();
        LOG2(EWMA, EInfo,"SMS :number of segments %d data length %d",
             numberOfSegments, aLength);
        delete smsData;
    });
    return (error == KErrNone)? numberOfSegments : error;
}

/**
 * Gets the best Message encoding for the message.
 */
TSmsDataCodingScheme::TSmsAlphabet CSmsPlatformServiceS60Impl ::
getEncodingSchemeL(const TDesC& aSmsData,const TInt aMsgType,RFs& aFs)
{
    JELOG2(EWMA);
    TSmsDataCodingScheme::TSmsAlphabet msgEncoding;
    if (aMsgType == MESSAGE_TEXT)
    {
        TBool isGSM7Convertible = EFalse;
        TPtrC tempData(aSmsData);
        TBuf8<KCharacterConversionBufferSize> tempBuf;
        TInt charsToBeConverted = aSmsData.Length();
        TInt nonConvertibleCharacters;
        CCnvCharacterSetConverter* cConverter =
            CCnvCharacterSetConverter::NewL();
        CCnvCharacterSetConverter::TAvailability cnvAvailable =
            cConverter->PrepareToConvertToOrFromL(
                KCharacterSetIdentifierSms7Bit, aFs);

        if (cnvAvailable == CCnvCharacterSetConverter::EAvailable)
        {
            isGSM7Convertible = ETrue;
            while (charsToBeConverted> 0)
            {
                charsToBeConverted = cConverter->ConvertFromUnicode(
                                         tempBuf,tempData, nonConvertibleCharacters);

                if (nonConvertibleCharacters> 0)
                {
                    isGSM7Convertible = EFalse;
                    break;
                }
                else
                {
                    // Nothing to do
                }

                tempBuf.Zero();
                tempData.Set(tempData.Right(charsToBeConverted));
            }
        }
        else
        {
            // Nothing to do
        }
        (isGSM7Convertible)?msgEncoding = TSmsDataCodingScheme::ESmsAlphabet7Bit
                                          :msgEncoding = TSmsDataCodingScheme::ESmsAlphabetUCS2;
        delete cConverter;
        cConverter = 0;
    }
    else
    {
        msgEncoding = TSmsDataCodingScheme::ESmsAlphabet8Bit;
    }
    return msgEncoding;
}
/**
 * Method to send the message.
 */
int CSmsPlatformServiceS60Impl::send()
{
    JELOG2(EWMA);
    int error =KErrNone;
    TRAP(error,
    {
        TBuf<RMobilePhone::KMaxMobileTelNumberSize>ServiceCentreNumber;
        //get the service centre number
        CServiceCentreNumRetrieverS60Impl::retrieveServiceCentreNumberL(
            ServiceCentreNumber);
        (mSendMessage->SmsPDU()).SetServiceCenterAddressL(ServiceCentreNumber);
        RSmsSocketWriteStream socketWrite(mSmsSendSocket);
        CleanupClosePushL(socketWrite);
        socketWrite << *mSendMessage;
        socketWrite.CommitL();
        CleanupStack::PopAndDestroy(&socketWrite);
        TRequestStatus status;
        mSmsSendSocket.Ioctl(KIoctlSendSmsMessage,status,&mSmsSendDummyBuf,
                             KSolSmsProv);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());
    });
    return error;
}

/**
 * Gets the Message type of the received Message
 */
int CSmsPlatformServiceS60Impl :: receivedMessageType(ServerConnectionBase&
        aServerConn)
{
    JELOG2(EWMA);
    int messageType = KErrNone;
    if (0 == mMessageParameters)
    {
        mMessageParameters = new TJavaMessageParametersBuf();
    }
    else
    {
        // Nothing to do
    }
    int error = aServerConn.retrieveMessage(*mMessageParameters);
    if (error != KErrNone)
    {
        ELOG1(EWMA, "WMA :: receivedMessageType error: %d", error);
        return error;
    }
    else
    {
        switch ((*mMessageParameters)().mEncoding)
        {
        case TSmsDataCodingScheme::ESmsAlphabet7Bit:
        case TSmsDataCodingScheme::ESmsAlphabetUCS2:
        case TSmsDataCodingScheme::ESmsAlphabetReserved:
        {
            messageType = MESSAGE_TEXT;
            break;
        }
        case TSmsDataCodingScheme::ESmsAlphabet8Bit:
        {
            messageType = MESSAGE_BINARY;
            break;
        }
        default:
        {
            messageType = KErrNotSupported;
            ELOG(EWMA,"WMA : Invalid Message Type");
            break;
        }
        }
    }
    return messageType;
}
/**
 * Gets the Remote Host Address.
 */
jstring CSmsPlatformServiceS60Impl::getHostAddress(JNIEnv& aJni)
{
    JELOG2(EWMA);
    TBuf<KMaxUriSize> Uri;
    const TDesC& address = (*mMessageParameters)().mAddress;
    TUint16 port = (*mMessageParameters)().mPort;
    if (mUri.find(L"cbs://:") == std::wstring::npos)
    {
        Uri.Append(KSmsProtocol);
        Uri.Append(KUriColonAndSlashes);
        Uri.Append(address);
    }
    else
    {
        Uri.Append(KCbsProtocol);
        Uri.Append(KUriColonAndSlashes);
        Uri.Append(KCBSEmptyAddress);
    }
    Uri.Append(':');
    Uri.AppendNum(port);
    return aJni.NewString((jchar*)Uri.Ptr(),Uri.Length());
}
/**
 * gets the received message as jobject to java side.
 */
jobject CSmsPlatformServiceS60Impl::getMessageData(
    JNIEnv& aJni,const int aMessageType)
{
    JELOG2(EWMA);
    jobject javaSmsData = 0;
    if (aMessageType ==MESSAGE_TEXT)
    {
        javaSmsData = aJni.NewString(
                          (jchar*)((*mMessageParameters)().mData).Ptr(),
                          ((*mMessageParameters)().mData).Length());
    }
    else if (aMessageType == MESSAGE_BINARY)
    {
        TInt length = ((*mMessageParameters)().mData).Length();
        TBuf8<KSmsMaxDataSize> dataBuf;
        dataBuf.SetLength(length);
        dataBuf.Copy(((const TDesC16&)((*mMessageParameters)().mData)));
        jbyteArray javaSmsBinaryData = aJni.NewByteArray(dataBuf.Length());
        if (javaSmsBinaryData)
        {
            TInt nativeBufferLength = dataBuf.Length();
            TUint8* nativeBufferPtr = const_cast<TUint8*>(dataBuf.Ptr());
            jbyte* jNativeBufferPtr = reinterpret_cast<jbyte*>(
                                          nativeBufferPtr);
            aJni.SetByteArrayRegion(javaSmsBinaryData, 0,
                                    nativeBufferLength,jNativeBufferPtr);
            javaSmsData = javaSmsBinaryData;
        }
        else
        {
            // Noting to do
        }
    }
    else
    {
        // Not a supported message type, It is already handled
    }
    return javaSmsData;
}
/**
 * Gets the Time stamp of the received message.
 */
jlong CSmsPlatformServiceS60Impl::getTimeStamp()
{
    JELOG2(EWMA);
    if (mUri.find(L"cbs://:") != std::wstring::npos)
    {
        return 0;
    }
    else
    {
        return java::util::S60CommonUtils::
               TTimeToJavaTime((*mMessageParameters)().mTimestamp);
    }

}
} //namespace wma
} //namespace java


