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

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Hashtable;

import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;

import com.nokia.mj.impl.security.common.RuntimeSecurityException;

import com.nokia.mj.impl.utils.ErrorMessageBase;
import com.nokia.mj.impl.utils.ResourceLoader;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.utils.LineReader;

/**
 * Class for handling the runtime error dialogs. There are three kind of dialogs
 * that may be shown in the error cases: Some thread doesn't catch exception,
 * application start up fails by throwing some exception or DRM rights has been
 * expired.
 */

final class RuntimeErrorDialog extends ErrorMessageBase
{
    /**
     * Runtime error codes.
     */
    private static final int UNHANDLED_ERROR         = 1;
    private static final int UNHANDLED_ERROR_DETAILS = 2;
    private static final int START_UP_ERROR          = 3;
    private static final int START_UP_ERROR_DETAILS  = 4;
    private static final int DRM_EXPIRED             = 5;

    /**
     * Maximum count of the lines in stack trace.
     */
    private static final int MAX_LINE_COUNT = 50;

    /**
     * Maximum count of char in sigle line.
     */
    private static final int MAX_CHAR_COUNT = 34;

    /**
     * A hash tble for storing the short and detailed error messages.
     */
    private static Hashtable mMessageTable = null;

    /**
     * A resource loader for loading the localized strings.
     */
    private static ResourceLoader mRes = null;

    /**
     * Will contain all the necessary data for prompting the error dialog.
     */
    private ExceptionBase mExceptionBase;


    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Not allowed to use default constructor of the RuntimeErrorDialog.
     */
    private RuntimeErrorDialog()
    {
    }

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Instantiates the error dialog for the unhandled exception case.
     */
    static final RuntimeErrorDialog getUnhandledDialog(Throwable t)
    {
        if (Log.mOn) Log.logI("Creating unhandled dialog.");
        return new RuntimeErrorDialog(t, UNHANDLED_ERROR,
                                      UNHANDLED_ERROR_DETAILS);
    }

    /**
     * Instantiates the error dialog for the start up error case.
     */
    static final RuntimeErrorDialog getStartUpErrorDialog(Throwable t)
    {
        if (Log.mOn) Log.logI("Creating start up failure dialog.");
        return new RuntimeErrorDialog(t, START_UP_ERROR,
                                      START_UP_ERROR_DETAILS);
    }

    /**
     * Instantiates the error dialog for the start up error case.
     * Detailed message is normal String.
     */
    static final RuntimeErrorDialog getDrmExpiredDialog()
    {
        if (Log.mOn) Log.logI("Creating drm exipred dialog.");
        return new RuntimeErrorDialog(DRM_EXPIRED);

    }

    /**
     * Shows the authentication failure dialog.
     */
    static final void showAuthenticationFailed(RuntimeSecurityException rse)
    {
        if (Log.mOn) Log.logI("Showing authentication failure dialog.");
        // Display the error to user
        RuntimeUi ui = RuntimeUiFactory.getRuntimeUi();
        ui.error(ApplicationInfoImpl.getMidletInfo().getSuiteName(), rse);
        ui.destroy();
    }

    void showDialog()
    {
        if (Log.mOn) Log.logI("Showing failure dialog.");
        RuntimeUi ui = RuntimeUiFactory.getRuntimeUi(true);
        ui.error(ApplicationInfoImpl.getMidletInfo().getName(), mExceptionBase);
        ui.destroy();
    }


    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Constructs the RuntimeErrorDialog with given arguments.
     * @param t The caught throwable leading to error dialog.
     * @param errCode The main error code.
     * @param detailedErrCode The detailed error code.
     */
    private RuntimeErrorDialog(Throwable t, int errCode, int detailedErrCode)
    {
        String[] detailedParams = new String[1];

        // Get the stack trace.
        detailedParams[0] = getStackTrace(t);

        // Show the error dialog.
        mExceptionBase = new ExceptionBase(this,
                                           errCode,
                                           null, // no params for short msg
                                           this,
                                           detailedErrCode,
                                           detailedParams);
    }

    /**
     * Constructs the RuntimeErrorDialog with given arguments.
     * This dialog will not provide RSK.
     * @param errCode The main error code.
     */
    private RuntimeErrorDialog(int errCode)
    {
        // Show the error dialog.
        mExceptionBase = new ExceptionBase(this,
                                           errCode,
                                           null, // no params for short msg
                                           null,
                                           ErrorMessageBase.NO_MSG,
                                           null);
    }


    /**
     * Method for getting message table where key is an Integer value for
     * the error code, and value is an error message String id.
     * @return The hash table containing logical error strings.
     */
    protected Hashtable getMessageTable()
    {
        if (mMessageTable == null)
        {
            mMessageTable = new Hashtable();
            mMessageTable.put(new Integer(UNHANDLED_ERROR), "runtime_error_unhandled");
            mMessageTable.put(new Integer(START_UP_ERROR), "system_app_cannot_start");
            mMessageTable.put(new Integer(UNHANDLED_ERROR_DETAILS), "runtime_error_unhandled_details");
            mMessageTable.put(new Integer(START_UP_ERROR_DETAILS), "runtime_error_unhandled_details");
            mMessageTable.put(new Integer(DRM_EXPIRED), "secur_error_drm_rights_not_valid");
        }
        return mMessageTable;
    }

    /**
     * Method for retrieving the ResourceLoader instance that is used
     * to localise error messages.
     * @return The resource loader to be used when loading localized strings.
     */
    protected ResourceLoader getResourceLoader()
    {
        synchronized (this)
        {
            if (mRes == null)
            {
                mRes = ResourceLoader.getInstance("javausermessages",
                                                  "qtn_java_");
            }
        }
        return mRes;
    }

    /**
     * Gets the stack trace of the throwable. The problem in printing
     * the stack trace into error dialog is that the class names with
     * packages can be very long. The intention is to keep one stack
     * trace element in one line. This is done so that if there are
     * more than MAX_CHAR_COUNT cahrcters in the line, only end of
     * class name will be shown. Also the sources will be stripped away.
     * Example:
     *   at ExceptionInStartUp.startApp(Unknown Source)
     *   at javax.microedition.midlet.MidletApplication.invokeStartApp(MidletApplication.java:30)
     *   at com.nokia.mj.impl.rt.midp.MidletInvoker.run(MidletInvoker.java:76)
     *   at java.lang.Thread.run(Unknown Source)
     * will be:
     *   at ExceptionInStartUp.startApp
     *   at ...tApplication.invokeStartApp
     *   at ...l.rt.midp.MidletInvoker.run
     *   at java.lang.Thread.run
     * If the line count in the stack trace exceeds MAX_LINE_COUNT then
     * the end is stripped away.
     */
    private static String getStackTrace(Throwable t)
    {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        PrintStream printStream = new PrintStream(baos);

        // Use native to get the stack trace into PrintStream.
        _getStackTrace(t, printStream);

        // Create a new ByteArrayInputStream containing the stack trace.
        ByteArrayInputStream bais =
            new ByteArrayInputStream(baos.toByteArray());

        StringBuffer sb = new StringBuffer();
        String lineSeparator = "\u2028";

        try
        {
            // Use LineReader to loop all the lines in ByteArrayInputStream.
            LineReader lr = new LineReader(new InputStreamReader(bais));
            String line;

            boolean firstLine = true;
            int lineCount = 0;

            while ((line = lr.readLine()) != null && lineCount < MAX_LINE_COUNT)
            {
                // Appending a line feed before each new line except for the
                // first line.
                if (firstLine)
                {
                    firstLine = false;
                }
                else
                {
                    sb.append(lineSeparator);
                }

                // Convert the tabulators to single white space.
                line = line.replace('\t', ' ');

                // Remove the end of line after '('
                int index = line.indexOf('(');
                if (index > 0)
                {
                    line = line.substring(0, index);
                    if (line.length() > MAX_CHAR_COUNT)
                    {
                        sb.append(" at ...");

                        index = Math.max(line.length() - MAX_CHAR_COUNT + 7, 4);
                        line = line.substring(index);
                    }
                }
                // Append the line into StringBuffer.
                sb.append(line);
                lineCount++;
            }

            // If the maximum line count was exceeded, mark this using set of
            // dots in the last line.
            if (lineCount == MAX_LINE_COUNT)
            {
                sb.append(lineSeparator);
                sb.append(".......");
            }
            bais.close();
            baos.close();
        }
        catch (IOException ioe)
        {
            Log.logE("Error in stack trace: ", ioe);
        }
        return sb.toString();
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Puts the stack trace into given PrintStream.
     * @param t The caught throwable from where the stack trace should be got.
     * @param printStream A PrintStream where to put the stack trace.
     */
    private static native void _getStackTrace(Throwable t, PrintStream printStream);

}
