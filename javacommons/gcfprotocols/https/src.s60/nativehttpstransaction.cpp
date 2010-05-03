/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h>
#include "nativehttptransaction.h"
#include "com_nokia_mj_impl_https_HttpsConnectionNative.h"

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_https_HttpsConnectionNative__1getMSecureConnectionInfo(
    JNIEnv*,
    jobject,
    jint aNativeHttpTransaction)
{
    jint handle = -1;
    NativeHttpTransaction* tran = reinterpret_cast<NativeHttpTransaction*>(aNativeHttpTransaction);
    handle = tran->GetSecurityInfo();
    return handle;
}
