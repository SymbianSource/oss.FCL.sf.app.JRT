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
* Description:  MmcEventProvider
*
*/

#ifndef MMCEVENTPROVIDER_H
#define MMCEVENTPROVIDER_H

#include "javaosheaders.h"
#include "driveutilities.h"

#include "extensionplugininterface.h"

namespace java
{
namespace captain
{

using namespace java::fileutils;

class CoreInterface;
class EventConsumerInterface;

OS_NONSHARABLE_CLASS(MmcEventProvider) : public ExtensionPluginInterface,
        public DriveListenerInterface
{
public:
    MmcEventProvider();
    virtual ~MmcEventProvider();

    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // DriveListenerInterface
    virtual void driveChanged(const int& aOperation, const driveInfo& aDriveInfo);

private:
    CoreInterface*                  mCore;
    DriveObserverServerInterface*   mDOServer;
};

} // namespace captain
} // namespace java

#endif // MMCEVENTPROVIDER_H
