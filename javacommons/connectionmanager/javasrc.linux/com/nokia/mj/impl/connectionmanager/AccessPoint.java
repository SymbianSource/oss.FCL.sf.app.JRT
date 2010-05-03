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


package com.nokia.mj.impl.connectionmanager;

import com.nokia.mj.impl.rt.support.*;
import com.nokia.mj.impl.utils.Uid;



/*
JavaImplementation of AccessPoint class
*/

public class AccessPoint
{
    public static int NAP_INITIAL = -1;
    public static int NAP_ERR = 0;
    public static int NAP_NONE = 1;
    public static int NAP_SNAP = 2;
    public static int NAP_IAP = 3;
    public static int NAP_ONCE = 4;
    public static int NAP_ASKALWAYS = 5;
    public static int NAP_DEFAULT = 6;

    private int iType = -1;
    private int iNapId = -1;

    public AccessPoint()
    {
    }
    public AccessPoint(int aType, int aId)
    {
        iType = aType;
        iNapId = aId;
    }

    public int getType()
    {
        return iType;
    }

    public int getNapId()
    {
        return iNapId;
    }
    public void setType(int aType)
    {
        iType = aType;
    }
}
