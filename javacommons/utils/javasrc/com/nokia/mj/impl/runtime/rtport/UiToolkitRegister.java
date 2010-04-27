/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.runtime.rtport;

/**
 * Common interface to UI Toolkit Register implementations.
 */
public interface UiToolkitRegister
{
    /**
     * Registers given UI toolkit for current application.
     *
     * @param toolkitId ID of the toolkit
     * @throws RuntimeException if given toolkit cannot be registered
     *         for current application.
     */
    public void registerUiToolkit(String toolkitId);

    /**
     * Clears the registered UI toolkit associated with current application.
     *
     * After this it is possible to register another UI toolkit.
     * It is OK to call this even if a toolkit has not been registered.
     *
     * @param toolkitId ID of the toolkit to unregister
     */
    public void unregisterUiToolkit(String toolkitId);

    /**
     * Provides the names of the registered allowed toolkits.
     *
     * @return IDs of the allowed UI toolkits, or empty array if no toolkit
     *         has not yet been registered.
     */
    public String[] getRegisteredUiToolkits();

    /**
     * Returns true if UI toolkit is foreground.
     *
     * @return true if toolkit is foreground. If toolkit has not set the value,
     * false is returned.
     */
    public boolean isToolkitForeground();

    /**
     * Changes the value to be returned from isToolkitForeground.
     *
     * @param foreground true if switched to foreground
     */
    public void setToolkitForeground(boolean foreground);
}
