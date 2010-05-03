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
* Description:  Accelerometer sensor implementation
*
*/


#ifndef CSENSORPROPERTIES_H
#define CSENSORPROPERTIES_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvgeneralproperties.h>
#include <sensrvtypes.h>

// CONSTANTS
//local data types
struct TNameKey
{
    const TText* iName;
    TInt iKey;
};

LOCAL_D TNameKey const KQuantity[] =
{
    {_S("not_defined"), ESensrvQuantityNotdefined },
    {_S("acceleration"), ESensrvQuantityAcceleration },
    {_S("com.tapping"), ESensrvQuantityTapping }, // not in standard
    {_S("direction"), ESensrvQuantityOrientation }, //?
    {_S("angular_velocity"), ESensrvQuantityRotation }, //?
    {_S("magnetic_field_strength"), ESensrvQuantityMagnetic },
    {_S("angle"), ESensrvQuantityAngle },
    {_S("proximity"), ESensrvQuantityProximity },
    { 0, 0 }
};


LOCAL_D TNameKey const KContextType[] =
{
    {_S("not_defined"), ESensrvContextTypeNotDefined },
    {_S("ambient"), ESensrvContextTypeAmbient },
    {_S("device"), ESensrvContextTypeDevice },
    {_S("user"), ESensrvContextTypeUser },
    { 0, 0 }
};

LOCAL_D TNameKey const KConnectionType[] =
{
    { _S("connectiontype=0"), ESensrvConnectionTypeNotDefined },
    { _S("connectiontype=1"), ESensrvConnectionTypeEmbedded },
    { _S("connectiontype=8"), ESensrvConnectionTypeWired},
    { _S("connectiontype=4"), ESensrvConnectionTypeWireless},
    { 0, 0 }
};


LOCAL_D TNameKey const KChannelUnit[] =
{
    {_S("none"), ESensrvChannelUnitNotDefined },
    {_S("G"), ESensrvChannelUnitGravityConstant },
    {_S("m/s^2"), ESensevChannelUnitAcceleration }, // typo in native api
    { 0, 0 }
};

LOCAL_D TNameKey const KAcceleratorChannelName[] =
{
    {_S("axis_x"), TSensrvAccelerometerAxisData::EAxisX },
    {_S("axis_y"), TSensrvAccelerometerAxisData::EAxisY },
    {_S("axis_z"), TSensrvAccelerometerAxisData::EAxisZ },
    { 0, 0 }
};

const TInt KOFNChannelPointX = 0;
const TInt KOFNChannelPointY = 1;

LOCAL_D TNameKey const KOFNChannelName[] =
{
    {_S("point_x"), KOFNChannelPointX },
    {_S("point_y"), KOFNChannelPointY },
    { 0, 0 }
};

enum TSensorDescriptionId
{
    EAccelerometerSensor  = 0,
    EAccelerometerSensorRawData,
    EBatteryChargeSensor,
    EChargerStateSensor,
    ENetworkFieldIntensitySensor,
    EOFNSensor
};

LOCAL_D TNameKey const KSensorDescription[] =
{
    {_S("acceleration"), EAccelerometerSensor },
    {_S("acceleration2"), EAccelerometerSensorRawData },
    {_S("battery_charge"), EBatteryChargeSensor },
    {_S("charger_state"), EChargerStateSensor },
    {_S("network_field_intensity"), ENetworkFieldIntensitySensor },
    {_S("ofn sensor has two channels that give OFN input devices x,y point location. Always on"), EOFNSensor },
    { 0, 0 }
};

// CLASS DECLARATION

/**
 *  CCSensorProperties
 *
 */
class SensorProperties
{
public: // new methods
    static const TPtrC GetPropertyString(const TNameKey* aKey, TInt aValue);

};

#endif // CSENSORPROPERTIES_H
