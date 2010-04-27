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
* Description:  Factory for sensors
*
*/


// INTERNAL INCLUDES
#include "csensorfinder.h"
#include "cpsbatterychargesensor.h"
#include "cpschargerstatesensor.h"
#include "cacceleratorsensor.h"
#include "cnetworkfieldintensitysensor.h"
#include "cacceleratorsensorrawdata.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <stdlib.h>
#include <string.h>
#include <sensrvchannelfinder.h>



CSensorFinder::CSensorFinder()
{
    JELOG2(ESensor);
}

CSensorFinder::~CSensorFinder()
{
    JELOG2(ESensor);
    iSensors.ResetAndDestroy();
}

unsigned short** CSensorFinder::FindSensors(int* aSensorCount)
{
    JELOG2(ESensor);
    unsigned short** result = NULL;
    TRAPD(err,
          FindSensorsL();
          TInt sensorCount = iSensors.Count();
          if (sensorCount > 0)
{
    result = new unsigned short*[ sensorCount ];
        User::LeaveIfNull(result);
    }

    for (TInt i = 0; i < sensorCount; i++)
{
    HBufC* description = iSensors[ i ]->CreateDescriptionLC();

        // one slot for terminator
        TInt resultStringLength = description->Length() + 1;
        result[ i ] = new unsigned short[ resultStringLength ];
        User::LeaveIfNull(result[ i ]);
        TPtr resPtr((TUint16*)result[ i ], resultStringLength);
        resPtr.Copy(*description);
        resPtr.Append('\0');
        CleanupStack::PopAndDestroy();
    }
         );
    if (err != KErrNone)
    {
        delete[] result;
        result = NULL;
    }
    *aSensorCount = iSensors.Count();
    return result;
}

Sensor* CSensorFinder::CreateSensor(int aIndex)
{
    JELOG2(ESensor);
    CSensorBase* base = NULL;
    TRAP_IGNORE(base = iSensors[ aIndex ]->DuplicateL());
    return base;
}

void CSensorFinder::AddCustomSensorsL()
{
    JELOG2(ESensor);
    CPSBatteryChargeSensor* battery_charge = CPSBatteryChargeSensor::NewL();
    CleanupStack::PushL(battery_charge);
    iSensors.AppendL(battery_charge);
    CleanupStack::Pop(); // self

    CPSChargerStateSensor* charger_state = CPSChargerStateSensor::NewL();
    CleanupStack::PushL(charger_state);
    iSensors.AppendL(charger_state);
    CleanupStack::Pop(); // self

    CNetworkFieldIntensitySensor* network_field_intensity =
        CNetworkFieldIntensitySensor::NewL();
    CleanupStack::PushL(network_field_intensity);
    iSensors.AppendL(network_field_intensity);
    CleanupStack::Pop(); // self
}


void CSensorFinder::FindSensorsL()
{
    JELOG2(ESensor);
    if (iSensors.Count() == 0)
    {
        //Construct a channel finder.
        if (!(CActiveScheduler::Current()))
        {
            CActiveScheduler::Install(new(ELeave)CActiveScheduler());
        }
        CSensrvChannelFinder* channelFinder;
        channelFinder = CSensrvChannelFinder::NewL();
        CleanupStack::PushL(channelFinder);

        //List of found channels.
        RSensrvChannelInfoList channelInfoList;
        CleanupClosePushL(channelInfoList);

        //Create and fill channel search criteria.
        TSensrvChannelInfo accelerationChannelInfo;
        accelerationChannelInfo.iQuantity = ESensrvQuantityAcceleration;

        //Find the channel for acceleration sensor
        channelFinder->FindChannelsL(channelInfoList, accelerationChannelInfo);



        TInt sensorCount = channelInfoList.Count();
        for (TInt i = 0; i < sensorCount; i++)
        {

            CAcceleratorSensor* sensor = CAcceleratorSensor::NewLC(channelInfoList[i]);
            iSensors.AppendL(sensor);
            CleanupStack::Pop();

            CAcceleratorSensorRawData* sensor2 = CAcceleratorSensorRawData::NewLC(channelInfoList[i]);
            iSensors.AppendL(sensor2);
            CleanupStack::Pop();

        }
        CleanupStack::PopAndDestroy(2);
        AddCustomSensorsL();
    }
}


// End of file
