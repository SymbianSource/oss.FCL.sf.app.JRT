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

package com.nokia.mj.impl.utils;

import java.util.Hashtable;

/**
 * Class defining OTA status codes and their mapping to OTA status
 * report messages.
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class OtaStatusCode
{
    /**
     * OTA status codes.
     */
    public static final int SUCCESS = 900;
    public static final int INSUFFICIENT_MEMORY = 901;
    public static final int USER_CANCELLED = 902;
    public static final int LOSS_OF_SERVICE = 903;
    public static final int JAR_SIZE_MISMATCH = 904;
    public static final int ATTRIBUTE_MISMATCH = 905;
    public static final int INVALID_DESCRIPTOR = 906;
    public static final int INVALID_JAR = 907;
    public static final int INCOMPATIBLE_CONFIGURATION_OR_PROFILE = 908;
    public static final int APPLICATION_AUTHENTICATION_FAILURE = 909;
    public static final int APPLICATION_AUTHORIZATION_FAILURE = 910;
    public static final int PUSH_REGISTRATION_FAILURE = 911;
    public static final int DELETION_NOTIFICATION = 912;
    public static final int RMS_INITIALIZATION_FAILURE = 913;
    public static final int APPLICATION_INTEGRITY_FAILURE = 914;
    public static final int ONE_OR_MORE_MISSING_LIBLETS = 915;
    public static final int CIRCULAR_LIBLET_DEPENDENCY = 916;
    public static final int LIBLET_NAMESPACE_COLLISION = 917;
    public static final int CONTENT_HANDLER_CONFLICTS = 938;
    public static final int CONTENT_HANDLER_INSTALL_FAILED = 939;
    public static final int NONACCEPTABLE_CONTENT = 953;
    // Non-standard status code to be used for internal errors.
    public static final int INTERNAL_ERROR = USER_CANCELLED;

    /**
     * Mapping between OTA status codes and OTA status report message bodys.
     */
    private static Hashtable messageTable = null;

    /**
     * Initialization of the mapping.
     */
    static
    {
        messageTable = new Hashtable();
        messageTable.put(new Integer(SUCCESS), "900 Success");
        messageTable.put(new Integer(INSUFFICIENT_MEMORY), "901 Insufficient Memory");
        messageTable.put(new Integer(USER_CANCELLED), "902 User Cancelled");
        messageTable.put(new Integer(LOSS_OF_SERVICE), "903 Loss of Service");
        messageTable.put(new Integer(JAR_SIZE_MISMATCH), "904 JAR Size Mismatch");
        messageTable.put(new Integer(ATTRIBUTE_MISMATCH), "905 Attribute Mismatch");
        messageTable.put(new Integer(INVALID_DESCRIPTOR), "906 Invalid Descriptor");
        messageTable.put(new Integer(INVALID_JAR), "907 Invalid JAR");
        messageTable.put(new Integer(INCOMPATIBLE_CONFIGURATION_OR_PROFILE), "908 Incompatible Configuration or Profile");
        messageTable.put(new Integer(APPLICATION_AUTHENTICATION_FAILURE), "909 Application Authentication Failure");
        messageTable.put(new Integer(APPLICATION_AUTHORIZATION_FAILURE), "910 Application Authorization Failure");
        messageTable.put(new Integer(PUSH_REGISTRATION_FAILURE), "911 Push registration failure");
        messageTable.put(new Integer(DELETION_NOTIFICATION), "912 Deletion Notification");
        messageTable.put(new Integer(RMS_INITIALIZATION_FAILURE), "913 RMS Initialization failure");
        messageTable.put(new Integer(APPLICATION_INTEGRITY_FAILURE), "914 Application Integrity failure");
        messageTable.put(new Integer(ONE_OR_MORE_MISSING_LIBLETS), "915 One or more missing LIBlets");
        messageTable.put(new Integer(CIRCULAR_LIBLET_DEPENDENCY), "916 Circular LIBlet dependency");
        messageTable.put(new Integer(LIBLET_NAMESPACE_COLLISION), "917 LIBlet namespace collision");
        messageTable.put(new Integer(CONTENT_HANDLER_CONFLICTS), "938 Content Handler Conflicts With Other Handlers");
        messageTable.put(new Integer(CONTENT_HANDLER_INSTALL_FAILED), "939 Content Handler Install Failed");
        messageTable.put(new Integer(NONACCEPTABLE_CONTENT), "953 Non-Acceptable Content");
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Returns OTA status report message body.
     *
     * @param otaCode The OTA status code whose message body is queried
     * @return The corresponding OTA status report message body
     */
    public static String getOtaStatusReportBody(int otaCode)
    {
        String result = (String)messageTable.get(new Integer(otaCode));
        if (result == null)
        {
            result = Integer.toString(otaCode);
        }
        return result;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */

}
