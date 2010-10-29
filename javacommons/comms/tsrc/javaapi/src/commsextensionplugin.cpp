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
* Description:  Java Captain extension plugin for Comms Module tests
*
*/

#include <string>
#include <iostream>
#include <unistd.h>

#include "comms.h"
#include "logger.h"
#include "coreinterface.h"
#include "javacommonutils.h"
#include "exceptionbase.h"

#include "commsextensionplugin.h"

using namespace std;

const int MODULE_ID_A           = 900;
const int MODULE_ID_B           = 901;
const int MODULE_ID_DATA_CHECK  = 902;
const int MODULE_ID_NO_REPLY    = 903;
const int MODULE_ID_SLEEP_1S    = 1000;
const int MODULE_ID_SLEEP_5S    = 5000;
const int MODULE_ID_SLEEP_10S   = 10000;
const int MODULE_ID_DELAY_REPLY = 11000;

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::comms::CommsExtensionPlugin();
}

namespace java
{
namespace comms
{
// helper method for conversion
std::string wstringToUtf8(const std::wstring& str)
{
    string result;
    try
    {
        char* utf8 = java::util::JavaCommonUtils::wstringToUtf8(str);
        result.assign(utf8);
        delete[] utf8;
    }
    catch (java::util::ExceptionBase& e)
    {
        ELOG1(EJavaComms, "wstringToUtf8 failed: %s", e.toString().c_str());
    }
    return result;
}

CommsExtensionPlugin::CommsExtensionPlugin() : mCore(0), mComms(0), mDelayedReplyCount(0)
{
    JELOG2(EJavaComms);
}

CommsExtensionPlugin::~CommsExtensionPlugin()
{
    JELOG2(EJavaComms);
}

void CommsExtensionPlugin::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaComms);
    mCore = core;
    mComms = core->getComms();

    mComms->registerListener(PLUGIN_ID_JAVACAPTAIN_COMMS_TESTER_C, this);

    mComms->registerListener(MODULE_ID_A, this);
    mComms->registerListener(MODULE_ID_B, this);
    mComms->registerListener(MODULE_ID_DATA_CHECK, this);
    mComms->registerListener(MODULE_ID_NO_REPLY, this);
    mComms->registerListener(MODULE_ID_SLEEP_1S, this);
    mComms->registerListener(MODULE_ID_SLEEP_5S, this);
    mComms->registerListener(MODULE_ID_SLEEP_10S, this);
    mComms->registerListener(MODULE_ID_DELAY_REPLY, this);
}

void CommsExtensionPlugin::stopPlugin()
{
    JELOG2(EJavaComms);
    mComms->unregisterListener(PLUGIN_ID_JAVACAPTAIN_COMMS_TESTER_C, this);

    mComms->unregisterListener(MODULE_ID_A, this);
    mComms->unregisterListener(MODULE_ID_B, this);
    mComms->unregisterListener(MODULE_ID_DATA_CHECK, this);
    mComms->unregisterListener(MODULE_ID_NO_REPLY, this);
    mComms->unregisterListener(MODULE_ID_SLEEP_1S, this);
    mComms->unregisterListener(MODULE_ID_SLEEP_5S, this);
    mComms->unregisterListener(MODULE_ID_SLEEP_10S, this);
    mComms->unregisterListener(MODULE_ID_DELAY_REPLY, this);

    mCore = 0;
    mComms = 0;
}

void CommsExtensionPlugin::processMessage(CommsMessage& aMessage)
{
    /*
        LOG1(EJavaComms, EInfo, "processMessage, sender    = %d", aMessage.getSender());
        LOG1(EJavaComms, EInfo, "processMessage, messageId = %d", aMessage.getMessageId());
        LOG1(EJavaComms, EInfo, "processMessage, moduleId  = %d", aMessage.getModuleId());
        LOG1(EJavaComms, EInfo, "processMessage, messageRef= %d", aMessage.getMessageRef());
    */
    switch (aMessage.getModuleId())
    {
    case MODULE_ID_DATA_CHECK:
    {
        // keep this in sync with java side test case

        const string str("abcdefghijklmnopqrstyvwxyz");
        int num = 0xCAFEBABE;
        // latin small letter a with diaeresis, latin small letter a with ring above, latin small letter o with diaeresis
        const string utf8("\xC3\xA4 \xC3\xA5 \xC3\xB6 \xE2\x82\xAC"); // ä å ö €
        const wstring utf16(L"\u00E4 \u00E5 \u00F6 \u20AC");

        // Japanese Yen symbol, Roman AE with acute accent, Greek Capital Alpha, Greek Capital Omega,
        // Euro symbol, Rupee symbol, Cyrillic capital letter DZHE, Arabic letter TEH
        const wstring symbols = L"\u00a5\u01FC\u0391\u03A9\u20ac\u20a8\u040F\u062A";
        const string symbols_utf8 = wstringToUtf8(symbols);

        // devanagari letter a, tibetan digit zero, katakana letter ya
        const wstring letters = L"\u0905 \u0F20 \u30E4";
        const string letters_utf8_manually("\xE0\xA4\x85 \xE0\xBC\xA0 \xE3\x83\xA4");
        const string letters_utf8 = wstringToUtf8(letters);

        // array of all 8-bit values
        const int len = 0xff;
        char bytes[len];
        for (int i=0; i < len; i++)
        {
            bytes[i] = i;
        }

        string s, u, sym, let, sbytes;
        wstring w, wsym, wlet;
        int i, zero, one, max;
        aMessage >> s >> i >> zero >> one >> max >> u >> w >> wsym >> sym;
        aMessage >> let >> wlet; // read in 'wrong' order
        aMessage >> sbytes;

        const char* bytes_out = sbytes.data();

        if (str.compare(s)!=0 || num != i || zero != 0 || one != 1 || max != 0xFFFFFFFF ||
                utf8.compare(u)!=0 || utf16.compare(w)!=0 || symbols.compare(wsym)!=0 || symbols_utf8.compare(sym)!=0 ||
                letters_utf8.compare(let)!=0 || letters_utf8_manually.compare(let)!=0 || letters.compare(wlet)!=0 ||
                memcmp(bytes, bytes_out, len) != 0)
        {
            ELOG(EJavaComms, "Data does not match - test will fail !!!!!!!!!");
            mReply = aMessage;
            mReply.setReceiver(aMessage.getSender());
            mComms->send(mReply);
        }
        else
        {
            CommsMessage reply;
            reply.setReceiver(aMessage.getSender());
            reply.setMessageId(aMessage.getMessageId());
            reply.setMessageRef(aMessage.getMessageRef());

            const string input("ABCDEFGHIJKLMNOPQRSTYWXYZ");
            int j = 0xDEADBEEF;

            reply << input << j << 0 << 1 << (int)0xFFFFFFFF << letters_utf8_manually;
            reply << symbols << utf16 << utf8 << sbytes;
            mComms->send(reply);
        }
    }
    break;

    case MODULE_ID_NO_REPLY:
        break;

    case MODULE_ID_SLEEP_1S:
    case MODULE_ID_SLEEP_5S:
    case MODULE_ID_SLEEP_10S:
        usleep(1000*aMessage.getModuleId());
        // fall through
#ifdef __SYMBIAN32__
        break;
#endif
    case MODULE_ID_DELAY_REPLY:
    {
        if (mDelayedReplyCount == 0)
        {
            mDelayedReply = aMessage;
            mDelayedReplyCount++;
        }
        else
        {
            // reply to first message
            CommsMessage reply;
            reply.replyTo(mDelayedReply);
            mComms->send(reply);

            // reply to message client
            reply.replyTo(aMessage);
            mComms->send(reply);
            mDelayedReplyCount = 0;
        }
    }
    break;

    default:
        mReply = aMessage;
        mReply.setReceiver(aMessage.getSender());
        mComms->send(mReply);
        break;
    }
}


} // namespace comms
} // namespace java


