/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include "jutils.h"


void AddToJavaStringArrayL(JNIEnv& aJni, jobjectArray& aContainer, TInt aPosition, const TDesC& aString)
{
    jstring javaString = CreateJavaString(aJni, aString);
    User::LeaveIfNull(javaString);
    //
    aJni.SetObjectArrayElement(aContainer, aPosition, javaString);
    aJni.DeleteLocalRef(javaString);
}


EXPORT_C jobjectArray CopyToNewJavaStringArrayL(JNIEnv& aJni, const CDesCArray& aNativeArray)
{
    jclass stringClass = aJni.FindClass("java/lang/String");
    User::LeaveIfNull(stringClass);
    //
    jobjectArray result = aJni.NewObjectArray(aNativeArray.Count(), stringClass, NULL);
    if (result != NULL)
    {
        TPtrC item;
        for (int i=0; i<aNativeArray.Count(); ++i)
        {
            item.Set(aNativeArray[i]);
            AddToJavaStringArrayL(aJni, result, i, item);
        }
    }
    return result;
}
