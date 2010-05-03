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

#ifndef COMMSEXTENSIONPLUGIN_H
#define COMMSEXTENSIONPLUGIN_H

#include "javaosheaders.h"
#include "commslistener.h"
#include "commsmessage.h"
#include "commsserverendpoint.h"
#include "extensionplugininterface.h"

namespace java
{
namespace comms
{

using namespace java::captain;

OS_NONSHARABLE_CLASS(CommsExtensionPlugin) : public ExtensionPluginInterface,
        public CommsListener
{
public:
    CommsExtensionPlugin();
    virtual ~CommsExtensionPlugin();

    // ExtensionPluginInterface methods
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();
    virtual CommsListener* getCommsListener()
    {
        return this;
    }

    // CommsListener
    virtual void processMessage(CommsMessage& message);

private:
    CoreInterface*  mCore;
    CommsEndpoint* mComms;
    CommsMessage mReply;
};

} // namespace comms
} // namespace java

#endif // COMMSEXTENSIONPLUGIN_H

