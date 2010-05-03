/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <jni.h>

#include "logger.h"
#include "commonproperties.h"
#include "javajniutils.h"

using namespace java::util;

jstring java::util::getLocaleImpl(JNIEnv* aEnv)
{
    JELOG2(EUtils);
    jstring loc = 0;
    // Impl missing...
    JniUtils::throwNewException
    (aEnv,"java/lang/RuntimeException",
     "microedition.locale system property not supported in linux");
    return loc;
}

jstring java::util::getPlatformImpl(JNIEnv* aEnv)
{
    JELOG2(EUtils);
    jstring platformStr = 0;
    // Impl missing...
    JniUtils::throwNewException
    (aEnv,"java/lang/RuntimeException",
     "microedition.platform system property not supported in linux");
    return platformStr;
}

jint java::util::getFreeMemoryImpl(JNIEnv* aEnv)
{
    JELOG2(EUtils);
    // Impl missing...
    JniUtils::throwNewException
    (aEnv,"java/lang/RuntimeException",
     "com.nokia.memoryramfree system property not supported in linux");
    return -1;
}

