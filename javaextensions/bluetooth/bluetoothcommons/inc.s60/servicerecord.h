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


#ifndef SERVICERECORD_H
#define SERVICERECORD_H

#include <e32std.h>
#include <bttypes.h>
#include <btsdp.h>
#include <bt_sock.h>
#include <stdio.h>

#include "bluetoothfunctionserver.h"
#include "javajniutils.h"

namespace java
{
namespace bluetooth
{

class ServiceRecord
{
public:
    //
    // Methods related to Discovery Agent
    //
    OS_IMPORT
    static ServiceRecord* NewL(BluetoothFunctionServer* aServer);

    OS_IMPORT static void cleanup(ServiceRecord*& aServer);

    OS_IMPORT
    void initializeRecordL(TInt aProtocol, TInt aPsmValue, TUUID &aServiceUUID,
                           TDesC8 &aServiceName);

    OS_IMPORT
    int initializeRecord(int aProtocol, int aPSMValue, std::wstring aUuid,
                         std::wstring aServiceName);

    OS_IMPORT int restorePersistentRecord();

    OS_IMPORT int attributeListStart(int aAttrId, int aAttrType);

    OS_IMPORT int attributeListEnd(int aAttrId);

    OS_IMPORT int setAttributeUint(int aAttrId, int aAttrType, TDesC8 &aNumVal);

    OS_IMPORT int setAttributeInt(int aAttrId, int aAttrType, TDesC8 &aNumVal);

    OS_IMPORT int setAttributeBool(int aAttrId, TBool aBoolVal);

    OS_IMPORT int setAttributeUUID(int aAttrId, TUUID &aUUIDVal);

    OS_IMPORT int setAttributeString(int aAttrId, TDesC8 &aStrVal);

    OS_IMPORT int setAttributeUrl(int aAttrId, TDesC8 &aUrlVal);

    OS_IMPORT int setAttributeNil(int aAttrId);

    // Only used by ServerConnection::AcceptL & ServerConnection::HandleAcceptL
    OS_IMPORT int setAdvertiseFs(TBool aAvailable);

    OS_IMPORT void initializeUpdateRecord(int aDeviceServiceClasses);

    OS_IMPORT void completesUpdateRecord();

    OS_IMPORT int getServiceRecordID();

    OS_IMPORT
    void restoreJavaServiceRecordL(JNIEnv *aJni, jobject aServiceRecordImpl);

private:
    ~ServiceRecord();

    void ConstructL();

    void cleanUp();

    void initializeRecordFsL(TInt aProtocol, TInt aPsmValue,
                             TUUID &aServiceUUID, TDesC8 &aServiceName);

    int initializeRecordFs(int aProtocol, int aPSMValue, std::wstring aUuid,
                           std::wstring aServiceName);

    int restorePersistentRecordFs();

    TInt8 charToInt(wchar_t ch);

    int attributeListStartFs(int aAttrId, int aAttrType,
                             TBool isPersistenceRequired = ETrue);

    int attributeListEndFs(int aAttrId, TBool isPersistenceRequired = ETrue);

    int setAttributeUintFs(int aAttrId, int aAttrType, TDesC8 &aNumVal,
                           TBool isPersistenceRequired = ETrue);

    int setAttributeIntFs(int aAttrId, int aAttrType, TDesC8 &aNumVal,
                          TBool isPersistenceRequired = ETrue);

    int setAttributeBoolFs(int aAttrId, TBool aBoolVal,
                           TBool isPersistenceRequired = ETrue);

    int setAttributeUUIDFs(int aAttrId, TUUID &aUUIDVal,
                           TBool isPersistenceRequired = ETrue);

    int setAttributeStringFs(int aAttrId, TDesC8 &aStrVal,
                             TBool isPersistenceRequired = ETrue);

    int setAttributeUrlFs(int aAttrId, TDesC8 &aUrlVal,
                          TBool isPersistenceRequired = ETrue);

    int setAttributeNilFs(int aAttrId, TBool isPersistenceRequired = ETrue);

    ServiceRecord(BluetoothFunctionServer* aServer);

    HBufC8* convertIntToDesC(TUint8 *aBytes, int aDataType);

    TUint8 *uuidToByteArray(std::wstring aUuid, int &aUuidByteLength);

    std::wstring getPersistentFileName();

    std::wstring uuidToWstring(TUUID aUuid);

private:
    // Discovery Agent Data
    BluetoothFunctionServer* mFunctionServer;

    // Holds full set of attributes
    CSdpAttrValueList* mSdpAttrValueList;

    // Refers to the last attribute of the full set of attributes
    MSdpElementBuilder *mLastSdpAttr;

    // Service discovery protocol session
    RSdp mSdp;

    // Service discovery database (sdp)
    RSdpDatabase mSdpDB;

    // Service record Handle
    TSdpServRecordHandle mRecord;

    // Members related to restore Java Service Record.
    // Flag related to Persistent Records.
    bool mInitializedByPush;
    bool mRestoredFromPersistentFile;

    // Increments each time record updates
    int mRecordState;

    // Persistent file handler
    int mSrvRecFd;
    TInt mProtocol;

    // UUID & protocol stored for the use of constructing
    // persistent storage file name
    std::wstring mOrigUuid;
};

} //end namespace bluetooth
} //end namespace java


#endif // SERVICERECORD_H
