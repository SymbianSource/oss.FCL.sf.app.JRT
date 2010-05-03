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
* Description:  ?Description
*
*/

#ifndef CERTSTORETESTEXTENSIONPLUGIN_H
#define CERTSTORETESTEXTENSIONPLUGIN_H

#include "javaosheaders.h"
#include "commslistener.h"
#include "commsmessage.h"
#include "commsserverendpoint.h"
#include "extensionplugininterface.h"

namespace java
{
namespace security
{

const int IPC_START_JAVA_CERT_STORE_EXT_PLUGIN = 398;
const int IPC_STOP_JAVA_CERT_STORE_EXT_PLUGIN = 399;

using namespace java::captain;

OS_NONSHARABLE_CLASS(CertStoreTestExtensionPlugin) : public ExtensionPluginInterface,
        public java::comms::CommsListener
{
public:
    CertStoreTestExtensionPlugin();
    virtual ~CertStoreTestExtensionPlugin();

    // ExtensionPluginInterface methods
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();
    virtual java::comms::CommsListener* getCommsListener()
    {
        return this;
    }

    // CommsListener
    virtual void processMessage(java::comms::CommsMessage& message);

private:
    CoreInterface*  mCore;
    java::comms::CommsEndpoint* mComms;
};

} // namespace security
} // namespace java

#endif // CERTSTORETESTEXTENSIONPLUGIN_H

