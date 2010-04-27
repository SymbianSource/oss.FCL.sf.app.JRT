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
* Description:  Interface for Sensor
*
*/

#ifndef SENSORCONNECTION_H
#define SENSORCONNECTION_H

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "sensor.h"
#include "sensorlistener.h"

// DATA STRUCTURES
struct DataFillerParams
{
    DataFillerParams():
            iDatas(0),
            iIsDataLost(false),
            iDataCount(0),
            iDataObjects(0),
            iJavaPeer(0),
            iJniEnv(0)
    { };
    SensorData** iDatas;
    bool iIsDataLost;
    int iDataCount;
    void* iDataObjects;
    void* iJavaPeer;
    void* iJniEnv;
};

typedef void (*ConditionMetCB)(void*, void*, void*, int, double, long);
typedef void (*DataReceivedCB)(DataFillerParams*);

// CLASS DESCRIPTION
/**
 * Interface for Sensor
 * Used for controlling sensors
 *
 * @lib N/A
 * @since S60 3.2
 */
class SensorConnection : public SensorListener
{
public: // Constructor and Destructor
    /**
     * Destructor
     */
    ~SensorConnection();

    /**
     * Constructor
     * @param aSensor, Sensor assosiated with this connection
     */
    SensorConnection(Sensor* sensor);

public: // From SensorListener
    void DataReceived(SensorData** aData, bool aIsDataLost);

    void ConditionMet(void* aHandle, int aChannelId,
                      double aValue, long aTimeStamp);

public: // New methods
    /**
     * Returns pointer to actual sensor implementation
     */
    Sensor* GetSensor();

    /**
     * Returns assigned java peer object
     */
    void* GetJavaPeer()
    {
        return iJavaPeer;
    };

    /**
     * Sets java peer objet
     * @param aJavaPeer, peer object to set
     */
    void SetJavaPeer(void* aJavaPeer)
    {
        iJavaPeer = aJavaPeer;
    };

    /**
     * Store jni env
     */
    void SetJni(void* aJni)
    {
        iJniEnv = aJni;
    };

    /**
     * Sets callback methods
     * @param aDataReceived
     * @param aConditionMet
     */
    void SetCallbacks(DataReceivedCB aDataReceived,
                      ConditionMetCB aConditionMet)
    {
        iDataReceived = aDataReceived;
        iConditionMet = aConditionMet;
    }
    /**
     * Prepare for data listening
     * Creates sensor datas etc when needed
     * @return error code or 0 if no error
     */
    int PrepareDataListening(void* aNewDataObjects,
                             int aDataCount,
                             void*& aOldDataObject,
                             int aDataType);

    /**
     * Starts data listening
     */
    int StartDataListening(int aBufferSize,
                           long aBufferingPeriod,
                           bool aTimestampsIncluded,
                           bool aValiditiesIncluded,
                           bool aIsOneShot);

    /**
     * Returns stored dataobjects
     */
    void* DataObjects();

private: // New methods

    // Allocates new data object
    SensorData** AllocateData(int aBufferSize,
                              int aDataType);

    // deletes data filler params and corresponding
    void ClearData(SensorData** aDatas, int aCount);

private: // Data
    // Sensor, owned
    Sensor* iSensor;

    // Data filling parameters, owned
    DataFillerParams* iParams;

    // JNIEnv, not owned
    void* iJniEnv;

    // Java side peer object, not owned
    void* iJavaPeer;

    // Callback methods
    DataReceivedCB iDataReceived;
    ConditionMetCB iConditionMet;

    int iBufferSize;
    bool iTimeStamps;
    bool iValidities;
};

#endif // SENSORCONNECTION_H

// End of file
