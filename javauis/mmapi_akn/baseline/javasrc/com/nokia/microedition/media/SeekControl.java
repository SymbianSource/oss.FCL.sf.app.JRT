/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface to seek stream.
*
*/

package com.nokia.microedition.media;

import java.io.IOException;
import javax.microedition.media.Control;

/**
 * Interface to seek stream to wanted position.
 */
public interface SeekControl extends Control
{
    /**
     * Seeks stream to wanted position.
     * @param aWhere position in the stream.
     * @throw IOException Thrown if an I/O error occurs.
     */
    void seek(int aWhere) throws IOException;

    /**
     * Close controlled stream.
     */
    void close();
}
// End of File
