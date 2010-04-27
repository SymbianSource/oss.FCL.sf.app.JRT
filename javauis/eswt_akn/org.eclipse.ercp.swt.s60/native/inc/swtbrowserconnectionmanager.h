/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTBROWSERCONNECTIONMANAGER_H
#define SWTBROWSERCONNECTIONMANAGER_H


#include <etelmm.h>
#include "browserconnection.h"
#include "swtbrowserconnectionobserver.h"


class CCommsDatabase;
class CSwtBrowserConnectorSyncWrapper;
class TCommDbConnPref;
class MBrowserApiProvider;
class CRepository;
class CApAccessPointItem;


/**
 * CSwtBrowserConnectionManager
 * This class is responsible for the logic that lies in connection handling.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserConnectionManager)
        : public CBase
        , public MBrowserConnection
        , public MConnectionStageObserver
{
public:
    /**
     * Constructor.
     * @param aCommsModel Comms model
     * @param aApiProvider Api provider.
     * @return The constructed browser connection manager object
     */
    CSwtBrowserConnectionManager(CCommsDatabase* aCommsDb,
    MBrowserApiProvider& aApiProvider);

    /**
     * Use this function to stop the connection
     */
    void StopConnection();

    /**
     * Destructor
     */
    virtual ~CSwtBrowserConnectionManager();

private:
    enum TWaitIconType { EWaitNoIcon, EWaitDataIcon, EWaitGPRSIcon };

    /**
     * Check if there is a connection to
     * iRequestedAPIds.iFirstPreference
     * @param aNewIapId contains connected IAP Id
     * when this function returns true.
     * @return ETrue if there is a connection to
     * iRequestedAPIds.iFirstPreference, otherwise EFalse
     */
    TBool CheckIfAlreadyConnected(TUint32& aNewIapId);

    /**
     * Does the connection
     * It does not display any errors
     * @param aPref1 the first preference to try
     * @param aPref2 the second preference to try
     * @return KErrNone or KErrGeneral
     */
    TInt DoConnect(TUint32 aIAPId1);

    /**
     * Update connection name
     * Do not call it if there is not a valid connection.
     */
    void UpdateConnectionNameL();

    /**
     * Update connection name and start connection observer
     * Do not call it if there is not a valid connection.
     */
    void UpdateNameAndStartObserverL();

    /**
     * Check network availability
     * @param aGPRSAvailable on return it is ETrue if there is GPRS coverage
     * @return EFalse if there is no network
     */
    TBool CheckNetwork(TBool& aGPRSAvailable);

    /**
     * Return the bearer type of the given AP.
     * @param aIAPId the IAPId
     */
    TInt BearerTypeL(TUint32 aIAPId);

    /**
     * Check if connnection is allowed.
     * @return ETrue if the connection is allowed EFalse otherwise.
     */
    TBool IsConnectionAllowed();


public:
// from MBrowserConnection
    void SetRequestedAP(TUint32 aRequestedAPId);
    TBool Connected();
    TUint32 CurrentAPId() const;
    TInt CurrentBearerType() const;
    void Disconnect();
    TInt StartConnectionL();
    RConnection& Connection();
    TName* ConnectionNameL();
    RSocketServ& SocketServer();
    TBool Check3GNetwork();
    TBool IsThereActiveHSCSDConnection();
    TBool IsThereActiveVoiceCall();

protected:
// From MConnectionStageObserver
    void ConnectionStageAchievedL();
    void StartConnectionObservingL();
    void StopConnectionObserving();
    void UpdateEasyWlanFlag();
    void InitializeL();

private: //Data

    /**
     * Requested AP pair
     */
    TUint32 iRequestedAPId;

    /**
     * Flag for current connection status
     */
    TBool iConnected;

    /**
     * Flag to indicate if the connection manager has been initialized
     */
    TBool iInitialized;

    /**
     * Flag to indicate if easy wlan is in use.
     */
    TBool iEasyWlan;

    /**
     * Flag to indicate if a new connection has been established.
     */
    TBool iNewConnectionEstablished;

    /**
     * Flag to indicate if the connection service is alrady in use.
     */
    TBool iAlreadyInUse;

    /**
     * Internal error Id.
     */
    TInt  iInternalError;

    /**
     * Socket server.
     */
    RSocketServ iServ;

    /**
     * Current connection.
     */
    RConnection iConnection;

    /**
     * Tel server
     */
    RTelServer iTelServer;

    /**
     * Mobile phone service to access to mobile phone functionality
     */
    RMobilePhone iMobilePhone;

    /**
     * Repository to retrieve network info
     * Own
     */
    CRepository* iRepository;

    /**
     * Current AP
     * Not own
     */
    CApAccessPointItem* iCurrentAP;

    /**
     * Comm database
     * Not own
     */
    CCommsDatabase* iCommsDb;

    /**
     * Connection observer
     * Own
     */
    CSwtBrowserConnectionObserver* iStageNotifier;

    /**
     * Connection name
     * Own
     */
    HBufC* iConnName;

    /**
     * Network connector
     * Own
     */
    CSwtBrowserConnectorSyncWrapper* iSyncConnector;

    /**
     * Requested Ap
     * Not own
     */
    MBrowserApiProvider& iApiProvider;
};

#endif // SWTBROWSERCONNECTIONMANAGER_H
