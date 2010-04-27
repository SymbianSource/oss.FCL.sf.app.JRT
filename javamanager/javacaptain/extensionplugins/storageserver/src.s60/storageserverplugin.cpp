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
* Description:  StorageServerPlugin
*
*/

#include <memory>

#include "logger.h"
#include "coreinterface.h"
#include "javastorage.h"
#include "javastoragenames.h"

#include "storageserverplugin.h"


#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::captain::StorageServerPlugin();
}

using namespace java::comms;

namespace java
{
namespace captain
{

StorageServerPlugin::StorageServerPlugin():mCore(0)
{
    JELOG2(EJavaStorage);
}

StorageServerPlugin::~StorageServerPlugin()
{
    JELOG2(EJavaStorage);
}

void StorageServerPlugin::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaStorage);
    mCore = core;

    // This is for the first boot to create storage structures.
    try
    {
        std::auto_ptr<java::storage::JavaStorage> js(
            java::storage::JavaStorage::createInstance());
        js->open();
        js->close();
        js->open(JAVA_OTA_DATABASE_NAME);
        js->close();
        js.reset(0);
    }
    catch (java::storage::JavaStorageException& aJse)
    {
        ELOG1(EJavaStorage, "Storage init failed: %d", aJse.mStatus);
    }
}

void StorageServerPlugin::stopPlugin()
{
    JELOG2(EJavaStorage);
    mCore = 0;
}

EventConsumerInterface* StorageServerPlugin::getEventConsumer()
{
    JELOG2(EJavaStorage);
    return this;
}

void StorageServerPlugin::event(const std::string& /*aEventProvider*/,
                                java::comms::CommsMessage& /*aMsg*/)
{
    JELOG2(EJavaStorage);
}

} // namespace captain
} // namespace java

