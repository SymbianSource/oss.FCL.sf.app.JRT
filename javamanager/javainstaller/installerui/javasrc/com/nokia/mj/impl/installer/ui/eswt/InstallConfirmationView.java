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

import com.nokia.mj.impl.installer.ui.InstallInfo;

import java.io.InputStream;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.Listener;

/**
 * InstallConfirmationView asks installation confirmation
 * from the user.
 */
public class InstallConfirmationView extends ConfirmationViewBase
{
    private InstallDetailsView iInstallDetailsView = null;
    private InstallInfo iInstallInfo = null;
    /** Application suite icon */
    private Image iSuiteIcon = null;

    /** Constructor */
    protected InstallConfirmationView()
    {
        super();
    }

    /** Constructor */
    protected InstallConfirmationView(InstallerUiEswt aInstaller, Composite aParent)
    {
        super(aInstaller, aParent, 8);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALL));
        setCommands(InstallerUiTexts.get(InstallerUiTexts.OK),
                    InstallerUiTexts.get(InstallerUiTexts.CANCEL));
    }

    /**
     * Synchoronous method for asking user confirmation.
     */
    public boolean confirm(InstallInfo aInstallInfo)
    {
        iInstallInfo = aInstallInfo;
        boolean result = confirm();
        if (result)
        {
            log("installConfirmationView confirmed");
        }
        else
        {
            log("installConfirmationView cancelled");
        }
        return result;
    }

    /**
     * Calls setVisible method for superclass and calls
     * InstallerUiListener.uiIsReady when this view is
     * set visible for the first time,
     */
    private boolean iUiListenerNotified = false;
    public void setVisible(boolean aVisible)
    {
        super.setVisible(aVisible);
        if (aVisible && !iUiListenerNotified &&
                iInstallerUi.getListener() != null)
        {
            // Tell installer that UI is now ready and
            // "preparing installation" indicator can be hidden.
            iInstallerUi.getListener().uiIsReady();
            iUiListenerNotified = true;
        }
    }

    /**
     * This method is called once before view is opened.
     */
    protected void createView()
    {
        GridData gridData = null;
        int horizontalSpan = getColumns() - 1;
        int labelStyle = SWT.WRAP;

        // Add suite icon.
        InputStream iconInputStream = iInstallInfo.getIconInputStream();
        if (iconInputStream != null)
        {
            iSuiteIcon = InstallerUiEswt.loadImage
                         (getComposite().getDisplay(), iconInputStream,
                          iInstallInfo.getIconPath());
            if (iSuiteIcon != null)
            {
                Label iconLabel = createLabel(iSuiteIcon, 2, SWT.NONE);
                horizontalSpan = getColumns() - 3;
            }
            else
            {
                horizontalSpan = getColumns() - 1;
            }
        }

        // Add title.
        String title = InstallerUiTexts.get
                       (InstallerUiTexts.INSTALL_QUERY,
                        new String[] { iInstallInfo.getName() });
        if (iInstallInfo.getOldVersion() != null)
        {
            title = InstallerUiTexts.get
                    (InstallerUiTexts.UPDATE_QUERY,
                     new String[] { iInstallInfo.getName(),
                                    iInstallInfo.getOldVersion(),
                                    iInstallInfo.getVersion()
                                  });
        }
        Label titleLabel = createLabel(title, horizontalSpan, labelStyle);
        titleLabel.setFont(iInstallerUi.getBoldFont());

        // Add security icon.
        boolean identified = (iInstallInfo.getCertificates() != null);
        Label securityIconLabel = createSecurityLabel(identified);

        horizontalSpan = getColumns();

        // Add link to details.
        Link detailsLink = new Link(getComposite(), SWT.NONE);
        detailsLink.setText("<a>" +
                            InstallerUiTexts.get
                            (InstallerUiTexts.APP_DETAILS) +
                            "</a>");
        detailsLink.addListener(SWT.Selection, new Listener()
        {
            public void handleEvent(Event aEvent)
            {
                openDetailsView();
            }
        });
        gridData = new GridData(GridData.FILL_HORIZONTAL);
        gridData.horizontalSpan = horizontalSpan;
        detailsLink.setLayoutData(gridData);
        addSoftKeyListenerFor(detailsLink);
    }

    /**
     * This method is called after user has answered
     * to confirmation.
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

    /**
     * Opens installation details view.
     */
    private void openDetailsView()
    {
        if (iInstallDetailsView != null)
        {
            return;
        }
        iInstallDetailsView = new InstallDetailsView(
            iInstallerUi, getShell(), getTitle(), iInstallInfo, iSuiteIcon);
        // Hide this view, so that if details view is cancelled
        // this view is no longer visible.
        setVisible(false);
        // New dialog must be opened in a new thread,
        // because we cannot use the main thread (which
        // is blocked by InstallConfirmationView) and we
        // cannot block the UI main thread
        // where this method gets executed.
        new Thread(new Runnable()
        {
            public void run()
            {
                if (iInstallDetailsView.confirm())
                {
                    log("installDetailsView confirmed");
                    confirmOk();
                }
                else
                {
                    log("installDetailsView cancelled");
                    confirmCancel();
                }
                iInstallDetailsView.dispose();
                iInstallDetailsView = null;
            }
        }, "InstallerUiDetailsViewThread").start();
    }
}
