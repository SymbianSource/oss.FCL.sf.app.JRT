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

#ifndef NATIVEDYNAMICPROPERTY_H
#define NATIVEDYNAMICPROPERTY_H

//#include <wchar.h>
//#include "javaosheaders.h"
#include "logger.h"

#include <jni.h>

namespace java
{
namespace http
{
class NativeDynamicProperty
{
public :

    jstring getProxySettings(JNIEnv& aJNi);

    char* iProxyServerAddress;

};
} // end namespace http
}  // end namespace java
#endif // NATIVEDYNAMICPROPERTY_H
