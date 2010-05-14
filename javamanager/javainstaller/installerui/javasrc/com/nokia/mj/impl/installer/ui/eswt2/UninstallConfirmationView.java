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

import com.nokia.mj.impl.installer.ui.ApplicationInfo;
import com.nokia.mj.impl.installer.ui.UninstallInfo;

import java.io.InputStream;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

/**
 * UninstallConfirmationView asks uninstallation confirmation
 * from the user.
 */
public class UninstallConfirmationView extends ConfirmationViewBase
{
    private UninstallInfo iUninstallInfo = null;

    /** Constructor */
    protected UninstallConfirmationView()
    {
        super();
    }

    /** Constructor */
    protected UninstallConfirmationView(
        InstallerUiEswt aInstallerUi, Composite aParent)
    {
        super(aInstallerUi, aParent, 8);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.UNINSTALL));
        setCommands(InstallerUiTexts.get(InstallerUiTexts.OK),
                    InstallerUiTexts.get(InstallerUiTexts.CANCEL));
    }

    /**
     * Synchoronous method for asking user confirmation.
     */
    public boolean confirm(UninstallInfo aUninstallInfo)
    {
        iUninstallInfo = aUninstallInfo;
        boolean result = confirm();
        if (result)
        {
            log("uninstallConfirmationView confirmed");
        }
        else
        {
            log("uninstallConfirmationView cancelled");
        }
        return result;
    }

    /**
     * This method is called once before view is opened.
     * Inheriting class must implement this method.
     */
    protected void createView()
    {
        GridData gridData = null;
        int horizontalSpan = getColumns() - 1;
        int labelStyle = SWT.WRAP;

        InputStream iconInputStream = iUninstallInfo.getIconInputStream();
        if (iconInputStream != null)
        {
            Image image = InstallerUiEswt.loadImage
                          (getComposite().getDisplay(), iconInputStream,
                           iUninstallInfo.getIconPath());
            if (image != null)
            {
                Label iconLabel = createLabel(image, 2, SWT.NONE);
                horizontalSpan = getColumns() - 3;
            }
            else
            {
                horizontalSpan = getColumns() - 1;
            }
        }

        // Add title.
        Label titleLabel = createLabel
                           (InstallerUiTexts.get
                            (InstallerUiTexts.UNINSTALL_QUERY,
                             new String[] { iUninstallInfo.getName() }),
                            horizontalSpan, labelStyle);
        titleLabel.setFont(iInstallerUi.getBoldFont());

        // Add security icon.
        boolean identified = (iUninstallInfo.getCertificates() != null);
        Label securityIconLabel = createSecurityLabel(identified);

        horizontalSpan = getColumns();

        Label nameLabel = createLabel
                          (InstallerUiTexts.get
                           (InstallerUiTexts.NAME,
                            new String[] { iUninstallInfo.getName() }),
                           horizontalSpan, labelStyle);

        if (identified)
        {
            // Vendor information must be displayed only for
            // identified applications.
            Label vendorLabel = createLabel
                                (InstallerUiTexts.get
                                 (InstallerUiTexts.VENDOR,
                                  new String[] { iUninstallInfo.getVendor() }),
                                 horizontalSpan, labelStyle);
        }

        Label versionLabel = createLabel
                             (InstallerUiTexts.get
                              (InstallerUiTexts.VERSION,
                               new String[] { iUninstallInfo.getVersion() }),
                              horizontalSpan, labelStyle);

        ApplicationInfo[] apps = iUninstallInfo.getApplications();
        if (apps != null && apps.length > 0)
        {
            if (apps.length > 1 ||
                    !iUninstallInfo.getName().equals(apps[0].getName()))
            {
                Label appsLabel = createLabel
                                  (InstallerUiTexts.get
                                   (InstallerUiTexts.APPLICATIONS),
                                   horizontalSpan, labelStyle);

                for (int i = 0; i < apps.length; i++)
                {
                    Label appLabel = createLabel
                                     (InstallerUiTexts.get
                                      (InstallerUiTexts.APP_NAME,
                                       new String[] { apps[i].getName() }),
                                      horizontalSpan, labelStyle);
                }
            }
        }

        String deleteConfirm = iUninstallInfo.getDeleteConfirm();
        if (deleteConfirm != null)
        {
            Label deleteConfirmLabel = createLabel
                                       (deleteConfirm, horizontalSpan, labelStyle);
        }
    }

    /**
     * This method is called after user has answered
     * to confirmation.
     * Inheriting class must implement this method.
     */
    protected void getDataFromView()
    {
        // No data to get from uninstallation confirmation.
    }

    /**
     * Returns SWT style for this view.
     */
    protected int getStyle()
    {
        return SWT.V_SCROLL;
    }
}
