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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.pushregistrator.PushApplicationInfo;
import com.nokia.mj.impl.installer.pushregistrator.PushInfo;
import com.nokia.mj.impl.installer.pushregistrator.PushRegistrator;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.rt.installer.ApplicationUtilsImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.util.Hashtable;
import java.util.Vector;

public class RegisterPush extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;

        if (ball.iStorageHandler.getSession() == null)
        {
            ball.log("No StorageSession, skipping push registration");
            return;
        }

        PushRegistrator pushRegistrator = null;
        try
        {
            // Instantiate PushRegistrator implementation.
            Class clazz = Class.forName(PushInfo.PUSH_REGISTRATOR_CLASS);
            pushRegistrator = (PushRegistrator)clazz.newInstance();
        }
        catch (Exception ex)
        {
            InstallerException.internalError(
                "Instantiating PushRegistrator " +
                PushInfo.PUSH_REGISTRATOR_CLASS + " failed", ex);
        }

        // PushRegistrator uses ApplicationUtils.checkPermission(),
        // let's initialize ApplicationUtilsImpl before it is used.
        ApplicationUtilsImpl appUtilsImpl =
            (ApplicationUtilsImpl)ApplicationUtils.getInstance();
        appUtilsImpl.init(ball.iStorageHandler.getSession(), ball.isSilent(),
                          ball.iSuite.getUid(), ball.iSuite.getName(),
                          getAppUidNameTable(ball.iSuite));

        try
        {
            PushInfo[] pushInfos = getPushInfos(ball.iSuite);
            if (ball.iOldSuite != null)
            {
                // Old suite exists, this is an upgrade.
                PushApplicationInfo[] oldPushApplicationInfos =
                    getPushApplicationInfos(ball.iOldSuite);
                PushApplicationInfo[] newPushApplicationInfos =
                    getPushApplicationInfos(ball.iSuite);
                ball.log("Updating info to push registery...");
                pushRegistrator.upgradePush
                (ball.iStorageHandler.getSession(),
                 ball.iSuite.getUid(),
                 oldPushApplicationInfos,
                 newPushApplicationInfos,
                 pushInfos);
            }
            else
            {
                ball.log("Adding info to push registery...");
                pushRegistrator.registerPush
                (ball.iStorageHandler.getSession(), ball.iSuite.getUid(), pushInfos);
            }
        }
        catch (InstallerExceptionBase ieb)
        {
            throw ieb;
        }
        finally
        {
            appUtilsImpl.destroy();
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    private PushApplicationInfo[] getPushApplicationInfos(SuiteInfo aSuite)
    {
        if (aSuite == null)
        {
            return null;
        }
        Vector appInfos = aSuite.getApplications();
        PushApplicationInfo[] result = new PushApplicationInfo[appInfos.size()];
        for (int i = 0; i < appInfos.size(); i++)
        {
            ApplicationInfo appInfo = (ApplicationInfo)appInfos.elementAt(i);
            result[i] = new PushApplicationInfo(
                appInfo.getUid(), appInfo.getMainClass());
        }
        return result;
    }

    private PushInfo[] getPushInfos(SuiteInfo aSuite)
    {
        if (aSuite == null)
        {
            return null;
        }
        Vector pushInfos = new Vector();
        Vector appInfos = aSuite.getApplications();
        for (int i = 1; true; i++)
        {
            String pushAttrName = "MIDlet-Push-" + i;
            String pushAttrValue = aSuite.getAttributeValue(pushAttrName);
            if (pushAttrValue != null)
            {
                PushInfo pushInfo = new PushInfo(null, pushAttrValue);
                ApplicationInfo appInfo = getAppInfoByClass(
                                              appInfos, pushInfo.getClassName());
                if (appInfo == null)
                {
                    Log.logError("No matching MIDlet-n attribute for " +
                                 pushAttrName + ": " + pushAttrValue);
                    throw new InvalidAttributeException
                    (InstallerErrorMessage.INST_PUSH_REG_ERR, null,
                     InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                     new String[] { pushAttrValue },
                     OtaStatusCode.PUSH_REGISTRATION_FAILURE);
                }
                pushInfo.setName(appInfo.getName());
                pushInfo.setUid(appInfo.getUid());
                pushInfos.addElement(pushInfo);
                Log.log("PushInfo[" + i + "]: " + pushInfo);
            }
            else
            {
                break;
            }
        }
        PushInfo[] result = new PushInfo[pushInfos.size()];
        pushInfos.copyInto(result);
        return result;
    }

    private ApplicationInfo getAppInfoByClass(Vector aAppInfos, String aClass)
    {
        ApplicationInfo result = null;
        for (int i = 0; i < aAppInfos.size(); i++)
        {
            ApplicationInfo appInfo = (ApplicationInfo)aAppInfos.elementAt(i);
            if (appInfo.getMainClass().equals(aClass))
            {
                result = appInfo;
                break;
            }
        }
        return result;
    }

    /**
     * Returns hashtable where application uid is the key and
     * application name is the value.
     */
    private Hashtable getAppUidNameTable(SuiteInfo aSuite)
    {
        if (aSuite == null)
        {
            return null;
        }
        Vector appInfos = aSuite.getApplications();
        Hashtable result = new Hashtable();
        for (int i = 0; i < appInfos.size(); i++)
        {
            ApplicationInfo appInfo = (ApplicationInfo)appInfos.elementAt(i);
            result.put(appInfo.getUid(), appInfo.getName());
        }
        return result;
    }
}
