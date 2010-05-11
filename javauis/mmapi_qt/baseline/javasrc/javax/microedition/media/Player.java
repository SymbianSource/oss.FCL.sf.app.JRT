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
* Description:
*
*/


package javax.microedition.media;

public interface Player extends Controllable
{

    public static final int UNREALIZED=100;
    public static final int REALIZED=200;
    public static final int PREFETCHED=300;
    public static final int STARTED=400;
    public static final int CLOSED=0;
    public static final long TIME_UNKNOWN=-1;

    public void realize() throws MediaException;

    public void prefetch() throws MediaException;

    public void start() throws MediaException;

    public void stop() throws MediaException;

    public void deallocate();

    public void close();

    public long setMediaTime(long aTime) throws MediaException;

    public long getMediaTime();

    public int getState();

    public long getDuration();

    public String getContentType();

    public void setLoopCount(int aCount);

    public void addPlayerListener(PlayerListener aPlayerListener);

    public void removePlayerListener(PlayerListener aPlayerListener);

    /**
     * The following methods do not belong to the MIDP-NG multimedia subset
     */

    public void setTimeBase(TimeBase aTimeBase) throws MediaException;

    public TimeBase getTimeBase();

}