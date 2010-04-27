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

package com.nokia.mj.impl.security.midp.authentication;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.security.utils.SecurityErrorMessage;
import com.nokia.mj.impl.security.utils.SecurityDetailedErrorMessage;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import com.nokia.mj.impl.security.common.InstallerSecurityException;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.ConfirmData;
import com.nokia.mj.impl.security.utils.Logger;

/**
 * Ocsp client performing revocation checks for certificate chains according
 * to the Online Certificate Status Protocol specified by RFC2560
 *
 */
public final class OcspChecker
        implements Runnable
{
    static
    {
        Jvm.loadSystemLibrary("javasecurity");
    }

    // the array of data for which the OCSP is to be performed
    private OcspData[] ocspData = null;

    // the iap/snap to be used when doing the OCSP checks
    long iap = 0;
    long snap = 0;

    /**
     * The ocsp settings
     */
    private static OcspSettings iOcspSettings;
    private static OcspUserPreferences iOcspUserPreferences;

    // flag indicating if the OCSP checks are ongoing
    private boolean running = false;

    // the corresponding Thread object
    private Thread thread = null;

    // handle to the native thread: all the native calls
    // will be executed within the same native thread
    private int iNativeThreadHandle;

    // the native handler
    private int iNativePeerHandle;

    // the listener for the ocsp events
    private OcspEventListener iEventListener;

    // flag indicating if the ocsp check was cancelled
    private boolean iOcspCancelled = false;

    /**
     * Constructor
     *
     * @return An instance of OcspClient
     */
    public OcspChecker(OcspData[] ocspData, OcspSettings ocspSettings, OcspUserPreferences ocspUserPreferences, OcspEventListener eventListener)
    {
        this.ocspData = ocspData;
        try
        {
            this.iap = Long.parseLong(ocspSettings.iap);
        }
        catch (NumberFormatException e)
        {
            Logger.logWarning("Invalid ocsp settings: iap is not a valid. Default value (0) will be used instead.");
        }
        try
        {
            this.snap = Long.parseLong(ocspSettings.snap);
        }
        catch (NumberFormatException e)
        {
            Logger.logWarning("Invalid ocsp settings: snap is not a valid. Default value (0) will be used instead.");
        }
        iNativeThreadHandle = _createNativeThread();
        iNativePeerHandle = _createNativePeer(iNativeThreadHandle,
                                              this.iap, this.snap, ocspUserPreferences.getOcspUrl());
        this.thread = new Thread(this, "OcspChecker");
        Jvm.setThreadAsDaemon(this.thread, true);
        iEventListener = eventListener;
        iOcspSettings = ocspSettings;
        iOcspUserPreferences = ocspUserPreferences;
        iOcspCancelled = false;
    }

    /**
     * Starts the OCSP checks for the provided cert chains.
     */
    public void start()
    {
        // inform the ocsp listener (if any) about the start
        // of the ocsk check
        if (iEventListener != null)
        {
            iEventListener.ocspEvent(OcspEventListener.OCSP_START_EVENT,
                                     null /* eventError */);
        }

        // start
        running = true;
        thread.start();
        Logger.log("Ocsp checks started");
    }

    public void run()
    {
        // do the actual OCSP checks
        _ocspChecks(iNativeThreadHandle, iNativePeerHandle, ocspData);

        // finish
        finish(false /*ignoreResponse */);
    }

    public void cancel()
    {
        Logger.log("Ocsp checks cancelled");
        // mark down that the ocsp was cancelled
        iOcspCancelled = true;

        // cancel the native peer
        _cancelOcspChecks(iNativeThreadHandle, iNativePeerHandle);

        // finish
        finish(true /*ignoreResponse */);
    }

    public void destroy()
    {
        _destroyNativePeer(iNativeThreadHandle, iNativePeerHandle);
        _destroyNativeThread(iNativeThreadHandle);
        iOcspCancelled = false;
    }

    private synchronized void finish(boolean ignoreResponse)
    {
        // if not running, then exit quickly
        if (!running)
        {
            return;
        }

        // mark that the checks are done
        running = false;

        // handle ocsp response (if applicable)
        Throwable response = null;
        if (!ignoreResponse)
        {
            response = handleResponse();
        }

        // inform the ocsp listener (if any) about the finish
        // of the ocsp check
        if (iEventListener != null)
        {
            iEventListener.ocspEvent(OcspEventListener.OCSP_FINISH_EVENT,
                                     response);
        }
        Logger.log("Ocsp checks finished");
    }

    private Throwable handleResponse()
    {
        Throwable response = null;
        // the selected ocsp summary response
        int selectedOcspResponse = OcspData.DEFAULT_RESPONSE;
        // the selected ocsp individual responses
        int[] selectedOcspResponses = null;
        // check if the current ocsp response is a better/"more positive"
        // than the currently selected one
        for (int i=0; ocspData!= null && i<ocspData.length; i++)
        {
            int currentOcspResponse = ocspData[i].getSummary();
            if (betterOcspResponse(currentOcspResponse,
                                   selectedOcspResponse))
            {
                // discard the previous selection and keep the current one
                selectedOcspResponse = currentOcspResponse;
                selectedOcspResponses = ocspData[i].getIndividualResponses();
            }
        }
        // log the response
        Logger.logOcspResponse(selectedOcspResponse, selectedOcspResponses);
        // handle the response
        switch (selectedOcspResponse)
        {
        case OcspData.RESPONSE_GOOD:
            // it's all good
            break;
        case OcspData.DEFAULT_RESPONSE:
            // show a warning to the user -> if the user says No, throw exception
            if (!handleOcspWarning(selectedOcspResponses))
            {
                response = new InstallerSecurityException(
                    new SecurityErrorMessage(),
                    SecurityErrorMessage.OCSP_GENERAL_ERR,
                    null, /* no params for short msg */
                    new SecurityDetailedErrorMessage(),
                    getOcspDetailedError(selectedOcspResponses),
                    null /* no params for detailed msg */,
                    OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
            }
            break;
        case OcspData.RESPONSE_UNKNOWN:
            // if all the individual responses correspond to ocsp warning level
            //   -> show a warning to the user
            //   -> if the user says No, throw an exception
            // if at least one individual response corresponds to ocsp error level
            //   -> throw an exception
            if (!isOcspWarning(selectedOcspResponses)
                    || !handleOcspWarning(selectedOcspResponses))
            {
                response = new InstallerSecurityException(
                    new SecurityErrorMessage(),
                    SecurityErrorMessage.OCSP_GENERAL_ERR,
                    null, /* no params for short msg */
                    new SecurityDetailedErrorMessage(),
                    getOcspDetailedError(selectedOcspResponses),
                    null /* no params for detailed msg */,
                    OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
            }
            break;
        case OcspData.RESPONSE_REVOKED:
            response = new InstallerSecurityException(
                new SecurityErrorMessage(),
                SecurityErrorMessage.OCSP_GENERAL_ERR,
                null, /* no params for short msg */
                new SecurityDetailedErrorMessage(),
                SecurityDetailedErrorMessage.OCSP_REVOKED_ERR,
                null /* no params for detailed msg */,
                OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
        }
        return response;
    }

    /**
     * Checks if ocspResponse1 is better than ocspResponse2.
     *
     * The hierarchy of ocsp responses is this:
     * 1) RESPONSE_GOOD
     * 2) RESPONSE_UNKNOWN
     * 3) DEFAULT_RESPONSE
     * 4) RESPONSE_REVOKED
     */
    private boolean betterOcspResponse(int ocspResponse1, int ocspResponse2)
    {
        switch (ocspResponse2)
        {
        case OcspData.RESPONSE_GOOD:
            // there is nothing better than GOOD
            return false;
        case OcspData.RESPONSE_UNKNOWN:
            // only GOOD is bettter than UNKNOWN
            return (ocspResponse1 == OcspData.RESPONSE_GOOD);
        case OcspData.RESPONSE_REVOKED:
            // either GOOD or UNKNOWN or DEFAULT are better
            return (ocspResponse1 == OcspData.RESPONSE_GOOD
                    || ocspResponse1 == OcspData.RESPONSE_UNKNOWN
                    || ocspResponse1 == OcspData.DEFAULT_RESPONSE);
        case OcspData.DEFAULT_RESPONSE:
            // anything is better than the default
            return true;
        default:
            // return value for purely unknown responses
            return false;
        }
    }

    /**
     * Handles ocsp warning according to the iOcspUserPreferences.
     * IF the iOcspUserPreferences is ON it handles ocsp warning in one
     * of the following ways:
     *   - ignores it
     *   - displays a message to user. In this case, based on the user input,
     *     it signals the caller that the warning was handled succesfully or not
     * ELSE it signals the caller that the warning was not handled succesfully
     */
    private boolean handleOcspWarning(int[] ocspResponses)
    {
        if (iOcspSettings.silent
                || iOcspCancelled
                || ignoreOcspWarning(ocspResponses))
        {
            // signal that the warning was handled succesfully
            return true;
        }
        if (iOcspSettings.ocspWarning == OcspSettings.OCSP_WARNING_IGNORE
                || iOcspUserPreferences.getOcspMode()
                == OcspUserPreferences.OCSP_MODE_ON)
        {
            RuntimeUi ui = RuntimeUiFactory.getRuntimeUi(
                               true /* aIdentified */);
            SecurityPromptMessage pm = SecurityPromptMessage.getInstance();
            String[] answerOptions =
            {
                pm.getText(SecurityPromptMessage.ANSWER_ID_YES, null),
                pm.getText(SecurityPromptMessage.ANSWER_ID_NO, null)
            };
            ConfirmData cData = new ConfirmData(
                pm.getText(SecurityPromptMessage.OCSP_WARNING_ID,null),
                answerOptions,
                0 /* highlight always the first option */);
            ui.confirm(pm.getText(
                           SecurityPromptMessage.SECURITY_WARNING_ID,null),
                       cData);
            ui.destroy();
            return (cData.getAnswer() == 0);
        }
        // if we got here it means that warnings are not accepted
        // -> signal that the warning was not handled succesfully
        return false;
    }

    /**
     * Checks if all the provided ocsp responses are ocsp warnings.
     *
     * It returns false if at least one of the ocsp resonses is:
     * Ocspdata.RESPONSE_SIGNATURE_VALIDATION_FAILURE or OcspData.UNKNOWN or OcspData.REVOKED
     * or it returns true otherwise
     */
    private boolean isOcspWarning(int[] ocspResponses)
    {
        for (int i=0; i<ocspResponses.length; i++)
        {
            if (ocspResponses[i] == OcspData.RESPONSE_SIGNATURE_VALIDATION_FAILURE
                    || ocspResponses[i] == OcspData.RESPONSE_UNKNOWN
                    || ocspResponses[i] == OcspData.RESPONSE_REVOKED)
            {
                return false;
            }
        }
        return true;
    }

    /*
     * Some ocsp warnings are silently ignored.
     * This method checks if any of the ocsp responses corresponds
     * to a warning which should be ignored, case in which it returns true.
     * Otherwise it returns false
     */
    private boolean ignoreOcspWarning(int[] ocspResponses)
    {
        if (ocspResponses != null)
        {
            for (int i=0; i<ocspResponses.length; i++)
            {
                if (ocspResponses[i] == OcspData.RESPONSE_MISSING_NONCE)
                {
                    return true;
                }
            }
        }
        return false;
    }

    private int getOcspDetailedError(int[] ocspResponses)
    {
        if (ocspResponses != null)
        {
            for (int i=0; i<ocspResponses.length; i++)
            {
                switch (ocspResponses[i])
                {
                case OcspData.RESPONSE_INVALID_REVOCATION_SERVER_URI:
                case OcspData.RESPONSE_INVALID_REVOCATION_SERVER_RESPONSE:
                    return SecurityDetailedErrorMessage.OCSP_SETTINGS_ERR;
                case OcspData.RESPONSE_REVOKED:
                    return SecurityDetailedErrorMessage.OCSP_REVOKED_ERR;
                }
            }
        }
        return SecurityDetailedErrorMessage.OCSP_GENERAL_ERR;
    }

    private native int _createNativeThread();
    private native void _destroyNativeThread(int nativeThreadHandle);
    private native int _createNativePeer(int nativeThreadHandle, long iap, long snap, String defaultUrl);
    private native void _destroyNativePeer(int nativeThreadHandle, int nativePeerHandle);
    private native void _ocspChecks(int nativeThreadHandle, int nativePeerHandle, OcspData[] ocspData);
    private native void _cancelOcspChecks(int nativeThreadHandle, int nativePeerHandle);
}
