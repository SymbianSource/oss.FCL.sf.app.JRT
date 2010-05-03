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
* Description:  ?Description
*
*/


package com.nokia.mj.impl.utils;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;

public final class StartUpTrace
{
    static
    {

        try
        {
            Jvm.loadSystemLibrary("javautils");
        }

        catch (Exception e)
        {
            Logger.ELOG(Logger.EUtils, "Error loading startup trace native.",
                        e);
        }
    }


    /**
     * Utility method for tracing the startup time.
     * @param[in] header The header of the trace.
     * @param[in] freeMem The amount of free memory of the Java heap. Set to -1
     *                    if unknown.
     * @param[in] totalMem The size of the Java heap. Set to -1 if unknown.
     */
    public static void doTrace(String header)
    {
        Runtime rt = Runtime.getRuntime();
        _doTrace(header, rt.freeMemory(), rt.totalMemory());
    }

    private static native void _doTrace(String header, long free, long total);
}
