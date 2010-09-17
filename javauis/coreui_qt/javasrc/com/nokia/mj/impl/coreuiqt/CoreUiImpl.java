/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java side Core UI accessor.
*
*/
package com.nokia.mj.impl.coreuiqt;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.coreui.CoreUi;


/**
 * A gate to CoreUi Qt implementation.
 */
public class CoreUiImpl extends CoreUi
{

    public CoreUiImpl()
    {
        // Load the native.
        Jvm.loadSystemLibrary("javacoreui");
    }

    /**
     * Connects to the already created CoreUi.
     * @return permission to start the application. There is a small time
     *         window where user is able to cancel the application
     *         start. In this case the CoreUi stores the request and
     *         informs the caller of this method that the application
     *         should not be started.
     */
    protected boolean connectToUiImpl()
    {
        return true;
    }

    /**
     * For creating the UI from Java side. This is meant for the pre-warmed
     * VM use case. Calling this method will lead creation of the CoreUI.
     * @param uid The UID of the application.
     * @param backGroundStart Should the UI be put into background.
     */
    protected void createUiImpl(Uid uid, boolean backGroundStart)
    {
        // CoreUiStartScreen.showStartScreen(backGroundStart);
    }

    /**
     * For asking the runtime to do the shutdown of the application.
     */
    protected void shutdownRequestImpl()
    {
        ApplicationUtils.getInstance().notifyExitCmd();
    }

    /**
     * For asking the runtime to bring the application to foreground.
     */
    protected void foregroundRequestImpl()
    {
    }

    /**
     * For asking if the UI is in foreground.
     */
    protected boolean isUiInForegroundImpl()
    {
        return true;
    }

    /**
     * For asking if the UI to remove the icon from the task manager.
     */
    protected void hideApplicationImpl(boolean hide)
    {
        // Native logic is reversed between Symbian^3 and Symbian^4.
        // platform specific part.
        _hideApplication(!hide);
    }

    private native void _hideApplication(boolean hide);
}
