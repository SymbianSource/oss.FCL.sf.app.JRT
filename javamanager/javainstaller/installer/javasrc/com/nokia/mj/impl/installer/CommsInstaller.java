/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer;

import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.exception.CommsException;
import com.nokia.mj.impl.installer.ui.InstallerUi;
import com.nokia.mj.impl.installer.ui.InstallerUiFactory;
import com.nokia.mj.impl.installer.utils.Args;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Uid;


/**
 * Communicates with TCK Runner, preinstaller, appconverter or debugapi and installs
 * or uninstalls java applications as instructed by those applications.
 *
 * Message Name        Id      Contents
 *
 * Get Next Operation  500     None
 *
 * Operation           501     int operation   (install = 0, uninstall = 1, exit = 2)
 *                             String url      (present if operation = install)
 *                             String uid      (present if operation = uninstall)
 *                             int current     (1..total, present if operation = install
 *                                              and caller is appconverter)
 *                             int total       (total number of apps to convert, present
 *                                              if operation = install and caller is appconverter)
 *
 * Operation Result    502     int operation   (install = 0, uninstall = 1, exit = 2)
 *                             int result      (ok = 0, error codes are negative)
 *                             int appUidCount (present if operation = install, value = uids.length)
 *                             String[] appUids(present if operation = install)
 *                             int suiteUid    (present if operation = install)
 *
 */

public final class CommsInstaller
{
    /**
     * 'Get Next Operation' message
     */
    public static final int MSG_ID_GET_NEXT_OPERATION = 500;
    /**
     * 'Operation' message
     */
    public static final int MSG_ID_OPERATION = 501;
    /**
     * 'Operation Result' message
     */
    public static final int MSG_ID_OPERATION_RESULT = 502;

    /**
     * 'install' operation
     */
    public static final int INSTALL_OPERATION = 0;
    /**
     * 'uninstall' operation
     */
    public static final int UNINSTALL_OPERATION = 1;
    /**
     * 'exit' operation
     */
    public static final int EXIT_OPERATION = 2;

    /**
     * Timeout (seconds) for communication with preinstaller, appconverter and debugapi
     */
    private static final int TIMEOUT_PREINSTALLER = 5;
    /**
     * Timeout (seconds) for communication with TCK Runner
     */
    private static final int TIMEOUT_TCK_RUNNER = CommsEndpoint.WAIT_FOR_EVER;

    /**
     * for internal use only
     */
    protected Args     iArgs;
    /**
     * this is used to pass arguments to the actuall install/uninstall operation
     */
    protected String[] iStringArgs;
    /**
     * for communicating with TCK Runner, preinstaller, appconverter or debugapi
     */
    protected CommsEndpoint iCommsPoll;

    /**
     * InstallerUi instance.
     */
    private InstallerUi iInstallerUi = null;
    /**
     * Flag telling if conversion is to be made without ui.
     */
    private boolean iSilentConversion = false;

    public CommsInstaller(String[] aStringArgs)
    {
        iArgs = new Args(aStringArgs);

        if (iArgs.get("silentconversion") != null)
        {
            iSilentConversion = true;
        }

        // All install and uninstall operations started by
        // TCK Runner, preinstaller, javaappconverter and debugapi are silent ->
        // add '-silent' argument if it is not specified
        if (iArgs.get("silent") == null)
        {
            iStringArgs = new String[aStringArgs.length + 1];
            for (int i = 0; i < aStringArgs.length; i++)
            {
                iStringArgs[i] = aStringArgs[i];
            }
            iStringArgs[aStringArgs.length] = "-silent";
        }
        else
        {
            iStringArgs = aStringArgs;
        }
    }

    public int run()
    {
        int commsEndpoint = 0;
        int returnCode = Installer.ERR_NONE;
        boolean exitInstaller = false;
        if (iArgs.valueContains("address", "tck"))
        {
            commsEndpoint = CommsEndpoint.TCK_RUNNER;
        }
        else if (iArgs.valueContains("address", "preinstall"))
        {
            commsEndpoint = CommsEndpoint.JAVA_PREINSTALLER;
        }
        else if (iArgs.valueContains("address", "convert"))
        {
            commsEndpoint = CommsEndpoint.JAVA_APPCONVERTER;
        }
        else if (iArgs.valueContains("address", "debugapi"))
        {
            commsEndpoint = CommsEndpoint.JAVA_DEBUGAPI;
        }
        if (0 == commsEndpoint)
        {
            // cannot start in poll mode
            Log.logError("CommsInstaller: Unknown or missing value of " +
                         "-address argument, cannot start in poll mode.");
            return Installer.ERR_GENERAL;
        }

        iCommsPoll = CommsEndpoint.find("InstallerCommsPoll");
        if (iCommsPoll == null)
        {
            Log.logError(
                "CommsInstaller: Cannot start in poll mode (conn to TCK " +
                "Runner/preinstaller/appconverter/debugapi failed). ");
            return Installer.ERR_GENERAL;
        }

        do
        {
            // Send 'Get Next Operation' message
            CommsMessage message = new CommsMessage();
            message.setMessageId(MSG_ID_GET_NEXT_OPERATION);
            message.setModuleId(0);  // not used with TCK runner, preinstaller, appconverter or debugapi

            // preinstaller, appconverter and debugapi should respond quickly
            // but running the previously installed
            // test cases may take for a very long time in TCK Runner
            int timeout = (commsEndpoint == CommsEndpoint.TCK_RUNNER) ?
                          TIMEOUT_TCK_RUNNER : TIMEOUT_PREINSTALLER;
            CommsMessage response;

            try
            {
                response = iCommsPoll.sendReceive(message, timeout);
            }
            catch (CommsException ce)
            {
                Log.logError("CommsInstaller: Cannot get next operation from TCK " +
                             "Runner, preinstaller, appconverter or debugapi. ", ce);
                return Installer.ERR_GENERAL;
            }

            // Parse operation from response ('Operation' message)
            if (response.getMessageId() != MSG_ID_OPERATION)
            {
                Log.logError("CommsInstaller: Illegal response to next operation request.");
                return Installer.ERR_GENERAL;
            }

            int oper = response.readInt();
            int ret = Installer.ERR_NONE;
            if (EXIT_OPERATION == oper)
            {
                if (commsEndpoint == CommsEndpoint.JAVA_APPCONVERTER &&
                        !iSilentConversion)
                {
                    if (iInstallerUi != null)
                    {
                        // Dismiss app conversion ui indicator and dispose ui.
                        updateAppConversionIndicator(0, 0);
                        iInstallerUi = null;
                    }
                }
                // Stop polling for new operations and eventually
                // exit whole installer normally.
                exitInstaller = true;
            }
            else if (INSTALL_OPERATION == oper)
            {
                // Call installer with modified command line

                // replace 'poll' with 'install'
                iStringArgs[0] = "install";

                // if file name ends with '.jad' then file is .jad,
                // else it is .jar file
                String jadJarUrl = response.readString();
                if (jadJarUrl.endsWith(".jad"))
                {
                    // replace '-address=...' with '-jad=...'
                    iStringArgs[1] = "-jad=" + jadJarUrl;
                }
                else
                {
                    // replace '-address=...' with '-jar=...'
                    iStringArgs[1] = "-jar=" + jadJarUrl;
                }

                // javaappconverter sends number of converted apps,
                // lets update the conversion indicator.
                if (commsEndpoint == CommsEndpoint.JAVA_APPCONVERTER &&
                        !iSilentConversion)
                {

                    int current = response.readInt();
                    int total = response.readInt();
                    if (total > 2)
                    {
                        // Update conversion indicator only if there is
                        // more than two applications to convert.
                        updateAppConversionIndicator(current, total);
                    }
                }

                ret = Installer.mainWithResult(iStringArgs);
            }
            else if (UNINSTALL_OPERATION == oper)
            {
                // TCK Runner uses the string representation for uid
                String midletUid = response.readString();

                // Call installer with modified command line

                // replace 'poll' with 'uninstall'
                iStringArgs[0] = "uninstall";
                // replace '-address=...' with '-uid=...'
                iStringArgs[1] = "-uid=" + midletUid;

                ret = Installer.mainWithResult(iStringArgs);
            }
            else
            {
                Log.logError("CommsInstaller: Unknown operation requested.");
                return Installer.ERR_GENERAL;
            }

            // Installer.mainWithResult can return internal return code
            // Installer.RET_NO_SYSTEM_EXIT. Remember that this function
            // must return with the same return code but do not use it
            // in Comms message
            if (ret == Installer.RET_NO_SYSTEM_EXIT)
            {
                returnCode = Installer.RET_NO_SYSTEM_EXIT;
                ret = Installer.ERR_NONE;
            }

            // Send 'Operation Result' message
            CommsMessage result = new CommsMessage();
            result.setMessageId(MSG_ID_OPERATION_RESULT);
            result.setModuleId(0);  // not used with TCK runner,
            // preinstaller, appconverter or debugapi
            result.write(oper);
            result.write(ret);
            if (INSTALL_OPERATION == oper)
            {
                if (Installer.ERR_NONE != ret)
                {
                    // installation failed, result contains 0 uids
                    result.write(0);
                }
                else
                {
                    // Get the uids of the installed applications
                    // from Installer static uid array.
                    result.write(Installer.iInstalledApps.length);
                    for (int i = 0; i < Installer.iInstalledApps.length; i++)
                    {
                        result.write(Installer.iInstalledApps[i].toString());
                    }
                    // Get installed suite uid.
                    if (Installer.iInstalledSuite != null)
                    {
                        result.write(Installer.iInstalledSuite.toString());
                    }
                    else
                    {
                        // Suite uid not available, write a zero.
                        result.write(0);
                    }
                }
            }

            try
            {
                iCommsPoll.send(result);
            }
            catch (CommsException ce)
            {
                Log.logError("CommsInstaller: Cannot send operation result to TCK " +
                             "Runner, preinstaller, appconverter or debugapi. ", ce);
                return Installer.ERR_GENERAL;
            }
        }
        while (!exitInstaller);

        return returnCode;
    }

    private void updateAppConversionIndicator(int aCurrent, int aTotal)
    {
        if (iInstallerUi == null)
        {
            try
            {
                iInstallerUi = InstallerUiFactory.getInstallerUi
                               (InstallerUi.MODE_APP_CONVERSION, null);
            }
            catch (Throwable t)
            {
                Log.logError("CommsInstaller: Error while getting UI", t);
            }
        }
        if (iInstallerUi != null)
        {
            try
            {
                iInstallerUi.updateAppConversionIndicator(aCurrent, aTotal);
            }
            catch (Throwable t)
            {
                Log.logError("CommsInstaller: Error during UI update", t);
            }
        }
    }
}
