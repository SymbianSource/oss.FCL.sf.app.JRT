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
* Description:
*
*/


#ifndef BTDEVICECLASSMANAGER_H
#define BTDEVICECLASSMANAGER_H

#include "javaosheaders.h"
#include "commslistener.h"
#include "extensionplugininterface.h"
#include "eventconsumerinterface.h"
#include "applicationmanagementeventsinterface.h"
#include "applicationruntimeeventsinterface.h"
#include "serviceclasslisthandler.h"

namespace java
{
namespace bluetooth
{



OS_NONSHARABLE_CLASS(BtDeviceClassManager) : public java::captain::ExtensionPluginInterface,
        public java::comms::CommsListener
{
public:
    BtDeviceClassManager();
    virtual ~ BtDeviceClassManager();

    // PluginInterface
    virtual void startPlugin(java::captain::CoreInterface* aCore);
    virtual void stopPlugin();
    // CommsListener
    virtual java::comms::CommsListener* getCommsListener();

protected:
    // CommsListener methods
    virtual void processMessage(CommsMessage& message);
    int setServiceClassBits(unsigned int aDeviceServiceClass);
    unsigned int getServiceClassBits();

private:
    java::captain::CoreInterface* mCore;
    ServiceClassListHandler mServiceListHandler;
    TUint32 mInitialServiceClassBits;
};

} // namespace bluetooth
} // namespace java

#endif // BTDEVICECLASSMANAGER_H

