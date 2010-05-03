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

#include "certstoretestextensionplugin.h"

using namespace std;
using namespace java::comms;

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::security::CertStoreTestExtensionPlugin();
}

namespace java
{
namespace security
{
CertStoreTestExtensionPlugin::CertStoreTestExtensionPlugin() : mCore(0), mComms(0)
{
    JELOG2(EJavaSecurity);
}

CertStoreTestExtensionPlugin::~CertStoreTestExtensionPlugin()
{
    JELOG2(EJavaSecurity);
}

void CertStoreTestExtensionPlugin::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaSecurity);
    mCore = core;
    mComms = core->getComms();

    mComms->registerListener(PLUGIN_ID_CERT_STORE_PLUGIN_TESTER_C, this);
}

void CertStoreTestExtensionPlugin::stopPlugin()
{
    JELOG2(EJavaSecurity);
    mComms->unregisterListener(PLUGIN_ID_CERT_STORE_PLUGIN_TESTER_C, this);
    mCore = 0;
    mComms = 0;
}

void CertStoreTestExtensionPlugin::processMessage(CommsMessage& aMessage)
{
    JELOG2(EJavaSecurity);
    switch (aMessage.getMessageId())
    {
    case IPC_START_JAVA_CERT_STORE_EXT_PLUGIN:
    {
        mCore->loadExtensionPlugin("ondemand_7");
        break;
    }
    case IPC_STOP_JAVA_CERT_STORE_EXT_PLUGIN:
    {
        mCore->unloadExtensionPlugin("ondemand_7");
        break;
    }
    default:
        ELOG1(EJavaPush, "ERROR!!! Unsupported method arrives to processMessage(): %d",
              aMessage.getMessageId());
    }//end switch
}


} // namespace security
} // namespace java


