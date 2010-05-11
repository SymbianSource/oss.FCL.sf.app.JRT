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

package com.nokia.mj.impl.installer.ui.eswt2;

import com.nokia.mj.impl.rt.ui.ConfirmData;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;
import com.nokia.mj.impl.utils.Logger;

/**
 * Implementation for JavaInstaller specific RuntimeUI.
 */
public class InstallerRuntimeUi extends RuntimeUi
{
    private static InstallerUiEswt iInstallerUi = null;

    /**
     * Initializes static members of this class.
     */
    static void init(InstallerUiEswt aInstallerUi)
    {
        iInstallerUi = aInstallerUi;
        iLogId = Logger.EJavaInstaller;
    }

    /**
     * Constructor
     */
    public InstallerRuntimeUi()
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
        boolean result = super.confirm(aAppName, aConfirmData);
        if (iInstallerUi != null)
        {
            result = iInstallerUi.confirm(aAppName, aConfirmData);
        }
        return result;
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
        super.error(aAppName, aException);
        if (iInstallerUi != null &&
            aException instanceof InstallerExceptionBase)
        {
            iInstallerUi.error((InstallerExceptionBase)aException);
        }
        else
        {
            logError("Unexpected exception from " + aAppName, aException);
        }
    }
}
