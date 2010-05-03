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

import java.lang.ClassNotFoundException;
import javax.microedition.io.ConnectionNotFoundException;
import java.io.IOException;
import java.lang.String;
import com.nokia.mj.impl.gcf.PushSecurityUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.storage.StorageSession;
import java.util.Date;
import java.util.Vector;
import com.nokia.mj.impl.rt.support.Jvm;

public final class PushRegistryImpl
{

    static private PushMidletData[] mMidletsInSuiteInfo = null;

    static
    {
        Jvm.loadSystemLibrary("javapushregistry");
    }

    /**
     *
     */
    private PushRegistryImpl() {}

    /**
    *
    */
    public static synchronized String getFilter(String aURI)
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.getFilter()");

        if (null == aURI)
            return null;
        aURI = aURI.trim();
        if (0 == aURI.compareTo(""))
            return null;

        Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();
        return getFilter0(aURI,suiteUid.toString());
    }

    /**
     *
     */
    public static synchronized String getMIDlet(String aURI)
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.getMIDlet()");

        if (null == aURI)
            return null;
        aURI = aURI.trim();
        if (0 == aURI.compareTo(""))
            return null;

        Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();
        return getMIDlet0(aURI,suiteUid.toString());

        //Uid appUid = ApplicationInfo.getInstance().getUid();
        //boolean isPushConn = isPushConnection0(aURI,appUid.toString());
        //if(false == isPushConn)
        //  return null;

        //String classNameOfThisMidlet = ApplicationInfo.getInstance().getMainClass();
        //return classNameOfThisMidlet;
    }

    /**
     *
     */
    public static synchronized String[] listConnections(boolean aAvailable)
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.listConnections()");

        Uid appUid = ApplicationInfo.getInstance().getSuiteUid();
        String[] retConns = listConnections0(aAvailable,appUid.toString());
        if (null == retConns)
        {
            return new String[0];
        }
        return retConns;
    }

    /**
     *
     */
    public static synchronized long registerAlarm(String aMIDletClassName,long aTime)
    throws ClassNotFoundException,ConnectionNotFoundException
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.registerAlarm()");

        if (null == aMIDletClassName)
            throw new ClassNotFoundException("'midlet' argument is null");

        aMIDletClassName = aMIDletClassName.trim();

        if (0 == aMIDletClassName.compareTo(""))
            throw new ClassNotFoundException("'midlet' argument is empty");

        //PushMidletData tmpObj = null;
        //try{
        //  tmpObj = isValidClassName(aMIDletClassName);
        //}
        //catch(IOException ex){
        //  //We must return 0 because IOException cannot be thrown from this operation.
        //  Logger.LOG(Logger.EJavaPush,Logger.EError,"ERROR!!! Exception: " + ex);
        //  return 0;
        //}

        //Note: Validation of the time is done in the native side.
        if (0 >= aTime)
            throw new IllegalArgumentException("alarm time must be greater than zero");

        PushSecurityUtils.ensurePermission("dynamicregistration",null);

        Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();
        String classNameOfThisMidlet = ApplicationInfo.getInstance().getMainClass();
        boolean isClassNameOfThisMidlet = false;
        if (0 == aMIDletClassName.compareTo(classNameOfThisMidlet))
            isClassNameOfThisMidlet = true;

        return registerAlarm0(aTime,suiteUid.toString(),
                              aMIDletClassName,isClassNameOfThisMidlet);
    }

    /**
     * In the java side is checked that aMIDletClassName argument has correct value.
     * PushRegistryPlugin checks whether push conn already exists if it is registered
     * to the other MIDlet.
     * If psuh conn is registered to 'this' MIDlet then "already exists" check is done
     * by PushController.
     */
    public static synchronized void registerConnection(String aConnection,
            String aMIDletClassName,
            String aFilter)
    throws ClassNotFoundException,java.io.IOException
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.registerConnection()");

        if (null == aConnection)
            throw new IllegalArgumentException("'connection' argument is null");
        if (null == aFilter)
            throw new IllegalArgumentException("'filter' argument is null");
        if (null == aMIDletClassName)
            throw new ClassNotFoundException("'midlet' argument is null");

        aConnection = aConnection.trim();
        aFilter = aFilter.trim();
        aMIDletClassName = aMIDletClassName.trim();

        if (0 == aConnection.compareTo(""))
            throw new IllegalArgumentException("'connection' argument is empty");
        if (0 == aFilter.compareTo(""))
            throw new IllegalArgumentException("'filter' argument is empty");
        if (0 == aMIDletClassName.compareTo(""))
            throw new ClassNotFoundException("'midlet' argument is empty");

        String nameOfThisMidlet = ApplicationInfo.getInstance().getName();
        Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();
        PushValidatorLoader.getInstance().validatePushArguments(aConnection,aFilter,suiteUid,
                nameOfThisMidlet,false);

        PushMidletData tmpObj = isValidClassName(aMIDletClassName);

        PushSecurityUtils.ensurePermission("dynamicregistration",null);

        registerConnection0(aConnection,aMIDletClassName,aFilter,tmpObj.mUid.toString(),
                            tmpObj.mIsClassNameOfThisMidlet);
    }

    /**
     *
     */
    public static synchronized boolean unregisterConnection(String aConnection)
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.unregisterConnection()");

        if (null == aConnection)
            return false;
        aConnection = aConnection.trim();
        if (0 == aConnection.compareTo(""))
            return false;

        UidAndIsClassNameOfThisMidletInfo tmpObj = null;
        try
        {
            tmpObj = isValidConnToUnReg(aConnection);
        }
        catch (SecurityException ex)
        {
            throw ex;
        }
        catch (Throwable ex)
        {
            Logger.ELOG(Logger.EJavaPush,
                        "Error: unregisterConnection(): " + ex.toString());
            return false;
        }

        return unregisterConnection0(aConnection,tmpObj.mUid.toString(),
                                     tmpObj.mIsClassNameOfThisMidlet);
    }

    /**
     * @return PushMidletData this object is never null.
     *
     * @throws  ClassNotFoundException 'aMIDletClassName' does not match to any MIDlet's
     *                                  class names in the MIdlet Suite.
     *          IOException             Retrieving MIDlets' class names fails
     */
    static PushMidletData isValidClassName(String aMIDletClassName)
    throws IOException,ClassNotFoundException
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.isValidClassName()");

        String classNameOfThisMidlet = ApplicationInfo.getInstance().getMainClass();
        boolean isClassNameOfThisMidlet = true;
        Uid uidOfMidlet = null;
        String midletName = null;
        if (0 == aMIDletClassName.compareTo(classNameOfThisMidlet))
        {
            uidOfMidlet = ApplicationInfo.getInstance().getUid();
            midletName = ApplicationInfo.getInstance().getName();
            PushMidletData pushMidletDataObj =
                new PushMidletData(uidOfMidlet,midletName,aMIDletClassName,true);
            return pushMidletDataObj;
        }

        //MIDlet name didn't match to the name of 'this' MIDlet so next we have to retrieve
        //names of other MIDlets in the suite.
        isClassNameOfThisMidlet = false;
        //Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();
        PushMidletData[] midletData = getMidletSuiteInformation(null);
        //  PushDbHandler.getClassNamesAndUidsOfMidletsInSuite(suiteUid,null);
        if (null == midletData)
        {
            throw new IOException("Retrieving MIDlet data failed");
        }
        boolean classNameMatched = false;

        for (int i = 0; i < midletData.length; i++)
        {
            if (0 == aMIDletClassName.compareTo(midletData[i].mMainClass))
            {
                classNameMatched = true;
                uidOfMidlet = midletData[i].mUid;
                midletName = midletData[i].mMidletName;
                break;
            }
        }//end for
        if (false == classNameMatched)
        {
            throw new ClassNotFoundException("'midlet' argument, " + aMIDletClassName +
                                             ", is not a class name of any MIDlet in this suite");
        }
        return new PushMidletData(uidOfMidlet,midletName,aMIDletClassName,false);
    }//end isValidClassName()

    /**
     * @throws ClassNotFoundException in the following situations:
     *          - Push connection has not been registered
     *          - Type of the push connection is 'static'
     *          IOException if retrieving MIDlet data failed.
     *          SecurityException if connection has been registered by other MIDlet suite.
     */
    static UidAndIsClassNameOfThisMidletInfo isValidConnToUnReg(String aConnection)
    throws IOException,ClassNotFoundException
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.isValidConnToUnReg()");
        StorageSession sessionObj = null;

        try
        {
            sessionObj = StorageFactory.createSession();
            sessionObj.open();
            //Next is retrieved push registration data from the storage.
            PushRegData pushInfoObj =
                PushRuntimeDbHandler.getPushRegByConnection(aConnection,sessionObj);
            //Push reg was not in the storage.
            if (null == pushInfoObj)
            {
                throw new ClassNotFoundException("Push connection has not been registered: " + aConnection);
            }

            //Next is checked whether push registration belongs to 'this' MIDlet.
            Uid uidOfPushRegMidlet = pushInfoObj.getUid();
            if ((true == ApplicationInfo.getInstance().getUid().equals(uidOfPushRegMidlet))
                    && (PushRegData.REG_DYNAMIC == pushInfoObj.getRegType()))
            {
                return new UidAndIsClassNameOfThisMidletInfo(uidOfPushRegMidlet,true);
            }

            //First is retrieved uids and names of MIDlets in the suite.
            //Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();
            PushMidletData[] midletData = getMidletSuiteInformation(sessionObj);
            //  PushDbHandler.getClassNamesAndUidsOfMidletsInSuite(suiteUid,sessionObj);
            if (null == midletData)
            {
                //This should never happened.
                throw new IOException("Retrieving MIDlet data failed");
            }

            //Next is checked whether push registration belongs to other MIDlet
            //in the same MIDlet suite.
            boolean midletMatched = false;
            for (int i = 0; i < midletData.length; i++)
            {
                if (true == uidOfPushRegMidlet.equals(midletData[i].mUid))
                {
                    midletMatched = true;
                    break;
                }
            }//end for
            if (false == midletMatched)
            {
                //Push reg is registred by other MIDlet suite. SecurityException must be thrown
                //in this situation.
                throw new SecurityException
                ("Connection '" + aConnection + "' is already registered by another MIDlet suite");
            }

            //This check must be after "Connection has been registered by other MIDlet suite" check.
            //Otherwise midp tck fails.
            //Next is checked that type of the unregistered push conn is 'dynamic'.
            if (PushRegData.REG_STATIC == pushInfoObj.getRegType())
            {
                throw new ClassNotFoundException("Type of the push connection is 'static'");
            }

            return new UidAndIsClassNameOfThisMidletInfo(uidOfPushRegMidlet,false);
        }//end try
        finally
        {
            if (null != sessionObj)
            {
                sessionObj.close();
                sessionObj.destroySession();
            }
        }//end finally
    }//end isValidConnToUnReg()

    /**
     * @param aSession Value of this argument can be also null.
     */
    static private PushMidletData[] getMidletSuiteInformation(StorageSession aSession)
    throws IOException
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"PushRegistryImpl.getMidletSuiteInformation()");

        if (null == mMidletsInSuiteInfo)
        {
            Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();
            mMidletsInSuiteInfo = PushDbHandler.getClassNamesAndUidsOfMidletsInSuite(suiteUid,aSession);
        }
        return mMidletsInSuiteInfo;
    }

    private static native String getFilter0(String aURI,String aSuiteUid);
    private static native String getMIDlet0(String aURI,String aSuiteUid);
    private static native boolean isPushConnection0(String aURI,String aAppUid);
    private static native String[] listConnections0(boolean aAvailable,String aAppUid);
    private static native long registerAlarm0(long aTimeInMillSecs,String aSuiteUid,
            String aaMIDletClassName,
            boolean aIsUidOfThisMidlet);
    private static native void registerConnection0(String aConnection,String aMIDletClassName,
            String aFilter,String aAppUid,
            boolean aIsUidOfThisMidlet);
    private static native boolean unregisterConnection0(String aConnection,String aAppUid,
            boolean aIsUidOfThisMidlet);
}

