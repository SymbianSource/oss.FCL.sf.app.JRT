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


package com.nokia.mj.impl.installer.ui.eswt2;

import com.nokia.mj.impl.utils.exception.ExceptionBase;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

/**
 * Displays error message details.
 */
public class ErrorDetailsView extends ConfirmationViewBase
{
    private ExceptionBase iException = null;

    /** Constructor */
    protected ErrorDetailsView()
    {
        super();
    }

    /** Constructor */
    protected ErrorDetailsView(InstallerUiEswt aInstallerUi, Composite aParent)
    {
        super(aInstallerUi, aParent, 8);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALL_FAILED));
        setCommands(InstallerUiTexts.get(InstallerUiTexts.OK), null);
    }

    /**
     * Synchoronous method for displaying error message details.
     */
    public boolean error(ExceptionBase aException)
    {
        iException = aException;
        boolean result = confirm();
        if (result)
        {
            log("ErrorDetailsView confirmed");
        }
        else
        {
            log("ErrorDetailsView cancelled");
        }
        return result;
    }

    /**
     * This method is called once before view is opened.
     */
    protected void createView()
    {
        // Add title.
        //String title = InstallerUiTexts.get(InstallerUiTexts.INSTALL_FAILED);
        String title = "Installation failed";
        if (iInstallerUi.getInstallInfo() != null)
        {
            if (iInstallerUi.getInstallInfo().getOldVersion() != null)
            {
                title = "Update failed";
            }
            Label titleLabel = createLabel(title, getColumns() - 1, SWT.WRAP);
            titleLabel.setFont(iInstallerUi.getBoldFont());
            // Add security icon.
            iCertificates = iInstallerUi.getInstallInfo().getCertificates();
            createSecurityButton();
        }
        else
        {
            Label titleLabel = createLabel(title, getColumns(), SWT.WRAP);
            titleLabel.setFont(iInstallerUi.getBoldFont());
        }

        int horizontalSpan = getColumns();
        int labelStyle = SWT.WRAP;

        // Begin widgets creation.
        Label errorLabel = createLabel(
            iException.getDetailedMessage(), horizontalSpan, labelStyle);
        // End of widgets creation.
    }

    /**
     * This method is called after user has answered
     * to confirmation.
     * Inheriting class must implement this method.
     */
    protected void getDataFromView()
    {
        // nop
    }

    /**
     * Returns SWT style for this view.
     */
    protected int getStyle()
    {
        return SWT.V_SCROLL;
    }
}
