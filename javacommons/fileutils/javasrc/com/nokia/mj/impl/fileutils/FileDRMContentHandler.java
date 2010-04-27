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

package com.nokia.mj.impl.fileutils;

import com.nokia.mj.impl.utils.Logger;

/**
 * Manages DRM Content. Provides utility to check if a given file is DRM
 * protected.
 *
 */
public class FileDRMContentHandler extends FileStreamHandler
{
    public static final int Peek = 0; // Intent to peek at content.

    public static final int Play = 1; // Intent to play content.

    public static final int View = 2; // Intent to view content.

    public static final int Execute = 3; // Intent to execute content.

    public static final int Print = 4; // Intent to print content.

    /**
     * Checks to see if the file name presented is DRM protected.
     *
     * @param aFileName
     *            absolute path of the file.
     * @return true in case it is DRM protected content. False otherwise.
     */
    public static boolean isDRMProtectedFile(String aFileName)
    {
        if (null == aFileName)
        {
            throw new NullPointerException("Argument passed was null");
        }

        return _isProtectedFile(aFileName);
    }

    /**
     * Construction and usage of FileDRMContentHandler must be restricted to
     * FileUtility alone. It cannot be used as a stand alone class.
     *
     * @param aFileName
     *            name of the file.
     */
    FileDRMContentHandler(String aFileName)
    {
        this(aFileName, Execute, false);
    }

    FileDRMContentHandler(String aFileName, int aIntent, boolean aExecuteIntent)
    {
        super(aFileName);
        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "--> FileDRMContentHandler:Constructor:  Setting DRM Params");
        checkIntentParam(aIntent);
        _setDrmArguments(iNativeHandle, aExecuteIntent, aIntent);
    }

    /**
     * Checks to see if the intent specified is correct.
     *
     * @param aIntent
     *            intent specified by the user
     */
    private void checkIntentParam(int aIntent)
    {
        switch (aIntent)
        {
        case Peek: // Fallthrough
        case Play: // Fallthrough
        case View: // Fallthrough
        case Execute: // Fallthrough
        case Print:
            return;
        default:
            throw new IllegalArgumentException(
                "Specified DRM intent was incorrect");
        }
    }

    /**
     * Creates native DRM Handler
     */
    protected int createNativePeer(String aFileName)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "--> FileDRMContentHandler:Createnativepeer");
        return _createDrmHandler(aFileName);
    }

    protected void closeFileStream()
    {
        // This is called only when changing attributes of a file.
        // In case it is a DRM file, it is not allowed. Throw security Exception
        throw new SecurityException("Permission denied");
    }

    private static native boolean _isProtectedFile(String aFileName);

    private native void _setDrmArguments(int aNativeHandle,
                                         boolean aExecuteIntent, int aIntent);

    private native int _createDrmHandler(String aFileName);
}
