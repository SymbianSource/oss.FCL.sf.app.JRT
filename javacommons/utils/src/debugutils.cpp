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


#include "logger.h"
#include "javacommonutils.h"
#include "threaddumper.h"


#include "com_nokia_mj_impl_utils_DebugUtils.h"

using namespace java::util;


JNIEXPORT void JNICALL Java_com_nokia_mj_impl_utils_DebugUtils__1doThreadDump
(JNIEnv* /*aEnv*/, jclass)
{
    ThreadDump::doDump();
}


JNIEXPORT void JNICALL Java_com_nokia_mj_impl_utils_DebugUtils__1closeThreadDumper
(JNIEnv* /*aEnv*/, jclass)
{
    ThreadDump::closeDumper();
}

