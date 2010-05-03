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

package com.nokia.mj.impl.utils.exception;

import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;

/**
 * Exception used to signal errors when handling descriptor/manifest attributes
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class InvalidAttributeException extends InstallerExceptionBase
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor
     *
     * @param aShortMsgId id for short error message
     * @param aShortMsgParams parameters for short error message
     * @param aDetailedMsgId id for detailed error message
     * @param aDetailedMsgParams parameters for detailed error message
     * @param aOtaStatusCode OTA status code
     */
    public InvalidAttributeException(int aShortMsgId, String[] aShortMsgParams,
                                     int aDetailedMsgId, String[] aDetailedMsgParams,
                                     int aOtaStatusCode)
    {
        super(new InstallerErrorMessage(), aShortMsgId, aShortMsgParams,
              new InstallerDetailedErrorMessage(), aDetailedMsgId, aDetailedMsgParams,
              aOtaStatusCode);
    }

    /**
     * Constructor
     *
     * @param aShortMsgId id for short error message
     * @param aShortMsgParams parameters for short error message
     * @param aDetailedMsgId id for detailed error message
     * @param aDetailedMsgParams parameters for detailed error message
     * @param aOtaStatusCode OTA status code
     * @param aRootException the exception which caused this exception
     */
    public InvalidAttributeException(int aShortMsgId, String[] aShortMsgParams,
                                     int aDetailedMsgId, String[] aDetailedMsgParams,
                                     int aOtaStatusCode, Throwable aRootException)
    {
        super(new InstallerErrorMessage(), aShortMsgId, aShortMsgParams,
              new InstallerDetailedErrorMessage(), aDetailedMsgId, aDetailedMsgParams,
              aOtaStatusCode, aRootException);
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
