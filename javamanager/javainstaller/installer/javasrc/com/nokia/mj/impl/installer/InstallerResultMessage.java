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

import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.installer.applicationregistrator.SifNotifier;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.Uid;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

/**
 * InstallerResultMessage contains information about install, uninstall
 * and componentinfo operations. It is sent to Comms endpoint specified
 * at JavaInstaller startup.
 */
public class InstallerResultMessage
{
    public static final String NAME_OPERATION = "operation";
    public static final String NAME_RESULT = "result";
    public static final String NAME_ERROR_CATEGORY = "error-category";
    public static final String NAME_ERROR_MSG = "error-message";
    public static final String NAME_ERROR_DETAILS = "error-details";
    public static final String NAME_SUITE_UID = "suite-uid";
    public static final String NAME_MIDLET_UID = "midlet-uid-";
    public static final String NAME_SUITE_CID = "suite-cid";
    public static final String NAME_MIDLET_CID = "midlet-cid-";
    public static final String NAME_SUITE_GID = "suite-gid";
    public static final String NAME_MIDLET_GID = "midlet-gid-";
    public static final String NAME_SUITE_NAME = "suite-name";
    public static final String NAME_MIDLET_NAME=  "midlet-name-";
    public static final String NAME_VENDOR = "vendor";
    public static final String NAME_VERSION = "version";
    public static final String NAME_INSTALL_STATUS = "install-status";
    public static final String NAME_AUTHENTICITY = "authenticity";
    public static final String NAME_COMPONENT_SIZE = "component-size";

    private Hashtable iNamedIntValues = null;
    private Hashtable iNamedStringValues = null;

    private SifNotifier iSifNotifier = null;

    /**
     * Parses comms endpoints from given argument string.
     */
    public static int[] parseEndpoints(String aArg) throws InstallerException
    {
        String[] endpoints = Tokenizer.split(aArg, ",");
        int[] result = new int[endpoints.length];
        for (int i = 0; i < endpoints.length; i++)
        {
            try
            {
                int endpoint = Integer.parseInt(endpoints[i]);
                result[i] = endpoint;
                Log.log("Added CommsResultEndpoint " + endpoint);
            }
            catch (NumberFormatException nfe)
            {
                InstallerException.internalError(
                    "Invalid comms endpoint: " + endpoints[i]);
            }
        }
        return result;
    }

    /**
     * Default constructor.
     */
    public InstallerResultMessage()
    {
        iNamedIntValues = new Hashtable();
        iNamedStringValues = new Hashtable();
    }

    /**
     * Initializes this message from given SuiteInfo.
     */
    public void init(SuiteInfo aSuite)
    {
        addValue(NAME_SUITE_UID, PlatformUid.getIntValue(aSuite.getUid()));
        addValue(NAME_SUITE_GID, aSuite.getGlobalId());
        addValue(NAME_SUITE_NAME, aSuite.getName());
        addValue(NAME_VENDOR, aSuite.getVendor());
        addValue(NAME_VERSION, aSuite.getVersion().toString());
        addValue(NAME_AUTHENTICITY, (aSuite.isTrusted()? 1: 0));
        addValue(NAME_COMPONENT_SIZE, aSuite.getInitialSize());
        if (aSuite.getComponentId() != null)
        {
            addValue(NAME_SUITE_CID, aSuite.getComponentId().getId());
        }
        Vector apps = aSuite.getApplications();
        for (int i = 0; i < apps.size(); i++)
        {
            int index = i+1;
            ApplicationInfo app = (ApplicationInfo)apps.elementAt(i);
            addValue(NAME_MIDLET_UID+index,
                     PlatformUid.getIntValue(app.getUid()));
            addValue(NAME_MIDLET_GID+index, aSuite.getGlobalId(i));
            addValue(NAME_MIDLET_NAME+index, app.getName());
            if (app.getComponentId() != null)
            {
                addValue(NAME_MIDLET_CID+index, app.getComponentId().getId());
            }
        }
    }

    /**
     * Sets SifNotifier instance to be used when sending installation results.
     */
    public void setSifNotifier(SifNotifier aSifNotifier)
    {
        iSifNotifier = aSifNotifier;
    }

    /**
     * Adds result value to this message basing on error codes
     * in given exception.
     */
    public void addResult(Exception aException)
    {
        if (aException == null)
        {
            // No exception, set result to ERR_NONE.
            addValue(NAME_RESULT, Installer.ERR_NONE);
            return;
        }
        int resultCode = Installer.ERR_GENERAL;
        if (aException instanceof ExceptionBase)
        {
            ExceptionBase eb = (ExceptionBase)aException;
            String msg = eb.getShortMessage();
            if (msg != null)
            {
                addValue(NAME_ERROR_MSG, msg);
            }
            msg = eb.getDetailedMessage();
            if (msg != null)
            {
                addValue(NAME_ERROR_DETAILS, msg);
            }
            addErrorCategory(eb);
        }
        if (aException instanceof InstallerException)
        {
            InstallerException ie =
                (InstallerException)aException;
            resultCode = ie.getResultCode();
        }
        addValue(NAME_RESULT, resultCode);
    }

    /**
     * Add a new string value to this message.
     */
    public void addValue(String aName, String aValue)
    {
        iNamedStringValues.put(aName, aValue);
    }

    /**
     * Add a new int value to this message.
     */
    public void addValue(String aName, int aValue)
    {
        iNamedIntValues.put(aName, new Integer(aValue));
    }

    /**
     * Get a string value from this message.
     * @throws IllegalArgumentException if value with given name is not found
     */
    public String getStringValue(String aName)
    {
        Object value = iNamedStringValues.get(aName);
        if (value instanceof String)
        {
            return (String)value;
        }
        throw new IllegalArgumentException(
            "InstallerResultMessage: string value " + aName + " not found");
    }

    /**
     * Get an int value from this message.
     * @throws IllegalArgumentException if value with given name is not found
     */
    public int getIntValue(String aName)
    {
        Object value = iNamedIntValues.get(aName);
        if (value instanceof Integer)
        {
            return ((Integer)value).intValue();
        }
        throw new IllegalArgumentException(
            "InstallerResultMessage: int value " + aName + " not found");
    }

    /**
     * Removes a value from this message.
     */
    public void removeValue(String aName)
    {
        iNamedStringValues.remove(aName);
        iNamedIntValues.remove(aName);
    }

    /**
     * Sends this message to given Comms endpoints.
     *
     * @param aEndpoints Comms endpoints to which the message is sent
     */
    public void send(int[] aEndpoints) throws InstallerException
    {
        if (aEndpoints != null)
        {
            for (int i = 0; i < aEndpoints.length; i++)
            {
                send(aEndpoints[i]);
            }
        }
        if (iSifNotifier != null)
        {
            int errCategory = 0;
            int errCode = 0;
            String errMsg = null;
            String errDetails = null;
            if (getIntValue(NAME_RESULT) != Installer.ERR_NONE)
            {
                errCategory = getIntValue(NAME_ERROR_CATEGORY);
                errCode = Installer.ERR_GENERAL;
                errMsg = getStringValue(NAME_ERROR_MSG);
                errDetails = getStringValue(NAME_ERROR_DETAILS);
            }
            try
            {
                iSifNotifier.notifyEnd(errCategory, errCode, errMsg, errDetails);
            }
            catch (Throwable t)
            {
                Log.logError("InstallerResultMessage: SifNotifier.notifyEnd failed", t);
            }
        }
    }

    /**
     * Sends this message to given Comms endpoint.
     *
     * @param aEndpoint Comms endpoint to which the message is sent
     */
    private void send(int aEndpoint) throws InstallerException
    {
        CommsEndpoint comms = new CommsEndpoint();
        if (comms == null)
        {
            InstallerException.internalError(
                "InstallerResultMessage: creating comms endpoint failed");
        }
        try
        {
            comms.connect(aEndpoint);
            CommsMessage msg = new CommsMessage();
            msg.setMessageId(601);
            // Initialise the message data.
            msg.write(iNamedIntValues.size());
            Enumeration e = iNamedIntValues.keys();
            while (e.hasMoreElements())
            {
                String name = (String)e.nextElement();
                msg.write(name);
                msg.write(((Integer)iNamedIntValues.get(name)).intValue());
            }
            msg.write(iNamedStringValues.size());
            e = iNamedStringValues.keys();
            while (e.hasMoreElements())
            {
                String name = (String)e.nextElement();
                msg.write(name);
                msg.write((String)iNamedStringValues.get(name));
            }
            // Send the message.
            Log.log("Sending InstallerResultMessage to " + aEndpoint);
            CommsMessage installerResultResponse = comms.sendReceive(msg, 5);
            comms.disconnect();
            Log.log("Received InstallerResultResponse from " + aEndpoint);
        }
        catch (Throwable t)
        {
            InstallerException.internalError(
                "InstallerResultMessage: sending message to endpoint " +
                aEndpoint + " failed", t);
        }
        finally
        {
            if (comms != null)
            {
                comms.destroy();
            }
        }
    }

    /**
     * Initializes this message from the given CommsMessage.
     */
    public void parse(CommsMessage aMsg)
    {
        iNamedIntValues.clear();
        iNamedStringValues.clear();
        int count = aMsg.readInt();
        for (int i = 0; i < count; i++)
        {
            iNamedIntValues.put(
                aMsg.readString(), new Integer(aMsg.readInt()));
        }
        count = aMsg.readInt();
        for (int i = 0; i < count; i++)
        {
            iNamedStringValues.put(
                aMsg.readString(), aMsg.readString());
        }
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer("InstallerResultMessage:\n");
        Enumeration e = iNamedStringValues.keys();
        while (e.hasMoreElements())
        {
            String name = (String)e.nextElement();
            buf.append(name).append(": ");
            buf.append(iNamedStringValues.get(name)).append("\n");
        }
        e = iNamedIntValues.keys();
        while (e.hasMoreElements())
        {
            String name = (String)e.nextElement();
            buf.append(name).append(": ");
            buf.append(iNamedIntValues.get(name)).append(" (0x");
            buf.append(Integer.toHexString(
                           ((Integer)iNamedIntValues.get(name)).intValue()));
            buf.append(")\n");
        }
        return buf.toString();
    }

    /**
     * Adds error category to the result message.
     */
    private void addErrorCategory(ExceptionBase aEb)
    {
        switch (aEb.getShortMessageId())
        {
        case InstallerErrorMessage.INST_NO_MEM:
            addValue(NAME_ERROR_CATEGORY, 2); // ELowDiskSpace
            break;
        case InstallerErrorMessage.INST_NO_NET:
            addValue(NAME_ERROR_CATEGORY, 3); // ENetworkUnavailable
            break;
        case InstallerErrorMessage.INST_CORRUPT_PKG:
            addValue(NAME_ERROR_CATEGORY, 5); // ECorruptedPackage
            break;
        case InstallerErrorMessage.INST_COMPAT_ERR:
            addValue(NAME_ERROR_CATEGORY, 6); // EApplicationNotCompatible
            break;
        case InstallerErrorMessage.INST_AUTHORIZATION_ERR:
            // fall through
        case InstallerErrorMessage.INST_AUTHENTICATION_ERR:
            addValue(NAME_ERROR_CATEGORY, 7); // ESecurityError
            break;
        case InstallerErrorMessage.INST_PUSH_REG_ERR:
            // fall through
        case InstallerErrorMessage.INST_UNEXPECTED_ERR:
            // fall through
        case InstallerErrorMessage.UNINST_UNEXPECTED_ERR:
            // fall through
        case InstallerErrorMessage.OTHER_UNEXPECTED_ERR:
            addValue(NAME_ERROR_CATEGORY, 8); // EUnexpectedError
            break;
        case InstallerErrorMessage.INST_CANCEL:
            // fall through
        case InstallerErrorMessage.UNINST_CANCEL:
            addValue(NAME_ERROR_CATEGORY, 9); // EUserCancelled
            break;
        case InstallerErrorMessage.UNINST_NOT_ALLOWED:
            addValue(NAME_ERROR_CATEGORY, 10); // EUninstallationBlocked
            break;
        default:
            addValue(NAME_ERROR_CATEGORY, 11); // EUnknown
        }
    }
}
