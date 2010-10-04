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
* Description:  Implementation class of GlobalIndicators API.
 *
*/

package com.nokia.mj.impl.nokiauiapi;

import com.nokia.mid.ui.GlobalIndicators;
import com.nokia.mid.ui.GlobalIndicatorsException;
import com.nokia.mj.impl.utils.OsErrorMessage;
import com.nokia.mj.impl.rt.support.ApplicationInfo;


public class GlobalIndicatorsImpl
{

    // Static instance, can be got with getInstace method
    private static GlobalIndicatorsImpl sGlobalIndicator;
    // Static block of the class.

    static
    {
//        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javaglobalindicators");

        // This is called when class is loaded for the first time
        sGlobalIndicator = new GlobalIndicatorsImpl();
    }

    /**
     * This private constructor can be called only from static block.
     */
    private GlobalIndicatorsImpl()
    {
    }

    /**
     * Return GlobalIndicatorsImpl instance
     */
    public static GlobalIndicatorsImpl getInstance()
    {
        return sGlobalIndicator;
    }

    /**
     * See class GlobalIndicators for comments
     */
    public int setGlobalIndicator(int aIndicator, int aValue)
    throws GlobalIndicatorsException
    {
        int ret;
        String protectionDomain = ApplicationInfo.getInstance().getProtectionDomain();
        if (protectionDomain == null)
        {
            throw new GlobalIndicatorsException(
                "Failed to set global indicator for unidentified domain");
        }
        else
        {
            // For mail and chat indicators only show and hide are supported
            if (((aIndicator == GlobalIndicators.MAIL_INDICATOR) ||
                    (aIndicator == GlobalIndicators.CHAT_INDICATOR)) &&
                    ((aValue != GlobalIndicators.INDICATOR_SHOW) &&
                     (aValue != GlobalIndicators.INDICATOR_HIDE)))
            {
                throw new GlobalIndicatorsException(
                    "Value is not supported for global indicator");
            }

            //int ret = _setGlobalIndicator( aIndicator, aValue );
            ret = 0;//_setGlobalIndicator(aIndicator, aValue);
            if (ret < OsErrorMessage.SUCCESS)
            {
                throw new GlobalIndicatorsException(
                    "Failed to set global indicator", ret);
            }
        }
        return ret;
    }

    /**
     * See class GlobalIndicators for comments
     */
    public int getGlobalIndicator(int aIndicator)
    throws GlobalIndicatorsException
    {
        int ret = 0;//_getGlobalIndicator(aIndicator);
        if (ret < OsErrorMessage.SUCCESS)
        {
            throw new GlobalIndicatorsException(
                "Failed to get global indicator value", ret);
        }
        return ret;
    }
   
}
