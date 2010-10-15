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

package com.nokia.mj.impl.rt.ui;

import com.nokia.mj.impl.utils.ResourceLoader;

/**
 * Class defining RuntimeUi texts.
 */
public class RuntimeUiTexts
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    public static final String OK = "secur_prompt_ok,prompt_ok";
    public static final String DETAILS_QUERY = "secur_error_details_message,error_details";

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
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance(
                "javausermessages,javaapplicationsecuritymessages",
                "qtn_java_,txt_java_secur_button_");
        }
        return iRes.format(aTextId, aTextParameters);
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /** ResourceLoader for fetching localised texts. */
    private static ResourceLoader iRes = null;

    /*** ----------------------------- NATIVE ----------------------------- */
}
