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

public interface PlayerListener
{

    public static final String STARTED="started";
    public static final String STOPPED="stopped";
    public static final String END_OF_MEDIA="endOfMedia";
    public static final String DURATION_UPDATED="durationUpdated";
    public static final String DEVICE_UNAVAILABLE="deviceUnavailable";
    public static final String DEVICE_AVAILABLE="deviceAvailable";
    public static final String VOLUME_CHANGED="volumeChanged";
    public static final String ERROR="error";
    public static final String CLOSED="closed";


    /**
     * The following constants do not belong to the MIDP-NG multimedia subset
     */

    public static final String STOPPED_AT_TIME="stoppedAtTime";
    public static final String SIZE_CHANGED="sizeChanged";
    public static final String RECORD_STARTED="recordStarted";
    public static final String RECORD_STOPPED="recordStopped";
    public static final String RECORD_ERROR="recordError";
    public static final String BUFFERING_STARTED="bufferingStarted";
    public static final String BUFFERING_STOPPED="bufferingStopped";

    /**
     * This method belongs to the MIDP-NG multimedia subset
     */

    public void playerUpdate(Player aPlayer, String aEvent, Object aEventData);

}