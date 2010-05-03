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


package javax.microedition.sensor.control;


import javax.microedition.sensor.ChannelInfo;
import javax.microedition.sensor.MeasurementRange;


/**
 * Please refer JSR 256 for more details.
 */
public interface MeasurementRangeControl extends Control
{

    /**
     * Please refer JSR 256 for more details.
     */
    public MeasurementRange getMeasurementRange(ChannelInfo channelInfo);


    /**
     * Please refer JSR 256 for more details.
     */
    public void setMeasurementRange(ChannelInfo channelInfo, MeasurementRange measurementRange);

}
