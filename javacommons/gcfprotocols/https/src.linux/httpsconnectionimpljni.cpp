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
* Description:  ?Description
*
*/

#include "com_nokia_mj_impl_https_HttpsConnectionImpl.h"
#include "stdlib.h"
#include "nativehttpsconnection.h"
#include "javajniutils.h"

#include "wchar.h"

using namespace java::https;

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_https_HttpsConnectionImpl__1createNativePeer
(JNIEnv* aJni, jobject /*peer*/, jstring aProxy, jint aProxyPort, jstring aHost, jint aPort)
{
    const char* proxy = aJni->GetStringUTFChars(aProxy, 0);
    const char* host = aJni->GetStringUTFChars(aHost, 0);
    NativeHttpsConnection *httpsPeer = new NativeHttpsConnection();
    int sock_desc = httpsPeer->createSocket(proxy,aProxyPort,host,aPort);
    return sock_desc;
}
