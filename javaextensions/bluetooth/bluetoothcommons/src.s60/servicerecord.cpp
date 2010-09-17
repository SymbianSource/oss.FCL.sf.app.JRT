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


#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "fs_methodcall.h"
#include "javajniutils.h"
#include "servicerecord.h"
#include "logger.h"
#include "javasymbianoslayer.h"
#include "bluetoothconsts.h"
#include "applicationinfo.h"
#include "fileutilities.h"
#include "javaoslayer.h"
#include "javacommonutils.h"
#include "btservicerecordpopulator.h"
#include "serviceclasshandler.h"
#include "fileutilities.h"
#include "exceptionbase.h"

using namespace java::util;
using namespace java::fileutils;

namespace java
{
namespace bluetooth
{

#define SERVICE_AVAILABLE 0xFF
#define KGOEP 0x0008    // This is the fixed Id. Should not be modified at any time.
#define SERVICE_CLASSES_BITS_POS    13

// Permission: Read,Write,Execute for both Owner & Group
#define BT_DIR_PERM 0777

/**
 * Returns the new ServiceRecord object.
 */
OS_EXPORT ServiceRecord* ServiceRecord::NewL(BluetoothFunctionServer* aServer)
{
    JELOG2(EJavaBluetooth);
    ServiceRecord* servRec = new ServiceRecord(aServer);
    CleanupStack::PushL(servRec);

    servRec->ConstructL();

    CleanupStack::Pop(servRec);

    return servRec;
}

OS_EXPORT void ServiceRecord::cleanup(ServiceRecord*& aServer)
{
    JELOG2(EJavaBluetooth);
    if (aServer)
    {
        aServer->cleanUp();
        delete aServer;
        aServer = NULL;
    }
}

void ServiceRecord::ConstructL()
{
    JELOG2(EJavaBluetooth);

    mInitializedByPush = false;
    mRestoredFromPersistentFile = false;

    // Open sdp session
    User::LeaveIfError(mSdp.Connect());

    // Open sdp database session
    User::LeaveIfError(mSdpDB.Open(mSdp));

    mRecordState = 0;
}

ServiceRecord::ServiceRecord(BluetoothFunctionServer* aServer)
{
    JELOG2(EJavaBluetooth);
    mFunctionServer = aServer;
    mRecord = 0;
    mSrvRecFd = 0;
}

ServiceRecord::~ServiceRecord()
{
    JELOG2(EJavaBluetooth);
    mFunctionServer = NULL;
}

void ServiceRecord::cleanUp()
{
    JELOG2(EJavaBluetooth);
    if (mRecord)
    {
        TRAP_IGNORE(mSdpDB.DeleteRecordL(mRecord));
        mRecord = 0;
    }
    mSdpDB.Close();
    mSdp.Close();
}

/**
 * Used in case there is no persistence storage of service record
 * On success returns 0; otherwise err number.
 *
 * Creates and initializes the Service record.
 * aProtocol: One among the PROTOCOL_L2CAP, PROTOCOL_RFCOMM, PROTOCOL_GOEP
 * aPSMValue: In case of L2CAP it should be PSM Value;
 *            In case of RFComm it should be channel number.
 * aUuid: UUID string of the current service
 * aServiceName: Name of the current service.
 */

OS_EXPORT int ServiceRecord::initializeRecord(int aProtocol, int aPSMValue,
        std::wstring aUuid, std::wstring aServiceName)
{
    JELOG2(EJavaBluetooth);

    int uuidByteLength = 0;
    TUint8* uuidBytes = uuidToByteArray(aUuid, uuidByteLength);

    TUUID uuid;

    TPtrC8 uuidDes(uuidBytes, uuidByteLength);
    TRAPD(err, uuid.SetL(uuidDes));

    delete[] uuidBytes;
    uuidBytes = NULL;

    if (KErrNone != err)
    {
        LOG2(EJavaBluetooth, EInfo,
             "- ServiceRecord::initializeRecord Error SetL:%d [UUID:%S]",
             err, aUuid.c_str());
        return err;
    }

    HBufC* nameString = wstringToBuf(aServiceName);
    TPtrC nameBufPointer(nameString->Des());

    HBufC8* utf8NameBuffer = HBufC8::NewMax(aServiceName.length() + 1);
    TPtr8 utf8NameBufferPtr(utf8NameBuffer->Des());

    utf8NameBufferPtr.Copy(nameBufPointer);

    TRAP(err, initializeRecordL(aProtocol, aPSMValue, uuid, utf8NameBufferPtr));

    return err;
}

/**
 * Used in case of persistence storage of updated service record
 */
OS_EXPORT int ServiceRecord::restorePersistentRecord()
{
    JELOG2(EJavaBluetooth);
    int ret = 0;

    CallMethod(ret, this, &ServiceRecord::restorePersistentRecordFs,
               mFunctionServer);
    return ret;
}

int ServiceRecord::restorePersistentRecordFs()
{
    JELOG2(EJavaBluetooth);

    // Can set this flag as this function will get called
    // only in case of push invocation
    mInitializedByPush = true;

    std::wstring persistentFileName = getPersistentFileName();
    int persistentRecFd = -1;
    int ret = 0;
    int err = 0;

    if (persistentFileName.length() > 0)
    {
        try
        {
            char *fileName = java::util::JavaCommonUtils::wstringToUtf8(
                                 persistentFileName);
            persistentRecFd = open(fileName, O_RDONLY);
            delete[] fileName;
        }
        catch (ExceptionBase &ex)
        {
            ELOG1(
                EJavaBluetooth,
                "- ServiceRecord::restorePersistentRecordFs exception Caught: %S",
                ex.toString().c_str());
        }
    }

    if (persistentRecFd < 0)
        return errno;

    // Reading and restoring the DeviceServiceClasses bits
    int devServClass = 0;
    ret = read(persistentRecFd, &devServClass, sizeof(devServClass));

    if (ret <= 0)
        return errno;

    if (devServClass != 0)
    {
        LOG1(EJavaBluetooth, EInfo,
             "  ServiceRecord::restorePersistentRecordFs Setting COD:0x%X",
             devServClass);
        ret = ServiceClassHandler::setDeviceServiceClass(devServClass, true);
        LOG1(EJavaBluetooth, EInfo,
             "  ServiceRecord::restorePersistentRecordFs Set COD Return:%d",
             ret);
    }

    // Restoring all other service record data
    int attrId = 0;
    while ((ret = read(persistentRecFd, &attrId, sizeof(attrId))) > 0)
    {
        int dataType = 0;

        ret = read(persistentRecFd, &dataType, sizeof(dataType));
        if (ret <= 0)
            break;

        LOG2(EJavaBluetooth, EInfo, "  ServiceRecord reading %X, %X", attrId,
             dataType);

        switch (dataType)
        {
        case DataElement_DATSEQ:
        case DataElement_DATALT:
        {
            err = attributeListStartFs(attrId, dataType, EFalse);
            break;
        }
        case DataElement_EndList:
        {
            err = attributeListEndFs(attrId, EFalse);
            break;
        }
        case DataElement_NULL:
        {
            err = setAttributeNilFs(attrId, EFalse);
            break;
        }
        case DataElement_BOOL:
        {
            TUint8 val = 0;
            ret = read(persistentRecFd, &val, sizeof(val));
            if (ret <= 0)
                break;

            LOG1(EJavaBluetooth, EInfo, "  ServiceRecord reading Bool %X", val);
            err = setAttributeBoolFs(attrId, (val) ? ETrue : EFalse, EFalse);
            break;
        }
        case DataElement_INT_1:
        case DataElement_INT_2:
        case DataElement_INT_4:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >4))
                break;

            TUint8 buf[4] = { 0 };
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;
            std::auto_ptr<HBufC8> val(convertIntToDesC(buf, dataType));
            err = setAttributeIntFs(attrId, dataType, *val, EFalse);
            break;
        }
        case DataElement_INT_8:
        case DataElement_INT_16:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >16))
                break;

            TUint8 buf[16] = { 0 };
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;
            TPtrC8 val(buf, len);
            err = setAttributeIntFs(attrId, dataType, val, EFalse);
            break;
        }
        case DataElement_U_INT_1:
        case DataElement_U_INT_2:
        case DataElement_U_INT_4:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >4))
                break;

            TUint8 buf[4] = { 0 };
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo, "  ServiceRecord reading INT8 %X, %S",
                 len, buf);

            std::auto_ptr<HBufC8> val(convertIntToDesC(buf, dataType));
            err = setAttributeUintFs(attrId, dataType, *val, EFalse);
            break;
        }
        case DataElement_U_INT_8:
        case DataElement_U_INT_16:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >16))
                break;

            TUint8 buf[16] = { 0 };
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo, "  ServiceRecord reading INT16 %X, %S",
                 len, buf);

            TPtrC8 val(buf, len);
            err = setAttributeUintFs(attrId, dataType, val, EFalse);
            break;
        }
        case DataElement_UUID:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >256))
                break;

            TUint8 buf[256] = { 0 };
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo, "  ServiceRecord reading UUID %X, %S",
                 len, buf);

            TPtrC8 uuidDes(buf, len);

            TUUID val;
            TRAP(err, val.SetL(uuidDes));

            if (KErrNone == err)
            {
                err = setAttributeUUIDFs(attrId, val, EFalse);
            }
            break;
        }
        case DataElement_STRING:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >1024))
                break;

            TUint8 buf[1024] = {0};
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG1(EJavaBluetooth, EInfo, "  ServiceRecord reading STRING %X",
                 len);

            TPtrC8 val(buf, len);
            err = setAttributeStringFs(attrId, val, EFalse);
            break;
        }
        case DataElement_URL:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >1024))
                break;

            TUint8 buf[1024] = {0};
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo, "  ServiceRecord reading URL %X, %S",
                 len, buf);

            TPtrC8 val(buf, len);
            err = setAttributeUrlFs(attrId, val, EFalse);
            break;
        }
        default:
            break;
        }

        // Treated as EOF reached.
        if (ret <= 0)
            break;
    }

    // Indicates a successful retrieval
    // of the service record from the persistent file
    mRestoredFromPersistentFile = true;

    return ret;
}

/**
 * Creates and initializes the Service record.
 * aProtocol: One among the PROTOCOL_L2CAP, PROTOCOL_RFCOMM, PROTOCOL_GOEP
 * aPSMValue: In case of L2CAP it should be PSM Value;
 *            In case of RFComm it should be channel number.
 * aServiceUUID: UUID of the current service
 * aServiceName: Name of the current service.
 */
OS_EXPORT void ServiceRecord::initializeRecordL(TInt aProtocol, TInt aPsmValue,
        TUUID &aServiceUUID, TDesC8 &aServiceName)
{
    JELOG2(EJavaBluetooth);
    CallMethodL(this, &ServiceRecord::initializeRecordFsL, aProtocol,
                aPsmValue, aServiceUUID, aServiceName, mFunctionServer);
}

/**
 * Returns the service record handle.
 */
OS_EXPORT int ServiceRecord::getServiceRecordID()
{
    return mRecord;
}

void ServiceRecord::initializeRecordFsL(TInt aProtocol, TInt aPsmValue,
                                        TUUID &aServiceUUID, TDesC8 &aServiceName)
{
    JELOG2(EJavaBluetooth);
    // Open sdp session
    User::LeaveIfError(mSdp.Connect());
    // Open sdp database session
    User::LeaveIfError(mSdpDB.Open(mSdp));

    LOG1(
        EJavaBluetooth,
        EInfo,
        "  ServiceRecord::initializeRecordFsL: After registering to SDPDB: %S",
        aServiceUUID.Des().Ptr());

    // Creating proper ServiceClassID list
    CSdpAttrValueDES *serviceClassIds = CSdpAttrValueDES::NewDESL(NULL);
    serviceClassIds->BuildUUIDL(aServiceUUID);
    if (PROTOCOL_RFCOMM == aProtocol)
    {
        LOG(EJavaBluetooth, EInfo,
            "  ServiceRecord::initializeRecordFsL: Adding KSerialPortUUID");

        serviceClassIds->BuildUUIDL(TUUID(KSerialPortUUID));
    }

    // Create a record of the correct service class
    mSdpDB.CreateServiceRecordL(*serviceClassIds, mRecord);

    // Add a protocol to the record
    mSdpAttrValueList = CSdpAttrValueDES::NewDESL(NULL);
    mLastSdpAttr = mSdpAttrValueList;
    CleanupStack::PushL(mSdpAttrValueList);

    // Getting port number
    TSdpIntBuf<TUint> portBuf(aPsmValue);
    LOG1(EJavaBluetooth, EInfo,
         "  ServiceRecord::initializeRecordFsL: port value : %d", aPsmValue);

    LOG(EJavaBluetooth, EInfo,
        "  ServiceRecord::initializeRecordFsL: Creating protocol list");

    // Create protocol list for our service

    switch (aProtocol)
    {
    case PROTOCOL_L2CAP:
        LOG(EJavaBluetooth, EInfo,
            "  ServiceRecord::initializeRecordFsL: PROTOCOL_L2CAP");
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildDESL();
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildUUIDL(KL2CAP);
        mLastSdpAttr = mLastSdpAttr -> BuildUintL(portBuf);
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        break;
    case PROTOCOL_RFCOMM:
        LOG(EJavaBluetooth, EInfo,
            "  ServiceRecord::initializeRecordFsL: PROTOCOL_RFCOMM");
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildDESL();
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildUUIDL(KL2CAP);
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        mLastSdpAttr = mLastSdpAttr -> BuildDESL();
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildUUIDL(KRFCOMM);
        mLastSdpAttr = mLastSdpAttr -> BuildUintL(portBuf);
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        break;
    case PROTOCOL_GOEP:
        LOG(EJavaBluetooth, EInfo,
            "  ServiceRecord::initializeRecordFsL: PROTOCOL_GOEP");
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildDESL();
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildUUIDL(KL2CAP);
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        mLastSdpAttr = mLastSdpAttr -> BuildDESL();
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildUUIDL(KRFCOMM);
        mLastSdpAttr = mLastSdpAttr -> BuildUintL(portBuf);
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        mLastSdpAttr = mLastSdpAttr -> BuildDESL();
        mLastSdpAttr = mLastSdpAttr -> StartListL();
        mLastSdpAttr = mLastSdpAttr -> BuildUUIDL(KGOEP);
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        mLastSdpAttr = mLastSdpAttr -> EndListL();
        break;
    }

    LOG(EJavaBluetooth, EInfo,
        "  ServiceRecord::initializeRecordFsL: Set protocol list to the record");
    // Set protocol list to the record
    mSdpDB.UpdateAttributeL(mRecord, KSdpAttrIdProtocolDescriptorList,
                            *mSdpAttrValueList);
    CleanupStack::PopAndDestroy(mSdpAttrValueList);

    mSdpAttrValueList = NULL;
    mLastSdpAttr = NULL;

    LOG(EJavaBluetooth, EInfo,
        "  ServiceRecord::initializeRecordFsL: Update attribute");

    if (aServiceName.Size() > 0)
    {
        // Add a name to the record
        mSdpDB.UpdateAttributeL(mRecord, KSdpAttrIdBasePrimaryLanguage
                                + KSdpAttrIdOffsetServiceName, aServiceName);
        LOG1(EJavaBluetooth, EInfo,
             "  ServiceRecord::initializeRecordFsL: Update Name: %d",
             KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName);
    }

    mOrigUuid = uuidToWstring(aServiceUUID);

    LOG1(EJavaBluetooth, EInfo,
         "  ServiceRecord::initializeRecordFsL mOrigUuid = %S",
         mOrigUuid.c_str());

    mProtocol = aProtocol;
}

/**
 * Opens the persistent file. Called when Update Record triggered.
 * And stores the DeviceServiceClasses bits (32 bit) in the persistent file
 * While retrieving the service record back from the file, it is must to read first
 * one integer which depicts this DeviceServiceClasses. If the service classes is not
 * set for the corresponding service, then the value 0(32 bits) is stored in place of the
 * DeviceServiceClasses bits.
 *
 */
OS_EXPORT void ServiceRecord::initializeUpdateRecord(int aDeviceServiceClasses)
{
    JELOG2(EJavaBluetooth);
    std::wstring persistentFileName;

    persistentFileName = getPersistentFileName();

    if (persistentFileName.length() > 0)
    {
        try
        {
            char *fileName = java::util::JavaCommonUtils::wstringToUtf8(
                                 persistentFileName);
            mSrvRecFd = open(fileName, O_CREAT | O_WRONLY | O_TRUNC, 0666);

            if (mSrvRecFd < 0)
            {
                ELOG1(
                    EJavaBluetooth,
                    "- ServiceRecord::initializeUpdateRecord Err while file open. %d",
                    errno);
                return;
            }
            // Storing the Current DeviceClass (32 bits)
            write(mSrvRecFd, (char *) &aDeviceServiceClasses,
                  sizeof(aDeviceServiceClasses));

            delete[] fileName;
        }
        catch (ExceptionBase &ex)
        {
            ELOG1(
                EJavaBluetooth,
                "- ServiceRecord::initializeUpdateRecord exception Caught: %S",
                ex.toString().c_str());
        }
    }
}

/**
 * Closes the persistent file.
 */
OS_EXPORT void ServiceRecord::completesUpdateRecord()
{
    JELOG2(EJavaBluetooth);
    if (0 != mSrvRecFd)
    {
        close(mSrvRecFd);
        mSrvRecFd = 0;
    }
}

/**
 * Gets the persistent file name. This function returns the file name pattern
 * srv_rec_<uuid>_<protocol>.txt And also, the srv_rec_XXX.txt
 * file name will be prepended with midlet's private directory for
 * bluetooth. That is, the final outcome of this function may be something
 * like, <midlet_private_dir>/bt/srv_rec_XXX.txt
 *
 * @param aUuid
 *            uuid string
 * @param aProtocol
 *            protocol name
 * @return Null when connection not registered for push;
 *         otherwise srv_rec_XXX file name
 */
std::wstring ServiceRecord::getPersistentFileName()
{
    JELOG2(EJavaBluetooth);
    std::wstring noFileName;
    std::wstring srvRecFile;

    LOG1(EJavaBluetooth, EInfo, "+ ServiceRecord:getPersistentFileName %d",
         mProtocol);

    const java::runtime::ApplicationInfo& appInf =
        java::runtime::ApplicationInfo::getInstance();
    const std::wstring btFilesPath = appInf.getRootPath() + L"bt\\";

    if (false == FileUtilities::exists(btFilesPath))
    {
        try
        {
            LOG1(EJavaBluetooth, EInfo,
                 "  ServiceRecord:getPersistentFileName mkdir:%S",
                 btFilesPath.c_str());

            char *dirName = JavaCommonUtils::wstringToUtf8(btFilesPath);
            if (mkdir(dirName, BT_DIR_PERM) < 0)
            {
                ELOG1(EJavaBluetooth,
                      "- ServiceRecord:getPersistentFileName Err, mkdir:%d",errno);
                delete[] dirName;
                return noFileName;
            }
            delete[] dirName;
        }
        catch (ExceptionBase &ex)
        {
            ELOG1(EJavaBluetooth,
                  "- ServiceRecord::getPersistentFileName exception Caught: %S",
                  ex.toString().c_str());
            return noFileName;
        }
    }

    srvRecFile = btFilesPath + L"srv_rec_";

    switch (mProtocol)
    {
    case PROTOCOL_L2CAP:
        srvRecFile += mOrigUuid + L"_btl2cap.txt";
        break;
    case PROTOCOL_RFCOMM:
        srvRecFile += mOrigUuid + L"_btspp.txt";
        break;
    case PROTOCOL_GOEP:
        srvRecFile += mOrigUuid + L"_btgoep.txt";
        break;
    }

    LOG1(EJavaBluetooth, EInfo,
         "- ServiceRecord:getPersistentFileName %S", srvRecFile.c_str());
    return srvRecFile;
}

/**
 * Exporting this function only for the use of ServerConnection::AcceptL
 * and ServerConnection::HandleAcceptL methods. In rest of the cases,
 * this method should be called via setAdvertise()
 */
OS_EXPORT int ServiceRecord::setAdvertiseFs(TBool aAvailable)
{
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::setAdvertise %s",
         (aAvailable)?"True":"False");
    int err = 0;

    TInt state = aAvailable ? SERVICE_AVAILABLE : !SERVICE_AVAILABLE;

    // Set availability
    TRAP(err,
    {
        mSdpDB.UpdateAttributeL(mRecord,
        KSdpAttrIdServiceAvailability, state);

        mSdpDB.UpdateAttributeL(mRecord,
                                KSdpAttrIdServiceRecordState, ++mRecordState);
    }
        );
    return err;
}

/**
 * Creates and starts the new list (DEA/DES)
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::attributeListStart(int aAttrId, int aAttrType)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::attributeListStartFs,
               aAttrId, aAttrType, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::attributeListStartFs(int aAttrId, int aAttrType, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::attributeListStart %d", aAttrId);

    TRAPD(err,
    {
        if (aAttrId >= 0)
        {
            switch (aAttrType)
            {
            case DataElement_DATSEQ:
                mSdpAttrValueList = CSdpAttrValueDES::NewDESL(NULL);
                break;
            case DataElement_DATALT:
                mSdpAttrValueList = CSdpAttrValueDEA::NewDEAL(NULL);
                break;
            default:
                break;
            }
            mLastSdpAttr = mSdpAttrValueList;
        }
        else
        {
            switch (aAttrType)
            {
            case DataElement_DATSEQ:
                if (mLastSdpAttr)
                    mLastSdpAttr = mLastSdpAttr -> BuildDESL();
                break;
            case DataElement_DATALT:
                if (mLastSdpAttr)
                    mLastSdpAttr = mLastSdpAttr -> BuildDESL();
                break;
            default:
                break;
            }
        }
        mLastSdpAttr = mLastSdpAttr -> StartListL();
    }
         );

    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        LOG2(EJavaBluetooth, EInfo,  "  ServiceRecord Writing %d, %d",
             aAttrId, aAttrType);
        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&aAttrType, sizeof(aAttrType));
    }
    return err;
}

/**
 * Ends the list (DEA/DES), and update the attribute list of SDP database
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::attributeListEnd(int aAttrId)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::attributeListEndFs,
               aAttrId, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::attributeListEndFs(int aAttrId, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::attributeListEnd %d", aAttrId);
    int err = 0;

    if (mLastSdpAttr)
        TRAP(err, mLastSdpAttr = mLastSdpAttr -> EndListL());
    if (KErrNone == err && aAttrId >= 0)
    {
        LOG1(EJavaBluetooth, EInfo,  "*  ServiceRecord::attributeListEnd %d", aAttrId);
        TRAP(err, mSdpDB.UpdateAttributeL(mRecord, aAttrId, *mSdpAttrValueList));
        LOG1(EJavaBluetooth, EInfo,  "x  ServiceRecord::attributeListEnd %d", aAttrId);
        delete mSdpAttrValueList;
        mSdpAttrValueList = NULL;
        mLastSdpAttr = NULL;
    }

    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        int type = DataElement_EndList;
        LOG2(EJavaBluetooth, EInfo,  "  ServiceRecord Writing %d, %d",
             aAttrId, type);
        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&type, sizeof(type));
    }
    return err;
}

/**
 * Adds unsigned integer to the list (DEA/DES),
 * or update the attribute list of SDP database
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::setAttributeUint(int aAttrId, int aAttrType, TDesC8 &aNumVal)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::setAttributeUintFs,
               aAttrId, aAttrType, aNumVal, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::setAttributeUintFs(int aAttrId, int aAttrType, TDesC8 &aNumVal, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::setAttributeUint %d", aAttrId);

    TRAPD(err,
    {
        if (aAttrId < 0)
        {
            if (mLastSdpAttr)
                mLastSdpAttr = mLastSdpAttr -> BuildUintL(aNumVal);
        }
        else
        {
            CSdpAttrValueUint *val = NULL;
            val = CSdpAttrValueUint::NewUintL(aNumVal);
            mSdpDB.UpdateAttributeL(mRecord, aAttrId, *val);
            delete val;
        }
    }
         );

    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        int size = aNumVal.Size();
        const TUint8 *ptr = aNumVal.Ptr();

        LOG4(EJavaBluetooth, EInfo,"  ServiceRecord Writing %d, %d, %d, [%S]",
             aAttrId, aAttrType, size, ptr);

        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&aAttrType, sizeof(aAttrType));
        write(mSrvRecFd, (char *)&size, sizeof(size));
        write(mSrvRecFd, (char *)ptr, size);
    }
    return err;
}

/**
 * Adds integer to the list (DEA/DES),
 * or update the attribute list of SDP database
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::setAttributeInt(int aAttrId, int aAttrType, TDesC8 &aNumVal)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::setAttributeIntFs,
               aAttrId, aAttrType, aNumVal, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::setAttributeIntFs(int aAttrId, int aAttrType, TDesC8 &aNumVal, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::setAttributeInt %d", aAttrId);

    TRAPD(err,
    {
        if (aAttrId < 0)
        {
            if (mLastSdpAttr)
                mLastSdpAttr = mLastSdpAttr -> BuildIntL(aNumVal);
        }
        else
        {
            CSdpAttrValueInt *val = NULL;
            val = CSdpAttrValueInt::NewIntL(aNumVal);
            mSdpDB.UpdateAttributeL(mRecord, aAttrId, *val);
            delete val;
        }
    }
         );

    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        int size = aNumVal.Size();
        const TUint8 *ptr = aNumVal.Ptr();

        LOG4(EJavaBluetooth, EInfo,"  ServiceRecord Writing %d, %d, %d, [%S]",
             aAttrId, aAttrType, size, ptr);

        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&aAttrType, sizeof(aAttrType));
        write(mSrvRecFd, (char *)&size, sizeof(size));
        write(mSrvRecFd, (char *)ptr, size);
    }
    return err;
}

/**
 * Adds boolean value to the list (DEA/DES),
 * or update the attribute list of SDP database
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::setAttributeBool(int aAttrId, TBool aBoolVal)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::setAttributeBoolFs,
               aAttrId, aBoolVal, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::setAttributeBoolFs(int aAttrId, TBool aBoolVal, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::setAttributeBool %d", aAttrId);

    TRAPD(err,
    {
        if (aAttrId < 0)
        {
            if (mLastSdpAttr)
                mLastSdpAttr = mLastSdpAttr -> BuildBooleanL(aBoolVal);
        }
        else
        {
            CSdpAttrValueBoolean  *val = NULL;
            val =  CSdpAttrValueBoolean::NewBoolL(aBoolVal);
            mSdpDB.UpdateAttributeL(mRecord, aAttrId, *val);
            delete val;
        }
    }
         );

    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        // Taking largest type for storage
        int type = DataElement_BOOL;
        TUint8 val = (aBoolVal) ? 1 : 0;

        LOG3(EJavaBluetooth, EInfo,"  ServiceRecord Writing %d, %d, %d",
             aAttrId, type, val);

        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&type, sizeof(type));
        write(mSrvRecFd, (char *)&val, 1);
    }
    return err;
}

/**
 * Adds UUID to the list (DEA/DES),
 * or update the attribute list of SDP database
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::setAttributeUUID(int aAttrId, TUUID &aUUIDVal)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::setAttributeUUIDFs,
               aAttrId, aUUIDVal, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::setAttributeUUIDFs(int aAttrId, TUUID &aUUIDVal, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::setAttributeUUID %d", aAttrId);

    TRAPD(err,
    {
        if (aAttrId < 0)
        {
            if (mLastSdpAttr)
                mLastSdpAttr = mLastSdpAttr -> BuildUUIDL(aUUIDVal);
        }
        else
        {
            CSdpAttrValueUUID  *val = NULL;
            val = CSdpAttrValueUUID::NewUUIDL(aUUIDVal);
            mSdpDB.UpdateAttributeL(mRecord, aAttrId, *val);
            delete val;
        }
    }
         );

    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        int type = DataElement_UUID;
        int size = aUUIDVal.Des().Size();
        const TUint8 *ptr = aUUIDVal.Des().Ptr();

        LOG4(EJavaBluetooth, EInfo,"  ServiceRecord Writing %d, %d, %d, [%S]",
             aAttrId, type, size, ptr);

        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&type, sizeof(type));
        write(mSrvRecFd, (char *)&size, sizeof(size));
        write(mSrvRecFd, (char *)ptr, size);
    }
    return err;
}

/**
 * Adds string to the list (DEA/DES),
 * or update the attribute list of SDP database
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::setAttributeString(int aAttrId, TDesC8 &aStrVal)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::setAttributeStringFs,
               aAttrId, aStrVal, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::setAttributeStringFs(int aAttrId, TDesC8 &aStrVal, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "+ ServiceRecord::setAttributeString %d", aAttrId);

    TRAPD(err,
    {
        if (aAttrId < 0)
        {
            if (mLastSdpAttr)
                mLastSdpAttr = mLastSdpAttr -> BuildStringL(aStrVal);
        }
        else
        {
            CSdpAttrValueString   *val = NULL;
            val = CSdpAttrValueString::NewStringL(aStrVal);
            mSdpDB.UpdateAttributeL(mRecord, aAttrId, *val);
            delete val;
        }
    }
         );

    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        int type = DataElement_STRING;
        int size = aStrVal.Size();
        const TUint8 *ptr = aStrVal.Ptr();

        LOG3(EJavaBluetooth, EInfo,"  ServiceRecord Writing %d, %d, %d",
             aAttrId, type, size);

        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&type, sizeof(type));
        write(mSrvRecFd, (char *)&size, sizeof(size));
        write(mSrvRecFd, (char *)ptr, size);
    }
    return err;
}

/**
 * Adds URL to the list (DEA/DES),
 * or update the attribute list of SDP database
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::setAttributeUrl(int aAttrId, TDesC8 &aUrlVal)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::setAttributeUrlFs,
               aAttrId, aUrlVal, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::setAttributeUrlFs(int aAttrId, TDesC8 &aUrlVal, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::setAttributeUrl %d", aAttrId);

    TRAPD(err,
    {
        if (aAttrId < 0)
        {
            if (mLastSdpAttr)
                mLastSdpAttr = mLastSdpAttr -> BuildURLL(aUrlVal);
        }
        else
        {
            CSdpAttrValueURL *val = NULL;
            val = CSdpAttrValueURL::NewURLL(aUrlVal);
            mSdpDB.UpdateAttributeL(mRecord, aAttrId, *val);
            delete val;
        }
    }
         );
    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        int type = DataElement_URL;
        int size = aUrlVal.Size();
        const TUint8 *ptr = aUrlVal.Ptr();

        LOG4(EJavaBluetooth, EInfo,"  ServiceRecord Writing %d, %d, %d, [%S]",
             aAttrId, type, size, ptr);

        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&type, sizeof(type));
        write(mSrvRecFd, (char *)&size, sizeof(size));
        write(mSrvRecFd, (char *)ptr, size);
    }
    return err;
}

/**
 * Adds NIL to the list (DEA/DES),
 * or update the attribute list of SDP database
 * On success returns 0; otherwise err number.
 */
OS_EXPORT int ServiceRecord::setAttributeNil(int aAttrId)
{
    JELOG2(EJavaBluetooth);
    int ret = 0;
    TBool persistentReq = ETrue;

    CallMethod(ret, this, &ServiceRecord::setAttributeNilFs,
               aAttrId, persistentReq, mFunctionServer);
    return ret;
}

int ServiceRecord::setAttributeNilFs(int aAttrId, TBool isPersistenceRequired)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,  "+ ServiceRecord::setAttributeNil %d", aAttrId);

    TRAPD(err,
    {
        if (aAttrId < 0)
        {
            if (mLastSdpAttr)
                mLastSdpAttr = mLastSdpAttr -> BuildNilL();
        }
        else
        {
            CSdpAttrValueNil *val = NULL;
            val = CSdpAttrValueNil::NewNilL();
            mSdpDB.UpdateAttributeL(mRecord, aAttrId, *val);
            delete val;
        }
    }
         );

    // Writing into persistent file for further usage
    if (0 != mSrvRecFd && isPersistenceRequired)
    {
        // Taking largest type for storage
        int type = DataElement_NULL;

        LOG2(EJavaBluetooth, EInfo,  "  ServiceRecord Writing %d, %d",
             aAttrId, type);

        write(mSrvRecFd, (char *)&aAttrId, sizeof(aAttrId));
        write(mSrvRecFd, (char *)&type, sizeof(type));
    }
    return err;
}

/*
 * This function is used to restore the Java Service Record by reading the
 * persistent file. This function will work only once after midlet took
 * the server connection handle from the Push framework.
 * This function tries to populate Java Service record only when there is a
 * successful retrieval of persistent storage exists.
 */

OS_EXPORT void ServiceRecord::restoreJavaServiceRecordL(
    JNIEnv *aJni, jobject aServiceRecordImpl)
{
    JELOG2(EJavaBluetooth);

    if (false == mInitializedByPush)
    {
        // No need of restoring anything since it is not a push service
        return;
    }

    // Setting service record handle.
    BTServiceRecordPopulator* srvRecPopulator =
        BTServiceRecordPopulator::New(aJni, aServiceRecordImpl);

    srvRecPopulator->SetServiceRecordHandle(mRecord);

    if (false == mRestoredFromPersistentFile)
    {
        // No need of restoring anything from persistent file.
        delete srvRecPopulator;
        return;
    }

    std::wstring persistentFileName = getPersistentFileName();

    int persistentRecFd = -1;
    int ret = 0;

    if (persistentFileName.length()> 0)
    {
        try
        {
            char *fileName =
                java::util::JavaCommonUtils::wstringToUtf8(persistentFileName);
            persistentRecFd = open(fileName, O_RDONLY);
            delete[] fileName;
        }
        catch (ExceptionBase &ex)
        {
            ELOG1(EJavaBluetooth,
                  "- ServiceRecord::restoreJavaServiceRecord exception caught: %S",
                  ex.toString().c_str());
        }
    }

    if (persistentRecFd < 0)
    {
        // Nothing else to be done, as persistent file can not be opened or
        // it is empty.
        delete srvRecPopulator;
        return;
    }

    MSdpElementBuilder* elementBuilder = (MSdpElementBuilder*) srvRecPopulator;

    // Reading and restoring the DeviceServiceClasses bits (If any)
    unsigned int devServClass = 0;
    ret = read(persistentRecFd, &devServClass, sizeof(devServClass));
    if (ret <= 0)
    {
        delete srvRecPopulator;
        return;
    }

    elementBuilder = srvRecPopulator->SetDeviceServiceClass(devServClass);


    // Calling first BuildDESL, is mandatory
    elementBuilder = elementBuilder->BuildDESL();
    elementBuilder = elementBuilder->StartListL();

    int attrId = 0;
    while ((ret = read(persistentRecFd, &attrId, sizeof(attrId))) > 0)
    {
        int dataType = 0;

        ret = read(persistentRecFd, &dataType, sizeof(dataType));
        if (ret <= 0)
            break;

        if (attrId > 0 && DataElement_EndList != dataType)
        {
            TSdpIntBuf<TUint> attrIdBuf(attrId);
            elementBuilder = elementBuilder->BuildUintL(attrIdBuf);
        }

        LOG2(EJavaBluetooth, EInfo,
             "  ServiceRecord reading %X, %X", attrId, dataType);

        switch (dataType)
        {
        case DataElement_DATSEQ:
        {
            elementBuilder = elementBuilder->BuildDESL();
            elementBuilder = elementBuilder->StartListL();
            break;
        }
        case DataElement_DATALT:
        {
            elementBuilder = elementBuilder->BuildDEAL();
            elementBuilder = elementBuilder->StartListL();
            break;
        }
        case DataElement_EndList:
        {
            elementBuilder = elementBuilder->EndListL();
            break;
        }
        case DataElement_NULL:
        {
            elementBuilder = elementBuilder->BuildNilL();
            break;
        }
        case DataElement_BOOL:
        {
            TUint8 val = 0;
            ret = read(persistentRecFd, &val, sizeof(val));
            if (ret <= 0)
                break;

            LOG1(EJavaBluetooth, EInfo,
                 "  ServiceRecord reading Bool %X", val);
            elementBuilder =
                elementBuilder->BuildBooleanL((val) ? ETrue : EFalse);
            break;
        }
        case DataElement_U_INT_1:
        case DataElement_U_INT_2:
        case DataElement_U_INT_4:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >4))
                break;

            TUint8 bytes[4] = {0};
            ret = read(persistentRecFd, bytes, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo,  "  ServiceRecord reading INT 0x%X, 0x%X", len, bytes);

            std::auto_ptr<HBufC8> buf(convertIntToDesC(bytes, dataType));
            elementBuilder = elementBuilder->BuildUintL(*buf);
            break;
        }
        case DataElement_U_INT_8:
        case DataElement_U_INT_16:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len > 16))
                break;

            TUint8 buf[16] = {0};
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo,  "  ServiceRecord reading INT 0x%X, %S", len, buf);

            TPtrC8 val(buf, len);
            elementBuilder = elementBuilder->BuildUintL(val);
            break;
        }
        case DataElement_INT_1:
        case DataElement_INT_2:
        case DataElement_INT_4:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >4))
                break;

            TUint8 bytes[4] = {0};
            ret = read(persistentRecFd, bytes, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo,  "  ServiceRecord reading INT 0x%X, %S", len, bytes);

            std::auto_ptr<HBufC8> buf(convertIntToDesC(bytes, dataType));
            elementBuilder = elementBuilder->BuildIntL(*buf);
            break;
        }
        case DataElement_INT_8:
        case DataElement_INT_16:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >16))
                break;

            TUint8 buf[16] = {0};
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo,  "  ServiceRecord reading INT %X, %S", len, buf);

            TPtrC8 val(buf, len);
            elementBuilder = elementBuilder->BuildIntL(val);
            break;
        }
        case DataElement_UUID:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >256))
                break;

            TUint8 buf[256] = {0};
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo,  "  ServiceRecord reading UUID %X, %S", len, buf);

            TPtrC8 uuidDes(buf, len);

            TUUID val;
            val.SetL(uuidDes);

            elementBuilder = elementBuilder->BuildUUIDL(val);
            break;
        }
        case DataElement_STRING:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >1024))
                break;

            TUint8 buf[1024] = {0};
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG1(EJavaBluetooth, EInfo,
                 "  ServiceRecord reading STRING %X", len);

            TPtrC8 val(buf, len);
            LOG(EJavaBluetooth, EInfo,
                "  ServiceRecord::RestoreJavaServiceRecord BuildStringL");
            elementBuilder = elementBuilder->BuildStringL(val);
            break;
        }
        case DataElement_URL:
        {
            int len = 0;
            ret = read(persistentRecFd, &len, sizeof(len));
            if (ret <= 0 || (len <= 0 || len >1024))
                break;

            TUint8 buf[1024] = {0};
            ret = read(persistentRecFd, buf, len);
            if (ret <= 0)
                break;

            LOG2(EJavaBluetooth, EInfo,
                 "  ServiceRecord reading URL %X, %S", len, buf);

            TPtrC8 val(buf, len);
            elementBuilder = elementBuilder->BuildURLL(val);
            break;
        }
        default:
            break;
        }
        if (0 == ret)
            break;
    }

    delete srvRecPopulator;

    return;
}

// Helper function
HBufC8* ServiceRecord::convertIntToDesC(TUint8 *aBytes, int aDataType)
{
    JELOG2(EJavaBluetooth);
    HBufC8* descBytes = NULL;
    switch (aDataType)
    {
    case DataElement_INT_1:
    case DataElement_U_INT_1:
    {
        descBytes = HBufC8::New(sizeof(TUint8) + 1);
        TPtr8 ptr = descBytes->Des();
        TUint value = *(TUint *)aBytes;
        value=ByteOrder::Swap32(value);

        TSdpIntBuf<TUint8> buf(value);
        ptr.Copy(buf);
        break;
    }
    case DataElement_INT_2:
    case DataElement_U_INT_2:
    {
        descBytes = HBufC8::New(sizeof(TUint16) + 1);
        TPtr8 ptr = descBytes->Des();
        TUint value = *(TUint *)aBytes;
        value=ByteOrder::Swap32(value);

        TSdpIntBuf<TUint16> buf(value);
        ptr.Copy(buf);
        break;
    }
    case DataElement_INT_4:
    case DataElement_U_INT_4:
    {
        descBytes = HBufC8::New(sizeof(TUint32) + 1);
        TPtr8 ptr = descBytes->Des();
        TUint value = *(TUint *)aBytes;
        value=ByteOrder::Swap32(value);

        TSdpIntBuf<TUint32> buf(value);
        ptr.Copy(buf);
        break;
    }
    default:
    {
        descBytes = HBufC8::New(sizeof(TUint64) + 1);
        TPtr8 ptr = descBytes->Des();

        TSdpIntBuf<TUint64> buf(*(TUint64 *)aBytes);
        ptr.Copy(buf);
        break;
    }
    }

    return descBytes;
}

TUint8 *ServiceRecord::uuidToByteArray(std::wstring aUuid,
                                       int &aUuidByteLength)
{
    JELOG2(EJavaBluetooth);

    int len = aUuid.length();
    int resultLen = (len/2) + 1;
    TUint8 *result = new TUint8[resultLen];
    int index = 0;
    int byteIndex = 0;

    LOG1(EJavaBluetooth, EInfo, "  ServiceRecord::uuidToByteArray Length:%d", len);

    if ((len % 2) != 0)
    {
        int charPos = charToInt(aUuid[index]);
        index += 1;
        result[byteIndex ++] = charPos;
    }

    while (index < len)
    {
        result[byteIndex ++] = (charToInt(aUuid[index]) << 4) |
                               charToInt(aUuid[index + 1]);
        index += 2;
    }
    aUuidByteLength = (len /2) + (len %2);

    return result;
}

TInt8 ServiceRecord::charToInt(wchar_t ch)
{
    std::wstring validUpperChars(L"0123456789ABCDEF"); // Never change this.
    std::wstring validLowerChars(L"0123456789abcdef"); // Never change this.
    TInt8 result = -1;
    if ((result = validUpperChars.find(ch)) == std::wstring::npos)
    {
        result = validLowerChars.find(ch);
    }
    return result;
}

std::wstring ServiceRecord::uuidToWstring(TUUID aUuid)
{
    JELOG2(EJavaBluetooth);

    std::wstring validChars(L"0123456789ABCDEF"); // Never change this.
    std::wstring result;

    int len = aUuid.Des().Size();
    const TUint8 *ptr = aUuid.Des().Ptr();

    for (int i = 0; i < len; i ++, ptr ++)
    {
        result += validChars[((*ptr) >> 4)];
        result += validChars[((*ptr) & 0x0F)];
        LOG1(EJavaBluetooth, EInfo,
             "  ServiceRecord::uuidToWstring %S",  result.c_str());
    }

    return result;
}

}  //end namespace bluetooth
}  //end namespace java
