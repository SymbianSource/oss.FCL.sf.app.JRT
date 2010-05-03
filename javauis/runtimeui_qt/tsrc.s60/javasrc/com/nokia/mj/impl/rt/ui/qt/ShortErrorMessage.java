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

package com.nokia.mj.impl.rt.ui.qt;

import com.nokia.mj.impl.utils.ErrorMessageBase;
import com.nokia.mj.impl.utils.ResourceLoader;
import java.util.Hashtable;

public final class ShortErrorMessage extends ErrorMessageBase
{
    public static final int MSG_WITH_DETAILS_ID = 1;
    public static final int MSG_WITHOUT_DETAILS_ID = 2;

    /*** ----------------------------- PUBLIC ------------------------------ */
    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Method for getting message table where key is an Integer value for
     * the error code, and value is an error message String id.
     */
    private static Hashtable iMessageTable = null;
    protected Hashtable getMessageTable()
    {
        if (iMessageTable != null)
        {
            return iMessageTable;
        }
        Hashtable messageTable = new Hashtable();
        messageTable.put(new Integer(MSG_WITH_DETAILS_ID), "Short error message. Make sure the details are available as well.");
        messageTable.put(new Integer(MSG_WITHOUT_DETAILS_ID), "Short error message. Make sure there are no details available.");
        iMessageTable = messageTable;
        return iMessageTable;
    }

    /**
     * Method for retrieving the ResourceLoader instance that is used
     * to localise error messages.
     */
    private static ResourceLoader iRes = null;
    protected ResourceLoader getResourceLoader()
    {
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance("", "");
        }
        return iRes;
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
