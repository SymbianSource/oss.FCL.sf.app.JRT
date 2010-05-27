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

package com.nokia.test.java.eswt.progressbar;

import java.util.TimerTask;
import org.eclipse.swt.widgets.ProgressBar;

public class ProgressTimerTask extends TimerTask
{

    private ProgressBar pb = null;
    public ProgressTimerTask(ProgressBar pb)
    {
        this.pb = pb;
    }
    private int count = 0;
    public void run()
    {
        if (count == pb.getMaximum())
        {
            count = 0;
            pb.setSelection(count);
            pb.getParent().update();
        }
        else
        {
            pb.setSelection(count);
            pb.getParent().update();
        }
        count++;
    }
}

