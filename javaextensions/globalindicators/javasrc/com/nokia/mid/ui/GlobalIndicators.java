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
* Description:  Public interface of the GlobalIndicator API.
 *
*/

package com.nokia.mid.ui;

import com.nokia.mj.impl.globalindicators.GlobalIndicatorsImpl;

/**
 * The <code>GlobalIndicators</code> is a class to handle the device global
 * indicators. Global indicators are small icons that appear to the device
 * status indicator area indicating for example the arrival of a new email or
 * chat message. These indicators remain in the status indicator area even if
 * the midlet is closed but rebooting the device will remove them, so the
 * indicators are not fully persistent.<P>
 *
 * These global indicators are common and also used by the native applications.
 * This means that at any time it is possible for any application to turn an
 * indicator on or off. For that reason it is good practice to query the state
 * of the indicator before setting the value.<P>
 *
 * There are also other indicators besides these <code>MAIL_INDICATOR</code> or
 * <code>CHAT_INDICATOR</code> defined in this API. GlobalIndicators API allows
 * to access those indicators too but the exact values of both the indicator and
 * its value need to be given as accurate integer parameters (as some of those
 * more complex indicators support a larger range of input parameters than just
 * <code>INDICATOR_HIDE</code> or <code>INDICATOR_SHOW</code>).<P>
 *
 * Example of API usage:<P>
 * <code>
 * int retVal = GlobalIndicators.getGlobalIndicator(<BR>
 * &nbsp;&nbsp;&nbsp;&nbsp;GlobalIndicators.MAIL_INDICATOR);<P>
 * if(retVal != GlobalIndicators.INDICATOR_SHOW)<BR>
 * &nbsp;&nbsp;&nbsp;&nbsp;{<BR>
 * &nbsp;&nbsp;&nbsp;&nbsp;retVal = GlobalIndicators.setGlobalIndicator(<BR>
 * &nbsp;&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp;&nbsp;GlobalIndicators.MAIL_INDICATOR,<BR>
 * &nbsp;&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp;&nbsp;GlobalIndicators.INDICATOR_SHOW);<BR>
 * &nbsp;&nbsp;&nbsp;&nbsp;}
 * </code>
 */
public final class GlobalIndicators
{

    private static GlobalIndicatorsImpl sInstance;

    static
    {
        sInstance = GlobalIndicatorsImpl.getInstance();
    }

    /**
     * A constant for email indicator.
     */
    public static final int MAIL_INDICATOR = 274;

    /**
     * A constant for chat message indicator.
     */
    public static final int CHAT_INDICATOR = 260;

    /**
     * A constant for indicator's default value uninitialized.
     */
    public static final int INDICATOR_UNINITIALIZED = 0;

    /**
     * A constant for indicator's value when the indicator is hidden.
     */
    public static final int INDICATOR_HIDE = 1;

    /**
     * A constant for indicator's value when the indicator is displayed.
     */
    public static final int INDICATOR_SHOW = 2;

    /**
     * Private constructor.
     */
    private GlobalIndicators()
    {
    }

    /**
     * Sets the value of the given global indicator.
     *
     * @param indicator <code>MAIL_INDICATOR</code>,
     * <code>CHAT_INDICATOR</code> or a specific other indicator given as
     * an integer.
     *
     * @param value the value to set: <code>INDICATOR_SHOW</code>,
     * <code>INDICATOR_HIDE</code> or a specific other value given as
     * an integer.
     *
     * @return the value that is currently set for the indicator:
     * <code>INDICATOR_UNINITIALIZED</code>, <code>INDICATOR_SHOW</code> or
     * <code>INDICATOR_HIDE</code>. For the other specific indicators the
     * return value is the integer value that is set for that indicator.
     *
     * @throws GlobalIndicatorsException if the given indicator or the
     * given value is not supported, or there occurs an error while trying to
     * access the indicator.
     */
    public static int setGlobalIndicator(int indicator, int value)
    throws GlobalIndicatorsException
    {
        return sInstance.setGlobalIndicator(indicator, value);
    }

    /**
     * Gets the value of the given global indicator.
     *
     * @param indicator <code>MAIL_INDICATOR</code>,
     * <code>CHAT_INDICATOR</code> or a specific other indicator given as
     * an integer.
     *
     * @return the value that is currently set for the indicator:
     * <code>INDICATOR_UNINITIALIZED</code>, <code>INDICATOR_SHOW</code> or
     * <code>INDICATOR_HIDE</code>. For the other specific indicators the
     * return value is the integer value that is currently set for that
     * indicator.
     *
     * @throws GlobalIndicatorsException if there occurs an error while trying
     * to access the indicator.
     */
    public static int getGlobalIndicator(int indicator)
    throws GlobalIndicatorsException
    {
        return sInstance.getGlobalIndicator(indicator);
    }
}
