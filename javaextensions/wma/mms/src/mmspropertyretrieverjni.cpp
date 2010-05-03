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
* Description:  ?Description
 *
*/


#include "logger.h"
#include "com_nokia_mj_impl_mms_MmsPropertyRetriever.h"
#include "mmspropertyretriever.h"

using namespace java::wma;

/*
 * Class:     com_nokia_mj_impl_mms_MmsRetriever
 * Method:    _getMMSC
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_mms_MmsPropertyRetriever__1getMMSC
(JNIEnv* aEnv, jclass)
{
    JELOG2(EWMA);
    jstring mmsc = MmsPropertyRetriever::retrieveMsgCenter(*aEnv);
    return mmsc;
}

/*
 * Class:     com_nokia_mj_impl_mms_MmsRetriever
 * Method:    _maxMMSSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_mms_MmsPropertyRetriever__1maxMMSSize
(JNIEnv* , jclass)
{
    JELOG2(EWMA);
    jint mmsMaxSize=NULL;
    mmsMaxSize = MmsPropertyRetriever::retrieveMaxMMSSize();
    return mmsMaxSize;
}

/*
 * Class:     com_nokia_mj_impl_mms_MmsRetriever
 * Method:    _isValidEncoding
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_mms_MmsPropertyRetriever__1isValidEncoding
(JNIEnv* aEnv , jclass ,jstring aContentEncoding)
{
    JELOG2(EWMA);
    return MmsPropertyRetriever::isValidEncoding(*aEnv,aContentEncoding);
}

