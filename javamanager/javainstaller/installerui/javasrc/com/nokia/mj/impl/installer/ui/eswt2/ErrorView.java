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
 * Displays error message.
 */
public class ErrorView extends ConfirmationViewBase
{
    private ExceptionBase iException = null;

    /** Constructor */
    protected ErrorView()
    {
        super();
    }

    /** Constructor */
    protected ErrorView(InstallerUiEswt aInstallerUi, Composite aParent)
    {
        super(aInstallerUi, aParent, 8);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALL_FAILED));
        setCommands(InstallerUiTexts.get(InstallerUiTexts.DETAILS),
                    InstallerUiTexts.get(InstallerUiTexts.CLOSE));
    }

    /**
     * Synchoronous method for displaying error message.
     */
    public boolean error(ExceptionBase aException)
    {
        iException = aException;
        boolean result = confirm();
        if (result)
        {
            log("ErrorView confirmed");
        }
        else
        {
            log("ErrorView cancelled");
        }
        return result;
    }

    /**
     * This method is called once before view is opened.
     */
    protected void createView()
    {
        // Add header.
        String title = InstallerUiTexts.get(InstallerUiTexts.INSTALL_FAILED);
        if (iInstallerUi.getInstallInfo() != null)
        {
            addHeader(title, iInstallerUi.getInstallInfo(), null);
        }
        else
        {
            Label titleLabel = createLabel(
                getHeadingComposite(), title, getHeadingColumns(), SWT.WRAP);
            setCssId(titleLabel, "heading");
        }

        int horizontalSpan = getColumns();
        int labelStyle = SWT.WRAP;

        // Begin widgets creation.
        Label errorLabel = createLabel(
            iException.getShortMessage(), horizontalSpan, labelStyle);
        setCssId(errorLabel, "errorLabel");
        // End of widgets creation.

        if (iInstallerUi.getInstallInfo() != null)
        {
            // After other widgets have been added, add content to
            // application info Composite.
            addAppInfo(iInstallerUi.getInstallInfo(), true, false);
        }
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
