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

#include <string>

#include "logger.h"
#include "coreinterface.h"
#include "comms.h"
#include "commsmessage.h"
#include "commsendpoint.h"

#include "rmsextensionplugin.h"

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::rms::RmsExtensionPlugin();
}

using namespace java::rms;

RmsExtensionPlugin::RmsExtensionPlugin() : mCore(0)
{
}

RmsExtensionPlugin::~RmsExtensionPlugin()
{
}

void RmsExtensionPlugin::startPlugin(CoreInterface* aCore)
{
    LOG(EMidpRms, EInfo, "RmsExtensionPlugin::startPlugin()");
    mCore = aCore;
    mCore->getComms()->registerListener(PLUGIN_ID_RMS_C, this);
}

void RmsExtensionPlugin::stopPlugin()
{
    LOG(EMidpRms, EInfo, "RmsExtensionPlugin::stopPlugin()");
    mCore->getComms()->unregisterListener(PLUGIN_ID_RMS_C, this);
    mCore = 0;
    mListeners.clear();
}

CommsListener* RmsExtensionPlugin::getCommsListener()
{
    return this;
}

void RmsExtensionPlugin::processMessage(CommsMessage& aMessage)
{
    JELOG2(EMidpRms);

    switch (aMessage.getMessageId())
    {
    case MSG_ID_RECORD_CHANGE:
    {
        std::string recordStore;
        aMessage >> recordStore;
        notifyListeners(recordStore, aMessage);
        break;
    }
    case MSG_ID_LISTENER_CHANGE:
    {
        std::string recordStore;
        int operation = 0;
        aMessage >> recordStore >> operation;
        if (operation == ADD_LISTENER)
        {
            addListener(recordStore, aMessage.getSender());
        }
        else
        {
            removeListener(recordStore, aMessage.getSender());
        }
        sendListenerCount(recordStore, aMessage);
        break;
    }
    default:
        WLOG1(EMidpRms, "processMessage() unknown message received, msgId:%d", aMessage.getMessageId());
        break;
    }
}

void RmsExtensionPlugin::addListener(std::string aName, int aAddress)
{
    LOG2(EMidpRms, EInfo, "addListener(%s, %d)", aName.c_str(), aAddress);
    mListeners[aName].push_back(aAddress);
}

void RmsExtensionPlugin::removeListener(std::string aName, int aAddress)
{
    listenersIterator iter = mListeners.find(aName);
    if (iter != mListeners.end())
    {
        for (std::vector<int>::iterator it = iter->second.begin(); it < iter->second.end(); it++)
        {
            if (aAddress == *it)
            {
                LOG3(EMidpRms, EInfo, "removeListener(%s, %d, left=%d)",
                     aName.c_str(), aAddress, iter->second.size()-1);
                iter->second.erase(it);
                break;
            }
        }
        if (iter->second.size() == 0)
        {
            mListeners.erase(iter);
        }
    }
}

void RmsExtensionPlugin::notifyListeners(std::string aName, CommsMessage& aMessage)
{
    listenersIterator iter = mListeners.find(aName);
    if (iter != mListeners.end())
    {
        CommsMessage notifyMsg = aMessage;
        notifyMsg.setMessageRef(0);
        for (std::vector<int>::iterator it = iter->second.begin(); it < iter->second.end();)
        {
            if (aMessage.getSender() != *it)
            {
                LOG2(EMidpRms, EInfo, "notifyListeners(%s, %d)", aName.c_str(), *it);
                notifyMsg.setReceiver(*it);
                int rc = send(notifyMsg);
                if (rc != 0)
                {
                    ELOG2(EMidpRms, "notifyListeners() failed (%s, %d)", aName.c_str(), *it);
                    it = iter->second.erase(it);
                    continue;
                }
            }
            it++;
        }
        if (iter->second.size() == 0)
        {
            mListeners.erase(iter);
        }
    }
}

void RmsExtensionPlugin::sendListenerCount(std::string aName, CommsMessage& aSender)
{
    listenersIterator iter = mListeners.find(aName);
    if (iter != mListeners.end())
    {
        CommsMessage msg = createListenerCountMsg(aName, iter->second.size());
        LOG2(EMidpRms, EInfo, "sendListenerCount(%s, %d)", aName.c_str(), iter->second.size());
        for (std::vector<int>::iterator it = iter->second.begin(); it < iter->second.end(); it++)
        {
            msg.setReceiver(*it);
            if (msg.getReceiver() == aSender.getSender())
            {
                // ensures that sender can use comms.sendReceive()
                msg.replyTo(aSender);
                msg.setMessageId(MSG_ID_LISTENER_COUNT);
            }
            send(msg);
        }
    }
}

CommsMessage RmsExtensionPlugin::createListenerCountMsg(std::string aName, int aCount)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_RMS_C);
    msg.setMessageId(MSG_ID_LISTENER_COUNT);
    msg << aName << aCount;
    return msg;
}


int RmsExtensionPlugin::send(CommsMessage& aMessage)
{
    int rc = mCore->getComms()->send(aMessage);
    return rc;
}
