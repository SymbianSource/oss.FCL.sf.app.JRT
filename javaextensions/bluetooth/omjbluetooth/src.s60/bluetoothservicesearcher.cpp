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


#include <bluetooth/hci/hcierrors.h>

#include "bluetoothconsts.h"
#include "bluetoothservicesearcher.h"
#include "discoveryagent.h"

namespace java
{
namespace bluetooth
{

#define GET_VALID_HCI_ERR_CODE(err_no) (((err_no) < KHCIErrorBase) ? \
                                        (KHCIErrorBase - (err_no)) : \
                                        (err_no))

BluetoothServiceSearcher* BluetoothServiceSearcher::New(
    DiscoveryAgent *aDiscoveryAgent, BluetoothFunctionServer* aServer)
{
    JELOG2(EJavaBluetooth);
    BluetoothServiceSearcher* searcher = new BluetoothServiceSearcher();
    searcher->Construct(aDiscoveryAgent, aServer);
    return searcher;
}

BluetoothServiceSearcher::BluetoothServiceSearcher()
{
}

void BluetoothServiceSearcher::Construct(DiscoveryAgent *aDiscoveryAgent,
        BluetoothFunctionServer* aServer)
{
    JELOG2(EJavaBluetooth);
    mDiscoveryAgent = aDiscoveryAgent;
    mServer = aServer;
    mSDPAgent = NULL;
    mSpat = NULL;
    mAttributeList = NULL;
    mSrPopulator = NULL;
    mFoundServiceRecord = false;
    mIsPopulateRecordMode = false;

    mJni = mServer->getValidJniEnv();
    mPeer = mServer->getPeer();

    jclass peerClass = mJni->GetObjectClass(mPeer);
    mCreateServiceRecordMethod
    = mJni->GetMethodID(peerClass, "createServiceRecord",
                        "(J)Lcom/intel/bluetooth/ServiceRecordImpl;");

    mServiceRecordImplClass = mJni->FindClass(
                                  "com/intel/bluetooth/ServiceRecordImpl");

}

BluetoothServiceSearcher::~BluetoothServiceSearcher()
{
    JELOG2(EJavaBluetooth);
    Cleanup();
}

void BluetoothServiceSearcher::SearchServicesL(TInt64 aRemoteAddress,
        TPtrC8 aUuidsDes, TPtrC16 aAttrIdsDes)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "+ BluetoothServiceSearcher:: SearchServices:%X",
         (long) aRemoteAddress);

    mIsPopulateRecordMode = false;

    TBTDevAddr devAddr(aRemoteAddress);

    FillSearchPatternL(aUuidsDes);

    FillAttributeListL(aAttrIdsDes);

    LOG(
        EJavaBluetooth,
        EInfo,
        "  BluetoothServiceSearcher::SearchServicesOnDeviceL: Clearing SDPAgent if any...");
    // Init new service discovery agent
    if (mSDPAgent)
    {
        delete mSDPAgent;
        mSDPAgent = NULL;
    }
    mSDPAgent = CSdpAgent::NewL(*this, devAddr);

    mSDPAgent->SetRecordFilterL(*mSpat);

    mSDPAgent->NextRecordRequestL();

}

void BluetoothServiceSearcher::cancelServiceSearch()
{
    JELOG2(EJavaBluetooth);
    DiscoveryAgent* tempDiscoveryAgent = mDiscoveryAgent;
    Cleanup();
    if (NULL == tempDiscoveryAgent)
        return;
    if (!mIsPopulateRecordMode)
    {
        tempDiscoveryAgent->ServiceSearchCompleted(SERVICE_SEARCH_TERMINATED);
    }
    else
    {
        tempDiscoveryAgent->PopulateRecordCompleted(SERVICE_SEARCH_TERMINATED);
    }
}

void BluetoothServiceSearcher::Cleanup()
{
    JELOG2(EJavaBluetooth);
    if (mSDPAgent)
    {
        delete mSDPAgent;
        mSDPAgent = NULL;
    }
    if (mSpat)
    {
        mSpat->Reset();
        delete mSpat;
        mSpat = NULL;
    }
    if (mAttributeList)
    {
        delete mAttributeList;
        mAttributeList = NULL;
    }
    if (mSrPopulator)
    {
        delete mSrPopulator;
        mSrPopulator = NULL;
    }

    mDiscoveryAgent = NULL;
}

void BluetoothServiceSearcher::NextRecordRequestComplete(TInt aError,
        TSdpServRecordHandle aHandle, TInt /*aTotalRecordsCount*/)
{
    JELOG2(EJavaBluetooth);
    LOG1(
        EJavaBluetooth,
        EInfo,
        "  BluetoothServiceSearcher::NextRecordRequestComplete aError=%d",
        aError);

    if (KErrEof == aError && !mFoundServiceRecord)
    {
        LOG(
            EJavaBluetooth,
            EInfo,
            "  BluetoothServiceSearcher::NextRecordRequestComplete SERVICE_SEARCH_NO_RECORDS");
        mDiscoveryAgent->ServiceSearchCompleted(SERVICE_SEARCH_NO_RECORDS);
    }
    else if (KErrNone == aError)
    {
        TInt64 servRecHandle = MAKE_TINT64(0, aHandle);
        mServiceRecordImpl = (*mJni).CallObjectMethod(mPeer,
                             mCreateServiceRecordMethod, servRecHandle);
        if (NULL == mServiceRecordImpl)
        {
            LOG(EJavaBluetooth, EInfo,
                "  BluetoothServiceSearcher::NextRecordRequestComplete SERVICE_SEARCH_ERROR");
            mDiscoveryAgent->ServiceSearchCompleted(SERVICE_SEARCH_ERROR);
        }
        else
        {
            LOG(EJavaBluetooth, EInfo,
                "  BluetoothServiceSearcher::NextRecordRequestComplete Populating record");
            delete mSrPopulator;
            mSrPopulator = BTServiceRecordPopulator::New(mJni,
                           mServiceRecordImpl);
            TRAPD(attrRequestErr, mSDPAgent->AttributeRequestL(mSrPopulator,
                    aHandle, *mAttributeList));
            if (KErrNone != attrRequestErr)
            {
                delete mSrPopulator;
                mSrPopulator = NULL;
                mDiscoveryAgent->ServiceSearchCompleted(SERVICE_SEARCH_ERROR);
            }
        }
    }
    else if (EPageTimedOut == GET_VALID_HCI_ERR_CODE(aError))
    {
        LOG(
            EJavaBluetooth,
            EInfo,
            "  BluetoothServiceSearcher::NextRecordRequestComplete SERVICE_SEARCH_DEVICE_NOT_REACHABLE");
        mDiscoveryAgent->ServiceSearchCompleted(
            SERVICE_SEARCH_DEVICE_NOT_REACHABLE);
    }
    else
    {
        LOG1(
            EJavaBluetooth,
            EInfo,
            "  BluetoothServiceSearcher::NextRecordRequestComplete SERVICE_SEARCH_COMPLETED %d",
            aError);
        mDiscoveryAgent->ServiceSearchCompleted(SERVICE_SEARCH_COMPLETED);
    }
}

void BluetoothServiceSearcher::PopulateServiceRecordsL(TInt64 aRemoteAddress,
        long aHandle, TPtrC16 aAttrIdsDes, jobject aServiceRecordImpl)
{
    JELOG2(EJavaBluetooth);

    mIsPopulateRecordMode = true;

    TBTDevAddr devAddr(aRemoteAddress);

    FillAttributeListL(aAttrIdsDes);

    LOG(
        EJavaBluetooth,
        EInfo,
        "  BluetoothServiceSearcher::PopulateServiceRecords: Clearing SDPAgent if any...");

    if (mSDPAgent)
    {
        mSDPAgent->Cancel();
        delete mSDPAgent;
        mSDPAgent = NULL;
    }
    mSDPAgent = CSdpAgent::NewL(*this, devAddr);

    mServiceRecordImpl = aServiceRecordImpl;

    delete mSrPopulator;
    mSrPopulator = BTServiceRecordPopulator::New(mJni, mServiceRecordImpl);

    mSDPAgent->AttributeRequestL(mSrPopulator, aHandle, *mAttributeList);
}

void BluetoothServiceSearcher::AttributeRequestResult(
    TSdpServRecordHandle /*aHandle*/, TSdpAttributeID /*aAttrID*/,
    CSdpAttrValue* /*aAttrValue*/)
{
    // No implementation required...
}

void BluetoothServiceSearcher::AttributeRequestComplete(
    TSdpServRecordHandle /*aHandle*/, TInt aError)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "  BluetoothServiceSearcher::AttributeRequestComplete aError=%d",
         aError);

    int status = 0;
    bool attributeFound = mSrPopulator->isAttributesFound();

    delete mSrPopulator;
    mSrPopulator = NULL;

    if (KErrNone == aError && !mIsPopulateRecordMode)
    {
        mFoundServiceRecord = true;
        LOG(
            EJavaBluetooth,
            EInfo,
            "  BluetoothServiceSearcher::AttributeRequestComplete Callback serviceDiscoveredCallback");
        jclass peerClass = mJni->GetObjectClass(mPeer);
        jmethodID serviceDiscoveredMethod = mJni->GetMethodID(peerClass,
                                            "serviceDiscoveredCallback",
                                            "(Lcom/intel/bluetooth/ServiceRecordImpl;)V");
        (*mJni).CallVoidMethod(mPeer, serviceDiscoveredMethod,
                               mServiceRecordImpl);

        LOG(
            EJavaBluetooth,
            EInfo,
            "  BluetoothServiceSearcher::AttributeRequestComplete. Trying for the next record if any");

        // In case of any leave, then this will terminates the search for the next record.
        TRAP_IGNORE(mSDPAgent->NextRecordRequestL());
    }
    else
    {
        if (KErrNone == aError)
        {
            mFoundServiceRecord = true;
            status = SERVICE_SEARCH_COMPLETED;
        }
        else if (EPageTimedOut == GET_VALID_HCI_ERR_CODE(aError))
        {
            LOG(
                EJavaBluetooth,
                EInfo,
                "  BluetoothServiceSearcher::AttributeRequestComplete SERVICE_SEARCH_DEVICE_NOT_REACHABLE");
            status = SERVICE_SEARCH_DEVICE_NOT_REACHABLE;
        }
        else
        {
            LOG(EJavaBluetooth, EInfo,
                "  BluetoothServiceSearcher::AttributeRequestComplete SERVICE_SEARCH_ERROR");
            status = SERVICE_SEARCH_ERROR;
        }

        if (!mIsPopulateRecordMode)
        {
            mDiscoveryAgent->ServiceSearchCompleted(status);
        }
        else
        {
            if (false == attributeFound)
                status = SERVICE_SEARCH_NO_RECORDS;
            mDiscoveryAgent->PopulateRecordCompleted(status);
        }
    }
}

void BluetoothServiceSearcher::BluetoothServiceSearcher::FillSearchPatternL(
    TDesC8& uuidsBytes)
{
    JELOG2(EJavaBluetooth);

    const TInt numBytes = uuidsBytes.Length();
    const TInt numberOfUUIDs = numBytes / KSdpUUIDMaxLength;

    TInt uuidsConverted = 0;
    TInt startByte = 0;

    if (mSpat)
    {
        mSpat->Reset();
        delete mSpat;
        mSpat = NULL;
    }
    mSpat = CSdpSearchPattern::NewL();
    CleanupStack::PushL(mSpat);

    while (uuidsConverted < numberOfUUIDs)
    {
        TUUID uuid;
        uuid.SetL(uuidsBytes.Mid(startByte, KSdpUUIDMaxLength));

        mSpat->AddL(uuid);
        startByte += KSdpUUIDMaxLength;
        uuidsConverted++;
    }
    CleanupStack::Pop(mSpat);
}

void BluetoothServiceSearcher::BluetoothServiceSearcher::FillAttributeListL(
    TDesC16& attrIds)
{
    JELOG2(EJavaBluetooth);
    delete mAttributeList;
    mAttributeList = NULL;
    mAttributeList = CSdpAttrIdMatchList::NewL();
    CleanupStack::PushL(mAttributeList);

    TInt numElements = attrIds.Length();

    for (TInt attrIndex = 0; attrIndex < numElements; attrIndex += 2)
    {
        const TUint16* attrIdStartPtr = (attrIds.Mid(attrIndex, 2)).Ptr();
        const TInt* attrId = reinterpret_cast<const TInt*>(attrIdStartPtr);
        const TUint16 attrId16 = static_cast<TUint16>(*attrId);
        TAttrRange attrRange(static_cast<TSdpAttributeID>(attrId16));

        mAttributeList->AddL(attrRange);
    }
    CleanupStack::Pop(mAttributeList);

}

} //end namespace bluetooth
} //end namespace java
