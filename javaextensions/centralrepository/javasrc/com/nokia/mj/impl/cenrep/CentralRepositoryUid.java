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

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mid.cenrep.CentralRepositoryException;

/**
 * Class for central repository UID.
 */
class CentralRepositoryUid extends Uid
{

    /**
     * Default constructor.
     */
    protected CentralRepositoryUid()
    {
        super();
    }

    /**
     * Returns int value
     *
     * @param value string representation of uid.
     * @throws CentralRepositoryException, if value is null, empty or not valid.
     */
    static int getIntValue(String value)
    throws CentralRepositoryException
    {
        Uid uid = Uid.createUid(value);
        if (uid == null)
        {
            throw new CentralRepositoryException("Uid is null or empty");
        }

        String numStr = uid.getStringValue();

        // Check if value is negative.
        boolean negative = false;
        if (numStr.startsWith("-"))
        {
            negative = true;
            numStr = numStr.substring(1);
        }

        // Check for optional radix prefix.
        int radix = 10;
        if (numStr.startsWith("0x"))
        {
            radix = 16;
            numStr = numStr.substring(2);
        }

        // Check if numStr is in Symbian TUid form [12345678].
        if (numStr.length() <= 10 && numStr.startsWith("[") && numStr.endsWith("]"))
        {
            radix = 16;
            numStr = numStr.substring(1, numStr.length()-1);
        }

        int result = 0;
        long val = Long.parseLong(numStr, radix);
        if (negative)
        {
            result = (int)-val;
        }
        else
        {
            result = (int)val;
        }
        return result;
    }

};
