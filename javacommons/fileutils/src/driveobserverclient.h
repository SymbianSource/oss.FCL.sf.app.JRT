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
* Description:  DriveObserverClient
*
*/

#ifndef DRIVEOBSERVERCLIENT_H
#define DRIVEOBSERVERCLIENT_H

#include <set>

#include "javaosheaders.h"
#include "commslistener.h"

namespace java
{
namespace comms
{
class CommsClientEndpoint;
}

namespace fileutils
{

using java::comms::CommsListener;
using java::comms::CommsMessage;

class DriveListenerInterface;

OS_NONSHARABLE_CLASS(DriveObserverClient) : public CommsListener
{
public:
    DriveObserverClient();
    virtual ~DriveObserverClient();

    int registerListener(DriveListenerInterface* aListener,
                         const int& aServerCommsAddress);
    int unregisterListener(DriveListenerInterface* aListener,
                           const int& aServerCommsAddress);

private:
    void enableEvents(const int& aServerCommsAddress);
    void disableEvents();

    // CommsListener
    virtual void processMessage(CommsMessage& aMessage);

    std::set<DriveListenerInterface*>   mListeners;

    bool                               mEventsSubscribed;
    java::comms::CommsClientEndpoint*  mComms;
    bool                               mCommsOwner;
};

} // end of namespace fileutils
} // end of namespace java

#endif // DRIVEOBSERVERCLIENT_H
