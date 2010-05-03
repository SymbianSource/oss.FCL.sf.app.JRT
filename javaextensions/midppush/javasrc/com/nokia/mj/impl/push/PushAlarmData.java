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
import java.lang.String;

final class PushAlarmData
{

    public Uid mUid;
    public String mTime = "";

    PushAlarmData() {}

    PushAlarmData(Uid aUid,String aTime)
    {
        mUid = aUid;
        mTime = aTime;
    }

}//end PushAlarmData

