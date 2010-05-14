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

package com.nokia.mj.impl.rt.ui.avkon;

import com.nokia.mj.impl.rt.ui.ConfirmData;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * Runtime UI AVKON implementation.
 */
public class RuntimeUiAvkon extends RuntimeUi
{

    static
    {
        Jvm.loadSystemLibrary("javaruntimeui");
    }

    /**
     * Constructor
     */
    public RuntimeUiAvkon()
    {
        super();
        log("constructor completed");
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
        _error(aAppName, aException.getShortMessage(), aException.getDetailedMessage());
        log("error returns");
    }

    private native boolean _confirm(String aAppName, ConfirmData aConfirmData, boolean aIdentifiedApp);
    private native void _error(String aAppName, String aShortMessage, String aDetailedMessage);
}
