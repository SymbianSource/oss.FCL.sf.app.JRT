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
#include <jutils.h>

/*
 * The following macro makes it possible to transfer data from a descriptor
 * to a java byte array
 */
#define JAVA_PTR(aPtr) reinterpret_cast< jbyte * >( const_cast< unsigned char * >( aPtr ) )


/**
* Sets an error code to the first element of given Java int array.
*
* @param aJniEnv Pointer to JNI environment.
* @param aErrorTo Java int array, where to put the error code.
* @param aErrorFrom Error code.
*
* @par Notes:
* @li The function panics if the array does not have at least one element.
*/
void STSSetJavaErrorCode(
    JNIEnv* aJniEnv,
    jintArray aErrorTo,
    const TInt& aErrorFrom);

/**
 * Creates a java string array to a native string array.
 *
 * @param aJniEnv JNI environment.
 * @param aJavaArray Java string array.
 *
 * @return Native string array, elements converted from \a aJavaArray.
 *
 * @par Notes:
 * @li It is assumed that the object array contains only jstring objects.
 * Object type is \b not checked.
 *
 * @par Leaving:
 * @li Any - internal error (probably OOM).
 */
CDesCArrayFlat* STSCreateNativeStringArrayL(
    JNIEnv* aJniEnv,
    jobjectArray aJavaArray);


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
jobjectArray CreateJavaStringArray(
    JNIEnv* aJniEnv,
    const CDesCArray& aNativeArray,
    TBool aConvertPIMNullArrayElements);

// End of File

