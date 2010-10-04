/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "jstringutils.h"
#include <mmsapplicationadapter.h>
#include "com_nokia_mj_impl_mms_MMSInstallerPlugin.h"

//using namespace java::wma;

/*
 * Class:     com_nokia_mj_impl_mms_MMSInstallerPlugin
 * Method:    _markForUnregister
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_mms_MMSInstallerPlugin__1unregisterFromMMSEngine
(JNIEnv* aJni, jobject /*aPeer*/, jstring aUri)
{
    JELOG2(EWMA);
    TRAPD(Error,
    {
       CActiveScheduler* activeScheduler = new(ELeave) CActiveScheduler();
       CleanupStack::PushL(activeScheduler);
       CActiveScheduler::Install(activeScheduler);
       CMmsApplicationAdapter* mmsApplicationAdapter = CMmsApplicationAdapter::NewL();
       CleanupStack::PushL(mmsApplicationAdapter);
       JStringUtils applicationId(*aJni,aUri);
       LOG1(EWMA,EInfo,"_markForUnregister - ApplicationId = %S",applicationId.Ptr());
       TBool isRegistered = mmsApplicationAdapter->RegisteredL(applicationId);
       if (isRegistered)
       { 
          LOG(EWMA,EInfo,"Unregistering from MMSEngine");
          mmsApplicationAdapter->UnregisterL(applicationId);
       }
       CleanupStack::PopAndDestroy(mmsApplicationAdapter);
       CleanupStack::PopAndDestroy(activeScheduler);
    });
    LOG1(EWMA,EInfo,"Unregistering status : %d",Error);
    return Error;
}
