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


package com.nokia.mj.impl.storage;

/**
 * StorageNames interface holding Storage related constants.
 */
public interface StorageNames
{
    /**
     * Application package related attributes.
     */
    public static final String APPLICATION_PACKAGE_TABLE =
        "APPLICATION_PACKAGE";

    /**
     * Application related attributes.
     */
    public static final String APPLICATION_TABLE = "APPLICATION";

    /**
     * Application package attributes. All manifest attributes are stored here.
     */
    public static final String APPLICATION_PACKAGE_ATTRIBUTES_TABLE =
        "APPLICATION_PACKAGE_ATTRIBUTES";

    /**
     * MIDP package related settings.
     */
    public static final String MIDP_PACKAGE_TABLE = "MIDP_PACKAGE";

    /**
     * Application permissions.
     */
    public static final String  MIDP_PERMISSIONS_TABLE = "MIDP_PERMISSIONS";

    /**
     * Security settings.
     */
    public static final String MIDP_FUNC_GRP_SETTINGS_TABLE =
        "MIDP_FUNCTION_GRP_SETTINGS";

    /**
     * Push registrations.
     */
    public static final String PUSH_REGISTRATIONS_TABLE = "PUSH_REGISTRATIONS";

    /**
     * Alarm registrations.
     */
    public static final String ALARM_REGISTRATIONS_TABLE =
        "ALARM_REGISTRATIONS";

    /**
     * Runtime related settings.
     */
    public static final String RUNTIME_SETTINGS_TABLE = "RUNTIME_SETTINGS";

    /**
     * OTA status codes.
     */
    public static final String OTA_STATUS_TABLE = "OTA_STATUS";

    /**
     * Preinstall related data.
     */
    public static final String PREINSTALL_TABLE = "PREINSTALL";
    
    /**
     * SoftNote related data.
     */
    public static final String JAVA_SOFTNOTE_TABLE = "JAVA_SOFTNOTE";

    /**
     * Java platform storage name. All Java platform related attributes and
     * settings are stored to this database.
     */
    public static final String JAVA_DATABASE_NAME = "JavaStorage";

    /**
     * Java platform storage name for OTA status codes. OTA status codes
     * are stored to this database.
     */
    public static final String JAVA_OTA_DATABASE_NAME = "JavaOtaStorage";

    /**
     * ID attribute name. ID is related to application UID and it is used
     * to identify application information within table structures.
     */
    public static final String ID = "ID";

    /**
     * Name.
     */
    public static final String NAME = "NAME";

    /**
     * Application suite name.
     */
    public static final String PACKAGE_NAME = "PACKAGE_NAME";

    /**
     * Vendor.
     */
    public static final String VENDOR = "VENDOR";

    /**
     * Version.
     */
    public static final String VERSION = "VERSION";

    /**
     * Application suite root path.
     */
    public static final String ROOT_PATH = "ROOT_PATH";

    /**
     * Media ID where application suite is installed.
     */
    public static final String MEDIA_ID = "MEDIA_ID";

    /**
     * Application suite initial size. This does not contain data
     * that is generated after installation.
     */
    public static final String INITIAL_SIZE = "INITIAL_SIZE";

    /**
     * Application suite descriptor path.
     */
    public static final String JAD_PATH = "JAD_PATH";

    /**
     * Application suite archive path.
     */
    public static final String JAR_PATH = "JAR_PATH";

    /**
     * URL pointing to JAD file source location.
     */
    public static final String JAD_URL = "JAD_URL";

    /**
     * URL pointing to JAR file source location.
     */
    public static final String JAR_URL = "JAR_URL";

    /**
     * Application suite specific access point.
     */
    public static final String ACCESS_POINT = "ACCESS_POINT";

    /**
     * Application suite content info.
     */
    public static final String CONTENT_INFO = "CONTENT_INFO";

    /**
     * Application suite content id.
     */
    public static final String CONTENT_ID = "CONTENT_ID";

    /**
     * ID indicating which application package this application belongs.
     */
    public static final String PACKAGE_ID = "PACKAGE_ID";

    /**
     * Application main class.
     */
    public static final String MAIN_CLASS = "MAIN_CLASS";

    /**
     * A flag indicating should application be started at boot time.
     */
    public static final String AUTORUN = "AUTORUN";

    /**
     * Manifest attribute value.
     */
    public static final String VALUE = "VALUE";

    /**
     * A flag indicating is this manifest attribute trusted.
     */
    public static final String TRUSTED = "TRUSTED";

    /**
     * Application type.
     */
    public static final String TYPE = "TYPE";

    /**
     * Application security domain.
     */
    public static final String SECURITY_DOMAIN = "SECURITY_DOMAIN";

    /**
     * Predefined domain category application belongs to.
     */
    public static final String SECURITY_DOMAIN_CATEGORY
    = "SECURITY_DOMAIN_CATEGORY";

    /**
     * Application hash value.
     */
    public static final String HASH = "HASH";

    /**
     * Hash of the root certificate used to authenticate the application.
     */
    public static final String CERT_HASH = "CERT_HASH";

    /**
     * Security warnings setting.
     */
    public static final String SECURITY_WARNINGS = "SECURITY_WARNINGS";

    /**
     * URL pointing to applications RMS data file.
     */
    public static final String RMS = "RMS";

    /**
     * List of successfully validated certificates. This is comma-separated
     * list of numbers of certificates validated successfully during
     * installation.
     */
    public static final String VALID_CERTS = "VALID_CERTS";

    /**
     * Application on-screen keypad setting.
     */
    public static final String ON_SCREEN_KEYPAD = "ON_SCREEN_KEYPAD";

    /**
     * MIDP3 permission class name.
     */
    public static final String CLASS = "CLASS";

    /**
     * Requested action for this resource.
     */
    public static final String ACTION = "ACTION";

    /**
     * The function group name where this action belongs.
     */
    public static final String FUNCTION_GROUP = "FUNCTION_GROUP";

    /**
     * Allowed trust settings for this function group.
     */
    public static final String ALLOWED_SETTINGS = "ALLOWED_SETTINGS";

    /**
     * Current security setting.
     */
    public static final String CURRENT_SETTING = "CURRENT_SETTING";

    /**
     * A flag indicating whether the UI prompt for blanket mode has been shown.
     */
    public static final String BLANKET_PROMPT = "BLANKET_PROMPT";

    /**
     * URL.
     */
    public static final String URL = "URL";

    /**
     * Push filter.
     */
    public static final String  FILTER = "FILTER";

    /**
     * Type of push registration.
     */
    public static final String REGISTRATION_TYPE = "REGISTRATION_TYPE";

    /**
     * Alarm date when application will be auto invocated.
     */
    public static final String ALARM_TIME = "ALARM_TIME";

    /**
     * List of paths of installed extensions.
     */
    public static final String EXTENSIONS = "EXTENSIONS";

    /**
     * OTA status code.
     */
    public static final String OTA_CODE = "OTA_CODE";

    /**
     * Last time action performed.
     */
    public static final String LATEST_RETRY_TIME = "LATEST_RETRY_TIME";

    /**
     * Count of retries.
     */
    public static final String RETRY_COUNT = "RETRY_COUNT";

    /**
     * Creation time.
     */
    public static final String CREATION_TIME = "CREATION_TIME";

    /**
     * Application suite intall state.
     */
    public static final String INSTALL_STATE = "INSTALL_STATE";
    
    /**
     * MIDlet ID of the softnote.
     */
    public static final String SOFTNOTE_MIDLET_ID = "SOFTNOTE_MIDLET_ID";

    /** 
     * Softnote ID.
     */
    public static final String SOFTNOTE_ID = "SOFTNOTE_ID";

    /**
     * Softnote Primary Text.
     */
    public static final String SOFTNOTE_PRIMARY_TEXT = "SOFTNOTE_PRIMARY_TEXT";

    /**
     * Softnote Secondary Text.
     */
    public static final String SOFTNOTE_SECONDARY_TEXT = "SOFTNOTE_SECONDARY_TEXT";

    /**
     * Softnote Image Path.
     */
    public static final String SOFTNOTE_IMAGE_PATH = "SOFTNOTE_IMAGE_PATH";
}
