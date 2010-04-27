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


package com.nokia.mj.impl.installer.ui.eswt;

import com.nokia.mj.impl.installer.ui.ApplicationInfo;
import com.nokia.mj.impl.installer.ui.LaunchAppInfo;

import java.io.InputStream;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;

/**
 * LaunchAppQueryView asks from the user if the application should be launched.
 */
public class LaunchAppQueryView extends ConfirmationViewBase
{
    private LaunchAppInfo iLaunchAppInfo = null;
    private Button iDefaultButton = null;

    /** Constructor */
    protected LaunchAppQueryView()
    {
        super();
    }

    /** Constructor */
    protected LaunchAppQueryView(InstallerUiEswt aInstaller, Composite aParent)
    {
        super(aInstaller, aParent, 1);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALLATION_COMPLETE));
        setCommands(InstallerUiTexts.get(InstallerUiTexts.START),
                    InstallerUiTexts.get(InstallerUiTexts.CLOSE));
    }

    /** Show launch app query. */
    public boolean launchAppQuery(LaunchAppInfo aLaunchAppInfo)
    {
        if (isDisposed() || aLaunchAppInfo == null)
        {
            return false;
        }

        iLaunchAppInfo = aLaunchAppInfo;
        // Use confirm() from super class to display the view.
        boolean result = confirm();
        // Return the result to the client.
        return result;
    }

    protected void createView()
    {
        GridData gridData = null;
        int labelStyle = SWT.WRAP;

        // Add title.
        String title = InstallerUiTexts.get
                       (InstallerUiTexts.INSTALLATION_COMPLETE);
        Label titleLabel = createLabel(title, getColumns(), labelStyle);

        ApplicationInfo[] applications = iLaunchAppInfo.getApplications();
        if (applications != null)
        {
            // Load suite icon if it is available.
            InputStream suiteIconInputStream =
                iLaunchAppInfo.getSuiteIconInputStream();
            Image suiteIcon = null;
            if (suiteIconInputStream != null)
            {
                suiteIcon = InstallerUiEswt.loadImage
                            (getComposite().getDisplay(), suiteIconInputStream,
                             iLaunchAppInfo.getSuiteIconPath());
            }

            InputStream[] appIconInputStreams =
                iLaunchAppInfo.getAppIconInputStreams();
            Button[] appButtons = new Button[applications.length];
            for (int i = 0; i < applications.length; i++)
            {
                // Load application icon if it is available.
                Image appIcon = null;
                if (appIconInputStreams != null &&
                        appIconInputStreams.length > i &&
                        appIconInputStreams[i] != null)
                {
                    appIcon = InstallerUiEswt.loadImage
                              (getComposite().getDisplay(), appIconInputStreams[i],
                               applications[i].getIconPath());
                }
                if (appIcon == null && suiteIcon != null)
                {
                    appIcon = suiteIcon;
                }

                // Add launch button for each application.
                appButtons[i] = new Button(getComposite(), SWT.PUSH);
                gridData = new GridData(GridData.FILL_HORIZONTAL);
                appButtons[i].setLayoutData(gridData);
                appButtons[i].setText(applications[i].getName());
                appButtons[i].setImage(appIcon);

                // Add listeners for the launch button.
                final int index = i;
                appButtons[i].addListener(SWT.Selection, new Listener()
                {
                    public void handleEvent(Event aEvent)
                    {
                        iLaunchAppInfo.setSelection(index);
                        confirmOk();
                    }
                });
                appButtons[i].addFocusListener(new FocusListener()
                {
                    public void focusGained(FocusEvent aEvent)
                    {
                        getScrolledComposite().showControl(
                            (Control)aEvent.widget);
                        if (!userHasAnswered())
                        {
                            iLaunchAppInfo.setSelection(index);
                        }
                    }
                    public void focusLost(FocusEvent aEvent)
                    {
                        // nop
                    }
                });
                addSoftKeyListenerFor(appButtons[i]);
            }
            iDefaultButton = appButtons[0];
        }
    }

    /**
     * This method is called after user has answered to query.
     */
    protected void getDataFromView()
    {
        // User selection data is initialized in button listeners.
    }

    /**
     * Returns true if the View should have focus after it has been opened.
     */
    protected boolean forceFocusToView()
    {
        return false;
    }

    /**
     * Returns SWT style for this view.
     */
    protected int getStyle()
    {
        return SWT.V_SCROLL;
    }

    /**
     * Called after view and commands have been created. Subclasses
     * can overrride this method to set focus to their own default
     * commands.
     */
    protected void setDefaultCommand()
    {
        if (iDefaultButton != null)
        {
            iDefaultButton.setFocus();
            getShell().setDefaultButton(iDefaultButton);
        }
    }
}
