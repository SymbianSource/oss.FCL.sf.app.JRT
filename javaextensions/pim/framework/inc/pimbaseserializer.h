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
* Description:  Serializer JNI wrapper.
 *
*/


#include "pimcommon.h"
#include <jni.h>

class pimbasemanager;
class pimbaseitem;

class pimbaseserializer
{
public:

    virtual pimbaseserializer::~pimbaseserializer()
    {
    };

    virtual jintArray fromSerialFormat(JNIEnv* aJniEnv, jbyteArray aBytes,
                                       int aByteLength, int aEncoding, jintArray aError) = 0;

    virtual jbyteArray toSerialFormat(pimbaseitem* aItem, int aEncoding,
                                      JNIEnv* aJniEnv, jintArray aError) = 0;

    virtual jobjectArray supportedSerialFormats(TPIMListType aPimListType,
            JNIEnv* aJniEnv, jintArray aError) = 0;

    static pimbaseserializer* getInstance(pimbasemanager* aManager);
};
