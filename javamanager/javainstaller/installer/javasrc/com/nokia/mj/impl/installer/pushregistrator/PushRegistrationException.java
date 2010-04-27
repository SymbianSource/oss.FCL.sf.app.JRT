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


package com.nokia.mj.impl.installer.pushregistrator;

import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

public class PushRegistrationException extends InstallerExceptionBase
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
    public PushRegistrationException(int aShortMsgId, String[] aShortMsgParams,
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
    public PushRegistrationException(int aShortMsgId, String[] aShortMsgParams,
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
