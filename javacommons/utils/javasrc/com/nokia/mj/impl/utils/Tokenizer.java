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

import java.util.Vector;

public final class Tokenizer
{

    /**
     * Splits a string into tokens based on a separator
     *
     * @param str The string from where the tokens are collected
     * @param separator The separator for the tokens
     * @return A list of tokens
     */
    public static String[] split(String str, String separator)
    {
        if (str == null)
        {
            return null;
        }
        if (separator == null || separator.length() == 0)
        {
            return new String[] {str};
        }
        Vector tokens = new Vector();
        int index = str.indexOf(separator);
        while (index != -1)
        {
            tokens.addElement(str.substring(0,index));
            str = str.substring(index + separator.length());
            index = str.indexOf(separator);
        }
        tokens.addElement(str); // add token after last separator
        String[] strTokens = new String[tokens.size()];
        tokens.copyInto(strTokens);
        return strTokens;
    }
}
