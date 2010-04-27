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
* Description:  Interface for SensorFinder
*
*/

#ifndef SENSORFINDER_H
#define SENSORFINDER_H

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

// FORWARD DECLARATIONS
class Sensor;

// CLASS DESCRIPTION
/**
 * Interface for SensorFinder
 * Used for finding sensors from underlying OS
 *
 * @lib N/A
 * @since S60 3.2
 */
class SensorFinder
{
public: // New methods
    /**
     * Finds sensors
     * @param aSensorCount int pointer for size of the returned sensor information array
     * @return Array of sensor information
     */
    virtual unsigned short** FindSensors(int* aSensorCount) = 0;

    /**
     * Creates new instance of Sensor
     * @param aIndex, Index number of sensor,
     *                   Sensors should be in same order as
     *                   they are received from FindSensors
     */
    virtual Sensor* CreateSensor(int aIndex) = 0;

    /**
     * Destructor, to allow deletion through this interface
     */
    virtual ~SensorFinder() {};
};
#endif // SENSORFINDER_H

// End of file
