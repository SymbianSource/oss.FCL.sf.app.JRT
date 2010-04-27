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

#ifndef STORAGESERVERPLUGIN_H
#define STORAGESERVERPLUGIN_H

#include "javaosheaders.h"

#include "extensionplugininterface.h"
#include "eventconsumerinterface.h"

using namespace java::storage;
using namespace std;

namespace java
{
namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(StorageServerPlugin) : public ExtensionPluginInterface,
        public EventConsumerInterface
{
public:
    StorageServerPlugin();
    virtual ~StorageServerPlugin();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // ExtensionPluginInterface methods
    virtual EventConsumerInterface* getEventConsumer();

    // EventConsumerInterface
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg);

private:
    CoreInterface* mCore;
};

} // namespace captain
} // namespace java

#endif // STORAGESERVERPLUGIN_H

