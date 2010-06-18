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

package com.nokia.midp.test.midptest;

import javax.microedition.lcdui.*;
import java.util.*;

class TestTimer extends TimerTask
{
    Timer iTimer;
    Runnable iRunnable;

    public TestTimer(Runnable aRunnable)
    {
        iRunnable = aRunnable;
        iTimer = new Timer();
        iTimer.scheduleAtFixedRate(this, (long)1000, 1000);
    }

    public void run()
    {
        iRunnable.run();
    }
}
