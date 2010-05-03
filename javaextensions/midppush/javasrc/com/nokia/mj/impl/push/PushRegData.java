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

package com.nokia.mj.impl.push;

import com.nokia.mj.impl.utils.Uid;

final class PushRegData
{
    public static final int REG_DYNAMIC = 0;
    public static final int REG_STATIC = 1;

    public Uid mUid = null;
    public int mRegType = REG_STATIC;

    PushRegData() {}

    PushRegData(Uid aUid,int aRegType)
    {
        mUid = aUid;
        mRegType = aRegType;
    }

    public Uid getUid()
    {
        return mUid;
    }

    public int getRegType()
    {
        return mRegType;
    }
}//end final class PushRegData

