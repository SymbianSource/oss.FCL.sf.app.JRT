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

#ifndef JSTRINGUTILS_H
#define JSTRINGUTILS_H

#include <e32base.h>
#include <jni.h>

#include <badesca.h>
#include "javaosheaders.h"


class JStringUtils : public TPtrC16
{
public:
    OS_IMPORT JStringUtils(JNIEnv& aJni, jstring aString);
    OS_IMPORT  ~JStringUtils();

private:
    // Prevent accidental copying because of the shared underlying Java
    // string
    OS_IMPORT JStringUtils(const JStringUtils&);
    OS_IMPORT JStringUtils& operator=(const JStringUtils&);

private:
    JNIEnv& iJni;
    jstring iString;
};

#endif // JSTRINGUTILS_H
