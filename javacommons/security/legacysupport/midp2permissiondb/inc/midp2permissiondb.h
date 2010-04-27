/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#ifndef MIDP2PERMISSIONDB_H
#define MIDP2PERMISSIONDB_H

#include <e32base.h>
#include <string>

namespace MIDP
{
// ** Please keep in synch with the below array!
enum TMidp2Domain
{
    EDomainUntrusted = 0,
    EDomainTTP,
    EDomainOperator,
    EDomainManufacturer,
    EDomainOnDeviceOperator,
    EDomainOperatorUserPerms,
    EDomainOnDeviceOperatorUserPerms,
    EDomainManufacturerUserPerms,

    ELastDomainMarker
};

static const TUint16* const KJcfDomains[] =
{
    _S16("*UTD*"), // 0
    _S16("*TTPD*"),
    _S16("*OPD*"),
    _S16("*MFD*"),
    _S16("*ODOPD*"),
    _S16("*OPDU*"),
    _S16("*ODOPDU*"),
    _S16("*MFDU*")
};

//
// ********************************************************************
// Permission type and mode definitions
// ********************************************************************
//
enum TMidp2PermissionType
{
    ENotAllowed,
    EUser,
    EAllowed
};

enum TMidp2PermissionMode
{
    EDenied  = 0x01,
    EOneShot = 0x02,
    ESession = 0x04,
    EBlanket = 0x08
};

//
// ********************************************************************
// Permission definitions
// ********************************************************************
//

// ** Please keep in synch with the below array!
enum TMidp2Permission
{
    // MIDP 2.0 JSR 118
    EPermissionConnectorHttp = 0,
    EPermissionConnectorHttps,
    EPermissionConnectorDatagram,
    EPermissionConnectorDatagramReceiver,
    EPermissionConnectorSocket,
    EPermissionConnectorServerSocket,
    EPermissionConnectorSsl,
    EPermissionConnectorComm,
    EPermissionPushRegistry,

    // Wireless Messaging API  JSRs 120 & 205
    EPermissionConnectorSms,
    EPermissionWirelessSmsSend,
    EPermissionWirelessSmsReceive,
    EPermissionConnectorCbs,
    EPermissionWirelessCbsReceive,
    EPermissionConnectorMms,       // JSR 205
    EPermissionWirelessMmsSend,    // JSR 205
    EPermissionWirelessMmsReceive, // JSR 205


    // Mobile Media API JSR 135
    EPermissionMediaRecordControl,
    EPermissionMediaGetSnapshot,
    EPermissionConnectorRtsp,

    // Bluetooth API JSR 82
    EPermissionConnectorBtClient,
    EPermissionConnectorBtServer,
    EPermissionConnectorObexClient,
    EPermissionConnectorObexClientTcp, //Not supported in releases before and including S60 3.2
    EPermissionConnectorObexServer,
    EPermissionConnectorObexServerTcp, //Not supported in releases before and including S60 3.2
    EPermissionJavaxObexClientSession,
    EPermissionJavaxObexOperation,

    // File API JSR 75
    EPermissionConnectorFileRead,
    EPermissionConnectorFileWrite,

    // PIM API JSR 75
    EPermissionPimContactListRead,
    EPermissionPimContactListWrite,
    EPermissionPimEventListRead,
    EPermissionPimEventListWrite,
    EPermissionPimToDoListRead,
    EPermissionPimToDoListWrite,

    // Location API JSR 179
    EPermissionLocationLocation,
    EPermissionLocationOrientation, // Not supported in releases before and including S60 3.2, still not commented out because handled by not defining fg binding in policy file
    EPermissionLocationProximityListener, //Not supported in releases before and including S60 3.2
    EPermissionLocationLandmarkStoreRead,
    EPermissionLocationLandmarkStoreWrite,
    EPermissionLocationLandmarkStoreCategory,
    EPermissionLocationLandmarkStoreManagement,

    // SATSA API JSR 177
    EPermissionSATSAAPDUApduSat, // bind to dummy functiongroup "SATSA APDU"
    EPermissionSmartCardCommunicationApduAid, // Not supported in releases before and including S60 3.2, still not commented out because handled by not defining fg binding in policy file
    EPermissionSmartCardCommunicationJcrmi, //Not supported in 3.1
    EPermissionAuthenticationSecServCMSMsgSignServ,

    // SIP API JSR 180
    EPermissionConnectorSip,
    EPermissionConnectorSips,

    // Content Handler JSR 211
//    EPermissionContentContentHandler, Not supported in releases before and including S60 3.2

    // API Extensions
    EPermissionAPIEXTActiveIdleSet,
    EPermissionAPIEXTSasfSet,
    EPermissionAPIEXTSasfGet,
    EPermissionAPIEXTSoftNotificationSet,
    EPermissionAPIEXTNewMailLEDSet,

    // Restricted API Extensions
    EPermissionAPIEXTMobinfoIMSI,

    // Platform Request
    EPermissionPlatformRequestNetAccess,
    EPermissionPlatformRequestTel,
    EPermissionPlatformRequestLocalApp,

    //new
    EPermissionAmmsControlCameraEnableShutterFeedback,
    EPermissionAmmsControlTunerSetPreset,
    EPermissionPaymentProcess,

    // API JSR 272
    EPermissionBroadcastRecordingRecordingSchedulerAdd,
    EPermissionBroadcastRecordingRecordingSchedulerRemove,
    EPermissionBroadcastRecordingRecordingSchedulerAccess,
    EPermissionBroadcastEsgServiceGuideAccess,
    EPermissionBroadcastServiceContextCreate,
    EPermissionBroadcastServiceContextDefault,
    EPermissionBroadcastServiceContextSelect,
    EPermissionBroadcastPurchaseSubscriptionManagerGetSubscriptions,
    EPermissionBroadcastPurchasePurchaseObjectPurchase,
    EPermissionBroadcastPurchasePurchaseObjectCancel,
    EPermissionBroadcastServiceContextBroadcastDatagram,
    EPermissionIoConnectorBroadcastFileRead,

    ELastPermissionMarker
};

static const TUint16* const KJcfPermissions[] =
{
    // MIDP 2.0 JSR 118
    _S16("javax.microedition.io.Connector.http"),  // 0
    _S16("javax.microedition.io.Connector.https"),
    _S16("javax.microedition.io.Connector.datagram"),
    _S16("javax.microedition.io.Connector.datagramreceiver"),
    _S16("javax.microedition.io.Connector.socket"),
    _S16("javax.microedition.io.Connector.serversocket"),
    _S16("javax.microedition.io.Connector.ssl"),
    _S16("javax.microedition.io.Connector.comm"),
    _S16("javax.microedition.io.PushRegistry"),

    // Wireless Messaging API  JSRs 120 & 205
    _S16("javax.microedition.io.Connector.sms"),
    _S16("javax.wireless.messaging.sms.send"),
    _S16("javax.wireless.messaging.sms.receive"),
    _S16("javax.microedition.io.Connector.cbs"),
    _S16("javax.wireless.messaging.cbs.receive"),
    _S16("javax.microedition.io.Connector.mms"),  // JSR 205
    _S16("javax.wireless.messaging.mms.send"),    // JSR 205
    _S16("javax.wireless.messaging.mms.receive"), // JSR 205

    // Mobile Media API JSR 135
    _S16("javax.microedition.media.control.RecordControl"),
    _S16("javax.microedition.media.control.VideoControl.getSnapshot"),
    _S16("javax.microedition.io.Connector.rtsp"),

    // Bluetooth API JSR 82
    _S16("javax.microedition.io.Connector.bluetooth.client"),
    _S16("javax.microedition.io.Connector.bluetooth.server"),
    _S16("javax.microedition.io.Connector.obex.client"),
    _S16("javax.microedition.io.Connector.obex.client.tcp"), //Not supported in releases before and including S60 3.2
    _S16("javax.microedition.io.Connector.obex.server"),
    _S16("javax.microedition.io.Connector.obex.server.tcp"), //Not supported in releases before and including S60 3.2
    _S16("javax.obex.ClientSession"),
    _S16("javax.obex.Operation"),

    // File API JSR 75
    _S16("javax.microedition.io.Connector.file.read"),
    _S16("javax.microedition.io.Connector.file.write"),

    // PIM API JSR 75
    _S16("javax.microedition.pim.ContactList.read"),
    _S16("javax.microedition.pim.ContactList.write"),
    _S16("javax.microedition.pim.EventList.read"),
    _S16("javax.microedition.pim.EventList.write"),
    _S16("javax.microedition.pim.ToDoList.read"),
    _S16("javax.microedition.pim.ToDoList.write"),

    // Location API JSR 179
    _S16("javax.microedition.location.Location"),
    _S16("javax.microedition.location.Orientation"), // Not supported in releases before and including S60 3.2, still not commented out because handled by not defining fg binding in policy file
    _S16("javax.microedition.location.ProximityListener"), //Not supported in releases before and including S60 3.2
    _S16("javax.microedition.location.LandmarkStore.read"),
    _S16("javax.microedition.location.LandmarkStore.write"),
    _S16("javax.microedition.location.LandmarkStore.category"),
    _S16("javax.microedition.location.LandmarkStore.management"),

    // SATSA API JSR 177
    _S16("javax.microedition.apdu.sat"), // bind to dummy functiongroup "SATSA APDU"
    _S16("javax.microedition.apdu.aid"), //// Not supported in releases before and including S60 3.2, still not commented out because handled by not defining fg binding in policy file
    _S16("javax.microedition.jcrmi"), //Not supported in 3.1
    _S16("javax.microedition.securityservice.CMSMessageSignatureService"),

    // SIP API JSR 180
    _S16("javax.microedition.io.Connector.sip"),
    _S16("javax.microedition.io.Connector.sips"),

    // Content Handler JSR 211
//    _S16("javax.microedition.content.ContentHandler"), Not supported in releases before and including S60 3.2

    // API Extensions
    _S16("com.nokia.mid.ActiveIdle.set"),
    _S16("com.nokia.mid.sasf.set"),
    _S16("com.nokia.mid.sasf.get"),
    _S16("com.nokia.mid.SoftNotification.set"),
    _S16("com.nokia.mid.NewMailLED.set"),

    // Restricted API Extensions
    _S16("com.nokia.mid.Mobinfo.IMSI"),

    // Platform Request
    _S16("com.nokia.midp.platformrequest.netaccess"),
    _S16("com.nokia.midp.platformrequest.tel"),
    _S16("com.nokia.midp.platformrequest.localapp"),

    //new
    _S16("javax.microedition.amms.control.camera.enableShutterFeedback"),
    _S16("javax.microedition.amms.control.tuner.setPreset"),
    _S16("javax.microedition.payment.process"),

    // API JSR 272
    _S16("javax.microedition.broadcast.recording.RecordingScheduler.add"),
    _S16("javax.microedition.broadcast.recording.RecordingScheduler.remove"),
    _S16("javax.microedition.broadcast.recording.RecordingScheduler.access"),
    _S16("javax.microedition.broadcast.esg.ServiceGuide.access"),
    _S16("javax.microedition.broadcast.ServiceContext.create"),
    _S16("javax.microedition.broadcast.ServiceContext.default"),
    _S16("javax.microedition.broadcast.ServiceContext.select"),
    _S16("javax.microedition.broadcast.purchase.SubscriptionManager.getSubscriptions"),
    _S16("javax.microedition.broadcast.purchase.PurchaseObject.purchase"),
    _S16("javax.microedition.broadcast.purchase.PurchaseObject.cancel"),
    _S16("javax.microedition.broadcast.ServiceContext.broadcastdatagram"),
    _S16("javax.microedition.io.Connector.broadcastfile.read")
};

//
// ********************************************************************
// Function grouping mappings
// ********************************************************************
//

// ** Please keep in synch with the below array!
enum TMidp2FunctionGroup
{
    EPhoneCall = 0,
    ENetAccess,
    ELowLevelNetAccess,
    ECallControl,
    EMessaging,
    ERestrictedMessaging,
    EAppAutoInvocation,
    ELocalConnectivity,
    EMultimediaRecording,
    EReadUserDataAccess,
    EWriteUserDataAccess,
    ELocation,
    ELandmark,
    EOrientationDummy,  // Dummy function group due to the fact that Orientation
    // permission is not tied to any real Func.group...
    ESatsaApduDummy,    // Dummy fg defined for SATSA permission javax.microedition.apdu.sat which
    // is not bind to any of those official function groups. See JSR 177, page 157.
    EAuthentication,
    ESmartCardCommunication,
    EAPIExtensions,             // Dummy fg defined for API Extensions such as ActiveIdle, SASF, etc.
    ERestrictedAPIExtensions,   // Dummy fg defined for API Extensions such as Mobinfo.
    EPlatformRequest,           // Dummy fg defined for PlatformRequest
    EMobileBroadcastingService,
    ELastFunctionGroupMarker
};

// Must be in sync with the previous enums AND ones defined in *.xpf policy file!
static const TUint16* const KJcfFunctionGroups[] =
{
    _S16("Phone Call"),
    _S16("Net Access"),
    _S16("Low Level Net Access"),
    _S16("Call Control"),
    _S16("Messaging"),
    _S16("Restricted Messaging"),
    _S16("Application Auto Invocation"),
    _S16("Local Connectivity"),
    _S16("Multimedia Recording"),
    _S16("Read User Data Access"),
    _S16("Write User Data Access"),
    _S16("Location"),
    _S16("Landmark"),
    _S16("Orientation"),   // Dummy
    _S16("Unassigned APIs"),    // Dummy
    _S16("Authentication"),
    _S16("Smart Card Communication"),
    _S16("API Extensions"), // Dummy
    _S16("Restricted API Extensions"), // Dummy
    _S16("Platform Request"), // Dummy
    _S16("Mobile Broadcasting Service")
};

// The function group visibility part of the enum must be kept in sync with TMidp2FunctionGroup
enum TMidp2ConfigKey
{
    ECFGfgPhoneCallVisible = 0,
    ECFGfgNetAccessVisible,
    ECFGfgLowLevelNetAccessVisible,
    ECFGfgCallControlVisible,
    ECFGfgMessagingVisible,
    ECFGfgRestrictedMessagingVisible,
    ECFGfgAppAutoInvocationVisible,
    ECFGfgLocalConnectivityVisible,
    ECFGfgMultimediaRecordingVisible,
    ECFGfgReadUserDataAccessVisible,
    ECFGfgWriteUserDataAccessVisible,
    ECFGfgLocationVisible,
    ECFGfgLandmarkVisible,
    ECFGfgOrientationDummyVisible,
    ECFGfgSatsaApduDummyVisible,
    ECFGfgAuthenticationVisible,
    ECFGfgSmartCardCommunicationVisible,
    ECFGfgAPIExtensionsVisible,
    ECFGfgRestrictedAPIExtensionsVisible,
    ECFGfgPlatformRequestVisible,
    ECFGfgMobileBroadcastingServiceVisible,
    ECFGfgLastFunctionGroupVisibleMarker
};

class Midp2PermissionDB : public CBase
{
public:
    Midp2PermissionDB();
    virtual ~Midp2PermissionDB();

    IMPORT_C static Midp2PermissionDB* NewL(const TDesC& aMidletSuiteName,
                                            const TDesC& aVendor,
                                            const TDesC& aVersion);

    IMPORT_C TInt GetDomain(TMidp2Domain& aDomain);

    IMPORT_C TInt GetFunctionGroupPermission(
        const enum TMidp2FunctionGroup& aFunctionGroup,
        TMidp2PermissionType& aCurrentType,
        TMidp2PermissionMode& aCurrentMode,
        TUint8& aAllowedModesMask);

    IMPORT_C TInt SetFunctionGroupPermission(
        const enum TMidp2FunctionGroup& aFunctionGroup,
        const TMidp2PermissionType& aCurrentType,
        const TMidp2PermissionMode& aCurrentMode);

    IMPORT_C void ReadMIDP2ConfigSetting(
        const enum TMidp2ConfigKey& aKey, TInt& aKeyValue);

    // **************************************************************************
    // Helper functions to convert string names vs. enumarated values
    // **************************************************************************
    //
    // Finds the enumerated domain value from the given plain text domain name.
    //
    IMPORT_C static TInt GetDomainFromName(
        const TDesC& aName, TMidp2Domain& aDomain);
    //
    // Finds the enumerated function group value from the given plain text
    // permission name.
    //
    IMPORT_C static TInt GetFunctionGroupFromName(
        const TDesC& aName, TMidp2FunctionGroup& aFunctionGroup);
    //
    // Finds the enumerated permission value from the given plain text
    // permission name.
    //
    IMPORT_C static TInt GetPermissionFromName(const TDesC& aName,
            TMidp2Permission& aPermission);
private:
    void ConstructL(const TDesC& aMidletSuiteName, const TDesC& aMidletSuiteVendor, const TDesC& aMidletSuiteVersion);
    std::wstring iMidletSuiteUid;
    std::wstring iMidletSuiteProtectionDomain;
    TBool IsFunctionGroupGranted(TInt aFGIndex);
};

} // namespace MIDP
#endif // MIDP2PERMISSIONDB_H
