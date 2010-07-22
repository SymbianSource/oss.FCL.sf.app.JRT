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
* Description:  ExtensionPluginInterface
*
*/

#ifndef SETTINGSLISTENERINTERFACE_H
#define SETTINGSLISTENERINTERFACE_H

namespace java
{

namespace captain
{

class CoreInterface;

class SettingsListenerInterface
{
public:
    virtual ~SettingsListenerInterface() {}

    virtual void StartListening() = 0;
    virtual void StopListening() = 0;

};

} // namespace captain
} // namespace java

#endif // SETTINGSLISTENERINTERFACE_H

