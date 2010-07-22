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
* Description:  autostarter is Java Captain Symbian plugin that starts
*               auto start Java applications after they have been installed
*               and when the device boots.
*
*/

#ifndef AUTOSTARTER_H
#define AUTOSTARTER_H

#include "javaosheaders.h"

#include "eventconsumerinterface.h"
#include "extensionplugininterface.h"

namespace java
{

namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(AutoStarter) :  public EventConsumerInterface,
        public ExtensionPluginInterface
{
public:
    AutoStarter();
    virtual ~AutoStarter();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // EventConsumerInterface
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg);

    // ExtensionPluginInterface methods
    virtual EventConsumerInterface* getEventConsumer();

private:
    bool isMIDletPresent(const std::wstring& aUid);
    bool startMIDletL(const std::wstring& aUid);
    void checkMIDletsToBeStartedL();

    CoreInterface* mCore;
};

} // namespace captain
} // namespace java

#endif // AUTOSTARTER_H
