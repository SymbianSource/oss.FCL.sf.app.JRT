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
* Description:  Base class for Publish & Subscripe -type Sensors
 *
*/

#ifndef CPSSENSORBASE_H
#define CPSSENSORBASE_H

#include <e32property.h> // Publish & Subscribe
#include "csensorbase.h"

// Forward declarations
class CPSSensorBaseAsyncQuery;

class CPSSensorBase : public CSensorBase
{
    // Asynchronous query helper class
    class CPSSensorBaseAsyncQuery : public CActive
    {
    public:
        static CPSSensorBaseAsyncQuery* NewL(CPSSensorBase& aSensorBase,
                                             TUid aPropertyCategory, TInt aPropertyKey);
        virtual ~CPSSensorBaseAsyncQuery();

    protected:
        void ConstructL(TUid aPropertyCategory, TInt aPropertyKey);
        CPSSensorBaseAsyncQuery(CPSSensorBase& aSensorBase);

    public:    // From CActive
        void RunL();
        void DoCancel();

    public:    // New methods
        void NotifyOnChange();

    private: // Members
        CPSSensorBase& iSensorBase;

        /**
         * Property handle used to query property changes
         */
        RProperty iProperty;
    };

    // Poll timer helper class
    class CPSSensorDataListenTimer : public CTimer
    {
    public:
        static CPSSensorDataListenTimer* NewL(CPSSensorBase& iSensorBase);
        virtual ~CPSSensorDataListenTimer();

    protected:
        CPSSensorDataListenTimer(CPSSensorBase& iSensorBase);

    public:    // from CTimer
        void RunL();
        void DoCancel();

    public:    // Members
        CPSSensorBase& iSensorBase;
    };

public:
    // Destructor
    virtual ~CPSSensorBase();

protected:
    // Second phase constructor
    void ConstructL(TUid aPropertyCategory, TInt aPropertyKey);

    // Constructor
    CPSSensorBase(TTimeIntervalMicroSeconds32 aPollInterval);

public:    // From Sensor

    virtual int OpenChannel(SensorListener* aListener);


public:    // From CSensorBase
    virtual TReal InterpretValue(TReal aValue) = 0;
    TBool EvaluateConditions(TInt aValue, TInt aChannelId);

    void AddConditionL(void** aHandle, int aChannelId, double aLowerLimit,
                       double aUpperLimit, int aLowerOp, int aUpperOp);

    void StartDataListeningL();
    void StartConditionListeningL();
    void CancelDataListeningL();
    void CloseChannelL();
    void StopConditionListeningL();

private: // New methods

    /**
     * Leaving version of OpenChannel
     */
    void OpenChannelL(SensorListener* aListener);
    /**
     * Reset data object and those member variables that are
     * involved in data accumulation round.
     */
    void ResetAccumulationData();

    /**
     * Callback method called by RunL of the poll timer.
     */
    void AccumulateDataL();

    /**
     * Used by AccumulateDataL to send data to Java side and do
     * necessary cleanup.
     */
    void SendData();

    /**
     * Called by RunL of iAsyncQuery when query is complete
     */
    void PropertyValueChanged();

private: // Members

    /**
     * Property values
     */
    TUid iPropertyCategory;
    TInt iPropertyKey;
    /**
     * Async query handler used to get property value change
     * notifications
     */
    CPSSensorBaseAsyncQuery* iAsyncQuery;

    /**
     * Used to get property values periodically.
     */
    CTimer* iPollTimer;

    /**
     * Property handle used to get property value
     */
    RProperty iProperty;

    /**
     * Beat interval used in periodic polling
     */
    TTimeIntervalMicroSeconds32 iPollInterval;

    /**
     * Values used in asynchronous polling
     */
    TTime iStartTime;
    TInt iBufferPosition;
};

#endif // CPSSENSORBASE_H
