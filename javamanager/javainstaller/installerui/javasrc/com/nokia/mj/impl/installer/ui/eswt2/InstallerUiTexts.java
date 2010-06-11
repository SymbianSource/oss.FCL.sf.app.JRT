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

    // Define constants for localised text ids.
    public static final String OK = "Ok";
    public static final String CANCEL = "Cancel";
    public static final String BACK = "Ok";
    public static final String HIDE = "Hide";
    public static final String SHOW = "Show";
    public static final String CLOSE = "Close";
    public static final String INSTALL_QUERY = "Install?";
    public static final String UPDATE_QUERY = "Update?";
    public static final String OCSP_CHECK_PROGRESS = "Checking certificate validity";
    public static final String INSTALLING = "Installing";
    public static final String INSTALL_FAILED = "Installation failed";
    public static final String INSTALLATION_COMPLETE = "Installed";
    public static final String DOWNLOADING = "Downloading";
    public static final String CONNECT_TO = "Connect to";
    public static final String DOWNLOAD_APPLICATION = "To download %U you need to authenticate yourself";
    public static final String USERNAME = "Username:";
    public static final String PASSWORD = "Password:";
    public static final String SUITE_NAME = "%U (%U)";
    public static final String SUITE_VENDOR = "%U";
    public static final String SUITE_VERSION = "%U";
    public static final String APP_NAME = "%U";
    public static final String SIZE_KB = "%U kB";
    public static final String SIZE_MB = "%U MB";
    public static final String RETAIN_USER_DATA = "Retain application data";
    // Certificate info texts.
    public static final String NOT_CERTIFIED_TITLE = "Application is not certified.";
    public static final String NOT_CERTIFIED_INFO = "Application %U is from an unknown source.";
    public static final String CERTIFICATE_TITLE = "Application is certified.";
    public static final String DOMAIN = "Domain: %U";
    public static final String DOMAIN_MANU = "Manufacturer";
    public static final String DOMAIN_OPER = "Operator";
    public static final String DOMAIN_ITP = "Trusted 3rd party";
    public static final String DOMAIN_UTP = "Untrusted 3rd party";
    public static final String ISSUER = "Issuer: %U";
    public static final String SUBJECT = "Subject: %U";
    public static final String ORGANIZATION = "Organization: %U";
    public static final String VALID_FROM = "Valid from: %U";
    public static final String VALID_UNTIL = "Valid until: %U";
    public static final String SERIAL_NUMBER = "Serial number: %U";
    public static final String FINGERPRINT = "Fingerprint: %U";
    // Texts for installation drive selection.
    public static final String DRIVE_TYPE_UNKNOWN = "%U: Unknown";
    public static final String DRIVE_TYPE_UNKNOWN_KB = "%U: Unknown (%N kB)";
    public static final String DRIVE_TYPE_UNKNOWN_MB = "%U: Unknown (%N MB)";
    public static final String DRIVE_TYPE_UNKNOWN_GB = "%U: Unknown (%N GB)";
    public static final String DRIVE_TYPE_PHONE_MEMORY = "%U: Phone memory";
    public static final String DRIVE_TYPE_PHONE_MEMORY_KB = "%U: Phone memory (%N kB)";
    public static final String DRIVE_TYPE_PHONE_MEMORY_MB = "%U: Phone memory (%N MB)";
    public static final String DRIVE_TYPE_PHONE_MEMORY_GB = "%U: Phone memory (%N GB)";
    public static final String DRIVE_TYPE_MEMORY_CARD = "%U: Memory card";
    public static final String DRIVE_TYPE_MEMORY_CARD_KB = "%U: Memory card (%N kB)";
    public static final String DRIVE_TYPE_MEMORY_CARD_MB = "%U: Memory card (%N MB)";
    public static final String DRIVE_TYPE_MEMORY_CARD_GB = "%U: Memory card (%N GB)";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE = "%U: Internal Mass Storage";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_KB = "%U: Internal Mass Storage (%N kB)";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_MB = "%U: Internal Mass Storage (%N MB)";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_GB = "%U: Internal Mass Storage (%N GB)";
    // Texts for permission confirmation view.
    public static final String PERM_QUERY = "Access for protected functionality needed.";
    public static final String PERM_VIEW_DETAILS = "Details";
    public static final String PERM_ALLOW_ALWAYS = "Allow always";
    public static final String PERM_ASK_ME_LATER = "Ask me later";
    public static final String PERM_CANCEL = "Cancel installing";
    public static final String PERM_VIEW_DETAILS_TITLE = "Application asks permissions for:";

    /*
    // Define constants for localised text ids.
    public static final String OK = "button_ok";
    public static final String CANCEL = "button_cancel";
    public static final String BACK = "button_back_ok";
    public static final String HIDE = "button_hide";
    public static final String SHOW = "button_show";
    public static final String CLOSE = "button_close";
    public static final String INSTALL_QUERY = "title_install";
    public static final String UPDATE_QUERY = "title_update_query";
    public static final String OCSP_CHECK_PROGRESS = "ocsp_check_progress";
    public static final String INSTALLING = "title_installing";
    public static final String INSTALL_FAILED = "title_inst_failed";
    public static final String INSTALLATION_COMPLETE = "title_installation_complete";
    public static final String DOWNLOADING = "title_downloading";
    public static final String CONNECT_TO = "title_auth_query";
    public static final String DOWNLOAD_APPLICATION = "info_auth_yourself";
    public static final String USERNAME = "dialog_username";
    public static final String PASSWORD = "dialog_password";
    public static final String SUITE_NAME = "info_app_suite_name";
    public static final String SUITE_VENDOR = "info_vendor";
    public static final String SUITE_VERSION = "info_version";
    public static final String APP_NAME = "info_app_name";
    public static final String SIZE_KB = "info_size_kb";
    public static final String SIZE_MB = "info_size_mb";
    public static final String RETAIN_USER_DATA = "info_update_retain_user_data";
    // Certificate info texts.
    public static final String NOT_CERTIFIED_TITLE = "title_app_not_cert";
    public static final String NOT_CERTIFIED_INFO = "info_inst_pkg_not_cert";
    public static final String CERTIFICATE_TITLE = "title_app_cert";
    public static final String DOMAIN = "setlabel_cert_domain";
    public static final String DOMAIN_MANU = "setlabel_cert_domain_val_manufacturer";
    public static final String DOMAIN_OPER = "setlabel_cert_domain_val_operator";
    public static final String DOMAIN_ITP = "setlabel_cert_domain_val_trusted_third_party";
    public static final String DOMAIN_UTP = "setlabel_cert_domain_val_untrusted_third_party";
    public static final String ISSUER = "setlabel_issuer";
    public static final String SUBJECT = "setlabel_subject";
    public static final String ORGANIZATION = "setlabel_organization";
    public static final String VALID_FROM = "setlabel_valid_from";
    public static final String VALID_UNTIL = "setlabel_valid_until";
    public static final String SERIAL_NUMBER = "setlabel_serial_number";
    public static final String FINGERPRINT = "setlabel_fingerprint";
    // Texts for installation drive selection.
    public static final String DRIVE_TYPE_UNKNOWN = "list_unknown";
    public static final String DRIVE_TYPE_UNKNOWN_KB = "list_unknown_kb";
    public static final String DRIVE_TYPE_UNKNOWN_MB = "list_unknown_mb";
    public static final String DRIVE_TYPE_UNKNOWN_GB = "list_unknown_gb";
    public static final String DRIVE_TYPE_PHONE_MEMORY = "list_phone_mem";
    public static final String DRIVE_TYPE_PHONE_MEMORY_KB = "list_phone_mem_kb";
    public static final String DRIVE_TYPE_PHONE_MEMORY_MB = "list_phone_mem_mb";
    public static final String DRIVE_TYPE_PHONE_MEMORY_GB = "list_phone_mem_gb";
    public static final String DRIVE_TYPE_MEMORY_CARD = "list_mem_card";
    public static final String DRIVE_TYPE_MEMORY_CARD_KB = "list_mem_card_kb";
    public static final String DRIVE_TYPE_MEMORY_CARD_MB = "list_mem_card_mb";
    public static final String DRIVE_TYPE_MEMORY_CARD_GB = "list_mem_card_gb";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE = "list_mass_mem";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_KB = "list_mass_mem_kb";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_MB = "list_mass_mem_mb";
    public static final String DRIVE_TYPE_INTERNAL_MASS_STORAGE_GB = "list_mass_mem_gb";
    // Texts for permission confirmation view.
    public static final String PERM_QUERY = "info_perm_query";
    public static final String PERM_VIEW_DETAILS = "button_view_perm_details";
    public static final String PERM_ALLOW_ALWAYS = "button_perm_allow_always";
    public static final String PERM_ASK_ME_LATER = "button_perm_ask_me_later";
    public static final String PERM_CANCEL = "button_perm_cancel_installing";
    public static final String PERM_VIEW_DETAILS_TITLE = "title_perm_view_details";
    */

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
        return new com.nokia.mj.impl.utils.Formatter(aTextId).format(aTextParameters);
        //if (iRes == null)
        //{
        //    iRes = ResourceLoader.getInstance("javaapplicationinstaller", "txt_java_inst_");
        //}
        //return iRes.format(aTextId, aTextParameters);
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /** ResourceLoader for fetching localised texts. */
    private static ResourceLoader iRes = null;

    /*** ----------------------------- NATIVE ----------------------------- */
}
