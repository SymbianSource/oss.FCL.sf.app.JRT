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

package javax.microedition.media.control;

import java.io.IOException;
import java.io.OutputStream;
import javax.microedition.media.MediaException;

public interface RecordControl extends javax.microedition.media.Control
{
    void setRecordStream(OutputStream aStream);

    void setRecordLocation(String aLocator)
    throws IOException, MediaException;

    String getContentType();

    void startRecord();

    void stopRecord();

    void commit() throws IOException;

    int setRecordSizeLimit(int aSize) throws MediaException;

    void reset() throws IOException;
}
