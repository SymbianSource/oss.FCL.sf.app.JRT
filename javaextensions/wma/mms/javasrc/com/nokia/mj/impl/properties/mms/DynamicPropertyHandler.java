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


package com.nokia.mj.impl.properties.mms;

import com.nokia.mj.impl.mms.MmsPropertyRetriever;
import com.nokia.mj.impl.rt.support.SystemPropertyProvider;

/**
 * This class implements SystemPropertyProvider and provides getProperty to
 * retrieve dynamic system property.
 */
public final class DynamicPropertyHandler implements SystemPropertyProvider
{

    public String getProperty(String key)
    {
        return MmsPropertyRetriever.getMMSC();
    }

    public boolean isStatic(String key)
    {
        return false;
    }
}