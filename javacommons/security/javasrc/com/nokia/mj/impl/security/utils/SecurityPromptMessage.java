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

package com.nokia.mj.impl.security.utils;

import com.nokia.mj.impl.utils.Id;
import com.nokia.mj.impl.utils.LocalizedTextProvider;
import com.nokia.mj.impl.utils.ResourceLoader;

public class SecurityPromptMessage extends LocalizedTextProvider
{
    /** Singleton instance of SecurityPromptMessage. */
    private static SecurityPromptMessage self = null;
    /** ResourceLoader for fetching localised texts. */
    private static ResourceLoader iRes = null;

    // identifiers for the security prompt questions
    public static final Id QUESTION_ID_NETWORK_USAGE = new Id("network_usage", "info_network_usage");
    public static final Id QUESTION_ID_SMS_OPENING_CONN = new Id("sms_opening_conn", "info_sms_opening");
    public static final Id QUESTION_ID_MMS_OPENING_CONN = new Id("mms_opening_conn", "info_mms_opening");
    public static final Id QUESTION_ID_CBS_OPENING_CONN = new Id("cbs_opening_conn", "info_cbs_opening");
    public static final Id QUESTION_ID_SMS_RECEIVING = new Id("sms_receiving", "info_sms_receiving");
    public static final Id QUESTION_ID_MMS_RECEIVING = new Id("mms_receiving", "info_mms_receiving");
    public static final Id QUESTION_ID_CBS_RECEIVING = new Id("cbs_receiving", "info_cbs_receiving");
    public static final Id QUESTION_ID_SMS_SENDING = new Id("sms_sending", "info_sms_sending");
    public static final Id QUESTION_ID_SMS_SINGLE_SENDING = new Id("sms_single_sending", "info_sms_single_sending");
    public static final Id QUESTION_ID_MMS_SINGLE_SENDING = new Id("mms_single_sending", "info_mms_single_sending");
    public static final Id QUESTION_ID_MMS_SINGLE_SENDING_MULTIPLE_DESTINATIONS = new Id("mms_single_sending_multiple_destinations", "info_mms_single_send_multiple_dest");
    public static final Id QUESTION_ID_SEND_RECEIVE_MESSAGES = new Id("receive_send_messages", "info_receive_send_messages");
    public static final Id QUESTION_ID_LOCAL_CONNECTIVITY = new Id("local_connectivity", "info_local_connectivity");
    public static final Id QUESTION_ID_AUDIO_VIDEO_RECORDING = new Id("audio_video_recording", "info_audio_video_recording");
    public static final Id QUESTION_ID_READING_USER_DATA = new Id("reading_user_data", "info_reading_user_data");
    public static final Id QUESTION_ID_WRITING_USER_DATA = new Id("writing_user_data", "info_writing_user_data");
    public static final Id QUESTION_ID_READING_FILE = new Id("reading_file", "info_reading_file");
    public static final Id QUESTION_ID_MANAGE_USER_DATA = new Id("manage_user_data", "info_manage_user_data");
    public static final Id QUESTION_ID_MODIFYING_FILE = new Id("modifying_file", "info_modifying_file");
    public static final Id QUESTION_ID_READING_LOCATION_DATA = new Id("reading_location_data", "info_reading_location_data");
    public static final Id QUESTION_ID_READING_LANDMARK_DATA = new Id("reading_landmark_data", "info_reading_landmark_data");
    public static final Id QUESTION_ID_READING_SMARTCARD = new Id("reading_smartcard", "info_reading_smartcard");
    public static final Id QUESTION_ID_AUTH_SERVICES_USAGE = new Id("auth_services_usage", "info_auth_services_usage");
    public static final Id QUESTION_ID_BLUETOOTH_USAGE = new Id("bluetooth_usage", "info_bluetooth_usage");
    public static final Id QUESTION_ID_READING_CONTACTS = new Id("reading_contacts", "info_reading_contacts");
    public static final Id QUESTION_ID_MODIFYING_CONTACTS = new Id("modifying_contacts", "info_modifying_contacts");
    public static final Id QUESTION_ID_READING_EVENTS = new Id("reading_events", "info_reading_events");
    public static final Id QUESTION_ID_MODIFYING_EVENTS = new Id("modifying_events", "info_modifying_events");
    public static final Id QUESTION_ID_READING_TODOS = new Id("reading_todos", "info_reading_todos");
    public static final Id QUESTION_ID_MODIFYING_TODOS = new Id("modifying_todos", "info_modifying_todos");
    public static final Id QUESTION_ID_DELETING_ITEM = new Id("deleting_item", "info_deleting_item");
    public static final Id QUESTION_ID_UPDATING_ITEM = new Id("updating_item", "info_updating_item");
    public static final Id QUESTION_ID_DELETING_CATEGORY = new Id("deleting_category", "info_deleting_category");
    public static final Id QUESTION_ID_UPDATING_CATEGORY = new Id("updating_category", "info_updating_category");
    public static final Id QUESTION_ID_DELETING_UNNAMED_ITEM = new Id("deleting_unnamed_item", "info_deleting_unnamed_item");
    public static final Id QUESTION_ID_UPDATING_UNNAMED_ITEM = new Id("updating_unnamed_item", "info_updating_unnamed_item");
    public static final Id QUESTION_ID_ADDING_CALENDAR = new Id("adding_calendar", "info_adding_calendar");
    public static final Id QUESTION_ID_DELETING_CALENDAR = new Id("deleting_calendar", "info_deleting_calendar");
    public static final Id QUESTION_ID_LISTING_CALENDARS = new Id("listing_calendars", "listing_calendars");
    public static final Id QUESTION_ID_ORIENTATION_DATA_USAGE = new Id("orientation_data_usage", "info_orientation_data_usage");
    public static final Id QUESTION_ID_POSITIONING_DATA_USAGE = new Id("positioning_data_usage", "info_positioning_data_usage");
    public static final Id QUESTION_ID_LANDMARK_DATA_USAGE = new Id("landmark_data_usage", "info_landmarks_data_usage");
    public static final Id QUESTION_ID_PROXIMITY_LISTENER_REGISTRATION = new Id("proximity_listener_registration", "info_proximity_listener_reg");
    public static final Id QUESTION_ID_MOBITV_DATA_USAGE = new Id("tv_broadcast_user_data_access", "info_tv_broadcast_user_data_access");
    public static final Id QUESTION_ID_NETWORK_USAGE_VIA_PLAT_REQ = new Id("network_usage_via_plat_req", "info_network_usage_via_plat_req");
    public static final Id QUESTION_ID_APP_LAUNCH_VIA_PLAT_REQ = new Id("app_launch_via_plat_req", "info_app_launch_via_plat_req");
    public static final Id QUESTION_ID_PUSH_STATIC_REGISTRATION = new Id("push_registration_static", "info_push_registration_dynamic"); // should be "info_push_registration_static"
    public static final Id QUESTION_ID_PUSH_DYNAMIC_REGISTRATION = new Id("push_registration_dynamic", "info_push_registration_dynamic");
    public static final Id QUESTION_ID_AUTO_INVOCATION = new Id("auto_invocation", "info_auto_invocation");
    public static final Id QUESTION_ID_NDEF_TAG_WRITE = new Id("nfc_ndef_tag_write", "info_nfc_ndef_tag_write");
    public static final Id QUESTION_ID_APP_AUTO_LAUNCH = new Id("url_start", "info_url_start");

    // identifiers for the security prompt answer options
    public static final Id ANSWER_ID_ALLOW = new Id("prompt_allow", "button_prompt_allow");
    public static final Id ANSWER_ID_DENY = new Id("prompt_deny", "button_prompt_deny");
    public static final Id ANSWER_ID_YES = new Id("prompt_yes", "N/A"); // not used in 10.1TB
    public static final Id ANSWER_ID_NO = new Id("prompt_no", "N/A"); // not used in 10.1TB

    // general purpose identifiers
    public static final Id SECURITY_WARNING_ID = new Id("general_warning", "N/A"); // not used in 10.1TB
    public static final Id OCSP_WARNING_ID = new Id("ocsp_warning", "info_ocsp_warning");

    // Localised resources
    private static final String AVKON_LOC_FILE = "javausermessages";
    private static final String AVKON_PREFIX = "qtn_java_secur_";
    private static final String QT_LOC_FILE = "javaapplicationsecuritymessages";
    private static final String QT_PREFIX = "txt_java_secur_";

    /**
     * Creates an instance of the SecurityPromptMessage
     *
     * @return An instance of SecurityPromptMessage
     */
    public static SecurityPromptMessage getInstance()
    {
        if (self == null)
        {
            self = new SecurityPromptMessage();
        }
        return self;
    }

    /**
     * Constructor
     */
    private SecurityPromptMessage()
    {
    }

    /**
     * Returns the localized texts associated with different security prompts
     *
     * @param aTextId     id for the localized text (one of the pre-defined
     *                    constants)
     * @param aTextParams parameters for the localized text
     * @return            the localized text associated with the provided
     *                    identifier and parameters
     */
    public String getText(Id aTextId, Object[] aTextParams)
    {
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance(AVKON_LOC_FILE, AVKON_PREFIX, QT_LOC_FILE, QT_PREFIX);
        }

        return iRes.format(aTextId, aTextParams);
    }
}
