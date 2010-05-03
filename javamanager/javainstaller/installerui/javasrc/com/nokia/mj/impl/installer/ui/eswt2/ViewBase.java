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
import com.nokia.mj.impl.installer.ui.InstallInfo;
import com.nokia.mj.impl.installer.ui.UninstallInfo;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;

import java.io.InputStream;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

/**
 * Base class for different InstallerUi views.
 */
abstract public class ViewBase
{
    /** Maximum view height in percentage from display client area height. */
    protected static final int MAX_VIEW_HEIGHT = 80;
    /** Parent shell for this view. */
    protected Shell iParent = null;
    /** Container for the contents of the view */
    private Composite iContainer = null;
    /** ScrolledComposite for iComposite. */
    private ScrolledComposite iScrolledComposite = null;
    /** Composite to which subclasses add their widgets. */
    private Composite iComposite = null;
    /** ScrolledComposite for iAppInfoComposite. */
    private ScrolledComposite iAppInfoScrolledComposite = null;
    /** Composite to which application info is added. */
    private Composite iAppInfoComposite = null;
    /** Composite to which command buttons are added. */
    private Composite iCommandComposite = null;
    /** Application suite icon */
    protected Image iSuiteIcon = null;
    /** InstallerUi owning this view. */
    protected InstallerUiEswt iInstallerUi = null;
    /** True if this view is visible. */
    private boolean iVisible = false;
    /** Number of columns for this view's GridLayout. */
    private int iColumns = 1;
    /** Default content size. */
    private Point iDefaultContentSize = null;
    /** Certificate details view.  */
    private CertificateDetailsView iCertificateDetailsView = null;
    /** Certificates for this application. */
    private SigningCertificate[] iCertificates = null;

    /** Constructor */
    protected ViewBase()
    {
    }

    /** Constructor */
    protected ViewBase(InstallerUiEswt aInstaller, Composite aParent, int aColumns)
    {
        this(aInstaller, aParent, aColumns, false);
    }

    /** Constructor */
    protected ViewBase(InstallerUiEswt aInstaller, Composite aParent, int aColumns, boolean aScrollable)
    {
        iInstallerUi = aInstaller;

        // Each view gets a shell to be used as a parameter.
        iParent = (Shell)aParent;

        iContainer = new Composite(iParent, 0);
        iContainer.setVisible(false);

        iColumns = aColumns;

        // Store the default bounds for later height adjustments.
        Rectangle defShellBounds = iInstallerUi.getDefaultShellBounds();
        Rectangle defShellClientBounds = iInstallerUi.getDefaultShellClientBounds();
        iContainer.setSize(defShellClientBounds.width, defShellClientBounds.height);

        // Let the contents fill the Shell.
        iContainer.setLayout(setZeroMargins(new GridLayout()));

        if (aScrollable)
        {
            // Create a ScrolledComposite for views which need ScrollBars.
            iScrolledComposite = new ScrolledComposite(iContainer, getStyle());
            iScrolledComposite.setAlwaysShowScrollBars(false);
            iScrolledComposite.setExpandHorizontal(true);
            iScrolledComposite.setLayoutData(new GridData(GridData.FILL_BOTH));
            // Create the content composite for the ScrolledComposite.
            iComposite = new Composite(iScrolledComposite, SWT.NONE);
            iComposite.setLayout(setZeroMargins(new GridLayout(getColumns(), true)));
            iScrolledComposite.setContent(iComposite);
        }
        else
        {
            // Create the composite without ScrollBars.
            iComposite = new Composite(iContainer, SWT.NONE);
            GridLayout compLayout =
                setZeroMargins(new GridLayout(getColumns(), true));
            iComposite.setLayout(compLayout);
            iComposite.setLayoutData(new GridData(GridData.FILL_BOTH));
        }

        // Create a composite for command buttons.
        iCommandComposite = new Composite(iContainer, SWT.NONE);
        GridLayout cmdLayout = setZeroMargins(new GridLayout(2, true));
        cmdLayout.marginTop = 5;
        iCommandComposite.setLayout(cmdLayout);
        iCommandComposite.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        // Layout now and get the default size of the content area.
        iContainer.layout(true);
        Rectangle rect = null;
        if (aScrollable)
        {
            rect = iScrolledComposite.getClientArea();
        }
        else
        {
            rect = iComposite.getClientArea();
        }
        iDefaultContentSize = new Point(rect.width, rect.height);
    }

    /**
     * Returns composite which holds the contents of this view.
     */
    public Composite getContainer()
    {
        return iContainer;
    }

    /** Returns composite to which subclasses can add their widgets. */
    public Composite getComposite()
    {
        return iComposite;
    }

    /** Returns ScrolledComposite for Composite. */
    public ScrolledComposite getScrolledComposite()
    {
        return iScrolledComposite;
    }

    /** Returns composite for application info widgets. */
    public Composite getAppInfoComposite()
    {
        return iAppInfoComposite;
    }

    /** Returns ScrolledComposite for AppInfoComposite. */
    public ScrolledComposite getAppInfoScrolledComposite()
    {
        return iAppInfoScrolledComposite;
    }

    /** Returns composite to which command buttons are added. */
    public Composite getCommandComposite()
    {
        return iCommandComposite;
    }

    /** Returns a shell for this view. */
    public Shell getShell()
    {
        return iParent;
    }

    /** Returns display for this view. */
    public Display getDisplay()
    {
        return getShell().getDisplay();
    }

    /**
     * Returns title for this view. If the title has not been
     * set, returns an empty string.
     */
    public String getTitle()
    {
        return iParent.getText();
    }

    /** Set title for this view. */
    public void setTitle(String aTitle)
    {
        // Dialog shells have no title anymore
    }

    /** Disposes this view. */
    public void dispose()
    {
        // This must be final because it is used from inner class.
        final String className = this.toString();
        log(className + ": disposing");
        // UI updates must be executed in UI thread.
        iParent.getDisplay().syncExec
        (new Runnable()
        {
            public void run()
            {
                log(className + ": disposing view container");
                iContainer.dispose();
            }
        });
    }

    /** Returns true this view has been disposed, false otherwise. */
    public boolean isDisposed()
    {
        if (iContainer.isDisposed())
        {
            return true;
        }
        return false;
    }

    /** Set this view visible. Any previously visible view will be hidden.
     *  The size of the dialog Shell host will be changed to fit this view.*/
    public void setVisible(boolean aVisible)
    {
        if (iVisible == aVisible)
        {
            return;
        }
        log(this.toString() + ": setVisible " + aVisible);
        iVisible = aVisible;
        if (iVisible)
        {
            iInstallerUi.setActiveView(this);
        }
        else
        {
            if (iInstallerUi.getActiveView() == this)
            {
                iInstallerUi.setActiveView(null);
            }
        }
        // UI updates must be executed in UI thread.
        iParent.getDisplay().syncExec
        (new Runnable()
        {
            public void run()
            {
                if (iVisible)
                {
                    updateSize();
                }
                iContainer.setVisible(iVisible);
                if (iVisible)
                {
                    if (iParent.getDisplay().getActiveShell() != iParent)
                    {
                        iParent.open();
                    }
                    if (!iParent.isVisible())
                    {
                        iParent.setVisible(true);
                    }
                    if (forceFocusToView())
                    {
                        iContainer.forceFocus();
                    }
                }
            }
        });
    }

    /** Returns true if this view is visible, false otherwise. */
    public boolean isVisible()
    {
        return iVisible;
    }

    /** Sets the view size according to display size. */
    protected void updateSize()
    {
        getShell().setRedraw(false);
        // First calculate size without vertical scrollbar.
        doUpdateSize(false);
        if (getScrolledComposite() != null &&
                getScrolledComposite().getVerticalBar() != null &&
                getScrolledComposite().getVerticalBar().getVisible())
        {
            // If scrollbar is visible recalculate size with scrollbar.
            doUpdateSize(true);
        }
        getShell().setRedraw(true);
    }

    /** Sets the view size according to display size. */
    private void doUpdateSize(boolean aVerticalScrollBarVisible)
    {
        Shell shell = getShell();
        Composite comp = getComposite();
        Composite cmdComp = getCommandComposite();

        if (getAppInfoComposite() != null)
        {
            // Recalculate the size of the app info composite.
            getAppInfoComposite().setSize(
                getAppInfoComposite().computeSize(
                    SWT.DEFAULT, SWT.DEFAULT));
        }

        int contentWidth = iDefaultContentSize.x;
        if (aVerticalScrollBarVisible)
        {
            int verticalScrollBarWidth =
                getScrolledComposite().getVerticalBar().getSize().x;
            contentWidth = iDefaultContentSize.x - verticalScrollBarWidth;
        }

        // Recalculate the size of the content.
        Point contentSize = comp.computeSize(contentWidth, SWT.DEFAULT);
        comp.setSize(contentSize);
        Point cmdContentSize = cmdComp.computeSize(iDefaultContentSize.x, SWT.DEFAULT);
        cmdComp.setSize(cmdContentSize);

        // Adjust Shell height. The Shell never changes the x position, nor the width.
        Rectangle dispRect = shell.getDisplay().getClientArea();
        int offset = iDefaultContentSize.y - contentSize.y - cmdContentSize.y;

        Rectangle defShellBounds = iInstallerUi.getDefaultShellBounds();
        int newHeight = defShellBounds.height - offset;
        int maxHeight = dispRect.height * MAX_VIEW_HEIGHT / 100;

        if (newHeight > maxHeight)
        {
            offset -= maxHeight - newHeight;
            newHeight = maxHeight;
        }

        Rectangle dispBounds = shell.getDisplay().getBounds();
        int y = dispBounds.height - newHeight;
        // Always center vertically.
        y /= 2;
        // For landscape orientation center vertically
        //if (dispRect.width > dispRect.height)
        //{
        //    y /= 2;
        //}
        // Set bounds when command Buttons are in use.
        shell.setBounds(defShellBounds.x, y, defShellBounds.width, newHeight);
        Rectangle clientArea = shell.getClientArea();
        iContainer.setSize(clientArea.width, clientArea.height);
        iContainer.layout(true);
    }

    /**
     * Called when screen orientation changes.
     */
    protected void screenOrientationChanged()
    {
        updateSize();
    }

    /**
     * Returns true if the View should have focus after it has been opened.
     */
    protected boolean forceFocusToView()
    {
        return true;
    }

    /**
     * Returns SWT style for this view. Subclasses should override
     * this method to return style with SWT.H_SCROLL and/or SWT.V_SCROLL
     * if they need scrollbars.
     */
    protected int getStyle()
    {
        return SWT.NONE;
    }

    /**
     * Returns number of columns for this view.
     */
    protected int getColumns()
    {
        return iColumns;
    }

    /**
     * Adds header used in installation and uninstallation views.
     */
    protected void addHeader(
        String aTitle, InstallInfo aInstallInfo, UninstallInfo aUninstallInfo)
    {
        // Add title.
        String title = "Install?";
        if (aInstallInfo != null)
        {
            if (aInstallInfo.getOldVersion() != null)
            {
                title = "Update?";
            }
            iCertificates = aInstallInfo.getCertificates();
        }
        if (aUninstallInfo != null)
        {
            title = "Uninstall?";
            iCertificates = aUninstallInfo.getCertificates();
        }
        Label titleLabel = createLabel(aTitle, getColumns() - 1, SWT.WRAP);
        titleLabel.setFont(iInstallerUi.getBoldFont());

        // Add security icon.
        createSecurityButton();

        // Add suite icon.
        InputStream iconInputStream = null;
        String iconPath = null;
        if (aInstallInfo != null)
        {
            iconInputStream = aInstallInfo.getIconInputStream();
            iconPath = aInstallInfo.getIconPath();
        }
        if (aUninstallInfo != null)
        {
            iconInputStream = aUninstallInfo.getIconInputStream();
            iconPath = aUninstallInfo.getIconPath();
        }
        if (iSuiteIcon == null && iconInputStream != null)
        {
            iSuiteIcon = InstallerUiEswt.loadImage(
                             getComposite().getDisplay(), iconInputStream, iconPath);
        }
        int iconColumns = 0;
        if (iSuiteIcon != null)
        {
            iconColumns = 2;
            Label iconLabel = createLabel(iSuiteIcon, iconColumns, SWT.NONE);
        }

        // Create a Composite for displaying application info.
        iAppInfoScrolledComposite =
            new ScrolledComposite(getComposite(), SWT.H_SCROLL | SWT.V_SCROLL);
        iAppInfoScrolledComposite.setAlwaysShowScrollBars(false);
        iAppInfoScrolledComposite.setExpandHorizontal(true);
        GridData gridData = new GridData(GridData.FILL_BOTH);
        gridData.horizontalSpan = getColumns() - iconColumns;
        iAppInfoScrolledComposite.setLayoutData(gridData);
        iAppInfoComposite = new Composite(iAppInfoScrolledComposite, SWT.NONE);
        iAppInfoComposite.setLayout(new GridLayout(1, true));
        iAppInfoScrolledComposite.setContent(iAppInfoComposite);
    }

    /**
     * Adds application information to the header.
     */
    protected void addAppInfo(InstallInfo aInstallInfo, boolean aFull)
    {
        if (aInstallInfo == null)
        {
            // Nothing to add.
            return;
        }

        // Add suite name.
        createAppInfoLabel(
            aInstallInfo.getName() + " " + aInstallInfo.getVersion());
        // Add size.
        long size = 0;
        if (aInstallInfo.getJarSize() > 0)
        {
            size += aInstallInfo.getJarSize();
        }
        if (aInstallInfo.getDataSize() > 0)
        {
            size += aInstallInfo.getDataSize();
        }
        if (size > 0)
        {
            createAppInfoLabel(
                InstallerUiTexts.get(
                    InstallerUiTexts.SIZE,
                    new String[] { Long.toString(1 + size/1024) }));
        }
        if (aFull)
        {
            // Add vendor.
            if (aInstallInfo.getCertificates() != null)
            {
                // Vendor information must be displayed only for
                // identified applications.
                createAppInfoLabel(
                    InstallerUiTexts.get(
                        InstallerUiTexts.VENDOR,
                        new String[] { aInstallInfo.getVendor() }));
            }
            // Add application names.
            ApplicationInfo[] apps = aInstallInfo.getApplications();
            if (apps != null && apps.length > 0)
            {
                if (apps.length > 1 ||
                        !aInstallInfo.getName().equals(apps[0].getName()))
                {
                    for (int i = 0; i < apps.length; i++)
                    {
                        createAppInfoLabel(apps[i].getName());
                    }
                }
            }
        }
    }

    /**
     * Creates a new label with given text and adds it to this view.
     * The created label takes one column from this view's GridLayout.
     *
     * @param aText text for the label
     * @param aStyle SWT style for the label
     * @return label that was added to this view
     */
    protected Label createLabel(String aText, int aStyle)
    {
        return createLabel(aText, getColumns(), aStyle);
    }

    /**
     * Creates a new label with given text and adds it to this view.
     *
     * @param aText text for the label
     * @param aColumns number of columns the label takes
     * @param aStyle SWT style for the label
     * @return label that was added to this view
     */
    protected Label createLabel(String aText, int aColumns, int aStyle)
    {
        Label label = new Label(getComposite(), aStyle);
        label.setText(aText);
        GridData gridData = new GridData(GridData.FILL_HORIZONTAL);
        gridData.horizontalSpan = aColumns;
        label.setLayoutData(gridData);
        return label;
    }

    /**
     * Creates a new label with given image and adds it to this view.
     * The created label takes one column from this view's GridLayout.
     *
     * @param aImage image for the label
     * @param aStyle SWT style for the label
     * @return label that was added to this view
     */
    protected Label createLabel(Image aImage, int aStyle)
    {
        return createLabel(aImage, getColumns(), aStyle);
    }

    /**
     * Creates a new label with given image and adds it to this view.
     * The created label takes one column from this view's GridLayout.
     *
     * @param aImage image for the label
     * @param aColumns number of columns the label takes
     * @param aStyle SWT style for the label
     * @return label that was added to this view
     */
    protected Label createLabel(Image aImage, int aColumns, int aStyle)
    {
        Label label = new Label(getComposite(), aStyle);
        label.setImage(aImage);
        GridData gridData = new GridData(GridData.FILL_HORIZONTAL);
        gridData.horizontalSpan = aColumns;
        gridData.horizontalAlignment = SWT.CENTER;
        gridData.verticalAlignment = SWT.CENTER;
        label.setLayoutData(gridData);
        return label;
    }

    /**
     * Creates a new label with given text and adds it
     * to application info composite for this view.
     *
     * @param aText text for the label
     * @return label that was added to this view
     */
    protected Label createAppInfoLabel(String aText)
    {
        Label label = new Label(getAppInfoComposite(), SWT.WRAP);
        label.setText(aText);
        GridData gridData = new GridData(GridData.FILL_HORIZONTAL);
        gridData.horizontalSpan = 1;
        label.setLayoutData(gridData);
        return label;
    }

    /**
     * Creates a new label with security icon.
     *
     * @param aIdentified true if security icon is for an
     * identified application, false otherwise
     * @return label that was added to this view
     */
    protected Label createSecurityLabel(boolean aIdentified)
    {
        Label label = createLabel((Image)null, 1, SWT.NONE);
        Image securityIcon = null;
        if (iInstallerUi instanceof InstallerUiEswt)
        {
            securityIcon = ((InstallerUiEswt)iInstallerUi).getSecurityIcon
                           (getDisplay(), aIdentified);
        }
        if (securityIcon != null)
        {
            label.setImage(securityIcon);
        }
        else
        {
            label.setText(aIdentified? "I": "U");
        }
        return label;
    }

    protected Button createSecurityButton()
    {
        Button button = new Button(getComposite(), SWT.PUSH);
        GridData gridData = new GridData(
            GridData.FILL_HORIZONTAL | GridData.FILL_VERTICAL);
        gridData.horizontalSpan = 1;
        gridData.horizontalAlignment = SWT.CENTER;
        gridData.verticalAlignment = SWT.CENTER;
        button.setLayoutData(gridData);
        Image securityIcon = null;
        if (iInstallerUi instanceof InstallerUiEswt)
        {
            securityIcon =
                ((InstallerUiEswt)iInstallerUi).getSecurityIcon(
                    getDisplay(), iCertificates != null);
        }
        if (securityIcon != null)
        {
            button.setImage(securityIcon);
        }
        else
        {
            button.setText(iCertificates != null? "I": "U");
        }
        button.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent aEvent)
            {
                openCertsView(iCertificates, (iCertificates != null? 0: -1));
            }
        });
        return button;
    }

    /**
     * Sets margins and spacing for the given layout to zero.
     */
    protected GridLayout setZeroMargins(GridLayout aLayout)
    {
        aLayout.horizontalSpacing = 0;
        aLayout.verticalSpacing = 0;
        aLayout.marginBottom = 0;
        aLayout.marginTop = 0;
        aLayout.marginHeight = 0;
        aLayout.marginWidth = 0;
        aLayout.marginLeft = 0;
        aLayout.marginRight = 0;
        return aLayout;
    }

    protected void log(String aMsg)
    {
        if (iInstallerUi != null)
        {
            iInstallerUi.log(aMsg);
        }
    }

    protected void logError(String aMsg, Throwable aThrowable)
    {
        if (iInstallerUi != null)
        {
            iInstallerUi.logError(aMsg, aThrowable);
        }
    }

    /**
     * Opens certificate information view.
     */
    protected void openCertsView(SigningCertificate[] aCerts, int aCertIndex)
    {
        iCertificateDetailsView = new CertificateDetailsView(
            iInstallerUi, getShell(), getTitle(), aCerts, aCertIndex);
        // New dialog must be opened in a new thread,
        // because we cannot use the main thread and we
        // cannot block the UI main thread
        // where this method gets executed.
        new Thread(new Runnable()
        {
            public void run()
            {
                if (iCertificateDetailsView.confirm())
                {
                    log("certificateDetailsView confirmed");
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
