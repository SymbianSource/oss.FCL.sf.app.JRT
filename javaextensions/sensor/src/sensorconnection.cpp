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
* Description:  List of SensorConnections
*
*/


// INTERNAL INCLUDES
#include "sensorconnection.h"
#include "sensor.h"
#include "sensornativeconstants.h"
#include "logger.h"

const int KMax_buffersize = 256;
SensorConnection::SensorConnection(Sensor* aSensor) :
        iSensor(aSensor),
        iParams(0),
        iBufferSize(-1)
{
    JELOG2(ESensor);
}

SensorConnection::~SensorConnection()
{
    JELOG2(ESensor);
    if (iParams)
    {
        ClearData(iParams->iDatas, iParams->iDataCount);
    }
    delete iParams;
    delete iSensor;
}

void SensorConnection::DataReceived(SensorData** aData, bool aIsDataLost)
{
    JELOG2(ESensor);
    DataFillerParams* params = iParams;
    if (params)
    {
        params->iJniEnv = iJniEnv;
        params->iDatas = aData;
        params->iIsDataLost = aIsDataLost;
        iDataReceived(params);
    }
}

void SensorConnection::ConditionMet(void* aHandle, int aChannelId,
                                    double aValue, long aTimeStamp)
{
    JELOG2(ESensor);
    iConditionMet(iJniEnv, iJavaPeer,
                  aHandle, aChannelId, aValue, aTimeStamp);
}

Sensor* SensorConnection::GetSensor()
{
    JELOG2(ESensor);
    return iSensor;
}

int SensorConnection::PrepareDataListening(void* aNewDataObjects,
        int aDataCount,
        void*& aOldDataObject,
        int aDataType)
{
    JELOG2(ESensor);
    if (!iParams)
    {
        iParams = new DataFillerParams();
        if (!iParams)
        {
            return ERROR_NOMEMORY;
        }
        iParams->iDataCount = aDataCount;
        iParams->iJavaPeer = iJavaPeer;
        iParams->iDatas = AllocateData(KMax_buffersize,
                                       aDataType);
        if (!iParams->iDatas)
        {
            delete iParams;
            iParams = 0;
            return ERROR_NOMEMORY;
        }
    }
    aOldDataObject = iParams->iDataObjects;
    iParams->iDataObjects = aNewDataObjects;
    return ERROR_NONE;
}

int SensorConnection::StartDataListening(int aBufferSize,
        long aBufferingPeriod,
        bool aTimestampsIncluded,
        bool aValiditiesIncluded,
        bool aIsOneShot)
{
    JELOG2(ESensor);
    for (int i = 0; i < iParams->iDataCount; i++)
    {
        SensorData* data = iParams->iDatas[ i ];

        data->iBufferSize = aBufferSize;
        data->iTimeStampsIncluded = aTimestampsIncluded;
        data->iValiditiesIncluded = aValiditiesIncluded;
    }
    iSensor->StartDataListening(iParams->iDatas, aBufferSize, aBufferingPeriod,
                                aTimestampsIncluded, aValiditiesIncluded,
                                aIsOneShot);
    return ERROR_NONE;
}

void* SensorConnection::DataObjects()
{
    JELOG2(ESensor);
    void* dataObjects = 0;
    if (iParams)
    {
        dataObjects = iParams->iDataObjects;
    }
    return dataObjects;
}

SensorData** SensorConnection::AllocateData(int aBufferSize,
        int aDataType)
{
    JELOG2(ESensor);
    SensorData** senData = new SensorData*[ iParams->iDataCount ];
    if (!senData)
    {
        return 0;
    }
    // Null all datas before continuing
    for (int i = 0; i < iParams->iDataCount; i++)
    {
        senData[ i ] = 0;
    }
    // create data containers
    for (int i = 0; i < iParams->iDataCount; i++)
    {
        senData[ i ] = new SensorData();
        if (!senData[ i ])
        {
            ClearData(senData, iParams->iDataCount);
            return 0;
        }
        // Data allocation
        if (aDataType == 2)  // Type 2 for INT
        {
            senData[ i ]->iIntValues = new int[ aBufferSize ];
            if (!senData[ i ]->iIntValues)
            {
                ClearData(senData, iParams->iDataCount);
                return 0;
            }
        }
        else
        {
            senData[ i ]->iDoubleValues = new double[ aBufferSize ];
            if (!senData[ i ]->iDoubleValues)
            {
                ClearData(senData, iParams->iDataCount);
                return 0;
            }
        }
        // validities
        senData[ i ]->iValidities = new bool[ aBufferSize ];
        if (!senData[ i ]->iValidities)
        {
            ClearData(senData, iParams->iDataCount);
            return 0;
        }
        // fill with trues by defaults
        for (int j = 0; j < aBufferSize; j++)
        {
            senData[ i ]->iValidities[ j ] = true;
        }

        // Time stamps
        senData[ i ]->iTimeStamps = new long long[ aBufferSize ];
        if (!senData[ i ]->iTimeStamps)
        {
            ClearData(senData, iParams->iDataCount);
            return 0;
        }
    }
    return senData;
}

void SensorConnection::ClearData(SensorData** aDatas, int aCount)
{
    JELOG2(ESensor);
    if (!aDatas)
    {
        return;
    }

    for (int i = 0; i < aCount; i++)
    {
        delete aDatas[ i ];
    }
    delete [] aDatas;
}
// End of file
