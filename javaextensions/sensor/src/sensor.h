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

#ifndef SENSOR_H
#define SENSOR_H


// FORWARD DECLARATIONS
class SensorListener;

/**
 * SensorData struct for storing data arrays
 */
struct SensorData
{
    SensorData():
            iIntValues(0),
            iDoubleValues(0),
            iValidities(0),
            iTimeStamps(0),
            iNumOfValues(0)
    { }
    ~SensorData()
    {
        delete iIntValues;
        delete iDoubleValues;
        delete iValidities;
        delete iTimeStamps;
    }
    // Array of int values (could be null, if not supported by sensor)
    int* iIntValues;
    // Array of double values (could be null, if not supported by sensor)
    double* iDoubleValues;
    // Array of validities (could be null if not asked by user)
    bool* iValidities;
    // Array of time stamps (could be null if not asked by user)
    long long* iTimeStamps;
    // Number of data values
    long iNumOfValues;
    // Wanted buffer size
    int iBufferSize;
    // Time stamps included
    bool iTimeStampsIncluded;
    // Validities included
    bool iValiditiesIncluded;
};

// CLASS DESCRIPTION
/**
 * Interface for Sensor
 * Used for controlling sensors
 *
 * @lib N/A
 * @since S60 3.2
 */
class Sensor
{
public: // New methods
    /**
     * Starts the server for listening the events, does nothing if not
     * needed
     */
    virtual void StartServer() = 0;

    /**
     * Stops server
     */
    virtual void StopServer() = 0;

    /**
     * Opens channel to sensor, must be called before
     * starting listening, sets listener
     * @param Listener
     * @return possible error codes, 0 for no error
     */
    virtual int OpenChannel(SensorListener* aListener) = 0;

    /**
     * Starts listening the data
     * @param aData, array of sensor datas to be filled
     * @param aBufferSize number of data values listened
     * @param aBufferingPeriod, the time to buffer values -
     *                             given in milliseconds,
     *                             bufferingPeriod < 1 means the
     *                             period is left undefined
     * @param aTimestampsIncluded if true timestamps should
     *                               be included in returned Data objects
     * @param aValiditiesIncluded if true validities should be included
     *                               in returned Data objects
     * @param aIsOneShot, true if only one data is needed, (getData)
     *                       false if continuos data retrieval (data listening)
     * @return possible error codes, 0 for no error
     */
    virtual int StartDataListening(SensorData** aData,
                                   int aBufferSize,
                                   long aBufferingPeriod,
                                   bool aTimestampsIncluded,
                                   bool aValiditiesIncluded,
                                   bool aIsOneShot) = 0;

    /**
     * Cancel listening to data
     */
    virtual int CancelDataListening() = 0;

    /**
     * Closes channel to sensor
     */
    virtual void CloseChannel() = 0;


    /**
     * Adds condition and starts condition listening
     *
     * @param aHandle pointer to created condition object
     * @param aChannelId channel index
     * @param aLowerLimit limit or lower limit of range
     * @param aUpperLimit upper limit or discarded in case of LimitCondition
     * @param aLowerOp operator type for limit or lower limit in range
     * @param aUpperOp upper limit operator or discarded in case of LimitCondition
     * @return int error code. ERROR_NONE if no error.
     */
    virtual int AddCondition(
        void** aHandle,
        int aChannelId,
        double aLowerLimit,
        double aUpperLimit,
        int aLowerOp,
        int aUpperOp) = 0;

    /**
     * Removes condition from native side
     * @param aHandle pointer to condition object to remove
     */
    virtual int RemoveCondition(
        void* aHandle) = 0;

    /**
     * Starts condition listening
     * @param aListeningType, 0 means normal condition listening
     *                           1 means that condition listening is done in java side
     *                             and all values are passed (native conditions ignored)
     *
     */
    virtual int StartConditionListening(int aListeningType) = 0;

    /**
     * Starts condition listening
     */
    virtual int StopConditionListening() = 0;

    /**
     * Destructor, to allow deletion through this interface
     */
    virtual ~Sensor() {};
};
#endif // SENSOR_H

// End of file
