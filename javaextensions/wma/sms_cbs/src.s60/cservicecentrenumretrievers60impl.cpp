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


#include <smutset.h>
#include <csmsaccount.h>

#include "logger.h"
#include "cservicecentrenumretrievers60impl.h"
#include "smsservicecentrenumretriever.h"

namespace java
{
namespace wma
{

/**
 * This function retrieves service centre number. Returns null if not able to
 * retrieve service centre number
 */
jstring SmsServiceCentreNumRetriever::getServiceCentreNum(JNIEnv& aJni)
{
    JELOG2(EWMA);
    TBuf<RMobilePhone::KMaxMobileTelNumberSize> scNumber;
    TRAPD(error,
          CServiceCentreNumRetrieverS60Impl::retrieveServiceCentreNumberL(scNumber));
    if (error != KErrNone)
    {
        ELOG1(EWMA, "SMS : error while retrieving SMSC number : %d", error);
        return NULL;
    }
    return aJni.NewString((jchar*) scNumber.Ptr(), scNumber.Length());
}

/* Fetches the  service centre phone number from the Messaging settings.
 * If there is no service center set in Message settings, it leaves.
 */
void CServiceCentreNumRetrieverS60Impl::retrieveServiceCentreNumberL(
    TDes& smscNum)
{
    JELOG2(EWMA);
    CSmsAccount* messageAccount = CSmsAccount::NewLC();
    CSmsSettings* messageSettings = CSmsSettings::NewLC();
    messageAccount->LoadSettingsL(*messageSettings);
    TInt scIndex =  messageSettings->DefaultServiceCenter();
    if (scIndex < KErrNone)
    {
        ELOG(EWMA,"SMS ERROR: SMSC is not defined in SMS Settings");
        User::LeaveIfError(scIndex);
    }
    LOG(EWMA, EInfo, "SMS :: Defined SMSC in SmsSettings");
    // get the service center currently in use from
    // message settings
    CSmsServiceCenter& sc = messageSettings->GetServiceCenter(scIndex);
    smscNum.Append(sc.Address());
    CleanupStack::PopAndDestroy(messageSettings);
    CleanupStack::PopAndDestroy(messageAccount);
}

} //namespace wma
} //namespace java
