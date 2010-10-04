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
package com.nokia.mj.impl.mms; 

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.push.PushUtil;
import com.nokia.mj.impl.push.PushUtil.PushRegInfo;
import com.nokia.mj.impl.installer.jsrpluginnotifier.InstallerExtension;
import com.nokia.mj.impl.installer.jsrpluginnotifier.InstallerExtensionInfo;

public final class MMSInstallerPlugin implements InstallerExtension
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javawmamms");
        }
        catch (Throwable Th)
        {
            Logger.ELOG(Logger.EWMA, "Loading WMA protocol failed", Th);
        }
    }
  /**
     * This method is called when installation has been done
     * nearly completely (only committing changes is left).
     * @param aInstallerExtensionInfo installation info
     * @return true if installation can be finished
     *              false if installation must be cancelled
     */
    public boolean install(InstallerExtensionInfo aInstallerExtensionInfo){
        return true;
    }

    /**
     * This method is called when uninstallation is starting
     * (all information is still available in databases).
     * @param aInstallerExtensionInfo uninstallation info
     * @return true if uninstallation can be finished
     *              false if uninstallation must be cancelled
     */
    public boolean uninstall(InstallerExtensionInfo aInstallerExtensionInfo){
        PushRegInfo[] pushRegArray = PushUtil.getPushRegs(
         aInstallerExtensionInfo.iStorageSession ,aInstallerExtensionInfo.iUid);
         for(int i = 0;i < pushRegArray.length;i++)
         {
             String url =  pushRegArray[i].getUrl();
             if (url.startsWith("mms://:"))
             {
                 int err = _unregisterFromMMSEngine(url.substring(7));
                 if (0 != err)
                    return false;
             }
         }
         return true;
    }

    /**
     * Called when installation has failed and will be rolled back.
     * Called after install().
     *
     * @param aInstallerExtensionInfo installation info
     */
    public void rollbackInstall(InstallerExtensionInfo aInstallerExtensionInfo){
    }

    /**
     * Called when uninstallation has failed and will be rolled back.
     * Called after uninstall().
     *
     * @param aInstallerExtensionInfo installation info
     */
    public void rollbackUninstall(InstallerExtensionInfo aInstallerExtensionInfo){
    }
    
    private native int _unregisterFromMMSEngine(String aUri);
    
}