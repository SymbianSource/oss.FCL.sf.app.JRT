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


package com.nokia.mj.impl.connectionmanager;

import com.nokia.mj.impl.connectionmanager.*;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.*;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.storage.*;

/*
 * JavaImplementation of ConnectionManager class
 */

public class ConnectionManager
{

    // Constants for URL paramater names
    private static String NOKIANETID = ";nokia_netid=";

    private static String NOKIAAPNID = ";nokia_apnid=";

    // Constants for application setting types
    private static char NONE = 'N'; // Supported by app mgr

    private static char ASKALWAYS = 'A'; // this is needed since device
    // default supports it

    private static char DEFAULTCONN = 'F'; // Supported by app mgr

    private static char ASKONCE = 'O'; // supported by App mgr

    private static char SNAPID = 'D';

    private static char IAPID = 'I';

    private int iNativeHandle;

    private static ConnectionManager iCmInstance = null;

    public AccessPoint iSessionNetworkAccessPoint = null;

    static
    {
        try
        {
            if (Logger.Activated[Logger.ESOCKET])
            {
                Logger.ILOG(Logger.ESOCKET,"Loading connection manager...");
            }
            Jvm.loadSystemLibrary("javaconnectionmanager");
            if (Logger.Activated[Logger.ESOCKET])
            {
                Logger.ILOG(Logger.ESOCKET, "Loading connection manager ok");
            }
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
            // Hmm... Should probably kill the application as cannot use
            // the native methods anyway
        }
    }

    public ConnectionManager()
    {
    }

    public synchronized static ConnectionManager getInstance()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+ConnectionManager getInstance");
        if (iCmInstance == null)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "CMImpl instance is null so creating it");
            iCmInstance = new ConnectionManager();
            iCmInstance.iSessionNetworkAccessPoint = new AccessPoint(
                AccessPoint.NAP_INITIAL, 0);
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "-ConnectionManager getInstance");

        return iCmInstance;
    }

    private void log(String str)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "ConnectionManager- " + str);
    }

    private void logReturn(AccessPoint ap)
    {
        Logger.ILOG(Logger.ESOCKET,
                    "ConnectionManager- ap.type = " + ap.getType() + "ap.id = "
                    + ap.getNapId());
    }

    /**
     * The network access point selection is done as follows:
     *
     * 1. If the SNAP ID parameter (nokia_netid) or the IAP ID parameter
     * (nokia_apnid) is specified in the URI, use it. No caching. 2. If the
     * cache contains an earlier selected IAP/SNAP, use it. 3. If the cache is
     * invalid (connection manager has been reseted), go to the step 7 4. If the
     * application suite has a default IAP/SNAP in the settings, update the
     * cache and use the IAP/SNAP. 5. If the application suite has the setting
     * "Default" and the device has a default connection, update the cache, and
     * use it. 6. If the application suite has the setting "Ask Once", go to
     * step 7 7. Prompt the user for a SNAP, update the cache and use the
     * selected SNAP.
     *
     * If user cancels a network selection prompt, no changes are made to cache.
     *
     * Reseting the Connection Manager invalidates the cache and causes the next
     * query to execute the step 3.
     *
     * Returns a AccessPoint instance (of type NAP_SNAP or NAP_IAP) or null if
     * cannot find any accesspoint or the user cancelled the prompt.
     */
    public synchronized AccessPoint getNetworkAccessPoint(Uid aAppSuiteUid,
            String aUri) throws Exception
    {
        log("CMImpl +getNetworkAccessPoint");
        int id;
        boolean deviceDefSet;
        String apn = null;
        AccessPoint temp1 = null;
        AccessPoint temp2 = null;

        /* Step 1: Check if a SNAP or an IAP is set in the URL */
        log("getNetworkAccessPoint trying NOKIANETID");
        AccessPoint accesspoint = getNetworkAccessPointFromUrl(aUri,
                                  NOKIANETID, AccessPoint.NAP_SNAP);
        if (accesspoint == null)
        {
            log("getNetworkAccessPoint trying NOKIAAPNID");
            accesspoint = getNetworkAccessPointFromUrl(aUri, NOKIAAPNID,
                          AccessPoint.NAP_IAP);
        }
        if (accesspoint != null)
        {
            // Found access point from the URL, not cached
            logReturn(accesspoint);
            return accesspoint;
        }

        // Step 2: Check if the session cache has IAP or SNAP already
        log("getNetworkAccessPoint trying cache");
        logReturn(iSessionNetworkAccessPoint);

        int cacheType = iSessionNetworkAccessPoint.getType();
        if (cacheType == AccessPoint.NAP_SNAP
                || cacheType == AccessPoint.NAP_IAP)
        {
            // End of step 2: Found access point from the cache
            logReturn(iSessionNetworkAccessPoint);
            return iSessionNetworkAccessPoint;
        }

        // Handle application settings (steps 4, 5, 6) except after a reset
        // (step 3)
        // In case of cache reset prompt the user
        AccessPoint appDefault;
        if (iSessionNetworkAccessPoint.getType() != AccessPoint.NAP_ERR)
        {
            // Start steps 4, 5 and 6
            log("getNetworkAccessPoint trying application default");
            appDefault = getApplicationDefault(aAppSuiteUid);
            log(" appDefault returned ");
            logReturn(appDefault);
            int type = appDefault.getType();
            // special case where the app mgr store "I0" for case of NONE IAP
            if (type == AccessPoint.NAP_SNAP || type == AccessPoint.NAP_IAP)
            {
                if (appDefault.getNapId() == 0)
                {
                    type = AccessPoint.NAP_NONE;
                    appDefault.setType(AccessPoint.NAP_NONE);
                }
            }
            if (type == AccessPoint.NAP_SNAP || type == AccessPoint.NAP_IAP)
            {
                // End of step 4: Found proper access point, cache it
                logReturn(appDefault);
                iSessionNetworkAccessPoint = appDefault;
                return appDefault;
            }
            // If app Default is of types DEFAULT/NONE then fetch the device
            // default and cache it
            // or else prompt the user

            else if (type == AccessPoint.NAP_DEFAULT
                     || type == AccessPoint.NAP_NONE)
            {
                // Trying next the device's default
                log("getNetworkAccessPoint looking for device's default");
                // accesspoint = _getDeviceDefaultAccessPoint();
                apn = _getDeviceDefaultAccessPoint();
                if (apn == null)
                {
                    return null;
                }
                // String apnInfo = apn[0].substring(0,apn[0].length());
                accesspoint = parseNetworkAccess(apn);
                // if device default is of type ASKALWAYS then prompt the user
                if (accesspoint != null
                        && accesspoint.getType() != AccessPoint.NAP_ASKALWAYS)
                {
                    // End of step 5: Found device's default, cache it
                    logReturn(accesspoint);
                    iSessionNetworkAccessPoint = accesspoint;
                    return accesspoint;
                }
                else
                {
                    log("device default not defined");
                }
                // If there was no default access point for the device,
                // continue in step 7
            }

            // Also the cases NAP_ERR and NAP_ONCE are handled in the step 7
        }

        // Step 7: Prompt the user for a SNAP
        log("getNetworkAccessPoint calling _askNetworkAccessPoint");
        // accesspoint = _askNetworkAccessPoint();
        apn = _askNetworkAccessPoint();
        if (apn == null)
        {
            return null;
        }
        // String apnInfo = apn[0].substring(0,apn[0].length());
        accesspoint = parseNetworkAccess(apn);
        if (accesspoint == null)
        {
            // User cancelled the prompt
            logReturn(null);
            return null;
        }

        // End of step 7: Update the cache (clear possible invalid state also)
        iSessionNetworkAccessPoint = accesspoint;
        logReturn(accesspoint);
        return accesspoint;
    }

    /**
     * Returns a AccessPoint instance in one of the following states: NAP_NONE -
     * Couldn't find IAP/SNAP or other settings, errors are logged NAP_ONCE -
     * "Ask Once" setting specified NAP_DEFAULT - "Default" setting specified
     * NAP_IAP - IAP found NAP_SNAP - SNAP found
     */
    public AccessPoint getApplicationDefault(Uid aAppSuiteUid) throws Exception
    {
        String apn = null;
        String iap = null;
        String snap = null;

        log("+getApplicationDefault()");

        // Check java settings for application default IAP/SNAP

        StorageSession session = null;
        StorageEntry readEntry;
        StorageAttribute attr;
        AccessPoint temp1 = null;

        try
        {
            session = StorageFactory.createSession();
            session.open();

            readEntry = session.readEntry(
                            StorageNames.APPLICATION_PACKAGE_TABLE, aAppSuiteUid);

            if (readEntry != null)
            {
                StorageAttribute nameAttr = readEntry
                                            .getAttribute(StorageNames.ACCESS_POINT);
                if (nameAttr != null)
                    apn = nameAttr.getValue();
            }
        }
        catch (StorageException se)
        {
            // Log the error but otherwise ignore it
            Logger.ELOG(Logger.ESOCKET, "storage exception " + se);
            return new AccessPoint();
        }
        // close the storage session
        finally
        {
            if (session != null)
            {
                session.close();
                session.destroySession();
            }
        }

        log("app default is " + apn);

        if (apn == null)
        {
            // Nothing stored for this application or the entry was invalid.
            // Log this but let the show continue
            log("Application default is null");
            return new AccessPoint();
        }

        if (apn.equals("N"))
        {
            return new AccessPoint(AccessPoint.NAP_NONE, 0);
        }
        else if (apn.equals("O"))
        {
            return new AccessPoint(AccessPoint.NAP_ONCE, 0);
        }
        // Default Connection case set in app settings
        else if (apn.equals("F"))
        {
            log("default connection case in app mgr");
            return new AccessPoint(AccessPoint.NAP_DEFAULT, 0);
        }
        // Ask always or Ask when needed case set in app settings
        else if (apn.equals("A"))
        {
            log("Ask always or ask when needed case in app mgr");
            return new AccessPoint(AccessPoint.NAP_ASKALWAYS, 0);
        }

        else if (apn.startsWith("I") || apn.startsWith("D"))
        {
            try
            {
                int id = Integer.parseInt(apn.substring(1));
                int type = apn.startsWith("I") ? AccessPoint.NAP_IAP
                           : AccessPoint.NAP_SNAP;
                return new AccessPoint(type, id);
            }
            catch (NumberFormatException e)
            {
                log("Application default in invalid Format");
                return new AccessPoint();
            }
        }
        else
        {
            // unknown setting, log but let continue
            log(" Unknown app mgr settings");
            return new AccessPoint();
        }
    }

    // This method is used to parse the string passed from native side
    // The device default suppports ASKALWAYS/IAP/SNAP options
    // or else if user is prompted the access point is of 2 types : IAP/SNAP
    private AccessPoint parseNetworkAccess(String aAccess) throws Exception
    {
        int index;
        char ch;
        int id = -1;
        int type;
        String idString = null;
        ch = aAccess.charAt(0);

        if (ch == ASKALWAYS)
        {
            type = AccessPoint.NAP_ASKALWAYS;
        }
        else if (ch == SNAPID)
        {
            type = AccessPoint.NAP_SNAP;
        }
        else if (ch == IAPID)
        {
            type = AccessPoint.NAP_IAP;
        }
        else
        {
            throw new Exception("Illegal network access format");
        }

        if (type == AccessPoint.NAP_ASKALWAYS)
        {
            if (aAccess.length() > 1)
            {
                throw new Exception("Illegal network access format");
            }
        }
        else
        {
            if (aAccess.length() <= 1)
            {
                throw new Exception("Illegal network access format");
            }
            else
            {
                idString = aAccess.substring(1);
                if (idString != null)
                {
                    try
                    {
                        id = Integer.parseInt(idString);
                    }
                    catch (NumberFormatException e)
                    {
                        throw new Exception("Illegal network access format");
                    }
                }
            }
        }

        return new AccessPoint(type, id);
    }

    public AccessPoint getNetworkAccessPointFromUrl(String aUri, String aParam,
            int aType) throws Exception
    {
        int index = aUri.indexOf(aParam);
        if (index != -1)
        {
            String NapString = aUri.substring(index + aParam.length());
            if (NapString != null)
            {
                int id;
                try
                {
                    id = Integer.parseInt(NapString);
                    return new AccessPoint(aType, id);
                }
                catch (NumberFormatException e)
                {
                    log("url does not have valid IAP/SNAP id");
                }
            }
        }
        return null;
    }

    public synchronized void reset()
    {
        log("reset()");
        iSessionNetworkAccessPoint = new AccessPoint(AccessPoint.NAP_ERR, 0);
    }

    private native String _getDeviceDefaultAccessPoint();

    private native String _askNetworkAccessPoint();
}
