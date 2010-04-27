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
* Description:  List of existing SensorConnections
*
*/


#ifndef SENSORCONNECTIONLIST_H
#define SENSORCONNECTIONLIST_H

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "sensorfinder.h"

// FORWARD DECLARATIONS
class SensorConnection;

// CLASS DESCRIPTION
/**
 * List of ServiceConnections
 *
 * @lib N/A
 * @since S60 3.2
 */
class SensorConnectionList
{
    // Simple linked list
    struct SensorList
    {
        SensorConnection* iSensorConnection;
        SensorList* iNext;
    };

public:
    /**
     * Default constructor
     */
    SensorConnectionList();

    /**
     * Destructor
     */
    virtual ~SensorConnectionList();

public: // New methods

    /**
     * Inits sensors
     * @aparam aSensorCount int pointer where to store the number of sensors
     * @return Array of sensor information
     */
    unsigned short** InitSensors(int* aSensorCount);

    /**
     * Creates new SensorConnection
     * @param aSensorIndex, Index of created sensor
     * @return new SensorConnection or NULL if creation failed
     */
    SensorConnection* CreateSensorConnection(int aSensorIndex);

    /**
     * Removes connection from list and deletes it
     * Silently ignores if this connection cannot be found in list
     * @param aConnection Connection to be removed
     */
    void RemoveConnection(SensorConnection* aConnection);

private: // Data
    // SensorFinder, owned
    SensorFinder* iFinder;

    // List of sensorconnections, owned
    SensorList* iSensors;
};

#endif // SENSORCONNECTIONLIST_H

// End of file
