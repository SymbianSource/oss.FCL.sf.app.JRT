/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.media.control;

/**
 * This interface provides extension point for GUIControl.USE_GUI_PRIMITIVE
 * display mode.
 */
public interface MMAGUIFactory
{
    /**
     * Initializes display mode and returns Object that can be used with
     * GUI.
     * @return Associated GUI object
     */
    Object initDisplayMode();

    /**
     * MMA will set content for container with this method after display
     * mode is initialized.
     * @param aHandle MMMADirectContent native handle.
     * @throw NullPointerException if initDisplayMode has not been called.
     */
    void setContentHandle(int aHandle);
}
