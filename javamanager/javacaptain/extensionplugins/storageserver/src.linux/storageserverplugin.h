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
* Description:  StorageServerHandler
*
*/

#ifndef STORAGESERVERPLUGIN_H
#define STORAGESERVERPLUGIN_H

#include "javaosheaders.h"

#include "extensionplugininterface.h"
#include "processmanagementeventsinterface.h"

using namespace java::captain;

namespace java
{
namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(StorageServerPlugin) :  public ExtensionPluginInterface,
        public ProcessManagementEventsInterface
{
public:
    StorageServerPlugin();
    virtual ~StorageServerPlugin();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // ExtensionPluginInterface
    virtual ProcessManagementEventsInterface* getProcessManagementListener()
    {
        return this;
    }

    // ProcessManagementEventsInterface
    virtual void pmcTerminated(const int& pid, const int& exitCode);

private:
    CoreInterface* mCore;
    int mJavaStoragePid;
};

} // namespace captain
} // namespace java

#endif // STORAGESERVERPLUGIN_H

