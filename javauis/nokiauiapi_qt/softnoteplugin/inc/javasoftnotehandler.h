/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef JAVASOFTNOTEHANDLER_H
#define JAVASOFTNOTEHANDLER_H

#include <extensionplugininterface.h>
#include <eventconsumerinterface.h>
#include <applicationmanagementeventsinterface.h>

using namespace java::captain;

OS_NONSHARABLE_CLASS(javasoftnotehandler) :  public ExtensionPluginInterface,
        public EventConsumerInterface,
        public ApplicationManagementEventsInterface
{
public:
    OS_IMPORT javasoftnotehandler();
    OS_IMPORT virtual ~javasoftnotehandler();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();
  
    // ExtensionPluginInterface methods
    virtual EventConsumerInterface* getEventConsumer();
    virtual ApplicationManagementEventsInterface* getApplicationManagementListener();
  
    // EventConsumerInterface
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg);

    // ApplicationManagementEventsInterface
    virtual void amAdded(const uids_t& uids);
    virtual void amUpdated(const uids_t& uids);
    virtual void amDeleted(const uids_t& uids);

private:
    void createIndicatorsL();
    void deleteIndicatorsL(const uids_t& aUids);
};

#endif // JAVASOFTNOTEHANDLER_H

