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

import java.io.InputStream;

/**
 * Launch app info for JavaInstaller UI.
 */
public class LaunchAppInfo
{
    /** Applications that can be launched. */
    private ApplicationInfo[] iApplications = null;
    /** InputStreams where the UI can read the application icons. */
    private InputStream[] iAppIconInputStreams = null;
    /** InputStream where the UI can read the suite icon. */
    private InputStream iSuiteIconInputStream = null;
    /** Path to the suite icon file. */
    private String iSuiteIconPath = null;
    /** Index of application to be launched. */
    private int iSelection = 0;

    /** Constructor. */
    private LaunchAppInfo()
    {
    }

    /** Constructor. */
    public LaunchAppInfo(ApplicationInfo[] aApplications,
                         InputStream[] aAppIcons,
                         InputStream aSuiteIcon,
                         String aSuiteIconPath)
    {
        iApplications = aApplications;
        iAppIconInputStreams = aAppIcons;
        iSuiteIconInputStream = aSuiteIcon;
        iSuiteIconPath = aSuiteIconPath;
    }

    /** Get applications. */
    public ApplicationInfo[] getApplications()
    {
        return iApplications;
    }

    /** Get application icon InputStreams. */
    public InputStream[] getAppIconInputStreams()
    {
        return iAppIconInputStreams;
    }

    /** Get suite icon InputStream. */
    public InputStream getSuiteIconInputStream()
    {
        return iSuiteIconInputStream;
    }

    /** Get suite icon path. */
    public String getSuiteIconPath()
    {
        return iSuiteIconPath;
    }

    /** Get selection index. */
    public int getSelection()
    {
        return iSelection;
    }

    /** Set index of selected application. */
    public void setSelection(int aSelection)
    {
        iSelection = aSelection;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer("LaunchAppInfo\n");
        buf.append("Selection: ").append(getSelection()).append("\n");
        for (int i = 0; iApplications != null && i < iApplications.length; i++)
        {
            buf.append("Applications[").append(i).append("]: ")
            .append(iApplications[i]).append("\n");
        }
        buf.append("SuiteIconPath: ").append(getSuiteIconPath()).append("\n");
        return buf.toString();
    }
}
