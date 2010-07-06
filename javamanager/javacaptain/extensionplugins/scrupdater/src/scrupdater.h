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
* Description:  ScrUpdater is Java Captain Symbian plugin that updates
*               presence information of Java Applications in USIF SCR
*               when removable drive is added or removed to the device.
*
*/

#ifndef SCRUPDATER_H
#define SCRUPDATER_H

#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>

#include "javaosheaders.h"

#include "eventconsumerinterface.h"
#include "extensionplugininterface.h"

namespace java  // codescanner::namespace
{

namespace captain  // codescanner::namespace
{

class CoreInterface;

OS_NONSHARABLE_CLASS(ScrUpdater) :  public EventConsumerInterface,
        public ExtensionPluginInterface
{
public:
    ScrUpdater();
    virtual ~ScrUpdater();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // EventConsumerInterface
    virtual void event(const std::string& aEventProvider,
                       java::comms::CommsMessage& aMsg);

    // ExtensionPluginInterface methods
    virtual EventConsumerInterface* getEventConsumer();

private:
    void removeScrPresencesL(driveInfo *aInfo);
    void addScrPresencesL(driveInfo *aInfo);
    void initializeScrPresenceInfoL();

    Usif::RSoftwareComponentRegistry *createScrL();
};

} // namespace captain
} // namespace java

#endif // SCRUPDATER_H
