/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements TimeBase
*
*/


package com.nokia.microedition.media;

import javax.microedition.media.TimeBase;

public class SystemTimeBase implements TimeBase
{

    /**
     * Constructor
     */
    public SystemTimeBase()
    {
    }

    /**
     * From TimeBase interface
     * @return current time
     */
    public long getTime()
    {
        return System.currentTimeMillis();
    }
}
