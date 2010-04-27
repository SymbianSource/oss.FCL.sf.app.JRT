/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "javajniutils.h"
#include "com_nokia_mj_impl_security_midp_common_GeneralSecuritySettings.h"

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_security_midp_common_GeneralSecuritySettings__1getSecurityPolicy
(JNIEnv *, jclass)
{
    return NULL;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_midp_common_GeneralSecuritySettings__1getDefaultSecurityWarningsMode
(JNIEnv *, jclass)
{
    return -1;
}
