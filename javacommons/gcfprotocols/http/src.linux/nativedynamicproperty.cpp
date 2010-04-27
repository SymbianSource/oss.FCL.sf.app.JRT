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


#include "nativedynamicproperty.h"
#include <stdlib.h>
#include <stdio.h>

using namespace java::http;

jstring NativeDynamicProperty::getProxySettings(JNIEnv& aJni)
{
    // proxy address hard coded for now as its retrieval not possible
    return aJni.NewStringUTF("gxwebproxy01.apac.nokia.com:8080");
}
