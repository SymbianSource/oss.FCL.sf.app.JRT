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


#include <e32cmn.h>
#include <bttypes.h>

#include "functionserver.h"
#include "javajniutils.h"
#include "jniarrayutils.h"
#include "jstringutils.h"
#include "logger.h"
#include "fs_methodcall.h"
#include "monitor.h"
#include "exceptionbase.h"

#include "bluetoothconsts.h"
#include "bluetoothfunctionserver.h"
#include "discoveryagent.h"
#include "bluetoothclientconnection.h"
#include "btl2capserverconnection.h"
#include "btrfcommserverconnection.h"
#include "bluetoothstructs.h"
#include "servicerecord.h"
#include "bluetoothremotedevice.h"

//Push related stuff
#include "btl2cappushserverconnection.h"
#include "btl2capserverconnectionfactory.h"
#include "btrfcommpushserverconnection.h"
#include "btrfcommserverconnectionfactory.h"
#include "btobexserverconnectionfactory.h"
#include "pushexception.h"
#include "pusherrorcodes.h"

#include "com_nokia_mj_impl_bluetooth_BluetoothStackS60.h"

using namespace std;
using namespace java::util;
using namespace java::push;
using namespace java::bluetooth;

//
// JNI functions specific to Bluetooth Stack
//

JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1createFunctionServer
(JNIEnv *aJni, jobject aPeer)
{
    JELOG2(EJavaBluetooth);
    java::bluetooth::BluetoothFunctionServer* server =
        new java::bluetooth::BluetoothFunctionServer(*aJni, aPeer);
    return reinterpret_cast<jlong>(server);
}


//
// JNI functions specific to Discovery Agent
//

JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1createNativeDiscoveryAgent
(JNIEnv* /*aJni*/, jobject /*aPeer*/, jlong aServer)
{
    JELOG2(EJavaBluetooth);
    java::bluetooth::BluetoothFunctionServer *server =
        reinterpret_cast<java::bluetooth::BluetoothFunctionServer *>(
            static_cast<long>(aServer));

    java::bluetooth::DiscoveryAgent* agent =
        new java::bluetooth::DiscoveryAgent(server);

    jlong ret = reinterpret_cast<jlong>(agent);
    return ret;
}


//
// JNI functions specific to Device Discovery
//

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1runDeviceInquiry
(JNIEnv *aJni, jobject aPeer, jint aAccessCode, jlong aDiscAgent)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo, "+ JNI::runDeviceInquiry %X", aAccessCode);

    java::bluetooth::DiscoveryAgent* agent =
        reinterpret_cast<java::bluetooth::DiscoveryAgent *>(
            static_cast<long>(aDiscAgent));

    int result = KErrNone;

    result = agent->startDeviceDiscovery(aJni, aPeer, aAccessCode);

    if (KErrNone != result)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::runDeviceInquiry Error before starting inquiry %d",
              result);

        java::util::JniUtils::throwNewException(aJni,
                                                "javax/bluetooth/BluetoothStateException",
                                                "Device inquiry failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(result));
        return INQUIRY_ERROR;
    }

    int discStatus = 0;

    do
    {
        //Wait for discovery agent to unlock this.
        LOG(EJavaBluetooth, EInfo,
            "  JNI::runDeviceInquiry: Getting next device");

        DiscoveredDevice device;

        discStatus = agent->getNextDevice(device);

        agent->doDeviceDiscoveryCallback(aJni, aPeer, device);

        device.clean();

        LOG1(EJavaBluetooth, EInfo,
             "  JNI::runDeviceInquiry doDeviceDiscoveryCallback return: %d",
             discStatus);

    }
    while ((INQUIRY_ERROR != discStatus) &&
            (INQUIRY_COMPLETED != discStatus) &&
            (INQUIRY_TERMINATED != discStatus));

    return discStatus;
}

JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1cancelInquiry
(JNIEnv* /*aJni*/, jobject /*aPeer*/, jlong aDiscAgent)
{
    JELOG2(EJavaBluetooth);
    java::bluetooth::DiscoveryAgent* agent =
        reinterpret_cast<java::bluetooth::DiscoveryAgent *>(
            static_cast<long>(aDiscAgent));

    jboolean ret = agent->cancelDeviceDiscovery();
    return ret;
}

JNIEXPORT jstring JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1getRemoteDeviceFriendlyName
(JNIEnv* aJni, jobject /*aPeer*/, jlong aAgent, jlong aAddr)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "+ JNI::getRemoteDeviceFriendlyName DeviceAddress:%ld",
         aAddr);

    java::bluetooth::DiscoveryAgent* dAgent =
        reinterpret_cast<java::bluetooth::DiscoveryAgent *>(
            static_cast<long>(aAgent));

    std::wstring* friendlyName = NULL;
    TRAPD(err,
          friendlyName = dAgent->lookupFriendlyNameL(aAddr));

    if (KErrNone != err)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::getRemoteDeviceFriendlyName while lookupFriendlyNameL:%d",
              err);
        java::util::JniUtils::throwNewException(aJni,
                                                "java/io/IOException",
                                                "Retrieving remote device friendly name failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(err));
        return NULL;
    }

    jstring ret;
    try
    {
        if (NULL != friendlyName)
        {
            ret = java::util::JniUtils::wstringToJstring(aJni, *friendlyName);
            delete friendlyName;
        }
        else
        {
            std::wstring str(L"");
            ret = java::util::JniUtils::wstringToJstring(aJni, str);
        }
    }
    catch (ExceptionBase ex)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::getRemoteDeviceFriendlyName Exception:%S", ex.what());
        java::util::JniUtils::throwNewException(aJni,
                                                "java/lang/RunTimeException", "Retrieving friendly name failed");
        delete friendlyName;
        return NULL;
    }
    return ret;
}

//
// JNI functions specific to Service Search & Populate Records
//

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1runSearchServices
(JNIEnv *aEnv, jobject aPeer, jlong aRemoteAddress, jbyteArray aUuidSet,
 jintArray aAttrSet, jlong aDiscAgent)
{
    JELOG2(EJavaBluetooth);
    java::bluetooth::DiscoveryAgent* agent =
        reinterpret_cast<java::bluetooth::DiscoveryAgent *>(
            static_cast<long>(aDiscAgent));

    // ...convert the UUID bytes into an 8-bit descriptor...
    jbyte* byteArray = aEnv->GetByteArrayElements(aUuidSet, NULL);
    TInt numUuidBytes = aEnv->GetArrayLength(aUuidSet);
    TPtrC8 uuidsDes(reinterpret_cast<TUint8*>(byteArray), numUuidBytes);

    // ...convert the Attribute IDs into pairs of 16-bits in a descriptor...
    int numAttrIds = aEnv->GetArrayLength(aAttrSet);
    jint* intArray = aEnv->GetIntArrayElements(aAttrSet, NULL);
    TPtrC16 attrIdsDes(reinterpret_cast<TUint16*>(intArray), numAttrIds*2);

    java::util::Monitor* serviceSearchMonitor =
        java::util::Monitor::createMonitor();

    LOG(EJavaBluetooth, EInfo,
        "   JNI::runSearchServices calling StartSearchServices");
    int result = KErrNone;

    result = agent->StartSearchServices(
                 aEnv, aPeer, aRemoteAddress, uuidsDes, attrIdsDes,
                 serviceSearchMonitor);
    LOG(EJavaBluetooth, EInfo,
        "  JNI::runSearchServices returned from StartSearchServices");

    if (KErrNone != result)
    {
        LOG(EJavaBluetooth, EInfo,
            "  JNI::runSearchServices Error occurred before starting service search!");
        delete serviceSearchMonitor;
        serviceSearchMonitor = NULL;
        return SERVICE_SEARCH_ERROR;
    }

    int searchStatus = -1;

    LOG(EJavaBluetooth, EInfo,
        "  JNI::runSearchServices waiting for search complete");

    while ((SERVICE_SEARCH_ERROR != searchStatus) &&
            (SERVICE_SEARCH_COMPLETED != searchStatus) &&
            (SERVICE_SEARCH_TERMINATED != searchStatus) &&
            (SERVICE_SEARCH_NO_RECORDS != searchStatus) &&
            (SERVICE_SEARCH_DEVICE_NOT_REACHABLE != searchStatus))
    {
        //Wait for discoveryagent to unlock this.
        serviceSearchMonitor->wait();
        searchStatus = agent->getStatusOfCompletion();
        LOG1(EJavaBluetooth, EInfo,
             "  JNI::runSearchServices doServiceDiscoveredCallback return: %d",
             searchStatus);
    }

    delete serviceSearchMonitor;
    serviceSearchMonitor = NULL;
    LOG1(EJavaBluetooth, EInfo,
         "- JNI::runSearchServices status:%d", searchStatus);
    return searchStatus;
}

JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1cancelServiceSearch
(JNIEnv* /*aJni*/, jobject /*aPeer*/, jlong aDiscAgent)
{
    JELOG2(EJavaBluetooth);
    java::bluetooth::DiscoveryAgent* agent =
        reinterpret_cast<java::bluetooth::DiscoveryAgent *>(
            static_cast<long>(aDiscAgent));

    jboolean ret = agent->CancelServiceSearch();
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1populateServiceRecordAttributeValues
(JNIEnv *aEnv, jobject aPeer, jlong aRemoteDevice, jlong aHandle,
 jintArray aAttrSet, jobject aServiceRecordImpl, jlong aDiscAgent)
{
    JELOG2(EJavaBluetooth);
    java::bluetooth::DiscoveryAgent* agent =
        reinterpret_cast<java::bluetooth::DiscoveryAgent *>(
            static_cast<long>(aDiscAgent));

    // ...convert the Attribute IDs into pairs of 16-bits in a descriptor...
    int numAttrIds = aEnv->GetArrayLength(aAttrSet);
    jint* intArray = aEnv->GetIntArrayElements(aAttrSet, NULL);
    TPtrC16 attrIdsDes(reinterpret_cast<TUint16*>(intArray), numAttrIds*2);

    java::util::Monitor* serviceSearchMonitor =
        java::util::Monitor::createMonitor();

    jobject serviceRecGlobalRef = aEnv->NewGlobalRef(aServiceRecordImpl);

    LOG(EJavaBluetooth, EInfo,
        "   JNI::populateServiceRecordAttributeValues calling PopulateServiceRecord");
    int result = KErrNone;
    result = agent->PopulateServiceRecordAttrValue(
                 aEnv, aPeer, aRemoteDevice, aHandle, attrIdsDes, serviceRecGlobalRef,
                 serviceSearchMonitor);

    if (KErrNone != result)
    {
        aEnv->DeleteGlobalRef(serviceRecGlobalRef);
        return SERVICE_SEARCH_ERROR;
    }

    // Waiting for populateServiceRecord to finish
    serviceSearchMonitor->wait();

    aEnv->DeleteGlobalRef(serviceRecGlobalRef);

    return agent->GetPopulateServiceRecordStatus();
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1cancelPopulateServicesRecordAttributeValues
(JNIEnv* /*aJni*/, jobject /*aPeer*/, jlong aDiscAgent)
{
    JELOG2(EJavaBluetooth);
    java::bluetooth::DiscoveryAgent* agent =
        reinterpret_cast<java::bluetooth::DiscoveryAgent *>(
            static_cast<long>(aDiscAgent));

    jboolean result = agent->CancelPopulateServiceRecordAttrValue();
    LOG1(EJavaBluetooth, EInfo,
         "- JNI::cancelPopulateServicesRecordAttributeValues Result:%d",
         result);
    return;
}

JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1getRemoteAddress
(JNIEnv* /*aJni*/, jobject /*aPeer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    BluetoothClientConnection *conn =
        reinterpret_cast<BluetoothClientConnection *>(
            static_cast<long>(aHandle));

    return conn->getRemoteAddress();
}

JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2OpenClientConnection
(JNIEnv* aJni, jobject /*peer*/, jlong aBtAddress, jint aChannel,
 jboolean aAuthenticate, jboolean aEncrypt, jint aReceiveMTU,
 jint aTransmitMTU, jlong aServer)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "+ JNI:l2OpenClientConnection: btAddress = %x", aBtAddress);

    BluetoothFunctionServer *server =
        reinterpret_cast<BluetoothFunctionServer *>(
            static_cast<long>(aServer));

    BluetoothClientConnection* conn = new BluetoothClientConnection(server);

    //Initialize Bluetooth Connection to L2CAP
    int error = conn->init(PROTOCOL_L2CAP);
    if (error != 0)
    {
        // If initialization was not successful,
        // then delete BluetoothClientConnection.
        conn->close();
        delete conn;
        ELOG1(EJavaBluetooth,
              "JNI:l2OpenClientConnection: Error during init: %d", error);

        jclass cls = aJni->FindClass(
                         "javax/bluetooth/BluetoothConnectionException");

        jmethodID constructorID = aJni->GetMethodID(cls, "<init>", "(I)V");
        jthrowable bcexception = (jthrowable) aJni->NewObject(
                                     cls, constructorID, BC_FAILED_NOINFO);

        aJni->Throw(bcexception);
        // free the local ref
        aJni->DeleteLocalRef(cls);
        return error;
    }

    //Connect to the server
    error = conn->connect(aBtAddress, aChannel, aAuthenticate, aEncrypt,
                          aReceiveMTU, aTransmitMTU);

    if (error<0)
    {
        ELOG1(EJavaBluetooth,
              "JNI:l2OpenClientConnection: Error during connect: %d",
              error);
        jclass cls = aJni->FindClass(
                         "javax/bluetooth/BluetoothConnectionException");
        jmethodID constructorID = aJni->GetMethodID(cls, "<init>", "(I)V");
        if (error == HCI_TIMEOUT)
        {
            jthrowable bcexception = (jthrowable) aJni->NewObject(
                                         cls, constructorID, BC_TIMEOUT);
            aJni->Throw(bcexception);
        }
        else
        {
            jthrowable bcexception = (jthrowable) aJni->NewObject(
                                         cls, constructorID, BC_FAILED_NOINFO);
            aJni->Throw(bcexception);
        }
        // free the local ref
        aJni->DeleteLocalRef(cls);
        conn->close();
        delete conn;
        return error;
    }

    LOG1(EJavaBluetooth, EInfo,
         "- JNI:l2OpenClientConnection: Handle = %ld", conn);
    return reinterpret_cast<jlong>(conn);
}

JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1rfClientOpen
(JNIEnv* aJni, jobject /*peer*/, jlong aBtAddress, jint aChannel,
 jboolean aAuthenticate, jboolean aEncrypt, jlong aServer)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo,
         "+ JNI:rfClientOpen: btAddress = %ld", aBtAddress);
    BluetoothFunctionServer *server =
        reinterpret_cast<BluetoothFunctionServer *>(
            static_cast<long>(aServer));

    BluetoothClientConnection* conn = new BluetoothClientConnection(server);

    //Initialize Bluetooth Connection to RFCOMM
    int error = conn->init(PROTOCOL_RFCOMM);
    if (error != 0)
    {
        // If initialization was not successful,
        // delete BluetoothClientConnection.
        conn->close();
        delete conn;
        ELOG1(EJavaBluetooth,
              "JNI:rfClientOpen: Error during init: %d", error);

        jclass cls = aJni->FindClass(
                         "javax/bluetooth/BluetoothConnectionException");
        jmethodID constructorID = aJni->GetMethodID(cls, "<init>", "(I)V");
        jthrowable bcexception = (jthrowable) aJni->NewObject(
                                     cls, constructorID, BC_FAILED_NOINFO);
        aJni->Throw(bcexception);
        // free the local ref
        aJni->DeleteLocalRef(cls);
        return error;
    }

    //Connect to the server
    error = conn->connect(aBtAddress, aChannel, aAuthenticate, aEncrypt);
    if (error<0)
    {
        ELOG1(EJavaBluetooth,
              "JNI:rfClientOpen: Error during connect: %d", error);
        jclass cls = aJni->FindClass(
                         "javax/bluetooth/BluetoothConnectionException");
        jmethodID constructorID = aJni->GetMethodID(cls, "<init>", "(I)V");
        if (error == HCI_TIMEOUT)
        {
            jthrowable bcexception = (jthrowable) aJni->NewObject(
                                         cls, constructorID, BC_TIMEOUT);
            aJni->Throw(bcexception);
        }
        else
        {
            jthrowable bcexception = (jthrowable) aJni->NewObject(
                                         cls, constructorID, BC_FAILED_NOINFO);
            aJni->Throw(bcexception);
        }
        // free the local ref
        aJni->DeleteLocalRef(cls);
        conn->close();
        delete conn;
        return error;
    }

    LOG1(EJavaBluetooth, EInfo, "- JNI:rfClientOpen: Handle %ld", conn);
    return reinterpret_cast<jlong>(conn);
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2GetTransmitMTU
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    BluetoothClientConnection *conn =
        reinterpret_cast<BluetoothClientConnection *>(
            static_cast<long>(aHandle));

    int retVal = conn->getTransmitMTU();
    return retVal;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2GetReceiveMTU
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    BluetoothClientConnection *conn =
        reinterpret_cast<BluetoothClientConnection *>(
            static_cast<long>(aHandle));

    int retVal = conn->getReceiveMTU();
    return retVal;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1available
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    BluetoothClientConnection *conn =
        reinterpret_cast<BluetoothClientConnection *>(
            static_cast<long>(aHandle));

    int retVal = conn->available();
    return retVal;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1clientReceive
(JNIEnv *aJni, jobject /*peer*/, jlong aHandle, jbyteArray aBuffer)
{
    JELOG2(EJavaBluetooth);
    BluetoothClientConnection *conn =
        reinterpret_cast<BluetoothClientConnection *>(
            static_cast<long>(aHandle));

    // Memory needs to be allocated inside ClientConnection implementation.
    // Reason to keep this here is to prevent passing JNI parameters to client.
    char* bufferToFill = NULL;
    int length = 0;

    // Length returned will be positive if no errors resent.
    // Else it will be error code.
    length = conn->receive(bufferToFill);

    if (length>0)
    {
        if (NULL == bufferToFill)
        {
            LOG(EJavaBluetooth, EInfo,
                "JNI_BluetoothStackS60_clientReceive: Buffer NULL");
            return 0; //EOF
        }

        LOG1(EJavaBluetooth, EInfo,
             "JNI_BluetoothStackS60_clientReceive: Data %s",
             bufferToFill);

        JNIArrayUtils::CopyToJava((*aJni),
                                  bufferToFill, length, aBuffer, 0, length);
    }
    LOG1(EJavaBluetooth, EInfo,
         "JNI_BluetoothStackS60_clientReceive: Returning %d", length);

    delete[] bufferToFill;
    return length;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1clientSend
(JNIEnv *aJni, jobject /*peer*/, jlong aHandle,
 jbyteArray aBuffer, jlong aLength)
{
    JELOG2(EJavaBluetooth);
    BluetoothClientConnection *conn =
        reinterpret_cast<BluetoothClientConnection *>(
            static_cast<long>(aHandle));

    char* bufferToSend = new char[aLength+1];

    JNIArrayUtils::CopyToNative((*aJni), aBuffer, 0, aLength, bufferToSend);
    bufferToSend[aLength] = '\0';
    int res = conn->send(bufferToSend, aLength);
    LOG1(EJavaBluetooth,EInfo,
         "Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1clientSend: Sent %d"
         , res);

    delete[] bufferToSend;

    return res;
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1clientClose
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    BluetoothClientConnection *conn =
        reinterpret_cast<BluetoothClientConnection *>(
            static_cast<long>(aHandle));

    conn->close();
    delete conn;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    registerForCallback
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1registerForCallback
(JNIEnv *aJni, jobject peer, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    BluetoothClientConnection *conn =
        reinterpret_cast<BluetoothClientConnection *>(
            static_cast<long>(aHandle));

    conn->registerCallback(aJni, peer);
}

//------------------------------------------------------------------------------
//    L2CAP Server Implementations
//------------------------------------------------------------------------------

JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1isL2ServerAlreadyRunning
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    L2CapPushServerConnection* server =
        reinterpret_cast<L2CapPushServerConnection *>(
            static_cast<long>(aHandle));

    return server->isActive();
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1restoreL2ServiceRecord(
    JNIEnv *aJni, jobject /*peer*/,
    jlong aHandle, jobject aServiceRecordImpl)
{
    JELOG2(EJavaBluetooth);
    L2CapPushServerConnection* server =
        reinterpret_cast<L2CapPushServerConnection *>(
            static_cast<long>(aHandle));

    L2CAPServerConnection *srvHandle = server->getServerObject();

    ServiceRecord* srvRecHandle = srvHandle->getServiceRecordHandle();
    if (NULL != srvRecHandle)
    {
        TRAP_IGNORE(
            srvRecHandle->restoreJavaServiceRecordL(aJni, aServiceRecordImpl));
    }
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    l2ServerOpen
 * Signature: (ZZZZZII)J
 */
JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2ServerOpen
(JNIEnv *aJni, jobject /*peer*/, jstring aUrl, jboolean aAuthorize,
 jboolean aAuthenticate, jboolean aEncrypt, jboolean aMaster,
 jint aReceiveMTU, jint aTransmitMTU)
{
    JELOG2(EJavaBluetooth);
    LOG2(EJavaBluetooth, EInfo,
         "+ JNI::l2ServerOpen TxMTU:%d RxMTU:%d",
         aTransmitMTU, aReceiveMTU);

    std::wstring connectionUrl;
    try
    {
        connectionUrl = JniUtils::jstringToWstring(aJni, aUrl);
    }
    catch (ExceptionBase ex)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::l2ServerOpen Exception:%S", ex.what());
        java::util::JniUtils::throwNewException(aJni,
                                                "java/lang/RunTimeException",
                                                "Opening L2CAP server connection failed");
        return NULL;
    }

    //Get handle to ServerConnectionFactory object.
    BtL2CapServerConnectionFactory& connectionFactory =
        BtL2CapServerConnectionFactory::getFactory();

    L2CapPushServerConnection *l2CapPushServer;

    ServerConnection *serverConn = connectionFactory.create(connectionUrl);
    l2CapPushServer =
        reinterpret_cast<L2CapPushServerConnection*>(serverConn);

    if (l2CapPushServer->isActive())
    {
        //if we have already accepted connection or are listening, we need not
        //go any further. return handle.
        LOG(EJavaBluetooth, EInfo,
            "- JNI::l2ServerOpen: Bluetooth server already running ");
        return reinterpret_cast<jlong>(l2CapPushServer);
    }
    else
    {
        LOG(EJavaBluetooth, EInfo,
            "  JNI::l2ServerOpen: Trying to create and run a bluetooth server ");

        L2CAPServerConnection* srvHandle = l2CapPushServer->getServerObject();

        LOG(EJavaBluetooth, EInfo,
            "  JNI::l2ServerOpen: Calling open on Server Object");
        TInt err;

        err = srvHandle->ServerOpen(aAuthorize, aAuthenticate, aEncrypt,
                                    aMaster, aReceiveMTU, aTransmitMTU);

        if (err != KErrNone)
        {
            ELOG1(EJavaBluetooth,
                  "- JNI::l2ServerOpen Error on L2CAPServerConnection.ServerOpen: %d",
                  err);
            if (l2CapPushServer->isCreatedByPush())
            {
                connectionFactory.releaseConnection(connectionUrl);
            }
            else
            {
                l2CapPushServer->close();
                delete l2CapPushServer;
            }
            java::util::JniUtils::throwNewException(aJni,
                                                    "java/io/IOException",
                                                    "Opening L2CAP server connection failed. Symbian OS error: "
                                                    + JavaCommonUtils::intToString(err));
            return NULL;
        }
        return reinterpret_cast<jlong>(l2CapPushServer);
    }
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    l2ServerGetPSM
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2ServerGetPSM
(JNIEnv *aJni, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    L2CapPushServerConnection* server =
        reinterpret_cast<L2CapPushServerConnection *>(
            static_cast<long>(aHandle));

    L2CAPServerConnection *srvHandle = server->getServerObject();

    //We should never be here if push plugin is already running.
    int psmVal = srvHandle->GetServerPSM();

    if (psmVal < 0)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::l2ServerGetPSM Error while GetServerPSM: %d",
              psmVal);

        java::util::JniUtils::throwNewException(aJni,
                                                "java/io/IOException",
                                                "Opening L2CAP server connection failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(psmVal));
    }

    LOG1(EJavaBluetooth, EInfo, "- JNI::l2ServerGetPSM PSM value:%d ", psmVal);
    return psmVal;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    l2ServerGetServiceRecord
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2ServerGetServiceRecord
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);

    L2CapPushServerConnection* server =
        reinterpret_cast<L2CapPushServerConnection *>(
            static_cast<long>(aHandle));

    L2CAPServerConnection *srvHandle = server->getServerObject();
    int servRecId = 0;
    ServiceRecord* srvRecHandle = srvHandle->getServiceRecordHandle();

    if (NULL != srvRecHandle)
    {
        servRecId = srvRecHandle->getServiceRecordID();
    }
    LOG1(EJavaBluetooth, EInfo,
         "- JNI::l2ServerGetServiceRecord: %d", servRecId);
    return servRecId;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    l2capRegisterSDPRecord
 * Signature: (JI[BLjava/lang/String;)I
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2capRegisterSDPRecord
(JNIEnv *aJni, jobject /*peer*/, jlong aHandle, jint aPsmVal,
 jbyteArray aUuid, jstring aServerName)
{
    JELOG2(EJavaBluetooth);
    TInt err;

    L2CapPushServerConnection* server =
        reinterpret_cast<L2CapPushServerConnection *>(
            static_cast<long>(aHandle));

    if (server->isActive())
    {
        //If the server is active it means that it has already done registration
        //and has started advertising.
        //If connection has already been accepted, then no issues at all.
        return;
    }

    L2CAPServerConnection *srvHandle = server->getServerObject();

    jbyte* byteArray = aJni->GetByteArrayElements(aUuid, NULL);

    TPtrC8 uuidDes(reinterpret_cast<TUint8*>(byteArray),
                   aJni->GetArrayLength(aUuid));
    TUUID uuid;

    TRAP(err, uuid.SetL(uuidDes));
    if (err != KErrNone)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::l2capRegisterSDPRecord Error while converting UUID: %d",
              err);
        java::util::JniUtils::throwNewException(aJni,
                                                "java/io/IOException",
                                                "Opening L2CAP server connection failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(err));
        return;
    }

    JStringUtils servName(*aJni, aServerName);
    TPtrC8 servNameP(
        reinterpret_cast<const TUint8*>(servName.Ptr()),
        servName.Size());

    err = srvHandle->initializeServiceRecord(aPsmVal, uuid, servNameP);

    if (err != KErrNone)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::l2capRegisterSDPRecord Error while initializeServiceRecord: %d",
              err);
        java::util::JniUtils::throwNewException(aJni,
                                                "java/io/IOException",
                                                "Opening L2CAP server connection failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(err));
    }

}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    l2ServerAcceptAndOpenServerConnection
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2ServerAcceptAndOpenServerConnection
(JNIEnv *aJni, jobject /*peer*/, jlong aHandle, jintArray aReceiveMtu)
{
    JELOG2(EJavaBluetooth);

    BluetoothClientConnection *clientHandle;
    int receiveMtu = 0;

    L2CapPushServerConnection* server =
        reinterpret_cast<L2CapPushServerConnection *>(
            static_cast<long>(aHandle));

    if (server->isConnectionAccepted())
    {
        LOG(EJavaBluetooth, EInfo,
            "  JNI::l2ServerAcceptAndOpenServerConnection: returning already accepted push client ");
        //Since connection has already been accepted, return the client object.
        clientHandle = server->getConnectedClient();
        receiveMtu = clientHandle->getReceiveMTU();
        aJni->SetIntArrayRegion(aReceiveMtu, 0, 1, &receiveMtu);
        return reinterpret_cast<jlong>(clientHandle);
    }

    if (server->isListening())
    {
        //Wait till connect and then return.
        //Then we return client here.
        java::util::Monitor* acceptMonitor =
            java::util::Monitor::createMonitor();

        server->setAcceptMonitor(acceptMonitor);
        LOG(EJavaBluetooth, EInfo,
            "  JNI::l2ServerAcceptAndOpenServerConnection: waiting for connection accept ");
        acceptMonitor->wait();
        server->unsetAcceptMonitor();
        delete acceptMonitor;

        clientHandle = server->getConnectedClient();
        receiveMtu = clientHandle->getReceiveMTU();
        aJni->SetIntArrayRegion(aReceiveMtu, 0, 1, &receiveMtu);
        return reinterpret_cast<jlong>(clientHandle);
    }

    // NOTE: In case we reached here, it means that there was no corresponding
    // push plugin active when the midlet was launched.
    // So we go ahead with usual stuff.

    L2CAPServerConnection *srvHandle = server->getServerObject();

    long retVal = srvHandle->Accept();

    if (retVal < KErrNone)
    {
        ELOG1(EJavaBluetooth,
              "  JNI::l2ServerAcceptAndOpenServerConnection Error while Accept: %d ",
              retVal);

        if (KErrCancel == retVal)
        {
            java::util::JniUtils::throwNewException(aJni,
                                                    "java/io/InterruptedIOException",
                                                    "acceptAndOpen() failed: L2CAP server connection is closed");
        }
        else
        {
            java::util::JniUtils::throwNewException(aJni,
                                                    "java/io/IOException",
                                                    "acceptAndOpen() on L2CAP server connection failed. Symbian OS error: "
                                                    + JavaCommonUtils::intToString(retVal));
        }
    }
    else
    {
        clientHandle = reinterpret_cast<BluetoothClientConnection *>(retVal);
        receiveMtu = clientHandle->getReceiveMTU();
        aJni->SetIntArrayRegion(aReceiveMtu, 0, 1, &receiveMtu);
    }
    return retVal;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    l2ServerClose
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1l2ServerClose
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    L2CapPushServerConnection* server =
        reinterpret_cast<L2CapPushServerConnection *>(
            static_cast<long>(aHandle));

    BtL2CapServerConnectionFactory& connectionFactory =
        BtL2CapServerConnectionFactory::getFactory();

    std::wstring uri = server->getUri();

    server->unsetClearServiceClassBitsFlag();
    if (server->isCreatedByPush())
    {
        connectionFactory.releaseConnection(uri);
    }
    else
    {
        server->close();
        delete server;
    }

    return 0;
}

//-----------------------------------------------------------------------------
//    RFCOMM Server Implementations
//-----------------------------------------------------------------------------


JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1isRfServerAlreadyRunning
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    RFCOMMPushServerConnection* server =
        reinterpret_cast<RFCOMMPushServerConnection *>(
            static_cast<long>(aHandle));

    return server->isActive();
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1restoreRfServiceRecord(
    JNIEnv *aJni, jobject /*peer*/,
    jlong aHandle, jobject aServiceRecordImpl)
{
    JELOG2(EJavaBluetooth);

    RFCOMMPushServerConnection* server =
        reinterpret_cast<RFCOMMPushServerConnection *>(
            static_cast<long>(aHandle));

    RFCOMMServerConnection *srvHandle = server->getServerObject();

    ServiceRecord* srvRecHandle = srvHandle->getServiceRecordHandle();

    if (NULL != srvRecHandle)
    {
        TRAP_IGNORE(
            srvRecHandle->restoreJavaServiceRecordL(aJni, aServiceRecordImpl));
    }
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    rfServerOpen
 * Signature: (ZZZJ)J
 */
JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1rfServerOpen
(JNIEnv *aJni, jobject /*peer*/, jstring aUrl, jboolean aAuthorize,
 jboolean aAuthenticate, jboolean aEncrypt, jboolean aIsGOEP)
{
    JELOG2(EJavaBluetooth);

    std::wstring connectionUrl;
    try
    {
        connectionUrl = JniUtils::jstringToWstring(aJni, aUrl);
    }
    catch (ExceptionBase ex)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::rfServerOpen Exception:%S", ex.what());
        java::util::JniUtils::throwNewException(aJni,
                                                "java/lang/RunTimeException",
                                                "Opening server connection failed");
        return NULL;
    }

    //Get handle to ServerConnectionFactory object.
    ServerConnectionFactory* connectionFactory = 0;
    if (!aIsGOEP)
    {
        connectionFactory = &BTRFCOMMServerConnectionFactory::getFactory();
    }
    else
    {
        connectionFactory = &BTOBEXServerConnectionFactory::getFactory();
    }

    RFCOMMPushServerConnection *rfcommPushServer;

    ServerConnection *serverConn =
        connectionFactory->create(connectionUrl);
    rfcommPushServer =
        reinterpret_cast<RFCOMMPushServerConnection*>(serverConn);

    if (rfcommPushServer->isActive())
    {
        //if we have already accepted connection or are listening, we need not
        //go any further. return handle.
        LOG(EJavaBluetooth, EInfo,
            "- JNI::rfServerOpen: Bluetooth server already running ");
        return reinterpret_cast<jlong>(rfcommPushServer);
    }
    else
    {
        LOG(EJavaBluetooth, EInfo,
            "  JNI::rfServerOpen: Opening server ");
        RFCOMMServerConnection* srvHandle =
            rfcommPushServer->getServerObject();

        TInt err;

        err = srvHandle->ServerOpen(aAuthorize, aAuthenticate, aEncrypt);

        if (err != KErrNone)
        {
            ELOG1(EJavaBluetooth,
                  "- JNI::rfServerOpen Error on RFCOMMServerConnection.ServerOpen: %d",
                  err);
            if (rfcommPushServer->isCreatedByPush())
            {
                connectionFactory->releaseConnection(connectionUrl);
            }
            else
            {
                rfcommPushServer->close();
                delete rfcommPushServer;
            }

            java::util::JniUtils::throwNewException(aJni,
                                                    "java/io/IOException",
                                                    "Opening server connection failed. Symbian OS error: "
                                                    + JavaCommonUtils::intToString(err));
            return NULL;
        }
    }
    return reinterpret_cast<jlong>(rfcommPushServer);
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    rfcommRegisterSDPRecord
 * Signature: (JI[BLjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1rfcommRegisterSDPRecord
(JNIEnv *aJni, jobject /*peer*/, jlong aHandle, jint aChannel,
 jbyteArray aUuid, jstring aServerName, jboolean aIsObex)
{
    JELOG2(EJavaBluetooth);
    TInt err;

    RFCOMMPushServerConnection* server =
        reinterpret_cast<RFCOMMPushServerConnection *>(
            static_cast<long>(aHandle));

    if (server->isActive())
    {
        //If the server is active it means that it has already done
        //registration and has started advertising.
        //If connection has already been accepted, then no issues at all.
        return;
    }

    LOG(EJavaBluetooth, EInfo,
        "  JNI::rfcommRegisterSDPRecord Getting server object");

    RFCOMMServerConnection *srvHandle = server->getServerObject();

    jbyte* byteArray = aJni->GetByteArrayElements(aUuid, NULL);
    TPtrC8 uuidDes(
        reinterpret_cast<TUint8*>(byteArray), aJni->GetArrayLength(aUuid));
    TUUID uuid;

    TRAP(err, uuid.SetL(uuidDes));
    if (err != KErrNone)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::rfcommRegisterSDPRecord Error while converting UUID: %d",
              err);
        java::util::JniUtils::throwNewException(aJni,
                                                "java/io/IOException",
                                                "Opening server connection failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(err));
        return;
    }

    JStringUtils servName(*aJni, aServerName);
    TPtrC8 servNameP(
        reinterpret_cast<const TUint8*>(servName.Ptr()), servName.Size());

    err = srvHandle->initializeServiceRecord(
              aChannel, uuid, servNameP, aIsObex);

    if (err != KErrNone)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::rfcommRegisterSDPRecord Error while initializeServiceRecord: %d",
              err);

        java::util::JniUtils::throwNewException(aJni,
                                                "java/io/IOException",
                                                "Opening server connection failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(err));
    }
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    rfGetChannel
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1rfGetChannel
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);

    RFCOMMPushServerConnection* server =
        reinterpret_cast<RFCOMMPushServerConnection *>(
            static_cast<long>(aHandle));

    RFCOMMServerConnection *srvHandle = server->getServerObject();
    int channel = srvHandle->GetRfListeningChannel();

    LOG1(EJavaBluetooth, EInfo, "- JNI::rfGetChannel: %d", channel);
    return channel;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    rfServerGetServiceRecord
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1rfServerGetServiceRecord
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);

    RFCOMMPushServerConnection* server =
        reinterpret_cast<RFCOMMPushServerConnection *>(
            static_cast<long>(aHandle));

    RFCOMMServerConnection *srvHandle = server->getServerObject();
    int servRecId = 0;
    ServiceRecord* srvRecHandle = srvHandle->getServiceRecordHandle();

    if (NULL != srvRecHandle)
    {
        srvRecHandle->getServiceRecordID();
    }
    LOG1(EJavaBluetooth, EInfo,
         "- JNI::rfServerGetServiceRecord: %d", srvRecHandle);
    return servRecId;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    rfServerAcceptAndOpenServerConnection
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1rfServerAcceptAndOpenServerConnection
(JNIEnv *aJni, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);

    RFCOMMPushServerConnection* server =
        reinterpret_cast<RFCOMMPushServerConnection *>(
            static_cast<long>(aHandle));

    if (server->isConnectionAccepted())
    {
        LOG(EJavaBluetooth, EInfo,
            "  JNI::rfServerAcceptAndOpenServerConnection: returning already accepted push client ");
        //Since connection has already been accepted, return the client object.
        return reinterpret_cast<jlong>(server->getConnectedClient());
    }

    if (server->isListening())
    {
        //Wait till connect and then return.
        //Then we return client here.
        java::util::Monitor* acceptMonitor =
            java::util::Monitor::createMonitor();

        server->setAcceptMonitor(acceptMonitor);
        LOG(EJavaBluetooth, EInfo,
            "  JNI::rfServerAcceptAndOpenServerConnection: waiting for connection accept ");
        acceptMonitor->wait();
        server->unsetAcceptMonitor();
        delete acceptMonitor;

        return reinterpret_cast<jlong>(server->getConnectedClient());
    }

    LOG(EJavaBluetooth, EInfo,
        "  JNI::rfServerAcceptAndOpenServerConnection Getting server object");

    //NOTE: In case we reached here, it means that there was no corresponding
    //push plugin active when the midlet was launched.
    //So we go ahead with usual stuff.

    RFCOMMServerConnection *srvHandle = server->getServerObject();

    long clientHandle;

    clientHandle = srvHandle->Accept();

    if (clientHandle < KErrNone)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::rfServerAcceptAndOpenServerConnection Error while Accept: %d ",
              clientHandle);

        if (KErrCancel == clientHandle)
        {
            java::util::JniUtils::throwNewException(aJni,
                                                    "java/io/InterruptedIOException",
                                                    "acceptAndOpen() on server connection failed: connection is closed ");
        }
        else
        {
            java::util::JniUtils::throwNewException(aJni,
                                                    "java/io/IOException",
                                                    "acceptAndOpen() on server connection failed. Symbian OS error: "
                                                    + JavaCommonUtils::intToString(clientHandle));
        }
    }
    return (clientHandle);
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    rfServerClose
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1rfServerClose
(JNIEnv* /*aJni*/, jobject /*peer*/, jlong aHandle)
{
    JELOG2(EJavaBluetooth);

    RFCOMMPushServerConnection* server =
        reinterpret_cast<RFCOMMPushServerConnection *>(
            static_cast<long>(aHandle));

    ServerConnectionFactory* connectionFactory = 0;
    if (!(server->isGOEPConnection()))
    {
        connectionFactory = &BTRFCOMMServerConnectionFactory::getFactory();
    }
    else
    {
        connectionFactory = &BTOBEXServerConnectionFactory::getFactory();
    }
    std::wstring uri = server->getUri();

    server->unsetClearServiceClassBitsFlag();
    if (server->isCreatedByPush())
    {
        connectionFactory->releaseConnection(uri);
    }
    else
    {
        server->close();
        delete server;
    }
    return 0;
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    initializeUpdateRecord
 * Signature: (Ljava/lang/String;J)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1initializeUpdateRecord
(JNIEnv *aJni, jobject /*peer*/,
 jstring aProtocol, jlong aHandle, jint aDeviceServiceClasses)
{
    JELOG2(EJavaBluetooth);
    ServiceRecord* srvRec;

    std::wstring protocol;

    try
    {
        protocol = java::util::JniUtils::jstringToWstring(aJni, aProtocol);
    }
    catch (ExceptionBase ex)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::initializeUpdateRecord Exception:%S", ex.what());
        java::util::JniUtils::throwNewException(aJni,
                                                "java/lang/RunTimeException", "Updating service record failed");
        return;
    }

    if (0 == protocol.compare(L"btl2cap"))
    {
        L2CapPushServerConnection* server =
            reinterpret_cast<L2CapPushServerConnection *>(
                static_cast<long>(aHandle));

        L2CAPServerConnection *srvHandle = server->getServerObject();
        srvRec = srvHandle->getServiceRecordHandle();
    }
    else //if  protocol is "btspp"
    {
        RFCOMMPushServerConnection* server =
            reinterpret_cast<RFCOMMPushServerConnection *>(
                static_cast<long>(aHandle));

        RFCOMMServerConnection *srvHandle = server->getServerObject();
        srvRec = srvHandle->getServiceRecordHandle();
    }

    srvRec->initializeUpdateRecord(aDeviceServiceClasses);
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    completesUpdateRecord
 * Signature: (Ljava/lang/String;J)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1completesUpdateRecord
(JNIEnv *aJni, jobject /*peer*/, jstring aProtocol, jlong aHandle)
{
    JELOG2(EJavaBluetooth);
    ServiceRecord* srvRec;

    std::wstring protocol;

    try
    {
        protocol = java::util::JniUtils::jstringToWstring(aJni, aProtocol);
    }
    catch (ExceptionBase ex)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::completesUpdateRecord Exception:%S", ex.what());
        java::util::JniUtils::throwNewException(aJni,
                                                "java/lang/RunTimeException", "Updating service record failed");
        return;
    }
    if (0 == protocol.compare(L"btl2cap"))
    {
        L2CapPushServerConnection* server =
            reinterpret_cast<L2CapPushServerConnection *>(
                static_cast<long>(aHandle));

        L2CAPServerConnection *srvHandle = server->getServerObject();

        srvRec = srvHandle->getServiceRecordHandle();
    }
    else // if protocol is "btspp"
    {
        RFCOMMPushServerConnection* server =
            reinterpret_cast<RFCOMMPushServerConnection *>(
                static_cast<long>(aHandle));

        RFCOMMServerConnection *srvHandle = server->getServerObject();

        srvRec = srvHandle->getServiceRecordHandle();
    }

    srvRec->completesUpdateRecord();
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    sdpAddAttribute
 * Signature: (Ljava/lang/String;JIIJ[B)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1sdpAddAttribute
(JNIEnv *aJni, jobject /*peer*/, jstring aProtocol, jlong aHandle,
 jint aAttrID, jint aAttrType, jlong aNumValue, jbyteArray aBytesValue)
{
    JELOG2(EJavaBluetooth);
    ServiceRecord* srvRec;
    int err = 0;

    std::wstring protocol;

    try
    {
        protocol = java::util::JniUtils::jstringToWstring(aJni, aProtocol);
    }
    catch (ExceptionBase ex)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::sdpAddAttribute Exception:%S", ex.what());
        java::util::JniUtils::throwNewException(aJni,
                                                "java/lang/RunTimeException", "Updating service record failed");
        return;
    }
    if (0 == protocol.compare(L"btl2cap"))
    {
        L2CapPushServerConnection* server =
            reinterpret_cast<L2CapPushServerConnection *>(
                static_cast<long>(aHandle));

        L2CAPServerConnection *srvHandle = server->getServerObject();

        srvRec = srvHandle->getServiceRecordHandle();
    }
    else // if protocol is "btspp"
    {
        RFCOMMPushServerConnection* server =
            reinterpret_cast<RFCOMMPushServerConnection *>(
                static_cast<long>(aHandle));

        RFCOMMServerConnection *srvHandle = server->getServerObject();

        srvRec = srvHandle->getServiceRecordHandle();
    }

    LOG1(EJavaBluetooth, EInfo,
         "  JNI::sdpAddAttribute aAttrType:%d", aAttrType);
    switch (aAttrType)
    {
    case DataElement_NULL:
    {
        err = srvRec->setAttributeNil(aAttrID);
        break;
    }
    case DataElement_BOOL:
    {
        TBool val = (aNumValue) ? ETrue : EFalse;

        err = srvRec->setAttributeBool(aAttrID, val);
        break;
    }
    case DataElement_U_INT_1:
    case DataElement_INT_1:
    case DataElement_U_INT_2:
    case DataElement_INT_2:
    case DataElement_U_INT_4:
    case DataElement_INT_4:
    {
        ELOG(EJavaBluetooth, "  JNI::sdpAddAttribute UINT/INT/1/2/4 element.");
        TSdpIntBuf<TUint> val(aNumValue);

        err = srvRec->setAttributeInt(aAttrID, aAttrType, val);
        break;
    }
    case DataElement_INT_8:
    {
        TSdpIntBuf<TUint64> val(aNumValue);

        err = srvRec->setAttributeInt(aAttrID, aAttrType, val);
        break;
    }
    case DataElement_U_INT_8:
    case DataElement_U_INT_16:
    case DataElement_INT_16:
    {
        jbyte* bytes = aJni->GetByteArrayElements(aBytesValue, NULL);
        TPtrC8 val(reinterpret_cast<TUint8*>(bytes),
                   aJni->GetArrayLength(aBytesValue));

        err = srvRec->setAttributeUint(aAttrID, aAttrType, val);
        break;
    }
    case DataElement_UUID:
    {
        jbyte* bytes = aJni->GetByteArrayElements(aBytesValue, NULL);
        TPtrC8 uuidDes(reinterpret_cast<TUint8*>(bytes),
                       aJni->GetArrayLength(aBytesValue));

        TUUID val;

        TRAP(err, val.SetL(uuidDes));
        if (err != KErrNone)
        {
            ELOG1(EJavaBluetooth,
                  "- JNI::sdpAddAttribute Error while converting UUID: %d",
                  err);
            java::util::JniUtils::throwNewException(aJni,
                                                    "javax/bluetooth/ServiceRegistrationException",
                                                    "Updating service record failed. Symbian OS error: "
                                                    + JavaCommonUtils::intToString(err));
            return;
        }
        err = srvRec->setAttributeUUID(aAttrID, val);
        break;
    }
    case DataElement_STRING:
    {
        jbyte* bytes = aJni->GetByteArrayElements(aBytesValue, NULL);
        TPtrC8 val(reinterpret_cast<TUint8*>(bytes),
                   aJni->GetArrayLength(aBytesValue));

        err = srvRec->setAttributeString(aAttrID, val);
        break;
    }
    case DataElement_URL:
    {
        jbyte* bytes = aJni->GetByteArrayElements(aBytesValue, NULL);
        TPtrC8 val(reinterpret_cast<TUint8*>(bytes),
                   aJni->GetArrayLength(aBytesValue));

        err = srvRec->setAttributeUrl(aAttrID, val);
        break;
    }
    default:
        break;
    }

    if (KErrNone != err)
    {

        ELOG1(EJavaBluetooth,
              "- JNI::sdpAddAttribute Failed to update service record %d",
              err);
        java::util::JniUtils::throwNewException(aJni,
                                                "javax/bluetooth/ServiceRegistrationException",
                                                "Updating service record failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(err));
    }
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    sdpAttributeListStart
 * Signature: (Ljava/lang/String;JII)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1sdpAttributeListStart
(JNIEnv *aJni, jobject /*peer*/, jstring aProtocol, jlong aHandle,
 jint aAttrID, jint aAttrType)
{
    JELOG2(EJavaBluetooth);
    ServiceRecord* srvRec;
    int err = 0;
    std::wstring protocol;

    try
    {
        protocol = java::util::JniUtils::jstringToWstring(aJni, aProtocol);
    }
    catch (ExceptionBase ex)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::sdpAttributeListStart Exception:%S", ex.what());
        java::util::JniUtils::throwNewException(aJni,
                                                "java/lang/RunTimeException", "Updating service record failed");
        return;
    }
    if (0 == protocol.compare(L"btl2cap"))
    {
        L2CapPushServerConnection* server =
            reinterpret_cast<L2CapPushServerConnection *>(
                static_cast<long>(aHandle));

        L2CAPServerConnection *srvHandle = server->getServerObject();

        srvRec = srvHandle->getServiceRecordHandle();
    }
    else // if protocol is "btspp"
    {
        RFCOMMPushServerConnection* server =
            reinterpret_cast<RFCOMMPushServerConnection *>(
                static_cast<long>(aHandle));

        RFCOMMServerConnection *srvHandle = server->getServerObject();

        srvRec = srvHandle->getServiceRecordHandle();
    }

    err = srvRec->attributeListStart(aAttrID, aAttrType);

    if (KErrNone != err)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::sdpAttributeListStart Error while attributeListStart: %d",
              err);
        java::util::JniUtils::throwNewException(aJni,
                                                "javax/bluetooth/ServiceRegistrationException",
                                                "Updating service record failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(err));
    }
}

/*
 * Class:     com_nokia_mj_impl_bluetooth_BluetoothStackS60
 * Method:    sdpAttributeListEnd
 * Signature: (Ljava/lang/String;JI)V
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1sdpAttributeListEnd
(JNIEnv *aJni, jobject /*peer*/, jstring aProtocol, jlong aHandle,
 jint aAttrID)
{
    JELOG2(EJavaBluetooth);
    ServiceRecord* srvRec;
    int err = 0;
    std::wstring protocol;

    try
    {
        protocol = java::util::JniUtils::jstringToWstring(aJni, aProtocol);
    }
    catch (ExceptionBase ex)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::sdpAttributeListEnd Exception:%S", ex.what());
        java::util::JniUtils::throwNewException(aJni,
                                                "java/lang/RunTimeException", "Updating service record failed");
        return;
    }
    if (0 == protocol.compare(L"btl2cap"))
    {
        L2CapPushServerConnection* server =
            reinterpret_cast<L2CapPushServerConnection *>(
                static_cast<long>(aHandle));

        L2CAPServerConnection *srvHandle = server->getServerObject();

        srvRec = srvHandle->getServiceRecordHandle();
    }
    else // if protocol is "btspp"
    {
        RFCOMMPushServerConnection* server =
            reinterpret_cast<RFCOMMPushServerConnection *>(
                static_cast<long>(aHandle));

        RFCOMMServerConnection *srvHandle = server->getServerObject();

        srvRec = srvHandle->getServiceRecordHandle();
    }

    err = srvRec->attributeListEnd(aAttrID);

    if (KErrNone != err)
    {
        ELOG1(EJavaBluetooth,
              "- JNI::sdpAttributeListEnd Error while attributeListEnd: %d",
              err);

        java::util::JniUtils::throwNewException(aJni,
                                                "javax/bluetooth/ServiceRegistrationException",
                                                "Updating service record failed. Symbian OS error: "
                                                + JavaCommonUtils::intToString(err));
    }
}

//
// JNI functions specific to Remote Device operations
//


JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1isAuthenticated
(JNIEnv* /*aJni*/, jobject /*aPeer*/, jlong aRemoteAddress)
{
    JELOG2(EJavaBluetooth);

    jboolean ret = java::bluetooth::BluetoothRemoteDevice::getSecurityProperty(
                       REMOTE_AUTHENTICATED, aRemoteAddress);

    return ret;
}

JNIEXPORT jboolean JNICALL
Java_com_nokia_mj_impl_bluetooth_BluetoothStackS60__1isTrusted
(JNIEnv* /*aJni*/, jobject /*aPeer*/, jlong aRemoteAddress)
{
    JELOG2(EJavaBluetooth);

    jboolean ret = java::bluetooth::BluetoothRemoteDevice::getSecurityProperty(
                       REMOTE_TRUSTED, aRemoteAddress);

    return ret;
}
