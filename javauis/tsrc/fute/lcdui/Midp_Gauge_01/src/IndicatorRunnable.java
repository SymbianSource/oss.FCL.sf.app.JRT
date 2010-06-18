/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * import midp classes.
 */
import javax.microedition.lcdui.*;

/**
 *
 */

public class IndicatorRunnable extends Gauge implements Runnable
{

    private boolean run = false;
    public IndicatorRunnable()
    {
        super("INCREMENTAL_UPDATING", false,  Gauge.INDEFINITE, Gauge.INCREMENTAL_IDLE);
        new Thread(this).start();
    }

    public void setGo()
    {
        new Thread(this).start();
    }

    public void run()
    {
        while (!run)
        {
            setValue(Gauge.INCREMENTAL_UPDATING);
            try
            {
                Thread.currentThread().sleep(500);
            }
            catch (InterruptedException err)
            {
                System.out.print(err.toString());
            }
        }
    }
    void setRun()
    {
        run = true;
    }

}


