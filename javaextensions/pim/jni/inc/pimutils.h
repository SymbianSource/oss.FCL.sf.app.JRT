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


#ifndef PIMUTILS_H
#define PIMUTILS_H

//  INCLUDE FILES
#include <jni.h>
#include <badesca.h>
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
void SetJavaErrorCode(JNIEnv* aJniEnv, jintArray aErrorTo,
                      const TInt& aErrorFrom);

#endif // PIMUTILS_H
// End of File
