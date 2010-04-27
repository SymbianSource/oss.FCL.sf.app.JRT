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
* Description:
*
*/

#include "jstringutils.h"


/**
 * Converts a  JNI string a to an  TPtrC Desc.
 */
OS_EXPORT JStringUtils::JStringUtils(JNIEnv& aJni, jstring aString)
        : iJni(aJni), iString(aString)
{
    if (iString != NULL)
    {
        const jchar* ptr = aJni.GetStringChars(iString, NULL);
        const jint   len = aJni.GetStringLength(iString);
        if (ptr)
        {
            Set(ptr, len);
        }
    }
}


OS_EXPORT JStringUtils::~JStringUtils()
{
    if (iString != NULL)
    {
        iJni.ReleaseStringChars(iString, this->Ptr());
    }
}
