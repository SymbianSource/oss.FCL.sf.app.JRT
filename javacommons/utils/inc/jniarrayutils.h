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

#ifndef JNIARRAYUTILS_H
#define JNIARRAYUTILS_H

#include <jni.h>
#include "javaosheaders.h"


class JNIArrayUtils
{
public:
    OS_IMPORT static int CopyToNative(JNIEnv& aJni, jbyteArray aJavaBuffer,
                                      int aOffset, int aLength, char* aNativeBuffer);


    OS_IMPORT static int CopyToJava(JNIEnv& aJni, const char * aNativeBuffer,
                                    int aNativeBufferLength, jbyteArray aJavaBuffer, int aOffset,
                                    int aLength);

    OS_IMPORT static jstring WStringToJavaString(JNIEnv& aJni, const wchar_t* aString,
            const int aLength);


};


#endif // JNIARRAYUTILS_H
