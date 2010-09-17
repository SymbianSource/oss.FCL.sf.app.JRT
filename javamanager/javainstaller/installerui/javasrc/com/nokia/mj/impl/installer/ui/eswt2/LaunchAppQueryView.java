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


package com.nokia.mj.impl.installer.ui.eswt2;

import com.nokia.mj.impl.installer.ui.LaunchAppInfo;

import org.eclipse.swt.widgets.Composite;

/**
 * LaunchAppQueryView asks from the user if the installed
 * application should be displayed.
 */
public class LaunchAppQueryView extends ConfirmationViewBase
{
    /** Constructor */
    protected LaunchAppQueryView()
    {
        super();
    }

    /** Constructor */
    protected LaunchAppQueryView(
        InstallerUiEswt aInstallerUi, Composite aParent)
    {
        super(aInstallerUi, aParent, 8);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALLATION_COMPLETE));
        setCommands(InstallerUiTexts.get(InstallerUiTexts.SHOW),
                    InstallerUiTexts.get(InstallerUiTexts.CLOSE));
    }

    /** Show launch app query. */
    public boolean launchAppQuery(LaunchAppInfo aLaunchAppInfo)
    {
        if (isDisposed() || aLaunchAppInfo == null)
        {
            return false;
        }

        // Use confirm() from super class to display the view.
        boolean result = confirm();
        // Return the result to the client.
        return result;
    }

    protected void createView()
    {
        // Add header.
        addHeader(InstallerUiTexts.get(InstallerUiTexts.INSTALLATION_COMPLETE),
                  iInstallerUi.getInstallInfo(), null);
        // Add content to the application info Composite.
        addAppInfo(iInstallerUi.getInstallInfo(), true, false);
    }

    /**
     * This method is called after user has answered to query.
     */
    protected void getDataFromView()
    {
        // User selection data is initialized in button listeners.
    }
}
