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
* Description:
*
*/


package com.nokia.mj.impl.rt.legacy;

public interface ToolkitObserverNGAExtension extends ToolkitObserver
{
    /**
     * Notification about MIDlet going to background or coming foreground.
     * @param foreground    true, when MIDlet came to foreground
     *                      false, when MIDlet was sent to background
     */
    void foregroundEvent(boolean foreground);
}