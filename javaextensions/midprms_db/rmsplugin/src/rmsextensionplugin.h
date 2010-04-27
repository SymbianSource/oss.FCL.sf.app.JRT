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
* Description: Java Captain extension plugin for RMS
*
*/

#ifndef RMSEXTENSIONPLUGIN_H
#define RMSEXTENSIONPLUGIN_H

#include <map>
#include <vector>

#include "javaosheaders.h"
#include "commslistener.h"
#include "extensionplugininterface.h"

namespace java
{
namespace rms
{
const int MSG_ID_RECORD_CHANGE = 1;
const int MSG_ID_LISTENER_CHANGE = 2;
const int MSG_ID_LISTENER_COUNT = 3;

const int ADD_LISTENER = 0;
const int REMOVE_LISTENER = 1;

using namespace java::captain;
using namespace java::comms;

OS_NONSHARABLE_CLASS(RmsExtensionPlugin) : public ExtensionPluginInterface, public CommsListener
{
public:
    RmsExtensionPlugin();
    virtual ~RmsExtensionPlugin();

    // ExtensionPluginInterface methods
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();
    virtual CommsListener* getCommsListener();

    // CommsListener
    virtual void processMessage(CommsMessage& aMessage);

protected:
    virtual void addListener(std::string aName, int aAddress);
    virtual void removeListener(std::string aName, int aAddress);
    virtual void notifyListeners(std::string aName, CommsMessage& aMessage);
    virtual void sendListenerCount(std::string aName, CommsMessage& aSender);
    virtual CommsMessage createListenerCountMsg(std::string aName, int aCount);
    virtual int send(CommsMessage& aMessage);

    CoreInterface* mCore;

    typedef std::map< std::string, std::vector<int> > listeners_t;
    typedef listeners_t::iterator listenersIterator;
    listeners_t mListeners;
};

} // namespace rms
} // namespace java

#endif // RMSEXTENSIONPLUGIN_H

