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


package com.nokia.mj.impl.security.utils;

import com.nokia.mj.impl.storage.StorageException;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationException;
import com.nokia.mj.impl.security.midp.authentication.Credentials;
import com.nokia.mj.impl.security.midp.authentication.OcspData;
import com.nokia.mj.impl.security.midp.storage.AuthenticationStorageData;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import java.util.Vector;

public class Logger
{
    /** Security log id */
    private static final int SECURITY_LOG_ID
    = com.nokia.mj.impl.utils.Logger.EJavaSecurity;

    /** Enaled/disabled flag */
    private static boolean logEnabled = false;

    static
    {
        logEnabled = com.nokia.mj.impl.utils.Logger.Activated[SECURITY_LOG_ID];
    }

    /**
     * Writes info log message.
     *
     * @param aMsg message to be logged
     */
    public static void log(String aMsg)
    {
        com.nokia.mj.impl.utils.Logger.ILOG(SECURITY_LOG_ID, aMsg);
    }

    /**
     * Writes error log message.
     *
     * @param aMsg message to be logged
     */
    public static void logError(String aMsg)
    {
        // Error messages are logged always
        com.nokia.mj.impl.utils.Logger.ELOG(SECURITY_LOG_ID, aMsg);
    }

    /**
     * Writes error log message.
     *
     * @param aMsg message to be logged
     */
    public static void logWarning(String aMsg)
    {
        // Warning messages are logged always
        com.nokia.mj.impl.utils.Logger.WLOG(SECURITY_LOG_ID, aMsg);
    }

    /**
     * Writes info log message.
     *
     * @param aExc exception to be logged
     */
    public static void log(Exception aExc)
    {
        //if (!logEnabled)
        //{
        //    return;
        //}
        if (aExc instanceof AuthenticationException)
        {
            logAuthenticationException((AuthenticationException)aExc);
        }
        else
        {
            // log it as a base exception
            logWarning(aExc.getMessage());
        }
    }

    /**
     * Writes info log message about the ocsp response.
     *
     * @param aOcspSummaryResponse     the ocsp summary response to be logged
     * @param aOcspIndividualResponses the ocsp individual responses to be
     *                                 logged
     */
    public static void logOcspResponse(int aOcspSummaryResponse, int[] aOcspIndividualResponses)
    {
        //if (!logEnabled)
        //{
        //    return;
        //}
        StringBuffer logMsg = new StringBuffer();
        logMsg.append("Ocsp response = ");
        // add the summary
        logMsg.append(" summary:");
        switch (aOcspSummaryResponse)
        {
        case OcspData.RESPONSE_GOOD:
            logMsg.append("GOOD");
            break;
        case OcspData.DEFAULT_RESPONSE:
        case OcspData.RESPONSE_UNKNOWN:
            logMsg.append("UNKNOWN");
            break;
        case OcspData.RESPONSE_REVOKED:
            logMsg.append("REVOKED");
            break;
        }
        // add the individual responses
        logMsg.append(" individual responses:");
        if (aOcspIndividualResponses != null && aOcspIndividualResponses.length > 0)
        {
            for (int i=0; i<aOcspIndividualResponses.length; i++)
            {
                switch (aOcspIndividualResponses[i])
                {
                case OcspData.RESPONSE_GOOD:
                    logMsg.append("GOOD");
                    break;
                case OcspData.RESPONSE_UNKNOWN:
                    logMsg.append("UNKNOWN");
                    break;
                case OcspData.RESPONSE_REVOKED:
                    logMsg.append("REVOKED");
                    break;
                case OcspData.RESPONSE_CANNOT_OBTAIN_CERT_STATUS:
                    logMsg.append("CANNOT_OBTAIN_CERT_STATUS");
                    break;
                case OcspData.RESPONSE_INVALID_REVOCATION_SERVER_URI:
                    logMsg.append("INVALID_REVOCATION_SERVER_URI");
                    break;

                case OcspData.RESPONSE_SIGNATURE_VALIDATION_FAILURE:
                    logMsg.append("SIGNATURE_VALIDATION_FAILURE");
                    break;
                case OcspData.RESPONSE_INVALID_REVOCATION_SERVER_RESPONSE:
                    logMsg.append("INVALID_REVOCATION_SERVER_RESPONSE");
                    break;
                case OcspData.RESPONSE_MISSING_NONCE:
                    logMsg.append("RESPONSE_MISSING_NONCE");
                    break;
                case OcspData.RESPONSE_INVALID_CERT_STATUS_INFO:
                    logMsg.append("INVALID_CERT_STATUS_INFO");
                    break;
                }
                if (i<aOcspIndividualResponses.length - 1)
                {
                    logMsg.append(",");
                }
            }
        }
        else
        {
            logMsg.append("NULL");
        }
        log(logMsg.toString());
    }

    /**
     * Writes info log message about all the authentication credentials.
     *
     * @param aAuthCredentials the authentication credentials to be logged
     */
    public static void logAuthenticationCredentials(Vector aAuthCredentials)
    {
        //if (!logEnabled)
        //{
        //    return;
        //}
        StringBuffer logMsg = new StringBuffer();
        logMsg.append("Authentication credentials:");
        if (aAuthCredentials != null)
        {
            for (int i=0; i<aAuthCredentials.size(); i++)
            {
                Credentials authCredentials =
                    (Credentials)aAuthCredentials.elementAt(i);
                logMsg.append(authCredentials);
            }
        }
        else
        {
            logMsg.append("NULL");
        }
        log(logMsg.toString());
    }

    /**
     * Writes info log message about the selected authentication credentials.
     *
     * @param aAuthData the authentication credentials to be logged
     */
    public static void logAuthenticationData(AuthenticationStorageData aAuthData)
    {
        if (!logEnabled)
        {
            return;
        }
        StringBuffer logMsg = new StringBuffer();
        logMsg.append("Authentication data:");
        if (aAuthData != null)
        {
            logMsg.append(aAuthData.toString());
        }
        else
        {
            logMsg.append("NULL");
        }
        log(logMsg.toString());
    }

    /**
     * Writes info log message about the granted permissions.
     *
     * @param aGrantedPermissions the granted permissions to be logged
     */
    public static void logGrantedPermissions(Vector aGrantedPermissions)
    {
        //if (!logEnabled)
        //{
        //    return;
        //}
        StringBuffer logMsg = new StringBuffer();
        logMsg.append("Granted permissions:");
        if (aGrantedPermissions != null)
        {
            for (int i=0; i<aGrantedPermissions.size(); i++)
            {
                PolicyBasedPermissionImpl permission =
                    (PolicyBasedPermissionImpl)aGrantedPermissions.elementAt(i);
                logMsg.append(permission.toString());
            }
        }
        else
        {
            logMsg.append("NULL");
        }
        log(logMsg.toString());
    }

    /**
     * Writes info log message about the blanket permissions
     *
     * @param aBlanketPermissions the blanket permissions to be logged
     */
    public static void logBlanketPermissions(String[] aBlanketPermissions)
    {
        //if (!logEnabled)
        //{
        //    return;
        //}
        StringBuffer logMsg = new StringBuffer();
        logMsg.append("Blanket permissions = ");
        if (aBlanketPermissions != null)
        {
            for (int i=0; i<aBlanketPermissions.length; i++)
            {
                logMsg.append(aBlanketPermissions[i]);
                if (i < aBlanketPermissions.length - 1)
                {
                    logMsg.append(",");
                }
            }
        }
        else
        {
            logMsg.append("NULL");
        }
        log(logMsg.toString());
    }

    private static void logAuthenticationException(AuthenticationException aExc)
    {
        String logMsg = "Authentication failed because ";
        switch (aExc.getErrorCode())
        {
        case AuthenticationException.CONNECTION_TO_CAPTAIN_FAILED:
            logMsg += "the connection to JavaCaptain could not be established";
            break;
        case AuthenticationException.SENDING_MSG_TO_CAPTAIN_FAILED:
            logMsg += "sending a comms message to JavaCertStore failed";
            break;
        case AuthenticationException.CERT_VERIFICATION_FAILED:
            logMsg += "the signing certificate chain validation failed";
            break;
        case AuthenticationException.CERT_NOT_YET_VALID:
            logMsg += "the signing certificate is not yet valid";
            break;
        case AuthenticationException.CERT_EXPIRED:
            logMsg += "the signing certificate is expired";
            break;
        case AuthenticationException.ROOT_CERT_IN_CHAIN:
            logMsg += "the signing certificate chain includes the root certificate";
            break;
        case AuthenticationException.MISSING_ROOT:
            logMsg += "the signing certificate chain does not point to a trusted root";
            break;
        case AuthenticationException.SIG_VERIFICATION_FAILED:
            logMsg += "the signature verification failed";
            break;
        case AuthenticationException.MISSING_DOMAIN_MAPPING:
            logMsg += "the root certificate is not mapped to any protection domain";
            break;
        case AuthenticationException.UNKNOWN_EXT_KEY_USAGE:
            logMsg += "the signing certificate contains an unknown value for the extended key usage extension";
            break;
        default:
            logMsg += "of an unknown reason";
            break;
        }
        logWarning(logMsg);
    }

    private static void log(String aMsg, int logLevel)
    {
        com.nokia.mj.impl.utils.Logger.LOG(SECURITY_LOG_ID, logLevel, aMsg);
    }
}
