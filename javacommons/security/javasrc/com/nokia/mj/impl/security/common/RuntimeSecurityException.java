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


package com.nokia.mj.impl.security.common;

import com.nokia.mj.impl.security.utils.SecurityErrorMessage;
import com.nokia.mj.impl.security.utils.SecurityDetailedErrorMessage;
import com.nokia.mj.impl.utils.exception.ExceptionBase;

public class RuntimeSecurityException extends ExceptionBase
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor
     *
     * @param aShortMsgId id for short error message
     * @param aShortMsgParams parameters for short error message
     * @param aDetailedMsgId id for detailed error message
     * @param aDetailedMsgParams parameters for detailed error message
     */
    public RuntimeSecurityException(int aShortMsgId, String[] aShortMsgParams,
                                    int aDetailedMsgId, String[] aDetailedMsgParams)
    {
        super(new SecurityErrorMessage(),
              aShortMsgId, aShortMsgParams,
              new SecurityDetailedErrorMessage(),
              aDetailedMsgId, aDetailedMsgParams);
    }

    /**
     * Constructor
     *
     * @param aShortMsgId id for short error message
     * @param aShortMsgParams parameters for short error message
     * @param aDetailedMsgId id for detailed error message
     * @param aDetailedMsgParams parameters for detailed error message
     * @param aRootException the exception which caused this exception
     */
    public RuntimeSecurityException(int aShortMsgId, String[] aShortMsgParams,
                                    int aDetailedMsgId, String[] aDetailedMsgParams,
                                    Throwable aRootException)
    {
        super(new SecurityErrorMessage(),
              aShortMsgId, aShortMsgParams,
              new SecurityDetailedErrorMessage(),
              aDetailedMsgId, aDetailedMsgParams,
              aRootException);
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
