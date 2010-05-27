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

package com.nokia.mj.impl.properties.test;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;

public class Property2 implements SystemPropertyProvider
{

    private static int mCounter = 0;

    public String getProperty(String key)
    {
        mCounter++;
        return key +" DynamicValue2 ("+mCounter+")";
    }

    public boolean isStatic(String key)
    {
        return true;
    }
}
