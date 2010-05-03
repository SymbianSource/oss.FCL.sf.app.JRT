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


#include "logger.h"
#include "smsservicecentrenumretriever.h"
#include "com_nokia_mj_impl_sms_SMSCRetriever.h"

using namespace java::wma;

/*
 * Class:     com_nokia_mj_impl_sms_SMSCRetriever
 * Method:    _getSmscNum
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_sms_SMSCRetriever__1getSmscNum
(JNIEnv* aEnv, jclass)
{
    JELOG2(EWMA);
    jstring smsc = NULL;
    smsc = SmsServiceCentreNumRetriever::getServiceCentreNum(*aEnv);
    return smsc;

}

