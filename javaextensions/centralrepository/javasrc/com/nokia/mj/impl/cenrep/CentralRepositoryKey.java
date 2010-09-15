/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.cenrep;

import com.nokia.mid.cenrep.CentralRepositoryException;
/**
 * Class for central repository key representation
 */
class CentralRepositoryKey
{

    /**
     * Default constructor.
     */
    protected CentralRepositoryKey()
    {
    }

    /**
     * Returns long value represantion.
     *
     * @param value string represantion of key.
     * @throws CentralRepositoryException, if key is null, empty, negative
     *        or not valid.
     */
    static long getLongValue(String value)
    throws CentralRepositoryException
    {
        if (value == null || value.length() == 0)
        {
            throw new CentralRepositoryException("Key is null or empty!");
        }

        String numStr = value;

        // Negative value is not allowed
        if (numStr.startsWith("-"))
        {
            throw new CentralRepositoryException("Nagative value is not allowed!");
        }

        // Check for optional radix prefix.
        int radix = 10;
        if (numStr.startsWith("0x"))
        {
            radix = 16;
            numStr = numStr.substring(2);
        }

        return Long.parseLong(numStr, radix);
    }

};
