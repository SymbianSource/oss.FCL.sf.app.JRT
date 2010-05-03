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

package com.nokia.mj.impl.cbs;

import java.io.IOException;
import java.io.InterruptedIOException;
import javax.wireless.messaging.Message;
import com.nokia.mj.impl.smscbs.utils.WmaUrl;
import com.nokia.mj.impl.sms.SMSConnectionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

public final class CBSConnectionImpl extends SMSConnectionImpl
{
    private static final String CBS_MSG =
        "Message sending failed: CBS can only receive messages";

    public CBSConnectionImpl(WmaUrl aMsgUri,
                             boolean aServerMode) throws IOException
    {
        super(aMsgUri, aServerMode);
    }

    public void send(Message aMsg) throws IOException, InterruptedIOException
    {
        throw new IOException(CBS_MSG);
    }

    /**
     * Checks for security permission.
     * @exception throws SecurityException if no valid permission exists.
     */
    protected void checkReceivePermission()
    {
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        CBSPermissionImpl permission = new CBSPermissionImpl("cbs://*",
                "receive");
        appUtils.checkPermission(permission);

    }
}