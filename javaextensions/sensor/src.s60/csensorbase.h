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
* Description:  Base class for Sensors
*
*/


#ifndef CSENSORBASE_H
#define CSENSORBASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "sensor.h"

// FORWARD DECLARATIONS
class SensorListener;
class CSensorConditionBase;

// CONSTANTS
const TInt KMaxSensorDescriptionLength = 4096;

// CLASS DECLARATION
/**
 *  CSensorBase
 *
 */
class CSensorBase : public Sensor, public CActive
{
    class TMethod
    {
    public: // Enumerations
        enum TMethodType
        {
            ENone,
            EStartDataListening,
            ECancelDataListening,
            EStartConditionListening,
            EStopConditionListening,
            ECloseChannel,
            EStopServer,
            ERemoveCondition
        };
    public: // Constructor & Destructor
        TMethod(TMethodType aMethodType)
        {
            iMethodType = aMethodType;
        };
        ~TMethod() {};

    public: // New methods
        inline void SetSensorData(SensorData** aData)
        {
            iData = aData;
        };
        inline void SetBufferSize(TInt aBufferSize)
        {
            iBufferSize = aBufferSize;
        };
        inline void SetBufferingPeriod(TInt64 aBufferingPeriod)
        {
            iBufferingPeriod = aBufferingPeriod;
        };
        inline void SetTimestampsIncluded(TBool aTimestampsIncluded)
        {
            iTimestampsIncluded = aTimestampsIncluded;
        };
        inline void SetValiditiesIncluded(TBool aValiditiesIncluded)
        {
            iValiditiesIncluded = aValiditiesIncluded;
        };
        inline void SetIsOneShot(TBool aIsOneShot)
        {
            iIsOneShot = aIsOneShot;
        };
        inline void SetJavaConditionEval(TBool aJavaConditionEval)
        {
            iJavaConditionEval = aJavaConditionEval;
        };
        inline void SetConditionToRemove(void* aCondition)
        {
            iConditionToRemove = aCondition;
        };

        inline SensorData** GetSensorData()
        {
            return iData;
        };
        inline TInt GetBufferSize()
        {
            return iBufferSize;
        };
        inline TInt64 GetBufferingPeriod()
        {
            return iBufferingPeriod;
        };
        inline TBool GetTimestampsIncluded()
        {
            return iTimestampsIncluded;
        };
        inline TBool GetValiditiesIncluded()
        {
            return iValiditiesIncluded;
        };
        inline TBool GetIsOneShot()
        {
            return iIsOneShot;
        };
        inline TInt GetJavaConditionEval()
        {
            return iJavaConditionEval;
        };
        inline void* GetConditionToRemove()
        {
            return iConditionToRemove;
        };
        inline TMethodType GetMethodType()
        {
            return iMethodType;
        };

    public: // Member data
        SensorData** iData;
        TInt iBufferSize;
        TInt64 iBufferingPeriod;
        TBool iTimestampsIncluded;
        TBool iValiditiesIncluded;
        TBool iIsOneShot;
        TInt iJavaConditionEval;
        void* iConditionToRemove;
        TMethodType iMethodType;
    };

public:
    /**
     * Destructor
     */
    virtual ~CSensorBase();

    /**
     * Constructor
     */
    CSensorBase();

public: // From Sensor
    virtual void StartServer();
    virtual void StopServer();
    virtual int AddCondition(
        void** aHandle,
        int aChannelId,
        double aLowerLimit,
        double aUpperLimit,
        int aLowerOp,
        int aUpperOp);

    virtual int RemoveCondition(
        void* aHandle);

    virtual int StartDataListening(SensorData** aData,
                                   int aBufferSize,
                                   long aBufferingPeriod,
                                   bool aTimestampsIncluded,
                                   bool aValiditiesIncluded,
                                   bool aIsOneShot);
    virtual int CancelDataListening();

    virtual int StartConditionListening(int aListeningType);

    virtual int StopConditionListening();

    virtual void CloseChannel();


public: // From CActive
    void RunL();
    void DoCancel();

public: // New methods
    /**
     * Creates description string for this sensor
     * @return new description buffer
     */
    virtual    HBufC* CreateDescriptionLC() = 0;

    /**
     * Creates duplicate of this class
     * This method is used when new instance of specified sensor is needed
     */
    virtual CSensorBase* DuplicateL() = 0;

    /**
     * Start data listening
     */
    virtual void StartDataListeningL() = 0;

    /**
     * Cancel data listening
     */
    virtual void CancelDataListeningL() = 0;

    /**
     * Start condition listening
     */
    virtual void StartConditionListeningL() = 0;

    /**
     * Stop condition listening
     */
    virtual void StopConditionListeningL() = 0;

    /**
     * Close the channel
     */
    virtual void CloseChannelL() = 0;

protected: // Second phase construction

    virtual void ConstructL();

protected: // New methods
    /**
     * Creates timestamp for specified time
     * @param Time to convert
     * @return new timestamp value pointing for specified time
     */
    TInt64 TimeStamp(const TTime& aTime);

    /*
     * Interpret the value obtained from native source in terms
     * Java Sensor API
     * @param aValue value obtained from native source
     * @return int value in terms of this specific sensor for Java
     */
    virtual TReal InterpretValue(TReal aValue) = 0;

    /**
     * Evaluate currently set conditions
     * @param aValue sensor value which is evaluated against
     * conditions
     * @param aChannelId ID number of the channel the value
     * comes from
     * @return true if one or more conditions matched,
     *            false if none matched
     */
    TBool EvaluateConditions(TReal aValue, TInt aChannelId);

    /**
     * Add native condition
     * @param aHandle Native handle to created condition object.
     * Filled by this method.
     * @param aChannelId Number of the channel this condition
     * is interested in
     * @param aLowerLimit Lower limit of range condition or limit
     * of limit condition
     * @param aUpperLimit Upper limit of range condition or -1
     * for limit condition
     * @param aLowerOp Lower operand of range condition or operand
     * of limit condition
     * @param aUpperOp Upper operand of range condition or -1
     * for limit condition
     */
    void AddConditionL(
        void** aHandle,
        int aChannelId,
        double aLowerLimit,
        double aUpperLimit,
        int aLowerOp,
        int aUpperOp);

    /**
     * Remove native condition
     * @param aHandle Handle of the native condition object
     */
    void RemoveConditionL(
        void* aHandle);

private: // New methods
    /**
     * Make context switch to running thread
     */
    void AsyncCallback(TMethod aMethod);

protected: // Data

    /**
     * Listener, not owned
     */
    SensorListener* iSensorListener;

    RPointerArray< CSensorConditionBase > iConditions;

    // active scheduler for this thread
    CActiveScheduler* iActiveScheduler;

    // Data to be filled
    SensorData** iData;

    TInt iBufferSize;
    // Buffering period in microseconds
    TInt64 iBufferingPeriod;
    // Flag indicating oneshot getData call
    TBool iIsOneShot;

    TBool iTimestampsIncluded;
    TBool iValiditiesIncluded;

    TInt iListeningType;
    TInt iJavaConditionEval;

    RThread iRunnerThread;

    RArray< TMethod > iMethodArray;

};

#endif // CSENSORBASE_H
