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
* Description:  javamobinfo implementation
 *
*/


#include <e32base.h>
#include <etelmm.h>
#include <mmtsy_names.h>    // TSY and Phone name
#include <commsdattypesv1_1.h>
#include <cdblen.h>
#include <commdb.h>
#include <rconnmon.h>

#include <cmmanager.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginpacketdatadef.h>
#include <cmpluginwlandef.h>
#include <cmpluginhscsddef.h>
#include "connectionmanager.h"

using namespace CMManager;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock.h>
#include <es_enum.h>
#else
#include <es_enum_internal.h>
#include <es_sock_partner.h>
#endif

#include "logger.h"
#include "s60commonutils.h"

#include "com_nokia_mj_impl_properties_mobinfo_MobileInfoProperties.h"
#include "mobinfoconstants.h"

#include "javauids.h"

using namespace std;
using namespace CommsDat;

_LIT(KNetworkAvailable, "available");
_LIT(KNetworkUnavailable, "unavailable");

_LIT(KNetworkAccessPdLAN, "lan"); // Local Area Network
_LIT(KNetworkAccessPdGSM, "pd"); // Packet data in GSM network
_LIT(KNetworkAccessPdEDGE, "pd.EDGE"); // Packet data in EDGE network
_LIT(KNetworkAccessPd3G, "pd.3G"); // Packet data in 3G network
_LIT(KNetworkAccessHSPDA, "pd.HSPDA"); // High speed downlink packet access for 3G networks
_LIT(KNetworkAccessCSD, "csd"); // Circuit switched data, for example GSM CSD/HSCSD data call
_LIT(KNetworkAccessBtPan, "bt_pan"); // Bluetooth PAN network
_LIT(KNetworkAccessWLAN, "wlan"); // Wireless network, for example 802.11b, 802.11G or WiMaX
_LIT(KNetworkAccessNa, "na"); // Not applicable, the implementation can’t determine the type

_LIT(KNetworkStatusHome, "home"); // Home operator
_LIT(KNetworkStatusRoaming, "roaming"); // Roaming
_LIT(KNetworkStatusUnknown, "unknown"); // Unknown operator

_LIT(KMSISDNSeparator, " "); // separator MSISDN numbers

const TInt KMaxPropertySize = 50;
const TInt KJVMProcessUid = KJavaMidp;

// ---------------------------------------------------------------------------
//    forward declarations
// ---------------------------------------------------------------------------
HBufC* GetProperty(const TInt aProperty, const TUint32 aAppDefaultApn, const TUint32 aAppDefaultApType);
HBufC* GetTelephonyPropertyL(const TInt aProperty);
HBufC* GetDateTimePropertyL(const TInt aProperty);
HBufC* GetNetworkAccessL(const TUint32 aAppDefaultApn, const TUint32 aAppDefaultApType);
TUint32     GetUsedIapL();
HBufC*      GetBearerTypeL(TUint32 aIapId);
HBufC*      GetIapBearerTypeL(const TUint32 aAppDefaultApn, const TUint32 aAppDefaultApType);
HBufC*      TranslateBearerInfoToNetworkAccessL(TConnMonBearerInfo aBearerInfo);
HBufC*      GetAppDefIapBearerTypeL(const TUint32 aAppDefaultApn);

/*
 * Class:     com_nokia_mj_impl_properties_mobinfo_MobileInfoProperties
 * Method:    _getProperty
 * Signature: (I)Ljava/lang/String;
 */

// ---------------------------------------------------------------------------
//    JNI function called from java side from MobileInfoProperties class.
//    Each mobinfo java property implementation calls the getPropertyMsid()
//    function of the MobileInfoProperties class.
//    @param aProperty name of the actual property
//    @param aAppDefaultApn application default access point id
//    @param aAppDefaultApType application default access point type ( SNAP, IAP)
//    @param aAppDefaultApn Application default apn
// -----------------------------------------------------------------------------
JNIEXPORT jstring JNICALL
Java_com_nokia_mj_impl_properties_mobinfo_MobileInfoProperties__1getPropertyApn
(JNIEnv* aJni, jclass, jint aProperty, jint aAppDefaultApn, jint aAppDefaultApType)
{
    LOG1(ESOCKET, EInfo, "MOBINFO + getPropertyMsid(), msid: %D", aAppDefaultApn);
    HBufC* property = NULL;
    jstring str = NULL;
    property = GetProperty(aProperty, aAppDefaultApn,aAppDefaultApType);
    if (NULL != property)
    {
        LOG(ESOCKET,EInfo,"_getPropertyApn, property is not null");
        str = java::util::S60CommonUtils::NativeToJavaString(*aJni, property->Des());
        LOG(ESOCKET, EInfo,"MOBINFO - after nativetojavastring");
        //LOG1( ESOCKET, EInfo,"MOBINFO  returning value: %S", property->Des());
        delete property;
    }
    LOG(ESOCKET, EInfo, "MOBINFO - getPropertyMsid()");
    return str;
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_properties_mobinfo_MobileInfoProperties__1getProperty
(JNIEnv *aJni, jobject, jint aKey)
{
    LOG1(ESOCKET, EInfo, "MOBINFO Key: %d", aKey);
    HBufC* property = NULL;
    jstring str = NULL;

    property = GetProperty(aKey,NULL,0);
    if (NULL != property)
    {
        TPtrC data(property->Des());
        str = java::util::S60CommonUtils::NativeToJavaString(*aJni, data);
        delete property;
    }
    return str;
}

HBufC* GetProperty(const TInt aProperty, const TUint32 aAppDefaultApn, const TUint32 aAppDefaultApType)
{
    JELOG2(ESOCKET);
    LOG(ESOCKET, EInfo, "MOBINFO + GetProperty()");
    HBufC* property = NULL;
    TInt err;

    switch (aProperty)
    {
    case DATEFORMAT:
    case TIMEFORMAT:
        TRAP(err, property = GetDateTimePropertyL(aProperty));
        break;
    case NETWORK_ACCESS:
        TRAP(err, property = GetNetworkAccessL(aAppDefaultApn,aAppDefaultApType));
        LOG(ESOCKET, EInfo,"after GetNetworkAccessL");
        break;

    default:
        TRAP(err, property = GetTelephonyPropertyL(aProperty));
        break;
    }
    return property;
}

// ---------------------------------------------------------------------------
//    Finds the default TSY module name from the CommDB. The GlobalSettings
//    table contains a field: ModemForPhoneServicesAndSMS, this field point to
//    a record in the ModemBearer table. The function reads the TsyName from
//    this linked table.
//    @param aTsyName TSY name to be initialized
// -----------------------------------------------------------------------------
void GetDefaultTSYL(TDes& aTsyName)
{
    JELOG2(ESOCKET);

    CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
    CleanupStack::PushL(db);

    // Load Global Settings record (there is always only 1 record in it)
    CCDGlobalSettingsRecord
    * ptrSingleGlSettingRecord =
        static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(
                                                  KCDTIdGlobalSettingsRecord));
    CleanupStack::PushL(ptrSingleGlSettingRecord);

    ptrSingleGlSettingRecord->iRecordTag.SetL(1);
    if (ptrSingleGlSettingRecord->FindL(*db))
    {
        // Find record by RecordTag
        CCDModemBearerRecord
        * ptrModemRecord =
            static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(
                                                   KCDTIdModemBearerRecord));
        CleanupStack::PushL(ptrModemRecord);

        ptrModemRecord->iRecordTag.SetL(
            ptrSingleGlSettingRecord->iModemForPhoneServicesAndSMS);
        if (ptrModemRecord->FindL(*db))
        {
            aTsyName.Copy(ptrModemRecord->iTsyName);
        }

        CleanupStack::PopAndDestroy(ptrModemRecord);
    }

    CleanupStack::PopAndDestroy(ptrSingleGlSettingRecord);
    CleanupStack::PopAndDestroy(db);
}

// ---------------------------------------------------------------------------
//    Initializes the given RTelServer with the default TSY (GetDefaultTSYL)
//    module and opens the corresponding RMobilePhone. The searching of the
//    correct phone is based on the TSY name.
//    @param aMobilePhone RMobilePhone to be opened
//    @param aTelServer RTelServer instance to be initialized
// -----------------------------------------------------------------------------
void GetDefaultPhoneL(RMobilePhone& aMobilePhone, RTelServer& aTelServer)
{
    JELOG2(ESOCKET);

    HBufC* tsyName = HBufC16::NewLC(KCommsDbSvrMaxFieldLength);
    TPtr tsyPtr(tsyName->Des());
    GetDefaultTSYL(tsyPtr);
    //LOG1(ESOCKET, EInfo, "MOBINFO  Default Tsy = %S", tsyPtr);

    User::LeaveIfError(aTelServer.LoadPhoneModule(*tsyName));
    LOG(ESOCKET, EInfo, "MOBINFO  TSY loaded...");

    // Get the name of the first phone
    TInt numPhones = 0;
    User::LeaveIfError(aTelServer.EnumeratePhones(numPhones));
    LOG1(ESOCKET, EInfo, "MOBINFO - Phone count %D", numPhones);

    // Iterate through the phones until we find the correct one
    while (numPhones > 0)
    {
        // The GetTsyName function requires the index of the phone
        // so we must decrease the number here
        --numPhones;
        TName phoneTsy;
        User::LeaveIfError(aTelServer.GetTsyName(numPhones, phoneTsy));
        //LOG1(ESOCKET, EInfo, "MOBINFO  Available phone...%S", phoneTsy);
        if (phoneTsy.CompareF(*tsyName) == KErrNone)
        {
            // Get the phone info
            RTelServer::TPhoneInfo info;
            User::LeaveIfError(aTelServer.GetPhoneInfo(numPhones, info));

            //LOG1(ESOCKET, EInfo, "MOBINFO  Opening phone...%S", info.iName);

            TInt error = aMobilePhone.Open(aTelServer, info.iName);
            LOG1(ESOCKET, EInfo, "MOBINFO  Phone status %D", error);
            User::LeaveIfError(error);

            // We can finish the search here
            numPhones = 0;
        }
    }

    CleanupStack::PopAndDestroy(tsyName);
}

// ---------------------------------------------------------------------------
//    Retrieves the IMEI or IMSI number.
//    @param aMobilePhone initialized RMobilePhone instance.
//    @param aProperty the name of the actual property
//    @return percentage of battery charge level
// -----------------------------------------------------------------------------
HBufC* GetIdentityPropertyL(RMobilePhone& aMobilePhone, const TInt aProperty)
{
    JELOG2(ESOCKET);

    TUint32 identityCaps = 0;
    HBufC* telephonyProperty = NULL;
    TRequestStatus status;

    LOG(ESOCKET, EInfo, "MOBINFO + GetIdentityPropertyL()");
    LOG(ESOCKET, EInfo, "MOBINFO  Checking phone identitycaps");

    TInt identityStatus = aMobilePhone.GetIdentityCaps(identityCaps);
    LOG1(ESOCKET, EInfo, "MOBINFO  Checking identity caps status=%D", identityStatus);

    if ((identityCaps & RMobilePhone::KCapsGetSubscriberId)
            && (aProperty == IMSI))
    {
        LOG(ESOCKET, EInfo, "MOBINFO  retrieving IMSI");

        // initialise a variable to hold IMSI
        RMobilePhone::TMobilePhoneSubscriberId imsi;

        // put values into variable
        aMobilePhone.GetSubscriberId(status, imsi);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());
        LOG(ESOCKET, EInfo, "MOBINFO  returning property IMSI");
        telephonyProperty = imsi.AllocL();
    }

    if ((identityCaps & RMobilePhone::KCapsGetSerialNumber)
            && (aProperty == IMEI))
    {
        LOG(ESOCKET, EInfo, "MOBINFO  retrieving IMEI");

        RMobilePhone::TMobilePhoneIdentityV1 phoneId;
        aMobilePhone.GetPhoneId(status, phoneId);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());
        LOG(ESOCKET, EInfo, "MOBINFO  returning property IMEI");
        telephonyProperty = (phoneId.iSerialNumber).AllocL();
    }

    return telephonyProperty;
}

// ---------------------------------------------------------------------------
//    Retrieves the battery charge level in percentage.
//    @param aMobilePhone initialized RMobilePhone instance.
//    @return percentage of battery charge level
// -----------------------------------------------------------------------------
HBufC* GetBatteryPropertyL(RMobilePhone& aMobilePhone)
{
    JELOG2(ESOCKET);

    HBufC* telephonyProperty = NULL;
    LOG(ESOCKET, EInfo, "MOBINFO + GetBatteryPropertyL()");

    // Check BatteryCaps
    TUint32 batteryCaps;
    LOG(ESOCKET, EInfo, "MOBINFO  Checking phone BatteryCaps");

    TInt batteryCapsStatus = aMobilePhone.GetBatteryCaps(batteryCaps);
    LOG1(ESOCKET, EInfo, "MOBINFO  Checking Battery caps status=%D",
         batteryCapsStatus);

    if (batteryCapsStatus == KErrNone)
    {
        TRequestStatus status;

        LOG(ESOCKET, EInfo, "MOBINFO  retrieving battery charge level");
        RMobilePhone::TMobilePhoneBatteryInfoV1 info;
        aMobilePhone.GetBatteryInfo(status, info);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());
        LOG(ESOCKET, EInfo, "MOBINFO  returning property Battery charge level");

        TBuf<KMaxPropertySize> buf; // percentage of battery charge level
        buf.Num(info.iChargeLevel);
        telephonyProperty = buf.AllocL();
    }

    LOG(ESOCKET, EInfo, "MOBINFO - GetBatteryPropertyL()");
    return telephonyProperty;
}

// ---------------------------------------------------------------------------
//    Retrieves the network related properties.
//    @param aMobilePhone initialized RMobilePhone instance.
//    @param aProperty name of the actual property
//    @return property value
// -----------------------------------------------------------------------------
HBufC* GetNetworkPropertyL(RMobilePhone& aMobilePhone, const TInt aProperty)
{
    JELOG2(ESOCKET);

    HBufC* telephonyProperty = NULL;
    LOG(ESOCKET, EInfo, "MOBINFO + GetNetworkPropertyL()");

    if ((aProperty == NETWORK_ID) || (aProperty == COUNTRY_CODE) || (aProperty
            == CELLID))
    {
        // Check NetworkCaps
        TUint32 networkCaps;
        LOG(ESOCKET, EInfo, "MOBINFO  Checking phone NetworkCaps");

        TInt networkCapsStatus = aMobilePhone.GetNetworkCaps(networkCaps);
        LOG1(ESOCKET, EInfo, "MOBINFO  Checking network caps status=%D",
             networkCapsStatus);
        if (networkCapsStatus == KErrNone)
        {
            TRequestStatus status;

            if ((networkCaps & RMobilePhone::KCapsGetCurrentNetwork)
                    && (aProperty == NETWORK_ID))
            {
                LOG(ESOCKET, EInfo, "MOBINFO  retrieving NetworkId");

                RMobilePhone::TMobilePhoneOPlmnV3 locInfo;
                RMobilePhone::TMobilePhoneOPlmnV3Pckg pckgLocInfo(locInfo);

                RMobilePhone::TMobilePhoneNetworkNameV3 networkName;
                RMobilePhone::TMobilePhoneNetworkNameV3Pckg pckgNetworkName(
                    networkName);
                aMobilePhone.GetCurrentNetworkName(status, pckgNetworkName,
                                                   pckgLocInfo);
                User::WaitForRequest(status);
                User::LeaveIfError(status.Int());
                LOG(ESOCKET, EInfo, "MOBINFO  returning property NetworkId");
                _LIT(KStringFormat, "%S (%S)");

                // 8 is the max length of MNC, + 3 chars needed for the
                // formatting chars like space, "(" and ")"
                TBuf<KMaxPropertySize + RMobilePhone::KMaxNetworkNameFieldSize>
                disp;
                disp.Format(KStringFormat, &locInfo.iNetworkId,
                            &networkName.iShortName);
                telephonyProperty = disp.AllocL();
            }
            else if ((networkCaps & RMobilePhone::KCapsGetCurrentNetwork)
                     && (aProperty == CELLID))
            {
                LOG(ESOCKET, EInfo, "MOBINFO  retrieving CellId");

                RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
                RMobilePhone::TMobilePhoneNetworkInfoV1Pckg networkInfoPckg(
                    networkInfo);
                RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
                aMobilePhone.GetCurrentNetwork(status, networkInfoPckg,
                                               locationArea);

                User::WaitForRequest(status);
                User::LeaveIfError(status.Int());

                LOG(ESOCKET, EInfo, "MOBINFO  returning property CellId");

                TBuf<KMaxPropertySize> disp;
                disp.Num(locationArea.iCellId, EDecimal);
                telephonyProperty = disp.AllocL();
            }
            else // COUNTRY_CODE
            {
                LOG(ESOCKET, EInfo, "MOBINFO  retrieving CountryCode");

                RMobilePhone::TMobilePhoneOPlmnV3 locInfo;
                RMobilePhone::TMobilePhoneOPlmnV3Pckg pckgLocInfo(locInfo);

                RMobilePhone::TMobilePhoneNetworkNameV3 networkName;
                RMobilePhone::TMobilePhoneNetworkNameV3Pckg pckgNetworkName(
                    networkName);
                aMobilePhone.GetCurrentNetworkName(status, pckgNetworkName,
                                                   pckgLocInfo);
                User::WaitForRequest(status);
                User::LeaveIfError(status.Int());
                LOG(ESOCKET, EInfo, "MOBINFO  returning property CountryCode");
                telephonyProperty = (locInfo.iCountryCode).AllocL();
            }
        }
    }
    else if (aProperty == NETWORK_SIGNAL)
    {
        // Check Signal related Caps
        TUint32 signalCaps;
        LOG(ESOCKET, EInfo, "MOBINFO  Checking phone SignalCaps");

        TInt signalCapsStatus = aMobilePhone.GetSignalCaps(signalCaps);
        LOG1(ESOCKET, EInfo, "MOBINFO  Checking signal caps status=%D",
             signalCapsStatus);
        if (signalCapsStatus == KErrNone)
        {
            TRequestStatus status;

            if (signalCaps & RMobilePhone::KCapsGetSignalStrength)
            {
                LOG(ESOCKET, EInfo, "MOBINFO  retrieving SignalStrength");

                TInt32 signalStrength;
                TInt8 bar;
                aMobilePhone.GetSignalStrength(status, signalStrength, bar);
                User::WaitForRequest(status);
                User::LeaveIfError(status.Int());
                LOG(ESOCKET, EInfo,
                    "MOBINFO  returning property SignalStrength");
                _LIT(KStringFormat, "%d (%d dBm)");

                TBuf<KMaxPropertySize> disp;
                disp.Format(KStringFormat, bar, signalStrength);
                telephonyProperty = disp.AllocL();
            }
        }
    }
    else if (aProperty == NETWORKSTATUS)
    {
        // Check network status
        LOG(ESOCKET, EInfo, "MOBINFO  Checking network status");

        // Check NetworkCaps
        TUint32 networkCaps;
        TInt networkCapsStatus = aMobilePhone.GetNetworkCaps(networkCaps);
        LOG1(ESOCKET, EInfo, "MOBINFO  Checking network caps status=%D",
             networkCapsStatus);
        if (networkCapsStatus == KErrNone)
        {
            TRequestStatus status;
            RMobilePhone::TMobilePhoneRegistrationStatus networkStatus;

            aMobilePhone.GetNetworkRegistrationStatus(status, networkStatus);
            User::WaitForRequest(status);
            User::LeaveIfError(status.Int());

            switch (networkStatus)
            {
            case RMobilePhone::ERegisteredOnHomeNetwork:
            case RMobilePhone::ERegisteredBusy:
                telephonyProperty = KNetworkStatusHome().AllocL();
                break;

            case RMobilePhone::ERegisteredRoaming:
                telephonyProperty = KNetworkStatusRoaming().AllocL();
                break;

            default:
                telephonyProperty = KNetworkStatusUnknown().AllocL();
                break;
            }
        }
        LOG(ESOCKET, EInfo, "MOBINFO  returning property NetworkStatus");
    }
    else // NETWORK_AVAILABILITY
    {
        // Check NetworkAvailability related Caps
        TUint32 indCaps;
        TUint32 actionCaps;
        LOG(ESOCKET, EInfo, "MOBINFO  Checking phone IndicatorCaps");

        TInt indicatorCapsStatus = aMobilePhone.GetIndicatorCaps(actionCaps,
                                   indCaps);
        LOG1(ESOCKET, EInfo, "MOBINFO  Checking IndicatorCaps status=%D",
             indicatorCapsStatus);
        if (indicatorCapsStatus == KErrNone)
        {
            TRequestStatus status;

            if (indCaps & RMobilePhone::KCapsGetIndicator)
            {
                LOG(ESOCKET, EInfo, "MOBINFO  retrieving Network Availability");

                TUint32 indicator;
                aMobilePhone.GetIndicator(status, indicator);
                User::WaitForRequest(status);
                User::LeaveIfError(status.Int());
                LOG(ESOCKET, EInfo,
                    "MOBINFO  returning property NetworkAvailability");
                if (indicator & RMobilePhone::KIndNetworkAvailable)
                {
                    telephonyProperty = HBufC::NewL(KMaxPropertySize);
                    telephonyProperty->Des().Append(KNetworkAvailable);
                }
                else
                {
                    telephonyProperty = HBufC::NewL(KMaxPropertySize);
                    telephonyProperty->Des().Append(KNetworkUnavailable);
                }
            }
        }
    }

    LOG(ESOCKET, EInfo, "MOBINFO  - GetNetworkPropertyL()");
    return telephonyProperty;
}

// ---------------------------------------------------------------------------
//    Retrieves the date and time format related properties.
//    @param aProperty name of the actual property
//    @return error code
// -----------------------------------------------------------------------------
HBufC* GetDateTimePropertyL(const TInt aProperty)
{
    JELOG2(ESOCKET);

    HBufC* property = HBufC::NewMax(KMaxPropertySize);
    TLocale locale;
    TDateFormat dformat;
    TChar dseparator;
    TTimeFormat tformat;
    TChar separator;

    switch (aProperty)
    {
    case DATEFORMAT:
        dformat = locale.DateFormat();
        dseparator = locale.DateSeparator(1);

        switch (dformat)
        {
        case EDateAmerican:
            property->Des().Copy(_L("MM"));
            property->Des().Append(dseparator);
            property->Des().Append(_L("dd"));
            property->Des().Append(dseparator);
            property->Des().Append(_L("yyyy"));
            break;

        case EDateEuropean:
            property->Des().Copy(_L("dd"));
            property->Des().Append(dseparator);
            property->Des().Append(_L("MM"));
            property->Des().Append(dseparator);
            property->Des().Append(_L("yyyy"));
            break;

        case EDateJapanese:
            property->Des().Copy(_L("yyyy"));
            property->Des().Append(dseparator);
            property->Des().Append(_L("MM"));
            property->Des().Append(dseparator);
            property->Des().Append(_L("dd"));
            break;
        }
        break;

    case TIMEFORMAT:
        tformat = locale.TimeFormat();
        separator = locale.TimeSeparator(1);

        switch (tformat)
        {
        case ETime12:
            property->Des().Copy(_L("hh"));
            property->Des().Append(separator);
            property->Des().Append(_L("mm a"));
            break;

        case ETime24:
            property->Des().Copy(_L("HH"));
            property->Des().Append(separator);
            property->Des().Append(_L("mm"));
            break;
        }
    }
    return property;
}

// ---------------------------------------------------------------------------
//    Retrieves the network related properties.
//    @param aMobilePhone initialized RMobilePhone instance.
//    @param aTelServer initialized RTelServer instance.
//    @return property value
// ---------------------------------------------------------------------------
HBufC* GetMSISDNPropertyL(RMobilePhone& aMobilePhone, RTelServer& aTelServer)
{
    JELOG2(ESOCKET);
    LOG(ESOCKET, EInfo, "MOBINFO + GetMSISDNPropertyL()");

    HBufC* property = NULL;

    // get default TSY name
    HBufC* tsyName = HBufC16::NewLC(KCommsDbSvrMaxFieldLength);
    TPtr tsyPtr(tsyName->Des());
    GetDefaultTSYL(tsyPtr); // used in GetDefaultPhone()

    LOG(ESOCKET, EInfo, "MOBINFO  Retrieving info about for own number store");

    TBool supported;
    User::LeaveIfError(aTelServer.IsSupportedByModule(tsyPtr,
                       KETelFuncMobileOwnNumberStore, supported));

    LOG1(
        ESOCKET,
        EInfo,
        "MOBINFO  Checking own number store suppport by TSY, supported = %D: ",
        (TInt) supported);
    CleanupStack::PopAndDestroy(tsyName);

    if (supported)
    {
        RMobileONStore ownNumberStore;
        User::LeaveIfError(ownNumberStore.Open(aMobilePhone));
        CleanupClosePushL(ownNumberStore);

        RMobileONStore::TMobileONStoreInfoV1 ownStoreInfo;
        RMobileONStore::TMobileONStoreInfoV1Pckg ownStoreInfoPckg(ownStoreInfo);

        TRequestStatus status;
        ownNumberStore.GetInfo(status, ownStoreInfoPckg);
        User::WaitForRequest(status);

        LOG1(ESOCKET, EInfo,
             "MOBINFO  Checking getting own number store info status = %D ",
             status.Int());
        User::LeaveIfError(status.Int());

        TInt ownStoreInfoUsedEntries = ownStoreInfo.iUsedEntries;
        LOG1(ESOCKET, EInfo,
             "MOBINFO  Entries stored in own number store: %D ",
             ownStoreInfoUsedEntries);

        if (ownStoreInfoUsedEntries >= 1)
        {
            // calculate length for property
            TInt size = ownStoreInfoUsedEntries
                        * RMobileONStore::KOwnNumberTextSize;
            // take into account separators if more than one entry stored
            size = size + ((ownStoreInfoUsedEntries - 1)
                           * KMSISDNSeparator().Length());
            property = HBufC::NewL(size);

            for (TInt index = 1; index <= ownStoreInfoUsedEntries; index++)
            {
                LOG(ESOCKET, EInfo, "MOBINFO  Retrieving property MSISDN ");
                RMobileONStore::TMobileONEntryV1 ownNumberEntry;
                ownNumberEntry.iIndex = index; // = index
                RMobileONStore::TMobileONEntryV1Pckg ownNumberEntryPckg(
                    ownNumberEntry);

                status = KRequestPending;
                ownNumberStore.Read(status, ownNumberEntryPckg);
                User::WaitForRequest(status);

                LOG1(ESOCKET, EInfo,
                     "MOBINFO  Checking reading entry status = ",
                     status.Int());
                User::LeaveIfError(status.Int());

                LOG(ESOCKET, EInfo, "MOBINFO  Returning property MSISDN");

                property->Des().Append(ownNumberEntry.iNumber.iTelNumber);

                if (index < ownStoreInfoUsedEntries)
                {
                    property->Des().Append(KMSISDNSeparator);
                }
            }
        }
        CleanupStack::PopAndDestroy(&ownNumberStore);
    }

    LOG(ESOCKET, EInfo, "MOBINFO - GetMSISDNPropertyL()");
    return property;
}

// ---------------------------------------------------------------------------
//    The funtion used for "Telephony related" properties which are requires an
//    initialized RTel server and RMobilePhone.
//    @param aProperty name of the actual property
//    @return property value
// -----------------------------------------------------------------------------
HBufC* GetTelephonyPropertyL(const TInt aProperty)
{
    JELOG2(ESOCKET);
    LOG(ESOCKET, EInfo, "MOBINFO + GetTelephonyPropertyL");

    // Construct a connection to the RTel server
    RTelServer telServer;
    User::LeaveIfError(telServer.Connect());
    LOG(ESOCKET, EInfo,
        "MOBINFO  GetTelephonyPropertyL: connected to the RTel server");
    CleanupClosePushL(telServer);

    RMobilePhone mobilePhone;
    CleanupClosePushL(mobilePhone);
    GetDefaultPhoneL(mobilePhone, telServer);
    LOG(ESOCKET, EInfo,
        "MOBINFO  GetTelephonyPropertyL: after get default phoneconnected to the RTel server");

    HBufC* telephonyProperty = NULL;

    switch (aProperty)
    {
    case IMSI:
    case IMEI:
        telephonyProperty = GetIdentityPropertyL(mobilePhone, aProperty);
        break;

    case MSISDN:
        telephonyProperty = GetMSISDNPropertyL(mobilePhone, telServer);
        break;

    case BATTERY_LEVEL:
        telephonyProperty = GetBatteryPropertyL(mobilePhone);
        break;

    case NETWORK_ID:
    case NETWORK_SIGNAL:
    case NETWORK_AVAILABILITY:
    case COUNTRY_CODE:
    case CELLID:
    case NETWORKSTATUS:
        telephonyProperty = GetNetworkPropertyL(mobilePhone, aProperty);
        break;
    }

    CleanupStack::PopAndDestroy(&mobilePhone);
    CleanupStack::PopAndDestroy(&telServer);
    LOG(ESOCKET, EInfo, "MOBINFO - GetTelephonyPropertyL()");
    return telephonyProperty;
}

// ---------------------------------------------------------------------------
// Get type network access of currently used active connection
// @return network access, ownership hand over
// ---------------------------------------------------------------------------
HBufC* GetNetworkAccessL(TUint32 aAppDefaultApn, const TUint32 aAppDefaultApType)
{
    LOG(ESOCKET, EInfo, "MOBINFO + GetNetworkAccessL");
    HBufC* bearerType = NULL;
    TUint32 iapId = GetUsedIapL();
    if (iapId != 0)
    {
        // active connection present, opened by MIDlet.
        bearerType = GetBearerTypeL(iapId);

    }
    else
    {
        // if there is no active connection by the application, return the app default bearer type.
        if (aAppDefaultApType == 3)
        {
            // app default is IAP
            bearerType = GetAppDefIapBearerTypeL(aAppDefaultApn);

        }
        else
        {
            bearerType = GetIapBearerTypeL(aAppDefaultApn,aAppDefaultApType);
        }

    }
    LOG(ESOCKET, EInfo, "MOBINFO - GetNetworkAccessL");
    return bearerType;
}


//// ---------------------------------------------------------------------------
//// Check if device default is set (SNAP or IAP)
//// @return true if device default defined, else false
//// ---------------------------------------------------------------------------

bool isDeviceDefaultPresentL()
{
    TCmDefConnType type;
    RCmManager * mgr = new(ELeave) RCmManager();
    mgr->OpenL();

    TCmDefConnValue obj;
    mgr->ReadDefConnL(obj);
    type = obj.iType;

    if ((type!=ECmDefConnConnectionMethod) && (type!=ECmDefConnDestination))
    {
        // device default is other than SNAP or IAP
        return false;
    }
    else
    {
        return true;

    }



}

//// ---------------------------------------------------------------------------
//// Get bearer type of Access point ID
//// @return bearer type Acess point ID
//// ---------------------------------------------------------------------------

HBufC* GetIapBearerTypeL(const TUint32 aAppDefaultApn, const TUint32 aAppDefaultApType)
{

    LOG(ESOCKET, EInfo, "MOBINFO + GetIapBearerTypeL()");
    RConnectionMonitor      monitor;
    TUint                   connectionCount(0);
    TUint                   iapId(0);
    TRequestStatus          status(KErrNone);
    HBufC*                  bearerType(NULL);
    TUint                   connectionId(0);
    TUint                   subConnectionCount(0);
    TConnMonBearerInfo      bearerInfo(EBearerInfoUnknown);
    bool                    flag = false;
    TUint                   connIdx = 1;

    User::LeaveIfError(monitor.ConnectL());
    CleanupClosePushL(monitor);

    status = KRequestPending;
    monitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    LOG1(ESOCKET, EInfo, "MOBINFO: GetIapBearerTypeL: Count of active connections: %D", connectionCount);

    // Go through all connections
    for (connIdx = 1; connIdx <= connectionCount;  ++connIdx)
    {

        connectionId = 0;
        subConnectionCount = 0;
        flag = false;

        // Get connection ID
        monitor.GetConnectionInfo(connIdx, connectionId, subConnectionCount);
        User::LeaveIfError(connectionId);
        // Get connection IAP ID
        status = KRequestPending;
        monitor.GetUintAttribute(connectionId, 0, KIAPId, iapId, status);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());
        LOG1(ESOCKET, EInfo, "MOBINFO: GetIapBearerTypeL Connection 's IAP ID found: %D", iapId);
        if (aAppDefaultApType == 2)
        {
            LOG(ESOCKET, EInfo, "MOBINFO: Matching MIDlet defined SNAP to active IAP ID ");
            // Midlet has a SNAP defined, so check if the active IAP is under this SNAP.
            flag = ConnectionManager::isIapDefault(iapId, aAppDefaultApn,false);
            if (!flag)
            {
                continue;  // active access point doesn't belong to MIDlet SNAP, check the next active access point.
            }
            else
            {
                break;
            }
        }
        else
        {
            // No SNAP defined for the MIDlet, check for system default SNAP/IAP.
            if (isDeviceDefaultPresentL())
            {
                LOG(ESOCKET, EInfo, "MOBINFO: Matching device default to active IAP ID ");
                flag = ConnectionManager::isIapDefault(iapId, 0, true);
                if (!flag)
                {
                    continue;  // active access point doesn't belong to MIDlet SNAP, check the next active access point.
                }
                else
                {
                    break;
                }
            }
            else
            {
                LOG(ESOCKET, EInfo, "MOBINFO: Returning the bearer type of first active IAP ");
                flag = true;
                break; // no MIDlet SNAP, no device default, select this access point and return the bearer type

            }
        }

    } // end of for loop

    if (flag)
    {
        // we did not reach end of active connections, one of the conditions matched , find the bearer type
        status = KRequestPending;
        monitor.GetIntAttribute(connectionId,
                                0,
                                KBearerInfo,
                                (TInt&)bearerInfo,
                                status);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());

        // Translate bearer type to type of network access
        bearerType = TranslateBearerInfoToNetworkAccessL(bearerInfo);
    }
    CleanupStack::PopAndDestroy(&monitor);
    if (bearerType == NULL)
    {
        // No active connection (or) active IAP didn't match the conditions
        bearerType = KNetworkAccessNa().AllocL();
    }
    return bearerType;


}

//// ---------------------------------------------------------------------------
//// Get currently used Access point ID
//// @return currently used Acess point ID
//// ---------------------------------------------------------------------------
TUint32 GetUsedIapL()
{
    LOG(ESOCKET, EInfo, "MOBINFO + GetUsedIapL()");

    RSocketServ                 socketServer;
    RConnection                 connection;
    TConnEnumArgBuf             enumArgBuf;
    TConnGetClientInfoArgBuf    clientInfoBuf;
    TUint                       connectionCount(0);
    TConnectionInfoBuf          connInfoBuf;
    TUint32                     iapId(0);

    // Get MIDlet process ID
    RProcess process;
    RThread thread;
    TProcessId currProcessId(process.Id());
    TThreadId currThreadId(thread.Id());
    //LOG1( ESOCKET, EInfo, "MOBINFO  Current process ID: %D", currProcessId );
    //LOG1( ESOCKET, EInfo, "MOBINFO  Current thread ID: %D", currThreadId );

    // Connect to socket server and use system global pool (-1)
    // for message slots.
    LOG(ESOCKET, EInfo, "MOBINFO  Connecting to socket server");
    User::LeaveIfError(socketServer.Connect(static_cast< TUint >(-1)));
    CleanupClosePushL(socketServer);
    LOG(ESOCKET, EInfo, "MOBINFO  Connected to socket server");

    // Open RConnection,
    // this object is not attached to any connection
    LOG(ESOCKET, EInfo, "MOBINFO  Opening connection");
    User::LeaveIfError(connection.Open(socketServer));
    CleanupClosePushL(connection);
    LOG(ESOCKET, EInfo, "MOBINFO  Connection opened");

    // Get count of current active connections.
    LOG(ESOCKET, EInfo, "MOBINFO  Getting count of current active connections");
    User::LeaveIfError(connection.EnumerateConnections(connectionCount));
    LOG1(ESOCKET, EInfo, "MOBINFO  Count of active connections %D", connectionCount);

    // Go through all active connections
    for (TUint connIdx = 1; connIdx <= connectionCount && iapId == 0; connIdx++)
    {
        enumArgBuf().iIndex = connIdx;

        // Get the number of clients
        LOG1(ESOCKET, EInfo, "MOBINFO  Getting number of clients for active connection %D.", connIdx);
        User::LeaveIfError(connection.Control
                           (
                               KCOLConnection,
                               KCoEnumerateConnectionClients,
                               enumArgBuf
                           )
                          );
        LOG1(ESOCKET, EInfo, "MOBINFO  Number of clients: %D", enumArgBuf().iCount);

        // Go through all clients
        for (TUint clientIdx = 1;
                clientIdx <= enumArgBuf().iCount && iapId == 0; clientIdx++)
        {
            clientInfoBuf().iIndex = clientIdx;
            LOG1(ESOCKET, EInfo, "MOBINFO  Retreiving client %D. info", clientIdx);
            User::LeaveIfError(connection.Control
                               (
                                   KCOLConnection,
                                   KCoGetConnectionClientInfo,
                                   clientInfoBuf
                               )
                              );
            LOG(ESOCKET, EInfo, "MOBINFO  Client info retreived: ");

            TUid uid(clientInfoBuf().iClientInfo.iUid);
            TProcessId processId(clientInfoBuf().iClientInfo.iProcessId);
            TThreadId threadId(clientInfoBuf().iClientInfo.iThreadId);
            LOG1(ESOCKET, EInfo, "MOBINFO  Client UID:        %D", uid.iUid);
            //LOG1( ESOCKET, EInfo, "MOBINFO  Client process ID: %D", processId );
            //LOG1( ESOCKET, EInfo, "MOBINFO  Client thread ID:  %D", threadId );

            // MIDlet's active connection found
            if (uid.iUid == KJVMProcessUid && currProcessId == processId)
            {
                LOG(ESOCKET, EInfo, "MOBINFO  Active connection of current MIDlet found");
                TConnectionInfoBuf connInfoBuf;

                LOG1(ESOCKET, EInfo, "MOBINFO  Getting connection info %D", clientIdx);
                User::LeaveIfError(
                    connection.GetConnectionInfo(connIdx, connInfoBuf)
                );
                LOG(ESOCKET, EInfo, "MOBINFO  Connection info gotten");
                iapId  = connInfoBuf().iIapId;
                LOG1(ESOCKET, EInfo, "MOBINFO  currently used IAP ID found: %D", iapId);
            }
        }
    }

    LOG1(ESOCKET, EInfo, "MOBINFO  currently used IAP ID: %D", iapId);
    CleanupStack::PopAndDestroy(&connection);
    CleanupStack::PopAndDestroy(&socketServer);
    LOG(ESOCKET, EInfo, "MOBINFO - GetUsedIapL()");
    return iapId;
}

// ---------------------------------------------------------------------------
// Get IAP's bearer type
// @param aIapId IAP ID
// @return bearer type, ownership handed over
// "LAN"       Local area network
// 'pd.GSM"    Packet data in GSM network
// "pd.EDGE"   Packet data in EDGE network
// "pd.3G"     Packet data in 3G network
// "csd"       Circuit switched data, for example GSM CSD/HSCSD data call
// "bt_pan"    Bluetooth PAN network
// "wlan"      Wireless network, for example 802.11b, 802.11G or WiMaX
// "hspda"     High speed downlink packet access for 3G networks
// "na"        Not applicable, the implementation can’t determine the type
// ---------------------------------------------------------------------------
HBufC* GetBearerTypeL(TUint32 aIapId)
{
    LOG(ESOCKET, EInfo, "MOBINFO + GetBearerTypeL()");
    RConnectionMonitor      monitor;
    TUint                   connectionCount(0);
    TUint                   iapId(0);
    TRequestStatus          status(KErrNone);
    HBufC*                  bearerType(NULL);
    TUint                   connectionId(0);
    TUint                   subConnectionCount(0);
    TConnMonBearerInfo      bearerInfo(EBearerInfoUnknown);

    LOG(ESOCKET, EInfo, "MOBINFO  Connecting to Connection Monitor");
    User::LeaveIfError(monitor.ConnectL());
    CleanupClosePushL(monitor);
    LOG(ESOCKET, EInfo, "MOBINFO  Connected to Connection Monitor");

    LOG(ESOCKET, EInfo, "MOBINFO  Getting count of active connections");
    status = KRequestPending;
    monitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    LOG1(ESOCKET, EInfo, "MOBINFO  Count of active connections: %D", connectionCount);

    // Go through all connections
    for (TUint connIdx = 1; connIdx <= connectionCount && iapId == 0;
            ++connIdx)
    {
        connectionId = 0;
        subConnectionCount = 0;

        // Get connection ID
        LOG1(ESOCKET, EInfo, "MOBINFO  Getting connection info %D", connIdx);
        monitor.GetConnectionInfo(connIdx, connectionId, subConnectionCount);
        User::LeaveIfError(connectionId);
        LOG(ESOCKET, EInfo, "MOBINFO  Connection info gotten");

        // Get connection IAP ID
        LOG1(ESOCKET, EInfo, "MOBINFO  Getting connection %D IAP ID", connIdx);
        status = KRequestPending;
        monitor.GetUintAttribute(connectionId, 0, KIAPId, iapId, status);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());
        LOG1(ESOCKET, EInfo, "MOBINFO  Connection 's IAP ID found: %D", iapId);

        if (iapId == aIapId)
        {
            // MIDlet's IAP found
            LOG(ESOCKET, EInfo,  "MOBINFO  Midlet's IAP found");

            // Get bearer type
            LOG(ESOCKET, EInfo, "MOBINFO  Getting bearer type");
            status = KRequestPending;
            monitor.GetIntAttribute(connectionId,
                                    0,
                                    KBearerInfo,
                                    (TInt&)bearerInfo,
                                    status);
            User::WaitForRequest(status);
            User::LeaveIfError(status.Int());
            LOG1(ESOCKET, EInfo, "MOBINFO  Bearer type found: %D", bearerInfo);
        }
    }

    // Translate bearer type to type of network access
    bearerType = TranslateBearerInfoToNetworkAccessL(bearerInfo);

    CleanupStack::PopAndDestroy(&monitor);
    LOG(ESOCKET, EInfo, "MOBINFO - GetBearerTypeL()");
    return bearerType;
}

// ---------------------------------------------------------------------------
// Translate bearer info to type of network access
// @param aBearerInfo bearer info
// @return type of network access, ownership hand over
HBufC* TranslateBearerInfoToNetworkAccessL(TConnMonBearerInfo aBearerInfo)
{
    LOG(ESOCKET, EInfo, "MOBINFO + TranslateBearerInfoToNetworkAccessL()");
    HBufC* bearerType(NULL);
    switch (aBearerInfo)
    {
    case EBearerInfoLAN:
        bearerType = KNetworkAccessPdLAN().AllocL();
        break;
    case EBearerInfoGPRS:
        bearerType = KNetworkAccessPdGSM().AllocL();
        break;
    case EBearerInfoEdgeGPRS:
        bearerType = KNetworkAccessPdEDGE().AllocL();
        break;
    case EBearerInfoWCDMA:
        bearerType = KNetworkAccessPd3G().AllocL();
        break;
    case EBearerInfoCSD:
    case EBearerInfoHSCSD:
    case EBearerInfoWcdmaCSD:
        bearerType = KNetworkAccessCSD().AllocL();
        break;
    case EBearerInfoBluetooth:
        bearerType = KNetworkAccessBtPan().AllocL();
        break;
    case EBearerInfoWLAN:
        bearerType = KNetworkAccessWLAN().AllocL();
        break;
    case EBearerInfoHSDPA:
    case EBearerInfoHSUPA:
    case EBearerInfoHSxPA:
        bearerType = KNetworkAccessHSPDA().AllocL();
        break;
    case EBearerInfoUnknown:
    case EBearerInfoCDMA2000:
    case EBearerInfoVirtual:
    case EBearerInfoVirtualVPN:
    default:
        bearerType = KNetworkAccessNa().AllocL();
        break;
    }
    LOG(ESOCKET, EInfo, "MOBINFO - TranslateBearerInfoToNetworkAccessL()");
    return bearerType;
}



// --------------------------------------------------------------------
// Get network access type of default access point set in MIDlet settings
// @param aAppDefaultApn MIDlet default IAP
// @return network access type, ownership hand over
HBufC* GetAppDefIapBearerTypeL(const TUint32 aAppDefaultApn)
{
    LOG1(ESOCKET, EInfo, "MOBINFO + GetAppDefIapBearerTypeL aId = %d ", aAppDefaultApn);
    HBufC* bearerType(NULL);

    if (aAppDefaultApn == 0 || aAppDefaultApn == (TUint32)-1)
    {
        bearerType = KNetworkAccessNa().AllocL();
    }
    else
    {

        RCmManager localCmManager;
        localCmManager.OpenL();
        TUint32 iap = aAppDefaultApn;
        TUint32 tmpval = localCmManager.GetConnectionMethodInfoIntL(iap,ECmBearerType);
        ELOG1(ESOCKET,  "GetConnectionMethodInfoIntL: %d", tmpval);
        switch (tmpval)
        {
        case KUidHSCSDBearerType:
            bearerType = KNetworkAccessCSD().AllocL();
            break;
        case KUidPacketDataBearerType:
            bearerType = KNetworkAccessPdGSM().AllocL();
            break;
        case KUidWlanBearerType :
            bearerType = KNetworkAccessWLAN().AllocL();
            break;

            /* Bearer type LAN,LANModem currently not supported by cmmanager */
        default :
            bearerType = KNetworkAccessNa().AllocL();
            break;
        }
    }
    LOG(ESOCKET, EInfo, "MOBINFO - GetAppDefIapBearerTypeL");
    return bearerType;
}
