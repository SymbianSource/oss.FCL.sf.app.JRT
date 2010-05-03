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
package javax.microedition.amms;

import javax.microedition.media.*;

import java.io.InputStream;
import java.io.OutputStream;

/**
 * Please refer JSR 234 for more details.
 */
public interface MediaProcessor extends Controllable
{

    /**
     * Please refer JSR 234 for more details.
     */
    public final int UNKNOWN = -1;

    /**
     * Please refer JSR 234 for more details.
     *
     */
    public final int UNREALIZED = 100;

    /**
     * Please refer JSR 234 for more details.
     */
    public final int REALIZED = 200;

    /**
     * Please refer JSR 234 for more details.
     */
    public final int STARTED = 400;

    /**
     * Please refer JSR 234 for more details.
     */
    public final int STOPPED = 300;



    /**
     * Please refer JSR 234 for more details.
     *
     */
    public void setInput(InputStream input, int length) throws javax.microedition.media.MediaException;

    /**
     * Please refer JSR 234 for more details.
     *
     */
    public void setInput(Object image) throws javax.microedition.media.MediaException;

    /**
     * Please refer JSR 234 for more details.
     */
    public void setOutput(OutputStream output);

    /**
     * Please refer JSR 234 for more details.
     */
    public void start() throws MediaException;

    /**
     * Please refer JSR 234 for more details.
     */
    public void stop() throws MediaException;

    /**
     * Please refer JSR 234 for more details.
     */
    public void complete() throws MediaException;

    /**
     * Please refer JSR 234 for more details.
     */
    public void abort();

    /**
     * Please refer JSR 234 for more details.
     */
    public void addMediaProcessorListener(MediaProcessorListener mediaProcessorListener);

    /**
     * Please refer JSR 234 for more details.
     */
    public void removeMediaProcessorListener(MediaProcessorListener mediaProcessorListener);

    /**
     * Please refer JSR 234 for more details.
     */
    public int getProgress();

    /**
     * Please refer JSR 234 for more details.
     */
    public int getState();

}
