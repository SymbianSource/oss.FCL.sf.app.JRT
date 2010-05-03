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
* Description:  BootEventProvider
*
*/

#ifndef BOOTEVENTPROVIDER_H
#define BOOTEVENTPROVIDER_H

#include "extensionplugininterface.h"

namespace java
{
namespace captain
{

class CoreInterface;
class EventConsumerInterface;

class BootEventProvider : public ExtensionPluginInterface
{
public:
    BootEventProvider();
    virtual ~BootEventProvider();

    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

private:
    bool isFirstDeviceBoot() const;
    bool isIadBoot() const;
    void setBootFlag(const std::string& aPath, bool aCreate) const;

    void dispatchEvent(const std::string& aEvent, int aType);

    bool fileExists(const std::string& aPath) const;
    void createFile(const std::string& aPath) const;
    void removeFile(const std::string& aPath) const;

    CoreInterface* mCore;
};

} // namespace captain
} // namespace java

#endif // BOOTEVENTPROVIDER_H

