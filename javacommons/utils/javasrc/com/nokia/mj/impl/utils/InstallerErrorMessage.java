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

package com.nokia.mj.impl.utils;

import com.nokia.mj.impl.utils.ResourceLoader;
import java.util.Hashtable;

/**
 * Class defining Installer short error messages.
 */
public final class InstallerErrorMessage extends ErrorMessageBase
{
    // Installation error codes.
    public static final int INST_NO_MEM = 1 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int INST_NO_NET = 2 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int INST_CORRUPT_PKG = 3 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int INST_COMPAT_ERR = 4 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int INST_AUTHORIZATION_ERR = 5 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int INST_AUTHENTICATION_ERR = 6 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int INST_PUSH_REG_ERR = 7 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int INST_UNEXPECTED_ERR = 8 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int INST_CANCEL = 9 + ErrorMessageBase.INSTALLER_RANGE_START;

    // Uninstallation error codes.
    public static final int UNINST_NOT_ALLOWED = 10 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int UNINST_UNEXPECTED_ERR = 11 + ErrorMessageBase.INSTALLER_RANGE_START;
    public static final int UNINST_CANCEL = 12 + ErrorMessageBase.INSTALLER_RANGE_START;

    /**
     * Error codes for other than install and uninstall operations.
     * Do not use these error codes in installation or uninstallation.
     * These are only used in installer "list" and "test" commands.
     */
    public static final int OTHER_UNEXPECTED_ERR = 13 + ErrorMessageBase.INSTALLER_RANGE_START;

    /*** ----------------------------- PUBLIC ------------------------------ */
    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Method for getting message table where key is an Integer value for
     * the error code, and value is an error message String id.
     */
    private static Hashtable iMessageTable = null;
    protected Hashtable getMessageTable()
    {
        if (iMessageTable != null)
        {
            return iMessageTable;
        }
        Hashtable messageTable = new Hashtable();
        // Change this after Qt localisation files are in place.
        //if (ResourceLoader.getLocaleIdQt() == null)
        if (true)
        {
            messageTable.put(new Integer(INST_NO_MEM), "no_mem");
            messageTable.put(new Integer(INST_NO_NET), "no_net");
            messageTable.put(new Integer(INST_CORRUPT_PKG), "corrupt_pkg");
            messageTable.put(new Integer(INST_COMPAT_ERR), "compat_err");
            messageTable.put(new Integer(INST_AUTHORIZATION_ERR), "authorization_err");
            messageTable.put(new Integer(INST_AUTHENTICATION_ERR), "authentication_err");
            messageTable.put(new Integer(INST_PUSH_REG_ERR), "push_reg_err");
            messageTable.put(new Integer(INST_UNEXPECTED_ERR), "unexpected");
            messageTable.put(new Integer(INST_CANCEL), "cancel");
            messageTable.put(new Integer(UNINST_NOT_ALLOWED), "uninst_not_allowed");
            messageTable.put(new Integer(UNINST_UNEXPECTED_ERR), "uninst_unexpected");
            messageTable.put(new Integer(UNINST_CANCEL), "uninst_cancel");
            messageTable.put(new Integer(OTHER_UNEXPECTED_ERR), "other_unexpected");
        }
        else
        {
            messageTable.put(new Integer(INST_NO_MEM), "info_there_is_not_enough_space_currently");
            messageTable.put(new Integer(INST_NO_NET), "info_network_is_unavailable_currently");
            messageTable.put(new Integer(INST_CORRUPT_PKG), "info_installation_package_is_invalid");
            messageTable.put(new Integer(INST_COMPAT_ERR), "info_application_is_not_compatible_with");
            messageTable.put(new Integer(INST_AUTHORIZATION_ERR), "info_there_is_a_security_issue_with_this");
            messageTable.put(new Integer(INST_AUTHENTICATION_ERR), "info_there_is_a_security_issue_with_this");
            messageTable.put(new Integer(INST_PUSH_REG_ERR), "info_an_unexpected_error_occurred");
            messageTable.put(new Integer(INST_UNEXPECTED_ERR), "info_an_unexpected_error_occurred");
            messageTable.put(new Integer(INST_CANCEL), "info_application_not_installed");
            messageTable.put(new Integer(UNINST_NOT_ALLOWED), "info_application_cannot_be_deleted");
            messageTable.put(new Integer(UNINST_UNEXPECTED_ERR), "installer_info_uninstallation_failed");
            messageTable.put(new Integer(UNINST_CANCEL), "info_application_not_deleted");
            messageTable.put(new Integer(OTHER_UNEXPECTED_ERR), "info_an_unexpected_error_occurred");
        }
        iMessageTable = messageTable;
        return iMessageTable;
    }

    /**
     * Method for retrieving the ResourceLoader instance that is used
     * to localise error messages.
     */
    private static ResourceLoader iRes = null;
    protected ResourceLoader getResourceLoader()
    {
        if (iRes == null)
        {
            // Change this after Qt localisation files are in place.
            //if (ResourceLoader.getLocaleIdQt() == null)
            if (true)
            {
                iRes = ResourceLoader.getInstance("javainstallation", "qtn_java_inst_error_");
            }
            else
            {
                iRes = ResourceLoader.getInstance("common_errors", "txt_error_");
            }
        }
        return iRes;
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
