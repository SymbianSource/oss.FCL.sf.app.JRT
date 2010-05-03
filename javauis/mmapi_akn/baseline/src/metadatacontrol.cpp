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
* Description:  This class has JNI wrappers for CMMAMetaDataControl
*
*/


#include <jdebug.h>
#include "com_nokia_microedition_media_control_MetaDataControl.h"

#include "cmmaeventsource.h"
#include "cmmametadatacontrol.h"


/* ...getKeyCount
 *
 * Returns the number of metadata keys or symbian error code
 */

JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_MetaDataControl__1getKeysCount
(JNIEnv* /*aJniEnv*/, jobject, jint aEventSourceHandle, jint aMetaDataControlHandle)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAMetaDataControl* metaDataControl =
        JavaUnhand< CMMAMetaDataControl >(aMetaDataControlHandle);

    TInt keys;
    TInt err = eventSource->ExecuteTrap(&CMMAMetaDataControl::StaticKeysCountL, metaDataControl, &keys);

    return err == KErrNone ? keys : err;
}


/* ... getKey
 *
 * Get the key name at the given index from the actual native implementation. If no key is available,
 * return null.
 *
 * The ownership of the native data is received from the actual native implementation.
 * The data is duplicated to the Java side and the native data is destroyed here.
 */

JNIEXPORT jstring JNICALL Java_com_nokia_microedition_media_control_MetaDataControl__1getKey
(JNIEnv* aJniEnv, jobject, jint aEventSourceHandle, jint aMetaDataControlHandle, jint aIndex)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAMetaDataControl* metaDataControl =
        JavaUnhand< CMMAMetaDataControl >(aMetaDataControlHandle);

    HBufC* key = NULL;
    jstring javaStr = NULL;

    TInt err = eventSource->ExecuteTrap(&CMMAMetaDataControl::StaticKeyL, metaDataControl, &key, aIndex);

    if (err == KErrNone)
    {
        javaStr = CreateJavaString(*aJniEnv, key->Des());
    }
    delete key;

    return javaStr;
}

/* ...getKeyValue
 *
 * Get the value of the given key from the actual native implementation. If no key is available,
 * return null (as specified in the MetaDataControl interface).
 *
 * The ownership of the native data is received from the actual native implementation.
 * The data is duplicated to the Java side and the native data is destroyed here.
 */

JNIEXPORT jstring JNICALL Java_com_nokia_microedition_media_control_MetaDataControl__1getKeyValue
(JNIEnv* aJniEnv, jobject, jint aEventSourceHandle, jint aMetaDataControlHandle, jstring aKey)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAMetaDataControl* metaDataControl =
        JavaUnhand< CMMAMetaDataControl >(aMetaDataControlHandle);

    HBufC* value = NULL;
    RJString key(*aJniEnv, aKey);

    TInt retVal = eventSource->ExecuteTrap(
                      &CMMAMetaDataControl::StaticKeyValueL,
                      metaDataControl,
                      &value,
                      (TDesC*) &key);

    jstring retString = NULL;

    if (KErrNone == retVal)
    {
        retString = CreateJavaString(*aJniEnv, *value);
    }

    delete value;

    return retString;
}

//  END OF FILE
