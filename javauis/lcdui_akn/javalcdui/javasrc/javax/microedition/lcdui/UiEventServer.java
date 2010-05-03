/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

package javax.microedition.lcdui;

import com.nokia.mj.impl.rt.legacy.MIDEventServer;
import com.nokia.mj.impl.rt.legacy.MIDEventServerFactory;
import com.nokia.mj.impl.rt.legacy.NativeError;

class UiEventServer extends MIDEventServer
{
    UiEventServer(String aName, int aAppUid)
    {
        super(aName, new UiEventServerFactory(aAppUid));
    }
}

class UiEventServerFactory implements MIDEventServerFactory
{
    private final int iAppUid;
    UiEventServerFactory(int aAppUid)
    {
        iAppUid=aAppUid;
    }
    public int createServer(String aName)
    {
        return _createUiServer(iAppUid);
    }
    private native int _createUiServer(int aAppUid);
}
