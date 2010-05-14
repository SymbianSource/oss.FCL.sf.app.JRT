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

import com.nokia.mj.impl.utils.ResourceLoader;

import java.util.Date;
import java.util.Hashtable;

/**
 * Class defining InstallerUi texts.
 */
public class InstallerUiTexts
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    // Define constants for logical names of localised strings.
    public static final String OK = "ok";
    public static final String CANCEL = "cancel";
    public static final String BACK = "back";
    public static final String DETAILS = "failed_details";
    public static final String INSTALL = "progress";
    public static final String UNINSTALL = "uninstalling_progress";
    public static final String START = "start";
    public static final String SHOW = "show";
    public static final String CLOSE = "close";
    public static final String INSTALL_QUERY = "query";
    public static final String UPDATE_QUERY = "update_query";
    public static final String UNINSTALL_QUERY = "uninstall_query";
    public static final String DETAILS_QUERY = "view_app_details";
    public static final String APP_CONVERSION_PROGRESS = "app_conversion_progress";
    public static final String OCSP_CHECK_PROGRESS = "ocsp_check_progress";
    public static final String INSTALLING = "progress";
    public static final String UNINSTALLING = "uninstalling_progress";
    public static final String INSTALL_FAILED = "failed";
    public static final String UNINSTALL_FAILED = "uninst_failed";
    public static final String INSTALLATION_COMPLETE = "complete";
    public static final String INSTALLED_TO = "installed_to";
    public static final String DOWNLOADING = "download_progress";
    public static final String CONNECT_TO = "auth_query";
    public static final String DOWNLOAD_APPLICATION = "auth_yourself";
    public static final String USERNAME = "auth_username";
    public static final String PASSWORD = "auth_password";
    public static final String APP_DETAILS = "view_app_details";
    public static final String NAME = "app_details_name";
    public static final String VENDOR = "app_details_vendor";
    public static final String VERSION = "app_details_version";
    public static final String SIZE = "app_details_size_kb";
    public static final String APPLICATIONS = "app_details_apps_list";
    public static final String APP_NAME = "application_name";
    public static final String NOT_CERTIFIED_INFO = "view_not_certified_details";
    public static final String NOT_CERTIFIED_TITLE = "not_certified_title";
    public static final String NOT_CERTIFIED_WARNING = "not_certified_warning";
    public static final String CERTIFICATE_INFO = "view_cert_details";
    public static final String CERTIFICATE_INFO_FOR_APP = "cert_details_title";
    public static final String DOMAIN = "cert_details_domain";
    public static final String DOMAIN_MANU = "cert_details_domain_manufacturer";
    public static final String DOMAIN_OPER = "cert_details_domain_operator";
    public static final String DOMAIN_ITP = "cert_details_domain_trusted_third_party";
    public static final String DOMAIN_UTP = "cert_details_domain_untrusted_third_party";
    public static final String ISSUER = "cert_details_issuer";
    public static final String SUBJECT = "cert_details_subject";
    public static final String VALID_FROM = "cert_details_valid_from";
    public static final String VALID_UNTIL = "cert_details_valid_until";
    public static final String SERIAL_NUMBER = "cert_details_serial_number";
    public static final String FINGERPRINT = "cert_details_fingerprint";
    public static final String INSTALL_TO_DRIVE = "app_details_install_to_drive";
    public static final String INSTALL_TO_GROUP = "app_details_install_to_group";
    public static final String RETAIN_USER_DATA = "update_retain_user_data";
    public static final String DRIVE_TYPE_UNKNOWN = "drive_type_unknown";
    public static final String DRIVE_TYPE_UNKNOWN_KB = "drive_type_unknown_kb";
    public static final String DRIVE_TYPE_UNKNOWN_MB = "drive_type_unknown_mb";
    public static final String DRIVE_TYPE_UNKNOWN_GB = "drive_type_unknown_gb";
    public static final String DRIVE_TYPE_PHONE_MEMORY = "drive_type_phone_memory";
    public static final String DRIVE_TYPE_PHONE_MEMORY_KB = "drive_type_phone_memory_kb";
    public static final String DRIVE_TYPE_PHONE_MEMORY_MB = "drive_type_phone_memory_mb";
    public static final String DRIVE_TYPE_PHONE_MEMORY_GB = "drive_type_phone_memory_gb";
    public static final String DRIVE_TYPE_MEMORY_CARD = "drive_type_memory_card";
    public static final String DRIVE_TYPE_MEMORY_CARD_KB = "drive_type_memory_card_kb";
    public static final String DRIVE_TYPE_MEMORY_CARD_MB = "drive_type_memory_card_mb";
    public static final String DRIVE_TYPE_MEMORY_CARD_GB = "drive_type_memory_card_gb";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE = "drive_type_internal_mass_memory";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_KB = "drive_type_internal_mass_memory_kb";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_MB = "drive_type_internal_mass_memory_mb";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_GB = "drive_type_internal_mass_memory_gb";
    // Texts for permission confirmation view.
    public static final String PERM_QUERY = "perm_query";
    public static final String PERM_VIEW_DETAILS = "perm_view_details";
    public static final String PERM_VIEW_DETAILS_TITLE = "perm_view_details_title";
    public static final String PERM_ALLOW_ALWAYS = "perm_allow_always";
    public static final String PERM_ASK_ME_LATER = "perm_ask_me_later";

    /**
     * Method for retrieving a text string with given id and no parameters.
     *
     * @param aTextId id for the text string
     * @return The corresponding text
     */
    public static String get(String aTextId)
    {
        return get(aTextId, (Object[])null);
    }

    /**
     * Method for retrieving a text string with given id and parameters.
     *
     * @param aTextId id for the text string
     * @param aTextParameters parameters to be filled into the text
     * @return The corresponding text including the provided parameters
     */
    public static String get(String aTextId, Object[] aTextParameters)
    {
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance("javainstallation", "qtn_java_inst_");
        }
        return iRes.format(aTextId, aTextParameters);
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /** ResourceLoader for fetching localised texts. */
    private static ResourceLoader iRes = null;

    /*** ----------------------------- NATIVE ----------------------------- */
}
