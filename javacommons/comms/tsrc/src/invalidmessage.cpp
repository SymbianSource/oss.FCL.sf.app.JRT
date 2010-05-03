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
* Description:  Unit tests for invalid messages: message size is wrong,
*               message type is wrong, messages sent in wrong order etc.
*
*/

#include <sstream>
#include <string>
#include "TestHarness.h"
#include "echoserver.h"
#include "transport.h"

using namespace java::comms;

class DummyListener : public IpcListener
{
public:
    virtual void processMessage(const ipcMessage_t* /*aMsg*/) {};
    virtual void onStart() {};
    virtual void onExit() {};
};

#ifdef __SYMBIAN32__
#include "javasymbianoslayer.h"
enum ECommsOperations
{
    ESend,
    EReceive,
    ECancelReceive
};

class RBadClient : public RSessionBase
{
public:
    TInt connect(int aAddr)
    {
        std::stringstream address;
        address << "java-comms-" << aAddr;
        std::auto_ptr<HBufC> serverName(stringToDes(address.str().c_str()));

        TInt rc = CreateSession(serverName->Des(), TVersion(1,1,1));
        return rc;
    };

    TInt send(const TDesC8& aData)
    {
        return SendReceive(ESend, TIpcArgs(&aData));
    };

    TInt cancel()
    {
        return SendReceive(ECancelReceive, TIpcArgs(&KNullDesC8));
    };

    void receive(RBuf8& aBuffer, TPckgBuf<TInt>& aRequiredSize, TRequestStatus& aStatus)
    {
        TIpcArgs args;
        args.Set(0, &aBuffer);
        args.Set(1, &aRequiredSize);

        SendReceive(EReceive, args, aStatus);
    };
    using RSessionBase::SendReceive;
};
#endif

TEST_GROUP(InvalidMessage)
{
    EchoServer server;

    TEST_SETUP()
    {
        server.start(IPC_ADDRESS_COMMS_MODULE_TEST);
    }

    TEST_TEARDOWN()
    {
        server.stop();
    }
};

#ifdef __SYMBIAN32__
TEST(InvalidMessage, invalidMessage)
{
    EXPECT_N_LEAKS(1);
    DummyListener listener;
    std::auto_ptr<IpcConnectionInterface> client(IpcConnectionFactory::createConnection(&listener));

    CHECK(!client->connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    ipcMessage_t msg;

    msg.ipcHeader.length = 0;
    int rc = client->send(&msg);
    CHECK(rc != 0);

    msg.ipcHeader.length = 1;
    rc = client->send(&msg);
    CHECK(rc != 0);

    msg.ipcHeader.length = sizeof(ipcHeader_t)-1;
    rc = client->send(&msg);
    CHECK(rc != 0);

    msg.ipcHeader.length = sizeof(ipcHeader_t);
    rc = client->send(&msg);
    CHECK(rc == 0);

    client->disconnect();
}

TEST(InvalidMessage, messageAndHeaderSizeMismatch)
{
    EXPECT_N_LEAKS(1);
    RBadClient client;
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    int realSize = sizeof(ipcHeader_t);
    std::auto_ptr<HBufC8> data(HBufC8::New(realSize));
    TPtr8 ptr = data->Des();

    ipcMessage_t msg;
    TPtr8 ptr8((TUint8 *)&msg, realSize);
    ptr8.SetLength(realSize);

    // length in header is too big
    msg.ipcHeader.length = realSize + 1;
    ptr.Copy(ptr8);
    int rc = client.send(data->Des());
    CHECK(rc != 0);

    // length in header is too small
    msg.ipcHeader.length = realSize -1;
    ptr.Copy(ptr8);
    rc = client.send(data->Des());
    CHECK(rc != 0);

    // length in header is correct
    msg.ipcHeader.length = realSize;
    ptr.Copy(ptr8);
    rc = client.send(data->Des());
    CHECK(rc == 0);

    // empty message
    rc = client.send(KNullDesC8);
    CHECK(rc != 0);

    client.Close();
}

TEST(InvalidMessage, invalidMessageOrder)
{
    EXPECT_N_LEAKS(1);
    RBadClient client;
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    // multiple cancels when no receive sent
    CHECK(!client.cancel());
    CHECK(!client.cancel());

    // multiple sends when no receive sent
    int size = sizeof(ipcHeader_t);
    std::auto_ptr<HBufC8> data(HBufC8::New(size));
    TPtr8 ptr = data->Des();

    ipcMessage_t msg;
    TPtr8 ptr8((TUint8 *)&msg, size);
    ptr8.SetLength(size);
    msg.ipcHeader.length = size;
    ptr.Copy(ptr8);

    CHECK(!client.send(data->Des()));
    CHECK(!client.send(data->Des()));

    // receive sent messages
    RBuf8 buffer;
    buffer.Create(100);
    TPckgBuf<TInt> requiredLength;
    TRequestStatus status;

    client.receive(buffer, requiredLength, status);
    User::WaitForRequest(status);
    CHECK(status.Int() == KErrNone);

    client.receive(buffer, requiredLength, status);
    User::WaitForRequest(status);
    CHECK(status.Int() == KErrNone);

    // cancel receive message
    client.receive(buffer, requiredLength, status);
    CHECK(!client.cancel());
    User::WaitForRequest(status);
    CHECK(status.Int() == KErrCancel);

    // receive message with too small buffer
    buffer.Close();
    buffer.Create(1);
    client.receive(buffer, requiredLength, status);

    client.send(data->Des());
    User::WaitForRequest(status);
    CHECK(status.Int() == KErrOverflow);
    CHECK(!client.cancel());

    // invalid message type
    CHECK(client.SendReceive(313, TIpcArgs(&KNullDesC8)) == KErrNotSupported);

    buffer.Close();
    client.Close();
}
#endif

