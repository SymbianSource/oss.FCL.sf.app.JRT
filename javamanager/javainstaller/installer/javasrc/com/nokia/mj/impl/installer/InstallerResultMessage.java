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
import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.utils.ErrorMessageBase;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.Uid;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

/**
 * InstallerResultMessage contains information about install, uninstall
 * and componentinfo operation results. JavaInstaller sends it at the end
 * of operation to Comms endpoint specified at JavaInstaller startup with
 * -commsresult option.
 * <p>
 * InstallerResultMessage contains name-value pairs: name is a string,
 * value can be either a string or an int. Message syntax:
 * <p>
 * <pre>
 * message := length named_int_value* length named_string_value*
 * named_int_value := name int_value
 * named_string_value := name string_value
 * name := &lt;string&gt;
 * int_value := &lt;int&gt;
 * string_value := &lt;string&gt;
 * length := &lt;int telling the length of the table that follows&gt;
 * </pre>
 * <p>
 */
public class InstallerResultMessage
{
    /** Operation type. Value type: int.
        Possible values: 0: install, 1: uninstall, 2: componentinfo. */
    public static final String NAME_OPERATION = "operation";
    /** Status code indicating operation result. Value type: int. */
    public static final String NAME_RESULT = "result";
    /** SIF error category. Value type: int. */
    public static final String NAME_ERROR_CATEGORY = "error-category";
    /** Java runtime specific error code. Value type: int. */
    public static final String NAME_ERROR_CODE = "error-code";
    /** Localized error message. Value type: string. */
    public static final String NAME_ERROR_MSG = "error-message";
    /** Localized message with more details of the error reason. Value type: string. */
    public static final String NAME_ERROR_DETAILS = "error-details";
    /** Suite UID. Value type: int. */
    public static final String NAME_SUITE_UID = "suite-uid";
    /** Midlet-n UID. Value type: int. */
    public static final String NAME_MIDLET_UID = "midlet-uid-";
    /** Suite component id. Value type: int. */
    public static final String NAME_SUITE_CID = "suite-cid";
    /** Midlet-n component id. Value type: int. */
    public static final String NAME_MIDLET_CID = "midlet-cid-";
    /** Suite global id. Value type: string. */
    public static final String NAME_SUITE_GID = "suite-gid";
    /** Midlet-n global id. Value type: string. */
    public static final String NAME_MIDLET_GID = "midlet-gid-";
    /** Suite name. Value type: string. */
    public static final String NAME_SUITE_NAME = "suite-name";
    /** Midlet-n name. Value type: string. */
    public static final String NAME_MIDLET_NAME=  "midlet-name-";
    /** Suite vendor. Value type: string. */
    public static final String NAME_VENDOR = "vendor";
    /** Suite version. Value type: string. */
    public static final String NAME_VERSION = "version";
    /** Component installation status. Value type: int. Possible values:
        0: new component,
        1: upgrade,
        2: already installed,
        3: newer version already installed,
        4: invalid package, cannot be installed. */
    public static final String NAME_INSTALL_STATUS = "install-status";
    /** Component authenticity. Value type: int. Possible values:
        0: component is not authenticated, 1: component is authenticated. */
    public static final String NAME_AUTHENTICITY = "authenticity";
    /** Size of the files owned by the component at the time of the
        installation. Calculated from MIDlet-Data-Size and
        MIDlet-Jar-Size attributes. Value type: int. */
    public static final String NAME_COMPONENT_SIZE = "component-size";

    /** Id for installer result Comms message. */
    private static final int INSTALLER_RESULT_MESSAGE_ID = 601;

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
            addErrorCodes(eb);
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
     * Returns a string value from this message, or null
     * if string value is not present.
     */
    String getStringValue(String aName)
    {
        Object value = iNamedStringValues.get(aName);
        if (value instanceof String)
        {
            return (String)value;
        }
        return null;
    }

    /**
     * Get an int value from this message.
     * @throws IllegalArgumentException if value with given name is not found
     */
    int getIntValue(String aName)
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
    void removeValue(String aName)
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
        InstallerException installerException = null;
        if (aEndpoints != null)
        {
            try
            {
                for (int i = 0; i < aEndpoints.length; i++)
                {
                    send(aEndpoints[i]);
                }
            }
            catch (InstallerException ie)
            {
                installerException = ie;
            }
        }
        if (iSifNotifier != null)
        {
            try
            {
                int result = getIntValue(NAME_RESULT);
                int errCategory = 0;
                int errCode = 0;
                String errMsg = null;
                String errDetails = null;
                if (result != Installer.ERR_NONE)
                {
                    errCategory = getIntValue(NAME_ERROR_CATEGORY);
                    errCode = getIntValue(NAME_ERROR_CODE);
                    errMsg = getStringValue(NAME_ERROR_MSG);
                    errDetails = getStringValue(NAME_ERROR_DETAILS);
                }
                iSifNotifier.notifyEnd(errCategory, errCode, errMsg, errDetails);
            }
            catch (Throwable t)
            {
                Log.logError("InstallerResultMessage: SifNotifier.notifyEnd failed", t);
            }
        }
        if (installerException != null)
        {
            throw installerException;
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
            msg.setMessageId(INSTALLER_RESULT_MESSAGE_ID);
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
            comms.sendReceive(msg, 5);
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
     * Adds error codes to the result message.
     */
    private void addErrorCodes(ExceptionBase aEb)
    {
        if (aEb.getShortMessageId() >= ErrorMessageBase.INSTALLER_RANGE_START &&
            aEb.getShortMessageId() <= ErrorMessageBase.INSTALLER_RANGE_END)
        {
            addValue(NAME_ERROR_CATEGORY,
                     SifRegistrator.getErrorCategory(aEb.getShortMessageId()));
            addValue(NAME_ERROR_CODE,
                     aEb.getShortMessageId() + aEb.getDetailedMessageId());
        }
        else if (aEb.getShortMessageId() >= ErrorMessageBase.SECURITY_RANGE_START &&
                 aEb.getShortMessageId() <= ErrorMessageBase.SECURITY_RANGE_END)
        {
            addValue(NAME_ERROR_CATEGORY,
                     SifRegistrator.getErrorCategory(
                         InstallerErrorMessage.INST_AUTHORIZATION_ERR));
            addValue(NAME_ERROR_CODE,
                     aEb.getShortMessageId() + aEb.getDetailedMessageId());
        }
        else
        {
            addValue(NAME_ERROR_CATEGORY,
                     SifRegistrator.getErrorCategory(
                         InstallerErrorMessage.INST_UNEXPECTED_ERR));
            addValue(NAME_ERROR_CODE,
                     aEb.getShortMessageId()*1000 + aEb.getDetailedMessageId());
        }
    }
}
