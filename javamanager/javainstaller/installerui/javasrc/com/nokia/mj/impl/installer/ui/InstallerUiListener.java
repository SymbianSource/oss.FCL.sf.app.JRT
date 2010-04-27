/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.ui;

/**
 * JavaInstaller user interface listener interface.
 */
public interface InstallerUiListener
{
    /**
     * Called when user cancels installation or uninstallation.
     */
    public void cancel();

    /**
     * Called just before InstallerUi is about to be opened.
     * This call can be used to hide the "preparing installation"
     * indicator.
     */
    public void uiIsReady();
}
