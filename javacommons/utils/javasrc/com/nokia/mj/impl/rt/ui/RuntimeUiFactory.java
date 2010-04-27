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

package com.nokia.mj.impl.rt.ui;

import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * Factory for RuntimeUI interfaces.
 *
 * @author Nokia Corporation
 * @version $Rev: 0 $
 */
public class RuntimeUiFactory
{

    /**
     * Get instance of Runtime UI.
     *
     * @return Runtime UI instance
     */
    public static RuntimeUi getRuntimeUi()
    {
        boolean identified = false;
        String protectionDomain =
            ApplicationInfo.getInstance().getProtectionDomain();
        if (protectionDomain != null &&
                !protectionDomain.equals
                (ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN))
        {
            identified = true;
        }
        return getRuntimeUi(identified);
    }

    /**
     * Get instance of Runtime UI.
     *
     * @param aIdentified true if the UI instance corresponds to an identified
     *                    application, false otherwise
     * @return            Runtime UI instance
     */
    public static RuntimeUi getRuntimeUi(boolean aIdentified)
    {
        String className = "com.nokia.mj.impl.rt.ui.RuntimeUi";
        String uiProperty = System.getProperty("com.nokia.mj.impl.rt.ui");
        if (uiProperty != null)
        {
            className = uiProperty;
        }

        RuntimeUi runtimeUi = null;
        try
        {
            Class clazz = Class.forName(className);
            runtimeUi = (RuntimeUi)clazz.newInstance();
            runtimeUi.setIdentified(aIdentified);
        }
        catch (Exception ex)
        {
            RuntimeUi.logError("Instantiating " + className + " failed", ex);
        }
        return runtimeUi;
    }
}

