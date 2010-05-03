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


#include "s60commonutils.h"
#include "com_nokia_mj_impl_gcf_utils_NativeError.h"
#include <textresolver.h>

using namespace java::util;


// forward declaration
CTextResolver* createTextResolverL();
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_gcf_utils_NativeError__1getNativeErrorMessage
(JNIEnv* aJni, jclass /* aPeer */, jint aError)
{
    jstring result = NULL;

    CTextResolver* iTextResolver = NULL;
    TRAPD(err, createTextResolverL(); CleanupStack::PopAndDestroy());
    if (err < 0)
    {
        CleanupStack::PopAndDestroy(); // iTextResolver
        return result;
    }

    TPtrC errorMsg;
    errorMsg.Set(iTextResolver->ResolveErrorString(aError));
    result = S60CommonUtils::NativeToJavaString(*aJni, errorMsg);
    CleanupStack::PopAndDestroy(); // iTextResolver

    return result;
}

CTextResolver* createTextResolverL()
{
    CTextResolver * iResolver = CTextResolver::NewLC();
    return iResolver;
}



