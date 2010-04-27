/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <string>
#include <vector>
#include "runtimeuiavkon.h"
#include "globalmsgobserver.h"
#include <AknGlobalMsgQuery.h>
#include <AknGlobalListMsgQuery.h>
#include <aknconsts.h>
#include <avkon.mbg>

using namespace java::runtimeui;

OS_EXPORT void RuntimeUiAvkon::error(const TDesC& aAppName, const TDesC& aShortMsg, const TDesC& aDetailedMsg)
{
    // create the answer observer/listener through which
    // AVKON delivers the user's answer
    TRAP_IGNORE(
        GlobalMsgObserver* msgObserver = GlobalMsgObserver::NewLC();
        CAknGlobalMsgQuery* errorMsgQuery = CAknGlobalMsgQuery::NewLC();

        // select the soft keys based on the existence of details:
        // OK only if there are no details available or
        // OK and DETAILS if details are available
        TInt softKeys = R_AVKON_SOFTKEYS_OK_EMPTY__OK;
        if (&aDetailedMsg && aDetailedMsg.Size() > 0)
{
    softKeys = R_AVKON_SOFTKEYS_OK_DETAILS;
}

// show the short description of the error
errorMsgQuery->ShowMsgQueryL(
    msgObserver->iStatus,
    aShortMsg,
    softKeys,
    aAppName,
    KNullDesC());
if (msgObserver->getAnswer() == EAknSoftkeyDetails)
{
    // show more details of the error
    errorMsgQuery->ShowMsgQueryL(// codescanner::leave
        msgObserver->iStatus,
        aDetailedMsg,
        R_AVKON_SOFTKEYS_OK_EMPTY__OK,
        aAppName,
        KNullDesC());
        msgObserver->getAnswer();
    }
    CleanupStack::PopAndDestroy(errorMsgQuery);
    CleanupStack::PopAndDestroy(msgObserver);
    );
}

OS_EXPORT bool RuntimeUiAvkon::confirm(const TDesC& aAppName, ConfirmData& aConfirmData, bool aIdentified)
{
    TInt imageId = EMbmAvkonQgn_indi_midp_untrusted;
    TInt maskId = EMbmAvkonQgn_indi_midp_untrusted_mask;
    if (aIdentified)
    {
        imageId = EMbmAvkonQgn_indi_midp_trusted;
        maskId = EMbmAvkonQgn_indi_midp_trusted_mask;
    }
    bool answerAvailable = false;
    TRAP_IGNORE(
        GlobalMsgObserver* msgObserver = GlobalMsgObserver::NewLC();
        if (aConfirmData.iAnswerOptions.size() <= 2)
{
    // use GlobalMsgQuery
    CAknGlobalMsgQuery* confirmMsgQuery = CAknGlobalMsgQuery::NewLC();
        switch (aConfirmData.iAnswerOptions.size())
        {
        case 0:
            confirmMsgQuery->ShowMsgQueryL(
                msgObserver->iStatus,
                aConfirmData.iQuestion,
                R_AVKON_SOFTKEYS_OK_EMPTY__OK,
                aAppName,
                KAvkonBitmapFile,
                imageId,
                maskId);
            break;
        case 1:
            // SoftKeys should be something like R_AVKON_SOFTKEYS_iAnswerOptions[0]_EMPTY__OK
            confirmMsgQuery->ShowMsgQueryL(
                msgObserver->iStatus,
                aConfirmData.iQuestion,
                R_AVKON_SOFTKEYS_OK_EMPTY__OK,
                aAppName,
                KAvkonBitmapFile,
                imageId,
                maskId);
            break;
        default:
            // SoftKeys should be something like R_AVKON_SOFTKEYS_iAnswerOptions[0]_iAnswerOptions[1]__OK
            confirmMsgQuery->ShowMsgQueryL(// codescanner::leave
                msgObserver->iStatus,
                aConfirmData.iQuestion,
                R_AVKON_SOFTKEYS_YES_NO__YES,
                aAppName,
                KAvkonBitmapFile,
                imageId,
                maskId);
            break;
        }
    }
    else
    {
        // use GlobalListMsgQuery
        CDesCArray* answerOptions = new(ELeave) CDesCArrayFlat( // codescanner::leave
            aConfirmData.iAnswerOptions.size());
        CleanupStack::PushL(answerOptions);// codescanner::leave
        for (int i=0; i<aConfirmData.iAnswerOptions.size(); i++)
        {
            answerOptions->AppendL(*(aConfirmData.iAnswerOptions[i]));// codescanner::leave
        }
        CAknGlobalListMsgQuery* confirmMsgQuery =
            CAknGlobalListMsgQuery::NewLC();// codescanner::leave
        confirmMsgQuery->ShowListMsgQueryL(// codescanner::leave
            answerOptions,
            msgObserver->iStatus,
            aAppName,
            aConfirmData.iQuestion,
            aConfirmData.iAnswerSuggestion,
            R_AVKON_SOFTKEYS_OK_EMPTY__OK,
            EAknSoftkeyOk,
            0 /*EAknSoftkeyCancel*/,
            KAvkonBitmapFile,
            imageId,
            maskId);
    }
    int answer = msgObserver->getAnswer();
    CleanupStack::PopAndDestroy(); // confirmMsgQuery
    if (aConfirmData.iAnswerOptions.size() > 2)
{
    CleanupStack::PopAndDestroy(); // answerOptions
    }
    CleanupStack::PopAndDestroy(msgObserver);
    if (answer == EAknSoftkeyCancel)
{
    // no answer available
    answerAvailable = false;
}
else
{

    switch (answer)
        {
        case EAknSoftkeyOk:
        case EAknSoftkeyYes:
            answer = 0;
            break;
        case EAknSoftkeyNo:
            answer = 1;
            break;
        }
        if (aConfirmData.iAnswerOptions.size() > 0)
        {
            aConfirmData.iAnswer = answer;
        }
        answerAvailable = true;
    }
    );
    return answerAvailable;
}
