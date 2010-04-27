/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JavaDataAccessImpl
*
*/


#include "commsclientendpoint.h"
#include "commsmessage.h"
#include "javacommonutils.h"
#include "javadataaccessimpl.h"
#include "javastorageexception.h"
#include "javastoragemessage.h"
#include "logger.h"

using namespace java::comms;
using namespace java::storage;
using namespace java::util;
using namespace std;

JavaDataAccessImpl::JavaDataAccessImpl() throw(JavaStorageException)
        : mConnOpen(false), mCachedSessionID("")
{
    JELOG2(EJavaStorage);
    mConnOpen = false;
    mCliConn.reset(new CommsClientEndpoint());
    int resultCode = mCliConn->connect(IPC_ADDRESS_JAVA_STORAGE_C);

    if (resultCode != 0)
    {
        ELOG1(EJavaStorage, "Cannot connect via comms: %d", resultCode);
        throw JavaStorageException(resultCode, "Comms failure",
                                   __FILE__, __FUNCTION__, __LINE__);
    }
}

OS_EXPORT JavaDataAccessImpl::~JavaDataAccessImpl()
{
    JELOG2(EJavaStorage);

    // If intance is deleted prior connection is closed do close which triggers
    // rollback on server side.
    if (mConnOpen)
    {
        string msgID = JavaCommonUtils::intToString(JavaStorageMessage::EClose);
        string msgIDSize = JavaCommonUtils::intToString(msgID.size());
        string requestHeaders = msgIDSize + msgID + mCachedSessionID;
        CommsMessage message;
        close(requestHeaders, message);
    }

    int resultCode = mCliConn->disconnect();

    if (resultCode != 0)
    {
        LOG1(EJavaStorage, EInfo,
             "Comms disconnect result code: %d", resultCode);
    }
}

OS_EXPORT void JavaDataAccessImpl::open(
    const std::string& aHeaders,
    const std::string& aStorageName,
    CommsMessage& aReceivedMessage) throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    CommsMessage sendStorageMessage;
    sendStorageMessage.setMessageId(IPC_JS_CONN_REG);

    // Headers contain MsgIdentifier,SessionId
    sendStorageMessage<<aHeaders;
    sendStorageMessage<<aStorageName;

    sendReceiveMessage(sendStorageMessage, aReceivedMessage);
    // SessionID is cached for the case that client deleted storage instance
    // with open connection. SessionID is required to close open connection.
    int resultCode = 0;
    aReceivedMessage>>resultCode;
    if (resultCode >= 0)
    {
        aReceivedMessage>>mCachedSessionID;
        aReceivedMessage.begin();
        mConnOpen = true;
    }
}

OS_EXPORT void JavaDataAccessImpl::close(const std::string& aHeaders,
        CommsMessage& aReceivedMessage)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    CommsMessage sendStorageMessage;
    sendStorageMessage.setMessageId(IPC_JS_CONN_REG);

    // Headers contain MsgIdentifier,SessionId
    sendStorageMessage<<aHeaders;
    sendStorageMessage<<"";

    sendReceiveMessage(sendStorageMessage, aReceivedMessage);
    mConnOpen = false;
}

OS_EXPORT void JavaDataAccessImpl::execute(const std::string& aHeaders,
        const std::wstring& aSqlStatement,
        CommsMessage& aReceivedMessage)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    CommsMessage sendStorageMessage;
    sendStorageMessage.setMessageId(IPC_JS_CONN_REG);

    // Headers contain MsgIdentifier,SessionId
    sendStorageMessage<<aHeaders;
    // Data is pure sqlstatement
    sendStorageMessage<<aSqlStatement;

    sendReceiveMessage(sendStorageMessage, aReceivedMessage);
}

void JavaDataAccessImpl::sendReceiveMessage(CommsMessage& aSendMessage,
        CommsMessage& aReceivedMessage)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    int resultCode = -1;

    aSendMessage.setReceiver(IPC_ADDRESS_JAVA_STORAGE_C);
    aSendMessage.setModuleId(PLUGIN_ID_JAVASTORAGE_CLIENT_C);

    resultCode = mCliConn->sendReceive(aSendMessage, aReceivedMessage, 10);

    if (resultCode != 0)
    {
        ELOG1(EJavaStorage, "Comms result code: %d", resultCode);

        throw JavaStorageException(resultCode, "Comms failure",
                                   __FILE__, __FUNCTION__, __LINE__);
    }
}
