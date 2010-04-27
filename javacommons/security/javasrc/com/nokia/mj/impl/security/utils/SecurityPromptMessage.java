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

import com.nokia.mj.impl.utils.LocalizedTextProvider;
import com.nokia.mj.impl.utils.ResourceLoader;

public class SecurityPromptMessage extends LocalizedTextProvider
{
    /** Singleton instance of SecurityPromptMessage. */
    private static SecurityPromptMessage self = null;
    /** ResourceLoader for fetching localised texts. */
    private static ResourceLoader iRes = null;

    // identifiers for the security prompt questions
    public static final String QUESTION_ID_NETWORK_USAGE = "network_usage";
    public static final String QUESTION_ID_SMS_OPENING_CONN = "sms_opening_conn";
    public static final String QUESTION_ID_MMS_OPENING_CONN = "mms_opening_conn";
    public static final String QUESTION_ID_CBS_OPENING_CONN = "cbs_opening_conn";
    public static final String QUESTION_ID_SMS_RECEIVING = "sms_receiving";
    public static final String QUESTION_ID_MMS_RECEIVING = "mms_receiving";
    public static final String QUESTION_ID_CBS_RECEIVING = "cbs_receiving";
    public static final String QUESTION_ID_SMS_SENDING = "sms_sending";
    public static final String QUESTION_ID_SMS_SINGLE_SENDING = "sms_single_sending";
    public static final String QUESTION_ID_MMS_SENDING = "mms_sending";
    public static final String QUESTION_ID_MMS_SINGLE_SENDING = "mms_single_sending";
    public static final String QUESTION_ID_MMS_SINGLE_SENDING_MULTIPLE_DESTINATIONS = "mms_single_sending_multiple_destinations";
    public static final String QUESTION_ID_SEND_RECEIVE_MESSAGES = "receive_send_messages";
    public static final String QUESTION_ID_LOCAL_CONNECTIVITY = "local_connectivity";
    public static final String QUESTION_ID_AUDIO_VIDEO_RECORDING = "audio_video_recording";
    public static final String QUESTION_ID_READING_USER_DATA = "reading_user_data";
    public static final String QUESTION_ID_WRITING_USER_DATA = "writing_user_data";
    public static final String QUESTION_ID_READING_FILE = "reading_file";
    public static final String QUESTION_ID_MANAGE_USER_DATA = "manage_user_data";
    public static final String QUESTION_ID_MODIFYING_FILE = "modifying_file";
    public static final String QUESTION_ID_READING_LOCATION_DATA = "reading_location_data";
    public static final String QUESTION_ID_READING_LANDMARK_DATA = "reading_landmark_data";
    public static final String QUESTION_ID_READING_SMARTCARD = "reading_smartcard";
    public static final String QUESTION_ID_AUTH_SERVICES_USAGE = "auth_services_usage";
    public static final String QUESTION_ID_BLUETOOTH_USAGE = "bluetooth_usage";
    public static final String QUESTION_ID_READING_CONTACTS = "reading_contacts";
    public static final String QUESTION_ID_MODIFYING_CONTACTS = "modifying_contacts";
    public static final String QUESTION_ID_READING_EVENTS = "reading_events";
    public static final String QUESTION_ID_MODIFYING_EVENTS = "modifying_events";
    public static final String QUESTION_ID_READING_TODOS = "reading_todos";
    public static final String QUESTION_ID_MODIFYING_TODOS = "modifying_todos";
    public static final String QUESTION_ID_DELETING_ITEM = "deleting_item";
    public static final String QUESTION_ID_UPDATING_ITEM = "updating_item";
    public static final String QUESTION_ID_DELETING_CATEGORY = "deleting_category";
    public static final String QUESTION_ID_UPDATING_CATEGORY = "updating_category";
    public static final String QUESTION_ID_DELETING_UNNAMED_ITEM = "deleting_unnamed_item";
    public static final String QUESTION_ID_UPDATING_UNNAMED_ITEM = "updating_unnamed_item";
    public static final String QUESTION_ID_ADDING_CALENDAR = "adding_calendar";
    public static final String QUESTION_ID_DELETING_CALENDAR = "deleting_calendar";
    public static final String QUESTION_ID_LISTING_CALENDARS = "listing_calendars";
    public static final String QUESTION_ID_ORIENTATION_DATA_USAGE = "orientation_data_usage";
    public static final String QUESTION_ID_POSITIONING_DATA_USAGE = "positioning_data_usage";
    public static final String QUESTION_ID_LANDMARK_DATA_USAGE = "landmark_data_usage";
    public static final String QUESTION_ID_PROXIMITY_LISTENER_REGISTRATION = "proximity_listener_registration";
    public static final String QUESTION_ID_MOBITV_DATA_USAGE = "tv_broadcast_user_data_access";
    public static final String QUESTION_ID_NETWORK_USAGE_VIA_PLAT_REQ = "network_usage_via_plat_req";
    public static final String QUESTION_ID_APP_LAUNCH_VIA_PLAT_REQ = "app_launch_via_plat_req";
    public static final String QUESTION_ID_PUSH_STATIC_REGISTRATION = "push_registration_static";
    public static final String QUESTION_ID_PUSH_DYNAMIC_REGISTRATION = "push_registration_dynamic";
    public static final String QUESTION_ID_AUTO_INVOCATION = "auto_invocation";
    public static final String QUESTION_ID_NDEF_TAG_WRITE = "nfc_ndef_tag_write";

    // identifiers for the security prompt answer options
    public static final String ANSWER_ID_ALLOW = "prompt_allow";
    public static final String ANSWER_ID_DENY = "prompt_deny";
    public static final String ANSWER_ID_YES = "prompt_yes";
    public static final String ANSWER_ID_NO = "prompt_no";

    // general purpose identifiers
    public static final String SECURITY_WARNING_ID = "general_warning";
    public static final String OCSP_WARNING_ID = "ocsp_warning";

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
    public String getText(String aTextId, Object[] aTextParams)
    {
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance("javausermessages", "qtn_java_secur_");
        }
        return iRes.format(aTextId, aTextParams);
    }
}
