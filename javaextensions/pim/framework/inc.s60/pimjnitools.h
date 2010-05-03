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


#ifndef PIMJNITOOLS_H
#define PIMJNITOOLS_H

//  INCLUDE FILES
#include <jni.h>
#include <badesca.h>
#include "pimcommon.h"

//  FORWARD DECLARATIONS
class CPIMItem;



/**
 * Creates a new Java integer array from the elements in given native integer
 * array.
 *
 * @param aSourceArray Array of integers.
 *
 * @return A Java integer array with exactly the same elements as in
 *         \a aSourceArray or \c NULL in error, which can be taken as
 *         an out-of-memory situation.
 */
jintArray ConvertToJavaIntArray(JNIEnv& aJniEnv,
                                const CArrayFix<TInt>& aSourceArray);

/**
 * Extracts the handles from given items.
 *
 * @param aJniEnv JNI environment.
 * @param aItems Array of items.
 *
 * @return Java integer array of item handles constructed from the handles
 *         of \a aItems; or NULL on error, which can be taken as out-of-memory
 *         situation.
 */
jintArray GetJavaItemHandles(JNIEnv& aJniEnv,
                             const RPointerArray<CPIMItem>& aItems);

/**
 * Converts given TPIMDate array to Java int array, splitting each
 * source date element to contiguous two target int elements in
 * even-odd pairs.
 * @li High 32 bits go to even elements (0, 2, 4, ...)
 * @li Low 32 bits go to odd elements (1, 3, 5, ...)
 *
 * @param aDates Source array of dates.
 *
 * @return New array of Java ints, or \c NULL in error. The length
 *         of the returned array is 2 * length of \a aDates.
 */
jintArray NativeDatesToJavaInts(JNIEnv& aJniEnv,
                                const CArrayFix<TPIMDate>& aDates);

/**
 * Creates native string array from Java string array.
 * NULL elements are converted to KPIMNullArrayElement descriptors.
 *
 * @param aJniEnv JNI environment.
 * @param aJavaArray Java string array.
 *
 * @return Native string array, elements converted from \a aJavaArray.
 *
 * @par Notes:
 * @li It is assumed that the object array contains only jstring objects or
 * NULL pointers. Object type is \b not checked.
 *
 * @par Leaving:
 * @li Any - internal error (probably OOM).
 */
CDesCArray* CreateNativeStringArrayL(JNIEnv* aJniEnv, jobjectArray aJavaArray);

/**
 * Creates Java string array from native string array.
 *
 * @param aJniEnv JNI environment.
 * @param aNativeArray Native string array.
 * @param aConvertPIMNullArrayElements If ETrue, KPIMNullArrayElement
 *        elements are converted to NULL elements; otherwise they
 *        are not handled specially.
 *
 * @return Java string array, elements converted from \a aNativeArray
 *         or NULL on error (probably OOM).
 */
jobjectArray CreateJavaStringArray(JNIEnv* aJniEnv,
                                   const CDesCArray& aNativeArray, TBool aConvertPIMNullArrayElements);

#endif // PIMJNITOOLS_H
// End of File

