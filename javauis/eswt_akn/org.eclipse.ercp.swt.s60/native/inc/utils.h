/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef UTILS_H
#define UTILS_H


#include "eswtmobileextensions.h"
#include "swtdisplay.h"


class CSwtListBoxItem;
class MSwtImage;
class MSwtImageData;
class TRgb;


/**
 * Deletes an object that was allocated by the UI native thread
 */
template<class C>
inline void DeleteInUiHeap(C* aPtr)
{
    delete aPtr;
}



// Error handling
void Throw(TInt aError, JNIEnv* aJniEnv);
void ThrowIfError(TInt aError, JNIEnv* aJniEnv);

// Releasing a Java peer
void ReleasePeer(JNIEnv* aJniEnv, TSwtPeer aPeer);

// Method calling
jmethodID GetJavaMethodId(JNIEnv* aJniEnv, jobject aObject, const char* aMethodName, const char* aMethodSignature);
void      CallVoidJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, jobject aObject, const char* aMethodName, const char* aMethodSignature, ...);
void      CallStaticVoidJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, const char* aClazz, const char* aMethodName, const char* aMethodSignature, ...);
jboolean  CallBooleanJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, jobject aObject, const char* aMethodName, const char* aMethodSignature, ...);
jobject   CallObjectJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, jobject aObject, const char* aMethodName, const char* aMethodSignature, ...);
jobject   CallStaticObjectJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, const char* aClazz, const char* aMethodName, const char* aMethodSignature, ...);

// Field retrieval
jint     GetIntField(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, TBool& aFailed);
jboolean GetBooleanField(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, TBool& aFailed);
jobject  GetObjectField(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, const char* aSignature, TBool& aFailed);
HBufC*   GetStringFieldL(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, TBool& aFailed);
HBufC8*  GetByteArrayFieldL(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, TBool& aFailed);

// Type conversion
jboolean              ConvertBoolean(TBool aBool);
HBufC*                ConvertStringL(JNIEnv* aJniEnv, const jstring aString);
HBufC*                ConvertStringLC(JNIEnv* aJniEnv, const jstring aString);
HBufC16*              ConvertString16LC(JNIEnv* aJniEnv, const jstring aString);
TInt                  JavaIntsToPoints(JNIEnv* aJniEnv, jintArray aIntArray, TPoint*& aPoints);
TInt                  JavaIntsToInts(JNIEnv* aJniEnv, jintArray aIntArray, TInt*& aInts);
TInt*                 JavaIntRegionToInts(JNIEnv* aJniEnv, jintArray aIntArray, const jsize& aStart, const jsize& aCount);
void                  ConvertIntegerArrayL(JNIEnv* aJniEnv, jintArray aIntArray, RArray<TInt>& aResultArray);
HBufC8*               ConvertByteArrayL(JNIEnv* aJniEnv, jbyteArray aByteArray);
CDesCArray*           ConvertStringArrayL(JNIEnv* aJniEnv, jobjectArray aStringArray);
CSwtListBoxItem*      ConvertListBoxItemL(JNIEnv* aJniEnv, const jobject aJavaItem);
RSwtListBoxItemsArray ConvertListBoxItemArrayL(JNIEnv* aJniEnv, const jobjectArray aDataModelArray, TBool& aFailed);

// Creating Java objects
jbyteArray   NewJavaByteArray(JNIEnv* aJniEnv, const TDesC8& aArray);
jbyteArray   NewJavaByteArray(JNIEnv* aJniEnv, const TDesC8* aArray, TBool& aFailed);
jintArray    NewJavaIntArray(JNIEnv* aJniEnv, const TInt* aArray, TInt aCount);
jobject      NewJavaPoint(JNIEnv* aJniEnv, const TPoint& aPoint);
jobject      NewJavaRectangle(JNIEnv* aJniEnv, const TRect& aRect);
jobject      NewJavaRgb(JNIEnv* aJniEnv, const TRgb& aRgb);
jstring      NewJavaString(JNIEnv* aJniEnv, const TDesC16& aString);
jobject      NewJavaImageData(JNIEnv* aJniEnv, const MSwtImageData& aData);
jobjectArray NewJavaImageDataArray(JNIEnv* aJniEnv, const CSwtImageDataArray* aArray);
jobjectArray NewJavaControlArray(JNIEnv* aJniEnv, const CSwtPeerArray* aArray);
jobjectArray NewJavaObjectArray(JNIEnv* aJniEnv, const CSwtPeerArray* aArray, const char* aClassname);

TBool IsDefaultDisplayDisposed(TBool& aFailed, JNIEnv* aJniEnv);




/**
 * Converts a Symbian Leave error code into a Java Exception
 *
 * @param aError  Can be any Symbian error code or SWT error code; if it is 0
 *                nothing is thrown.
 * @param aJniEnv The JNI environment
 */
inline void ThrowIfError(TInt aError, JNIEnv* aJniEnv)
{
    if (aError)
        Throw(aError, aJniEnv);
}


/**
 * Converts a Symbian boolean to a Java boolean
 *
 * Java booleans are 8-bit values whereas Symbian ones are 32-bit values,
 * truncation may occur which could lead to unexpected results. This function
 * prevents truncation.
 */
inline jboolean ConvertBoolean(TBool aBool)
{
    return (aBool) ? static_cast<jboolean>(JNI_TRUE) : static_cast<jboolean>(JNI_FALSE);
}


/**
 * Converts a Java String object to a Symbian descriptor.
 *
 * Same as ConvertStringL() except the return value is placed onto
 * the cleanup stack.
 *
 * @sa ConvertStringL()
 * @sa ConvertString16LC()
 */
inline HBufC* ConvertStringLC(JNIEnv* aJniEnv, const jstring aString)
{
    return ConvertString16LC(aJniEnv, aString);
}


/**
 * Converts a Java String object to a Symbian descriptor.
 *
 * The returned TPtr points to a newly allocated buffer which
 * must be freed using FreeString() when not needed any longer.
 *
 * @sa ConvertStringLC()
 * @sa ConvertString16L()
 */
inline HBufC* ConvertStringL(JNIEnv* aJniEnv, const jstring aString)
{
    HBufC* result = ConvertStringLC(aJniEnv, aString);
    if (result != NULL)
        CleanupStack::Pop(result);
    return result;
}


/**
 * Creates a Java String object
 */
inline jstring NewJavaString(JNIEnv* aJniEnv, const TDesC16& aString)
{
    return aJniEnv->NewString(aString.Ptr(), aString.Length());
}


/**
 * Converts a device handle to a display pointer
 */
inline CSwtDisplay* DisplayFromDevice(jint aDeviceHandle)
{
    // Must pass the device handle.
    // Using display from TLS won't work unless we are in the UI thread.
    // E.g. GC can be used from a non-UI thread.
    ASSERT(aDeviceHandle != 0);

    return static_cast<CSwtDisplay*>(reinterpret_cast<MSwtDevice*>(aDeviceHandle));
}


// Signature generating macros
//#define OBJECT_SIGNATURE( aArgList, aReturnType) "(" aArgList ")" aReturnType


#endif // UTILS_H
