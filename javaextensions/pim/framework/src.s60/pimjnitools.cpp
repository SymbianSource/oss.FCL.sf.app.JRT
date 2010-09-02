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
* Description:  Tools for PIM JNI wrappers.
 *
*/


//  INCLUDE FILES
#include "pimjnitools.h"
#include <e32base.h>
#include "cpimitem.h"
#include "pimpanics.h"
#include "s60commonutils.h"
#include "jstringutils.h"
#include "logger.h"


jintArray ConvertToJavaIntArray(JNIEnv& aJniEnv,
                                const CArrayFix<TInt>& aSourceArray)
{
    JELOG2(EPim);
    const TInt n = aSourceArray.Count();
    jintArray targetArray = aJniEnv.NewIntArray(n);

    if (!targetArray)
    {
        return NULL; // Error
    }

    if (n > 0)
    {
        aJniEnv.SetIntArrayRegion(targetArray, 0, n,
                                  const_cast<int*>(aSourceArray.Back(0)));
    }

    return targetArray;
}

jintArray GetJavaItemHandles(JNIEnv& aJniEnv,
                             const RPointerArray<CPIMItem>& aItems)
{
    JELOG2(EPim);
    const TInt numItems = aItems.Count();
    jintArray itemHandles = aJniEnv.NewIntArray(numItems);

    if (!itemHandles)
    {
        return NULL; // Error
    }

    if (numItems > 0)
    {
        jint* elems = aJniEnv.GetIntArrayElements(itemHandles, NULL);

        if (elems == NULL)
        {
            return NULL; // Error
        }

        for (TInt i = 0; i < numItems; i++)
        {
            pimbaseitem* item = aItems[i];
            elems[i] = reinterpret_cast<int>(item);
        }

        aJniEnv.ReleaseIntArrayElements(itemHandles, elems, 0);
    }

    return itemHandles;
}

jintArray NativeDatesToJavaInts(JNIEnv& aJniEnv,
                                const CArrayFix<TPIMDate>& aDates)
{
    JELOG2(EPim);
    const TInt n = aDates.Count();

    // Create an int array of double size to fit the dates in half
    // size elements.
    jintArray retArr = aJniEnv.NewIntArray(n * 2);

    if (!retArr)
    {
        return NULL; // Error
    }

    if (n == 0)
    {
        return retArr; // Nothing to do, return empty (non-NULL) array
    }

    jint* elems = aJniEnv.GetIntArrayElements(retArr, NULL);

    if (elems == NULL)
    {
        return NULL; // Error
    }

    for (TInt i = 0; i < n; i++)
    {
        jlong javaDate = java::util::S60CommonUtils::TTimeToJavaTime(aDates[i]);
        TInt64 int64Val = *reinterpret_cast<TInt64*>(&javaDate);
        elems[2 * i] = I64HIGH(int64Val); // high part to even element
        elems[(2 * i) + 1] = I64LOW(int64Val); // low part to odd element
    }

    aJniEnv.ReleaseIntArrayElements(retArr, elems, 0);
    return retArr;
}

CDesCArray* CreateNativeStringArrayL(JNIEnv* aJniEnv, jobjectArray aJavaArray)
{
    JELOG2(EPim);
    const TInt numElems = aJniEnv->GetArrayLength(aJavaArray);

    CDesCArray* nativeArray = new(ELeave) CDesCArrayFlat(numElems + 1);  // ensure granularity of at least 1 (panics if 0)

    CleanupStack::PushL(nativeArray);

    for (TInt elemIndex = 0; elemIndex < numElems; elemIndex++)
    {
        // Creates a new local reference
        jobject javaElemObject = aJniEnv->GetObjectArrayElement(aJavaArray,
                                 elemIndex);

        jstring javaElem = static_cast<jstring>(javaElemObject);

        if (!javaElem)
        {
            nativeArray->AppendL(KPIMNullArrayElement);
        }
        else
        {
            // The value returned by GetObjectArrayElement is not really
            // a pointer to the object, but it is a "local reference".
            // In practice, it is a pointer to a pointer which points to the
            // object. If the object is null, we get a pointer which points
            // to a pointer which points to null.
            void** objPointer = reinterpret_cast<void**>(javaElemObject);
            void* obj = *objPointer;
            if (!obj)
            {
                nativeArray->AppendL(KPIMNullArrayElement);
            }
            else
            {
                JStringUtils nativeElem(*aJniEnv, javaElem);
                nativeArray->AppendL(nativeElem);
            }
        }
        // The VM runs out of temporary local references, unless the
        // elements are released here
        aJniEnv->DeleteLocalRef(javaElemObject);
    }

    CleanupStack::Pop(nativeArray);
    return nativeArray;
}

jobjectArray CreateJavaStringArray(JNIEnv* aJniEnv,
                                   const CDesCArray& aNativeArray, TBool aConvertPIMNullArrayElements)
{
    JELOG2(EPim);
    const TInt numElems = aNativeArray.Count();


    // Initializes the array with NULLs
    jobjectArray javaStringArray = aJniEnv->NewObjectArray(numElems,
                                   aJniEnv->FindClass("java/lang/String"), NULL);


    if (!javaStringArray)
    {
        return NULL;
    }

    for (TInt i = 0; i < numElems; i++)
    {
        TPtrC elem = aNativeArray[i];

        if (aConvertPIMNullArrayElements && elem == KPIMNullArrayElement)
        {
            // The element is marked "null", skip and leave the element NULL
            continue;
        }

        // Creates a new local reference
        jstring javaElem = java::util::S60CommonUtils::NativeToJavaString(
                               *aJniEnv, aNativeArray[i]);

        if (!javaElem)
        {
            // javaStringArray is left for Garbage Collector
            return NULL;
        }

        aJniEnv->SetObjectArrayElement(javaStringArray, i, javaElem);

        // Avoid running out of local references
        aJniEnv->DeleteLocalRef(javaElem);
    }
    return javaStringArray;
}

// End of File
