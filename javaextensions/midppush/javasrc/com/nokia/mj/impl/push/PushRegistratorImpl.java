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


package com.nokia.mj.impl.push;

import com.nokia.mj.impl.gcf.PushSecurityUtils;
import com.nokia.mj.impl.installer.pushregistrator.PushRegistrator;
import com.nokia.mj.impl.installer.pushregistrator.PushInfo;
import com.nokia.mj.impl.installer.pushregistrator.PushRegistrationException;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.installer.pushregistrator.PushApplicationInfo;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.storage.StorageException;
import com.nokia.mj.impl.utils.Uid;
import javax.microedition.io.ConnectionNotFoundException;
import java.security.AccessControlException;
import java.lang.IllegalArgumentException;
import java.util.Vector;
import java.util.Enumeration;
import java.util.Hashtable;

import com.nokia.mj.impl.utils.Logger;

public final class PushRegistratorImpl implements PushRegistrator
{

    private static final int DYN_PUSH_REG     = 0;
    private static final int STATIC_PUSH_REG  = 1;

    public PushRegistratorImpl()
    {
    }

    /**
     *
     */
    private boolean isUniqueConstraintErr(String aErrTxt)
    {
        if (-1 != aErrTxt.indexOf("-329"))
            return true;
        if (-1 != aErrTxt.indexOf("-95"))
            return true;
        return false;
    }

    /**
     *
     */
    public void registerPush(StorageSession aSession, Uid aSuiteUid, PushInfo[] aPushInfos)
    {
        Logger.ILOG(Logger.EJavaInstaller, "PushRegistratorImpl.registerPush() #1");

        if ((null == aSession) || (null == aSuiteUid))
        {
            throw new PushRegistrationException(InstallerErrorMessage.INST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "null passed as argument to registerPush()" },
                                                OtaStatusCode.INTERNAL_ERROR,null);
        }

        if (null == aPushInfos)
            return;
        if (0 == aPushInfos.length)
            return;

        int i = 0;
        try
        {
            for (; i < aPushInfos.length; i++)
            {
                PushSecurityUtils.ensurePermission(aPushInfos[i].getUid(),"staticregistration");
            }//end for

            //Validating push arguments. Note: Installer has already validated
            //class name of the application.
            for (i = 0; i < aPushInfos.length; ++i)
            {
                //Dynamic push regs are not needed to validate because those has been
                //already validated.
                Logger.LOG(Logger.EJavaInstaller, Logger.EInfo,
                           "Calling PushValidatorLoader.validatePushArguments() operation");
                PushValidatorLoader.getInstance().validatePushArguments
                (aPushInfos[i].getConnectionUrl(),aPushInfos[i].getFilter(),aSuiteUid,
                 aPushInfos[i].getName(),true);
            }//end for

            //Next push registrations are stored into the db.
            PushDbHandler.storePushRegs(aSession,aPushInfos);
        }
        catch (ConnectionNotFoundException ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"registerPush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.INST_PUSH_REG_ERR,null,
                                                InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                new String[] {aPushInfos[i].getConnectionUrl()},
                                                OtaStatusCode.PUSH_REGISTRATION_FAILURE);
        }
        catch (IllegalArgumentException ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"registerPush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.INST_PUSH_REG_ERR,null,
                                                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                new String[] {aPushInfos[i].getPushAttribute()},
                                                OtaStatusCode.PUSH_REGISTRATION_FAILURE);
        }
        catch (StorageException ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,
                        "registerPush(): Orig. exception: " + ex.toString());
            int errCode = InstallerErrorMessage.INST_UNEXPECTED_ERR;
            String[] errTxt = new String[] {"Storing push registrations failed to the db error"};
            if (true == isUniqueConstraintErr(ex.toString()))
            {
                errCode = InstallerErrorMessage.INST_PUSH_REG_ERR;
                errTxt = new String[] {"Storing push registrations failed to unique constraint error"};
            }
            throw new PushRegistrationException(errCode,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                errTxt,OtaStatusCode.PUSH_REGISTRATION_FAILURE);
        }
        catch (PushRegistrationException ex)
        {
            throw ex;
        }
        catch (AccessControlException ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"registerPush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.INST_PUSH_REG_ERR,null,
                                                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                new String[] {aPushInfos[i].getPushAttribute()},
                                                OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE,null);
        }
        catch (Throwable ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"registerPush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.INST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "Registering push attributes failed to unexpected exception" },
                                                OtaStatusCode.INTERNAL_ERROR,ex);
        }
    }//end registerPush

    /**
     *
     */
    public void unregisterPush(StorageSession aSession, Uid aSuiteUid, Uid[] aUids)
    {
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushRegistratorImpl.unregisterPush()");

        if (null == aSession)
        {
            throw new PushRegistrationException(InstallerErrorMessage.UNINST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "unregisterPush() 'aSession' argument is null" },
                                                OtaStatusCode.INTERNAL_ERROR,null);
        }
        if (aUids == null)
            return;

        try
        {
            for (int i = 0; i < aUids.length; i++)
            {
                PushDbHandler.deletePushRegs(aSession,aUids[i]);
                PushDbHandler.deleteAlarm(aSession,aUids[i]);
            }//end for
        }
        catch (StorageException ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"unregisterPush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.UNINST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "Deleting push registrations failed to the db error" },
                                                OtaStatusCode.INTERNAL_ERROR,ex);
        }
        catch (Throwable ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"unregisterPush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.UNINST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "Unregistering push attributes failed to unexpected exception" },
                                                OtaStatusCode.INTERNAL_ERROR,ex);
        }
    }

    /**
     *
     */
    public void upgradePush(StorageSession aSession,
                            Uid aSuiteUid,
                            PushApplicationInfo[] aOldPushAppInfos,
                            PushApplicationInfo[] aNewPushApplicationInfos,
                            PushInfo[] aNewPushInfos)
    {
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushRegistratorImpl.upgradePush()");

        //This should never happen.
        if ((null == aOldPushAppInfos) || (null == aNewPushApplicationInfos) || (null == aSession))
        {
            throw new PushRegistrationException(InstallerErrorMessage.INST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "upgradePush() One of the mandatory argument is null" },
                                                OtaStatusCode.INTERNAL_ERROR,null);
        }

        try
        {
            //First is deleted all push regs and alarms.
            for (int i = 0; i < aOldPushAppInfos.length; i++)
            {
                PushDbHandler.deletePushRegs(aSession,aOldPushAppInfos[i].getUid());
                PushDbHandler.deleteAlarm(aSession,aOldPushAppInfos[i].getUid());
            }
        }
        catch (StorageException ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"upgradePush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.INST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "Upgrading push registrations failed to the db error" },
                                                OtaStatusCode.INTERNAL_ERROR,ex);
        }

        registerPush(aSession,aSuiteUid,aNewPushInfos);
    }

    /**
     *
     */
    public PushInfo[] getPushInfos(Uid aUid)
    {
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushRegistratorImpl.getPushInfos()");

        StorageSession sessionObj = null;
        try
        {
            sessionObj = StorageFactory.createSession();
            sessionObj.open();

            PushInfo[] retTable = PushDbHandler.getPushRegs(sessionObj,aUid);
            return retTable;
        }
        catch (StorageException ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"upgradePush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.INST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "Getting push registrations failed to the db error" },
                                                OtaStatusCode.INTERNAL_ERROR,ex);
        }
        catch (Throwable ex)
        {
            Logger.ELOG(Logger.EJavaInstaller,"upgradePush(): Orig. exception: " + ex);
            throw new PushRegistrationException(InstallerErrorMessage.INST_UNEXPECTED_ERR,null,
                                                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                new String[] { "Getting push attributes failed to unexpected exception" },
                                                OtaStatusCode.INTERNAL_ERROR,ex);
        }
        finally
        {
            if (null != sessionObj)
            {
                sessionObj.close();
                sessionObj.destroySession();
            }
        }
    }//end getPushInfos

}//end PushRegistratorImpl

