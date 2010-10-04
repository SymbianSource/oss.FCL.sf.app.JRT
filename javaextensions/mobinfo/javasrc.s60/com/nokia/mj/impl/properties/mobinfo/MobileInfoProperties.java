/*
* Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.properties.mobinfo;

import java.util.Hashtable;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.Uid;

/**
 * Provides system properties related to Mobile Info
 */
public class MobileInfoProperties implements SystemPropertyProvider
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javamobinfo");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, "MOBINFO: Unable to load javamobinfo");
        }
    }

    // Strings that are exposed as system properties.
    private static final String NETWORK_ACCESS = "com.nokia.network.access";

    private static final String BATTERY_LEVEL = "com.nokia.mid.batterylevel";

    static final String COUNTRY_CODE = "com.nokia.mid.countrycode";

    private static final String IMEI = "com.nokia.mid.imei";

    static final String IMSI = "com.nokia.mid.imsi";

    private static final String NETWORK_AVAILABILITY = "com.nokia.mid.networkavailability";

    static final String NETWORK_ID = "com.nokia.mid.networkid";

    private static final String NETWORK_SIGNAL = "com.nokia.mid.networksignal";

    private static final String EMAIL_RECIEVE_SETTINGS = "com.nokia.mid.settings.email-receive-protocol";

    private static final String EMAIL_SEND_SETTINGS = "com.nokia.mid.settings.email-send-protocol";

    static final String CELLID = "com.nokia.mid.cellid";

    static final String MSISDN = "com.nokia.mid.msisdn";

    private static final String DATEFORMAT = "com.nokia.mid.dateformat";

    private static final String TIMEFORMAT = "com.nokia.mid.timeformat";

    private static final String NETWORKSTATUS = "com.nokia.mid.networkstatus";

    static final String SERVICE_PROVIDER_NAME = "com.nokia.mid.spn";

    static final String OPERATOR_NAME = "com.nokia.mid.ons";

    private static final int APP_DEFAULT_APN_NOT_SPECIFIED = -1;

    // Constants associated with the strings
    private static final int NETWORK_ACCESS_V = 1;

    private static final int BATTERY_LEVEL_V = 2;

    private static final int COUNTRY_CODE_V = 3;

    private static final int IMEI_V = 4;

    private static final int IMSI_V = 5;

    private static final int NETWORK_AVAILABILITY_V = 6;

    private static final int NETWORK_ID_V = 7;

    private static final int NETWORK_SIGNAL_V = 8;

    private static final int EMAIL_RECIEVE_SETTINGS_V = 9;

    private static final int EMAIL_SEND_SETTINGS_V = 10;

    private static final int CELLID_V = 11;

    private static final int MSISDN_V = 12;

    private static final int DATEFORMAT_V = 13;

    private static final int TIMEFORMAT_V = 14;

    private static final int NETWORKSTATUS_V = 15;

    private static final int SERVICE_PROVIDER_NAME_V = 16;

    private static final int OPERATOR_NAME_V = 17;

    private static Hashtable iPropertyKeys;

    static
    {
        iPropertyKeys = new Hashtable();
        iPropertyKeys.put(NETWORK_ACCESS,         new Integer(NETWORK_ACCESS_V));
        iPropertyKeys.put(BATTERY_LEVEL,          new Integer(BATTERY_LEVEL_V));
        iPropertyKeys.put(COUNTRY_CODE,           new Integer(COUNTRY_CODE_V));
        iPropertyKeys.put(IMEI,                   new Integer(IMEI_V));
        iPropertyKeys.put(IMSI,                   new Integer(IMSI_V));
        iPropertyKeys.put(NETWORK_AVAILABILITY,   new Integer(NETWORK_AVAILABILITY_V));
        iPropertyKeys.put(NETWORK_ID,             new Integer(NETWORK_ID_V));
        iPropertyKeys.put(NETWORK_SIGNAL,         new Integer(NETWORK_SIGNAL_V));
        iPropertyKeys.put(EMAIL_RECIEVE_SETTINGS, new Integer(EMAIL_RECIEVE_SETTINGS_V));
        iPropertyKeys.put(EMAIL_SEND_SETTINGS,    new Integer(EMAIL_SEND_SETTINGS_V));
        iPropertyKeys.put(CELLID,                 new Integer(CELLID_V));
        iPropertyKeys.put(MSISDN,                 new Integer(MSISDN_V));
        iPropertyKeys.put(DATEFORMAT,             new Integer(DATEFORMAT_V));
        iPropertyKeys.put(TIMEFORMAT,             new Integer(TIMEFORMAT_V));
        iPropertyKeys.put(NETWORKSTATUS,          new Integer(NETWORKSTATUS_V));
        iPropertyKeys.put(SERVICE_PROVIDER_NAME,  new Integer(SERVICE_PROVIDER_NAME_V));
        iPropertyKeys.put(OPERATOR_NAME,          new Integer(OPERATOR_NAME_V));
    }

    public String getProperty(String aKey)
    {
        // enforce security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        appUtils.checkPermission(new MobileInfoPermission(aKey, "read"));

        int value = ((Integer)iPropertyKeys.get(aKey)).intValue();
        Uid appSuiteUid;
        int apId = APP_DEFAULT_APN_NOT_SPECIFIED;        // access point id
        int apType = APP_DEFAULT_APN_NOT_SPECIFIED;      // MIDlet access point type, can be SNAP or IAP

        ConnectionManager CmInstance = null;
        String result = null;
        // Only network access seems to have another way to handle it.
        if (aKey.equals(EMAIL_RECIEVE_SETTINGS)
                || aKey.equals(EMAIL_SEND_SETTINGS))
        {
            return null;
        }
        if (aKey.equals(NETWORK_ACCESS))
        {
            // get application suite uid
            try
            {
                appSuiteUid = ApplicationInfo.getInstance().getSuiteUid();
                CmInstance = ConnectionManager.getInstance();
                if (CmInstance != null)
                {
                    AccessPoint apn = CmInstance.getApplicationDefault(appSuiteUid);
                    if (apn != null)
                    {
                        apId = apn.getNapId();
                        apType = apn.getType();


                        if ((apType!=AccessPoint.NAP_SNAP) && (apType!=AccessPoint.NAP_IAP))
                        {
                            apType = APP_DEFAULT_APN_NOT_SPECIFIED;
                            apId = APP_DEFAULT_APN_NOT_SPECIFIED;
                        }

                    }

                }
                result = _getPropertyApn(NETWORK_ACCESS_V, apId,apType);
            }
            catch (Exception e)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,"MobilInfoProperties :: getSuiteUid exception ");
                result = _getPropertyApn(NETWORK_ACCESS_V, apId,apType);
            }
            return result;
        }
        else
        {
            return _getProperty(value);
        }
    }

    public boolean isStatic(String aKey)
    {
        if (aKey.equals(IMEI) || aKey.equals(IMSI))
        {
            return true;
        }
        return false;
    }

    private native String _getProperty(int value);
    private static native String _getPropertyApn(int property, int aAppDefaultApn, int apType);

}