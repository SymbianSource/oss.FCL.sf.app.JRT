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
#include <gsmuset.h>
#include <charconv.h>

#include "logger.h"
#include "s60commonutils.h"
#include "jstringutils.h"

#include "javacommonutils.h"
#include "javaapduconnectionimpl.h"
#include "com_nokia_mj_impl_satsa_APDUConnectionImpl.h"
#include "cstsconnection.h"
#include "cstsresphandler.h"
#include "cstsmidletinfo.h"

#include "javajniutils.h"
#include "com_nokia_mj_impl_satsa_APDUConnectionImpl.h"
#include "fs_methodcall.h"

namespace java
{
namespace satsa
{

const TInt KConnTypeIndex = 1;

JavaApduConnectionImpl::JavaApduConnectionImpl()
{
    //Initialize any member to 0 or Null
}

JavaApduConnectionImpl::~JavaApduConnectionImpl()
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::~JavaApduConnectionImpl");
    //Release resource here (delete pointer)
    if (iConnection)
    {
        delete iConnection;
        iConnection = NULL;
    }
    if (mSlotConn)
    {
        delete mSlotConn;
        mSlotConn = NULL;
    }

}

JavaApduConnectionBase* JavaApduConnectionBase::getInstance()
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::getInstance ");
    JavaApduConnectionImpl* mesgConn = new JavaApduConnectionImpl();
    if (mesgConn == NULL)
    {
        throw ENOMEM;
    }
    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::getInstance ");
    return mesgConn;
}

int JavaApduConnectionImpl::CreateSlot(JNIEnv& aJni, jobject aPeer)
{
    LOG(ESATSA, EInfo, "+ Entering JavaApduConnectionImpl::CreateSlot");
    TInt err = 0;
    TRAP(err, CreateSlotConn(aJni, aPeer));
    return err;
}

void JavaApduConnectionImpl::CreateSlotConn(JNIEnv& aJni, jobject aPeer)
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::CreateSlotConn");
    CSTSRespHandler* respHandler = NULL;
    CSTSMidletInfo* midletInfo = NULL;
    // Create connection object
    mSlotConn = CSTSConnection::NewL(aJni, aPeer, respHandler, midletInfo);
    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::CreateSlotConn");
}

int JavaApduConnectionImpl::CreateConn(JNIEnv& aJni, jobject aPeer,
                                       jmethodID aHandleEventMethod, jstring aUri, jstring aName,
                                       jstring aVendor, jstring aVersion, jstring aDomain,
                                       jstring aRootCertHash, jstring aSigningCertHash,
                                       TInt* aReturnValueArray)
{
    LOG(ESATSA, EInfo, "+ Entering JavaApduConnectionImpl::CreateConn");
    TInt err = 0;
    TRAP(err, Create(aJni, aPeer, aHandleEventMethod, aUri, aName, aVendor,
                     aVersion, aDomain, aRootCertHash, aSigningCertHash,
                     aReturnValueArray));
    LOG(ESATSA, EInfo, "-- leaving JavaApduConnectionImpl::CreateConn");
    return err;

}

void JavaApduConnectionImpl::Create(JNIEnv& aJni, jobject aNotifyObject,
                                    jmethodID aHandleEventMethod, jstring aUri, jstring aName,
                                    jstring aVendor, jstring aVersion, jstring aDomain,
                                    jstring aRootCertHash, jstring aSigningCertHash,
                                    TInt* aReturnValueArray)
{
    LOG(ESATSA, EInfo, "+ Entering JavaApduConnectionImpl::Create");

    // Set the URI string contents to Symbian descriptor.
    JStringUtils aMUri(aJni, aUri);
    // Set the name string contents to Symbian descriptor.
    JStringUtils aMName(aJni, aName);
    // Set the vendor string contents to Symbian descriptor.
    JStringUtils aMVendor(aJni, aVendor);
    // Set the version string contents to Symbian descriptor.
    JStringUtils aMVersion(aJni, aVersion);
    // Set the version string contents to Symbian descriptor.
    JStringUtils aMDomain(aJni, aDomain);
    // Set the version string contents to Symbian descriptor.
    JStringUtils aMRootCert(aJni, aRootCertHash);
    // Set the version string contents to Symbian descriptor.
    JStringUtils aMSignCert(aJni, aSigningCertHash);

    LOG(ESATSA, EInfo, "Initializing the URI");
    CSTSURI* uri = CSTSURI::NewLC(aMUri);

    LOG(ESATSA, EInfo, "After initializing the URI");
    //set type of connection for java side
    if (uri->Type() == CSTSURI::EAID)
    {
        LOG(ESATSA, EInfo, "the URI is AID type");
        aReturnValueArray[KConnTypeIndex]
        = com_nokia_mj_impl_satsa_APDUConnectionImpl_ENUMERATION_AID;
    }
    else
    {
        LOG(ESATSA, EInfo, "the URI is SAT type");
        aReturnValueArray[KConnTypeIndex]
        = com_nokia_mj_impl_satsa_APDUConnectionImpl_ENUMERATION_SAT;
    }

    LOG(ESATSA, EInfo, "initialize CSTSMidletInfo");
    CSTSMidletInfo* midletInfo = CSTSMidletInfo::NewL(aMName, aMVendor,
                                 aMVersion, aMDomain, aMRootCert, aMSignCert);

    LOG(ESATSA, EInfo, "After initializing CSTSMidletInfo..");
    CleanupStack::PushL(midletInfo);

    LOG(ESATSA, EInfo, "midletInfo created");
    CSTSRespHandler* respHandler = CSTSRespHandler::NewL(aJni, aNotifyObject,
                                   aHandleEventMethod);

    LOG(ESATSA, EInfo, "respHandler created");

    // Connection object will own the midletInfo object
    CleanupStack::Pop(midletInfo);

    // Create connection object. RespHandler and midletinfo ownership
    // transferred
    iConnection = CSTSConnection::NewL(aJni, aNotifyObject, respHandler,
                                       midletInfo);

    // Connection object will own the uri object
    CleanupStack::Pop(uri);

    CSTSConnection::WrapSetURI(iConnection, uri);

    LOG(ESATSA, EInfo, "JavaApduConnectionImpl::Create::connection created");
    LOG(ESATSA, EInfo, "-- leaiving JavaApduConnectionImpl::Create");

}

int JavaApduConnectionImpl::DoPinMethod(JNIEnv& /*aJni*/, jclass /*aPeer*/,
                                        int /*aHandle*/, int aPinMethodType, int aPinID)
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::DoPinMethod ");
    //CSTSConnection* connection = reinterpret_cast< CSTSConnection* >( aHandle );
    int error = 0;
    TRAP(error, DoPinMethodL(iConnection, aPinMethodType, aPinID));

    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::DoPinMethod ");
    return error;

}

void JavaApduConnectionImpl::DoPinMethodL(CSTSConnection* aConnection,
        TInt aPinMethodType, TInt aPinID)
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::DoPinMethodL ");
    switch (aPinMethodType)
    {
    case com_nokia_mj_impl_satsa_APDUConnectionImpl_ENUMERATION_CHANGE_PIN:
    {
        CSTSConnection::WrapChangePinL(aConnection, aPinID);
        break;
    }
    case com_nokia_mj_impl_satsa_APDUConnectionImpl_ENUMERATION_DISABLE_PIN:
    {
        CSTSConnection::WrapDisablePinL(aConnection, aPinID);
        break;
    }
    case com_nokia_mj_impl_satsa_APDUConnectionImpl_ENUMERATION_ENABLE_PIN:
    {
        CSTSConnection::WrapEnablePinL(aConnection, aPinID);
        break;
    }
    case com_nokia_mj_impl_satsa_APDUConnectionImpl_ENUMERATION_ENTER_PIN:
    {
        CSTSConnection::WrapEnterPinL(aConnection, aPinID);
        break;
    }
    default:
    {
        ELOG(EWMA, "JavaApduConnectionImpl::DoPinMethodL: parameter error");

        break;
    }
    }
    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::DoPinMethodL ");
}

int JavaApduConnectionImpl::ExchangeAPDU(JNIEnv& aJni, jclass /*aPeer*/,
        int /*aHandle*/, jbyteArray aCommandAPDU)
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::ExchangeAPDU ");
    TInt err = KErrNone;
    // Convert the command bytes into an 8-bit descriptor...
    jbyte* byteArray = (aJni).GetByteArrayElements(aCommandAPDU, NULL);
    TInt numComBytes = (aJni).GetArrayLength(aCommandAPDU);
    TPtrC8 commandDes(reinterpret_cast<TUint8*>(byteArray), numComBytes);

    TRAP(err, CSTSConnection::WrapExchangeAPDUL(iConnection, commandDes));
    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::ExchangeAPDU ");
    return err;

}

int JavaApduConnectionImpl::UnblockPin(JNIEnv& /*aJni*/, jclass /*aPeer*/,
                                       int /*aHandle*/, int aBlockedPinID, int aUnblockingPinID)
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::UnblockPin ");
    TInt err = KErrNone;
    TRAP(err, CSTSConnection::WrapUnblockPinL(iConnection, aBlockedPinID,
            aUnblockingPinID));
    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::UnblockPin ");
    //return error code to caller
    return err;
}

std::wstring JavaApduConnectionImpl::GetATR()
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::GetATR ");
    std::wstring aATR;
    TRAPD(err, aATR = CSTSConnection::WrapGetATRL(iConnection));
    if (err != KErrNone)
    {
        return std::wstring(L"");
    }
    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::GetATR ");
    return aATR;
}

int JavaApduConnectionImpl::Open(JNIEnv& /*aJni*/, jclass /*aPeer*/, int /*aHandle*/)
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::Open ");
    TInt err = KErrNone;
    TRAP(err,CSTSConnection::WrapOpenL(iConnection));
    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::Open ");
    //return error code to caller
    return err;
}

std::wstring JavaApduConnectionImpl::GetSlot()
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::GetSLot ");
    std::wstring slotInfo(L"");
    TInt err = KErrNone;
    TRAP(err, slotInfo = CSTSConnection::WrapGetSlotsL(mSlotConn));
    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::GetSlot");
    if (KErrNone != err)
    {
        slotInfo = L"";
    }
    //return error code to caller
    return slotInfo;
}

int JavaApduConnectionImpl::Close(JNIEnv& /*aJni*/, jclass /*aPeer*/, int /*aHandle*/)
{
    LOG(ESATSA, EInfo, "+ JavaApduConnectionImpl::Close ");
    TInt err = KErrNone;
    TRAP(err,CSTSConnection::WrapCloseL(iConnection));

    LOG(ESATSA, EInfo, "-- JavaApduConnectionImpl::Close ");
    //return error code to caller
    return err;
}

} // namespace satsa
} // namespace java


