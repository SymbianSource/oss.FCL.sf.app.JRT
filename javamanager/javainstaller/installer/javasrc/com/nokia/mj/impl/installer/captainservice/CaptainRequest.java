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


package com.nokia.mj.impl.installer.captainservice;

import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.utils.Uid;

/**
 * Message used to notify JavaCaptain of new, updated and removed applications,
 * and to start and stop applications.
 *
 * JavaCaptain responds with CaptainResponse in case of stop operation.
 * There is no response to start and update operations.
 */
public class CaptainRequest extends CommsMessage
{
    /** Request to start specified applications. */
    public static final int OP_START = 1;
    /** Request to stop specified applications. */
    public static final int OP_STOP = 2;
    /** Notification that specified applications have been
        added, upgraded, or removed. */
    public static final int OP_UPDATE = 3;

    /** Indicates that no options are specified. */
    public static final int OPT_NONE = 0;
    /** Indicates that requested operation is to be performed silently,
        i.e. without any user interaction. */
    public static final int OPT_SILENT = 1;

    /** Message id and target module id. */
    private static final int MSG_ID = 110;
    private static final int MOD_ID = 6;

    /** Requested operation. */
    private int iOperation = 0;
    /** Options for the operation. Each bit presents one option. */
    private int iOptions = 0;
    /** Uids to which requested operation applies (for start and stop operations). */
    private Uid[] iUids = null;
    /** Uids to which requested operation applies (for update operation). */
    private Uid[] iBeforeUids = null;
    private Uid[] iAfterUids = null;

    /**
     * Construct a CaptainRequest message for start and stop operations.
     * @param aOperation requested operation
     * @param aOptions options for the operation
     * @param aUids Uids to which the operation applies
     */
    public CaptainRequest(int aOperation, int aOptions, Uid[] aUids)
    {
        super();
        if (aOperation != OP_START &&
                aOperation != OP_STOP)
        {
            InstallerException.internalError(
                "CaptainRequest: Invalid operation: " + aOperation);
        }
        if (aUids == null || aUids.length == 0)
        {
            InstallerException.internalError("CaptainRequest: Missing Uids.");
        }
        iOperation = aOperation;
        iOptions = aOptions;
        iUids = aUids;
        // Initialize the message.
        setMessageId(MSG_ID);
        setModuleId(MOD_ID);
        write(iOperation);
        write(iOptions);
        write(iUids.length);
        for (int i = 0; i < iUids.length; i++)
        {
            write(iUids[i].getStringValue());
        }
    }

    /**
     * Construct a CaptainRequest message update operation.
     * @param aOperation requested operation
     * @param aOptions options for the operation
     * @param aBeforeUids Uids valid before the operation
     * @param aAfterUids Uids valid after the operation
     */
    public CaptainRequest(int aOperation, int aOptions, Uid[] aBeforeUids, Uid[] aAfterUids)
    {
        super();
        if (aOperation != OP_UPDATE)
        {
            InstallerException.internalError(
                "CaptainRequest: Invalid operation: " + aOperation);
        }
        iOperation = aOperation;
        iOptions = aOptions;
        iBeforeUids = aBeforeUids;
        iAfterUids = aAfterUids;
        // Initialize the message.
        setMessageId(MSG_ID);
        setModuleId(MOD_ID);
        write(iOperation);
        write(iOptions);
        if (aBeforeUids != null)
        {
            // Write aBeforeUids
            write(aBeforeUids.length);
            for (int i = 0; i < aBeforeUids.length; i++)
            {
                write(aBeforeUids[i].getStringValue());
            }
        }
        else
        {
            // Write zero to indicate that no aBeforeUids are present
            write(0);
        }
        if (aAfterUids != null)
        {
            // Write aAfterUids
            write(aAfterUids.length);
            for (int i = 0; i < aAfterUids.length; i++)
            {
                write(aAfterUids[i].getStringValue());
            }
        }
        else
        {
            // Write zero to indicate that no aAfterUids are present
            write(0);
        }
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append("CaptainRequest: op=").append(iOperation);
        buf.append(", opt=").append(iOptions);
        if (iUids != null)
        {
            buf.append(", uids=");
            for (int i = 0; i < iUids.length; i++)
            {
                buf.append(iUids[i].toString());
                if (i < iUids.length-1)
                {
                    buf.append(",");
                }
            }
        }
        if (iBeforeUids != null)
        {
            buf.append(", beforeUids=");
            for (int i = 0; i < iBeforeUids.length; i++)
            {
                buf.append(iBeforeUids[i].toString());
                if (i < iBeforeUids.length-1)
                {
                    buf.append(",");
                }
            }
        }
        if (iAfterUids != null)
        {
            buf.append(", afterUids=");
            for (int i = 0; i < iAfterUids.length; i++)
            {
                buf.append(iAfterUids[i].toString());
                if (i < iAfterUids.length-1)
                {
                    buf.append(",");
                }
            }
        }
        return buf.toString();
    }
}
