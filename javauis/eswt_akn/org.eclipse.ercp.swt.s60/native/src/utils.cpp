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


#include <openfont.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <graphics/openfontconstants.h>
#endif

#include "mmf/common/mmfcontrollerpluginresolver.h"
#include "utils.h"
#include "eswtgraphics.h"
#include "eswtmobileextensions.h"
#include "javalocalref.h"


const TText16 KHighSurrogateFirst = 0xD800; ///< Min value of the high order word of a surrogate pair
const TText16 KHighSurrogateLast  = 0xDBFF; ///< Max value of the high order word of a surrogate pair
const TText16 KLowSurrogateLast   = 0xDFFF; ///< Max value of the low  order word of a surrogate pair

// Default granularity for listbox item arrays
static const TInt KListBoxItemArrGranularity = 4;


// ======== LOCAL FUNCTIONS ========


/**
 * Tests if a character is the high order word of a surrogate pair
 */
static inline TBool IsHighSurrogate(TText16 aChar)
{
    return (aChar>=KHighSurrogateFirst && aChar<=KHighSurrogateLast);
}


/**
 * Tests if a character is part of a surrogate pair.
 */
static inline TBool IsSurrogate(TText16 aChar)
{
    return (aChar>=KHighSurrogateFirst && aChar<=KLowSurrogateLast);
}





/**
 * Converts a Symbian error code into a Java Exception
 *
 * @param aError  Can be any Symbian error code or SWT error code, but it cannot be 0.
 * @param aJniEnv The JNI environment.
 */
void Throw(TInt aError, JNIEnv* aJniEnv)
{
    ASSERT(aError);

    // Convert Symbian error codes to SWT error codes when possible
    if (aError<0)
    {
        switch (aError)
        {
        case KErrNoMemory:
            aError = ESwtErrorNoHandles;
            break;
        case KErrArgument:
            aError = ESwtErrorInvalidArgument;
            break;
        case KErrPathNotFound:
            aError = ESwtErrorIO;
            break;
        case KErrAccessDenied:
            aError = ESwtErrorIO;
            break;
        case KErrWrite:
            aError = ESwtErrorIO;
            break;
        case KErrDisMounted:
            aError = ESwtErrorIO;
            break;
        case KErrEof:
            aError = ESwtErrorIO;
            break;
        case KErrDiskFull:
            aError = ESwtErrorIO;
            break;
        case KErrBadLibraryEntryPoint:
            aError = ESwtErrorFailedLoadLibrary;
            break;
        case KErrDivideByZero:
            aError = ESwtErrorCannotBeZero;
            break;
        case KErrBadPower:
            aError = ESwtErrorUnspecified;
            break;
        case KErrDirFull:
            aError = ESwtErrorIO;
            break;
        case KErrPermissionDenied:
            aError = ESwtErrorIO;
            break;
        default:
            aError = ESwtErrorUnspecified;
            break;
        }
    }

    // Generate the error
    ASSERT(aError > 0);  // SWT error code are > 0
    jclass clazz = aJniEnv->FindClass("org/eclipse/swt/SWT");
    if (clazz != NULL)
    {
        jmethodID id = aJniEnv->GetStaticMethodID(clazz, "error", "(I)V");
        if (id != NULL)
            aJniEnv->CallStaticVoidMethod(clazz, id, aError);
        aJniEnv->DeleteLocalRef(clazz);
    }
}


/**
 * Takes care of releasing a reference to a Java peer object
 */
void ReleasePeer(JNIEnv* aJniEnv, TSwtPeer aPeer)
{
    aJniEnv->DeleteWeakGlobalRef(aPeer);
    CSwtDisplay::Current().PeerDisposed(aPeer);
}


/**
 * Retrieves the ID of a Java method on the peer object
 */
jmethodID GetJavaMethodId(JNIEnv* aJniEnv, jobject aObject, const char* aMethodName, const char* aMethodSignature)
{
    jmethodID id    = NULL;
    jclass    clazz = aJniEnv->GetObjectClass(aObject);
    if (clazz != NULL)
    {
        id = aJniEnv->GetMethodID(clazz, aMethodName, aMethodSignature);
        aJniEnv->DeleteLocalRef(clazz);
    }

    return id;
}


/**
 * Calls a void-returning Java method
 */
void CallVoidJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, jobject aObject, const char* aMethodName, const char* aMethodSignature, ...)
{
    jmethodID id = GetJavaMethodId(aJniEnv, aObject, aMethodName, aMethodSignature);
    if (id != NULL)
    {
        va_list args;
        va_start(args, aMethodSignature);
        aJniEnv->CallVoidMethodV(aObject, id, args);
        va_end(args);
        aFailed = EFalse;
    }
    else
    {
        __DEBUGGER();
        aFailed = ETrue;
    }
}


/**
 * Calls a static void-returning Java method
 */
void CallStaticVoidJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, const char* aClazz, const char* aMethodName, const char* aMethodSignature, ...)
{
    aFailed = ETrue;
    jclass clazz = aJniEnv->FindClass(aClazz);
    if (clazz != NULL)
    {
        jmethodID id = aJniEnv->GetStaticMethodID(clazz, aMethodName, aMethodSignature);
        if (id != NULL)
        {
            va_list args;
            va_start(args, aMethodSignature);
            aJniEnv->CallStaticVoidMethodV(clazz, id, args);
            va_end(args);
            aFailed = EFalse;
        }
        aJniEnv->DeleteLocalRef(clazz);
    }
    if (aFailed)
    {
        __DEBUGGER();
    }
}


/**
 * Calls a boolean-returning Java method
 */
jboolean CallBooleanJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, jobject aObject, const char* aMethodName, const char* aMethodSignature, ...)
{
    jmethodID id = GetJavaMethodId(aJniEnv, aObject, aMethodName, aMethodSignature);
    if (id != NULL)
    {
        va_list args;
        va_start(args, aMethodSignature);
        jboolean result = aJniEnv->CallBooleanMethodV(aObject, id, args);
        va_end(args);
        aFailed = EFalse;
        return result;
    }
    else
    {
        __DEBUGGER();
        aFailed = ETrue;
        return JNI_FALSE; // got to return something...
    }
}


/**
 * Calls an object-returning Java method on the peer object
 */
jobject CallObjectJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, jobject aObject, const char* aMethodName, const char* aMethodSignature, ...)
{
    jmethodID id = GetJavaMethodId(aJniEnv, aObject, aMethodName, aMethodSignature);
    if (id != NULL)
    {
        va_list args;
        va_start(args, aMethodSignature);
        jobject result = aJniEnv->CallObjectMethodV(aObject, id, args);
        va_end(args);
        aFailed = EFalse;
        return result;
    }
    else
    {
        __DEBUGGER();
        aFailed = ETrue;
        return NULL; // got to return something...
    }
}


/**
 * Calls a static object-returning Java method
 */
jobject CallStaticObjectJavaMethod(TBool& aFailed, JNIEnv* aJniEnv, const char* aClazz, const char* aMethodName, const char* aMethodSignature, ...)
{
    jclass clazz = aJniEnv->FindClass(aClazz);
    if (clazz != NULL)
    {
        jmethodID id = aJniEnv->GetStaticMethodID(clazz, aMethodName, aMethodSignature);
        if (id != NULL)
        {
            va_list args;
            va_start(args, aMethodSignature);
            jobject result = aJniEnv->CallStaticObjectMethodV(clazz, id, args);
            va_end(args);
            aFailed = EFalse;
            aJniEnv->DeleteLocalRef(clazz);
            return result;
        }
        else
        {
            __DEBUGGER();
            aFailed = ETrue;
            aJniEnv->DeleteLocalRef(clazz);
            return NULL;
        }
    }
    else
    {
        __DEBUGGER();
        aFailed = ETrue;
        return NULL;
    }
}


/**
 * Gets the value of an integer field of a Java object.
 *
 * If the function fails it will set <code>aFailed</code> to <code>ETrue</code>.
 * If <code>aFailed</code> is true to begin with, nothing is performed.
 */
jint GetIntField(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, TBool& aFailed)
{
    jint result = 0;

    if (!aFailed)
    {
        jfieldID id = aJniEnv->GetFieldID(aClass, aName, "I");
        if (id == NULL)
            aFailed = ETrue;
        else
            result = aJniEnv->GetIntField(aObject, id);
    }

    return result;
}


/**
 * Gets the value of a boolean field of a Java object.
 *
 * If the function fails it will set <code>aFailed</code> to <code>ETrue</code>.
 * If <code>aFailed</code> is true to begin with, nothing is performed.
 */
jboolean GetBooleanField(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, TBool& aFailed)
{
    jboolean result = JNI_FALSE;

    if (!aFailed)
    {
        jfieldID id = aJniEnv->GetFieldID(aClass, aName, "Z");
        if (id == NULL)
            aFailed = ETrue;
        else
            result = aJniEnv->GetBooleanField(aObject, id);
    }

    return result;
}


/**
 * Gets the value of a jobject field of a Java object.
 *
 * If the function fails it will set <code>aFailed</code> to <code>ETrue</code>.
 * If <code>aFailed</code> is true to begin with, nothing is performed.
 */
jobject GetObjectField(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, const char* aSignature, TBool& aFailed)
{
    if (aFailed)
        return NULL;

    jobject result = NULL;

    jfieldID id = aJniEnv->GetFieldID(aClass, aName, aSignature);
    if (id == NULL)
        aFailed = ETrue;
    else
        result = aJniEnv->GetObjectField(aObject, id);

    return result;
}


/**
 * Gets the value of a string field of a Java object.
 *
 * If the function fails it will set <code>aFailed</code> to <code>ETrue</code>.
 * If <code>aFailed</code> is true to begin with, nothing is performed.
 */
HBufC* GetStringFieldL(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, TBool& aFailed)
{
    if (aFailed)
        return NULL;

    HBufC* result = NULL;

    jfieldID id = aJniEnv->GetFieldID(aClass, aName, "Ljava/lang/String;");
    if (id == NULL)
        aFailed = ETrue;
    else
    {
        RJavaLocalRef<jstring> str(aJniEnv, static_cast<jstring>(aJniEnv->GetObjectField(aObject, id)));
        if (str)
        {
            CleanupClosePushL(str);
            result = ConvertStringL(aJniEnv, str);
            CleanupStack::PopAndDestroy(); // str
        }
    }

    return result;
}


/**
 * Gets the data contained in a byte array field of a Java object.
 *
 * The function may fail in two different ways, it can leave if a native error
 * occurred or it will set <code>aFailed</code> to <code>ETrue</code> if a Java
 * error occurred.
 *
 * If <code>aFailed</code> is true upon function call, nothing is performed.
 *
 * @return A <code>HBufC8</code> to a buffer containing the data, the buffer may
 *         be <code>NULL</code> even if the function succeeded.
 */
HBufC8* GetByteArrayFieldL(JNIEnv* aJniEnv, jobject aObject, jclass aClass, const char* aName, TBool& aFailed)
{
    HBufC8* result = NULL;

    if (!aFailed)
    {
        jfieldID id = aJniEnv->GetFieldID(aClass, aName, "[B");
        if (id == NULL)
            aFailed = ETrue;
        else
        {
            RJavaLocalRef<jbyteArray> field(aJniEnv, static_cast<jbyteArray>(aJniEnv->GetObjectField(aObject, id)));
            if (field)
            {
                CleanupClosePushL(field);

                // Allocate a buffer to contain the data
                TInt length = aJniEnv->GetArrayLength(field);
                result = HBufC8::NewMaxL(length);

                // Copy the data to the newly allocated buffer
                aJniEnv->GetByteArrayRegion(field, 0, length, (jbyte*)(result->Ptr()));
                CleanupStack::PopAndDestroy(); // field
            }
        }
    }

    return result;
}


/**
 * Converts a Java <code>String</code> object to a 16-bit Symbian descriptor.
 *
 * Surrogate pairs (i.e. characters encoded on two 16-bits words) are not
 * supported by Symbian OS and are therefore replaced by <code>KReplacementCharacter</code>.
 *
 * @sa <code>ConvertStringL()</code>
 * @sa <code>ConvertString16LC()</code>
 */
HBufC16* ConvertString16LC(JNIEnv* aJniEnv, const jstring aString)
{
    if (aString == NULL)
        return NULL;

    // Allocate a buffer
    const TInt length = aJniEnv->GetStringLength(aString);
    HBufC16*   buffer = HBufC16::NewLC(length);
    TPtr16     des(buffer->Des());
    TText16*   ptr = const_cast<TText16*>(des.Ptr());

    // Copy the content (stripping surrogates on the fly)
    jboolean     isCopy;
    const jchar* chars  = aJniEnv->GetStringChars(aString, &isCopy);
    TInt outputIndex = 0;
    for (TInt inputIndex=0 ; inputIndex<length; ++inputIndex)
    {
        jchar ch = chars[inputIndex];
        if (!IsSurrogate(ch))
            ptr[outputIndex++] = ch;
        else
        {
            // Replace high surrogate only, the trailing low surrogate will be skipped.
            if (IsHighSurrogate(ch))
                ptr[outputIndex++] = KReplacementCharacter;
        }
    }

    des.SetLength(outputIndex);

    aJniEnv->ReleaseStringChars(aString, chars);
    return buffer;
}


/**
 * Converts a Java array of integers to an array of <code>TPoint</code>s.
 *
 * The resulting array is to be freed by the caller using <code>delete[]</code>.
 *
 * @param aJniEnv   The JNI environment
 * @param aIntArray The array to convert
 * @param aPoints   A reference to the pointer holding the resulting array,
 *                  must be <code>NULL</code> upon call. Will remain <code>NULL</code>
 *                  if an error occurs or there is nothing to be converted.
 *
 * @return The number of elements in <code>aPoints</code>.
 */
TInt JavaIntsToPoints(JNIEnv* aJniEnv, jintArray aIntArray, TPoint*& aPoints)
{
    ASSERT(aPoints==NULL);

    // Get the count of points
    TInt count = (aJniEnv->GetArrayLength(aIntArray)) / 2;
    if (count == 0)
        return 0;

    // Get the integers
    jboolean isCopy;
    jint* elements = aJniEnv->GetIntArrayElements(aIntArray, &isCopy);
    if (elements == NULL)
        return 0;

    // Alloc and fill an array of points
    aPoints = new TPoint[count]; // Intentionally not new(ELeave)
    if (aPoints != NULL)
    {
        for (TInt i=0; i<count; ++i)
            aPoints[i].SetXY(elements[i*2], elements[i*2+1]);
    }

    // Cleanup
    aJniEnv->ReleaseIntArrayElements(aIntArray, elements, JNI_ABORT);

    // Deal with memory exhaustion
    if (aPoints == NULL)
        Throw(KErrNoMemory, aJniEnv);

    return count;
}

/**
 * Converts a Java array of integers to an array of <code>TInt</code>s.
 *
 * The resulting array is to be freed by the caller using <code>delete[]</code>.
 *
 * @param aJniEnv   The JNI environment
 * @param aIntArray The array to convert
 * @param aInts     A reference to the pointer holding the resulting array,
 *                  must be <code>NULL</code> upon call. Will remain <code>NULL</code>
 *                  if an error occurs or there is nothing to be converted.
 *
 * @return The number of elements in <code>aInts</code>.
 */
TInt JavaIntsToInts(JNIEnv* aJniEnv, jintArray aIntArray, TInt*& aInts)
{
    ASSERT(aInts == NULL);

    // Get the count of points
    TInt count = aJniEnv->GetArrayLength(aIntArray);
    if (count == 0)
        return 0;

    // Get the integers
    jboolean isCopy;
    jint* elements = aJniEnv->GetIntArrayElements(aIntArray, &isCopy);
    if (elements == NULL)
        return 0;

    // Alloc and fill an array of TInts
    aInts = new TInt[count]; // Intentionally not new(ELeave)
    if (aInts != NULL)
    {
        for (TInt i = 0; i < count; ++i)
            aInts[i] = elements[i];
    }

    // Cleanup
    aJniEnv->ReleaseIntArrayElements(aIntArray, elements, JNI_ABORT);

    // Deal with memory exhaustion
    if (aInts == NULL)
        Throw(KErrNoMemory, aJniEnv);

    return count;
}


/**
 * Copies a region of Java array of integers to an array of <code>TInt</code>s.
 *
 * The resulting array is to be freed by the caller using <code>delete[]</code>.
 *
 * @param aJniEnv   The JNI environment
 * @param aIntArray The array to convert
 * @param aStart    The starting index
 * @param aCount    The number of elements to be copied
 *
 * @return Pointer to an array of <code>aCount</code> <code>TInt</code>s owned by the caller.
 */
TInt* JavaIntRegionToInts(JNIEnv* aJniEnv,
                          jintArray aIntArray,
                          const jint& aStart,
                          const jint& aCount)
{
    TInt* ints = new TInt[aCount]; // Intentionally not new(ELeave)

    if (ints && (aCount > 0))
    {
        // Get the integers
        aJniEnv->GetIntArrayRegion(aIntArray, aStart, aCount, ints);
    }

    // Deal with memory exhaustion
    if (ints == NULL)
    {
        Throw(KErrNoMemory, aJniEnv);
    }

    return ints;
}

/**
 * Converts a Java array of integers to a symbian dynamic array of <code>TInt</code>s.
 *
 * The resulting array is to be freed by the caller using the Close() method.
 *
 * @param aJniEnv        The JNI environment
 * @param aIntArray      The array to convert
 * @param aResultArray   A reference to the dynamic array
 *
 */
void ConvertIntegerArrayL(JNIEnv* aJniEnv, jintArray aIntArray, RArray<TInt>& aResultArray)
{
    TInt count = aJniEnv->GetArrayLength(aIntArray);

    jboolean isCopy;
    jint* elements = aJniEnv->GetIntArrayElements(aIntArray, &isCopy);
    if (elements == NULL)
        return;

    for (TInt i=0; i<count; ++i)
    {
        TInt error = aResultArray.Append(elements[i]);
        User::LeaveIfError(error);
    }

    aJniEnv->ReleaseIntArrayElements(aIntArray, elements, JNI_ABORT);
}


/**
 * Converts a Java array of bytes to a symbian dynamic array of <code>TUint8</code>s.
 *
 * The resulting array is to be freed by the caller except if the returned value is NULL.
 *
 * @param aJniEnv        The JNI environment
 * @param aByteArray     The array to convert
 *
 */
HBufC8* ConvertByteArrayL(JNIEnv* aJniEnv, jbyteArray aByteArray)
{
    TInt count = aJniEnv->GetArrayLength(aByteArray);

    HBufC8* buffer = HBufC8::NewL(count);
    TPtr8 des(buffer->Des());
    des.SetLength(count);
    jbyte* ptr = reinterpret_cast<jbyte*>(const_cast<TUint8*>(des.Ptr()));
    aJniEnv->GetByteArrayRegion(aByteArray, 0, count, ptr);

    return buffer;
}


/**
 * Converts a Java string Array into a Symbian string array.
 *
 * @param aJniEnv      The JNI environment.
 * @param aStringArray The array to convert.
 *
 * @return A newly created string array, or <code>NULL</code> if the passed-in
 *         array was empty
 */
CDesCArray* ConvertStringArrayL(JNIEnv* aJniEnv, jobjectArray aStringArray)
{
    ASSERT(aStringArray != NULL);

    TInt count = aJniEnv->GetArrayLength(aStringArray);
    if (count == 0)
        return NULL;

    CDesCArray* result = new(ELeave) CDesCArrayFlat(count);
    CleanupStack::PushL(result);

    for (TInt i = 0; i < count; ++i)
    {
        RJavaLocalRef<jstring> str(aJniEnv, static_cast<jstring>(aJniEnv->GetObjectArrayElement(aStringArray, i)));
        CleanupClosePushL(str);
        HBufC* buf = ConvertStringLC(aJniEnv, str);
        if (buf != NULL)
        {
            result->AppendL(*buf);
            CleanupStack::PopAndDestroy(buf);
        }
        else
            result->AppendL(KNullDesC());
        CleanupStack::PopAndDestroy(); // str
    }

    CleanupStack::Pop(result);
    return result;
}


/**
 * Creates a new Java byte array
 */
jbyteArray NewJavaByteArray(JNIEnv* aJniEnv, const TDesC8& aArray)
{
    const TInt length = aArray.Length();
    jbyteArray result = aJniEnv->NewByteArray(length);
    if (result != NULL)
        aJniEnv->SetByteArrayRegion(result, 0, length, reinterpret_cast<jbyte*>(const_cast<TText8*>(aArray.Ptr())));
    return result;
}


/**
 * Creates a new Java byte array
 */
jbyteArray NewJavaByteArray(JNIEnv* aJniEnv, const TDesC8* aArray, TBool& aFailed)
{
    if (aFailed || aArray==NULL)
        return NULL;

    jbyteArray result = NewJavaByteArray(aJniEnv,*aArray);
    if (result == NULL)
        aFailed = ETrue;
    return result;
}


/**
 * Creates a new Java Integer array
 */
jintArray NewJavaIntArray(JNIEnv* aJniEnv, const TInt* aArray, TInt aCount)
{
    jintArray result = aJniEnv->NewIntArray(aCount);
    if (result != NULL)
        aJniEnv->SetIntArrayRegion(result, 0, aCount, const_cast<TInt*>(aArray));
    return result;
}


/**
 * Creates a Java <code>Point</code> object
 */
jobject NewJavaPoint(JNIEnv* aJniEnv, const TPoint& aPoint)
{
    jobject result = NULL;

    jclass clazz = aJniEnv->FindClass("org/eclipse/swt/graphics/Point");
    if (clazz != NULL)
    {
        jmethodID id = aJniEnv->GetMethodID(clazz, "<init>", "(II)V");
        if (id != NULL)
            result = aJniEnv->NewObject(clazz, id, aPoint.iX, aPoint.iY);
        aJniEnv->DeleteLocalRef(clazz);
    }

    return result;
}


/**
 * Creates a Java <code>Rectangle</code> object
 */
jobject NewJavaRectangle(JNIEnv* aJniEnv, const TRect& aRect)
{
    jobject result = NULL;

    jclass clazz = aJniEnv->FindClass("org/eclipse/swt/graphics/Rectangle");
    if (clazz != NULL)
    {
        jmethodID id = aJniEnv->GetMethodID(clazz, "<init>", "(IIII)V");
        if (id != NULL)
            result = aJniEnv->NewObject(clazz, id, aRect.iTl.iX, aRect.iTl.iY, aRect.Width(), aRect.Height());
        aJniEnv->DeleteLocalRef(clazz);
    }

    return result;
}


/**
 * Creates a <code>PaletteData</code> object.
 */
static jobject NewJavaPaletteData(JNIEnv* aJniEnv, const MSwtPaletteData& aData)
{
    jobject result = NULL;

    jclass paletteClazz = aJniEnv->FindClass("org/eclipse/swt/graphics/PaletteData");
    if (paletteClazz != NULL)
    {
        if (aData.IsDirect())
        {
            jmethodID id = aJniEnv->GetMethodID(paletteClazz, "<init>", "(III)V");
            const MSwtPaletteData::TDirectData& directData = aData.DirectData();
            if (id != NULL)
                result = aJniEnv->NewObject(paletteClazz, id, directData.iRedMask, directData.iGreenMask, directData.iBlueMask);
        }
        else
        {
            jclass rgbClazz = aJniEnv->FindClass("org/eclipse/swt/graphics/RGB");
            if (rgbClazz != NULL)
            {
                const CPalette* colors = aData.IndirectData();
                const TInt      count  = colors->Entries();

                jobjectArray rgbArray = aJniEnv->NewObjectArray(count, rgbClazz, NULL);
                if (rgbArray != NULL)
                {
                    jmethodID rgbId = aJniEnv->GetMethodID(rgbClazz, "<init>", "(III)V");
                    if (rgbId != NULL)
                    {
                        for (TInt i=0; i<count; ++i)
                        {
                            TRgb col(colors->GetEntry(i));
                            jobject rgb = aJniEnv->NewObject(rgbClazz, rgbId, col.Red(), col.Green(), col.Blue());
                            aJniEnv->SetObjectArrayElement(rgbArray, i, rgb);
                            aJniEnv->DeleteLocalRef(rgb);
                        }
                    }

                    jmethodID paletteId = aJniEnv->GetMethodID(paletteClazz, "<init>", "([Lorg/eclipse/swt/graphics/RGB;)V");
                    if (paletteId != NULL)
                        result = aJniEnv->NewObject(paletteClazz, paletteId, rgbArray);

                    aJniEnv->DeleteLocalRef(rgbArray);
                }

                aJniEnv->DeleteLocalRef(rgbClazz);
            }
        }

        aJniEnv->DeleteLocalRef(paletteClazz);
    }

    return result;
}


/**
 * Creates a <code>ImageData</code> object
 */
jobject NewJavaImageData(JNIEnv* aJniEnv, const MSwtImageData& aData)
{
    jobject result = NULL;

    jclass clazz = aJniEnv->FindClass("org/eclipse/swt/graphics/ImageData");
    if (clazz != NULL)
    {
        jmethodID id = aJniEnv->GetMethodID(clazz, "<init>", "(IIILorg/eclipse/swt/graphics/PaletteData;I[BI[B[BIIIIIII)V");
        if (id != NULL)
        {
            TBool failure = EFalse;

            jobject    palette   = NewJavaPaletteData(aJniEnv, aData.Palette());
            jbyteArray pixelData = NewJavaByteArray(aJniEnv, aData.PixelBuffer());
            jbyteArray maskData  = NewJavaByteArray(aJniEnv, aData.MaskBuffer(),  failure);
            jbyteArray alphaData = NewJavaByteArray(aJniEnv, aData.AlphaBuffer(), failure);

            if (palette!=NULL && pixelData!=NULL && !failure)
            {
                const MSwtImageData::TInfo& info = aData.Info();
                result = aJniEnv->NewObject(clazz, id, info.iSize.iWidth, info.iSize.iHeight, info.iDepth,
                                            palette, info.iScanlinePad, pixelData, info.iMaskPad,  maskData,
                                            alphaData, info.iAlpha, info.iTransparentPixel, info.iType,
                                            info.iTopLeft.iX, info.iTopLeft.iY, info.iDisposalMethod, info.iDelayTime);
            }

            aJniEnv->DeleteLocalRef(palette);
            aJniEnv->DeleteLocalRef(pixelData);
            aJniEnv->DeleteLocalRef(maskData);
            aJniEnv->DeleteLocalRef(alphaData);
        }

        aJniEnv->DeleteLocalRef(clazz);
    }

    return result;
}


/**
 * Creates a <code>ImageData[]</code> object
 *
 */
jobjectArray NewJavaImageDataArray(JNIEnv* aJniEnv, const CSwtImageDataArray* aArray)
{
    jobjectArray resultArray = NULL;

    TInt count = aArray->Count();

    jclass clazz = aJniEnv->FindClass("org/eclipse/swt/graphics/ImageData");
    if (clazz == NULL)
        return NULL;


    resultArray = aJniEnv->NewObjectArray(count,clazz,NULL);
    aJniEnv->DeleteLocalRef(clazz);

    if (resultArray != NULL)
    {
        for (TInt index = 0; index < count; ++index)
        {
            jobject result = NewJavaImageData(aJniEnv, *(*aArray)[index]);
            aJniEnv->SetObjectArrayElement(resultArray,index,result);
        }
    }

    return resultArray;
}


/**
 * Creates a Java <code>RGB</code> object
 */
jobject NewJavaRgb(JNIEnv* aJniEnv, const TRgb& aRgb)
{
    jobject result = NULL;

    jclass clazz = aJniEnv->FindClass("org/eclipse/swt/graphics/RGB");
    if (clazz != NULL)
    {
        jmethodID id = aJniEnv->GetMethodID(clazz, "<init>", "(III)V");
        if (id != NULL)
            result = aJniEnv->NewObject(clazz, id, aRgb.Red(), aRgb.Green(), aRgb.Blue());
        aJniEnv->DeleteLocalRef(clazz);
    }

    return result;
}


/**
 * Creates a Java <code>Control[]</code> array
 *
 * @param aJniEnv The JNI environment
 * @param aArray  The array of peers to convert, if <code>NULL</code> an empty
 *                array is returned.
 */
jobjectArray NewJavaControlArray(JNIEnv* aJniEnv, const CSwtPeerArray* aArray)
{
    return NewJavaObjectArray(aJniEnv, aArray, "org/eclipse/swt/widgets/Control");
}


/**
 * Fills a SwtListBoxItem according to a Java ListBox item.
 *
 * @param aJniEnv   The JNI environment
 * @param aJavaItem The source item.
 */
CSwtListBoxItem* ConvertListBoxItemL(JNIEnv* aJniEnv, const jobject aJavaItem)
{
    TBool failed = EFalse;

    if (aJavaItem == NULL)
    {
        return NULL;
    }

    RJavaLocalRef<jclass> listBoxItemClazz(aJniEnv, aJniEnv->FindClass("org/eclipse/ercp/swt/mobile/ListBoxItem"));
    if (!listBoxItemClazz)
        return NULL;
    CleanupClosePushL(listBoxItemClazz);

    // Get the two strings first
    HBufC* string = GetStringFieldL(aJniEnv, aJavaItem, listBoxItemClazz, "detailText",  failed);
    CleanupStack::PushL(string);
    HBufC* headingString = GetStringFieldL(aJniEnv, aJavaItem, listBoxItemClazz, "headingText", failed);
    CleanupStack::PushL(headingString);

    const MSwtImage* icon        = NULL;
    const MSwtImage* headingIcon = NULL;

    jclass imageClazz = aJniEnv->FindClass("org/eclipse/swt/graphics/Image");
    if (imageClazz == NULL)
        failed = ETrue;
    else
    {
        // Get the icon
        jobjectArray images = static_cast<jobjectArray>(GetObjectField(aJniEnv, aJavaItem, listBoxItemClazz, "detailIcons", "[Lorg/eclipse/swt/graphics/Image;", failed));
        if (images != NULL)
        {
            // we only get the first image
            jobject image = aJniEnv->GetObjectArrayElement(images, 0);
            if (image)
            {
                TInt imageHandle = GetIntField(aJniEnv, image, imageClazz, "handle", failed);
                aJniEnv->DeleteLocalRef(image);
                icon = reinterpret_cast<MSwtImage*>(imageHandle);
            }
            aJniEnv->DeleteLocalRef(images);
        }

        // Get the heading icon
        images = static_cast<jobjectArray>(GetObjectField(aJniEnv, aJavaItem, listBoxItemClazz, "headingIcons", "[Lorg/eclipse/swt/graphics/Image;", failed));
        if (images != NULL)
        {
            // we only get the first image
            jobject image = aJniEnv->GetObjectArrayElement(images, 0);
            if (image)
            {
                TInt imageHandle = GetIntField(aJniEnv, image, imageClazz, "handle", failed);
                aJniEnv->DeleteLocalRef(image);
                headingIcon = reinterpret_cast<MSwtImage*>(imageHandle);
            }
            aJniEnv->DeleteLocalRef(images);
        }

        aJniEnv->DeleteLocalRef(imageClazz);
    }

    // Return
    CSwtListBoxItem* result = (!failed) ? new(ELeave) CSwtListBoxItem(string, icon, headingString, headingIcon) : NULL;
    CleanupStack::Pop(2, string);  // headingString, string
    CleanupStack::PopAndDestroy(); // listBoxItemClazz
    return result;
}


/**
 *
 */
RSwtListBoxItemsArray ConvertListBoxItemArrayL(JNIEnv* aJniEnv, const jobjectArray aDataModelArray, TBool& /*aFailed*/)
{
    const TInt count = aJniEnv->GetArrayLength(aDataModelArray);
    RSwtListBoxItemsArray array(KListBoxItemArrGranularity);
    CleanupResetAndDestroyPushL(array);

    for (TInt i=0; i<count; ++i)
    {
        RJavaLocalRef<jobject> jItem(aJniEnv, aJniEnv->GetObjectArrayElement(aDataModelArray, i));
        CleanupClosePushL(jItem);
        CSwtListBoxItem* item = ConvertListBoxItemL(aJniEnv, jItem);
        if (item)
        {
            CleanupStack::PushL(item);
        }
        // item can be also null
        User::LeaveIfError(array.Append(item));
        if (item)
        {
            CleanupStack::Pop(item);
        }
        CleanupStack::PopAndDestroy(); // jItem
    }

    CleanupStack::Pop(); // array
    return array;
}


/**
 * Creates a Java Object array
 *
 * @param aJniEnv    The JNI environment
 * @param aArray     The array of peers to convert, if <code>NULL</code> an
 *                   empty array is returned.
 * @param aClassname The Java class name of the elements in the array
 */
jobjectArray NewJavaObjectArray(JNIEnv* aJniEnv, const CSwtPeerArray* aArray, const char* aClassname)
{
    jobjectArray result = NULL;

    jclass objectClass = aJniEnv->FindClass(aClassname);
    if (objectClass != NULL)
    {
        const TInt count = (aArray!=NULL) ? aArray->Count() : 0;
        result = aJniEnv->NewObjectArray(count, objectClass, NULL);
        if (result != NULL)
        {
            for (TInt i=0; i<count; ++i)
                aJniEnv->SetObjectArrayElement(result, i, (*aArray)[i]); //lint !e613
        }
        aJniEnv->DeleteLocalRef(objectClass);
    }

    return result;
}


/**
 * Checks if default Display (that owns UI thread, event queue, ...) has been disposed
 */
TBool IsDefaultDisplayDisposed(TBool& aFailed, JNIEnv* aJniEnv)
{
    jobject defaultDisplay = CallStaticObjectJavaMethod(aFailed,
                             aJniEnv,
                             "org/eclipse/swt/internal/symbian/DefaultDisplayWrapper",
                             "get",
                             "()Lorg/eclipse/swt/widgets/Display;");
    if (defaultDisplay != NULL)
    {
        jboolean isDisposed = CallBooleanJavaMethod(aFailed, aJniEnv, defaultDisplay, "isDisposed", "()Z");
        return isDisposed;
    }
    else
    {
        return ETrue;
    }
}
