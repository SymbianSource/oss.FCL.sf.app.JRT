/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  URI parser specific to symbian
*
*/




#include "turi.h"
#include "com_nokia_mj_impl_gcf_utils_URI.h"
#include "jstringutils.h"
#include "s60commonutils.h"

using namespace java::util;

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_gcf_utils_URI__1parse
(JNIEnv* aJni, jobject, jstring aUriString, jint aUriPortion,
 jstring aParameterName, jintArray aErrorCache)
{
    JStringUtils uriString(*aJni, aUriString);
    TUri uri;
    TInt error = uri.Parse(uriString);
    TPtrC uriPart(KNullDesC());
    if (error == KErrNone)
    {
        switch (aUriPortion)
        {
        case com_nokia_mj_impl_gcf_utils_URI_PROTOCOL:
            uriPart.Set(uri.Scheme());
            break;
        case com_nokia_mj_impl_gcf_utils_URI_HOST:
            uriPart.Set(uri.Host());
            break;
        case com_nokia_mj_impl_gcf_utils_URI_PORT:
            uriPart.Set(uri.Port());
            break;
        case com_nokia_mj_impl_gcf_utils_URI_PATH:
            uriPart.Set(uri.Path());
            break;
        case com_nokia_mj_impl_gcf_utils_URI_PARAMETER:
        {
            JStringUtils parameterName(*aJni, aParameterName);
            error = uri.GetParameterValue(parameterName, uriPart);
            break;
        }
        case com_nokia_mj_impl_gcf_utils_URI_QUERY:
            uriPart.Set(uri.Query());
            break;
        default:
            __ASSERT_DEBUG(EFalse, User::Invariant());
        }
    }
    jstring result = NULL;
    // Return an empty string if the parameter value is not found
    if (error == KErrNone)
    {
        result = S60CommonUtils::NativeToJavaString(*aJni, uriPart);
    }
    else
    {
        // Cache the error code in the array, as we can't return it
        aJni->SetIntArrayRegion(aErrorCache, 0, 1, &error);
    }
    return result;
}


