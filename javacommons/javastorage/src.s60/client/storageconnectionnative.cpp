/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  storageconnectionnative
*
*/


#include "com_nokia_mj_impl_storage_StorageConnection.h"
#include "commsmessage.h"
#include "javacommonutils.h"
#include "javadataaccess.h"
#include "javajniutils.h"
#include "javastorageexception.h"
#include "logger.h"

using namespace java::comms;
using namespace java::storage;
using namespace java::util;

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_storage_StorageConnection__1startSession
(JNIEnv *, jclass)
{
    JELOG2(EJavaStorage);
    JavaDataAccess* dataAccess = JavaDataAccess::createInstance();
    // Return handle to session. Utilize the fact that in Symbian
    // all pointer addresses are MOD 4 so the last 2 bits are 0
    // and can be shifted out. This way the returned handle is
    // always positive whereas Symbian error codes are always negative.
    return reinterpret_cast<TUint>(dataAccess)>>2;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_storage_StorageConnection__1closeSession
(JNIEnv *, jclass, jint aSessionHandle)
{
    JELOG2(EJavaStorage);
    JavaDataAccess* dataAccess =
        reinterpret_cast<JavaDataAccess*>(aSessionHandle<<2);

    delete dataAccess;
    dataAccess = NULL;
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_storage_StorageConnection__1open
(JNIEnv *aEnv, jobject, jint aSessionHandle, jstring aHeaders, jstring aStorageName)
{
    JELOG2(EJavaStorage);
    JavaDataAccess* dataAccess =
        reinterpret_cast<JavaDataAccess*>(aSessionHandle<<2);

    CommsMessage receivedMessage;
    const char* tempHeaders = aEnv->GetStringUTFChars(aHeaders, 0);
    const char* tempStorageName = aEnv->GetStringUTFChars(aStorageName, 0);
    std::string headers(tempHeaders);
    std::string storageName(tempStorageName);

    aEnv->ReleaseStringUTFChars(aHeaders, tempHeaders);
    aEnv->ReleaseStringUTFChars(aStorageName, tempStorageName);

    try
    {
        dataAccess->open(headers, storageName, receivedMessage);
    }
    catch (JavaStorageException& jse)
    {
        JniUtils::throwNewException(
            aEnv,
            "com/nokia/mj/impl/storage/StorageException",
            jse.toString().c_str());

        // Return control to Java side. Otherwise JNI Error occur about
        // pending exception.
        return 0;
    }

    int statusCode = -1;
    receivedMessage>>statusCode;

    std::string responseBody = "";
    receivedMessage>>responseBody;

    if (statusCode < 0 || responseBody.length() == 0)
    {
        WLOG1(EJavaStorage, "Server error. ErrorCode: %d", statusCode);

        std::string statusStr = "Server error. ErrorCode: ";
        statusStr.append(JavaCommonUtils::intToString(statusCode));

        JniUtils::throwNewException(
            aEnv,
            "com/nokia/mj/impl/storage/StorageException",
            statusStr.c_str());

        // Return control to Java side. Otherwise JNI Error occur about
        // pending exception.
        return 0;
    }

    return aEnv->NewStringUTF(responseBody.c_str());
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_storage_StorageConnection__1close
(JNIEnv *aEnv, jobject, jint aSessionHandle, jstring aHeaders)
{
    JELOG2(EJavaStorage);
    JavaDataAccess* dataAccess =
        reinterpret_cast<JavaDataAccess*>(aSessionHandle<<2);

    CommsMessage receivedMessage;
    const char* tempHeaders = aEnv->GetStringUTFChars(aHeaders, 0);
    std::string headers(tempHeaders);

    aEnv->ReleaseStringUTFChars(aHeaders, tempHeaders);

    try
    {
        dataAccess->close(headers, receivedMessage);
    }
    catch (JavaStorageException& jse)
    {
        JniUtils::throwNewException(
            aEnv,
            "com/nokia/mj/impl/storage/StorageException",
            jse.toString().c_str());

        // Return control to Java side. Otherwise JNI Error occur about
        // pending exception.
        return;
    }

    int statusCode = -1;
    receivedMessage>>statusCode;

    std::string responseBody = "";
    receivedMessage>>responseBody;

    if (statusCode < 0 || responseBody.length() == 0)
    {
        WLOG1(EJavaStorage, "Server error. ErrorCode: %d", statusCode);

        std::string statusStr = "Server error. ErrorCode: ";
        statusStr.append(JavaCommonUtils::intToString(statusCode));

        JniUtils::throwNewException(
            aEnv,
            "com/nokia/mj/impl/storage/StorageException",
            statusStr.c_str());

        // Return control to Java side. Otherwise JNI Error occur about
        // pending exception.
        return;
    }
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_storage_StorageConnection__1execute
(JNIEnv *aEnv, jobject, jint aSessionHandle, jstring aHeaders, jstring aSqlStatement)
{
    JELOG2(EJavaStorage);

    JavaDataAccess* dataAccess =
        reinterpret_cast<JavaDataAccess*>(aSessionHandle<<2);

    CommsMessage receivedMessage;
    const char* tempHeaders = aEnv->GetStringUTFChars(aHeaders, 0);
    std::string headers(tempHeaders);

    aEnv->ReleaseStringUTFChars(aHeaders, tempHeaders);

    const jchar* sqlStmtTemp = aEnv->GetStringChars(aSqlStatement, 0);

    std::wstring sqlStatement(
        (wchar_t*)sqlStmtTemp, aEnv->GetStringLength(aSqlStatement));

    aEnv->ReleaseStringChars(aSqlStatement, sqlStmtTemp);

    // ########################################################################
    // LOG1WSTR(EJavaStorage, EInfo, "Java Sql: '%s'", sqlStatement);
    // ########################################################################

    try
    {
        dataAccess->execute(headers, sqlStatement, receivedMessage);
    }
    catch (JavaStorageException& jse)
    {
        ELOG(EJavaStorage, "Execute failed to storage exception.");

        JniUtils::throwNewException(
            aEnv,
            "com/nokia/mj/impl/storage/StorageException",
            jse.toString().c_str());

        // Return control to Java side. Otherwise JNI Error occur about
        // pending exception.
        return 0;
    }
    catch (...)
    {
        ELOG(EJavaStorage, "Execute failed to unexp exception.");

        JniUtils::throwNewException(
            aEnv,
            "com/nokia/mj/impl/storage/StorageException",
            "Internal error");

        // Return control to Java side. Otherwise JNI Error occur about
        // pending exception.
        return 0;

    }

    int statusCode = -1;
    receivedMessage>>statusCode;

    std::wstring responseBody = L"";
    receivedMessage>>responseBody;

    // ########################################################################
    // LOG1WSTR(EJavaStorage, EInfo, "Server response: '%s'", responseBody);
    // ########################################################################

    if (statusCode < 0 || responseBody.length() == 0)
    {
        WLOG1(EJavaStorage, "Server error. ErrorCode: %d", statusCode);

        std::string statusStr = "Server error. ErrorCode: ";
        statusStr.append(JavaCommonUtils::intToString(statusCode));

        JniUtils::throwNewException(
            aEnv,
            "com/nokia/mj/impl/storage/StorageException",
            statusStr.c_str());

        // Return control to Java side. Otherwise JNI Error occur about
        // pending exception.
        return 0;
    }

    return aEnv->NewString(
               (const jchar*)responseBody.c_str(), responseBody.length());
}
