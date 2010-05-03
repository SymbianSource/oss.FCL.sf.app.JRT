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


/**
 * Please refer JSR 256 for more details.
 */
public interface ChannelInfo
{
    /**
     * Please refer JSR 256 for more details.
     */
    public static final int TYPE_OBJECT = 4;

    /**
     * Please refer JSR 256 for more details.
     */
    public static final int TYPE_INT = 2;

    /**
     * Please refer JSR 256 for more details.
     */
    public static final int TYPE_DOUBLE = 1;


    /**
     * Please refer JSR 256 for more details.
     */
    public float getAccuracy();

    /**
     * Please refer JSR 256 for more details.
     */
    public int getDataType();


    /**
     * Please refer JSR 256 for more details.
     */
    public MeasurementRange[] getMeasurementRanges();

    /**
     * Please refer JSR 256 for more details.
     */
    public String getName();


    /**
     * Please refer JSR 256 for more details.
     */
    public int getScale();

    /**
     * Please refer JSR 256 for more details.
     */
    public Unit getUnit();



}