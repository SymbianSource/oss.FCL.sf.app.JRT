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

import com.nokia.mj.impl.installer.ui.InstallerUi;
import com.nokia.mj.impl.installer.ui.InstallInfo;
import com.nokia.mj.impl.installer.ui.UninstallInfo;
import com.nokia.mj.impl.rt.ui.ConfirmData;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;

/**
 * Minimal UI for JavaInstaller. This UI will only show simplified
 * confirmation dialogs using RuntimeUi.
 *
 * @author Nokia Corporation
 * @version $Rev: 0 $
 */
public class MinimalUi extends InstallerUi
{
    /**
     * Constructor.
     */
    public MinimalUi()
    {
    }

    /**
     * Confirm installation. UI should display an installation
     * confirmation dialog to the user. UI must update
     * aInstallInfo basing on user selections.
     * This method blocks until user has answered to the dialog.
     *
     * @param aInstallInfo installation information
     * @return true if user has accepted installation, false otherwise
     */
    public boolean confirm(InstallInfo aInstallInfo)
    {
        return confirmStatic(aInstallInfo);
    }

    /**
     * Confirm uninstallation. UI should display an uninstallation
     * confirmation dialog to the user.
     * This method blocks until user has answered to the dialog.
     *
     * @param aUninstallInfo uninstallation information
     * @return true if user has accepted uninstallation, false otherwise
     */
    public boolean confirm(UninstallInfo aUninstallInfo)
    {
        return confirmStatic(aUninstallInfo);
    }

    /**
     * Confirm installation. UI should display an installation
     * confirmation dialog to the user. UI must update
     * aInstallInfo basing on user selections.
     * This method blocks until user has answered to the dialog.
     *
     * @param aInstallInfo installation information
     * @return true if user has accepted installation, false otherwise
     */
    public static boolean confirmStatic(InstallInfo aInstallInfo)
    {
        boolean identified = (aInstallInfo.getCertificates() != null);

        String question = InstallerUiTexts.get(
                              InstallerUiTexts.INSTALL_QUERY,
                              new String[] { aInstallInfo.getName() });
        if (aInstallInfo.getOldVersion() != null)
        {
            question = InstallerUiTexts.get(
                           InstallerUiTexts.UPDATE_QUERY,
                           new String[] { aInstallInfo.getName(),
                                          aInstallInfo.getOldVersion(),
                                          aInstallInfo.getVersion()
                                        });
        }

        ConfirmData confirmData = new ConfirmData(
            question, new String[]
            {
                InstallerUiTexts.get(InstallerUiTexts.OK),
                InstallerUiTexts.get(InstallerUiTexts.CANCEL)
            }, 0);

        RuntimeUi runtimeUi = RuntimeUiFactory.getRuntimeUi(identified);
        boolean result = false;
        if (runtimeUi.confirm(
                    InstallerUiTexts.get(InstallerUiTexts.INSTALLING),
                    confirmData))
        {
            result = (confirmData.getAnswer() == 0);
        }
        runtimeUi.destroy();
        return result;
    }

    /**
     * Confirm uninstallation. UI should display an uninstallation
     * confirmation dialog to the user.
     * This method blocks until user has answered to the dialog.
     *
     * @param aUninstallInfo uninstallation information
     * @return true if user has accepted uninstallation, false otherwise
     */
    public static boolean confirmStatic(UninstallInfo aUninstallInfo)
    {
        boolean identified = (aUninstallInfo.getCertificates() != null);

        String question = InstallerUiTexts.get(
                              InstallerUiTexts.UNINSTALL_QUERY,
                              new String[] { aUninstallInfo.getName() });

        ConfirmData confirmData = new ConfirmData(
            question, new String[]
            {
                InstallerUiTexts.get(InstallerUiTexts.OK),
                InstallerUiTexts.get(InstallerUiTexts.CANCEL)
            }, 0);

        RuntimeUi runtimeUi = RuntimeUiFactory.getRuntimeUi(identified);
        boolean result = false;
        if (runtimeUi.confirm(
                    InstallerUiTexts.get(InstallerUiTexts.UNINSTALLING),
                    confirmData))
        {
            result = (confirmData.getAnswer() == 0);
        }
        runtimeUi.destroy();
        return result;
    }
}
