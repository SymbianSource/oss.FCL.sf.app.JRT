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
#include "sensorconnectionlist.h"
#include "sensorconnection.h"
#include "logger.h"

// OS SPECIFIC INCLUDE
#include "csensorfinder.h"

SensorConnectionList::SensorConnectionList() :
        iSensors(NULL)
{
    JELOG2(ESensor);
    iFinder = new CSensorFinder();
}

SensorConnectionList::~SensorConnectionList()
{
    JELOG2(ESensor);
    delete iFinder;
    // Delete all connections
    SensorList* list = iSensors;
    while (list && list->iNext)
    {
        delete list->iSensorConnection;
        SensorList* next = list->iNext;
        delete list;
        list = next;
    }
    delete list;
}

unsigned short** SensorConnectionList::InitSensors(int* aSensorCount)
{
    JELOG2(ESensor);
    return iFinder->FindSensors(aSensorCount);
}

SensorConnection* SensorConnectionList::CreateSensorConnection(int aSensorIndex)
{
    JELOG2(ESensor);
    if (!iSensors)
    {
        iSensors = new SensorList();
        iSensors->iNext = NULL;
        iSensors->iSensorConnection = NULL;
    }
    SensorList* senList = iSensors;
    // Find last
    while (senList->iNext)
    {
        senList = senList->iNext;
    }
    // Create next object to list
    SensorList* next = new SensorList();
    if (!next)
    {
        return NULL;
    }
    next->iNext = NULL;
    next->iSensorConnection = NULL;

    //  Create actual sensor
    Sensor* sensor = iFinder->CreateSensor(aSensorIndex);
    if (!sensor)
    {
        delete next;
        return NULL;
    }

    // Create sensor connection, and assign values to current list item
    senList->iSensorConnection = new SensorConnection(sensor);
    if (!senList->iSensorConnection)
    {
        delete sensor;
        delete next;
        return NULL;
    }
    senList->iNext = next;
    return senList->iSensorConnection;
}

void SensorConnectionList::RemoveConnection(SensorConnection* aConnection)
{
    JELOG2(ESensor);
    SensorList* currP = iSensors;
    SensorList* prevP = NULL;
    for (; currP != NULL; prevP = currP, currP = currP->iNext)
    {
        if (currP->iSensorConnection == aConnection)
        {
            if (prevP == NULL)
            {
                iSensors = currP->iNext;
            }
            else
            {
                prevP->iNext = currP->iNext;
            }
            delete currP->iSensorConnection; // Delete connection
            delete currP; // Delete node
            return;
        }
    }
}

// End of file
