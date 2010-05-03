/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for SensorListener
*
*/

#ifndef SENSORLISTENER_H
#define SENSORLISTENER_H

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

// FORWARD DECLARATIONS
struct SensorData;

// CLASS DESCRIPTION
/**
 * Interface for SensorListener
 * Used for getting data back from sensor
 *
 * @lib N/A
 * @since S60 3.2
 */
class SensorListener
{
public: // New methods
    /**
     * Callback method for data receiving
     * @param aData, array of data received
     * @param aIsDataLost, info if there is missed datas
     */
    virtual void DataReceived(SensorData** aData, bool aIsDataLost) = 0;

    /**
     * Callback method for signaling that a Condition is met
     * @param aHandle handle to condition which was met
     * @param aValue the measured value that condition matched to
     */
    virtual void ConditionMet(void* aHandle, int aChannelId,
                              double aValue, long aTimeStamp) = 0;
};

#endif // SENSORLISTENER_H

// End of file

