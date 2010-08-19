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

package com.nokia.mj.impl.security.midp.common;

import java.util.Vector;
import com.nokia.mj.impl.utils.Id;
import com.nokia.mj.impl.utils.ResourceLoader;

/**
 * Encapsulation of a security settings applied to user permissions.
 * The security settings consists of a name, an interaction mode currently
 * in use and a set of allowed interaction modes
 */
public class UserSecuritySettingsImpl
        implements UserSecuritySettings
{
    private String name;
    private int currentInteractionMode;
    private int[] allowedInteractionModes;
    private boolean blanketPrompt;
    private static ResourceLoader resLoader = null;
    private boolean active;

    private static final Id NET_ACCESS_ID = new Id("setting_net_access", "setlabel_net_access");
    private static final Id LOW_LEVEL_NET_ACCESS_ID = new Id("setting_low_level_net_access", "setlabel_low_level_net_access");
    private static final Id MESSAGING_ID = new Id("setting_messaging", "setlabel_messaging");
    private static final Id RESTRICTED_MESSAGING_ID = new Id("setting_restricted_messaging", "setlabel_restricted_messaging");
    private static final Id CALL_CONTROL_ID = new Id("setting_call_control", "setlabel_call_control");
    private static final Id LOCAL_CONNECTIVITY_ID = new Id("setting_local_conn", "setlabel_local_conn");
    private static final Id MULTIMEDIA_RECORDING_ID = new Id("setting_mm_record", "setlabel_mm_record");
    private static final Id READ_USER_DATA_ACCESS_ID = new Id("setting_read_data", "setlabel_read_data");
    private static final Id WRITE_USER_DATA_ACCESS_ID = new Id("setting_write_data", "setlabel_write_data");
    private static final Id APPLICATION_AUTO_INVOCATION_ID = new Id("setting_app_auto_invocat", "setlabel_app_auto_invoc");
    private static final Id PHONE_CALL_ID = new Id("setting_phone_call", "setlabel_call_control");
    private static final Id LOCATION_ID = new Id("setting_location", "setlabel_location");
    private static final Id LANDMARK_ID = new Id("setting_landmarks", "setlabel_landmarks");
    private static final Id SMART_CARD_COMMUNICATION_ID = new Id("setting_smartcard", "setlabel_smartcard");
    private static final Id AUTHENTICATION_ID = new Id("setting_auth", "setlabel_auth");
    private static final Id BROADCAST_ID = new Id("setting_broadcast", "setlabel_broadcast");
    private static final Id NFC_WRITE_ACCESS_ID = new Id("setting_nfc_write_access", "setlabel_nfc_write_access");
    private static final Id URL_START_ID = new Id("setting_url_start", "setlabel_url_start");

    private static final String AVKON_LOC_FILE = "javaapplicationsettings";
    private static final String AVKON_PREFIX = "qtn_java_";
    private static final String QT_LOC_FILE = "javaruntimeapplicationsettings";
    private static final String QT_PREFIX = "txt_java_sett_";

    public UserSecuritySettingsImpl(
        String name,
        int currentInteractionMode,
        int[] allowedInteractionModes)
    {
        this(name, currentInteractionMode, allowedInteractionModes, false /* blanketPrompt */);
    }

    public UserSecuritySettingsImpl(
        String name,
        int currentInteractionMode,
        int[] allowedInteractionModes,
        boolean blanketPrompt,
        boolean active)
    {
        this.name = name;
        setCurrentInteractionMode(currentInteractionMode);
        setAllowedInteractionModes(allowedInteractionModes);
        this.blanketPrompt = blanketPrompt;
        this.active = active;
    }

    public UserSecuritySettingsImpl(
        String name,
        int currentInteractionMode,
        int[] allowedInteractionModes,
        boolean blanketPrompt)
    {
        this(name, currentInteractionMode, allowedInteractionModes, blanketPrompt, true);
    }
    
    public boolean isActive()
    {
        return active;
    }

    public String getName()
    {
        return name;
    }

    /**
     * Method for retrieving the localized name of settings
     *
     * @param aSettingId The identifier of setting whose localized name
     *                   is retrieved
     * @return The corresponding localized name for the setting
     */
    public static String getLocalizedName(String aSettingName)
    {
        if (resLoader == null)
        {
            resLoader = ResourceLoader.getInstance(AVKON_LOC_FILE, AVKON_PREFIX, QT_LOC_FILE, QT_PREFIX);
        }
        Id id = null;
        if (NET_ACCESS_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = NET_ACCESS_ID;
        }
        else if (LOW_LEVEL_NET_ACCESS_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = LOW_LEVEL_NET_ACCESS_ID;
        }
        else if (MESSAGING_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = MESSAGING_ID;
        }
        else if (RESTRICTED_MESSAGING_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = RESTRICTED_MESSAGING_ID;
        }
        else if (CALL_CONTROL_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = CALL_CONTROL_ID;
        }
        else if (LOCAL_CONNECTIVITY_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = LOCAL_CONNECTIVITY_ID;
        }
        else if (MULTIMEDIA_RECORDING_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = MULTIMEDIA_RECORDING_ID;
        }
        else if (READ_USER_DATA_ACCESS_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = READ_USER_DATA_ACCESS_ID;
        }
        else if (WRITE_USER_DATA_ACCESS_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = WRITE_USER_DATA_ACCESS_ID;
        }
        else if (APPLICATION_AUTO_INVOCATION_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = APPLICATION_AUTO_INVOCATION_ID;
        }
        else if (PHONE_CALL_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = PHONE_CALL_ID;
        }
        else if (NFC_WRITE_ACCESS_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = NFC_WRITE_ACCESS_ID;
        }
        else if (LOCATION_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = LOCATION_ID;
        }
        else if (LANDMARK_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = LANDMARK_ID;
        }
        else if (SMART_CARD_COMMUNICATION_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = SMART_CARD_COMMUNICATION_ID;
        }
        else if (AUTHENTICATION_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = AUTHENTICATION_ID;
        }
        else if (BROADCAST_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = BROADCAST_ID;
        }
        else if (URL_START_SETTINGS.equalsIgnoreCase(aSettingName))
        {
            id = URL_START_ID;
        }
        else
        {
            return aSettingName;   
        }
        return resLoader.format(id, null /*aTextParameters*/);
    }

    public static String getLocalizedString(Id aStrId)
    {
        if (resLoader == null)
        {
            resLoader = ResourceLoader.getInstance(AVKON_LOC_FILE, AVKON_PREFIX, QT_LOC_FILE, QT_PREFIX);
        }
        return resLoader.format(aStrId, null /*aTextParameters*/);
    }

    public int getCurrentInteractionMode()
    {
        return currentInteractionMode;
    }

    public int[] getAllowedInteractionModes()
    {
        return allowedInteractionModes;
    }

    public boolean getBlanketPrompt()
    {
        return blanketPrompt;
    }

    public boolean isInteractionModeAllowed(int interactionMode)
    {
        for (int i=0; allowedInteractionModes != null
                && i<allowedInteractionModes.length; i++)
        {
            if (allowedInteractionModes[i] == interactionMode)
            {
                return true;
            }
        }
        return false;
    }


    public static UserSecuritySettings getPredefinedUserSecuritySettings(
        String preDefinedSettingsName)
    {
        int currentInteractionMode = ONESHOT_INTERACTION_MODE;
        int[] allowedInteractionModes = {ONESHOT_INTERACTION_MODE};
        return new UserSecuritySettingsImpl(NET_ACCESS_SETTINGS,
                                            currentInteractionMode, allowedInteractionModes);
    }

    public boolean equals(Object o)
    {
        if (this == o)
        {
            return true;
        }
        if (!(o instanceof UserSecuritySettings))
        {
            return false;
        }
        UserSecuritySettings settings = (UserSecuritySettings)o;
        if (this.getCurrentInteractionMode()
                == settings.getCurrentInteractionMode()
                && arraysEqual(this.getAllowedInteractionModes(), settings
                               .getAllowedInteractionModes())
                && ((this.getName() == null && settings.getName() == null)
                    || (this.getName() != null && this.getName().equals(
                            settings.getName()))))
        {
            return true;
        }
        return false;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\nSetting name:\"" + name + "\"");
        sb.append(" Settings current interaction mode:" + currentInteractionMode);
        sb.append(" Settings allowed interaction modes:");
        if (allowedInteractionModes != null)
        {
            for (int i=0; i<allowedInteractionModes.length; i++)
            {
                sb.append(allowedInteractionModes[i]);
                if (i < allowedInteractionModes.length - 1)
                {
                    sb.append(",");
                }
            }
        }
        else
        {
            sb.append("NULL");
        }
        return sb.toString();
    }

    private boolean arraysEqual(int[] array1, int[] array2)
    {
        if (array1 == null && array2 == null)
        {
            return true;
        }
        if ((array1 == null && array2 != null) ||
                (array1 != null && array2 == null) ||
                (array1.length != array2.length))
        {
            return false;
        }
        for (int i=0; i<array1.length; i++)
        {
            boolean found = false;
            for (int j=0; j<array2.length; j++)
            {
                if (array1[i] == array2[j])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
            return true;
        }
        return true;
    }

    private void setCurrentInteractionMode(int currentInteractionMode)
    {
        // by default the current interaction mode is NO
        this.currentInteractionMode = NO_INTERACTION_MODE;
        if (interactionModeKnown(currentInteractionMode))
        {
            this.currentInteractionMode = currentInteractionMode;
        }
    }

    private void setAllowedInteractionModes(int[] allowedInteractionModes)
    {
        // add NO_INTERACTION_MODE if not provided
        Vector vAllowedSettings = new Vector();
        vAllowedSettings.addElement(new Integer(NO_INTERACTION_MODE));
        // collect only the known interaction modes from the set of provided interaction modes
        if (allowedInteractionModes != null)
        {
            for (int i=0; i<allowedInteractionModes.length; i++)
            {
                if (interactionModeKnown(allowedInteractionModes[i]))
                {
                    Integer allowedInteractionMode = new Integer(
                        allowedInteractionModes[i]);
                    if (!vAllowedSettings.contains(allowedInteractionMode))
                    {
                        vAllowedSettings.addElement(allowedInteractionMode);
                    }
                }
            }
        }
        // add the current interaction mode (if not already provided)
        Integer currentInteractionMode = new Integer(this.currentInteractionMode);
        if (!vAllowedSettings.contains(currentInteractionMode))
        {
            vAllowedSettings.addElement(currentInteractionMode);
        }
        // copy the content of the vector int the int[]
        this.allowedInteractionModes = new int[vAllowedSettings.size()];
        for (int i=0; i<vAllowedSettings.size(); i++)
        {
            this.allowedInteractionModes[i] = ((Integer)vAllowedSettings
                                               .elementAt(i)).intValue();
        }
    }

    private boolean interactionModeKnown(int interactionMode)
    {
        switch (interactionMode)
        {
        case NO_INTERACTION_MODE:
        case ONESHOT_INTERACTION_MODE:
        case SESSION_INTERACTION_MODE:
        case BLANKET_INTERACTION_MODE:
            return true;
        }
        return false;
    }
}
