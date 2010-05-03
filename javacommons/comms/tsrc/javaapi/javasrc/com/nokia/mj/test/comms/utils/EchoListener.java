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


package com.nokia.mj.test.comms.utils;

import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.CommsEndpointBase;
import com.nokia.mj.impl.comms.CommsListener;


public class EchoListener implements CommsListener
{
    private CommsEndpointBase iComms = null;

    public EchoListener(CommsEndpointBase aComms)
    {
        iComms = aComms;
    }

    public void processMessage(CommsMessage aMessage)
    {
        try
        {
            CommsMessage msg = new CommsMessage();
            msg.replyTo(aMessage);
            msg.write(aMessage.readBytes());
            iComms.send(msg);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    }
}

