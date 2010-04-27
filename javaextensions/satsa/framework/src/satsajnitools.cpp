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
* Description:
 *
*/


//  INCLUDE FILES
#include "satsajnitools.h"
#include <e32base.h>

void STSSetJavaErrorCode(JNIEnv* aJniEnv, jintArray aErrorTo,
                         const TInt& aErrorFrom)
{
    jint javaError[1] =
        { aErrorFrom };
    aJniEnv->SetIntArrayRegion(aErrorTo, 0, 1, javaError);
}

CDesCArrayFlat* STSCreateNativeStringArrayL(JNIEnv* aJniEnv,
        jobjectArray aJavaArray)
{
    const TInt numElems = aJniEnv->GetArrayLength(aJavaArray);

    CDesCArrayFlat* nativeArray = new(ELeave) CDesCArrayFlat(numElems + 1);  // ensure granularity of at least 1 (panics if 0)

    CleanupStack::PushL(nativeArray);

    for (TInt elemIndex = 0; elemIndex < numElems; elemIndex++)
    {
        // Creates a new local reference
        jobject javaElemObject = aJniEnv->GetObjectArrayElement(aJavaArray,
                                 elemIndex);

        jstring javaElem = static_cast<jstring>(javaElemObject);

        // Guarded with blocks to delete RJString before deleting local
        // reference
        {
            RJString nativeElem(*aJniEnv, javaElem);
            nativeArray->AppendL(nativeElem);
        }
        // The VM runs out of temporary local references, unless the
        // elements are released here
        aJniEnv->DeleteLocalRef(javaElemObject);
    }

    CleanupStack::Pop(nativeArray);
    return nativeArray;
}
