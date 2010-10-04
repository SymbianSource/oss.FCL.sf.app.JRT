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

package com.nokia.mj.impl.rt.ui.qt;

import com.nokia.mj.impl.rt.ui.ConfirmData;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.rt.support.Jvm;

import com.nokia.mj.impl.utils.Id;
import com.nokia.mj.impl.utils.ResourceLoader;

/**
 * Runtime UI QT implementation.
 *
 */
public class RuntimeUiQt extends RuntimeUi
{
    private static final Id ALLOW_BUTTON = new Id(null, "prompt_allow");
    private static final Id DENY_BUTTON = new Id(null, "prompt_deny");
    private static final Id DETAILS_BUTTON = new Id(null, "error_details");
    private static final Id OK_BUTTON = new Id(null, "prompt_ok");
    private static final String QT_LOC_FILE = "javaapplicationsecuritymessages";
    private static final String QT_PREFIX = "txt_java_secur_button_";

    private static ResourceLoader iRes = null;

    static
    {
        Jvm.loadSystemLibrary("javaruntimeui");
    }

    /**
     * Constructor
     */
    public RuntimeUiQt()
    {
        super();
    }

    /**
     * Seeks confirmation from the user.
     *
     * @param aAppName     the name of the application on behalf of which the
     *                     confirmation is requested
     * @param aConfirmData the data to be confirmed. Unless the user has
     *                     canceled the confirmation, this data will be filled
     *                     in with user's answer upon return
     * @return             true if the user has answered, false if the user has
     *                     canceled the confirmation
     */
    public boolean confirm(String aAppName, ConfirmData aConfirmData)
    {

        if (aConfirmData == null)
        {
            Logger.LOG(Logger.EJavaRuntime,
                       Logger.EError, "RuntimeUi: ConfirmData null");
            return false;
        }

        // If no localized button text provided load default ones.
        if (aConfirmData.getAnswerOptions() == null)
        {
            if (iRes == null)
            {
                iRes = ResourceLoader.getInstance(null, null, QT_LOC_FILE, QT_PREFIX);
            }

            String allowButton = iRes.format(ALLOW_BUTTON, null);
            String denyButton = iRes.format(DENY_BUTTON, null);

            aConfirmData = new ConfirmData(aConfirmData.getQuestion(),
                                           new String[] {allowButton, denyButton},
                                           aConfirmData.getAnswerSuggestion());
        }

        return _confirm(aAppName, aConfirmData, isIdentified());
    }

    /**
     * Notifies the user that an error has occurred.
     * This method must return quickly.
     *
     * @param aAppName   the name of the application which generated the error
     *                   situation
     * @param aException exception indicating the error reason
     */
    public void error(String aAppName, ExceptionBase aException)
    {
        if (aException != null)
        {
            if (iRes == null)
            {
                iRes = ResourceLoader.getInstance(null, null, QT_LOC_FILE, QT_PREFIX);
            }

            String detailsButton = iRes.format(DETAILS_BUTTON, null);
            String okButton = iRes.format(OK_BUTTON, null);

            _error(aAppName,
                   aException.getShortMessage(),
                   aException.getDetailedMessage(),
                   detailsButton,
                   okButton);
        }
    }

    private native boolean _confirm(String aAppName, ConfirmData aConfirmData, boolean aIdentifiedApp);
    private native void _error(String aAppName, String aShortMessage, String aDetailedMessage, String aDetailsButton, String aOkButton);
}
