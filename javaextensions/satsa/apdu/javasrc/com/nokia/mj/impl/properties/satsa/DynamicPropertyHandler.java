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
* Description:  Provides Dynamic Property Handler
 *
*/


package com.nokia.mj.impl.properties.satsa;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
import com.nokia.mj.impl.properties.satsa.SmartCardSlots;

/**
 * Refer JSR-177 Specification for details
 */
public final class DynamicPropertyHandler implements SystemPropertyProvider
{

    public String getProperty(String key)
    {
        if (key.equals("microedition.smartcardslots"))
        {
            String slots = null;
            try
            {
                SmartCardSlots smartCardSlots = new SmartCardSlots();
                slots = smartCardSlots.getSlots();
                return slots;
            }
            catch (Exception e)
            {

            }
        }
        else
        {
            return null;
        }
        return null;
    }

    public boolean isStatic(String key)
    {
        return false;
    }
}
