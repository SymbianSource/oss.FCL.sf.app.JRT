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
* Description:  Connection Manager
*                Provides Native to java interface for conection
*
*/

#include "com_nokia_mj_impl_connectionmanager_ConnectionManager.h"
#include "connectionmanager.h"
#include "javajniutils.h"
#include "logger.h"

using namespace java::util;



//JNIEXPORT jboolean JNICALL
//Java_com_nokia_mj_impl_connectionmanager_ConnectionManager__1selectNetworkAccessPoint
//    (JNIEnv *aJni, jobject /*peer*/, jobjectArray aApn)
//{
//  LOG(ESOCKET,EInfo,"+ConnectionManagerjni");
//  jstring jnistring;
//  char * apnInfo = new char[256];
//  int  error = 0;
//  //jboolean isDeviceDef = ConnectionManager::SelectNetworkAccessPoint(apnInfo, &error);
//  if(error == KErrNone)
//  {
//          LOG1(ESOCKET,EInfo,"+apnInfo =  %s",apnInfo);
//          jnistring = aJni->NewStringUTF(apnInfo);
//
//          aJni->SetObjectArrayElement(aApn,0,jnistring);
//          aJni->DeleteLocalRef(jnistring);
//          delete[] apnInfo;
//  }
//  else
//  {
//          aJni->SetObjectArrayElement(aApn,0,NULL);
//          delete[] apnInfo;
//  }
//  return isDeviceDef;
//}

JNIEXPORT jstring JNICALL
Java_com_nokia_mj_impl_connectionmanager_ConnectionManager__1askNetworkAccessPoint
(JNIEnv *aJni, jobject /*peer*/)
{
    char * apnInfo = new char[256];
    TRAPD(err,ConnectionManager::PromptUserL(apnInfo));
    if (err == KErrNone)
    {
        LOG1(ESOCKET,EInfo,"+apnInfo =  %s",apnInfo);
        jstring jnistring = aJni->NewStringUTF(apnInfo);
        return jnistring;
    }
    else
    {
        return NULL;
    }

}

JNIEXPORT jstring JNICALL
Java_com_nokia_mj_impl_connectionmanager_ConnectionManager__1getDeviceDefaultAccessPoint
(JNIEnv *aJni, jobject /*peer*/)
{
    char * apnInfo = new char[256];
    LOG(ESOCKET,EInfo,"+getDeviceDefaultAccessPoint");
    TRAPD(err,ConnectionManager::GetDeviceDefaultAccessPointL(apnInfo));
    if (err == KErrNone)
    {
        LOG1(ESOCKET,EInfo,"+apnInfo =  %s",apnInfo);
        jstring jnistring = aJni->NewStringUTF(apnInfo);
        return jnistring;
    }
    else
    {
        return NULL;
    }
}
