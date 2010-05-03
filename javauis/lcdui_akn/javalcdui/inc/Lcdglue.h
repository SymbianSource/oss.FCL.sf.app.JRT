/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef LCDGLUE_H
#define LCDGLUE_H

#include <lcdui.h>
#include <lcdgr.h>

/**
 * MACRO determines whether transitional classes and methods are included
 * in this api. The transitional classes and methods work around the problem
 * of routing application level events though the MMIDEnv back to MMIDEnvObservers
 * without introducing a static dependency of the VM onto the ui plugin DLL.
 * Modifications to LCDUI.H and changes to the ui plugin would be required to
 * allow the plugin to cascade sytem level events itself without the assistance
 * of MMIDEnv.
 */
#define LCDUI_TRANSITIONAL_API

#ifdef LCDUI_TRANSITIONAL_API
/**
 * Compatibility interface - appui observer allows appui to routes
 * application level events through MMIDEnv to MMIDEnvObservers.
 *@deprecated
 */
class MMIDObserver
{
public:
    /**
     * Called by appui when system or user requests application exit.
     */
    virtual void HandleExitL()=0;
    /**
     * Called by appui when application gains or loses focus
     */
    virtual void HandleForegroundL(TBool aForeground)=0;
    /**
     * Called by appui in response to system event
     */
    virtual void HandleSwitchOnEventL()=0;
    /**
     * Called by appui in response to CONE resource change.
     */
    virtual void HandleResourceChangeL(TInt aType)=0;
};
#endif

class MLcduiPlugin
{
public:
    virtual MMIDComponentFactory* CreateComponentFactoryL() = 0;
#ifdef LCDUI_TRANSITIONAL_API
    virtual void SetObserver(MMIDObserver* aObserver) = 0;
#endif
};

/**
 * Glue interface between VM and UI plugin. Allows UI plugin DLL to attach and detach
 * from JVM. Whilst attached the JVM may execute functions on the UI thread.
 */
class MLcdui
{
public:
    virtual void AttachL(MLcduiPlugin* aPlugin) = 0;
    virtual void Detach() = 0;
    virtual MLcduiPlugin* Plugin() = 0;
};

#endif // LCDGLUE_H
