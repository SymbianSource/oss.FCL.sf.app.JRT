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
* Description:  Exception for Global Indicators
 *
*/

package com.nokia.mid.ui;

public class GlobalIndicatorsException extends Exception
{


    private int errorCode;

    /**
     * Default constructor is not allowed.
     */
    protected GlobalIndicatorsException() {}

    /**
     * Constructs an exception instance with a textual information.
     * @param info human readable information about the exception.
     */
    public GlobalIndicatorsException(String info)
    {
        super(info);
    }

    /**
     * Constructs an exception instance with a textual information and the
     * error code of the native call.
     * @param info human readable information about the exception.
     * @param errorCode the error code received from native side.
     */
    public GlobalIndicatorsException(String info, int errorCode)
    {
        super(info);
        this.errorCode = errorCode;
    }

    /**
     * Constructs human readable textual information
     * @return String the human readable textual information
     */
    public String toString()
    {
        if (errorCode == 0)
        {
            return super.toString();
        }
        else
        {
            return super.toString() + " Native error: " + errorCode;
        }
    }

    /**
     * Returns the native error code
     * @return the native error code
     */
    public int getErrorCode()
    {
        return errorCode;
    }
}
