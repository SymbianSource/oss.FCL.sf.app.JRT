/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.installer.applicationregistrator.SifNotifier;
import com.nokia.mj.impl.installer.exetable.ExeProgressListener;
import com.nokia.mj.impl.installer.ui.InstallerUi;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

/**
 * InstallationNotifier notifies interested parties about Java
 * application installation progress.
 */
public class InstallationNotifier implements ExeProgressListener
{
    // Installation states
    public static final int INSTALLING = 0x100;
    public static final int INSTALL_OK = 0x101;
    public static final int INSTALL_FAIL = 0x102;
    public static final int UNINSTALLING = 0x200;
    public static final int UNINSTALL_OK = 0x201;
    public static final int UNINSTALL_FAIL = 0x202;

    // Maximum number of progress updates to SysUtil.setProperty().
    private static final int MAX_PROPERTY_PROGRESS_UPDATES = 5;
    // Maximum number of progress updates to SIF.
    private static final int MAX_SIF_PROGRESS_UPDATES = 8;
    // Maximum number of progress updates to UI.
    private static final int MAX_UI_PROGRESS_UPDATES = 20;

    // Flag telling if the properties are already defined.
    private static boolean iPropertiesDefined = false;

    // InstallerUI instance.
    private InstallerUi iInstallerUi = null;
    // SifNotifier instance.
    private SifNotifier iSifNotifier = null;

    // Maximum progress value.
    private int iMaxValue = 1;
    // Tells how often property progress should be updated.
    private int iPropertyProgressStep = 1;
    // Tells how often SIF progress should be updated.
    private int iSifProgressStep = 1;
    // Tells how often UI progress should be updated.
    private int iUiProgressStep = 1;
    // Point between 0 and iMaxValue where the last property
    // update has been made.
    private int iLastPropertyUpdate = 0;
    // Point between 0 and iMaxValue where the last SIF update
    // has been made.
    private int iLastSifUpdate = 0;
    // Point between 0 and iMaxValue where the last UI update
    // has been made.
    private int iLastUiUpdate = 0;
    // Flag telling if the started() method has been called.
    private boolean iStarted = false;
    // Flag telling if the progress maximum value has been reached.
    private boolean iMaxValueReached = false;

    /**
     * Set InstallerUi to be used.
     */
    public void setInstallerUi(InstallerUi aUi)
    {
        iInstallerUi = aUi;
        if (iStarted && iInstallerUi != null)
        {
            // Notify InstallerUi that execution has been started.
            try
            {
                iInstallerUi.started();
            }
            catch (Throwable t)
            {
                Log.logError(
                    "InstallationNotifier: InstallerUi.started threw exception", t);
            }
        }
    }

    /**
     * Set SifNotifier to be used.
     */
    public void setSifNotifier(SifNotifier aSifNotifier)
    {
        iSifNotifier = aSifNotifier;
    }

    /**
     * Execution engine calls started() method to tell that
     * engine has been started.
     */
    public void started()
    {
        Log.log("InstallationNotifier.started");
        iStarted = true;
        if (iInstallerUi != null)
        {
            // Call setInstallerUi() which calls InstallerUi.started().
            setInstallerUi(iInstallerUi);
        }
    }

    /**
     * Execution engine calls ended() method to tell that
     * engine execution has ended.
     */
    public void ended()
    {
        Log.log("InstallationNotifier.ended");
        if (iSifNotifier != null)
        {
            // After this SifNotifier is no longer used, destroy it.
            try
            {
                iSifNotifier.destroy();
                iSifNotifier = null;
            }
            catch (Throwable t)
            {
                Log.logError("InstallationNotifier: SifNotifier.destroy failed", t);
            }
        }
        if (iInstallerUi != null)
        {
            try
            {
                iInstallerUi.ended();
            }
            catch (Throwable t)
            {
                Log.logError(
                    "InstallationNotifier: InstallerUi.ended threw exception", t);
            }
        }
    }

    /**
     * Execution engine calls error() method to tell that
     * an exception has been thrown during engine execution.
     * Even if error() gets called, the engine will still call
     * ended() in the end.
     */
    public void error(Exception aException)
    {
        Log.log("InstallationNotifier.error: " + aException);
        InstallerExceptionBase installerException = null;
        if (aException instanceof InstallerExceptionBase)
        {
            installerException = (InstallerExceptionBase)aException;
        }
        else
        {
            Log.logError(
                "InstallationNotifier.error: Unexpected exception type",
                aException);
            installerException = InstallerException.getInternalErrorException(
                                     aException.toString(), aException);
        }
        if (iInstallerUi != null && installerException != null)
        {
            try
            {
                iInstallerUi.error(installerException);
            }
            catch (Throwable t)
            {
                Log.logError(
                    "InstallationNotifier: InstallerUi.error threw exception", t);
            }
        }
    }

    /**
     * Execution engine calls setMax() method to tell what is the
     * maximum progress indicator value. This method is called once
     * before execution steps are executed. Between each execution
     * step set() method gets called to indicate execution progress.
     * @param maxValue maximum progress indicator value
     */
    public void setMax(int aMaxValue)
    {
        iMaxValue = aMaxValue;
        iPropertyProgressStep = iMaxValue / MAX_PROPERTY_PROGRESS_UPDATES;
        if (iPropertyProgressStep == 0)
        {
            iPropertyProgressStep = 1;
        }
        iSifProgressStep = iMaxValue / MAX_SIF_PROGRESS_UPDATES;
        if (iSifProgressStep == 0)
        {
            iSifProgressStep = 1;
        }
        iUiProgressStep = iMaxValue / MAX_UI_PROGRESS_UPDATES;
        if (iUiProgressStep == 0)
        {
            iUiProgressStep = 1;
        }
    }

    /**
     * Returns maximum progress indicator value.
     */
    public int getMax()
    {
        return iMaxValue;
    }

    /**
     * Execution engine calls set() method between each execution
     * step to indicate execution progress. When all steps are
     * execeuted progress value increases to limit told with
     * setMax() method.
     * @param currentValue current progress indicator value
     */
    public void set(int aCurrentValue)
    {
        if (iMaxValueReached)
        {
            // Ignore possible calls that are made after progress has
            // reached maximum value.
            return;
        }
        if (aCurrentValue >= iMaxValue)
        {
            iMaxValueReached = true;
        }
        int currentPercentage = (aCurrentValue * 100) / iMaxValue;
        Log.log("InstallationNotifier.set: progress " + currentPercentage);
        defineProperties();

        if (isUpdateNeeded(aCurrentValue, iMaxValue,
                           iLastPropertyUpdate, iPropertyProgressStep))
        {
            iLastPropertyUpdate = aCurrentValue;
            Log.log("InstallationNotifier.set: update property to " +
                    currentPercentage);
            // Update property values: progress.
            SysUtil.setPropertyValue(
                SysUtil.PROP_CATEGORY_SYSTEM,
                SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION_PROGRESS,
                currentPercentage);
        }

        if (isUpdateNeeded(aCurrentValue, iMaxValue,
                           iLastSifUpdate, iSifProgressStep))
        {
            if (iSifNotifier != null)
            {
                iLastSifUpdate = aCurrentValue;
                try
                {
                    iSifNotifier.notifyProgress(
                        iSifNotifier.SUB_OP_NO, currentPercentage, 100);
                }
                catch (Throwable t)
                {
                    Log.logError(
                        "InstallationNotifier: SifNotifier.notifyProgress threw exception", t);
                }
            }
        }

        if (isUpdateNeeded(aCurrentValue, iMaxValue,
                           iLastUiUpdate, iUiProgressStep))
        {
            if (iInstallerUi != null)
            {
                iLastUiUpdate = aCurrentValue;
                Log.log("InstallationNotifier.set: update ui to " +
                        currentPercentage);
                try
                {
                    iInstallerUi.updateProgress(currentPercentage);
                }
                catch (Throwable t)
                {
                    Log.logError(
                        "InstallationNotifier: InstallerUi.updateProgress threw exception", t);
                }
            }
        }
    }

    /**
     * This method is called from execution steps to notify
     * that installation or uninstallation
     * is about to start.
     * @param aState installation/uninstallation state
     */
    public void start(int aState)
    {
        Log.log("InstallationNotifier.start: state=" + aState);
        defineProperties();

        // Update property values: uid=0, state.
        SysUtil.setPropertyValue(SysUtil.PROP_CATEGORY_SYSTEM,
                                 SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION, 0);
        SysUtil.setPropertyValue(SysUtil.PROP_CATEGORY_SYSTEM,
                                 SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION_STATE, aState);
    }

    /**
     * This method is called from execution steps to notify
     * that installation or uninstallation is finished.
     * @param aUid uid of the installed/uinstalled application suite
     * @param aState installation/uninstallation state
     */
    public void finish(Uid aUid, int aState)
    {
        Log.log("InstallationNotifier.finish: uid=" + aUid +
                ", state=" + aState);

        // Update property values: uid, state.
        SysUtil.setPropertyValue(SysUtil.PROP_CATEGORY_SYSTEM,
                                 SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION, aUid);
        SysUtil.setPropertyValue(SysUtil.PROP_CATEGORY_SYSTEM,
                                 SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION_STATE, aState);
    }

    /**
     * Define the properties InstallationNotifier updates.
     * This method does not define properties again if this
     * JavaInstaller instance has already defined them.
     */
    static void defineProperties()
    {
        if (!iPropertiesDefined)
        {
            // Define properties to be updated.
            SysUtil.defineProperty(SysUtil.PROP_CATEGORY_SYSTEM,
                                   SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION);
            SysUtil.defineProperty(SysUtil.PROP_CATEGORY_SYSTEM,
                                   SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION_PROGRESS);
            SysUtil.defineProperty(SysUtil.PROP_CATEGORY_SYSTEM,
                                   SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION_STATE);
            iPropertiesDefined = true;
            Log.log("InstallationNotifier: properties defined");
        }
    }

    /**
     * Delete the properties InstallationNotifier updates.
     */
    static void deleteProperties()
    {
        deleteProperty(SysUtil.PROP_CATEGORY_SYSTEM,
                       SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION);
        deleteProperty(SysUtil.PROP_CATEGORY_SYSTEM,
                       SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION_PROGRESS);
        deleteProperty(SysUtil.PROP_CATEGORY_SYSTEM,
                       SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION_STATE);
        Log.log("InstallationNotifier: properties deleted");
    }

    /**
     * Unregisters specified property.
     */
    private static void deleteProperty(Uid aCategory, long aKey)
    {
        try
        {
            SysUtil.deleteProperty(aCategory, aKey);
            Log.log("Deleted property: Uid: " + aCategory +
                    ", key: 0x" + Long.toString(aKey, 16));
        }
        catch (Exception ex)
        {
            Log.logError(
                "InstallationNotifier: Deleting property failed", ex);
        }
    }

    /**
     * Returns true if progress update is needed, false otherwise.
     *
     * @param aCurrent current progress value
     * @param aMax maximum progress value
     * @param aPrevious previously updated progress value
     * @param aStep step between progress updates
     */
    private static boolean isUpdateNeeded(
        int aCurrent, int aMax, int aPrevious, int aStep)
    {
        if (aCurrent == 0 || aCurrent == aMax ||
            aCurrent >= aPrevious + aStep ||
            aCurrent <= aPrevious - aStep)
        {
            return true;
        }
        return false;
    }
}
