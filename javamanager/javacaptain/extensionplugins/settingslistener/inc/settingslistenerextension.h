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
* Description:  Extension plugin which is used for monitoring changes in various
*                     system settings.
*
*/

#ifndef SETTINGSLISTENEREXTENSION_H
#define SETTINGSLISTENEREXTENSION_H

#include "javaosheaders.h"

#include "extensionplugininterface.h"
#include "settingslistenerinterface.h"

namespace java
{
namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(SettingsListenerExtension) :  public ExtensionPluginInterface
{
public:
    SettingsListenerExtension();
    virtual ~SettingsListenerExtension();

    // ExtensionPluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

protected:

private:
    CoreInterface* mCore; // resource not owned
    SettingsListenerInterface* mSettingsListenerImpl; // resource owned
};

} // namespace captain
} // namespace java

#endif // SETTINGSLISTENEREXTENSION_H

