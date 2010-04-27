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
* Description:  JavaAPNControl implementation
 *
*/


#include <e32base.h>
#include <e32debug.h>
#include <logger.h>

//#include <j2me/net/CJavaInternetConnectionSession.h>
#include "com_nokia_mid_iapinfo_APNControl.h"

// ---------------------------------------------------------------------------
//    CONSTANTS
// -----------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_APNControl__1getLastIAP(JNIEnv* ,
        jclass)
{
    LOG(EJavaIapInfo, EInfo, "JAVAAPNCONTROL + getLastIAP()");

    //TInt iap = -1;

    // iap = CJavaInternetConnectionSession::GetLastAPNId();
    // LOG1(EJavaIapInfo, EInfo, "JAVAAPNCONTROL returning value: %D", iap );
    // LOG(EJavaIapInfo, EInfo, "JAVAAPNCONTROL - getLastIAP()" );

    // return 111;//iap;
    return 0;
}

