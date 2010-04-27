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

package com.nokia.mj.impl.gcf.protocol.file;

import java.io.IOException;
import com.nokia.mj.impl.gcf.utils.J9GcfConnectionBase;

public class Connection extends J9GcfConnectionBase
{
    public Connection()
    {
    }

    protected javax.microedition.io.Connection createConnection(String aName,
            int aMode, boolean aTimeouts) throws IOException
    {
        return (new Protocol()).openConnection(aName, aMode, aTimeouts);
    }
}