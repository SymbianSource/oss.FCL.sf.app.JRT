/*
* Copyright (c) 1999-2000 Nokia Corporation and/or its subsidiary(-ies).
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


#include "jutils.h"

/**
 * Constuctor taking a Java JNI string and converting it to an EPOC TPtrC.
 */
EXPORT_C RJString::RJString(JNIEnv& aJni, jstring aString)
        : iJni(aJni), iString(aString)
{
    // Potential for a string to be NULL, but NULL cannot be passed into
    // JNI methods, so must check. If string is NULL, will result in empty string
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



/**
 * Frees up the JNI string resources, if they need to be freed.
 */
EXPORT_C RJString::~RJString()
{
    if (iString != NULL)
    {
        iJni.ReleaseStringChars(iString, this->Ptr());
    }
}
