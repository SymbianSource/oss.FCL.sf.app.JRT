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
* Description:  ConfigExtensionPlugin
*
*/

#ifndef CONFIGEXTENSIONPLUGIN_H
#define CONFIGEXTENSIONPLUGIN_H

#include "javaosheaders.h"
#include "extensionplugininterface.h"

namespace java
{
namespace comms
{
class CommsMessage;
}

namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(ConfigExtensionPlugin) :  public ExtensionPluginInterface
{
public:
    ConfigExtensionPlugin();
    virtual ~ConfigExtensionPlugin();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

private:
    void loadExtensionPlugins();

    CoreInterface* mCore;
};

} // namespace captain
} // namespace java

#endif // CONFIGEXTENSIONPLUGIN_H

