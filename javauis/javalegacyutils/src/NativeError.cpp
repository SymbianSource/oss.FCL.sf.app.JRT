/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include <jutils.h>
#include "com_nokia_mj_impl_rt_legacy_NativeError.h"
#include <textresolver.h>

jstring getErrorL(JNIEnv* aJni, jint aError)
{
    jstring result = NULL;
    CTextResolver* iTextResolver = CTextResolver::NewLC();
    TPtrC errorMsg;
    errorMsg.Set(iTextResolver->ResolveErrorString(aError));
    result = CreateJavaString(*aJni, errorMsg);
    CleanupStack::PopAndDestroy(); // iTextResolver
    return result;
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_rt_legacy_NativeError__1getNativeErrorMessage
(JNIEnv* aJni, jclass /* aPeer */, jint aError)
{
    jstring result = NULL;
    TRAP_IGNORE(result = getErrorL(aJni, aError));
    return result;
}
