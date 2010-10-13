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

import org.eclipse.ercp.swt.mobile.Screen;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

/**
 * UsernamePasswordView asks username and password from the user.
 */
public class UsernamePasswordView extends ConfirmationViewBase
{
    private Text iUsernameText = null;
    private Text iPasswordText = null;
    private String iUrl = null;
    private String iUsername = null;
    private String iPassword = null;
    private String iAppName = null;
    private Object iAppNameWaitObject = new Object();

    /** Constructor */
    protected UsernamePasswordView()
    {
        super();
    }

    /** Constructor */
    protected UsernamePasswordView(
        InstallerUiEswt aInstallerUi, Composite aParent)
    {
        super(aInstallerUi, aParent, 1);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.CONNECT_TO));
        setCommands(InstallerUiTexts.get(InstallerUiTexts.OK),
                    InstallerUiTexts.get(InstallerUiTexts.CANCEL));
    }

    /** Set application name. */
    public void setAppName(String aAppName)
    {
        // Notify iAppNameWaitObject that iAppName is now available.
        synchronized (iAppNameWaitObject)
        {
            iAppName = aAppName;
            iAppNameWaitObject.notify();
        }
    }

    /** Get username and password. */
    public String[] getUsernamePassword(String aUrl)
    {
        if (isDisposed())
        {
            return null;
        }
        // If iAppName is null, wait until it gets set.
        synchronized (iAppNameWaitObject)
        {
            if (iAppName == null)
            {
                try
                {
                    iAppNameWaitObject.wait();
                }
                catch (InterruptedException ie)
                {
                }
            }
        }
        iUrl = aUrl;

        // UsernamePasswordView is always displayed in portrait mode.
        //forceScreenOrientation(Screen.PORTRAIT);
        // Use confirm() from super class to display the view.
        boolean confirmation = confirm();
        //forceScreenOrientation(SWT.DEFAULT);

        // And return the result to the client.
        String[] result = null;
        if (confirmation && iUsername != null && iPassword != null)
        {
            result = new String[] { iUsername, iPassword };
        }
        return result;
    }

    protected void createView()
    {

        int labelStyle = SWT.WRAP;

        Label titleLabel = createLabel
                           (InstallerUiTexts.get(InstallerUiTexts.CONNECT_TO), labelStyle);
        titleLabel.setFont(iInstallerUi.getBoldFont());

        Label urlLabel = createLabel(iUrl, labelStyle);

        Label authLabel = createLabel
                          (InstallerUiTexts.get
                           (InstallerUiTexts.DOWNLOAD_APPLICATION, new String[] { iAppName }),
                           labelStyle);

        Label usernameLabel = createLabel
                              (InstallerUiTexts.get(InstallerUiTexts.USERNAME), labelStyle);

        iUsernameText = new Text(getComposite(), SWT.BORDER);
        iUsernameText.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        addSoftKeyListenerFor(iUsernameText);

        Label passwordLabel = createLabel
                              (InstallerUiTexts.get(InstallerUiTexts.PASSWORD), labelStyle);

        iPasswordText = new Text(getComposite(), SWT.PASSWORD | SWT.BORDER);
        iPasswordText.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        addSoftKeyListenerFor(iPasswordText);
    }

    /**
     * This method is called after user has answered
     * to confirmation.
     */
    protected void getDataFromView()
    {
        iUsername = iUsernameText.getText();
        iPassword = iPasswordText.getText();
    }

    /**
     * Returns SWT style for this view.
     */
    protected int getStyle()
    {
        return SWT.V_SCROLL;
    }
}
