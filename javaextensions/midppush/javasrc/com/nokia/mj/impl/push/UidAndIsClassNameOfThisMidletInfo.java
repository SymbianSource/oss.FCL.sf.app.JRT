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

final class UidAndIsClassNameOfThisMidletInfo
{
    public Uid mUid;
    public boolean mIsClassNameOfThisMidlet = true;
    public UidAndIsClassNameOfThisMidletInfo(Uid aUid,boolean aIsClassNameOfThisMidlet)
    {
        mUid = aUid;
        mIsClassNameOfThisMidlet = aIsClassNameOfThisMidlet;
    }
}
