/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.mj.impl.coreuiavkon;

import java.util.Hashtable;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

import com.nokia.mj.impl.coreui.CoreUi;


/**
 * A gate to CoreUi Avkon implementation.
 */
public class CoreUiImpl extends CoreUi
{
    public static final int OrientationUndefined = 0;
    public static final int OrientationPortrait  = 1;
    public static final int OrientationLandscape = 2;

    public static final int ScreenModeNoStartScreen       = 0;
    public static final int ScreenModeDefaultStartScreen  = 1;
    public static final int ScreenModeMidletDefinedScreen = 2;

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
        return _connect();
    }

    /**
     * For creating the UI from Java side. This is meant for the pre-warmed
     * VM use case. Calling this method will lead creation of the CoreUI.
     * @param uid The UID of the application.
     * @param backGroundStart Should the UI be put into background.
     */
    protected void createUiImpl(Uid uid, boolean backGroundStart)
    {
        int selectedOrientation = OrientationUndefined;
        int selectedScreenMode  = ScreenModeDefaultStartScreen;

        ApplicationInfo appInfo = ApplicationInfo.getInstance();

        String rootPath = appInfo.getRootPath();

        String orientation = appInfo.getAttribute("Nokia-MIDlet-App-Orientation");
        if (orientation != null)
        {
            if (orientation.toLowerCase().equals("portrait"))
            {
                selectedOrientation = OrientationPortrait;
            }
            else if (orientation.toLowerCase().equals("landscape"))
            {
                selectedOrientation = OrientationLandscape;
            }
        }

        String splashScreenImg = appInfo.getAttribute("Nokia-MIDlet-Splash-Screen-Image");
        if (splashScreenImg != null)
        {
            if (splashScreenImg.toLowerCase().equals("suppress"))
            {
                selectedScreenMode = ScreenModeNoStartScreen;
                backGroundStart = true;
            }
            else
            {
                selectedScreenMode = ScreenModeMidletDefinedScreen;
            }
        }
        if (!_createUi(uid.toString(), selectedOrientation, selectedScreenMode,
                       rootPath, backGroundStart))
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo, "JAVA createUiImpl Error");
            throw new RuntimeException("Core UI creation failed!");
        }
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
        _toForeground();
    }

    /**
     * For asking if the UI is in foreground.
     */
    protected boolean isUiInForegroundImpl()
    {
        return _isForeground();
    }

    private native boolean _connect();
    private native boolean _createUi(String appUid, int orientation,
                                     int selectedScreenMode, String rootpath,
                                     boolean backGroundStart);
    private native void _toForeground();
    private native boolean _isForeground();
}
