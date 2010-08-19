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
 * Class defining Installer detailed error messages.
 */
public final class InstallerDetailedErrorMessage extends ErrorMessageBase
{
    // Installation and uninstallation related detailed error codes.
    public static final int NO_MEM = 1;
    public static final int NO_NET = 2;
    public static final int ATTR_BAD_SYNTAX = 3;
    public static final int ATTR_MISSING = 4;
    public static final int ATTR_MISMATCH = 5;
    public static final int ATTR_UNSUPPORTED = 6;
    public static final int ATTR_HANDLING_FAILED = 7;
    public static final int CERT_UNSUPPORTED = 8;
    public static final int PROT_PKG_MISUSE = 9;
    public static final int DIFFERENT_SIGNERS = 10;
    public static final int UNINST_BLOCKED = 11;
    public static final int INTERNAL_ERROR = 12;
    public static final int NO_MEM_MB = 13;
    public static final int INST_CANCEL = 14;
    public static final int UNINST_CANCEL = 15;

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
        if (getLocaleIdQt() == null)
        {
            messageTable.put(new Integer(NO_MEM), "no_mem_detail");
            messageTable.put(new Integer(NO_MEM_MB), "no_mem_detail_mb");
            messageTable.put(new Integer(NO_NET), "no_net_detail");
            messageTable.put(new Integer(ATTR_BAD_SYNTAX), "attr_bad_syntax");
            messageTable.put(new Integer(ATTR_MISSING), "attr_missing");
            messageTable.put(new Integer(ATTR_MISMATCH), "attr_mismatch");
            messageTable.put(new Integer(ATTR_UNSUPPORTED), "attr_unsupported_value");
            messageTable.put(new Integer(ATTR_HANDLING_FAILED), "attr_handling_failed");
            messageTable.put(new Integer(CERT_UNSUPPORTED), "cert_unsupported");
            messageTable.put(new Integer(PROT_PKG_MISUSE), "protected_pkg_misuse");
            messageTable.put(new Integer(DIFFERENT_SIGNERS), "different_signers");
            messageTable.put(new Integer(UNINST_BLOCKED), "uninst_blocked");
            messageTable.put(new Integer(INTERNAL_ERROR), "internal");
            messageTable.put(new Integer(INST_CANCEL), "Installation cancelled.");
            messageTable.put(new Integer(UNINST_CANCEL), "Uninstallation cancelled.");
        }
        else
        {
            messageTable.put(new Integer(NO_MEM), "error_no_mem_detail");
            messageTable.put(new Integer(NO_MEM_MB), "error_no_mem_detail_mb");
            messageTable.put(new Integer(NO_NET), "error_no_net_detail");
            messageTable.put(new Integer(ATTR_BAD_SYNTAX), "error_bad_syntax");
            messageTable.put(new Integer(ATTR_MISSING), "error_attr_missing");
            messageTable.put(new Integer(ATTR_MISMATCH), "error_attr_mismatch");
            messageTable.put(new Integer(ATTR_UNSUPPORTED), "error_attr_unsupported_value");
            messageTable.put(new Integer(ATTR_HANDLING_FAILED), "error_attr_handling_failed");
            messageTable.put(new Integer(CERT_UNSUPPORTED), "error_cert_unsupported");
            messageTable.put(new Integer(PROT_PKG_MISUSE), "error_pkg_misuse");
            messageTable.put(new Integer(DIFFERENT_SIGNERS), "error_different_signers");
            messageTable.put(new Integer(UNINST_BLOCKED), "uninst_blocked");
            messageTable.put(new Integer(INTERNAL_ERROR), "internal_error");
            messageTable.put(new Integer(INST_CANCEL), "error_cancel");
            messageTable.put(new Integer(UNINST_CANCEL), "error_uninst_cancel");
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
            if (getLocaleIdQt() == null)
            {
                iRes = ResourceLoader.getInstance("javainstallation", "qtn_java_inst_error_");
            }
            else
            {
                iRes = ResourceLoader.getInstance("javaapplicationinstaller", "txt_java_inst_info_");
            }
        }
        return iRes;
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
