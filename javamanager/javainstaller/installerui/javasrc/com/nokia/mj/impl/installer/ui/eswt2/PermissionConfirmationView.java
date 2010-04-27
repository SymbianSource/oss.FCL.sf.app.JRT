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

import com.nokia.mj.impl.installer.ui.InstallInfo;
import com.nokia.mj.impl.installer.ui.PermissionInfo;

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
    private InstallInfo iInstallInfo = null;
    private PermissionInfo iPermissionInfo = null;
    private Button iAllowButton = null;
    private Button iDenyButton = null;
    private Button iCancelButton = null;

    /** Constructor */
    protected PermissionConfirmationView()
    {
        super();
    }

    /** Constructor */
    protected PermissionConfirmationView(InstallerUiEswt aInstaller, Composite aParent)
    {
        super(aInstaller, aParent, 8, true);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALL));
        setCommands(null, null);
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
        // Add header.
        String title = "Install?";
        if (iInstallInfo != null)
        {
            if (iInstallInfo.getOldVersion() != null)
            {
                title = "Update?";
            }
        }
        addHeader(title, iInstallInfo, null);

        GridData gridData = null;
        int horizontalSpan = getColumns();
        int labelStyle = SWT.WRAP;

        // Add permission query label.
        Label domainLabel = createLabel(
                                InstallerUiTexts.get(InstallerUiTexts.PERM_QUERY),
                                horizontalSpan, labelStyle);

        // Add link for permission details.
        if (iPermissionInfo != null &&
                iPermissionInfo.getPermissionNames() != null &&
                iPermissionInfo.getPermissionNames().length > 0)
        {
            Link detailsLink = new Link(getComposite(), SWT.NONE);
            detailsLink.setText(
                "<a>" +
                InstallerUiTexts.get(InstallerUiTexts.PERM_VIEW_DETAILS) +
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

        // Number of pixels on top of the buttons.
        int verticalIndent = 5;

        // Add allow button.
        iAllowButton = new Button(getComposite(), SWT.NONE);
        iAllowButton.setText(InstallerUiTexts.get(
                                 InstallerUiTexts.PERM_ALLOW_ALWAYS));
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
        gridData.verticalIndent = verticalIndent;
        iAllowButton.setLayoutData(gridData);
        iAllowButton.setFocus();
        addSoftKeyListenerFor(iAllowButton);

        // Add deny button.
        iDenyButton = new Button(getComposite(), SWT.NONE);
        iDenyButton.setText(InstallerUiTexts.get(
                                InstallerUiTexts.PERM_ASK_ME_LATER));
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
        gridData.verticalIndent = verticalIndent;
        iDenyButton.setLayoutData(gridData);
        addSoftKeyListenerFor(iDenyButton);

        // Add cancel button.
        iCancelButton = new Button(getComposite(), SWT.NONE);
        iCancelButton.setText(InstallerUiTexts.get(InstallerUiTexts.CANCEL));
        iCancelButton.addListener(SWT.Selection, new Listener()
        {
            public void handleEvent(Event aEvent)
            {
                confirmCancel();
            }
        });
        iCancelButton.addFocusListener(new FocusListener()
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
        gridData.verticalIndent = verticalIndent;
        iCancelButton.setLayoutData(gridData);
        addSoftKeyListenerFor(iCancelButton);
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
        if (iCancelButton != null && iCancelButton.isFocusControl())
        {
            confirmCancel();
        }
        else
        {
            confirmOk();
        }
    }

    /**
     * Opens permission details view.
     */
    private void openDetailsView()
    {
        if (iPermissionDetailsView != null)
        {
            return;
        }
        iPermissionDetailsView = new PermissionDetailsView(
            iInstallerUi, getShell(), getTitle(), iPermissionInfo);
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
}
