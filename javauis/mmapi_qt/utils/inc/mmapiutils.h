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
#ifndef MMAPIUTILS_H
#define MMAPIUTILS_H
#include "jni.h"
#include "e32base.h"
#include "BADESCA.H"

class MMAPIUtils
{
public:
    
    static TInt CopyToJava(JNIEnv& aJni, const TDesC8& aNativeBuffer,
                                    jbyteArray aJavaBuffer, TInt aOffset, TInt aLength);  
    static jstring CreateJavaString(JNIEnv& aJni, const TDesC16& aString);

    static jobjectArray CopyToNewJavaStringArrayL(JNIEnv& aJni, const CDesCArray& aNativeArray);
    
    static void AddToJavaStringArrayL(JNIEnv& aJni, jobjectArray& aContainer, TInt aPosition, const TDesC& aString);
};

#endif // MMAPIUTILS_H
