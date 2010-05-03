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
* Description: Avkon CoreUI abstract class
*
*/

package com.nokia.mj.impl.coreui;

import java.util.Hashtable;

import com.nokia.mj.impl.utils.Uid;

/**
 * A factory type of class for accessing the CoreUi implementation.
 */
public abstract class CoreUi
{
    private static final String PACKAGE_END_PROPERTY_NAME = "com.nokia.coreui";

    private static final String PACKAGE_PREFIX = "com.nokia.mj.impl.";

    private static final String CORE_UI_CLASS = ".CoreUiImpl";

    private static CoreUi sInstance = null;

    static
    {
        //Get the system proptery defining the end of package.
        String packageEnd =
            System.getProperty(PACKAGE_END_PROPERTY_NAME);

        if (packageEnd != null)
        {
            //Construct the class
            String className = PACKAGE_PREFIX + packageEnd + CORE_UI_CLASS;
            try
            {
                Class clazz = Class.forName(className);
                sInstance = (CoreUi)clazz.newInstance();
            }
            catch (Exception e)
            {
                throw new RuntimeException("Not able to instantiate class " +
                                           className+". Reason is: " + e);
            }
        }
    }

    /**
     * Connects to the already created CoreUi.
     * @return permission to start the application. There is a small time
     *         window where user is able to cancel the application
     *         start. In this case the CoreUi stores the request and
     *         informs the caller of this method that the application
     *         should not be started.
     */
    public static boolean connectToUi()
    {
        boolean canBeStarted = true;
        if (sInstance != null)
        {
            canBeStarted = sInstance.connectToUiImpl();
        }
        return canBeStarted;
    }

    /**
     * For creating the UI from Java side. This is meant for the pre-warmed
     * VM use case. Calling this method will lead creation of the CoreUI.
     * @param uid The UID of the application.
     * @param backGroundStart Should the UI be put into background.
     */
    public static void createUi(Uid uid, boolean backGroundStart)
    {
        if (sInstance != null)
        {
            sInstance.createUiImpl(uid, backGroundStart);
        }
    }

    /**
     * For asking the runtime to do the shutdown of the application.
     * One user for this method is the CoreUi native implementation.
     */
    public static void shutdownRequest()
    {
        if (sInstance != null)
        {
            sInstance.shutdownRequestImpl();
        }
    }

    /**
     * For asking the runtime to bring the application to foreground.
     */
    public static void foregroundRequest()
    {
        if (sInstance != null)
        {
            sInstance.foregroundRequestImpl();
        }
    }

    /**
     * For asking if the UI is in foreground.
     */
    public static boolean isUiInForeground()
    {
        boolean fg = true;
        if (sInstance != null)
        {
            fg = sInstance.isUiInForegroundImpl();
        }
        return fg;
    }

    protected abstract boolean connectToUiImpl();
    protected abstract void createUiImpl(Uid uid, boolean backGroundStart);
    protected abstract void shutdownRequestImpl();
    protected abstract void foregroundRequestImpl();
    protected abstract boolean isUiInForegroundImpl();
}
