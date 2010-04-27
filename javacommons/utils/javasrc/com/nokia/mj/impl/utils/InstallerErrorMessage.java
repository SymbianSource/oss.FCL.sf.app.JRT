/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class InstallerErrorMessage extends ErrorMessageBase
{
    // Installation error codes.
    public static final int INST_NO_MEM = 1;
    public static final int INST_NO_NET = 2;
    public static final int INST_CORRUPT_PKG = 3;
    public static final int INST_COMPAT_ERR = 4;
    public static final int INST_AUTHORIZATION_ERR = 5;
    public static final int INST_AUTHENTICATION_ERR = 6;
    public static final int INST_PUSH_REG_ERR = 7;
    public static final int INST_UNEXPECTED_ERR = 8;
    public static final int INST_CANCEL = 9;

    // Uninstallation error codes.
    public static final int UNINST_NOT_ALLOWED = 101;
    public static final int UNINST_UNEXPECTED_ERR = 102;
    public static final int UNINST_CANCEL = 103;

    /**
     * Error codes for other than install and uninstall operations.
     * Do not use these error codes in installation or uninstallation.
     * These are only used in installer "list" and "test" commands.
     */
    public static final int OTHER_UNEXPECTED_ERR = 201;

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
            iRes = ResourceLoader.getInstance("javainstallation", "qtn_java_inst_error_");
        }
        return iRes;
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
