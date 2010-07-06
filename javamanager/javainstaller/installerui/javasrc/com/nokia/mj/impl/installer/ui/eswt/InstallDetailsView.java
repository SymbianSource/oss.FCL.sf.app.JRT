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
import com.nokia.mj.impl.installer.ui.InstallInfo;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;

/**
 * InstallDetailsView displays installation details to the user.
 */
public class InstallDetailsView extends ConfirmationViewBase
{
    private CertificateDetailsView iCertificateDetailsView = null;
    private InstallInfo iInstallInfo = null;
    /** Application suite icon */
    private Image iSuiteIcon = null;
    private Combo iDriveSelector = null;
    //private List iDriveSelector = null;
    private Button iRetainDataButton = null;

    /** Constructor */
    protected InstallDetailsView()
    {
        super();
    }

    /** Constructor */
    protected InstallDetailsView(InstallerUiEswt aInstaller, Composite aParent,
                                 String aTitle,
                                 InstallInfo aInstallInfo,
                                 Image aSuiteIcon)
    {
        super(aInstaller, aParent, 8);
        iInstallInfo = aInstallInfo;
        iSuiteIcon = aSuiteIcon;
        setTitle(aTitle);
        setCommands(InstallerUiTexts.get(InstallerUiTexts.OK),
                    InstallerUiTexts.get(InstallerUiTexts.CANCEL));
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

        // Add suite icon.
        if (iSuiteIcon != null)
        {
            try
            {
                Label iconLabel = createLabel(iSuiteIcon, 2, SWT.NONE);
                horizontalSpan = getColumns() - 3;
            }
            catch (Throwable t)
            {
                horizontalSpan = getColumns() - 1;
                logError("Exception while setting icon", t);
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

        Label nameLabel = createLabel
                          (InstallerUiTexts.get
                           (InstallerUiTexts.NAME,
                            new String[] { iInstallInfo.getName() }),
                           horizontalSpan, labelStyle);

        Label vendorLabel = createLabel
            (InstallerUiTexts.get
             (InstallerUiTexts.VENDOR,
              new String[] { iInstallInfo.getVendor() }),
             horizontalSpan, labelStyle);

        Label versionLabel = createLabel
                             (InstallerUiTexts.get
                              (InstallerUiTexts.VERSION,
                               new String[] { iInstallInfo.getVersion() }),
                              horizontalSpan, labelStyle);

        long size = 0;
        if (iInstallInfo.getJarSize() > 0)
        {
            size += iInstallInfo.getJarSize();
        }
        if (iInstallInfo.getDataSize() > 0)
        {
            size += iInstallInfo.getDataSize();
        }
        if (size > 0)
        {
            Label sizeLabel = createLabel
                              (InstallerUiTexts.get
                               (InstallerUiTexts.SIZE,
                                new String[] { Long.toString(1 + size/1024) }),
                               horizontalSpan, labelStyle);
        }

        ApplicationInfo[] apps = iInstallInfo.getApplications();
        if (apps != null && apps.length > 0)
        {
            if (apps.length > 1 ||
                    !iInstallInfo.getName().equals(apps[0].getName()))
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

        // Begin user selection widgets creation.
        int[] driveIds = iInstallInfo.getDriveIds();
        if (driveIds != null && driveIds.length > 0)
        {
            // Add installation drive selector.
            Label drivesLabel = createLabel(
                                    InstallerUiTexts.get(InstallerUiTexts.INSTALL_TO_DRIVE),
                                    horizontalSpan, labelStyle);
            iDriveSelector = new Combo(getComposite(), SWT.READ_ONLY);
            //iDriveSelector = new List(getComposite(), SWT.SINGLE);
            gridData = new GridData(GridData.FILL_HORIZONTAL);
            gridData.horizontalSpan = horizontalSpan;
            iDriveSelector.setLayoutData(gridData);
            int[] driveTypes = iInstallInfo.getDriveTypes();
            long[] driveFreeSpaces = iInstallInfo.getDriveFreeSpaces();
            int driveIndex = 0;
            if (driveTypes != null && driveFreeSpaces != null)
            {
                for (int i = 0; i < driveIds.length &&
                        i < driveTypes.length &&
                        i < driveFreeSpaces.length; i++)
                {
                    iDriveSelector.add(getDriveString(driveIds[i],
                                                      driveTypes[i],
                                                      driveFreeSpaces[i]));
                    if (iInstallInfo.getDriveId() == driveIds[i])
                    {
                        driveIndex = i;
                    }
                }
                iDriveSelector.select(driveIndex);
            }
            addSoftKeyListenerFor(iDriveSelector);
        }

        if (iInstallInfo.getRetainData() != null)
        {
            // Add retain user data option.
            iRetainDataButton = new Button(getComposite(), SWT.CHECK);
            gridData = new GridData(GridData.FILL_HORIZONTAL);
            gridData.horizontalSpan = horizontalSpan;
            iRetainDataButton.setLayoutData(gridData);
            iRetainDataButton.setSelection
            (iInstallInfo.getRetainData().booleanValue());
            iRetainDataButton.setText(InstallerUiTexts.get
                                      (InstallerUiTexts.RETAIN_USER_DATA));
            addSoftKeyListenerFor(iRetainDataButton);
        }
        // End of user selection widgets creation.

        // Add links to certificates.
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
        else
        {
            // Add not certified application link.
            Link certLink = new Link(getComposite(), SWT.NONE);
            certLink.setText("<a>" +
                             InstallerUiTexts.get(
                                 InstallerUiTexts.NOT_CERTIFIED_INFO) +
                             "</a>");
            certLink.addListener(SWT.Selection, new Listener()
            {
                public void handleEvent(Event aEvent)
                {
                    openCertsView(-1);
                }
            });
            gridData = new GridData(GridData.FILL_HORIZONTAL);
            gridData.horizontalSpan = horizontalSpan;
            certLink.setLayoutData(gridData);
            addSoftKeyListenerFor(certLink);
        }
    }

    /**
     * This method is called after user has answered
     * to confirmation.
     */
    protected void getDataFromView()
    {
        if (iDriveSelector != null && !iDriveSelector.isDisposed())
        {
            int selectionIndex = iDriveSelector.getSelectionIndex();
            int drive = -1;
            if (selectionIndex >= 0)
            {
                drive = iInstallInfo.getDriveIds()[selectionIndex];
            }
            iInstallInfo.setDriveId(drive);
            log("Selected drive: " + drive);
        }
        if (iRetainDataButton != null && !iRetainDataButton.isDisposed())
        {
            boolean selection = iRetainDataButton.getSelection();
            iInstallInfo.setRetainData(selection);
            log("RetainData: " + selection);
        }
    }

    /**
     * Returns SWT style for this view.
     */
    protected int getStyle()
    {
        return SWT.V_SCROLL;
    }

    /**
     * Opens certificate information view.
     */
    private void openCertsView(int aCertIndex)
    {
        iCertificateDetailsView = new CertificateDetailsView(
            iInstallerUi, getShell(),
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

    /**
     * Returns drive name to be displayed in the UI.
     */
    private String getDriveString(int aId, int aType, long aFreeSpace)
    {
        // Choose drive letter to be displayed.
        String driveLetter = "0";
        if (aId >= 0 || aId < 26)
        {
            driveLetter = new String(new char[] {(char)('A' + aId) });
        }

        // Choose amount of free space to be displayed and
        // kb, MB or GB prefix for disk space unit.
        int freeSpace = (int)aFreeSpace;
        int prefix = 0;
        if (aFreeSpace >= 1024*1024*1024)
        {
            freeSpace = (int)(aFreeSpace / (1024*1024*1024));
            prefix = 3;
        }
        else if (aFreeSpace >= 1024*1024)
        {
            freeSpace = (int)(aFreeSpace / (1024*1024));
            prefix = 2;
        }
        else
        {
            freeSpace = (int)(aFreeSpace / 1024);
            prefix = 1;
        }

        // Choose text id and text parameters.
        String textId = null;
        Object[] textParams = new Object[]
        {
            driveLetter, new Integer(freeSpace)
        };
        switch (aType)
        {
        case InstallInfo.DRIVE_TYPE_UNKNOWN:
            switch (prefix)
            {
            case 0:
                textId = InstallerUiTexts.DRIVE_TYPE_UNKNOWN;
                textParams = new String[] { driveLetter };
                break;
            case 1:
                textId = InstallerUiTexts.DRIVE_TYPE_UNKNOWN_KB;
                break;
            case 2:
                textId = InstallerUiTexts.DRIVE_TYPE_UNKNOWN_MB;
                break;
            case 3:
                textId = InstallerUiTexts.DRIVE_TYPE_UNKNOWN_GB;
                break;
            }
            break;
        case InstallInfo.DRIVE_TYPE_PHONE_MEMORY:
            switch (prefix)
            {
            case 0:
                textId = InstallerUiTexts.DRIVE_TYPE_PHONE_MEMORY;
                textParams = new String[] { driveLetter };
                break;
            case 1:
                textId = InstallerUiTexts.DRIVE_TYPE_PHONE_MEMORY_KB;
                break;
            case 2:
                textId = InstallerUiTexts.DRIVE_TYPE_PHONE_MEMORY_MB;
                break;
            case 3:
                textId = InstallerUiTexts.DRIVE_TYPE_PHONE_MEMORY_GB;
                break;
            }
            break;
        case InstallInfo.DRIVE_TYPE_MEMORY_CARD:
            switch (prefix)
            {
            case 0:
                textId = InstallerUiTexts.DRIVE_TYPE_MEMORY_CARD;
                textParams = new String[] { driveLetter };
                break;
            case 1:
                textId = InstallerUiTexts.DRIVE_TYPE_MEMORY_CARD_KB;
                break;
            case 2:
                textId = InstallerUiTexts.DRIVE_TYPE_MEMORY_CARD_MB;
                break;
            case 3:
                textId = InstallerUiTexts.DRIVE_TYPE_MEMORY_CARD_GB;
                break;
            }
            break;
        case InstallInfo.DRIVE_TYPE_INTERNAL_MASS_STORAGE:
            switch (prefix)
            {
            case 0:
                textId = InstallerUiTexts.DRIVE_TYPE_INTERNAL_MASS_STORAGE;
                textParams = new String[] { driveLetter };
                break;
            case 1:
                textId = InstallerUiTexts.DRIVE_TYPE_INTERNAL_MASS_STORAGE_KB;
                break;
            case 2:
                textId = InstallerUiTexts.DRIVE_TYPE_INTERNAL_MASS_STORAGE_MB;
                break;
            case 3:
                textId = InstallerUiTexts.DRIVE_TYPE_INTERNAL_MASS_STORAGE_GB;
                break;
            }
            break;
        }
        return InstallerUiTexts.get(textId, textParams);
    }
}
