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

#include "com_nokia_mj_impl_properties_http_DynamicPropertyHandler.h"
#include "stdlib.h"
#include "nativedynamicproperty.h"
#include "javajniutils.h"
#include "wchar.h"

using namespace java::http;

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_properties_http_DynamicPropertyHandler__1getProxyInfo
(JNIEnv* aJni, jobject /*peer*/)
{

    NativeDynamicProperty *peer = new NativeDynamicProperty();
    jstring ret = peer->getProxySettings(*aJni);
    delete peer;
    return ret;
}
