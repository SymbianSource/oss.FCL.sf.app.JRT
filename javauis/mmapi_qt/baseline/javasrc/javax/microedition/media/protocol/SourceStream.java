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

import java.io.IOException;
import javax.microedition.media.Controllable;

public interface SourceStream extends Controllable
{
    int NOT_SEEKABLE = 0;

    int SEEKABLE_TO_START = 1;

    int RANDOM_ACCESSIBLE = 2;

    ContentDescriptor getContentDescriptor();

    long getContentLength();

    int read(byte[] aB, int aOff, int aLen)
    throws IOException;

    int getTransferSize();

    long seek(long aWhere) throws IOException;

    long tell();

    int getSeekType();
}
