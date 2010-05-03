/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.gcf;

import java.io.IOException;
import java.lang.String;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.CommsEndpoint;


/**
 * This interface is used to validate push connection URI and 'filter' arguments passed
 * to PushRegistry.registerConnection operation or if those arguments has been read from
 * read from .jad/.jar files.
 * Each push capable protocol must implement this interface. Implementation class of this
 * interface must be located in the 'com.nokia.mj.impl.gcf.protocol.XXX' package. XXX means
 * scheme of the protocol(e.g. 'com.nokia.mj.impl.gcf.protocol.http'). Name of the implementation
 * class must be PushValidatorImpl.
 * Note: PushValidatorImpl must have default constructor(i.e. without arguments).
 */
public abstract class PushValidator
{

    private static final int MSG_ID = 300 + 15;
    private static final int MOD_ID = 3;

    public static final String INSTALLER_COMMS = "InstallerJavaCaptain";
    public static final String RUNTIME_COMMS = "javacaptain";

    /**
     * Validates push connection URI and 'filter' arguments.
     * @param aConnection dynamic push URI passed as argument to PushRegistry.registerConnection
     *                    operation. Note: null and empty string checks is not needed done by user.
     * @param aFilter     'filter' argument passed as argument to PushRegistry.registerConnection
     *                     operation. Note: null and empty string checks is not needed done by user.
     * @param aSuiteuid   MIDlet suite uid.
     * @param aMidletName name of the MIDlet.
     * @param aIsStaticRegistration indicates whether static/dynamic push registration is ongoing.
     * @throws            IllegalArgumentException if argument(s) are not valid.
     * @throws            IOException if validation fails to internal error.
     */
    public abstract void validate(String aConnection, String aFilter, Uid aSuiteuid,
                                  String aMidletName, boolean aIsStaticRegistration)
    throws IOException;

    /**
     * This method checks whether any URI of already registered push connection starts with
     * same URI passed as in aBaseUri argument. Comparison is case insensitive and it assumes
     * that URI contains only us-ascii characters. Non-ascii characters must be encoded by escaping
     * meechanism.
     * Currently only bluetooth protocol needs this functionality so this method does not need to
     * call unless specification of the protocol requires this functionality.
     * @param aBaseUri aBaseUri this argument is used to check whether any push URI starts
     *                          with same URI.
     *                          E.g. Existing "btspp://localhost:3B;name=ABC" push URI
     *                          and "btspp://localhost:3B" base URI are equals from this
     *                          operation's point of view.
     * @param aIgnoreThis This operation returns 'false' if base URI matches to start of existing
     *                    push URI and returned suite uid is equal with 'aIgnoreThis' argument.
     *                    Value of the aIgnoreThis argument can be also null. In this case returned
     *                    suite uid is not compared to 'aIgnoreThis' argument.
     * @param aCommsName  Name of the CommsEndpoint. Value of this argument must be
     *                    "InstallerJavaCaptain" in the Installer(= during static push registration)
     *                    and "javacaptain" when this operation is called from the midp runtime
     *                    (= e.g. during dynamic push registration). It is recommened to use
     *                    INSTALLER_COMMS and RUNTIME_COMMS constant as value of this argument.
     * @return            'true' if base URI matches to start of existing push URI and returned
     *                    suite uid is not equal with 'aIgnoreThis' argument.
     * @throws IOException  if comparison fails to the internal error.
     */
    public static boolean isRegisteredPushUriStartingWith(String aBaseUri, Uid aIgnoreThis,
            String aCommsName)
    throws IOException
    {
        Logger.LOG(Logger.EJavaPush, Logger.EInfo, "+ PushValidator.isRegisteredPushUriStartingWith()");

        if (null == aBaseUri)
            throw new NullPointerException("null pointer was passed as an argument to " +
                                           "PushValidator.isRegisteredPushUriStartingWith() operation");
        aBaseUri = aBaseUri.trim();

        CommsEndpoint commsObj = CommsEndpoint.find(aCommsName);
        if (null == commsObj)
            throw new IOException("Comms endpoint not initialized");

        CommsMessage msg = new CommsMessage();
        msg.setMessageId(MSG_ID);
        msg.setModuleId(MOD_ID);
        msg.write(aBaseUri);
        CommsMessage retMsg = commsObj.sendReceive(msg,15); // send the message

        return handleReplyMsg(retMsg,aIgnoreThis);

    }//end isRegisteredPushUriStartingWith

    /**
     *
     */
    private static boolean handleReplyMsg(CommsMessage aMsg, Uid aIgnoreThis)
    throws IOException
    {
        Logger.LOG(Logger.EJavaPush, Logger.EInfo, "+ PushValidator.handleReplyMsg()");

        int status = aMsg.readInt();
        if ((-1 != status) && (0 != status))
        {
            throw new IOException("Incorrect status value in IPC_PR_PUSH_URI_STARTS_WITH_QUERY_REPLY " +
                                  "comms msg");
        }

        if (-1 == status)
            return false;

        String uidAsStr = aMsg.readString();
        Uid suiteUid = Uid.createUid(uidAsStr);
        if (null == suiteUid)
        {
            throw new IOException("Incorrect suite uid value in IPC_PR_PUSH_URI_STARTS_WITH_QUERY_REPLY " +
                                  "comms msg");
        }

        if (null != aIgnoreThis)
        {
            if (true == aIgnoreThis.equals(suiteUid))
                return false;
        }

        return true;
    }//end handleReplyMsg

}//end PushValidator


