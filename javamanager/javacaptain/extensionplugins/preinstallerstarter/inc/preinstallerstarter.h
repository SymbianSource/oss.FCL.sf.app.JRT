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
* Description:  PreinstallerStarter is Java Captain S60 plugin that starts
*                preinstaller process when the device boots or removable drive
*                is added to the device.
*
*/

#ifndef PREINSTALLERSTARTER_H
#define PREINSTALLERSTARTER_H

#include "javaosheaders.h"

#include "eventconsumerinterface.h"
#include "extensionplugininterface.h"

namespace java
{

namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(PreinstallerStarter) :  public CActive,
        public EventConsumerInterface,
        public ExtensionPluginInterface
{
public:
    PreinstallerStarter();
    virtual ~PreinstallerStarter();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // EventConsumerInterface
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg);

    // ExtensionPluginInterface methods
    virtual EventConsumerInterface* getEventConsumer();

protected:
    // CActive
    virtual void RunL();
    virtual void DoCancel();

private:
    void startPreinstaller(TBool aIadBoot);
    void registerMidletApplicationTypeHandler();

    CoreInterface* mCore;
    RProcess*      mPreinstaller;
};

} // namespace captain
} // namespace java

#endif // PREINSTALLERSTARTER_H
