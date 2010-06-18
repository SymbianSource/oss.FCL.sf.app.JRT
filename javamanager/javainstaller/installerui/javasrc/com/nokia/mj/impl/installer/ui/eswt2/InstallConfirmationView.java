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

import java.io.InputStream;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

/**
 * InstallConfirmationView asks installation confirmation
 * from the user.
 */
public class InstallConfirmationView extends ConfirmationViewBase
{
    private InstallInfo iInstallInfo = null;
    private Combo iDriveSelector = null;
    private Button iRetainDataButton = null;

    /** Constructor */
    protected InstallConfirmationView()
    {
        super();
    }

    /** Constructor */
    protected InstallConfirmationView(
        InstallerUiEswt aInstallerUi, Composite aParent)
    {
        super(aInstallerUi, aParent, 8);
        setTitle(InstallerUiTexts.get(InstallerUiTexts.INSTALLING));
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
        // Add header.
        String title = InstallerUiTexts.get(InstallerUiTexts.INSTALL_QUERY);
        if (iInstallInfo != null)
        {
            if (iInstallInfo.getOldVersion() != null)
            {
                title = InstallerUiTexts.get(InstallerUiTexts.UPDATE_QUERY);
            }
        }
        addHeader(title, iInstallInfo, null);

        GridData gridData = null;
        int horizontalSpan = getColumns();
        int labelStyle = SWT.WRAP;

        // Begin user selection widgets creation.
        int[] driveIds = iInstallInfo.getDriveIds();
        if (driveIds != null && driveIds.length > 0)
        {
            // Add installation drive selector.
            iDriveSelector = new Combo(getComposite(), SWT.READ_ONLY);
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

        // After other widgets have been added, add content to
        // application info Composite.
        addAppInfo(iInstallInfo, true);
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
