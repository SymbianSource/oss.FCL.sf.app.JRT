/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.security.utils;

import com.nokia.mj.impl.utils.ErrorMessageBase;
import com.nokia.mj.impl.utils.ResourceLoader;
import java.util.Hashtable;

/**
 * Class defining Security detailed error messages.
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class SecurityDetailedErrorMessage extends ErrorMessageBase
{
    // Security related detailed error codes.
    public static final int JAR_TAMPERED = 1;
    public static final int CERT_DISABLED = 2;
    public static final int CERT_DELETED = 3;
    public static final int SIM_CHANGED = 4;
    public static final int UNIDENTIFIED_APPLICATION = 5;
    public static final int NETWORK_RESTRICTION_VIOLATION = 6;
    public static final int OCSP_GENERAL_ERR = 7;
    public static final int OCSP_SETTINGS_ERR = 8;
    public static final int OCSP_REVOKED_ERR = 9;


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
        messageTable.put(new Integer(JAR_TAMPERED), "jar_tampered_details");
        messageTable.put(new Integer(CERT_DISABLED), "cert_disabled");
        messageTable.put(new Integer(CERT_DELETED), "cert_deleted");
        messageTable.put(new Integer(SIM_CHANGED), "sim_changed");
        messageTable.put(new Integer(UNIDENTIFIED_APPLICATION), "unidentified_application");
        messageTable.put(new Integer(NETWORK_RESTRICTION_VIOLATION), "net_restr_violation_details");
        messageTable.put(new Integer(OCSP_GENERAL_ERR), "ocsp_general_details");
        messageTable.put(new Integer(OCSP_SETTINGS_ERR), "ocsp_settings");
        messageTable.put(new Integer(OCSP_REVOKED_ERR), "ocsp_revoked");
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
            iRes = ResourceLoader.getInstance("javausermessages", "qtn_java_secur_error_");
        }
        return iRes;
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
