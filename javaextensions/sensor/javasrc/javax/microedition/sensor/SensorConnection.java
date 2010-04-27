/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.sensor;

import java.io.IOException;

import javax.microedition.io.Connection;



/**
 * Please refer JSR 256 for more details.
 */
public interface SensorConnection extends Connection
{
    /**
     * Please refer JSR 256 for more details.
     */
    public static final int STATE_OPENED = 1;

    /**
     * Please refer JSR 256 for more details.
     */
    public static final int STATE_LISTENING = 2;

    /**
     * Please refer JSR 256 for more details.
     */
    public static final int STATE_CLOSED = 4;


    /**
     * Please refer JSR 256 for more details.
     */
    public Channel getChannel(ChannelInfo channelInfo);


    /**
     * Please refer JSR 256 for more details.
     */
    public Data[] getData(int bufferSize) throws IOException;



    /**
     * Please refer JSR 256 for more details.
     */
    public Data[] getData(int bufferSize, long bufferingPeriod, boolean isTimestampIncluded, boolean isUncertaintyIncluded, boolean isValidityIncluded) throws IOException;


    /**
     * Please refer JSR 256 for more details.
     */
    public int[] getErrorCodes();



    /**
     * Please refer JSR 256 for more details.
     */
    public String getErrorText(int errorCode);

    /**
     * Please refer JSR 256 for more details.
     */
    public SensorInfo getSensorInfo();



    /**
     * Please refer JSR 256 for more details.
     */
    public int getState();


    /**
     * Please refer JSR 256 for more details.
     */
    public void removeDataListener();



    /**
     * Please refer JSR 256 for more details.
     */
    public void setDataListener(DataListener listener, int bufferSize);



    /**
     * Please refer JSR 256 for more details.
     */
    public void setDataListener(DataListener listener, int bufferSize, long bufferingPeriod, boolean isTimestampIncluded, boolean isUncertaintyIncluded, boolean isValidityIncluded);



}
