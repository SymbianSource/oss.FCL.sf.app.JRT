/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package javax.microedition.lcdui;

import com.nokia.mj.impl.utils.ResourceLoader;
import org.eclipse.swt.SWT;

final class OpenLcduiLocalization
{

    private static ResourceLoader iRes;

    public static String getMessage(String key)
    {

        if(key == null) throw new IllegalArgumentException(
                MsgRepository.TEXT_EXCEPTION_TXT_IS_NULL);

        // arrow up key name
        if(key.equals("canvas_key_up"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // arrow down key name
        if(key.equals("canvas_key_down"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // arrow left key name
        if(key.equals("canvas_key_left"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // arrow right key name
        if(key.equals("canvas_key_right"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // selection key name
        if(key.equals("canvas_key_selection_key"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // media key stop, key name
        if(key.equals("canvas_media_key_stop"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // media key play/pause, key name
        if(key.equals("canvas_media_key_play"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // media key previous (rewind), key name
        if(key.equals("canvas_media_key_previous"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // media key next (forward), key name
        if(key.equals("canvas_media_key_next"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // SK1, positive softkey - key name, used mainly in on-screen-keypad
        if(key.equals("canvas_key_lsk"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // SK2, negative softkey - key name, used mainly in on-screen-keypad
        if(key.equals("canvas_key_rsk"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Send key, "green phone key" key name
        if(key.equals("canvas_key_send"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // End key, "red phone key" key name
        if(key.equals("canvas_key_end"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Clear key, in qwerty keyboard, key name
        if(key.equals("canvas_key_clear"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Applications key, key name
        if(key.equals("canvas_key_apps"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Enter key in qwerty keyboard, key name
        if(key.equals("canvas_key_enter"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Voice/volume key, key name
        if(key.equals("canvas_key_voice"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Modifier keys, key name
        if(key.equals("canvas_key_modifier"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Space key in qwerty keyboard, key name
        if(key.equals("canvas_key_space"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
       
        // Escape key in qwerty keyboard, key name
        if(key.equals("canvas_key_escape"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
       
        // Tab key in qwerty keyboard, key name
        if(key.equals("canvas_key_tab"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Delete key in qwerty keyboard, key name
        if(key.equals("canvas_key_delete"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Hash key in ITU-T keyboard, key name
        if(key.equals("canvas_key_hash_key"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Star key in ITU-T keyboard, key name
        if(key.equals("canvas_key_star_key"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // back space button key name
        if(key.equals("canvas_key_backspace"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for error Alert
        if(key.equals("info_alert_error"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for warning Alert
        if(key.equals("info_alert_warning"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for information Alert
        if(key.equals("info_alert_information"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for confirmation Alert
        if(key.equals("info_alert_confirmation"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for alarm Alert
        if(key.equals("info_alert_alarm"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for alarm Alert (AlertType.NULL)
        if(key.equals("info_alert"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for Options menu - fetch contact from device phone book
        if(key.equals("opt_fetch"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for context menu - fetch contact from device phone book
        if(key.equals("menu_fetch"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // default text for button - fetch contact from device phone book
        if(key.equals("button_fetch"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for Options menu - create a call
        if(key.equals("opt_call"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // default text for context menu - create a call
        if(key.equals("menu_call"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // default text for button - create a call
        if(key.equals("button_call"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // String displayed in empty form or list
        if(key.equals("info_no_data"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Softkey label in Alert that has only one button - for opening "Options" menu
        if(key.equals("button_options_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Softkey label in Alert that has two buttons - for opening "Options" menu
        if(key.equals("button_options_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Softkey label in Alert - for implicit dismiss command, Note: this command is then the only command in Alert
        if(key.equals("button_alert_ok"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // Softkey label in Alert - for implicit dismiss command, Note: in context menu. 
        if(key.equals("menu_alert_dismiss"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // IMPLICIT List default select command - in context menu
        if(key.equals("menu_implicit_list_select"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
                
        // Default command labels

        // HELP - in Options menu
        if(key.equals("opt_cmd_help"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // HELP - in context menu
        if(key.equals("menu_cmd_help"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // HELP - if only one button/softkey
        if(key.equals("button_cmd_help_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // HELP - if two buttons/softkeys 
        if(key.equals("button_cmd_help_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // STOP - in Options menu
        if(key.equals("opt_cmd_stop"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // STOP - in context menu
        if(key.equals("menu_cmd_stop"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // STOP - if only one button/softkey
        if(key.equals("button_cmd_stop_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // STOP - if two buttons/softkeys
        if(key.equals("button_cmd_stop_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // EXIT - in Options menu
        if(key.equals("opt_cmd_close"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // EXIT - in context menu
        if(key.equals("menu_cmd_close"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // EXIT - if only one button/softkey
        if(key.equals("button_cmd_close_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // EXIT - if two buttons/softkeys
        if(key.equals("button_cmd_close_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // SCREEN - in Options menu
        if(key.equals("opt_screen_cmd_select"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // SCREEN - in context menu
        if(key.equals("menu_screen_cmd_select"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // SCREEN - if only one button/softkey
        if(key.equals("button_screen_cmd_select_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // SCREEN - if two buttons/softkeys
        if(key.equals("button_screen_cmd_select_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }

        // BACK - in Options menu
        if(key.equals("opt_cmd_back"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // BACK - in context menu
        if(key.equals("menu_cmd_back"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // BACK - if only one button/softkey
        if(key.equals("button_cmd_back_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // BACK - if two buttons/softkeys
        if(key.equals("button_cmd_back_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // CANCEL - in Options menu
        if(key.equals("opt_cmd_cancel"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // CANCEL - in context menu
        if(key.equals("menu_cmd_cancel"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // CANCEL - if only one button/softkey
        if(key.equals("button_cmd_cancel_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // CANCEL - if two buttons/softkeys
        if(key.equals("button_cmd_cancel_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // OK - in Options menu
        if(key.equals("opt_cmd_ok"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // OK - in context menu
        if(key.equals("menu_cmd_ok"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // OK - if only one button/softkey
        if(key.equals("button_cmd_ok_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // OK - if two buttons/softkeys
        if(key.equals("button_cmd_ok_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // ITEM - in Options menu
        if(key.equals("opt_item_cmd_select"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // ITEM - in context menu
        if(key.equals("menu_item_cmd_select"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // ITEM - if only one button/softkey
        if(key.equals("button_item_cmd_select_1"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        // ITEM - if two buttons/softkeys
        if(key.equals("button_item_cmd_select_2"))
        {
            iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
            return iRes.format(key).toString();
        }
        
        return SWT.getMessage(key);
        
    }

}