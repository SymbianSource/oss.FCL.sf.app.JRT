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


#include <commdbconnpref.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <es_enum.h>
#include <cdbcols.h>
#include "swtbrowserconnectionmanager.h"
#include "swtbrowserconnector.h"
#include "swtbrowserconnectionobserver.h"
#include "browserapiprovider.h"
#include "swtbrowseraputils.h"
#include "swtlaffacade.h"


_LIT(KMmTsyModuleName, "PhoneTsy");


// ======== MEMBER FUNCTIONS ========


CSwtBrowserConnectionManager::CSwtBrowserConnectionManager(CCommsDatabase* aCommsDb,
        MBrowserApiProvider& aApiProvider)
        : iRequestedAPId(0)
        , iAlreadyInUse(EFalse)
        , iCommsDb(aCommsDb)
        , iApiProvider(aApiProvider)
{
}

CSwtBrowserConnectionManager::~CSwtBrowserConnectionManager()
{
    Disconnect();

    delete iStageNotifier;

    iMobilePhone.Close();
    iTelServer.Close();

    delete iRepository;

    if (iConnected)
    {
        iConnection.Close();
    }

    delete iConnName;
    delete iSyncConnector;
    iServ.Close();
}

void CSwtBrowserConnectionManager::InitializeL()
{
    if (iInitialized)
    {
        return;
    }

    if (!iSyncConnector)
    {
        iSyncConnector = CSwtBrowserConnectorSyncWrapper::NewL(iConnection);
    }

    if (!iRepository)
    {
        iRepository = CRepository::NewL(KCRUidCoreApplicationUIs);
    }

    if (!iTelServer.Handle())
    {
        // this initialization needed for identifying 3G networks
        User::LeaveIfError(iTelServer.Connect());
        User::LeaveIfError(iTelServer.LoadPhoneModule(KMmTsyModuleName));

        TInt numPhones;
        User::LeaveIfError(iTelServer.EnumeratePhones(numPhones));
        if (numPhones <= 0)
        {
            User::Leave(KErrCorrupt);
        }

        RTelServer::TPhoneInfo phoneInfo;
        User::LeaveIfError(iTelServer.GetPhoneInfo(0, phoneInfo));
        User::LeaveIfError(iMobilePhone.Open(iTelServer, phoneInfo.iName));
        User::LeaveIfError(iMobilePhone.Initialise());
    }
    if (!iStageNotifier)
    {
        iStageNotifier = new(ELeave) CSwtBrowserConnectionObserver();
    }

    if (!iServ.Handle())
    {
        User::LeaveIfError(iServ.Connect());
    }

    iInitialized = ETrue;
}

void CSwtBrowserConnectionManager::UpdateConnectionNameL()
{
    // Enable connection cloning
    _LIT_SECURITY_POLICY_C2(KProcPolicy1,
                            ECapabilityNetworkServices, ECapabilityNetworkControl);
    TSecurityPolicyBuf secPolBuf;
    secPolBuf().Set(KProcPolicy1().Package());
    iConnection.Control(KCOLConnection, KCoEnableCloneOpen, secPolBuf);

    // Obtain connnection name
    const TInt KQueryLen(20);
    const TInt KValLen(40);
    TUint32 iapId;
    TBuf< KQueryLen > query;
    TBuf< KValLen > val;
    _LIT(KFormatStr, "%s\\%s");

    query.Format(KFormatStr, IAP, COMMDB_ID);
    User::LeaveIfError(iConnection.GetIntSetting(query, iapId));
    iCurrentAP = reinterpret_cast< CApAccessPointItem* >(iapId);

    query.Format(KFormatStr, IAP, COMMDB_NAME);
    User::LeaveIfError(iConnection.GetDesSetting(query, val));

    delete iConnName;
    iConnName = NULL;
    iConnName = val.AllocL();
}

void CSwtBrowserConnectionManager::UpdateNameAndStartObserverL()
{
    UpdateConnectionNameL();
    StartConnectionObservingL();
}

TInt CSwtBrowserConnectionManager::DoConnect(TUint32 aIAPId)
{
    if (iAlreadyInUse)
    {
        ASSERT(EFalse);
    }
    iAlreadyInUse = ETrue;

    iInternalError = KErrNone;
    TInt connErr(KErrNone);
    // Check GPRS coverage
    TCommDbConnPref pref;
    TBool gprsCovered(ETrue);
    if (!CheckNetwork(gprsCovered))
    {
        // there is no coverage
        connErr = KErrGeneral;
    }
    else //there is coverage
    {
        TInt  bearerType(EApBearerTypeAllBearers);

        if (!iRequestedAPId)
        {
            // no ApId defined, a connection dialog will come
            pref.SetDialogPreference(ECommDbDialogPrefPrompt);
        }
        else
        {
            // will use defined ApId, check its bearer type
            // pref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
            pref.SetDialogPreference(ECommDbDialogPrefPrompt);

            TRAPD(err, bearerType = BearerTypeL(aIAPId));
            if (err != KErrNone)
            {
                bearerType = EApBearerTypeAllBearers;
            }
        }

        // set bearer type accordingly
        if (bearerType == EApBearerTypeGPRS)
        {
            pref.SetBearerSet(ECommDbBearerGPRS);
        }
        else if (bearerType == EApBearerTypeCSD)
        {
            pref.SetBearerSet(ECommDbBearerCSD);
        }
        else if (bearerType == EApBearerTypeWLAN)
        {
            pref.SetBearerSet(ECommDbBearerWLAN);
        }
        else if (bearerType == EApBearerTypeAllBearers)
        {
            pref.SetBearerSet(
                ECommDbBearerGPRS |
                ECommDbBearerCSD |
                ECommDbBearerPSD |
                ECommDbBearerLAN |
                ECommDbBearerVirtual |
                ECommDbBearerPAN |
                ECommDbBearerWLAN);
        }

        if (!connErr)
        {
            // open the connection
            connErr = iConnection.Open(iServ, KAfInet);
            if (connErr == KErrNone)
            {
                // Connect to
                connErr = iSyncConnector->Connect(&pref);
            }
            else
            {
                // close connection if something is wrong
                iConnection.Close();
            }
        }
    }

    if (connErr == KErrNone)
    {
        iNewConnectionEstablished = ETrue;
        iConnected = ETrue;
    }
    iInternalError = connErr;
    iAlreadyInUse = EFalse;
    return connErr;
}

TBool CSwtBrowserConnectionManager::CheckNetwork(TBool& aGPRSAvailable)
{
    aGPRSAvailable = ETrue;
    return ETrue;
}

TInt CSwtBrowserConnectionManager::BearerTypeL(TUint32 aIapId)
{
    if (!iRequestedAPId)
    {
        return EApBearerTypeAllBearers;
    }

#ifndef __WINS__
    CSwtBrowserApUtils* apUtils = CSwtBrowserApUtils::NewL(*iCommsDb);
    CleanupStack::PushL(apUtils);
    TUint32 wapId = apUtils->WapIdFromIapIdL(aIapId);
    TInt bearerType = apUtils->BearerTypeL(wapId);
    CleanupStack::PopAndDestroy(apUtils);
    return bearerType;
#else
    aIapId = 0;
    return EApBearerTypeGPRS;
#endif
}

void CSwtBrowserConnectionManager::StopConnection()
{
    StopConnectionObserving();
    iConnection.Close();
    iConnected = EFalse;
    iEasyWlan = EFalse;
    iCurrentAP = NULL;
}

RConnection& CSwtBrowserConnectionManager::Connection()
{
    return iConnection;
}

TName* CSwtBrowserConnectionManager::ConnectionNameL()
{
    if (!iConnected)
    {
        User::Leave(KErrGeneral);
    }

    TName *name = new(ELeave) TName;
    CleanupStack::PushL(name);

    User::LeaveIfError(iConnection.Name(*name));

    CleanupStack::Pop(name);

    return name;
}

RSocketServ& CSwtBrowserConnectionManager::SocketServer()
{
    return iServ;
}

void CSwtBrowserConnectionManager::SetRequestedAP(TUint32 aRequestedAPId)
{
    iRequestedAPId = aRequestedAPId;
}

TBool CSwtBrowserConnectionManager::Connected()
{
    return iConnected;
}

TUint32 CSwtBrowserConnectionManager::CurrentAPId() const
{
    return (TUint32)iCurrentAP;
}


TInt CSwtBrowserConnectionManager::CurrentBearerType() const
{
#ifndef __WINS__
    return EApBearerTypeAll;
#else
    return EApBearerTypeGPRS;
#endif
}

void CSwtBrowserConnectionManager::Disconnect()
{
    StopConnection();
}

TInt CSwtBrowserConnectionManager::StartConnectionL()
{
    if (iConnected)
    {
        return KErrNone;
    }

    TInt err(KErrNone);
    if (!iInitialized)
    {

        TRAP(err, InitializeL());
        if (err != KErrNone)
        {
            return err;
        }
    }

    TUint32 newIapId(0);
    TBool doConnect(EFalse);
    TUint32 usedAp(iRequestedAPId);
    TBool requestedApConnected = CheckIfAlreadyConnected(newIapId);
    if (requestedApConnected || (newIapId && !iRequestedAPId))
    {
        // requested ap is connected
        // or there is a connected ap and requested ap not defined
        // try to use existing connection
        usedAp = newIapId;

        // set it as requested ap if requested ap not defined
        if (!iRequestedAPId)
        {
            iRequestedAPId = newIapId;
        }
        // just connect without further checking
        doConnect = ETrue;
    }
    else
    {
        if (!IsConnectionAllowed())
        {
            return KErrGeneral;
        }

        // get bearer type
        TApBearerType bearerType(EApBearerTypeAllBearers);
        if (iRequestedAPId)
        {
            TInt bType(0);
            TRAP(err, bType = BearerTypeL(iRequestedAPId));
            if (err == KErrNone)
            {
                bearerType = static_cast<TApBearerType>(bType);
            }
        }
        // get 3g gvailability
        TBool nw3G = Check3GNetwork();

        // check if there is a HSCSDConnection
        // if  3G network or bearer type of WLAN is not the case
        TBool activeHSCSDConnection(EFalse);
        if (!(nw3G || bearerType == EApBearerTypeWLAN))
        {
            activeHSCSDConnection = IsThereActiveHSCSDConnection();
        }

        if (!activeHSCSDConnection)
        {
            // check if there is a voice connection
            // if 3G network or bearer type of WLAN is not the case
            TBool activeVoiceCall(EFalse);
            if (!(nw3G || bearerType == EApBearerTypeWLAN))
            {
                activeVoiceCall = IsThereActiveVoiceCall();
            }

            if (!activeVoiceCall)
            {
                // connection can be started.
                doConnect = ETrue;
            }
            else
            {
                err = iInternalError = KErrEtelCallAlreadyActive;
            }
        }
        else
        {
            err = KErrGeneral;
        }
    }

    if (!err)
    {
        if (doConnect)
        {
            err = DoConnect(usedAp);
        }
    }

    if (err == KErrNone)
    {
#if !defined( __WINS__ )
        UpdateEasyWlanFlag();
#endif
        // Start monitoring the connection
        TRAP(err, UpdateNameAndStartObserverL());
    }
    return err;
}

void CSwtBrowserConnectionManager::StartConnectionObservingL()
{

    TName* name = ConnectionNameL();
    __ASSERT_DEBUG(name, User::Panic(KNullDesC, KErrCorrupt));
    CleanupStack::PushL(name);
    iStageNotifier->StartNotificationL(name, KConnectionUninitialised, this, ETrue);

    CleanupStack::PopAndDestroy(name);
}

void CSwtBrowserConnectionManager::StopConnectionObserving()
{
    if (iStageNotifier)
    {
        iStageNotifier->Cancel();
    }
}

void CSwtBrowserConnectionManager::ConnectionStageAchievedL()
{
    // this is a connection closed event
    iConnection.Close();
    iConnected = EFalse;

    if (iApiProvider.StartedUp())
    {
        if (iApiProvider.Fetching())
        {
            iApiProvider.CancelFetchL(EFalse);
        }
    }

    iCurrentAP = NULL;
}

TBool CSwtBrowserConnectionManager::IsThereActiveHSCSDConnection()
{
    return EFalse;
}

TBool CSwtBrowserConnectionManager::CheckIfAlreadyConnected(TUint32& aNewIapId)
{
    TBool retVal(EFalse);
    RSocketServ serv;
    RConnection connection;
    TUint count;

    if (serv.Connect() == KErrNone)
    {
        if (connection.Open(serv, KAfInet) == KErrNone)
        {
            TUint32 reqId1(iRequestedAPId);
            if (connection.EnumerateConnections(count) == KErrNone)
            {
                TPckgBuf<TConnectionInfo> connInfo;
                for (TUint i = 1; i <= count; ++i)
                {
                    if (connection.GetConnectionInfo(i, connInfo) == KErrNone)
                    {
                        aNewIapId = connInfo().iIapId;
                        if (aNewIapId == reqId1)
                        {
                            retVal = ETrue;
                            break;
                        }
                    }
                }
            }
            connection.Close();
        }

        serv.Close();
    }
    return retVal;
}

TBool CSwtBrowserConnectionManager::Check3GNetwork()
{
    // This method must not be called if 2G/3G distinction is not supported;
    // the RMobilePhone-related classes are not open in that case.
    TBool is3g(EFalse);
#ifndef ESWT_ON_PUBLIC_SDK
#if !defined( __WINS__ )
    RMobilePhone::TMobilePhoneNetworkMode networkMode;
    if (iMobilePhone.GetCurrentMode(networkMode) != KErrNone)
    {
        return EFalse;
    }
    if (networkMode == RMobilePhone::ENetworkModeCdma2000 ||
            networkMode == RMobilePhone::ENetworkModeWcdma)
    {
        is3g = ETrue;
    }
#endif
#endif // ESWT_ON_PUBLIC_SDK
    return is3g;
}

TBool CSwtBrowserConnectionManager::IsThereActiveVoiceCall()
{
    return EFalse;
}

TBool CSwtBrowserConnectionManager::IsConnectionAllowed()
{
    TInt connAllowed(EFalse);
    TBool IsWLANSupported(EFalse);
    if (iRequestedAPId)
    {
        TInt bearerType(0);
        TRAPD(err, bearerType = BearerTypeL(iRequestedAPId));
        if (err ==KErrNone && bearerType == EApBearerTypeWLAN)
            // WLAN is enabled in off-line mode, too.
        {
            return ETrue;
        }
    }
    else
    {

#if !defined( __WINS__ )
        // check on device
        // check network availability
        iRepository->Get(KCoreAppUIsNetworkConnectionAllowed, connAllowed);

        //Check whether WLAN is supported in offline mode
        if (!connAllowed)
        {
            // FeatureManager has been initialized in CSwtBrowser Constructor
            IsWLANSupported =   CSwtLafFacade::FeatureSupported(KFeatureIdProtocolWlan);
        }
#else
        // Always ask - enable connection because of WLAN
        IsWLANSupported = ETrue;
#endif
    }

    return (connAllowed || IsWLANSupported);
}

void CSwtBrowserConnectionManager::UpdateEasyWlanFlag()
{
    iEasyWlan = EFalse;
    return;
}
