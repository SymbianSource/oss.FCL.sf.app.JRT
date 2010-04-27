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
* Description:  Provides API to fetch smart card property
 *
*/


package com.nokia.mj.impl.properties.satsa;

import java.io.IOException;
import com.nokia.mj.impl.satsa.SecurityError;

import com.nokia.mj.impl.satsa.APDUError;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * Refer JSR-177 Specification for details
 */
public class SmartCardSlots
{
    /**
     * Native handle. Set in constructor.
     */
    private final int iHandle;

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasatsa");
        }
        catch (Exception e)
        {

        }

    }

    /**
     * Refer JSR-177 Specification for details
     */
    public SmartCardSlots() throws IOException
    {
        // Create native object
        iHandle = _create();

        // Check is handle created or is some error code returned
        if (iHandle <= SecurityError.SUCCESS)
        {
            // Failed to create native object.
            if (iHandle == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }

            // IOException in all cases
            try
            {
                APDUError.check(iHandle, false, // do not check illegal argument
                                // error
                                false, // do not check security error
                                false, // do not check connection not found error
                                false, // do not check interrupted IO error
                                true);// throw IO exception as a default
            }
            catch (IOException ie)
            {
                throw new IOException("Creating slot connection failed: " + ie.getMessage());
            }
        }
        // else Native object is OK

    }

    /**
    * Refer JSR-177 Specification for details
    */
    public String getSlots()
    {

        String slots = null;
        // error array contains only one value, which indicates the error code
        int[] errArray = new int[1];

        slots = _getSlots(iHandle, errArray);

        // first element of error array is returned error code
        int err = errArray[0];

        // check error codes
        if (err < SecurityError.SUCCESS)
        {

            if (err == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }
            // we will return null if some other error code arrives
            slots = null;
        }
        return slots;
    }

    // NATIVE METHODS

    /**
     * Creates native instance or returns error if construction fails.
     *
     * @return Native handle or error code.
     */
    private native int _create();

    /**
    *
    */
    static private native String _getSlots(int aHandle, int[] aErrArray);

}
