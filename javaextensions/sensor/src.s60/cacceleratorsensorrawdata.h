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
* Description:  Accelerometer sensor raw data implementation
 *
*/


#ifndef CACCELERATORSENSORRAWDATA_H
#define CACCELERATORSENSORRAWDATA_H

// INCLUDES

#include "cacceleratorsensor.h"

// CLASS DECLARATION

/**
 *  CAcceleratorSensorRawData
 *
 *  Implementation of the acclerometer sensor which returns raw int values from
 *  the native api
 */
class CAcceleratorSensorRawData : public CAcceleratorSensor
{
public:// Constructors and destructor
    /**
     * Destructor.
     */
    ~CAcceleratorSensorRawData();

    /**
     * Two-phased constructor.
     */
    static CAcceleratorSensorRawData* NewL(const TSensrvChannelInfo& aChannelInfo);

    /**
     * Two-phased constructor.
     */
    static CAcceleratorSensorRawData* NewLC(const TSensrvChannelInfo& aChannelInfo);
private:
    /**
     * Constructor for performing 1st stage construction
     */
    CAcceleratorSensorRawData(const TSensrvChannelInfo& aChannelInfo);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

public: // from CSensorBase
    virtual CSensorBase* DuplicateL();
    virtual TReal InterpretValue(TReal aValue);

protected: // From CAcceleratorSensor
    virtual void Model(TPtr* aPtr);
    virtual void DataType(TPtr* aPtr);
    virtual void DataUnit(const RSensrvPropertyList aList, TPtr* aPtr);
    virtual void DataRange(const RSensrvPropertyList aList,
                           TPtr* aPtr);
    virtual void CopyData(const TSensrvAccelerometerAxisData aData,
                          TInt aIndex);
    virtual TInt SensorDescriptionId();

public:    // From MSensrvDataListener
    virtual void DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost);
};

#endif // CACCELERATORSENSORRAWDATA_H

