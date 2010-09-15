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


package com.nokia.mj.impl.installer.applicationregistrator;

import com.nokia.mj.impl.installer.ui.InstallerUi;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PropertyListener;
import com.nokia.mj.impl.installer.utils.PropertyProvider;

/**
 * Sends installation and uninstallation progress notifications
 * to platform's software installation framework.
 */
public final class SifNotifier
{
    /** Install operation. */
    public static final int OP_INSTALL = 1; // TSifOperationPhase::EInstalling
    /** Uninstall operation. */
    public static final int OP_UNINSTALL = 2; // TSifOperationPhase::EUninstalling
    /** Update operation. */
    public static final int OP_UPDATE = 3; // TSifOperationPhase::EUpgrading

    /** Indicates installation or uninstallation without
        specific suboperation. */
    public static final int SUB_OP_NO = 1; // TSifOperationSubPhase::ENoSubPhase
    /** OCSP phase during installation. */
    public static final int SUB_OP_OCSP = 2; // TSifOperationSubPhase::EOCSPCheck
    /** Download phase during installation. */
    public static final int SUB_OP_DOWNLOAD = 3; // TSifOperationSubPhase::EDownload
    /** Maximum progress notification value. */
    private static final int MAX_PROGRESS = 100;

    /** Operation being notified. */
    private int iOperation = 0;
    /** Suboperation during installation. */
    private int iSubOperation = 0;
    /** Global component id for the application. */
    private String iGlobalComponentId = null;
    /** Component name (i.e. suite name). */
    private String iComponentName = null;
    /** Application names. */
    private String[] iApplicationNames = null;
    /** Applications icons. */
    private String[] iApplicationIcons = null;
    /** Component initial size. */
    private int iComponentSize = 0;
    /** Icon dir. */
    private String iIconDir = null;
    /** Component icon. */
    private String iComponentIcon = null;

    /** Sending progress notifications is only allowed between start
     *  and end notifications. */
    private boolean iNotifyProgressAllowed = false;
    /**
     * Value of the last progress notification that has been sent with
     * SUB_OP_NO suboperation.
     */
    private int iLastProgressSent = 0;
    /** Current value of the last progress notification that has been sent. */
    private int iCurrentValue = 0;
    /** total value of the last progress notification that has been sent. */
    private int iTotalValue = 0;

    /** Native notifier object handle. */
    private int iHandle = 0;
    /** Native indicator object handle. */
    private int iIndicatorHandle = 0;
    /** InstallerUi handle. */
    private InstallerUi iInstallerUi = null;
    /** Provider for indicator status events. */
    private PropertyProvider iIndicatorStatusProvider = null;
    /** Indicator state. */
    private int iIndicatorState = -1;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor.
     */
    public SifNotifier()
    {
        init();
    }

    /**
     * Set InstallerUi used when handling indicator.
     */
    public void setInstallerUi(InstallerUi aInstallerUi)
    {
        iInstallerUi = aInstallerUi;
    }

    /**
     * Activates and updates indicator which displays installation
     * progress to user while installer UI is hidden. The notifyStart
     * and notifyProgress methods must be called at least once before
     * calling this method.
     */
    public void activateIndicator()
    {
        if (iInstallerUi == null)
        {
            return;
        }

        if (iIndicatorHandle == 0)
        {
            int ret = _initIndicator();
            if (ret < 0)
            {
                Log.logError(
                    "Initializing SifNotifier indicator failed with code " +
                    ret);
            }
            else
            {
                Log.log("SifNotifier indicator created");
            }
            iIndicatorHandle = ret;
        }

        if (iIndicatorHandle == 0)
        {
            return;
        }

        int phase = 0;
        switch (iSubOperation)
        {
        case SUB_OP_OCSP: phase = 2; break; // TInstallingPhase::ECheckingCerts
        case SUB_OP_DOWNLOAD: phase = 1; break; // TInstallingPhase::EDownloading
        default: phase = 0; // TInstallingPhase::EInstalling
        }
        int progress = (iTotalValue == 0? 0: iCurrentValue*100/iTotalValue);
        updateIndicator(iComponentName, phase, progress);

        if (iIndicatorStatusProvider == null)
        {
            // Create PropertyListener which listens indicator status events
            // and unhides UI when necessary.
            final int indicatorCategory = 0x20022FC5; // sifuiinstallindicatorplugin
            final int indicatorKey = 0x2002E690; // /SifUiInstallIndicator/Status
            iIndicatorStatusProvider = new PropertyProvider();
            iIndicatorStatusProvider.subscribe(
                indicatorCategory, indicatorKey, new PropertyListener()
                {
                    public void valueChanged(int aCategory, int aKey, int aValue)
                    {
                        Log.log("SifNotifier indicator status " + aValue +
                                " (category=" + aCategory + ", key=" + aKey + ")");
                        iIndicatorState = aValue;
                        if (iIndicatorState == 0)
                        {
                            // Indicator has been closed, unhide the UI.
                            iInstallerUi.hide(false);
                        }
                    }
                });
            Log.log("SifNotifier indicator status provider subscribed");
        }
    }

    /**
     * Updates indicator which displays installation progress to user
     * while installer UI is hidden. The activateindicator method must
     * be called before calling this method.
     */
    public void updateIndicator(String aName, int aPhase, int aProgress)
    {
        if (iInstallerUi == null || iIndicatorHandle == 0)
        {
            return;
        }

        final String name = aName;
        final int phase = aPhase;
        final int progress = aProgress;
        iInstallerUi.syncExec(new Runnable()
        {
            // Indicator must be updated from UI thread.
            public void run()
            {
                int ret = _updateIndicator(
                    iIndicatorHandle, name, phase, progress);
                if (ret < 0)
                {
                    Log.logError(
                        "Updating SifNotifier indicator failed with code " +
                        ret);
                }
                else
                {
                    Log.log("SifNotifier indicator updated: " + name +
                            ", " + phase + ", " + progress + "%");
                }
            }
        });
    }

    /**
     * Deactivates indicator which displays installation
     * progress to user while installer UI is hidden.
     */
    public void deactivateIndicator()
    {
        if (iIndicatorStatusProvider != null)
        {
            iIndicatorStatusProvider.unsubscribe();
            iIndicatorStatusProvider = null;
            Log.log("SifNotifier indicator status provider unsubscribed");
        }

        if (iInstallerUi == null)
        {
            return;
        }

        iInstallerUi.syncExec(new Runnable()
        {
            // Indicator must be deactivated from UI thread.
            public void run()
            {
                if (iIndicatorHandle == 0)
                {
                    return;
                }
                int ret = _destroyIndicator(iIndicatorHandle, iIndicatorState);
                if (ret < 0)
                {
                    Log.logError(
                        "Destroying SifNotifier indicator failed with code " +
                        ret);
                }
                else
                {
                    Log.log("SifNotifier indicator destroyed");
                }
                iIndicatorHandle = 0;
            }
        });
    }

    /**
     * Returns true if SIF progress notifications are enabled, false otherwise.
     */
    public static boolean enabled()
    {
        return _sifNotifierEnabled();
    }

    /*
     * Notifies SIF that installation or uninstallation has started.
     *
     * @throws InstallerException in case an error occurs
     */
    public void notifyStart(
        int aOperation, String aGlobalComponentId, String aComponentName,
        String[] aApplicationNames, String[] aApplicationIcons,
        int aComponentSize, String aIconDir, String aComponentIcon)
    {
        iOperation = aOperation;
        iGlobalComponentId = aGlobalComponentId;
        iComponentName = aComponentName;
        iApplicationNames = aApplicationNames;
        iApplicationIcons = aApplicationIcons;
        iComponentSize = aComponentSize;
        iIconDir = aIconDir;
        iComponentIcon = aComponentIcon;

        checkHandle();
        int ret = _notifyStart(
                      iHandle, aOperation, aGlobalComponentId, aComponentName,
                      aApplicationNames, aApplicationIcons,
                      aComponentSize, aIconDir, aComponentIcon);
        if (ret < 0)
        {
            Log.log("Notifying SIF start failed with code " + ret +
                    ", " + getInfoString());
            InstallerException.internalError(
                "Notifying SIF start failed with code " + ret);
        }
        else
        {
            Log.log("SifNotifier.notifyStart: " + getInfoString());
        }
        iNotifyProgressAllowed = true;
    }

    /**
     * Notifies SIF that installation or uinstallation has ended.
     *
     * @throws InstallerException in case an error occurs
     */
    public void notifyEnd(
        int aErrCategory, int aErrCode, String aErrMsg, String aErrMsgDetails)
    {
        checkHandle();
        if (aErrCategory == 0 && iLastProgressSent < MAX_PROGRESS)
        {
            // Before sending end notification, update progress to max if
            // operation was successful and max progress notification has
            // not yet been sent.
            notifyProgress(SUB_OP_NO, MAX_PROGRESS, MAX_PROGRESS);
        }
        // No more progress notifications allowed.
        iNotifyProgressAllowed = false;
        int ret = _notifyEnd(
                      iHandle, iGlobalComponentId, aErrCategory, aErrCode,
                      aErrMsg, aErrMsgDetails);
        String logMsg =
            "ErrCategory: " + aErrCategory +
            ", ErrCode: " + aErrCode +
            ", ErrMsg: " + aErrMsg +
            ", ErrMsgDetails: " + aErrMsgDetails;
        if (ret < 0)
        {
            Log.log("Notifying SIF end failed with code " + ret +
                    ", " + getInfoString() + ", " + logMsg);
            InstallerException.internalError(
                "Notifying SIF end failed with code " + ret);
        }
        else
        {
            Log.log("SifNotifier.notifyEnd: " + logMsg);
        }
    }

    /**
     * Notifies SIF about installation or uninstallation progress.
     *
     * @throws InstallerException in case an error occurs
     */
    public void notifyProgress(int aSubOperation, int aCurrent, int aTotal)
    {
        if (!iNotifyProgressAllowed)
        {
            return;
        }

        iSubOperation = aSubOperation;
        iCurrentValue = aCurrent;
        iTotalValue = aTotal;
        if (iIndicatorHandle != 0)
        {
            // Call activateIndicator so that indicator gets updated.
            activateIndicator();
        }

        checkHandle();
        if (aSubOperation == SUB_OP_NO)
        {
            iLastProgressSent = aCurrent;
        }
        int ret = _notifyProgress(
                      iHandle, iGlobalComponentId, iOperation, aSubOperation,
                      aCurrent, aTotal);
        String logMsg =
            "SubOp: " + aSubOperation +
            ", Current: " + aCurrent +
            ", Total: " + aTotal;
        if (ret < 0)
        {
            Log.log("Notifying SIF progress failed with code " + ret +
                    ", " + getInfoString() + ", " + logMsg);
            InstallerException.internalError(
                "Notifying SIF progress failed with code " + ret);
        }
        else
        {
            Log.log("SifNotifier.notifyProgress: " + logMsg);
        }
    }

    /**
     * Destroys SifNotifier. This method releawse native resources and
     * must be called after SifNotifier is no longer used.
     *
     * @throws InstallerException in case an error occurs
     */
    public void destroy()
    {
        deactivateIndicator();
        checkHandle();
        int ret = _destroy(iHandle);
        if (ret < 0)
        {
            InstallerException.internalError(
                "Destroying SIF notifier failed with code " + ret);
        }
        else
        {
            Log.log("SifNotifier destroyed");
        }
        iHandle = 0;
    }

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Initializes SifNotifier. This method must be called before any
     * other methods are called.
     *
     * @throws InstallerException if the notifier cannot be initialized
     */
    void init()
    {
        int ret = _init();
        if (ret < 0)
        {
            InstallerException.internalError(
                "Initializing SifNotifier failed with code " + ret);
        }
        else
        {
            Log.log("SifNotifier created");
        }
        iHandle = ret;
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Checks if notifier instance has been initialized.
     * @throws InstallerException if notifier has not been initialized
     */
    private void checkHandle()
    {
        if (iHandle == 0)
        {
            InstallerException.internalError(
                "SifNotifier.destroy: notifier has not been initialized");
        }
    }

    /**
     * Returns notification info string used in logging.
     */
    private String getInfoString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append("Operation: ").append(iOperation);
        buf.append(", GlobalComponentId: ").append(iGlobalComponentId);
        buf.append(", ComponentName: ").append(iComponentName);
        if (iApplicationNames != null)
        {
            for (int i = 0; i < iApplicationNames.length; i++)
            {
                buf.append(", ApplicationName[").append(i).append("]: ")
                    .append(iApplicationNames[i]);
            }
        }
        if (iApplicationIcons != null)
        {
            for (int i = 0; i < iApplicationIcons.length; i++)
            {
                buf.append(", ApplicationIcon[").append(i).append("]: ")
                    .append(iApplicationIcons[i]);
            }
        }
        buf.append(", ComponentSize: ").append(iComponentSize);
        if (iIconDir != null)
        {
            buf.append(", IconDir: ").append(iIconDir);
        }
        if (iComponentIcon != null)
        {
            buf.append(", ComponentIcon: ").append(iComponentIcon);
        }
        return buf.toString();
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Returns true if SIF progress notifications are enabled, false otherwise.
     */
    private static native boolean _sifNotifierEnabled();

    /**
     * Notifies SIF about installation/uinstallation start.
     *
     * @param aHandle
     * @param aOperation
     * @param aGlobalComponentId
     * @param aComponentName
     * @param aApplicationNames
     * @param aApplicationIcons
     * @param aComponentSize
     * @param aIconDir
     * @param aComponentIcon
     * @return Symbian error code (negative number) if operation fails,
     * otherwise 0
     */
    private static native int _notifyStart(
        int aHandle, int aOperation, String aGlobalComponentId,
        String aComponentName, String[] aApplicationNames,
        String[] aApplicationIcons, int aComponentSize,
        String aIconDir, String aComponentIcon);

    /**
     * Notifies SIF about installation/uinstallation completion.
     *
     * @param aHandle
     * @param aGlobalComponentId
     * @param aErrCategory
     * @param aErrCode
     * @param aErrMsg
     * @param aErrMsgDetails
     * @return Symbian error code (negative number) if operation fails,
     * otherwise 0
     */
    private static native int _notifyEnd(
        int aHandle, String aGlobalComponentId,
        int aErrCategory, int aErrCode,
        String aErrMsg, String aErrMsgDetails);

    /**
     * Notifies SIF about installation/uinstallation progress.
     *
     * @param aHandle
     * @param aGlobalComponentId
     * @param aOperation
     * @param aSubOperation
     * @param aCurrent
     * @param aTotal
     * @return Symbian error code (negative number) if operation fails,
     * otherwise 0
     */
    private static native int _notifyProgress(
        int aHandle, String aGlobalComponentId, int aOperation,
        int aSubOperation, int aCurrent, int aTotal);

    /**
     * Initializes SifNotifier. This method must be called before any
     * other methods are called.
     *
     * @return Symbian error code (negative number) if operation fails,
     * otherwise handle to the native side object
     */
    private static native int _init();

    /**
     * Cleans up SifNotifier. This method must be called after SifNotifier
     * is no longer used.
     *
     * @param aHandle
     * @return Symbian error code (negative number) if operation fails,
     * otherwise 0
     */
    private static native int _destroy(int aHandle);

    /**
     * Initializes SifNotifier indicator.
     *
     * @return Symbian error code (negative number) if operation fails,
     * otherwise handle to the native side object
     */
    private static native int _initIndicator();

    /**
     * Updates SifNotifier indicator.
     *
     * @param aHandle handle to indicator object
     * @param aName application name
     * @param aPhase operation phase
     * @param aProgress progress in percentage
     * @return Symbian error code (negative number) if operation fails,
     * otherwise handle to the native side object
     */
    private static native int _updateIndicator(
        int aHandle, String aName, int aPhase, int aProgress);

    /**
     * Destroys SifNotifier indicator.
     *
     * @param aHandle handle to indicator object
     * @param aState indicator state
     * @return Symbian error code (negative number) if operation fails,
     * otherwise 0
     */
    private static native int _destroyIndicator(int aHandle, int aState);

}
