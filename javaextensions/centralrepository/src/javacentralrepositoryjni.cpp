/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <centralrepository.h>      // KMaxUnicodeStringLength

#include "com_nokia_mj_impl_cenrep_CentralRepositoryImpl.h"
#include "cjavacentralrepository.h"
#include "centrepfunctionserver.h"
#include "javajniutils.h"
#include "jstringutils.h"
#include "s60commonutils.h"
#include "logger.h"
#include "fs_methodcall.h"

using namespace java::util;
/*
 * Class:     com_nokia_mj_impl_cenrep_CentralRepositoryImpl
 * Method:    _createFunctionSource
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_cenrep_CentralRepositoryImpl__1createFunctionSource
(JNIEnv *aJni, jobject /*aPeer*/)
{
    JELOG2(EJavaCentrep);
    java::centrep::CentrepFunctionServer* mFunctionServer;
    mFunctionServer = new java::centrep::CentrepFunctionServer();
    TInt handle = reinterpret_cast<jint>(mFunctionServer);
    if (handle < KErrNone)
    {
        JniUtils::throwNewException(aJni, "com/nokia/mid/cenrep/CentralRepositoryException" ,
                                    JavaCommonUtils::intToString(handle));
    }
    return handle;
}

/**
 * CreateCentralRepositoryL
 */
void CreateCentralRepositoryL(CJavaCentralRepository*& aCenRep,
                              TUid aRepositoryId)
{
    CJavaCentralRepository* cenrep =
        CJavaCentralRepository::NewL(aRepositoryId);
    aCenRep = cenrep;
}

/*
 * Class:     com_nokia_mj_impl_cenrep_CentralRepositoryImpl
 * Method:    _createNativePeer
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_cenrep_CentralRepositoryImpl__1createNativePeer
(JNIEnv *aJni, jobject, jint aFunctionSourceHandle, jint aRepositoryId)
{
    java::centrep::CentrepFunctionServer* mFunctionServer =
        reinterpret_cast< java::centrep::CentrepFunctionServer*>(aFunctionSourceHandle);

    CJavaCentralRepository* cenrep = NULL;

    TUid uid = TUid::Uid(aRepositoryId);
    TRAPD(err,CallMethodL(CreateCentralRepositoryL, cenrep, uid, mFunctionServer));

    if (err != KErrNone)
    {
        JniUtils::throwNewException(aJni, "com/nokia/mid/cenrep/CentralRepositoryException" ,
                                    JavaCommonUtils::intToString(err));
    }
    return reinterpret_cast<jint>(cenrep);
}

/**
 * Close
 */
void CloseCentralRepository(CJavaCentralRepository* aCenRep)
{
    aCenRep->Close();
}

/*
 * Class:     com_nokia_mj_impl_cenrep_CentralRepositoryImpl
 * Method:    _close
 * Signature: (II)I
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_cenrep_CentralRepositoryImpl__1close
(JNIEnv *, jobject, jint aFunctionSourceHandle, jint aCenrepHandle)
{
    java::centrep::CentrepFunctionServer* mFunctionServer =
        reinterpret_cast< java::centrep::CentrepFunctionServer*>(
            aFunctionSourceHandle);

    CJavaCentralRepository* cenrep =
        reinterpret_cast<CJavaCentralRepository *>(aCenrepHandle);

    CallMethod(CloseCentralRepository, cenrep, mFunctionServer);
}

/**
 * Dispose
 */
void Dispose(CJavaCentralRepository* aCenRep)
{
    delete aCenRep;
    aCenRep = NULL;
}

/*
 * Class:     com_nokia_mj_impl_cenrep_CentralRepositoryImpl
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_cenrep_CentralRepositoryImpl__1dispose
(JNIEnv *, jobject, jint aFunctionSourceHandle, jint aCenrepHandle)
{
    java::centrep::CentrepFunctionServer* mFunctionServer =
        reinterpret_cast< java::centrep::CentrepFunctionServer*>(
            aFunctionSourceHandle);

    CJavaCentralRepository* cenrep =
        reinterpret_cast<CJavaCentralRepository *>(aCenrepHandle);

    CallMethod(Dispose, cenrep, mFunctionServer);
    delete mFunctionServer;
}

/*
 * Class:     com_nokia_mj_impl_cenrep_CentralRepositoryImpl
 * Method:    _getString
 * Signature: (IIJLcom/nokia/mj/impl/cenrep/CentralRepositoryImpl$StringValue;)I
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_cenrep_CentralRepositoryImpl__1getString
(JNIEnv * aJni, jobject, jint aFunctionSourceHandle, jint aCenrepHandle,
 jlong aKey)
{
    java::centrep::CentrepFunctionServer* mFunctionServer =
        reinterpret_cast< java::centrep::CentrepFunctionServer*>(
            aFunctionSourceHandle);

    CJavaCentralRepository* cenrep =
        reinterpret_cast<CJavaCentralRepository *>(aCenrepHandle);

    HBufC* buf = NULL;

    TUint32 key = (TUint32)aKey;
    TRAPD(err, CallMethodL(buf, cenrep, &CJavaCentralRepository::GetStringL, key, mFunctionServer));

    jstring val = NULL;
    if (err == KErrNone && buf)
    {
        val = java::util::S60CommonUtils::NativeToJavaString(*aJni, *buf);
    }
    else
    {
        JniUtils::throwNewException(aJni, "com/nokia/mid/cenrep/CentralRepositoryException" ,
                                    JavaCommonUtils::intToString(err));
    }

    delete buf;
    return val;
}


/*
 * Class:     com_nokia_mj_impl_cenrep_CentralRepositoryImpl
 * Method:    _getInt
 * Signature: (IIJLcom/nokia/mj/impl/cenrep/CentralRepositoryImpl$IntValue;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_cenrep_CentralRepositoryImpl__1getInt
(JNIEnv * aJni, jobject, jint aFunctionSourceHandle, jint aCenrepHandle,
 jlong aKey)
{
    java::centrep::CentrepFunctionServer* mFunctionServer =
        reinterpret_cast< java::centrep::CentrepFunctionServer*>(
            aFunctionSourceHandle);

    CJavaCentralRepository* cenrep =
        reinterpret_cast<CJavaCentralRepository *>(aCenrepHandle);

    TInt val = 0;
    TUint32 key = (TUint32)aKey;

    TRAPD(err, CallMethodL(val, cenrep, &CJavaCentralRepository::GetIntL, key, mFunctionServer));

    if (err != KErrNone)
    {
        JniUtils::throwNewException(aJni, "com/nokia/mid/cenrep/CentralRepositoryException" ,
                                    JavaCommonUtils::intToString(err));
    }
    return val;
}

/**
 * SetStringL
 */
void SetStringL(CJavaCentralRepository* aCenRep, TUint32 aKey, TDesC* aValue)
{
    aCenRep->SetL(aKey, *aValue);
}

/*
 * Class:     com_nokia_mj_impl_cenrep_CentralRepositoryImpl
 * Method:    _setString
 * Signature: (IIJLjava/lang/String;)I
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_cenrep_CentralRepositoryImpl__1setString
(JNIEnv *aJni, jobject, jint aFunctionSourceHandle, jint aCenrepHandle,
 jlong aKey, jstring aValue)
{
    java::centrep::CentrepFunctionServer* mFunctionServer =
        reinterpret_cast< java::centrep::CentrepFunctionServer*>(
            aFunctionSourceHandle);

    CJavaCentralRepository* cenrep =
        reinterpret_cast<CJavaCentralRepository *>(aCenrepHandle);

    JStringUtils string(*aJni, aValue);
    TUint32 key = (TUint32)aKey;

    TDesC* tstring = (TDesC*) &string;
    TRAPD(err, CallMethodL(SetStringL, cenrep, key, tstring, mFunctionServer));

    if (err != KErrNone)
    {
        JniUtils::throwNewException(aJni, "com/nokia/mid/cenrep/CentralRepositoryException" ,
                                    JavaCommonUtils::intToString(err));
    }
}

/**
 * SetIntL
 */
void SetIntL(CJavaCentralRepository* aCenRep, TUint32 aKey, TInt aValue)
{
    aCenRep->SetL(aKey, aValue);
}

/*
 * Class:     com_nokia_mj_impl_cenrep_CentralRepositoryImpl
 * Method:    _setInt
 * Signature: (IIJI)I
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_cenrep_CentralRepositoryImpl__1setInt
(JNIEnv *aJni, jobject, jint aFunctionSourceHandle, jint aCenrepHandle,
 jlong aKey, jint aValue)
{
    java::centrep::CentrepFunctionServer* mFunctionServer =
        reinterpret_cast< java::centrep::CentrepFunctionServer*>(
            aFunctionSourceHandle);

    CJavaCentralRepository* cenrep =
        reinterpret_cast<CJavaCentralRepository *>(aCenrepHandle);

    TUint32 key = (TUint32)aKey;
    TInt value = (TInt)aValue;

    TRAPD(err, CallMethodL(SetIntL, cenrep, key, value, mFunctionServer));
    if (err != KErrNone)
    {
        JniUtils::throwNewException(aJni, "com/nokia/mid/cenrep/CentralRepositoryException" ,
                                    JavaCommonUtils::intToString(err));
    }
}


