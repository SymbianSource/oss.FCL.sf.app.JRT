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
* Description:  Sample
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
    JELOG2(EJavaCaptain);
    ILOG(EJavaCaptain, "SettingsListenerExtension::SettingsListenerExtension()");
}

SettingsListenerExtension::~SettingsListenerExtension()
{
    JELOG2(EJavaCaptain);
    ILOG(EJavaCaptain, "SettingsListenerExtension::~SettingsListenerExtension()");
}

void SettingsListenerExtension::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaCaptain);
    ILOG(EJavaCaptain, "SettingsListenerExtension::startPlugin()");
    if ( mSettingsListenerImpl || mCore )
    {
        WLOG(EJavaCaptain, "Settingslistener plugin already running, terminating previous instance first!");
        stopPlugin();
    }
    mCore = core;
    mSettingsListenerImpl = new SettingsListenerImpl(mCore);
    mSettingsListenerImpl->StartListening();
}

void SettingsListenerExtension::stopPlugin()
{
    JELOG2(EJavaCaptain);
    ILOG(EJavaCaptain, "SettingsListenerExtension::stopPlugin()");
    mCore = 0;
    if ( mSettingsListenerImpl )
    {
        mSettingsListenerImpl->StopListening();
        delete mSettingsListenerImpl;
        mSettingsListenerImpl = 0;
    }
}


} // namespace captain
} // namespace java

