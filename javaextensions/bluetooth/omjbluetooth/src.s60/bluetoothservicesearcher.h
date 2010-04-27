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
* Description:
*
*/


#ifndef BLUETOOTHSERVICESEARCHER_H
#define BLUETOOTHSERVICESEARCHER_H

#include <string>
#include <btsdp.h>
#include <bt_sock.h>
#include <bttypes.h>
#include <e32base.h>

#include "logger.h"
#include "bluetoothfunctionserver.h"
#include "btservicerecordpopulator.h"
//#include "discoveryagent.h"

namespace java
{
namespace bluetooth
{

class DiscoveryAgent;

class BluetoothServiceSearcher: public MSdpAgentNotifier
{
public:
    static BluetoothServiceSearcher* New(DiscoveryAgent *aDiscoveryAgent,
                                         BluetoothFunctionServer* aServer);

    ~BluetoothServiceSearcher();

    void SearchServicesL(TInt64 aRemoteAddress, TPtrC8 aUuidsDes,
                         TPtrC16 aAttrIdsDes);

    void cancelServiceSearch();

    // From MSdpAgentNotifier
    void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle,
                                   TInt aTotalRecordsCount);

    void AttributeRequestResult(TSdpServRecordHandle aHandle,
                                TSdpAttributeID aAttrID, CSdpAttrValue *aAttrValue);

    void AttributeRequestComplete(TSdpServRecordHandle, TInt aError);

    void PopulateServiceRecordsL(TInt64 aRemoteAddress, long aHandle,
                                 TPtrC16 aAttrIdsDes, jobject aServiceRecordImpl);
private:
    BluetoothServiceSearcher();

    void Construct(DiscoveryAgent *aDiscoveryAgent,
                   BluetoothFunctionServer* aServer);

    void FillSearchPatternL(TDesC8& uuidsBytes);

    void FillAttributeListL(TDesC16& attrIds);

    void Cleanup();

private:
    CSdpAgent* mSDPAgent;
    CSdpSearchPattern* mSpat;
    CSdpAttrIdMatchList* mAttributeList;
    DiscoveryAgent* mDiscoveryAgent;
    BluetoothFunctionServer* mServer;
    BTServiceRecordPopulator* mSrPopulator;

    TUUID mUUID;
    TInt64 mRemoteDeviceAddress;
    bool mFoundServiceRecord;
    bool mIsPopulateRecordMode;

    JNIEnv* mJni;
    jobject mPeer;
    jclass mServiceRecordImplClass;
    jmethodID mCreateServiceRecordMethod;
    jobject mServiceRecordImpl;
};

} //end namespace bluetooth
} //end namespace java

#endif // BLUETOOTHSERVICESEARCHER_H
