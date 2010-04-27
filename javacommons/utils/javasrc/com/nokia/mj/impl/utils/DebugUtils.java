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

public final class DebugUtils
{
    static
    {

        try
        {
            Jvm.loadSystemLibrary("javautils");
        }

        catch (Exception e)
        {
            Logger.ELOG(Logger.EUtils, "Error loading debug utils native.",
                        e);
        }
    }

    /**
     * Asks Vm to do thread dump if the dumping libraries are present.
     */
    public static void doThreadDump()
    {
        _doThreadDump();
    }

    /**
     * Closes the thread dumper thread.
     */
    public static void closeThreadDumper()
    {
        _closeThreadDumper();
    }

    private static native void _doThreadDump();
    private static native void _closeThreadDumper();
}