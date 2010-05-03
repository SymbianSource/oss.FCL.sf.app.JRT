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
import com.nokia.mj.impl.installer.ui.PermissionInfo;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;

import java.io.InputStream;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;

/**
 * PermissionConfirmationView asks permissions confirmation
 * from the user.
 */
public class PermissionConfirmationView extends ConfirmationViewBase
{
    private PermissionDetailsView iPermissionDetailsView = null;
    private CertificateDetailsView iCertificateDetailsView = null;
    private InstallInfo iInstallInfo = null;
    private PermissionInfo iPermissionInfo = null;
    /** Application suite icon */
    private Image iSuiteIcon = null;
    private Button iAllowButton = null;
    private Button iDenyButton = null;

    /** Constructor */
    protected PermissionConfirmationView()
    {
        super();
    }

    /** Constructor */
    protected PermissionConfirmationView(InstallerUiEswt aInstaller, Composite aParent)
    {
        super(aInstaller, aParent, 8);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALL));
        setCommands(null, InstallerUiTexts.get(InstallerUiTexts.CANCEL));
    }

    /**
     * Synchoronous method for asking user confirmation.
     */
    public boolean confirm(InstallInfo aInstallInfo,
                           PermissionInfo aPermissionInfo)
    {
        iInstallInfo = aInstallInfo;
        iPermissionInfo = aPermissionInfo;
        boolean result = confirm();
        if (result)
        {
            log("permissionConfirmationView confirmed");
        }
        else
        {
            log("permissionConfirmationView cancelled");
        }
        return result;
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

        // Add permission query label.
        Label domainLabel = createLabel(
                                InstallerUiTexts.get(InstallerUiTexts.PERM_QUERY),
                                horizontalSpan, labelStyle);

        // Add allow button.
        iAllowButton = new Button(getComposite(), SWT.NONE);
        iAllowButton.setText(InstallerUiTexts.get
                             (InstallerUiTexts.PERM_ALLOW_ALWAYS));
        iAllowButton.addListener(SWT.Selection, new Listener()
        {
            public void handleEvent(Event aEvent)
            {
                iPermissionInfo.setPermissionAllowed(true);
                confirmOk();
            }
        });
        iAllowButton.addFocusListener(new FocusListener()
        {
            public void focusGained(FocusEvent aEvent)
            {
                if (!userHasAnswered())
                {
                    iPermissionInfo.setPermissionAllowed(true);
                }
            }
            public void focusLost(FocusEvent aEvent)
            {
                // nop
            }
        });
        gridData = new GridData(GridData.FILL_HORIZONTAL);
        gridData.horizontalSpan = horizontalSpan;
        iAllowButton.setLayoutData(gridData);
        iAllowButton.setFocus();
        addSoftKeyListenerFor(iAllowButton);

        // Add deny button.
        iDenyButton = new Button(getComposite(), SWT.NONE);
        iDenyButton.setText(InstallerUiTexts.get
                            (InstallerUiTexts.PERM_ASK_ME_LATER));
        iDenyButton.addListener(SWT.Selection, new Listener()
        {
            public void handleEvent(Event aEvent)
            {
                iPermissionInfo.setPermissionAllowed(false);
                confirmOk();
            }
        });
        iDenyButton.addFocusListener(new FocusListener()
        {
            public void focusGained(FocusEvent aEvent)
            {
                if (!userHasAnswered())
                {
                    iPermissionInfo.setPermissionAllowed(false);
                }
            }
            public void focusLost(FocusEvent aEvent)
            {
                // nop
            }
        });
        gridData = new GridData(GridData.FILL_HORIZONTAL);
        gridData.horizontalSpan = horizontalSpan;
        iDenyButton.setLayoutData(gridData);
        addSoftKeyListenerFor(iDenyButton);

        // Add link for permission details.
        if (iPermissionInfo != null &&
                iPermissionInfo.getPermissionNames() != null &&
                iPermissionInfo.getPermissionNames().length > 0)
        {
            Link detailsLink = new Link(getComposite(), SWT.NONE);
            detailsLink.setText("<a>" +
                                InstallerUiTexts.get(
                                    InstallerUiTexts.PERM_VIEW_DETAILS) +
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

        // Add links for certificate details.
        SigningCertificate[] certs = iInstallInfo.getCertificates();
        if (certs != null)
        {
            // Add link to the first certificate only, it is the one
            // that will be used for MIDP2 applications.
            for (int i = 0; i < 1; i++)
            {
                Link certLink = new Link(getComposite(), SWT.NONE);
                certLink.setText("<a>" +
                                 InstallerUiTexts.get
                                 (InstallerUiTexts.CERTIFICATE_INFO) +
                                 "</a>");
                final int certIndex = i;
                certLink.addListener(SWT.Selection, new Listener()
                {
                    public void handleEvent(Event aEvent)
                    {
                        openCertsView(certIndex);
                    }
                });
                gridData = new GridData(GridData.FILL_HORIZONTAL);
                gridData.horizontalSpan = horizontalSpan;
                certLink.setLayoutData(gridData);
                addSoftKeyListenerFor(certLink);
            }
        }

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
        if (iAllowButton != null)
        {
            iAllowButton.setFocus();
            getShell().setDefaultButton(iAllowButton);
        }
    }

    /**
     * Called when user presses left soft key.
     */
    protected void lskPressed(KeyEvent aEvent)
    {
        if (iCancelCommand != null && iCancelCommand.isFocusControl())
        {
            confirmCancel();
        }
        else
        {
            confirmOk();
        }
    }

    /**
     * Opens installation details view.
     */
    private void openDetailsView()
    {
        if (iPermissionDetailsView != null)
        {
            return;
        }
        iPermissionDetailsView = new PermissionDetailsView
        (iInstallerUi, getShell(),
         getTitle(), iPermissionInfo);
        // New dialog must be opened in a new thread,
        // because we cannot use the main thread (which
        // is blocked by InstallConfirmationView) and we
        // cannot block the UI main thread
        // where this method gets executed.
        new Thread(new Runnable()
        {
            public void run()
            {
                if (iPermissionDetailsView.confirm())
                {
                    log("permissionDetailsView confirmed");
                    confirmOk();
                }
                else
                {
                    log("permissionDetailsView cancelled");
                }
                iPermissionDetailsView.dispose();
                iPermissionDetailsView = null;
                setVisible(true);
            }
        }, "InstallerUiPermDetailsViewThread").start();
    }

    /**
     * Opens certificate information view.
     */
    private void openCertsView(int aCertIndex)
    {
        iCertificateDetailsView = new CertificateDetailsView
        (iInstallerUi, getShell(),
         getTitle(), iInstallInfo, aCertIndex);
        // New dialog must be opened in a new thread,
        // because we cannot use the main thread (which
        // is blocked by InstallDetailsView) and we
        // cannot block the UI main thread
        // where this method gets executed.
        new Thread(new Runnable()
        {
            public void run()
            {
                if (iCertificateDetailsView.confirm())
                {
                    log("certificateDetailsView confirmed");
                    confirmOk();
                }
                else
                {
                    log("certificateDetailsView cancelled");
                }
                iCertificateDetailsView.dispose();
                iCertificateDetailsView = null;
                setVisible(true);
            }
        }, "InstallerUiCertViewThread").start();
    }
}
