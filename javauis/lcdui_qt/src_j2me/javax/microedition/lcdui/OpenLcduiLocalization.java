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

        if(key.equals("key_up"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("key_down"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("key_left"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("key_right"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("key_select"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("key_stop"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("alert_error"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("alert_warning"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("alert_info"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("alert_conf"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("alert_alarm"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("alert_null"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_");
            return iRes.format(key).toString();
        }

        if(key.equals("pb_fetch_number"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_option_");
            return iRes.format(key).toString();
        }

        if(key.equals("pb_fetch_number_short"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_option_");
            return iRes.format(key).toString();
        }

        if(key.equals("pb_fetch_email"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_option_");
            return iRes.format(key).toString();
        }

        if(key.equals("pb_fetch_email_short"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_option_");
            return iRes.format(key).toString();
        }

        if(key.equals("create_call"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_option_");
            return iRes.format(key).toString();
        }

        if(key.equals("create_call_short"))
        {
            iRes = ResourceLoader.getInstance("lcdui", "qtn_midp_option_");
            return iRes.format(key).toString();
        }

        if(key.equals("back"))
        {
            iRes = ResourceLoader.getInstance("lcduiavkon", "text_softkey_");
            return iRes.format(key).toString();
        }

        if(key.equals("close"))
        {
            iRes = ResourceLoader.getInstance("lcduiavkon", "text_softkey_");
            return iRes.format(key).toString();
        }

        if(key.equals("empty_list"))
        {
            iRes = ResourceLoader.getInstance("lcduiavkon", "qtn_selec_");
            return iRes.format(key).toString();
        }

        if(key.equals("help"))
        {
            iRes = ResourceLoader.getInstance("lcduiavkon", "qtn_options_");
            return iRes.format(key).toString();
        }

        return SWT.getMessage(key);
    }

}