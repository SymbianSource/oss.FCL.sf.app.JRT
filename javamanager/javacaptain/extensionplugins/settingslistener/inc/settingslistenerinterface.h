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
* Description:  Definition of interface, which is used for listening changes in
*                     in system settings.
*
*/

#ifndef SETTINGSLISTENERINTERFACE_H
#define SETTINGSLISTENERINTERFACE_H

namespace java
{

namespace captain
{

class SettingsListenerInterface
{
public:
    virtual ~SettingsListenerInterface() {}

    virtual void startListening() = 0;
    virtual void stopListening() = 0;

};

} // namespace captain
} // namespace java

#endif // SETTINGSLISTENERINTERFACE_H

