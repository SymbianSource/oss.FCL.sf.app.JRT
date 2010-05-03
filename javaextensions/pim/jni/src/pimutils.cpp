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
#include "pimutils.h"
#include "logger.h"

void SetJavaErrorCode(JNIEnv* aJniEnv, jintArray aErrorTo,
                      const TInt& aErrorFrom)
{
    JELOG2(EPim);
    jint javaError[1] = { aErrorFrom };
    aJniEnv->SetIntArrayRegion(aErrorTo, 0, 1, javaError);
}
