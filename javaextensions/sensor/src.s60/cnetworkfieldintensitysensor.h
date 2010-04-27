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
* Description:  Network field intensity sensor implementation
*
*/

#ifndef CNETWORKFIELDINTENSITYSENSOR_H
#define CNETWORKFIELDINTENSITYSENSOR_H

#include <etel3rdparty.h>
#include "csensorbase.h"

class CNetworkFieldIntensitySensor : public CSensorBase
{
    // Asynchronous query helper class
    class CSignalAsyncHandler : public CActive
    {
    public:
        enum TOperation
        {
            EOperationNone = 0,
            EGetSignalIntensity,
            ENotifySignalIntensityChange
        };

    public:
        static CSignalAsyncHandler* NewL(CNetworkFieldIntensitySensor& aSensor);
        virtual ~CSignalAsyncHandler();

    protected:
        CSignalAsyncHandler(CNetworkFieldIntensitySensor& aSensor);
        void ConstructL();

    public: // From CActive
        void RunL();
        void DoCancel();

    public: // New methods
        void SetDataListeningMode(TBool aIsDataListening);
        void SetConditionListeningMode(TBool aIsConditionListening);

    private: // New Methods
        void Start();

    private: // Members
        CNetworkFieldIntensitySensor& iSensor;

        CTelephony* iTelephony;
        CTelephony::TSignalStrengthV1 iSigStrengthV1;
        CTelephony::TSignalStrengthV1Pckg iSigStrengthV1Pckg;
        CSignalAsyncHandler::TOperation iCurrentOperation;

        /**
         * State variables
         */
        TBool iDataListening;
        TBool iConditionListening;

        /**
         * Used to evaluate whether to execute condition evaluation
         * when conditions are set and data listening is ongoing.
         */
        TInt iPrevSignalStrength;
    };

public:
    /**
     * Creates new CNetworkFieldIntensitySensor
     */
    static CNetworkFieldIntensitySensor* NewL();

    /**
     * Destructor
     */
    virtual ~CNetworkFieldIntensitySensor();

protected:

    /**
     * Symbian second phase constructor
     */
    void ConstructL();

    /**
     * Constructor
     */
    CNetworkFieldIntensitySensor();

public: // From Sensor

    virtual int OpenChannel(SensorListener* aListener);

public: // From CSensorBase

    virtual    HBufC* CreateDescriptionLC();
    virtual CSensorBase* DuplicateL();
    virtual TReal InterpretValue(TReal aValue);

    virtual void StartDataListeningL();
    virtual void CancelDataListeningL();
    virtual void CloseChannelL();
    virtual void StartConditionListeningL();
    virtual void StopConditionListeningL();

public: // New methods
    /**
     * Reset data object and those member variables that are
     * involved in data accumulation round.
     */
    void ResetAccumulationData();

    /**
     * Used by AccumulateDataL to send data to Java side and do
     * necessary cleanup.
     */
    void SendData();

    /**
     * Callback method called by RunL of iSignalRequest
     */
    void SignalLevelReceivedL(TInt aLevel);

    /**
     * Callback method called by RunL of iNotifier
     */
    void SignalLevelChangedL(TInt aLevel);

private: // Member data
    /**
     * Used to request notifications about signal level changes
     */
    CSignalAsyncHandler* iNotifier;

    /**
     * Values used in data accumulation
     */
    TTime iStartTime;
    TInt iBufferPosition;

};

#endif // CNETWORKFIELDINTENSITYSENSOR_H
