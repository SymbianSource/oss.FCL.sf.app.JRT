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
* Description:  javastoragenames
*
*/


#ifndef JAVASTORAGENAMES_H
#define JAVASTORAGENAMES_H

namespace java
{
namespace storage
{

/**
 * Application package related attributes.
 */
const char APPLICATION_PACKAGE_TABLE[] = "APPLICATION_PACKAGE";

/**
 * Application related attributes.
 */
const char APPLICATION_TABLE[] = "APPLICATION";

/**
 * Application package attributes. All manifest attributes are stored here.
 */
const char APPLICATION_PACKAGE_ATTRIBUTES_TABLE[] =
    "APPLICATION_PACKAGE_ATTRIBUTES";

/**
 * MIDP package related settings.
 */
const char MIDP_PACKAGE_TABLE[] = "MIDP_PACKAGE";

/**
 * Application permissions.
 */
const char MIDP_PERMISSIONS_TABLE[] = "MIDP_PERMISSIONS";

/**
 * Security settings.
 */
const char MIDP_FUNC_GRP_SETTINGS_TABLE[] = "MIDP_FUNCTION_GRP_SETTINGS";

/**
 * Push registrations.
 */
const char PUSH_REGISTRATIONS_TABLE[] = "PUSH_REGISTRATIONS";

/**
 * Alarm registrations.
 */
const char ALARM_REGISTRATIONS_TABLE[] = "ALARM_REGISTRATIONS";

/**
 * Runtime related settings.
 */
const char RUNTIME_SETTINGS_TABLE[] = "RUNTIME_SETTINGS";

/**
 * OTA status codes.
 */
const char OTA_STATUS_TABLE[] = "OTA_STATUS";

/**
 * Preinstall related data.
 */
const char PREINSTALL_TABLE[] = "PREINSTALL";

/**
 * Java platform storage name. All Java platform related attributes and
 * settings are stored to this database.
 */
const char JAVA_DATABASE_NAME[] = "JavaStorage";

/**
 * Java platform storage name for OTA status codes. OTA status codes
 * are stored to this database.
 */
const char JAVA_OTA_DATABASE_NAME[] = "JavaOtaStorage";

/**
 * ID attribute name. ID is related to application UID and it is used
 * to identify application information within table structures.
 */
const wchar_t ID[] = L"ID";

/**
 * Name.
 */
const wchar_t NAME[] = L"NAME";

/**
 * Application suite name.
 */
const wchar_t PACKAGE_NAME[] = L"PACKAGE_NAME";

/**
 * Vendor.
 */
const wchar_t VENDOR[] = L"VENDOR";

/**
 * Version.
 */
const wchar_t VERSION[] = L"VERSION";

/**
 * Application suite root path.
 */
const wchar_t ROOT_PATH[] = L"ROOT_PATH";

/**
 * Media ID where application suite is installed.
 */
const wchar_t MEDIA_ID[] = L"MEDIA_ID";

/**
 * Application suite initial size. This does not contain data
 * that is generated after installation.
 */
const wchar_t INITIAL_SIZE[] = L"INITIAL_SIZE";

/**
 * Application suite descriptor path.
 */
const wchar_t JAD_PATH[] = L"JAD_PATH";

/**
 * Application suite archive path.
 */
const wchar_t JAR_PATH[] = L"JAR_PATH";

/**
 * URL pointing to JAD file source location.
 */
const wchar_t JAD_URL[] = L"JAD_URL";

/**
 * URL pointing to JAR file source location.
 */
const wchar_t JAR_URL[] = L"JAR_URL";

/**
 * Application suite specific access point.
 */
const wchar_t ACCESS_POINT[] = L"ACCESS_POINT";

/**
 * Application suite content info.
 */
const wchar_t CONTENT_INFO[] = L"CONTENT_INFO";

/**
 * Application suite content id.
 */
const wchar_t CONTENT_ID[] = L"CONTENT_ID";

/**
 * ID indicating which application package this application belongs.
 */
const wchar_t PACKAGE_ID[] = L"PACKAGE_ID";

/**
 * Application main class.
 */
const wchar_t MAIN_CLASS[] = L"MAIN_CLASS";

/**
 * A flag indicating should application be started at boot time.
 */
const wchar_t AUTORUN[] = L"AUTORUN";

/**
 * Manifest attribute value.
 */
const wchar_t VALUE[] = L"VALUE";

/**
 * A flag indicating is this manifest attribute trusted.
 */
const wchar_t TRUSTED[] = L"TRUSTED";

/**
 * Application type.
 */
const wchar_t TYPE[] = L"TYPE";

/**
 * Name of the security domain.
 */
const wchar_t SECURITY_DOMAIN[] = L"SECURITY_DOMAIN";

/**
 * Predefined domain category application belongs to.
 */
const wchar_t SECURITY_DOMAIN_CATEGORY[] = L"SECURITY_DOMAIN_CATEGORY";

/**
 * Application hash value.
 */
const wchar_t HASH[] = L"HASH";

/**
 * Hash of the root certificate used to authenticate the application.
 */
const wchar_t CERT_HASH[] = L"CERT_HASH";

/**
 * Security warnings setting.
 */
const wchar_t SECURITY_WARNINGS[] = L"SECURITY_WARNINGS";

/**
 * URL pointing to applications RMS data file.
 */
const wchar_t RMS[] = L"RMS";

/**
 * List of successfully validated certificates. This is comma-separated
 * list of numbers of certificates validated successfully during
 * installation.
 */
const wchar_t VALID_CERTS[] = L"VALID_CERTS";

/**
 * Application on-screen keypad setting.
 */
const wchar_t ON_SCREEN_KEYPAD[] = L"ON_SCREEN_KEYPAD";

/**
 * MIDP3 permission class name.
 */
const wchar_t CLASS[] = L"CLASS";

/**
 * Requested action for this resource.
 */
const wchar_t ACTION[] = L"ACTION";

/**
 * The function group name where this action belongs.
 */
const wchar_t FUNCTION_GROUP[] = L"FUNCTION_GROUP";

/**
 * Allowed trust settings for this function group.
 */
const wchar_t ALLOWED_SETTINGS[] = L"ALLOWED_SETTINGS";

/**
 * Current security setting.
 */
const wchar_t CURRENT_SETTING[] = L"CURRENT_SETTING";

/**
 * A flag indicating whether the UI prompt for blanket mode has been shown.
 */
const wchar_t BLANKET_PROMPT[] = L"BLANKET_PROMPT";

/**
 * URL.
 */
const wchar_t URL[] = L"URL";

/**
 * Push filter.
 */
const wchar_t FILTER[] = L"FILTER";

/**
 * Type of push registration.
 */
const wchar_t REGISTRATION_TYPE[] = L"REGISTRATION_TYPE";

/**
 * Alarm date when application will be auto invocated.
 */
const wchar_t ALARM_TIME[] = L"ALARM_TIME";

/**
 * List of paths of installed extensions.
 */
const wchar_t EXTENSIONS[] = L"EXTENSIONS";

/**
 * OTA status code.
 */
const wchar_t OTA_CODE[] = L"OTA_CODE";

/**
 * Creation time.
 */
const wchar_t CREATION_TIME[] = L"CREATION_TIME";

/**
 * Last time action performed.
 */
const wchar_t LATEST_RETRY_TIME[] = L"LATEST_RETRY_TIME";

/**
 * Count of retries.
 */
const wchar_t RETRY_COUNT[] = L"RETRY_COUNT";

/**
 * Application suite intall state.
 */
const wchar_t INSTALL_STATE[] = L"INSTALL_STATE";

}    // java
}    // storage

#endif // JAVASTORAGENAMES_H
