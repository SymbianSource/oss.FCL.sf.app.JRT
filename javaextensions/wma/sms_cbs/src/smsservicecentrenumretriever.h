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


#include <jni.h>

namespace java
{
namespace wma
{
class SmsServiceCentreNumRetriever
{
public:

    /**
     * This function retrieves service centre number.
     * @param[in] aJni - Jni interface pointer.
     * @return jstring returns the SC number as jstring.
     */
    jstring static getServiceCentreNum(JNIEnv& aJni);

};
} //namespace wma
} //namespace java
