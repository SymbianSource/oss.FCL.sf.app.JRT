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


package com.nokia.mj.impl.utils;

/**
 * Class which provides text handling utility functions
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class TextUtil
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Method for inserting parameters into certain text. The placeholders for
     * the parameters are marked as %s. This method fills in as many
     * parameters as possible (depending on the number of available
     * placeholders for parameters and also depending on the number of
     * provided parameters)
     *
     * @param  str the string where the parameters are to be inserted
     * @param  aParams the parameters to be inserted
     * @return a string consisning of the content of the provided string filled
     *         in with the provided parameters
     * parameters
     */
    public static String insertParameters(String str, String[] parameters)
    {
        String newStr = str;
        if (parameters != null)
        {
            for (int i=0; i<parameters.length; i++)
            {
                if (parameters[i] != null)
                {
                    int index = newStr.indexOf("%s");
                    if (index != -1)
                    {
                        String prefix = newStr.substring(0, index);
                        String suffix = newStr.substring(index + 2);
                        newStr = prefix + parameters[i] + suffix;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        return newStr;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
