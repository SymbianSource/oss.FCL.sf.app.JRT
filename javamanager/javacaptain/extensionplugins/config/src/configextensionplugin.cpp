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


#include "logger.h"
#include "booteventprovidermessages.h"

#include "coreinterface.h"

#include "configextensionplugin.h"

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::captain::ConfigExtensionPlugin();
}

namespace java
{
namespace captain
{

ConfigExtensionPlugin::ConfigExtensionPlugin() : mCore(0)
{
    JELOG2(EJavaCaptain);
}

ConfigExtensionPlugin::~ConfigExtensionPlugin()
{
    JELOG2(EJavaCaptain);
}

void ConfigExtensionPlugin::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaCaptain);
    mCore = core;
    loadExtensionPlugins();
}

void ConfigExtensionPlugin::stopPlugin()
{
    JELOG2(EJavaCaptain);
    mCore = 0;
}

void ConfigExtensionPlugin::loadExtensionPlugins()
{
    JELOG2(EJavaCaptain);
    mCore->loadExtensionPlugin("storageserverplugin");
    mCore->loadExtensionPlugin("pushregistryplugin");
#ifdef __SYMBIAN32__
    // This S60 only extension plugin starts S60 preinstaller process
    // when the device boots
    mCore->loadExtensionPlugin("preinstallerstarter");
    mCore->loadExtensionPlugin("btdeviceclassmanager");
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    mCore->loadExtensionPlugin("scrupdater");
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#endif // __SYMBIAN32__
    mCore->loadExtensionPlugin("boot");
    mCore->loadExtensionPlugin("mmc");
}

} // namespace captain
} // namespace java

