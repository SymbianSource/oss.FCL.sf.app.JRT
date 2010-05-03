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


#include "com_nokia_mj_impl_push_PushRegistryImpl.h"
#include "logger.h"
#include "pushregistry.h"
#include "pushexception.h"
#include "javajniutils.h"
#include "javacommonutils.h"
#include "pusherrorcodes.h"
#include "libraryloaderexception.h"
#include "dynamiclibloader.h"
#include "pushcontrollerstarter.h"
#include "pushcontrollerstarterimpl.h"

using namespace java::push;
using namespace java::runtime;
using namespace java::util;

class StaticImplObjContainer
{
public:
    StaticImplObjContainer():mImpl(0) {}

    ~StaticImplObjContainer() {}

    PushRegistry* mImpl;
};

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static StaticImplObjContainer* sImpObjContainer = 0;
#endif

#ifdef __SYMBIAN32__
PushRegistry& getPushRegistryInstance()
#else
extern "C" PushRegistry& getPushRegistryInstance()
#endif
{
    JELOG2(EJavaPush);
    StaticImplObjContainer* container = 0;
#if defined(__SYMBIAN32__) && defined(__WINSCW__)
    TUid uid = TUid::Uid(0xE0000020);
    container = Pls<StaticImplObjContainer>(uid);
#else
    if (sImpObjContainer == 0)
    {
        sImpObjContainer = new StaticImplObjContainer();
    }
    container = sImpObjContainer;
#endif

    if (0 == container->mImpl)
    {
        std::auto_ptr<java::util::DynamicLibLoader> loader(NULL);
        PushControllerStarter& starter = PushControllerStarter::getPushControllerStarter(loader);
        //We have to leave this object to memory leak because we does not have correct place
        //to store DynamicLibLoader object. This just means that
        //pushcontroller dll is never unloaded from the runtime process.
        loader.release();
        PushRegistry* retObj = static_cast<PushControllerStarterImpl*>(&starter);
        container->mImpl = retObj;
        return *retObj;
    }
    return *container->mImpl;
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_push_PushRegistryImpl_getFilter0
(JNIEnv* aEnv, jclass, jstring aUri, jstring aSuiteUid)
{
    JELOG2(EJavaPush);
    try
    {
        Uid suiteUid(JniUtils::jstringToWstring(aEnv,aSuiteUid));
        std::wstring uriAsWStr = JniUtils::jstringToWstring(aEnv,aUri);
        PushRegistry& pushRegistry = getPushRegistryInstance();
        std::wstring filterStr = pushRegistry.getFilter(uriAsWStr,suiteUid);
        jstring filter = JniUtils::wstringToJstring(aEnv,filterStr);
        return filter;
    }
    catch (PushException& ex)
    {
        ELOG1(EJavaPush,"ERROR!!! PushException caught: %s",ex.toString().c_str());
        return NULL;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception was caught");
        return NULL;
    }
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_push_PushRegistryImpl_getMIDlet0
(JNIEnv* aEnv, jclass, jstring aUri, jstring aSuiteUid)
{
    JELOG2(EJavaPush);
    try
    {
        Uid suiteUid(JniUtils::jstringToWstring(aEnv,aSuiteUid));
        std::wstring uriAsWStr = JniUtils::jstringToWstring(aEnv,aUri);
        PushRegistry& pushRegistry = getPushRegistryInstance();
        std::wstring filterStr = pushRegistry.getClassNameOfMidlet(uriAsWStr,suiteUid);
        jstring filter = JniUtils::wstringToJstring(aEnv,filterStr);
        return filter;
    }
    catch (PushException& ex)
    {
        ELOG1(EJavaPush,"ERROR!!! PushException caught: %s",ex.toString().c_str());
        return NULL;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception was caught");
        return NULL;
    }
}


JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_push_PushRegistryImpl_isPushConnection0
(JNIEnv* aEnv, jclass, jstring aUri, jstring aAppUid)
{
    JELOG2(EJavaPush);
    try
    {
        Uid appUid(JniUtils::jstringToWstring(aEnv,aAppUid));
        std::wstring uriAsWStr = JniUtils::jstringToWstring(aEnv,aUri);
        PushRegistry& pushRegistry = getPushRegistryInstance();
        bool isPushConn = pushRegistry.isPushConnection(uriAsWStr,appUid);
        if (isPushConn)
            return JNI_TRUE;
        return JNI_FALSE;
    }
    catch (PushException& ex)
    {
        ELOG1(EJavaPush,"ERROR!!! PushException caught: %s",ex.toString().c_str());
        return JNI_FALSE;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception was caught");
        return JNI_FALSE;
    }
}

JNIEXPORT jobjectArray JNICALL Java_com_nokia_mj_impl_push_PushRegistryImpl_listConnections0
(JNIEnv* aEnv, jclass, jboolean aAvailable, jstring aAppUid)
{
    JELOG2(EJavaPush);
    try
    {
        bool available = false;
        if (JNI_TRUE == aAvailable)
            available = true;

        PushRegistry& pushRegistry = getPushRegistryInstance();
        Uid appUid(JniUtils::jstringToWstring(aEnv,aAppUid));
        std::list<std::wstring> uriList;
        pushRegistry.listConnections(available,appUid,uriList);
        if (0 == uriList.size())
            return NULL;

        jobjectArray array =
            (jobjectArray)aEnv->NewObjectArray(uriList.size(),
                                               aEnv->FindClass("java/lang/String"),
                                               aEnv->NewStringUTF(""));
        std::list<std::wstring>::iterator iter;
        int i = 0;
        for (iter = uriList.begin(); iter != uriList.end(); ++iter)
        {
            char* dynamicChr = JavaCommonUtils::wstringToUtf8((*iter));
            aEnv->SetObjectArrayElement(array,i,aEnv->NewStringUTF(dynamicChr));
            i++;
            delete [] dynamicChr;
        }//end for
        return array;
    }
    catch (PushException& ex)
    {
        ELOG1(EJavaPush,"ERROR!!! PushException caught: %s",ex.toString().c_str());
        return NULL;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception was caught");
        return NULL;
    }
}

JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_push_PushRegistryImpl_registerAlarm0
(JNIEnv* aEnv, jclass,jlong aTimeInMilliSecs, jstring aSuiteUid,
 jstring aMIDletClassName, jboolean aIsUidOfThisMidlet)
{
    JELOG2(EJavaPush);
    try
    {
        PushRegistry& pushRegistry = getPushRegistryInstance();
        Uid suiteUid(JniUtils::jstringToWstring(aEnv,aSuiteUid));
        std::wstring classNameWStr = JniUtils::jstringToWstring(aEnv,aMIDletClassName);
        bool isUidOfThisMidlet = false;
        if (JNI_TRUE == aIsUidOfThisMidlet)
            isUidOfThisMidlet = true;
        long long retValue = pushRegistry.registerAlarm(aTimeInMilliSecs,suiteUid,
                             classNameWStr,isUidOfThisMidlet);
        return retValue;
    }
    catch (PushException& ex)
    {
        ELOG1(EJavaPush,"ERROR!!! PushException caught: %s",ex.toString().c_str());
        if (INVALID_ALARM_TIME == ex.mErrCode)
        {
            JniUtils::throwNewException(aEnv,"java/lang/IllegalArgumentException",ex.toString());
        }
        //else if(INCORRECT_APP_DATA == ex.mErrCode)
        else if (std::string::npos != ex.toString().find("-11052"))
        {
            JniUtils::throwNewException(aEnv,"java/lang/ClassNotFoundException",ex.toString());
        }
        else
        {
            //We have to return 0 because IOException cannot be thrown.
            return 0;
        }
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception was caught");
    }
    return 0;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_push_PushRegistryImpl_registerConnection0
(JNIEnv* aEnv, jclass, jstring aUri, jstring aMIDletClassName, jstring aFilter,
 jstring aAppUid,jboolean aIsUidOfThisMidlet)
{
    JELOG2(EJavaPush);
    try
    {
        PushRegistry& pushRegistry = getPushRegistryInstance();
        std::wstring uriAsWStr = JniUtils::jstringToWstring(aEnv,aUri);
        std::wstring midletClassAsWStr = JniUtils::jstringToWstring(aEnv,aMIDletClassName);
        std::wstring filterAsWStr = JniUtils::jstringToWstring(aEnv,aFilter);
        Uid appUid(JniUtils::jstringToWstring(aEnv,aAppUid));
        bool isUidOfThisMidlet = false;
        if (JNI_TRUE == aIsUidOfThisMidlet)
            isUidOfThisMidlet = true;
        pushRegistry.registerDynamicPushConn(uriAsWStr,midletClassAsWStr,filterAsWStr,appUid,
                                             isUidOfThisMidlet);
    }
    catch (PushException& ex)
    {
        ELOG1(EJavaPush,"ERROR!!! registerConnection0(): PushException: %s",ex.toString().c_str());
        if (INVALID_URI == ex.mErrCode)
        {
            JniUtils::throwNewException(aEnv,"java/lang/IllegalArgumentException",ex.toString());
        }
        //else if(PUSH_CONNECTION_ALREADY_EXISTS == ex.mErrCode){
        //  JniUtils::throwNewException(aEnv,"java/io/IOException",ex.toString());
        //}
        else
        {
            JniUtils::throwNewException(aEnv,"java/io/IOException",ex.toString());
        }
    }
    catch (java::util::LibraryLoaderException& ex)
    {
        if (java::util::OPENING_LIBRARY_FAILED == ex.mErrCode)
        {
            JniUtils::throwNewException
            (aEnv,"javax/microedition/io/ConnectionNotFoundException",ex.toString());
        }
        else
        {
            JniUtils::throwNewException(aEnv,"java/io/IOException",ex.toString());
        }
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception was caught");
        JniUtils::throwNewException(aEnv,"java/io/IOException","Unexpected error occurred");
    }
}
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_push_PushRegistryImpl_unregisterConnection0
(JNIEnv* aEnv, jclass, jstring aUri, jstring aAppUid,jboolean aIsUidOfThisMidlet)
{
    JELOG2(EJavaPush);
    try
    {
        PushRegistry& pushRegistry = getPushRegistryInstance();
        std::wstring uriAsWStr = JniUtils::jstringToWstring(aEnv,aUri);
        Uid appUid(JniUtils::jstringToWstring(aEnv,aAppUid));
        bool isUidOfThisMidlet = false;
        if (JNI_TRUE == aIsUidOfThisMidlet)
            isUidOfThisMidlet = true;
        pushRegistry.unregisterDynamicPushConn(uriAsWStr,appUid,isUidOfThisMidlet);
        return true;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception was caught in the unregisterConnection0 operation");
        return false;
    }
}
