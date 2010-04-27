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


package com.nokia.mj.impl.installer.exetable;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

import java.util.Vector;

/**
 * Execution engine.
 *
 * @author Nokia Corporation
 * @version $Rev: 9041 $ $Date: 2009-12-07 18:36:08 +0200 (Mon, 07 Dec 2009) $
 */
public final class ExeEngine
{

    private ExeProgressListener iProgressListener = null;

    public ExeEngine()
    {
    }

    public ExeEngine(ExeProgressListener aProgressListener)
    {
        iProgressListener = aProgressListener;
    }

    public void execute(ExeTable aTable, ExeBall aBall)
    {
        long startTime = System.currentTimeMillis();
        int totalDuration = 0;
        int currentDuration = 0;
        Vector durationVector = new Vector();

        if (iProgressListener != null)
        {
            iProgressListener.started();
            aTable.reset();
            for (ExeStep step = aTable.getNext();
                    step != null; step = aTable.getNext())
            {
                int duration = step.estimateDuration(aBall);
                durationVector.addElement(new Integer(duration));
                totalDuration += duration;
            }
            iProgressListener.setMax(totalDuration);
            iProgressListener.set(currentDuration);
        }

        // Keep track on how long it takes to execute each step.
        long[] exeStepTimes = new long[aTable.size()];
        long[] cancelStepTimes = null;
        long time1 = 0;
        long time2 = 0;

        aTable.reset();
        int i = 0;
        for (ExeStep step = aTable.getNext();
                step != null; step = aTable.getNext())
        {
            aBall.exeStepChanged();
            try
            {
                // checkForCancel() method will throw appropriate
                // exception if user has cancelled the execution.
                aBall.checkForCancel();
                Log.log("Executing step " + i + ": " +
                        step.getClass().getName());
                time1 = System.currentTimeMillis();
                step.execute(aBall);
                time2 = System.currentTimeMillis();
                exeStepTimes[i] = time2 - time1;
                if (iProgressListener != null)
                {
                    currentDuration +=
                        ((Integer)durationVector.get(i)).intValue();
                    iProgressListener.set(currentDuration);
                }
                i++;
            }
            catch (Throwable t)
            {
                if (t instanceof InstallerExceptionBase)
                {
                    aBall.setExecuteException((InstallerExceptionBase)t);
                }
                else
                {
                    aBall.setExecuteException(
                        InstallerException.getInternalErrorException(
                            t.toString(), t));
                }
                aTable.getPrev(); // Do not call cancel for the failed step.
                break;
            }
        }

        if (aBall.getExecuteException() != null)
        {
            if (!aBall.isCancelled())
            {
                // Do not make error log entry in case user
                // has cancelled execution.
                Log.logError("Installer error at step " + i,
                             aBall.getExecuteException());
            }
            cancelStepTimes = new long[aTable.size()];
            i--; // Failed step is not cancelled, adjust index.
            for (ExeStep step = aTable.getPrev();
                    step != null; step = aTable.getPrev())
            {
                aBall.exeStepChanged();
                try
                {
                    Log.log("Cancelling step " + i + ": " +
                            step.getClass().getName());
                    time1 = System.currentTimeMillis();
                    step.cancel(aBall);
                    time2 = System.currentTimeMillis();
                    cancelStepTimes[i] = time2 - time1;
                    if (iProgressListener != null)
                    {
                        currentDuration -=
                            ((Integer)durationVector.get(i)).intValue();
                        iProgressListener.set(currentDuration);
                    }
                    i--;
                }
                catch (Throwable t)
                {
                    if (t instanceof InstallerExceptionBase)
                    {
                        aBall.setCancelException((InstallerExceptionBase)t);
                    }
                    else
                    {
                        aBall.setCancelException(
                            InstallerException.getInternalErrorException(
                                t.toString(), t));
                    }
                    break;
                }
            }
        }

        if (aBall.getCancelException() != null)
        {
            Log.logError("Installer cancellation error at step " + i,
                         aBall.getCancelException());
        }

        // Log time spent in different steps.
        logStepTimes(aTable, aBall, exeStepTimes, cancelStepTimes);
        // Log total time spent in execute method.
        long totalTime = System.currentTimeMillis() - startTime;
        logTimes(aBall, "Total execution time: " + totalTime + " ms");

        if (aBall.getExecuteException() != null &&
                iProgressListener != null &&
                !aBall.isCancelled())
        {
            iProgressListener.error(aBall.getExecuteException());
        }

        if (iProgressListener != null)
        {
            iProgressListener.ended();
        }
    }

    private static void logStepTimes
    (ExeTable aTable, ExeBall aBall,
     long[] aExeStepTimes, long[] aCancelStepTimes)
    {

        String timesMsg = aBall.getTimesMsg();
        if (timesMsg != null)
        {
            logTimes(aBall, timesMsg);
        }

        logTimes(aBall, "Time spent in execution steps (ms):");
        long totalTime = 0;
        int i = 0;
        aTable.reset();
        for (ExeStep step = aTable.getNext();
                step != null; step = aTable.getNext(), i++)
        {
            logTimes(aBall, "Step " + i + ": " + aExeStepTimes[i] +
                     " (" + step.getClass().getName() + ")");
            totalTime += aExeStepTimes[i];
        }
        logTimes(aBall, "Total time in execution steps: " +
                 totalTime + " ms");

        if (aCancelStepTimes != null)
        {
            logTimes(aBall, "Time spent in cancel steps (ms):");
            totalTime = 0;
            i = 0;
            aTable.reset();
            for (ExeStep step = aTable.getNext();
                    step != null; step = aTable.getNext(), i++)
            {
                logTimes(aBall, "Step " + i + ": " + aCancelStepTimes[i] +
                         " (" + step.getClass().getName() + ")");
                totalTime += aCancelStepTimes[i];
            }
            logTimes(aBall, "Total time in cancel steps: " +
                     totalTime + " ms");
        }
    }

    private static void logTimes(ExeBall aBall, String aMsg)
    {
        if (aBall != null && aBall.isDebugSet(aBall.DBG_TIMES_OUT))
        {
            // Write to System.out if DBG_TIMES_OUT is set.
            // Uncomment the next line to enable installer
            // execution times logging to System.out when
            // jad attribute
            //   Nokia-MIDlet-Install-Debug: times
            // has been sepcified.
            //System.out.println(aMsg);
        }
        // Always write to log file.
        Log.log(aMsg);
    }
}
