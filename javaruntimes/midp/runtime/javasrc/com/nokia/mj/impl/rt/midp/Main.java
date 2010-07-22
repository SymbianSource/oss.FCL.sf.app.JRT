/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.midp;

import com.nokia.mj.impl.rt.support.JvmInternal;

import com.nokia.mj.impl.utils.DebugUtils;
import com.nokia.mj.impl.utils.StartUpTrace;

/**
 * A Main entry class of the MIDP runtime.
 */
public final class Main
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * A main entry point of the MIDP runtime.
     * @param args The arguments provided to MIDP runtime.
     */
    public static void main(String[] args)
    {
        // Do the start up trace.
        StartUpTrace.doTrace("Midp runtime ready");
        if (Log.mOn) 
        {
            Log.logI("Starting MIDP runtime");
 	          Log.logI("java.version: " +
                     System.getProperty("java.version"));
            Log.logI("java.fullversion: " +
                     System.getProperty("java.fullversion"));
        }
        boolean ok = true;

        try
        {
            // Start the life cycle.
            MidletLifeCycle.getInstance().doRun(args);
            StartUpTrace.doTrace("Midp runtime closing");
        }
        catch (StartupException se)
        {
            // If the exception is set to be error, an error log is written and
            // status is marked as NOK.
            if (se.isError())
            {
                Log.logE("Startup failure: " + se);
                ok = false;
            }
            else
            {
                if (Log.mOn) Log.logI("Startup cancelled: "+ se);
            }
        }

        catch (Throwable th)
        {
            Log.logE("Error in MIDP runtime: ", th);
            ok = false;
        }

        // Do the cleaning.
        try
        {
            // Close thread dumper.
            DebugUtils.closeThreadDumper();

            // Destroy the lifecycle instance.
            MidletLifeCycle.destroyInstance();

            MemoryLogger.close();

            // Run gc and finalization.
            System.gc();
            JvmInternal.runFinalization();
        }
        catch (Throwable thr)
        {
            Log.logE("Error in MIDP runtime (final clean): ", thr);
            ok = false;
        }

        if (Log.mOn) Log.logI("Closing MIDP runtime, status " +
                                  (ok ? "OK":"NOK"));
    }
}

