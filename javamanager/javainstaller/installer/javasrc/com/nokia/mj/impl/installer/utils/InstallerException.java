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


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.installer.Installer;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

public class InstallerException extends InstallerExceptionBase
{

    /** Result code to be sent in InstallerResultMessage. */
    private int iResultCode = Installer.ERR_GENERAL;

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
    public InstallerException(int aShortMsgId, String[] aShortMsgParams,
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
    public InstallerException(int aShortMsgId, String[] aShortMsgParams,
                              int aDetailedMsgId, String[] aDetailedMsgParams,
                              int aOtaStatusCode, Throwable aRootException)
    {
        super(new InstallerErrorMessage(), aShortMsgId, aShortMsgParams,
              new InstallerDetailedErrorMessage(), aDetailedMsgId, aDetailedMsgParams,
              aOtaStatusCode, aRootException);
    }

    /**
     * Constructor
     *
     * @param aResultCode Result code to be sent in InstallerResultMessage
     * @param aShortMsgId id for short error message
     * @param aShortMsgParams parameters for short error message
     * @param aDetailedMsgId id for detailed error message
     * @param aDetailedMsgParams parameters for detailed error message
     * @param aOtaStatusCode OTA status code
     */
    public InstallerException(
        int aResultCode, int aShortMsgId, String[] aShortMsgParams,
        int aDetailedMsgId, String[] aDetailedMsgParams,
        int aOtaStatusCode)
    {
        super(new InstallerErrorMessage(), aShortMsgId, aShortMsgParams,
              new InstallerDetailedErrorMessage(), aDetailedMsgId, aDetailedMsgParams,
              aOtaStatusCode);
        iResultCode = aResultCode;
    }

    /**
     * Constructor
     *
     * @param aResultCode Result code to be sent in InstallerResultMessage
     * @param aShortMsgId id for short error message
     * @param aShortMsgParams parameters for short error message
     * @param aDetailedMsgId id for detailed error message
     * @param aDetailedMsgParams parameters for detailed error message
     * @param aOtaStatusCode OTA status code
     * @param aRootException the exception which caused this exception
     */
    public InstallerException(
        int aResultCode, int aShortMsgId, String[] aShortMsgParams,
        int aDetailedMsgId, String[] aDetailedMsgParams,
        int aOtaStatusCode, Throwable aRootException)
    {
        super(new InstallerErrorMessage(), aShortMsgId, aShortMsgParams,
              new InstallerDetailedErrorMessage(), aDetailedMsgId, aDetailedMsgParams,
              aOtaStatusCode, aRootException);
        iResultCode = aResultCode;
    }

    /**
     * Throws InstallerException with error codes appropriate to internal
     * errors and uses given aMsg as a parameter in the exception.
     *
     * @param aMsg parameter string for the internal error exception
     * @throws InstallerException
     */
    public static void internalError(String aMsg)
    {
        internalError(aMsg, null);
    }

    /**
     * Throws InstallerException with error codes appropriate to internal
     * errors and uses given aMsg as a parameter in the exception.
     *
     * @param aResultCode Result code to be sent in InstallerResultMessage
     * @param aMsg parameter string for the internal error exception
     * @throws InstallerException
     */
    public static void internalError(int aResultCode, String aMsg)
    {
        internalError(aResultCode, aMsg, null);
    }

    /**
     * Throws InstallerException with error codes appropriate to internal
     * errors and uses given aMsg as a parameter in the exception.
     *
     * @param aMsg parameter string for the internal error exception
     * @param aRootException the exception which caused this exception
     * @throws InstallerException
     */
    public static void internalError(String aMsg, Throwable aRootException)
    {
        throw getInternalErrorException(aMsg, aRootException);
    }

    /**
     * Throws InstallerException with error codes appropriate to internal
     * errors and uses given aMsg as a parameter in the exception.
     *
     * @param aResultCode Result code to be sent in InstallerResultMessage
     * @param aMsg parameter string for the internal error exception
     * @param aRootException the exception which caused this exception
     * @throws InstallerException
     */
    public static void internalError(
        int aResultCode, String aMsg, Throwable aRootException)
    {
        throw getInternalErrorException(aResultCode, aMsg, aRootException);
    }

    /**
     * Creates a new InstallerException with error codes appropriate
     * to internal errors and uses given aMsg as a parameter in
     * the exception.
     *
     * @param aMsg parameter string for the internal error exception
     * @param aRootException the exception which caused this exception
     * @returns InstallerException
     */
    public static InstallerException getInternalErrorException(
        String aMsg, Throwable aRootException)
    {
        return getInternalErrorException(
                   Installer.ERR_GENERAL, aMsg, aRootException);
    }

    /**
     * Creates a new InstallerException with error codes appropriate
     * to internal errors and uses given aMsg as a parameter in
     * the exception.
     *
     * @param aResultCode Result code to be sent in InstallerResultMessage
     * @param aMsg parameter string for the internal error exception
     * @param aRootException the exception which caused this exception
     * @returns InstallerException
     */
    public static InstallerException getInternalErrorException(
        int aResultCode, String aMsg, Throwable aRootException)
    {
        int shortMsgId = InstallerErrorMessage.OTHER_UNEXPECTED_ERR;
        switch (Installer.getInstallerState())
        {
        case Installer.STATE_COMPONENT_INFO:
        case Installer.STATE_INSTALLING:
            shortMsgId = InstallerErrorMessage.INST_UNEXPECTED_ERR;
            break;
        case Installer.STATE_UNINSTALLING:
            shortMsgId = InstallerErrorMessage.UNINST_UNEXPECTED_ERR;
            break;
        }
        return new InstallerException(aResultCode, shortMsgId, null,
                                      InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                      new String[] { aMsg },
                                      OtaStatusCode.INTERNAL_ERROR,
                                      aRootException);
    }

    /**
     * Creates a new InstallerException with error codes appropriate
     * to out-of-disk-space error situations.
     *
     * @param aRequiredSpace required free disk space amount
     * @param aRootException the exception which caused this exception
     * @returns InstallerException
     */
    public static InstallerException getOutOfDiskSpaceException(
        long aRequiredSpace, Throwable aRootException)
    {
        int shortMsgId = InstallerErrorMessage.INST_NO_MEM;
        if (Installer.getInstallerState() == Installer.STATE_UNINSTALLING)
        {
            shortMsgId = InstallerErrorMessage.UNINST_UNEXPECTED_ERR;
        }
        int detailsId = InstallerDetailedErrorMessage.NO_MEM;
        int requiredSpace = (int)(aRequiredSpace / 1024); // space in kB
        if (requiredSpace >= 1024)
        {
            detailsId = InstallerDetailedErrorMessage.NO_MEM_MB;
            requiredSpace = requiredSpace / 1024; // space in MB
        }
        return new InstallerException(
                   shortMsgId, null, detailsId,
                   new String[] { Integer.toString(requiredSpace + 1) },
                   OtaStatusCode.INSUFFICIENT_MEMORY, aRootException);
    }

    /**
     * Returns result code to be used in InstallerResultMessage.
     */
    public int getResultCode()
    {
        return iResultCode;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
