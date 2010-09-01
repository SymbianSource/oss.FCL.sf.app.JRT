/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Please refer JSR 135 for more details.
*
*/


package javax.microedition.media.protocol;

import javax.microedition.media.Controllable;
import java.io.IOException;

abstract public class DataSource implements Controllable
{
    private String iSourceLocator;

    public DataSource(String aLocator)
    {
        iSourceLocator = aLocator;
    }

    public abstract String getContentType();

    public abstract void connect() throws IOException;

    public abstract void disconnect();

    public abstract void start() throws IOException;

    public abstract void stop() throws IOException;

    public abstract SourceStream[] getStreams();

    public String getLocator()
    {
        return iSourceLocator;
    }
}
