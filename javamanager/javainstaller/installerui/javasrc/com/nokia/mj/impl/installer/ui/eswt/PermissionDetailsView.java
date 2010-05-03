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

import com.nokia.mj.impl.installer.ui.PermissionInfo;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

/**
 * PermissionDetailsView displays permission details to the user.
 */
public class PermissionDetailsView extends ConfirmationViewBase
{
    private PermissionInfo iPermissionInfo = null;

    /** Constructor */
    protected PermissionDetailsView()
    {
        super();
    }

    /** Constructor */
    protected PermissionDetailsView(InstallerUiEswt aInstaller, Composite aParent,
                                    String aTitle,
                                    PermissionInfo aPermissionInfo)
    {
        super(aInstaller, aParent, 1);
        iPermissionInfo = aPermissionInfo;
        setTitle(aTitle);
        setCommands(null, InstallerUiTexts.get(InstallerUiTexts.BACK));
    }

    /**
     * This method is called once before view is opened.
     * Inheriting class must implement this method.
     */
    protected void createView()
    {
        GridData gridData = null;
        int horizontalSpan = getColumns();
        int labelStyle = SWT.WRAP;

        // Add title label.
        Label detailsLabel = createLabel
                             (InstallerUiTexts.get
                              (InstallerUiTexts.PERM_VIEW_DETAILS_TITLE),
                              horizontalSpan, labelStyle);
        detailsLabel.setFont(iInstallerUi.getBoldFont());

        // Add permission names.
        String[] permNames = iPermissionInfo.getPermissionNames();
        if (permNames != null)
        {
            for (int i = 0; i < permNames.length; i++)
            {
                Label permLabel = createLabel(
                                      permNames[i], horizontalSpan, labelStyle);
            }
        }

        /*
        // Add permissions details message.
        if (iPermissionInfo.getPermissionsDetails() != null)
        {
            Label emptyLabel = createLabel("", horizontalSpan, labelStyle);
            Label permissionDetailsLabel = createLabel(
                iPermissionInfo.getPermissionsDetails(),
                horizontalSpan, labelStyle);
        }
        */
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
