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


#ifndef CACCELERATORSENSOR_H
#define CACCELERATORSENSOR_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvchannel.h>
#include <sensrvdatalistener.h>

#include "csensorbase.h"

// CLASS DECLARATION

/**
 *  Cacceleratorsensor
 *
 */
class CAcceleratorSensor : public CSensorBase, public MSensrvDataListener
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CAcceleratorSensor();

    /**
     * Two-phased constructor.
     */
    static CAcceleratorSensor* NewL(const TSensrvChannelInfo& aChannelInfo);

    /**
     * Two-phased constructor.
     */
    static CAcceleratorSensor* NewLC(const TSensrvChannelInfo& aChannelInfo);

public:    // From Sensor
    int OpenChannel(SensorListener* aListener);

public:    // From CSensorBase
    virtual HBufC* CreateDescriptionLC();
    virtual CSensorBase* DuplicateL();
    virtual TReal InterpretValue(TReal aValue);
    void StartDataListeningL();
    void StartConditionListeningL();
    void CancelDataListeningL();
    void CloseChannelL();
    void StopConditionListeningL();

private: // New methods
    void OpenChannelL();

    void DescriptorAppendChannel(TPtr* aPtr, const RSensrvPropertyList aList,
                                 TInt aItemIndex);
protected:
    void GetProperty(const RSensrvPropertyList aList,
                     const TSensrvPropertyId aPropertyId, const TInt aItemIndex,
                     TSensrvProperty& aProperty);

    void GetProperty(const RSensrvPropertyList aList,
                     const TSensrvPropertyId aPropertyId,
                     const TInt aItemIndex,
                     const TInt aArrayIndex,
                     TSensrvProperty& aProperty);

public:    // From MSensrvDataListener
    virtual void DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost);
    void DataError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError);
    void GetDataListenerInterfaceL(TUid aInterfaceUid, TAny*& aInterface);

protected:
    /**
     * Constructor for performing 1st stage construction
     */
    CAcceleratorSensor(const TSensrvChannelInfo& aChannelInfo);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

protected: // Methods used to differentiate between double value and raw data versions
    virtual void Model(TPtr* aPtr);
    virtual void DataType(TPtr* aPtr);
    virtual void DataUnit(const RSensrvPropertyList aList, TPtr* aPtr);
    virtual void DataRange(const RSensrvPropertyList aList,
                           TPtr* aPtr);
    virtual void CopyData(const TSensrvAccelerometerAxisData aData,
                          TInt aIndex);
    virtual TInt SensorDescriptionId();

protected: // Data

    /**
     * Channel information
     */
    TSensrvChannelInfo iChannelInfo;
    CSensrvChannel* iChannel;

    // Datas per channel
    SensorData* iChannelXData;
    SensorData* iChannelYData;
    SensorData* iChannelZData;

    // Multplier for scaled measurement values
    TReal iScaleFactor;

    // Start time of datalistening
    TTime iStartTime;
};

#endif // CACCELERATORSENSOR_H

