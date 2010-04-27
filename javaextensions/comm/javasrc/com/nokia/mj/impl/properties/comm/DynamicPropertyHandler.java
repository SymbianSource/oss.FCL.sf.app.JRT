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


package com.nokia.mj.impl.properties.comm;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
import com.nokia.mj.impl.rt.support.Jvm;

public class DynamicPropertyHandler implements SystemPropertyProvider
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javacomm");
        }
        catch (Exception e)
        {

        }
    }

    public String getProperty(String Key)
    {
        String ret = _getAvailCommPortInfo();
        return ret;
    }

    public boolean isStatic(String Key)
    {
        return false;
    }

    private native String _getAvailCommPortInfo();
}