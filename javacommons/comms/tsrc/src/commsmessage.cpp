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
* Description:  ?Description
*
*/

#include "TestHarness.h"
#include "javauid.h"
#include "javacommonutils.h"
#include "exceptionbase.h"

#include "commsmessage.h"
#include "commspermissions.h"
#include "commsclientendpoint.h"
#include "echoserver.h"

using namespace java::comms;
using namespace std;
using java::util::Uid;
using java::util::JavaCommonUtils;


class PermissionListener : public CommsListener
{
public:
    CommsMessage msg;
    CommsEndpoint* comms;

    virtual void processMessage(CommsMessage& aMessage)
    {
        msg = aMessage;
        CommsMessage reply;
        reply.replyTo(aMessage);
        comms->send(reply);
    }
};



TEST_GROUP(CommsMessage)
{
    EchoServer server;
    CommsClientEndpoint client;

    TEST_SETUP()
    {
        server.start(IPC_ADDRESS_COMMS_MODULE_TEST);
        client.connect(IPC_ADDRESS_COMMS_MODULE_TEST);
    }

    TEST_TEARDOWN()
    {
        client.disconnect();
        server.stop();
    }
};

/**
 * Test setters and getters
 * 1: test header setters and getters
 * 2: test message body setters and getters
 * 3: read contents multiple times
 */
TEST(CommsMessage, getterSetter)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(1);
#else
    EXPECT_N_LEAKS(2);
#endif
    // 1: test header setters and getters
    CommsMessage msg;
    msg.setModuleId(1111);
    msg.setMessageId(2222);
    msg.setMessageRef(3333);

    CommsMessage receivedMsg;
    CHECK(!client.sendReceive(msg, receivedMsg, 1));

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(msg.getMessageId() == receivedMsg.getMessageId());
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    // 2: test message body setters and getters
    string sin = "this is a string";
    wstring win = L"this is a wstring";
    int in = 1;
    long long lin = -1LL;
    Uid uid(L"sampleUid");

    msg << sin << win << in << lin << uid;
    CHECK(!client.sendReceive(msg, receivedMsg, 1));
    string sout;
    wstring wout;
    int out;
    long long lout;
    Uid uidOut;
    receivedMsg >> sout >> wout >> out >> lout >> uidOut;

    CHECK(0 == sin.compare(sout));
    CHECK(0 == win.compare(wout));
    CHECK(in == out);
    CHECK(lin == lout);
    CHECK(uid == uidOut);

    // 3: read contents multiple times
    CommsMessage m;
    m << "test" << 1;
    CommsMessage r;
    CHECK(!client.sendReceive(m, r, 1));
    string s, s2;
    int i, i2;
    r >> s >> i;
    r.begin();
    r >> s2 >> i2;
    CHECK(0 == s.compare(s));
    CHECK(i == i2);
}


/**
 * Test CommsMessage streaming
 * 1: check ascii chars
 */
TEST(CommsMessage, ascii)
{
    CommsMessage msg;
    const int len = 0xff;
    char chars[len];
    for (int i=0; i < len; i++)
    {
        chars[i] = i;
    }
    string ascii(chars, len);
    msg << ascii;
    string result;
    msg >> result;
    CHECK(0 == ascii.compare(result));

    char* arr = msg.toByteArray();
    CommsMessage msg2(reinterpret_cast<ipcMessage_t*>(arr));

    msg2 >> result;
    CHECK(0 == ascii.compare(result));
}


/**
 * Test CommsMessage streaming
 * 1: check special cases: space, '\n' and empty string
 */
TEST(CommsMessage, SpecialChars)
{
    CommsMessage msg;
    string a = "";
    string b = "\n";
    string c;
    string d = " ";
    string e = "  ";
    string f = "\n\n";
    string g = "\n ";
    string h = " \r\n";
    int i = 0;
    int j = 0xffffffff;
    long long k = -1LL;
    long long l = 0LL;
    Uid m;

    msg << a << i << k << b << c << j << d << e << l << f << g << h << m;
    string ao, bo, co, doo, eo, fo, go, ho;
    int io, jo;
    long long ko, lo;
    Uid mo;
    msg >> ao >> io >> ko >> bo >> co >> jo >> doo >> eo >> lo >> fo >> go >> ho >> mo;

    CHECK(0 == a.compare(ao));
    CHECK(0 == b.compare(bo));
    CHECK(0 == c.compare(co));
    CHECK(0 == d.compare(doo));
    CHECK(0 == e.compare(eo));
    CHECK(0 == f.compare(fo));
    CHECK(0 == g.compare(go));
    CHECK(0 == h.compare(ho));
    CHECK(i == io);
    CHECK(j == jo);
    CHECK(k == ko);
    CHECK(l == lo);
    CHECK(m == mo);
    // make copy
    char* arr = msg.toByteArray();
    CommsMessage msg2(reinterpret_cast<ipcMessage_t*>(arr));

    msg2 >> ao >> io >> ko >> bo >> co >> jo >> doo >> eo >> lo >> fo >> go >> ho >> mo;

    CHECK(0 == a.compare(ao));
    CHECK(0 == b.compare(bo));
    CHECK(0 == c.compare(co));
    CHECK(0 == d.compare(doo));
    CHECK(0 == e.compare(eo));
    CHECK(0 == f.compare(fo));
    CHECK(0 == g.compare(go));
    CHECK(0 == h.compare(ho));
    CHECK(i == io);
    CHECK(j == jo);
    CHECK(k == ko);
    CHECK(l == lo);
    CHECK(m == mo);
}

/**
 * Test CommsMessage streaming
 * 1: empty message
 */
TEST(CommsMessage, empty)
{
    CommsMessage msg;

    char* arr = msg.toByteArray();
    CommsMessage msg2(reinterpret_cast<ipcMessage_t*>(arr));

    CommsMessage msg3(msg);
    CommsMessage msg4 = msg;
    CommsMessage msg5 = msg;

    CHECK(0 == msg.toString().compare(msg2.toString()));
    CHECK(0 == msg.toString().compare(msg3.toString()));
    CHECK(0 == msg.toString().compare(msg4.toString()));

    // check that content can be added ok
    string s = "hello world";
    int i = 3;
    msg5 << i << s;

    string so;
    int io;

    msg5 >> io >> so;
    CHECK(0 == s.compare(so));
    CHECK(i == io);
}

/**
 * Test CommsMessage streaming
 * 1: copy constructor and assingment test
 * 2: copy constructor and assingment test (bytearray)
 * 3: self-assingment test
 */
TEST(CommsMessage, constuctor)
{
    // 1: copy constructor and assingment test
    CommsMessage msg;

    msg << "hello" << "world" << "this is test #" << 4;

    CommsMessage msg2(msg);
    CommsMessage msg3 = msg;

    CHECK(0 == msg.toString().compare(msg2.toString()));
    CHECK(0 == msg.toString().compare(msg3.toString()));

    // 2: copy constructor and assingment test (bytearray)
    CommsMessage m;

    m << "hello" << "world" << "this is test #" << 4;

    char* arr = m.toByteArray();
    CommsMessage m2(reinterpret_cast<ipcMessage_t*>(arr));

    CHECK(0 == m.toString().compare(m2.toString()));

    // 3: self-assingment test
    CommsMessage self;

    self << "hello";
    CHECK((self = self) == &self)
    string s;
    self >> s;
    CHECK(0 == s.compare("hello"));
}

/**
 * Test CommsMessage streaming
 * 1: read same value multiple times
 */
TEST(CommsMessage, multipleReads)
{
    CommsMessage msg;

    msg << "string 1" << "string 2" << "string 3" << "string 4";
    string a, b, c, temp;

    msg >> a;
    msg.begin();

    msg >> b;

    while (!(msg>>temp))
    {
        // read everything
    };

    msg.begin();
    msg >> c;

    CHECK(0 == a.compare(b));
    CHECK(0 == a.compare(c));
}

/**
 * Test CommsMessage streaming
 * 1: read in wrong order
 */
TEST(CommsMessage, readOrder)
{
    CommsMessage msg;

    msg << "string 1" << 1 << 2 << "string 2";
    string a, b;
    int i, j;

    msg >> i >> a >> b >> j;
    CHECK(0 == i);

    CommsMessage msg2;
    msg2 >> i >> a;
    CHECK(0 == i);
    CHECK(0 == a.compare(""));
}

/**
 * Test resetting CommsMessage
 * 1: reset message with payload
 * 2: reset empty message
 */
TEST(CommsMessage, reset)
{
    CommsMessage empty;

    // 1: reset message with payload
    CommsMessage msg;
    msg.setModuleId(1111);
    msg.setMessageId(2222);
    msg.setMessageRef(3333);
    msg.setSender(4444);
    msg.setReceiver(5555);
    msg << "string 1" << 1;
    msg.reset();

    CHECK(0 == msg.toString().compare(empty.toString()));
    CHECK(msg.getModuleId() == empty.getModuleId());
    CHECK(msg.getMessageId() == empty.getMessageId());
    CHECK(msg.getMessageRef() == empty.getMessageRef());
    CHECK(msg.getSender() == empty.getSender());
    CHECK(msg.getReceiver() == empty.getReceiver());

    // 2: reset empty message
    CommsMessage msg2;
    msg2.reset();

    CHECK(0 == msg2.toString().compare(empty.toString()));
    CHECK(msg2.getModuleId() == empty.getModuleId());
    CHECK(msg2.getMessageId() == empty.getMessageId());
    CHECK(msg2.getMessageRef() == empty.getMessageRef());
    CHECK(msg2.getSender() == empty.getSender());
    CHECK(msg2.getReceiver() == empty.getReceiver());
}

/**
 * Test replyTo method
 * 1: test replyTo method
 */
TEST(CommsMessage, replyTo)
{
    // 1: test replyTo method
    CommsMessage msg;
    msg.setModuleId(1111);
    msg.setMessageId(2222);
    msg.setMessageRef(3333);
    msg.setSender(4444);
    msg.setReceiver(5555);

    CommsMessage reply;
    reply.replyTo(msg);

    CHECK(msg.getModuleId() == reply.getModuleId());
    CHECK(msg.getMessageId() == reply.getMessageId());
    CHECK(msg.getMessageRef() == reply.getMessageRef());
    CHECK(msg.getSender() == reply.getReceiver());
    CHECK(msg.getReceiver() == reply.getSender());
}

/**
 * Test CommsMessage with different character encodings
 * 1: write and read UTF16 / UTF8 strings
 * 2: read-write mismatch cases string / wstring
 */
TEST(CommsMessage, charEncoding)
{
    // 1: write and read UTF16 / UTF8 strings
    CommsMessage msg;

    // devanagari letter A, tibetan digit zero, katakana letter YA
    wstring w1 = L"\u0905\u0F20\u30E4";
    string w1_in_utf8 = wstringToUtf8(w1);

    // latin small letter a with diaeresis, latin small letter a with ring above,
    // latin small letter o with diaeresis, Euro symbol
    wstring w2 = L"\u00E4\u00E5\u00F6\u20AC";
    string w2_in_utf8 = wstringToUtf8(w1);

    // Japanese Yen symbol, Roman AE with acute accent, Greek Capital Alpha, Greek Capital Omega,
    // Euro symbol, Rupee symbol, Cyrillic capital letter DZHE, Arabic letter TEH
    wstring w3 = L"\u00a5\u01FC\u0391\u03A9\u20ac\u20a8\u040F\u062A";
    string w3_in_utf8 = wstringToUtf8(w3);

    msg << w1 << w1_in_utf8 << w2 << w2_in_utf8 << w3 << w3_in_utf8;
    wstring wo1, wo2, wo3;
    string so1, so2, so3;
    msg >> wo1 >> so1 >> wo2 >> so2 >> wo3 >> so3;

    CHECK(0 == wo1.compare(w1));
    CHECK(0 == wo2.compare(w2));
    CHECK(0 == wo3.compare(w3));
    CHECK(0 == so1.compare(w1_in_utf8));
    CHECK(0 == so2.compare(w2_in_utf8));
    CHECK(0 == so3.compare(w3_in_utf8));

    // 2: read-write mismatch cases string / wstring
    msg.reset();
    string s4 = "hello world";
    wstring w4 = L"hello world";

    // devanagari letter A, tibetan digit zero, katakana letter YA
    string utf8 = "\xE0\xA4\x85\xE0\xBC\xA0\xE3\x83\xA4";
    wstring utf16 = L"\u0905\u0F20\u30E4";

    msg << s4 << w4 << utf8 << utf16;
    string so4, utf8_out;
    wstring wo4, utf16_out;
    // read in wrong order string as wstring and wstrings as string
    msg >> wo4 >> so4 >> utf16_out >> utf8_out;

    CHECK(0 == so4.compare(s4));
    CHECK(0 == wo4.compare(w4));

    CHECK(0 == utf8_out.compare(utf8));
    CHECK(0 == utf16_out.compare(utf16));

}


/**
 * Test hasPermission method
 * 1: test hasPermission method when used without client
 */
TEST(CommsMessage, hasPermission)
{
    // 1: test hasPermission method
    CommsMessage msg;
    CommsMessage msg2 = msg;
    CommsMessage msg3(msg);

    CHECK(msg.hasPermission(MANAGE_CERTIFICATES) == false);
    CHECK(msg2.hasPermission(MANAGE_CERTIFICATES) == false);
    CHECK(msg3.hasPermission(MANAGE_CERTIFICATES) == false);
}


/**
 * Test hasPermission method with all permissions
 * 1: check trust in client and server side
 */
TEST(CommsMessage, hasPermissionAll)
{
    // 1: check trust in client and server side
    CommsMessage msg;
    CommsMessage msg2;

    CommsServerEndpoint server;
    CommsClientEndpoint client;

    PermissionListener listener;
    listener.comms = &server;

    CHECK(!server.registerDefaultListener(&listener));
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+1));

    CommsPermission permissions = (CommsPermission)((int)MANAGE_CERTIFICATES |
                                  (int)INSTALL_APPLICATION |
                                  (int)LAUNCH_APPLICATION |
                                  (int)STOP_APPLICATION);

    CHECK(client.sendReceive(msg, msg2, 2) == 0);

    CHECK(msg2.hasPermission(permissions) == true);
    CHECK(listener.msg.hasPermission(permissions) == true);

    CHECK(!client.disconnect());
    CHECK(!server.stop());
}

/**
 * Test hasPermission method with manage certificates permission
 * 1: check trust in client and server side
 */
TEST(CommsMessage, hasPermissionManageCerts)
{
    // 1: check trust in client and server side
    CommsMessage msg;
    CommsMessage msg2;

    CommsServerEndpoint server;
    CommsClientEndpoint client;

    PermissionListener listener;
    listener.comms = &server;

    CHECK(!server.registerDefaultListener(&listener));
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+1));

    CommsPermission permissions = MANAGE_CERTIFICATES;

    CHECK(client.sendReceive(msg, msg2, 2) == 0);

    CHECK(msg2.hasPermission(permissions) == true);
    CHECK(listener.msg.hasPermission(permissions) == true);

    CHECK(!client.disconnect());
    CHECK(!server.stop());
}

/**
 * Test hasPermission method with install app permission
 * 1: check trust in client and server side
 */
TEST(CommsMessage, hasPermissionInstallApp)
{
    // 1: check trust in client and server side
    CommsMessage msg;
    CommsMessage msg2;

    CommsServerEndpoint server;
    CommsClientEndpoint client;

    PermissionListener listener;
    listener.comms = &server;

    CHECK(!server.registerDefaultListener(&listener));
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+1));

    CommsPermission permissions = INSTALL_APPLICATION;

    CHECK(client.sendReceive(msg, msg2, 2) == 0);

    CHECK(msg2.hasPermission(permissions) == true);
    CHECK(listener.msg.hasPermission(permissions) == true);

    CHECK(!client.disconnect());
    CHECK(!server.stop());
}

/**
 * Test hasPermission method with launch app permission
 * 1: check trust in client and server side
 */
TEST(CommsMessage, hasPermissionLaunchApp)
{
    // 1: check trust in client and server side
    CommsMessage msg;
    CommsMessage msg2;

    CommsServerEndpoint server;
    CommsClientEndpoint client;

    PermissionListener listener;
    listener.comms = &server;

    CHECK(!server.registerDefaultListener(&listener));
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+1));

    CommsPermission permissions = LAUNCH_APPLICATION;

    CHECK(client.sendReceive(msg, msg2, 2) == 0);

    CHECK(msg2.hasPermission(permissions) == true);
    CHECK(listener.msg.hasPermission(permissions) == true);

    CHECK(!client.disconnect());
    CHECK(!server.stop());
}

/**
 * Test hasPermission method with stop app permission
 * 1: check trust in client and server side
 */
TEST(CommsMessage, hasPermissionStopApp)
{
    // 1: check trust in client and server side
    CommsMessage msg;
    CommsMessage msg2;

    CommsServerEndpoint server;
    CommsClientEndpoint client;

    PermissionListener listener;
    listener.comms = &server;

    CHECK(!server.registerDefaultListener(&listener));
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+1));

    CommsPermission permissions = STOP_APPLICATION;

    CHECK(client.sendReceive(msg, msg2, 2) == 0);

    CHECK(msg2.hasPermission(permissions) == true);
    CHECK(listener.msg.hasPermission(permissions) == true);

    CHECK(!client.disconnect());
    CHECK(!server.stop());
}


