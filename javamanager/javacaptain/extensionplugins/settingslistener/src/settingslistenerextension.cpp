/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for creating system settings listener plugin
*
*/

#include "logger.h"
#include "settingslistenerextension.h"
#include "settingslistenerimpl.h"

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::captain::SettingsListenerExtension();
}

namespace java
{
namespace captain
{

SettingsListenerExtension::SettingsListenerExtension() : mCore(0), mSettingsListenerImpl(0)
{
}

SettingsListenerExtension::~SettingsListenerExtension()
{
}

void SettingsListenerExtension::startPlugin(CoreInterface* core)
{
    if (mSettingsListenerImpl || mCore)
    {
        WLOG(EJavaCaptain, "Settingslistener plugin already running, terminating previous instance first!");
        stopPlugin();
    }
    mCore = core;
    mSettingsListenerImpl = new SettingsListenerImpl(mCore);
    mSettingsListenerImpl->startListening();
}

void SettingsListenerExtension::stopPlugin()
{
    mCore = 0;
    if (mSettingsListenerImpl)
    {
        mSettingsListenerImpl->stopListening();
        delete mSettingsListenerImpl;
        mSettingsListenerImpl = 0;
    }
}


} // namespace captain
} // namespace java

